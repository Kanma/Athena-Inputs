/** @file   VirtualController.cpp
    @author Philip Abbet

    Implementation of the class 'Athena::Inputs::VirtualController'
*/

// Athena's includes
#include <Athena-Inputs/VirtualController.h>
#include <Athena-Inputs/IVirtualEventsListener.h>
#include <Athena-Inputs/InputsUnit.h>
#include <Athena-Core/Log/LogManager.h>
#include <Athena-Math/MathUtils.h>


using namespace Athena;
using namespace Athena::Inputs;
using namespace Athena::Math;
using namespace Athena::Log;
using namespace std;


/************************************** CONSTANTS **************************************/

/// Context used for logging
static const char* __CONTEXT__ = "Virtual controller";


/****************************** CONSTRUCTION / DESTRUCTION *****************************/

VirtualController::VirtualController()
: m_pEventsListener(0), m_bEnabled(true)
{
}

//-----------------------------------------------------------------------

VirtualController::~VirtualController()
{
}


/************************** MANAGEMENT OF THE VIRTUAL CONTROLLER ***********************/

void VirtualController::process(std::deque<tInputEvent> &events)
{
    // Declarations
    std::map<tVirtualID, tVirtualKey>::iterator     iterKey, iterKeyEnd;
    std::map<tVirtualID, tVirtualAxis>::iterator    iterAxis, iterAxisEnd;
    std::map<tVirtualID, tVirtualPOV>::iterator     iterPOV, iterPOVEnd;
    tVirtualKey*                                    pVirtualKey;
    tVirtualAxis*                                   pVirtualAxis;
    tVirtualPOV*                                    pVirtualPOV;
    tVirtualEvent                                   event;
    tInputEvent*                                    pEvent;
    std::deque<tInputEvent>::iterator               iter, iterEnd;

    // If the virtual controller isn't enabled, we're done
    if (!m_bEnabled)
        return;

    // Reset some values
    for (iterKey = m_virtualKeys.begin(), iterKeyEnd = m_virtualKeys.end();
         iterKey != iterKeyEnd; ++iterKey)
    {
        iterKey->second.bToggled = false;
    }

    for (iterAxis = m_virtualAxes.begin(), iterAxisEnd = m_virtualAxes.end();
         iterAxis != iterAxisEnd; ++iterAxis)
    {
        iterAxis->second.bChanged = false;
        if (iterAxis->second.pController && (iterAxis->second.pController->getType() == CONTROLLER_MOUSE))
            iterAxis->second.iValue = 0;
    }

    for (iterPOV = m_virtualPOVs.begin(), iterPOVEnd = m_virtualPOVs.end();
         iterPOV != iterPOVEnd; ++iterPOV)
    {
        iterPOV->second.bChanged = false;
    }


    // Process each event
    for (iter = events.begin(), iterEnd = events.end();
         iter != iterEnd; ++iter)
    {
        pEvent = &(*iter);

        switch (pEvent->part)
        {
        case PART_KEY:
            // Search in the list of keys registered on this virtual controller
            for (iterKey = m_virtualKeys.begin(), iterKeyEnd = m_virtualKeys.end();
                iterKey != iterKeyEnd; ++iterKey)
            {
                pVirtualKey = &iterKey->second;
                if ((pEvent->pController == pVirtualKey->pController) &&
                    (pEvent->partID.key == pVirtualKey->key))
                {
                    pVirtualKey->bToggled       = true;
                    pVirtualKey->bPressed       = pEvent->value.bPressed;
                    if (pVirtualKey->bPressed)
                        pVirtualKey->ulPressTimestamp   = pEvent->dwTimeStamp;
                    else
                        pVirtualKey->ulReleaseTimestamp = pEvent->dwTimeStamp;

                    if (m_pEventsListener)
                    {
                        event.part              = PART_KEY;
                        event.virtualID         = iterKey->first;
                        event.value.bPressed    = pVirtualKey->bPressed;
                        event.ulTimestamp       = pEvent->dwTimeStamp;

                        m_pEventsListener->onEvent(&event);
                    }

                    break;
                }
            }

            // Search in the list of virtual POVs registered on this virtual controller
            for (iterPOV = m_virtualPOVs.begin(), iterPOVEnd = m_virtualPOVs.end();
                iterPOV != iterPOVEnd; ++iterPOV)
            {
                pVirtualPOV = &iterPOV->second;
                if ((pVirtualPOV->part == PART_KEY) && (pEvent->pController == pVirtualPOV->pController) &&
                    ((pEvent->partID.key == pVirtualPOV->realPart.keys.keyUp) ||
                     (pEvent->partID.key == pVirtualPOV->realPart.keys.keyDown) ||
                     (pEvent->partID.key == pVirtualPOV->realPart.keys.keyLeft) ||
                     (pEvent->partID.key == pVirtualPOV->realPart.keys.keyRight)))
                {
                    pVirtualPOV->previousPosition = pVirtualPOV->position;

                    if (pEvent->partID.key == pVirtualPOV->realPart.keys.keyUp)
                    {
                        if (pEvent->value.bPressed)
                        {
                            switch (pVirtualPOV->position)
                            {
                            case POV_LEFT:
                            case POV_DOWNLEFT:
                                pVirtualPOV->position = POV_UPLEFT;
                                break;

                            case POV_RIGHT:
                            case POV_DOWNRIGHT:
                                pVirtualPOV->position = POV_UPRIGHT;
                                break;

                            default:
                                pVirtualPOV->position = POV_UP;
                            }
                        }
                        else
                        {
                            switch (pVirtualPOV->position)
                            {
                            case POV_UPLEFT:
                                pVirtualPOV->position = POV_LEFT;
                                break;

                            case POV_UPRIGHT:
                                pVirtualPOV->position = POV_RIGHT;
                                break;

                            case POV_UP:
                                pVirtualPOV->position = POV_CENTER;
                            }
                        }
                    }
                    else if (pEvent->partID.key == pVirtualPOV->realPart.keys.keyDown)
                    {
                        if (pEvent->value.bPressed)
                        {
                            switch (pVirtualPOV->position)
                            {
                            case POV_LEFT:
                            case POV_UPLEFT:
                                pVirtualPOV->position = POV_DOWNLEFT;
                                break;

                            case POV_RIGHT:
                            case POV_UPRIGHT:
                                pVirtualPOV->position = POV_DOWNRIGHT;
                                break;

                            default:
                                pVirtualPOV->position = POV_DOWN;
                            }
                        }
                        else
                        {
                            switch (pVirtualPOV->position)
                            {
                            case POV_DOWNLEFT:
                                pVirtualPOV->position = POV_LEFT;
                                break;

                            case POV_DOWNRIGHT:
                                pVirtualPOV->position = POV_RIGHT;
                                break;

                            case POV_DOWN:
                                pVirtualPOV->position = POV_CENTER;
                            }
                        }
                    }
                    else if (pEvent->partID.key == pVirtualPOV->realPart.keys.keyLeft)
                    {
                        if (pEvent->value.bPressed)
                        {
                            switch (pVirtualPOV->position)
                            {
                            case POV_UP:
                            case POV_UPRIGHT:
                                pVirtualPOV->position = POV_UPLEFT;
                                break;

                            case POV_DOWN:
                            case POV_DOWNRIGHT:
                                pVirtualPOV->position = POV_DOWNLEFT;
                                break;

                            default:
                                pVirtualPOV->position = POV_LEFT;
                            }
                        }
                        else
                        {
                            switch (pVirtualPOV->position)
                            {
                            case POV_DOWNLEFT:
                                pVirtualPOV->position = POV_DOWN;
                                break;

                            case POV_UPLEFT:
                                pVirtualPOV->position = POV_UP;
                                break;

                            case POV_LEFT:
                                pVirtualPOV->position = POV_CENTER;
                            }
                        }
                    }
                    else if (pEvent->partID.key == pVirtualPOV->realPart.keys.keyRight)
                    {
                        if (pEvent->value.bPressed)
                        {
                            switch (pVirtualPOV->position)
                            {
                            case POV_UP:
                            case POV_UPLEFT:
                                pVirtualPOV->position = POV_UPRIGHT;
                                break;

                            case POV_DOWN:
                            case POV_DOWNLEFT:
                                pVirtualPOV->position = POV_DOWNRIGHT;
                                break;

                            default:
                                pVirtualPOV->position = POV_RIGHT;
                            }
                        }
                        else
                        {
                            switch (pVirtualPOV->position)
                            {
                            case POV_DOWNRIGHT:
                                pVirtualPOV->position = POV_DOWN;
                                break;

                            case POV_UPRIGHT:
                                pVirtualPOV->position = POV_UP;
                                break;

                            case POV_RIGHT:
                                pVirtualPOV->position = POV_CENTER;
                            }
                        }
                    }
                    pVirtualPOV->ulPreviousChangeTimestamp  = pVirtualPOV->ulLastChangeTimestamp;
                    pVirtualPOV->ulLastChangeTimestamp      = pEvent->dwTimeStamp;
                    pVirtualPOV->bChanged                   = true;

                    if (m_pEventsListener)
                    {
                        event.part              = PART_POV;
                        event.virtualID         = iterPOV->first;
                        event.value.position    = pVirtualPOV->position;
                        event.ulTimestamp       = pEvent->dwTimeStamp;

                        m_pEventsListener->onEvent(&event);
                    }

                    break;
                }
            }

            // Search in the list of virtual axes registered on this virtual controller
            for (iterAxis = m_virtualAxes.begin(), iterAxisEnd = m_virtualAxes.end();
                 iterAxis != iterAxisEnd; ++iterAxis)
            {
                pVirtualAxis = &iterAxis->second;
                if ((pVirtualAxis->part == PART_KEY) && (pEvent->pController == pVirtualAxis->pController) &&
                    ((pEvent->partID.key == pVirtualAxis->realPart.keys.keyMin) ||
                     (pEvent->partID.key == pVirtualAxis->realPart.keys.keyMax)))
                {
                    if (pEvent->value.bPressed)
                    {
                        if (pEvent->partID.key == pVirtualAxis->realPart.keys.keyMin)
                        {
                            pVirtualAxis->bChanged = (pVirtualAxis->iValue != -255);
                            pVirtualAxis->iValue = -255;
                        }
                        else if (pEvent->partID.key == pVirtualAxis->realPart.keys.keyMax)
                        {
                            pVirtualAxis->bChanged = (pVirtualAxis->iValue != 255);
                            pVirtualAxis->iValue = 255;
                        }
                    }
                    else
                    {
                        pVirtualAxis->bChanged = (pVirtualAxis->iValue != 0);
                        pVirtualAxis->iValue = 0;
                    }

                    pVirtualAxis->ulTimestamp   = pEvent->dwTimeStamp;

                    if (m_pEventsListener)
                    {
                        event.part          = PART_AXIS;
                        event.virtualID     = iterAxis->first;
                        event.value.iValue  = pVirtualAxis->iValue;
                        event.ulTimestamp   = pEvent->dwTimeStamp;

                        m_pEventsListener->onEvent(&event);
                    }

                    // Don't break, because the other direction can be used for another axis
                    //break;
                }
            }
            break;

        case PART_AXIS:
            for (iterAxis = m_virtualAxes.begin(), iterAxisEnd = m_virtualAxes.end();
                 iterAxis != iterAxisEnd; ++iterAxis)
            {
                pVirtualAxis = &iterAxis->second;
                if ((pVirtualAxis->part == PART_AXIS) && (pEvent->pController == pVirtualAxis->pController) &&
                    (pEvent->partID.axis == pVirtualAxis->realPart.axis))
                {
                    pVirtualAxis->bChanged = (MathUtils::Abs(pVirtualAxis->iValue - pEvent->value.iValue) >= 10.0f);

                    pVirtualAxis->iValue        = pEvent->value.iValue;
                    pVirtualAxis->ulTimestamp   = pEvent->dwTimeStamp;

                    if (m_pEventsListener)
                    {
                        event.part          = PART_AXIS;
                        event.virtualID     = iterAxis->first;
                        event.value.iValue  = pVirtualAxis->iValue;
                        event.ulTimestamp   = pEvent->dwTimeStamp;

                        m_pEventsListener->onEvent(&event);
                    }
                    break;
                }
            }

            // Search in the list of virtual POVs registered on this virtual controller
            for (iterPOV = m_virtualPOVs.begin(), iterPOVEnd = m_virtualPOVs.end();
                iterPOV != iterPOVEnd; ++iterPOV)
            {
                pVirtualPOV = &iterPOV->second;
                if ((pVirtualPOV->part == PART_AXIS) && (pEvent->pController == pVirtualPOV->pController) &&
                    ((pEvent->partID.axis == pVirtualPOV->realPart.axes.axisUpDown) ||
                     (pEvent->partID.axis == pVirtualPOV->realPart.axes.axisLeftRight)))
                {
                    if (pEvent->partID.axis == pVirtualPOV->realPart.axes.axisUpDown)
                        pVirtualPOV->realPart.axes.iUpDownPos = pEvent->value.iValue;
                    else
                        pVirtualPOV->realPart.axes.iLeftRightPos = pEvent->value.iValue;

                    if (pVirtualPOV->realPart.axes.iUpDownPos <= -100)
                    {
                        if (pVirtualPOV->realPart.axes.iLeftRightPos <= -100)
                            pVirtualPOV->realPart.axes.tempPosition = POV_UPLEFT;
                        else if (pVirtualPOV->realPart.axes.iLeftRightPos >= 100)
                            pVirtualPOV->realPart.axes.tempPosition = POV_UPRIGHT;
                        else
                            pVirtualPOV->realPart.axes.tempPosition = POV_UP;
                    }
                    else if (pVirtualPOV->realPart.axes.iUpDownPos >= 100)
                    {
                        if (pVirtualPOV->realPart.axes.iLeftRightPos <= -100)
                            pVirtualPOV->realPart.axes.tempPosition = POV_DOWNLEFT;
                        else if (pVirtualPOV->realPart.axes.iLeftRightPos >= 100)
                            pVirtualPOV->realPart.axes.tempPosition = POV_DOWNRIGHT;
                        else
                            pVirtualPOV->realPart.axes.tempPosition = POV_DOWN;
                    }
                    else
                    {
                        if (pVirtualPOV->realPart.axes.iLeftRightPos <= -100)
                            pVirtualPOV->realPart.axes.tempPosition = POV_LEFT;
                        else if (pVirtualPOV->realPart.axes.iLeftRightPos >= 100)
                            pVirtualPOV->realPart.axes.tempPosition = POV_RIGHT;
                        else
                            pVirtualPOV->realPart.axes.tempPosition = POV_CENTER;
                    }

                    pVirtualPOV->ulPreviousChangeTimestamp  = pVirtualPOV->ulLastChangeTimestamp;
                    pVirtualPOV->ulLastChangeTimestamp      = pEvent->dwTimeStamp;
                    pVirtualPOV->bChanged                   = true;

                    if (m_pEventsListener)
                    {
                        event.part              = PART_POV;
                        event.virtualID         = iterPOV->first;
                        event.value.position    = pVirtualPOV->position;
                        event.ulTimestamp       = pEvent->dwTimeStamp;

                        m_pEventsListener->onEvent(&event);
                    }

                    break;
                }
            }
            break;


        case PART_POV:
            // Search in the list of POVs registered on this virtual controller
            for (iterPOV = m_virtualPOVs.begin(), iterPOVEnd = m_virtualPOVs.end();
                iterPOV != iterPOVEnd; ++iterPOV)
            {
                pVirtualPOV = &iterPOV->second;
                if ((pVirtualPOV->part == PART_POV) && (pEvent->pController == pVirtualPOV->pController) &&
                    (pEvent->partID.pov == pVirtualPOV->realPart.pov))
                {
                    pVirtualPOV->previousPosition           = pVirtualPOV->position;
                    pVirtualPOV->position                   = pEvent->value.position;
                    pVirtualPOV->ulPreviousChangeTimestamp  = pVirtualPOV->ulLastChangeTimestamp;
                    pVirtualPOV->ulLastChangeTimestamp      = pEvent->dwTimeStamp;
                    pVirtualPOV->bChanged                   = true;

                    if (m_pEventsListener)
                    {
                        event.part              = PART_POV;
                        event.virtualID         = iterPOV->first;
                        event.value.position    = pVirtualPOV->position;
                        event.ulTimestamp       = pEvent->dwTimeStamp;

                        m_pEventsListener->onEvent(&event);
                    }

                    break;
                }
            }

            // Search in the list of virtual axes registered on this virtual controller
            for (iterAxis = m_virtualAxes.begin(), iterAxisEnd = m_virtualAxes.end();
                 iterAxis != iterAxisEnd; ++iterAxis)
            {
                pVirtualAxis = &iterAxis->second;
                if ((pVirtualAxis->part == PART_POV) && (pEvent->pController == pVirtualAxis->pController) &&
                    (pEvent->partID.pov == pVirtualAxis->realPart.pov.pov))
                {
                    if (pVirtualAxis->realPart.pov.bUpDown)
                    {
                        if (pEvent->value.position & POV_UP)
                        {
                            pVirtualAxis->bChanged = (pVirtualAxis->iValue != -255);
                            pVirtualAxis->iValue = -255;
                        }
                        else if (pEvent->value.position & POV_DOWN)
                        {
                            pVirtualAxis->bChanged = (pVirtualAxis->iValue != 255);
                            pVirtualAxis->iValue = 255;
                        }
                        else
                        {
                            pVirtualAxis->bChanged = (pVirtualAxis->iValue != 0);
                            pVirtualAxis->iValue = 0;
                        }
                    }
                    else
                    {
                        if (pEvent->value.position & POV_RIGHT)
                        {
                            pVirtualAxis->bChanged = (pVirtualAxis->iValue != 255);
                            pVirtualAxis->iValue = 255;
                        }
                        else if (pEvent->value.position & POV_LEFT)
                        {
                            pVirtualAxis->bChanged = (pVirtualAxis->iValue != -255);
                            pVirtualAxis->iValue = -255;
                        }
                        else
                        {
                            pVirtualAxis->bChanged = (pVirtualAxis->iValue != 0);
                            pVirtualAxis->iValue = 0;
                        }
                    }

                    pVirtualAxis->ulTimestamp   = pEvent->dwTimeStamp;

                    if (m_pEventsListener)
                    {
                        event.part          = PART_AXIS;
                        event.virtualID     = iterAxis->first;
                        event.value.iValue  = pVirtualAxis->iValue;
                        event.ulTimestamp   = pEvent->dwTimeStamp;

                        m_pEventsListener->onEvent(&event);
                    }

                    // Don't break, because the other direction can be used for another axis
                    //break;
                }
            }
            break;
        }
    }


    for (iterPOV = m_virtualPOVs.begin(), iterPOVEnd = m_virtualPOVs.end();
        iterPOV != iterPOVEnd; ++iterPOV)
    {
        pVirtualPOV = &iterPOV->second;

        if ((pVirtualPOV->part == PART_AXIS) && (pVirtualPOV->realPart.axes.tempPosition != pVirtualPOV->position))
        {
            pVirtualPOV->previousPosition   = pVirtualPOV->position;
            pVirtualPOV->position           = pVirtualPOV->realPart.axes.tempPosition;

            if (m_pEventsListener)
            {
                event.part              = PART_POV;
                event.virtualID         = iterPOV->first;
                event.value.position    = pVirtualPOV->position;
                event.ulTimestamp       = pVirtualPOV->ulLastChangeTimestamp;

                m_pEventsListener->onEvent(&event);
            }
        }
    }
}

//-----------------------------------------------------------------------

void VirtualController::setEventsListener(IVirtualEventsListener* pEventsListener)
{
    m_pEventsListener = pEventsListener;
}

//-----------------------------------------------------------------------

void VirtualController::enable(bool bEnable)
{
    // Declarations
    std::map<tVirtualID, tVirtualKey>::iterator     iterKey, iterKeyEnd;
    std::map<tVirtualID, tVirtualAxis>::iterator    iterAxis, iterAxisEnd;
    std::map<tVirtualID, tVirtualPOV>::iterator     iterPOV, iterPOVEnd;

    if (bEnable)
    {
        m_bEnabled = true;

        for (iterKey = m_virtualKeys.begin(), iterKeyEnd = m_virtualKeys.end();
            iterKey != iterKeyEnd; ++iterKey)
        {
            if (iterKey->second.pController)
                iterKey->second.pController->activate();
        }

        for (iterAxis = m_virtualAxes.begin(), iterAxisEnd = m_virtualAxes.end();
            iterAxis != iterAxisEnd; ++iterAxis)
        {
            if (iterAxis->second.pController)
                iterAxis->second.pController->activate();
        }

        for (iterPOV = m_virtualPOVs.begin(), iterPOVEnd = m_virtualPOVs.end();
            iterPOV != iterPOVEnd; ++iterPOV)
        {
            if (iterPOV->second.pController)
                iterPOV->second.pController->activate();
        }
    }
    else
    {
        m_bEnabled = false;

        for (iterKey = m_virtualKeys.begin(), iterKeyEnd = m_virtualKeys.end();
            iterKey != iterKeyEnd; ++iterKey)
        {
            iterKey->second.bToggled = false;
            iterKey->second.bPressed = false;
        }

        for (iterAxis = m_virtualAxes.begin(), iterAxisEnd = m_virtualAxes.end();
            iterAxis != iterAxisEnd; ++iterAxis)
        {
            iterAxis->second.bChanged   = false;
            iterAxis->second.iValue     = 0;
        }

        for (iterPOV = m_virtualPOVs.begin(), iterPOVEnd = m_virtualPOVs.end();
            iterPOV != iterPOVEnd; ++iterPOV)
        {
            iterPOV->second.position            = POV_CENTER;
            iterPOV->second.previousPosition    = POV_CENTER;
        }
    }
}

//-----------------------------------------------------------------------

bool VirtualController::isEnabled()
{
    return m_bEnabled;
}


/***************************** MANAGEMENT OF THE VIRTUAL PARTS *************************/

void VirtualController::registerVirtualKey(tVirtualID virtualID,
                                            const std::string& strShortcut)
{
    // Assertions
    assert(InputsUnit::getSingletonPtr());

    // Declarations
    tVirtualKey virtualKey = { 0 };

    if (!getVirtualKey(virtualID))
    {
        if (!strShortcut.empty())
        {
            InputsUnit::getSingletonPtr()->registerShortcut(strShortcut, virtualID);
            virtualKey.bHasShortcut = true;
        }
        else
        {
            virtualKey.bHasShortcut = false;
        }

        m_virtualKeys[virtualID] = virtualKey;
    }
}

//-----------------------------------------------------------------------

void VirtualController::registerVirtualAxis(tVirtualID virtualID)
{
    // Declarations
    tVirtualAxis virtualAxis = { 0 };

    if (!getVirtualAxis(virtualID))
        m_virtualAxes[virtualID] = virtualAxis;
}

//-----------------------------------------------------------------------

void VirtualController::registerVirtualPOV(tVirtualID virtualID,
                                           const std::string& strShortcutUp,
                                           const std::string& strShortcutDown,
                                           const std::string& strShortcutLeft,
                                           const std::string& strShortcutRight)
{
    // Assertions
    assert(InputsUnit::getSingletonPtr());

    // Declarations
    tVirtualPOV virtualPOV = { 0 };

    if (!getVirtualPOV(virtualID))
    {
        if (!strShortcutUp.empty())
        {
            InputsUnit* pInputsUnit = InputsUnit::getSingletonPtr();

            pInputsUnit->registerShortcut(strShortcutUp, virtualID);
            pInputsUnit->registerShortcut(strShortcutDown, virtualID);
            pInputsUnit->registerShortcut(strShortcutLeft, virtualID);
            pInputsUnit->registerShortcut(strShortcutRight, virtualID);

            pInputsUnit->registerShortcut(strShortcutUp + strShortcutLeft, virtualID);
            pInputsUnit->registerShortcut(strShortcutDown + strShortcutLeft, virtualID);
            pInputsUnit->registerShortcut(strShortcutUp + strShortcutRight, virtualID);
            pInputsUnit->registerShortcut(strShortcutDown + strShortcutRight, virtualID);

            virtualPOV.shortcuts.strShortcutUp      = strShortcutUp;
            virtualPOV.shortcuts.strShortcutDown    = strShortcutDown;
            virtualPOV.shortcuts.strShortcutLeft    = strShortcutLeft;
            virtualPOV.shortcuts.strShortcutRight   = strShortcutRight;
        }

        m_virtualPOVs[virtualID] = virtualPOV;
    }
}

//-----------------------------------------------------------------------

void VirtualController::addVirtualKey(tVirtualID virtualID, Controller* pController,
                                      tKey key, const std::string& strShortcut)
{
    // Assertions
    assert(InputsUnit::getSingletonPtr());

    // Declarations
    tVirtualKey virtualKey = { 0 };

    virtualKey.pController  = pController;
    virtualKey.key          = key;
    virtualKey.bPressed     = false;
    virtualKey.bToggled     = false;

    if (!strShortcut.empty())
    {
        InputsUnit::getSingletonPtr()->registerShortcut(strShortcut, virtualID);
        virtualKey.bHasShortcut = true;
    }
    else
    {
        virtualKey.bHasShortcut = false;
    }

    m_virtualKeys[virtualID] = virtualKey;
}

//-----------------------------------------------------------------------

void VirtualController::addVirtualAxis(tVirtualID virtualID, Controller* pController,
                                        tAxis axis)
{
    // Declarations
    tVirtualAxis virtualAxis = { 0 };

    virtualAxis.pController     = pController;
    virtualAxis.part            = PART_AXIS;
    virtualAxis.realPart.axis   = axis;
    virtualAxis.iValue          = 0;
    virtualAxis.bChanged        = false;

    m_virtualAxes[virtualID] = virtualAxis;
}

//-----------------------------------------------------------------------

void VirtualController::addVirtualAxis(tVirtualID virtualID, Controller* pController,
                                        tPOV pov, bool bUpDown)
{
    // Declarations
    tVirtualAxis virtualAxis = { 0 };

    virtualAxis.pController             = pController;
    virtualAxis.part                    = PART_POV;
    virtualAxis.realPart.pov.pov        = pov;
    virtualAxis.realPart.pov.bUpDown    = bUpDown;
    virtualAxis.iValue                  = 0;
    virtualAxis.bChanged                = false;

    m_virtualAxes[virtualID] = virtualAxis;
}

//-----------------------------------------------------------------------

void VirtualController::addVirtualAxis(tVirtualID virtualID, Controller* pController,
                                        tKey keyMin, tKey keyMax)
{
    // Declarations
    tVirtualAxis virtualAxis = { 0 };

    virtualAxis.pController             = pController;
    virtualAxis.part                    = PART_KEY;
    virtualAxis.realPart.keys.keyMin    = keyMin;
    virtualAxis.realPart.keys.keyMax    = keyMax;
    virtualAxis.iValue                  = 0;
    virtualAxis.bChanged                = false;

    m_virtualAxes[virtualID] = virtualAxis;
}

//-----------------------------------------------------------------------

void VirtualController::addVirtualPOV(tVirtualID virtualID, Controller* pController, tPOV pov,
                                       const std::string& strShortcutUp,
                                       const std::string& strShortcutDown,
                                       const std::string& strShortcutLeft,
                                       const std::string& strShortcutRight)
{
    // Assertions
    assert(InputsUnit::getSingletonPtr());

    // Declarations
    tVirtualPOV virtualPOV = { 0 };

    virtualPOV.pController  = pController;
    virtualPOV.part         = PART_POV;
    virtualPOV.realPart.pov = pov;
    virtualPOV.position     = POV_CENTER;

    if (!strShortcutUp.empty())
    {
        InputsUnit* pInputsUnit = InputsUnit::getSingletonPtr();

        pInputsUnit->registerShortcut(strShortcutUp, virtualID);
        pInputsUnit->registerShortcut(strShortcutDown, virtualID);
        pInputsUnit->registerShortcut(strShortcutLeft, virtualID);
        pInputsUnit->registerShortcut(strShortcutRight, virtualID);

        pInputsUnit->registerShortcut(strShortcutUp + strShortcutLeft, virtualID);
        pInputsUnit->registerShortcut(strShortcutDown + strShortcutLeft, virtualID);
        pInputsUnit->registerShortcut(strShortcutUp + strShortcutRight, virtualID);
        pInputsUnit->registerShortcut(strShortcutDown + strShortcutRight, virtualID);

        virtualPOV.shortcuts.strShortcutUp      = strShortcutUp;
        virtualPOV.shortcuts.strShortcutDown    = strShortcutDown;
        virtualPOV.shortcuts.strShortcutLeft    = strShortcutLeft;
        virtualPOV.shortcuts.strShortcutRight   = strShortcutRight;
    }

    m_virtualPOVs[virtualID] = virtualPOV;
}

//-----------------------------------------------------------------------

void VirtualController::addVirtualPOV(tVirtualID virtualID, Controller* pController,
                                       tKey keyUp, tKey keyDown, tKey keyLeft, tKey keyRight,
                                       const std::string& strShortcutUp,
                                       const std::string& strShortcutDown,
                                       const std::string& strShortcutLeft,
                                       const std::string& strShortcutRight)
{
    // Assertions
    assert(InputsUnit::getSingletonPtr());

    // Declarations
    tVirtualPOV virtualPOV = { 0 };

    virtualPOV.pController              = pController;
    virtualPOV.part                     = PART_KEY;
    virtualPOV.realPart.keys.keyUp      = keyUp;
    virtualPOV.realPart.keys.keyDown    = keyDown;
    virtualPOV.realPart.keys.keyLeft    = keyLeft;
    virtualPOV.realPart.keys.keyRight   = keyRight;
    virtualPOV.position                 = POV_CENTER;

    if (!strShortcutUp.empty())
    {
        InputsUnit* pInputsUnit = InputsUnit::getSingletonPtr();

        pInputsUnit->registerShortcut(strShortcutUp, virtualID);
        pInputsUnit->registerShortcut(strShortcutDown, virtualID);
        pInputsUnit->registerShortcut(strShortcutLeft, virtualID);
        pInputsUnit->registerShortcut(strShortcutRight, virtualID);

        pInputsUnit->registerShortcut(strShortcutUp + strShortcutLeft, virtualID);
        pInputsUnit->registerShortcut(strShortcutDown + strShortcutLeft, virtualID);
        pInputsUnit->registerShortcut(strShortcutUp + strShortcutRight, virtualID);
        pInputsUnit->registerShortcut(strShortcutDown + strShortcutRight, virtualID);

        virtualPOV.shortcuts.strShortcutUp      = strShortcutUp;
        virtualPOV.shortcuts.strShortcutDown    = strShortcutDown;
        virtualPOV.shortcuts.strShortcutLeft    = strShortcutLeft;
        virtualPOV.shortcuts.strShortcutRight   = strShortcutRight;
    }

    m_virtualPOVs[virtualID] = virtualPOV;
}

//-----------------------------------------------------------------------

void VirtualController::addVirtualPOV(tVirtualID virtualID, Controller* pController,
                                       tAxis axisUpDown, tAxis axisLeftRight,
                                       const std::string& strShortcutUp,
                                       const std::string& strShortcutDown,
                                       const std::string& strShortcutLeft,
                                       const std::string& strShortcutRight)
{
    // Assertions
    assert(InputsUnit::getSingletonPtr());

    // Declarations
    tVirtualPOV virtualPOV = { 0 };

    virtualPOV.pController                  = pController;
    virtualPOV.part                         = PART_AXIS;
    virtualPOV.realPart.axes.axisUpDown     = axisUpDown;
    virtualPOV.realPart.axes.axisLeftRight  = axisLeftRight;
    virtualPOV.realPart.axes.iUpDownPos     = 0;
    virtualPOV.realPart.axes.iLeftRightPos  = 0;
    virtualPOV.realPart.axes.tempPosition   = POV_CENTER;
    virtualPOV.position                     = POV_CENTER;

    if (!strShortcutUp.empty())
    {
        InputsUnit* pInputsUnit = InputsUnit::getSingletonPtr();

        pInputsUnit->registerShortcut(strShortcutUp, virtualID);
        pInputsUnit->registerShortcut(strShortcutDown, virtualID);
        pInputsUnit->registerShortcut(strShortcutLeft, virtualID);
        pInputsUnit->registerShortcut(strShortcutRight, virtualID);

        pInputsUnit->registerShortcut(strShortcutUp + strShortcutLeft, virtualID);
        pInputsUnit->registerShortcut(strShortcutDown + strShortcutLeft, virtualID);
        pInputsUnit->registerShortcut(strShortcutUp + strShortcutRight, virtualID);
        pInputsUnit->registerShortcut(strShortcutDown + strShortcutRight, virtualID);

        virtualPOV.shortcuts.strShortcutUp      = strShortcutUp;
        virtualPOV.shortcuts.strShortcutDown    = strShortcutDown;
        virtualPOV.shortcuts.strShortcutLeft    = strShortcutLeft;
        virtualPOV.shortcuts.strShortcutRight   = strShortcutRight;
    }

    m_virtualPOVs[virtualID] = virtualPOV;
}

//-----------------------------------------------------------------------

bool VirtualController::isKeyPressed(tVirtualID virtualKey)
{
    // Declarations
    std::map<tVirtualID, tVirtualKey>::iterator iter;

    iter = m_virtualKeys.find(virtualKey);
    if (iter != m_virtualKeys.end())
    {
        return iter->second.bPressed;
    }

    return false;
}

//-----------------------------------------------------------------------

bool VirtualController::wasKeyToggled(tVirtualID virtualKey)
{
    // Declarations
    std::map<tVirtualID, tVirtualKey>::iterator iter;

    iter = m_virtualKeys.find(virtualKey);
    if (iter != m_virtualKeys.end())
    {
        return iter->second.bToggled;
    }

    return false;
}

//-----------------------------------------------------------------------

bool VirtualController::wasKeyPressed(tVirtualID virtualKey)
{
    // Declarations
    std::map<tVirtualID, tVirtualKey>::iterator iter;

    iter = m_virtualKeys.find(virtualKey);
    if (iter != m_virtualKeys.end())
    {
        return iter->second.bPressed && iter->second.bToggled;
    }

    return false;
}

//-----------------------------------------------------------------------

bool VirtualController::wasKeyReleased(tVirtualID virtualKey)
{
    // Declarations
    std::map<tVirtualID, tVirtualKey>::iterator iter;

    iter = m_virtualKeys.find(virtualKey);
    if (iter != m_virtualKeys.end())
    {
        return !iter->second.bPressed && iter->second.bToggled;
    }

    return false;
}

//-----------------------------------------------------------------------

unsigned int VirtualController::getKeyPressedDuration(tVirtualID virtualKey)
{
    // Declarations
    std::map<tVirtualID, tVirtualKey>::iterator iter;

    iter = m_virtualKeys.find(virtualKey);
    if (iter != m_virtualKeys.end())
    {
        if (iter->second.bPressed)
            return 0;

        return iter->second.ulReleaseTimestamp - iter->second.ulPressTimestamp;
    }

    return 0;
}

//-----------------------------------------------------------------------

int VirtualController::getAxisValue(tVirtualID virtualAxis)
{
    // Declarations
    std::map<tVirtualID, tVirtualAxis>::iterator    iter;

    iter = m_virtualAxes.find(virtualAxis);
    if (iter != m_virtualAxes.end())
    {
        return iter->second.iValue;
    }

    return 0;
}

//-----------------------------------------------------------------------

bool VirtualController::wasAxisChanged(tVirtualID virtualAxis)
{
    // Declarations
    std::map<tVirtualID, tVirtualAxis>::iterator    iter;

    iter = m_virtualAxes.find(virtualAxis);
    if (iter != m_virtualAxes.end())
    {
        return iter->second.bChanged;
    }

    return false;
}

//-----------------------------------------------------------------------

tPOVPosition VirtualController::getPOVPosition(tVirtualID virtualPOV)
{
    // Declarations
    std::map<tVirtualID, tVirtualPOV>::iterator iter;

    iter = m_virtualPOVs.find(virtualPOV);
    if (iter != m_virtualPOVs.end())
    {
        return iter->second.position;
    }

    return POV_CENTER;
}

//-----------------------------------------------------------------------

tPOVPosition VirtualController::getPOVPreviousPosition(tVirtualID virtualPOV)
{
    // Declarations
    std::map<tVirtualID, tVirtualPOV>::iterator iter;

    iter = m_virtualPOVs.find(virtualPOV);
    if (iter != m_virtualPOVs.end())
    {
        return iter->second.previousPosition;
    }

    return POV_CENTER;
}

//-----------------------------------------------------------------------

bool VirtualController::wasPOVChanged(tVirtualID virtualPOV)
{
    // Declarations
    std::map<tVirtualID, tVirtualPOV>::iterator iter;

    iter = m_virtualPOVs.find(virtualPOV);
    if (iter != m_virtualPOVs.end())
    {
        return iter->second.bChanged;
    }

    return false;
}

//-----------------------------------------------------------------------

tPOVPosition VirtualController::getPOVPositionFromShortcut(tVirtualID virtualPOV,
                                                           const std::string& strShortcut)
{
    // Declarations
    std::map<tVirtualID, tVirtualPOV>::iterator iter;

    iter = m_virtualPOVs.find(virtualPOV);
    if (iter != m_virtualPOVs.end())
    {
        if (strShortcut == iter->second.shortcuts.strShortcutUp)
            return POV_UP;
        else if (strShortcut == iter->second.shortcuts.strShortcutDown)
            return POV_DOWN;
        else if (strShortcut == iter->second.shortcuts.strShortcutLeft)
            return POV_LEFT;
        else if (strShortcut == iter->second.shortcuts.strShortcutRight)
            return POV_RIGHT;
        else if (strShortcut == iter->second.shortcuts.strShortcutDown + iter->second.shortcuts.strShortcutLeft)
            return POV_DOWNLEFT;
        else if (strShortcut == iter->second.shortcuts.strShortcutUp + iter->second.shortcuts.strShortcutLeft)
            return POV_UPLEFT;
        else if (strShortcut == iter->second.shortcuts.strShortcutDown + iter->second.shortcuts.strShortcutRight)
            return POV_DOWNRIGHT;
        else if (strShortcut == iter->second.shortcuts.strShortcutUp + iter->second.shortcuts.strShortcutRight)
            return POV_UPRIGHT;
        else
            return POV_CENTER;
    }

    return POV_CENTER;
}

//-----------------------------------------------------------------------

unsigned int VirtualController::getPOVPressedDuration(tVirtualID virtualPOV)
{
    // Declarations
    std::map<tVirtualID, tVirtualPOV>::iterator iter;

    iter = m_virtualPOVs.find(virtualPOV);
    if (iter != m_virtualPOVs.end())
    {
        if (iter->second.previousPosition == POV_CENTER)
            return 0;

        return iter->second.ulLastChangeTimestamp - iter->second.ulPreviousChangeTimestamp;
    }

    return 0;
}

//-----------------------------------------------------------------------

bool VirtualController::isKey(tVirtualID virtualID)
{
    return (m_virtualKeys.find(virtualID) != m_virtualKeys.end());
}

//-----------------------------------------------------------------------

bool VirtualController::isPOV(tVirtualID virtualID)
{
    return (m_virtualPOVs.find(virtualID) != m_virtualPOVs.end());
}

//-----------------------------------------------------------------------

unsigned int VirtualController::getNbVirtualKeys()
{
    return (unsigned int) m_virtualKeys.size();
}

//-----------------------------------------------------------------------

unsigned int VirtualController::getNbVirtualAxes()
{
    return (unsigned int) m_virtualAxes.size();
}

//-----------------------------------------------------------------------

unsigned int VirtualController::getNbVirtualPOVs()
{
    return (unsigned int) m_virtualPOVs.size();
}

//-----------------------------------------------------------------------

const tVirtualKey& VirtualController::getVirtualKey(unsigned int uiIndex, tVirtualID &id)
{
    // Assertions
    assert(uiIndex < (unsigned int) m_virtualKeys.size());

    // Declarations
    std::map<tVirtualID, tVirtualKey>::iterator iter;
    unsigned int i = 0;

    iter = m_virtualKeys.begin();
    while (i < uiIndex)
    {
        ++iter;
        ++i;
    }

    id = iter->first;
    return iter->second;
}

//-----------------------------------------------------------------------

const tVirtualAxis& VirtualController::getVirtualAxis(unsigned int uiIndex, tVirtualID &id)
{
    // Assertions
    assert(uiIndex < (unsigned int) m_virtualAxes.size());

    // Declarations
    std::map<tVirtualID, tVirtualAxis>::iterator iter;
    unsigned int i = 0;

    iter = m_virtualAxes.begin();
    while (i < uiIndex)
    {
        ++iter;
        ++i;
    }

    id = iter->first;
    return iter->second;
}

//-----------------------------------------------------------------------

const tVirtualPOV& VirtualController::getVirtualPOV(unsigned int uiIndex, tVirtualID &id)
{
    // Assertions
    assert(uiIndex < (unsigned int) m_virtualPOVs.size());

    // Declarations
    std::map<tVirtualID, tVirtualPOV>::iterator iter;
    unsigned int i = 0;

    iter = m_virtualPOVs.begin();
    while (i < uiIndex)
    {
        ++iter;
        ++i;
    }

    id = iter->first;
    return iter->second;
}

//-----------------------------------------------------------------------

tVirtualKey* VirtualController::getVirtualKey(tVirtualID id)
{
    // Declarations
    std::map<tVirtualID, tVirtualKey>::iterator iter;

    iter = m_virtualKeys.find(id);
    if (iter != m_virtualKeys.end())
    {
        return &iter->second;
    }

    return 0;
}

//-----------------------------------------------------------------------

tVirtualAxis* VirtualController::getVirtualAxis(tVirtualID id)
{
    // Declarations
    std::map<tVirtualID, tVirtualAxis>::iterator iter;

    iter = m_virtualAxes.find(id);
    if (iter != m_virtualAxes.end())
    {
        return &iter->second;
    }

    return 0;
}

//-----------------------------------------------------------------------

tVirtualPOV* VirtualController::getVirtualPOV(tVirtualID id)
{
    // Declarations
    std::map<tVirtualID, tVirtualPOV>::iterator iter;

    iter = m_virtualPOVs.find(id);
    if (iter != m_virtualPOVs.end())
    {
        return &iter->second;
    }

    return 0;
}
