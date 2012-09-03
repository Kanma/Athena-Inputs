/** @file   MacController.cpp
    @author Philip Abbet

    Implementation of the class 'Athena::Inputs::MacController'
*/

#import <CoreFoundation/CoreFoundation.h>
#import <IOKit/IOKitLib.h>
#import <Kernel/IOKit/hidsystem/IOHIDUsageTables.h>

#include <Athena-Inputs/osx/MacController.h>
#include <Athena-Inputs/osx/MacHelpers.h>
#include <Athena-Core/Log/LogManager.h>
#include <Athena-Core/Utils/StringConverter.h>


using namespace Athena;
using namespace Athena::Inputs;
using namespace Athena::Utils;
using namespace Athena::Log;
using namespace std;


static const char*  __CONTEXT__ = "Mac Controller";


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

MacController::MacController(io_object_t hidDevice, tControllerType type, unsigned int uiNo)
: Controller(type, uiNo), m_hidDevice(hidDevice), m_bExclusiveMode(true)
{
    IOObjectRetain(m_hidDevice);

    // Process the properties of the device the axes, POVs and buttons
    CFMutableDictionaryRef propertyMap = 0;
    if (IORegistryEntryCreateCFProperties(m_hidDevice, &propertyMap, kCFAllocatorDefault, kNilOptions) == KERN_SUCCESS)
    {
        // Retrieve its name
        CFStringRef str = getDictionaryItemAsStringRef(propertyMap, kIOHIDProductKey);
        if (str)
            m_strName = CFStringGetCStringPtr(str, CFStringGetSystemEncoding());
        else if (type == CONTROLLER_GAMEPAD)
            m_strName = "Gamepad #" + StringConverter::toString(uiNo);

        // Go through all items in this device (buttons, POVs, axes)
        parseDeviceProperties(propertyMap);

        // Count the number of axes found
        for (unsigned char i = 0; i < 8; ++i)
        {
            if (m_axes & (1 << i))
                ++m_uiNbAxes;
        }
    }

    CFRelease(propertyMap);

    // Now the controller is initialized
    m_bInitialized = true;
}

//-----------------------------------------------------------------------

MacController::~MacController()
{
    IOObjectRelease(m_hidDevice);
}


/**************************** MANAGEMENT OF THE CONTROLLER *****************************/

bool MacController::activate(bool bActivate)
{
    ATHENA_LOG_EVENT(toString() + (bActivate ? ": Actived" : ": Deactived"));

    m_bActive = bActivate;

    return acquire(bActivate);
}

//-----------------------------------------------------------------------

void MacController::setExclusiveMode(bool bExclusive)
{
    if (m_bExclusiveMode == bExclusive)
        return;

    m_bExclusiveMode = bExclusive;

    if (m_bActive)
    {
        acquire(false);
        acquire(true);
    }
}

//-----------------------------------------------------------------------

const string MacController::getNameOfKey(tKey key)
{
    // Declarations
    string strName;

    // Test if the name isn't already in the list
    strName = Controller::getNameOfKey(key);
    if (!strName.empty())
        return strName;

    strName = "Button #";
    strName += Utils::StringConverter::toString(key);

    // Put the name in the list
    m_strKeys[key] = strName;

    return strName;
}

//-----------------------------------------------------------------------

const string MacController::getNameOfAxis(tAxis axis)
{
    // Declarations
    string strName;

    // Test if the name isn't already in the list
    strName = Controller::getNameOfAxis(axis);
    if (!strName.empty())
        return strName;

    switch (axis)
    {
        case AXIS_X:        strName = "X-axis"; break;
        case AXIS_Y:        strName = "Y-axis"; break;
        case AXIS_Z:        strName = "Z-axis"; break;
        case AXIS_X_ROT:    strName = "X rotation axis"; break;
        case AXIS_Y_ROT:    strName = "Y rotation axis"; break;
        case AXIS_Z_ROT:    strName = "Z rotation axis"; break;
        case SLIDER_0:      strName = "Slider 0"; break;
        case SLIDER_1:      strName = "Slider 1"; break;
        default:            return "";
    }

    // Put the name in the list
    m_strAxes[axis] = strName;

    return strName;
}

//-----------------------------------------------------------------------

const string MacController::getNameOfPOV(tPOV pov)
{
    // Declarations
    string strName;

    // Test if the name isn't already in the list
    strName = Controller::getNameOfPOV(pov);
    if (!strName.empty())
        return strName;

    strName = "POV #";
    strName += Utils::StringConverter::toString(pov);

    // Put the name in the list
    m_strPOVs[pov] = strName;

    return strName;
}

//-----------------------------------------------------------------------

void MacController::parseDeviceProperties(CFDictionaryRef properties)
{
    if (!properties)
        return;

    // Retrieve an array of element properties
    CFArrayRef array = getDictionaryItemAsArrayRef(properties, kIOHIDElementKey);
    if (!array)
        return;

    // Iterate through the elements
    for (int i = 0; i < CFArrayGetCount(array); i++)
    {
        CFDictionaryRef element = getArrayItemAsDictionaryRef(array, i);
        if (!element)
            continue;

        // The element is a collection
        if ((getInt32(getDictionaryItemAsNumberRef(element, kIOHIDElementTypeKey)) == kIOHIDElementTypeCollection) &&
            (getInt32(getDictionaryItemAsNumberRef(element, kIOHIDElementUsagePageKey)) == kHIDPage_GenericDesktop))
        {
            parseDeviceProperties(element);
            continue;
        }

        tAxis axes = m_axes;
        tInputEventPart part;

        switch (getInt32(getDictionaryItemAsNumberRef(element, kIOHIDElementUsagePageKey)))
        {
            case kHIDPage_GenericDesktop:
                switch (getInt32(getDictionaryItemAsNumberRef(element, kIOHIDElementUsageKey)))
                {
                    case kHIDUsage_GD_X:
                        m_axes |= AXIS_X;
                        part.axis = AXIS_X;
                        onPartEnumerated(PART_AXIS, part, element);
                        ATHENA_LOG_DEBUG("Axis X found");
                        break;

                    case kHIDUsage_GD_Y:
                        m_axes |= AXIS_Y;
                        part.axis = AXIS_Y;
                        onPartEnumerated(PART_AXIS, part, element);
                        ATHENA_LOG_DEBUG("Axis Y found");
                        break;

                    case kHIDUsage_GD_Z:
                        m_axes |= AXIS_Z;
                        part.axis = AXIS_Z;
                        onPartEnumerated(PART_AXIS, part, element);
                        ATHENA_LOG_DEBUG("Axis Z found");
                        break;

                    case kHIDUsage_GD_Rx:
                        m_axes |= AXIS_X_ROT;
                        part.axis = AXIS_X_ROT;
                        onPartEnumerated(PART_AXIS, part, element);
                        ATHENA_LOG_DEBUG("Axis RX found");
                        break;

                    case kHIDUsage_GD_Ry:
                        m_axes |= AXIS_Y_ROT;
                        part.axis = AXIS_Y_ROT;
                        onPartEnumerated(PART_AXIS, part, element);
                        ATHENA_LOG_DEBUG("Axis RY found");
                        break;

                    case kHIDUsage_GD_Rz:
                        m_axes |= AXIS_Z_ROT;
                        part.axis = AXIS_Z_ROT;
                        onPartEnumerated(PART_AXIS, part, element);
                        ATHENA_LOG_DEBUG("Axis RZ found");
                        break;

                    case kHIDUsage_GD_Wheel:
                        m_axes |= AXIS_Z | AXIS_Z_ROT;
                        part.axis = AXIS_Z;
                        onPartEnumerated(PART_AXIS, part, element);
                        part.axis = AXIS_Z_ROT;
                        onPartEnumerated(PART_AXIS, part, element);
                        ATHENA_LOG_DEBUG("Wheel (axis Z + axis RZ) found");
                        break;

                    case kHIDUsage_GD_Slider:
                        m_axes |= (m_axes & SLIDER_0 ? SLIDER_1 : SLIDER_0);
                        part.axis = (m_axes & SLIDER_1 ? SLIDER_1 : SLIDER_0);
                        onPartEnumerated(PART_AXIS, part, element);

                        if (m_axes & SLIDER_1)
                            ATHENA_LOG_DEBUG("Slider 1 found");
                        else
                            ATHENA_LOG_DEBUG("Slider 2 found");
                        break;

                    case kHIDUsage_GD_Hatswitch:
                        ++m_uiNbPOVs;
                        part.pov = m_uiNbPOVs;
                        onPartEnumerated(PART_POV, part, element);
                        ATHENA_LOG_DEBUG("POV #" + StringConverter::toString(m_uiNbPOVs) + " found");
                        break;

                    case kHIDUsage_GD_DPadUp:
                    case kHIDUsage_GD_DPadDown:
                    case kHIDUsage_GD_DPadRight:
                    case kHIDUsage_GD_DPadLeft:
                        cout << "\tPad\n";
                        break;

                    default:
                        ATHENA_LOG_ERROR("Unknown controller element, usage key = " +
                                         StringConverter::toString(getInt32(getDictionaryItemAsNumberRef(element, kIOHIDElementUsageKey))));
                }
                break;

            case kHIDPage_Button:
                ++m_uiNbKeys;
                part.key = m_uiNbKeys;
                onPartEnumerated(PART_KEY, part, element);
                ATHENA_LOG_DEBUG("Button #" + StringConverter::toString(m_uiNbKeys) + " found");
                break;
        }
    }
}

//-----------------------------------------------------------------------

void MacController::onPartEnumerated(Athena::Inputs::tControllerPart type,
                                      Athena::Inputs::tInputEventPart part,
                                      CFDictionaryRef element)
{
    // Assertions
    assert(element);

    if (getType() != CONTROLLER_GAMEPAD)
        return;

    IOHIDElementCookie cookie = (IOHIDElementCookie) getInt32(getDictionaryItemAsNumberRef(element, kIOHIDElementCookieKey));

    tCookieInfos infos;
    infos.type = type;
    infos.part = part;

    m_cookies[cookie] = infos;
}
