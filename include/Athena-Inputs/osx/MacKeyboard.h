/** @file	MacKeyboard.h
	@author	Philip Abbet
 
	Declaration of the class 'Athena::Inputs::MacKeyboard'
*/

#ifndef _ATHENA_INPUTS_MACKEYBOARD_H_
#define _ATHENA_INPUTS_MACKEYBOARD_H_

#import <Carbon/Carbon.h>

#include <Athena-Inputs/Prerequisites.h>
#include <Athena-Inputs/Declarations.h>
#include <Athena-Inputs/osx/MacController.h>

namespace Athena {
namespace Inputs {


//---------------------------------------------------------------------------------------
/// @brief	Represents a keyboard on MacOS X
//---------------------------------------------------------------------------------------
class MacKeyboard: public MacController
{
	//_____ Construction / Destruction __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief	Constructor
    ///
    /// @param	hidDevice	HID device
    //-----------------------------------------------------------------------------------
	MacKeyboard(io_object_t hidDevice);

    //-----------------------------------------------------------------------------------
    /// @brief	Destructor
    //-----------------------------------------------------------------------------------
	virtual ~MacKeyboard();
	
	
	//_____ Management of the mouse __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief	Read the state of the keyboard's keys
    /// 
    /// @return 'true' if successful
    //-----------------------------------------------------------------------------------
	virtual bool readInputs();
	
	virtual const std::string getNameOfKey(tKey key);
	
protected:
	virtual bool acquire(bool bAcquire = true);
	
	
	//_____ Event handlers __________
public:
	static OSStatus KeyDownWrapper(EventHandlerCallRef nextHandler, EventRef theEvent, void* macKeyboard);
	static OSStatus KeyUpWrapper(EventHandlerCallRef nextHandler, EventRef theEvent, void* macKeyboard);
	static OSStatus KeyModWrapper(EventHandlerCallRef nextHandler, EventRef theEvent, void* macKeyboard);
	
	
	//_____ Internal types __________
protected:
	struct tKeyboardEvent
	{
		bool		bCharacter;
		tInputEvent event;
		std::string	strUTF8Character;
	};

	
	//_____ Attributes __________
protected:
	std::vector<tKeyboardEvent>	m_events;
	UInt32						m_previousModMask;
	
	// "Universal procedure pointers" - required reference for callbacks
	EventHandlerUPP m_keyDownUPP;
	EventHandlerRef m_keyDownRef;
	
	EventHandlerUPP m_keyUpUPP;
	EventHandlerRef m_keyUpRef;

	EventHandlerUPP m_keyModUPP;
	EventHandlerRef m_keyModRef;
};

}
}

#endif
