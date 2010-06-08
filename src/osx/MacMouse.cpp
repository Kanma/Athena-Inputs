/** @file	MacMouse.cpp
	@author	Philip Abbet
 
	Implementation of the class 'Athena::Inputs::MacMouse'
*/

#import <CoreFoundation/CoreFoundation.h>

#include <Athena-Inputs/osx/MacMouse.h>
#include <Athena-Core/Log/LogManager.h>
#include <Athena-Inputs/IEventsListener.h>


using namespace Athena;
using namespace Athena::Inputs;
using namespace Athena::Log;
using namespace std;


/******************************************* CONSTANTS ********************************************/

static const char*	__CONTEXT__ = "Mac Mouse";

// Events we subscribe to and remove from queue
const EventTypeSpec mouseEvents[] =
{
	{ kEventClassMouse, kEventMouseDown },
	{ kEventClassMouse, kEventMouseUp },
	{ kEventClassMouse, kEventMouseMoved },
	{ kEventClassMouse, kEventMouseDragged },
	{ kEventClassMouse, kEventMouseWheelMoved }		
};

const EventTypeSpec WinFocusAcquired [] =
{
	{ kEventClassApplication, kEventAppDeactivated },
	{ kEventClassApplication, kEventAppActivated }
};


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

MacMouse::MacMouse(io_object_t hidDevice)
: MacController(hidDevice, CONTROLLER_MOUSE, 1), m_fSensibility(1.0f), m_mouseEventRef(0),
  m_windowFocusHandler(0)
{
    // Get a "Universal procedure pointer" for our callbacks
    m_mouseUPP = NewEventHandlerUPP(MouseWrapper);
	m_windowFocusListener = NewEventHandlerUPP(WindowFocusChanged);
}

//-----------------------------------------------------------------------

MacMouse::~MacMouse()
{
	ATHENA_LOG_EVENT("Destruction");

	if (m_mouseEventRef)
		RemoveEventHandler(m_mouseEventRef);
	
	if (m_windowFocusHandler)
		RemoveEventHandler(m_windowFocusHandler);
	
	DisposeEventHandlerUPP(m_mouseUPP);
	DisposeEventHandlerUPP(m_windowFocusListener);
	
	// Restore Mouse
	CGAssociateMouseAndMouseCursorPosition(TRUE);
	CGDisplayShowCursor(kCGDirectMainDisplay);
}


/****************************** MANAGEMENT OF THE MOUSE ********************************/

bool MacMouse::readInputs()
{
	// Declarations
	vector<tInputEvent>::iterator		eventsIter, eventsIterEnd;
	vector<IEventsListener*>::iterator	listenersIter, listenersIterEnd;
	
	// Check that the mouse is active
	if (!m_bActive)
		return false;
	
	// Process each event
	for (eventsIter = m_events.begin(), eventsIterEnd = m_events.end(); eventsIter != eventsIterEnd; ++eventsIter)
	{
		tInputEvent event = *eventsIter;
		
		for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end(); listenersIter != listenersIterEnd; ++listenersIter)
		{
			(*listenersIter)->onEvent(&event);
		}
	}
	
	m_events.clear();
	
	return true;
}

//-----------------------------------------------------------------------

bool MacMouse::setSensibility(float fSensibility)
{
	m_fSensibility = fSensibility;
	return true;
}

//-----------------------------------------------------------------------

float MacMouse::getSensibility()
{
	return m_fSensibility;
}

//-----------------------------------------------------------------------

const string MacMouse::getNameOfKey(tKey key)
{
	// Declarations
	string strName;
	
	// Test if the name isn't already in the list
	strName = Controller::getNameOfKey(key);
	if (!strName.empty())
		return strName;
	
	switch (key)
	{
		case MOUSEKEY_LEFT:		strName = "Left button"; break;
		case MOUSEKEY_RIGHT:	strName = "Right button"; break;
		case MOUSEKEY_CENTER:	strName = "Center button"; break;
		default:				return MacController::getNameOfKey(key);
	}

	// Put the name in the list
	m_strKeys[key] = strName;
	
	return strName;
}

//-----------------------------------------------------------------------

const string MacMouse::getNameOfAxis(tAxis axis)
{
	// Declarations
	string strName;
	
	// Test if the name isn't already in the list
	strName = Controller::getNameOfAxis(axis);
	if (!strName.empty())
		return strName;
	
	switch (axis)
	{
		case AXIS_X:		strName = "X-axis"; break;
		case AXIS_Y:		strName = "Y-axis"; break;
		case AXIS_Z:		strName = "Wheel Y-axis"; break;
		case AXIS_Z_ROT:	strName = "Wheel X-axis"; break;
		default:			return MacController::getNameOfAxis(axis);
	}
	
	// Put the name in the list
	m_strAxes[axis] = strName;
	
	return strName;
}

//-----------------------------------------------------------------------

bool MacMouse::acquire(bool bAcquire)
{
	// Start listening for events
	EventTargetRef event = GetApplicationEventTarget();
    
	if (m_mouseEventRef)
	{
		RemoveEventHandler(m_mouseEventRef);
		m_mouseEventRef = 0;
	}
	
	if (m_windowFocusHandler)
	{
		RemoveEventHandler(m_windowFocusHandler);
		m_windowFocusHandler = 0;
	}
	
	if (bAcquire)
	{
		if (InstallEventHandler(event, m_mouseUPP, GetEventTypeCount(mouseEvents), mouseEvents, this, &m_mouseEventRef) != noErr)
		{
			ATHENA_LOG_ERROR("Failed to install the mouse event handler");
			return false;
		}
		
		if (InstallEventHandler(event, m_windowFocusListener, GetEventTypeCount(WinFocusAcquired), WinFocusAcquired, this, &m_windowFocusHandler) != noErr)
		{
			ATHENA_LOG_ERROR("Failed to install the window focus event handler");
			return false;
		}
	}

	return true;
}


/***************************************** EVENT HANDLERS *****************************************/

OSStatus MacMouse::WindowFocusChanged(EventHandlerCallRef nextHandler, EventRef theEvent, void* macMouse)
{
	MacMouse* pMouse = static_cast<MacMouse*>(macMouse);
    if (pMouse && pMouse->m_bExclusiveMode)
	{
		if (GetEventKind(theEvent) == kEventAppDeactivated)
		{
			CGAssociateMouseAndMouseCursorPosition(TRUE);
			CGDisplayShowCursor(kCGDirectMainDisplay);
		}
		else
		{
			CGAssociateMouseAndMouseCursorPosition(FALSE);
			CGDisplayHideCursor(kCGDirectMainDisplay);

			CGRect displayRect = CGDisplayBounds(kCGDirectMainDisplay);
			
			CGPoint warpPoint;
			warpPoint.x = (displayRect.size.width * 0.5f) + displayRect.origin.x;
			warpPoint.y = (displayRect.size.height * 0.5f) + displayRect.origin.y;
			CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, warpPoint);
		}
	}
	
	// Propagate the event down the chain
	return CallNextEventHandler(nextHandler, theEvent);		 
}

//-----------------------------------------------------------------------

OSStatus MacMouse::MouseWrapper(EventHandlerCallRef nextHandler, EventRef theEvent, void* macMouse)
{
	MacMouse* pMouse = static_cast<MacMouse*>(macMouse);
    if (pMouse)
	{
		OSStatus result = eventNotHandledErr;
		
		switch (GetEventKind(theEvent))
		{
			case kEventMouseDragged:
			case kEventMouseMoved:
			{
				HIPoint delta = {0.0f, 0.0f};
				
				GetEventParameter(theEvent, kEventParamMouseDelta, typeHIPoint, NULL, sizeof(HIPoint), NULL, &delta);

				if (fabs(delta.x) >= 1.0f)
				{
					tInputEvent event;

					event.part			= PART_AXIS;
					event.partID.axis	= AXIS_X;
					event.value.iValue	= (int) (delta.x * pMouse->m_fSensibility);
					event.pController	= pMouse;
					event.dwTimeStamp	= (unsigned int) GetEventTime(theEvent);
					event.dwSequence	= pMouse->m_events.size();
					
					pMouse->m_events.push_back(event);
				}
				
				if (fabs(delta.y) >= 1.0f)
				{
					tInputEvent event;

					event.part			= PART_AXIS;
					event.partID.axis	= AXIS_Y;
					event.value.iValue	= (int) (delta.y * pMouse->m_fSensibility);
					event.pController	= pMouse;
					event.dwTimeStamp	= (unsigned int) GetEventTime(theEvent);
					event.dwSequence	= pMouse->m_events.size();
					
					pMouse->m_events.push_back(event);
				}

				if (pMouse->m_bExclusiveMode)
				{
					CGRect displayRect = CGDisplayBounds(kCGDirectMainDisplay);
					
					CGPoint warpPoint;
					warpPoint.x = (displayRect.size.width * 0.5f) + displayRect.origin.x;
					warpPoint.y = (displayRect.size.height * 0.5f) + displayRect.origin.y;
					CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, warpPoint);
				}

				break;
			}
			
			case kEventMouseDown:
			case kEventMouseUp:
			{
				EventMouseButton	button		= 0;
				UInt32				modifiers	= 0;
				
				GetEventParameter(theEvent, kEventParamMouseButton, typeMouseButton, NULL, sizeof(EventMouseButton), NULL, &button);
				GetEventParameter(theEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifiers);

				tInputEvent event;

				event.part				= PART_KEY;
				event.value.bPressed	= (GetEventKind(theEvent) == kEventMouseDown);
				
				if ((button == kEventMouseButtonPrimary) && (modifiers & optionKey))
					event.partID.key = MOUSEKEY_CENTER;
				else if ((button == kEventMouseButtonPrimary) && (modifiers & controlKey))
					event.partID.key = MOUSEKEY_RIGHT;
				else
					event.partID.key = (tKey) button;
				
				event.pController	= pMouse;
				event.dwTimeStamp	= (unsigned int) GetEventTime(theEvent);
				event.dwSequence	= pMouse->m_events.size();
				
				pMouse->m_events.push_back(event);

				break;
			}

			case kEventMouseWheelMoved:
			{
				SInt32				wheelDelta	= 0;
				EventMouseWheelAxis	wheelAxis	= 0; 
				
				GetEventParameter(theEvent, kEventParamMouseWheelAxis, typeMouseWheelAxis, NULL, sizeof(EventMouseWheelAxis), NULL, &wheelAxis);
				GetEventParameter(theEvent, kEventParamMouseWheelDelta, typeSInt32, NULL, sizeof(SInt32), NULL, &wheelDelta);
				
				tInputEvent event;

				event.part			= PART_AXIS;
				event.partID.axis	= (wheelAxis == kEventMouseWheelAxisY ? AXIS_Z : AXIS_Z_ROT);
				event.value.iValue	= (int) (wheelDelta * pMouse->m_fSensibility);
				event.pController	= pMouse;
				event.dwTimeStamp	= (unsigned int) GetEventTime(theEvent);
				event.dwSequence	= pMouse->m_events.size();
				
				pMouse->m_events.push_back(event);
				
				break;
			}

			default:
				break;
		}    
	}
	
	// Propagate the event down the chain
	return CallNextEventHandler(nextHandler, theEvent);		 
}
