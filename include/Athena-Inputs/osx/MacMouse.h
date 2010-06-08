/** @file	MacMouse.h
	@author	Philip Abbet
 
	Declaration of the class 'Athena::Inputs::MacMouse'
*/

#ifndef _ATHENA_INPUTS_MACMOUSE_H_
#define _ATHENA_INPUTS_MACMOUSE_H_

#import <Carbon/Carbon.h>

#include <Athena-Inputs/Prerequisites.h>
#include <Athena-Inputs/Declarations.h>
#include <Athena-Inputs/osx/MacController.h>


namespace Athena {
namespace Inputs {


//---------------------------------------------------------------------------------------
/// @brief	Represents a mouse on MacOS X
//---------------------------------------------------------------------------------------
class MacMouse: public MacController
{
	//_____ Construction / Destruction __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief	Constructor
    ///
    /// @param	hidDevice	HID device
    //-----------------------------------------------------------------------------------
	MacMouse(io_object_t hidDevice);

    //-----------------------------------------------------------------------------------
    /// @brief	Destructor
    //-----------------------------------------------------------------------------------
	virtual ~MacMouse();
	
	
	//_____ Management of the mouse __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief	Read the state of the mouse's keys and axes
    /// 
    /// @return 'true' if successful
    //-----------------------------------------------------------------------------------
	virtual bool readInputs();
	
	virtual bool setSensibility(float fSensibility);
	virtual float getSensibility();
	
	virtual const std::string getNameOfKey(tKey key);
	virtual const std::string getNameOfAxis(tAxis axis);
	
protected:
	virtual bool acquire(bool bAcquire = true);


	//_____ Event handlers __________
public:
	static OSStatus WindowFocusChanged(EventHandlerCallRef nextHandler, EventRef theEvent, void* macMouse);
	static OSStatus MouseWrapper(EventHandlerCallRef nextHandler, EventRef theEvent, void* macMouse);
	
	
	//_____ Attributes __________
protected:
	float						m_fSensibility;
	std::vector<tInputEvent>	m_events;
	
	// "Universal procedure pointers" - required reference for callbacks
	EventHandlerUPP m_mouseUPP;
	EventHandlerRef m_mouseEventRef;
	
	EventHandlerUPP m_windowFocusListener;
	EventHandlerRef m_windowFocusHandler;
};

}
}

#endif
