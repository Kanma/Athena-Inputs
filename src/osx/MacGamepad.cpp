/** @file	MacGamepad.cpp
	@author	Philip Abbet
 
	Implementation of the class 'Athena::Inputs::MacGamepad'
*/

#import <CoreFoundation/CoreFoundation.h>
#import <IOKit/IOCFPlugIn.h>

#include <Athena-Inputs/osx/MacGamepad.h>
#include <Athena-Inputs/osx/MacHelpers.h>
#include <Athena-Inputs/IEventsListener.h>
#include <Athena-Core/Log/LogManager.h>


using namespace Athena;
using namespace Athena::Inputs;
using namespace Athena::Log;
using namespace std;


/************************************** CONSTANTS **************************************/

static const char*	__CONTEXT__ = "Mac Gamepad";

static const tPOVPosition POV_CONVERSION_TABLE[] =
{
	POV_UP,
	POV_UPRIGHT,
	POV_RIGHT,
	POV_DOWNRIGHT,
	POV_DOWN,
	POV_DOWNLEFT,
	POV_LEFT,
	POV_UPLEFT,
};

const unsigned int NB_POV_CONVERSIONS = sizeof(POV_CONVERSION_TABLE) / sizeof(tPOVPosition);


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

MacGamepad::MacGamepad(io_object_t hidDevice, unsigned int uiNo)
: MacController(hidDevice, CONTROLLER_GAMEPAD, uiNo), m_ppHidDeviceInterface(0),
  m_ppQueue(0)
{
	if (m_bInitialized)
	{
		IOCFPlugInInterface** plugInInterface = 0;
		SInt32 score = 0;
		IOReturn ioReturnValue = IOCreatePlugInInterfaceForService(hidDevice,
									kIOHIDDeviceUserClientTypeID, kIOCFPlugInInterfaceID,
									&plugInInterface, &score);
		if (ioReturnValue == kIOReturnSuccess)
		{
			// Call a method of the intermediate plug-in to create the device interface
			HRESULT plugInResult = (*plugInInterface)->QueryInterface(plugInInterface,
										CFUUIDGetUUIDBytes(kIOHIDDeviceInterfaceID),
										(void**) &m_ppHidDeviceInterface);
			if (plugInResult != S_OK)
				ATHENA_LOG_ERROR("Couldn't query HID class device interface for the device interface");

			(*plugInInterface)->Release(plugInInterface);

			// Open the device
			ioReturnValue = (*m_ppHidDeviceInterface)->open(m_ppHidDeviceInterface, 0);
			if (ioReturnValue == kIOReturnSuccess)
			{
				m_ppQueue = (*m_ppHidDeviceInterface)->allocQueue(m_ppHidDeviceInterface);
				if (m_ppQueue)
				{
					(*m_ppQueue)->create(m_ppQueue, 0, MAX_BUFFER_SIZE);
					
					// Add elements to the queue
					std::map<IOHIDElementCookie, tCookieInfos>::iterator iter, iterEnd;
					for (iter = m_cookies.begin(), iterEnd = m_cookies.end(); iter != iterEnd; ++iter)
						(*m_ppQueue)->addElement(m_ppQueue, iter->first, 0);
				}
				else
				{
					ATHENA_LOG_ERROR("Failed to allocate the queue");
				}
			}
			else
			{
				ATHENA_LOG_ERROR("Failed to open the device interface");
			}
		}
		else
		{
			ATHENA_LOG_ERROR("Failed to create the plugin interface via IOCreatePlugInInterfaceForService");
		}
	}
	
	m_bInitialized = (m_ppQueue != 0);
}

//-----------------------------------------------------------------------

MacGamepad::~MacGamepad()
{
	ATHENA_LOG_EVENT("Destruction");

	// Dispose the queue
	if (m_ppQueue)
	{
		(*m_ppQueue)->dispose(m_ppQueue);
		(*m_ppQueue)->Release(m_ppQueue);
	}

	// Release the device interface
	if (m_ppHidDeviceInterface)
	{
		(*m_ppHidDeviceInterface)->close(m_ppHidDeviceInterface);
		(*m_ppHidDeviceInterface)->Release(m_ppHidDeviceInterface);
	}
}


/****************************** MANAGEMENT OF THE MOUSE ********************************/

bool MacGamepad::readInputs()
{
	// Declarations
	AbsoluteTime zeroTime = {0,0};
	IOHIDEventStruct event;
	vector<IEventsListener*>::iterator	listenersIter, listenersIterEnd;
	
	while (true)
	{
		HRESULT result = (*m_ppQueue)->getNextEvent(m_ppQueue, &event, zeroTime, 0);
		
		if (result == kIOReturnUnderrun)
			break;
		else if (result != kIOReturnSuccess)
			return false;

		tCookieInfos infos = m_cookies[event.elementCookie];

		tInputEvent inputEvent;
		inputEvent.pController	= this;
		inputEvent.part			= infos.type;
		inputEvent.partID		= infos.part;
		inputEvent.dwTimeStamp	= event.timestamp.lo;
		inputEvent.dwSequence	= 0;
		
		switch (infos.type)
		{
			case PART_KEY:
				inputEvent.value.bPressed = (event.value != 0);
				break;

			case PART_AXIS:
				inputEvent.value.iValue = event.value;
				break;

			case PART_POV:
				if (event.value < NB_POV_CONVERSIONS)
					inputEvent.value.position = POV_CONVERSION_TABLE[event.value];
				else
					inputEvent.value.position = POV_CENTER;
				break;
		}
		
		for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end();
			 listenersIter != listenersIterEnd; ++listenersIter)
		{
			(*listenersIter)->onEvent(&inputEvent);
		}
	}
	
	return true;
}

//-----------------------------------------------------------------------

bool MacGamepad::acquire(bool bAcquire)
{
	if (bAcquire)
		return SUCCEEDED((*m_ppQueue)->start(m_ppQueue));
	else
		return SUCCEEDED((*m_ppQueue)->stop(m_ppQueue));
}
