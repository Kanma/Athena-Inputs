/** @file		MacControllerManager.h
	@author		Philip Abbet

	Declaration of the class 'Athena::Inputs::MacControllersManager'
*/

#ifndef _ATHENA_INPUTS_MACCONTROLLERMANAGER_H_
#define _ATHENA_INPUTS_MACCONTROLLERMANAGER_H_

#include <Athena-Inputs/Prerequisites.h>
#include <Athena-Inputs/Declarations.h>
#include <Athena-Inputs/IControllerManager.h>

#import <IOKit/hid/IOHIDKeys.h>


namespace Athena {
namespace Inputs {


//---------------------------------------------------------------------------------------
/// @brief	MacOS X-specific Controller Manager
//---------------------------------------------------------------------------------------
class MacControllerManager: public IControllerManager
{
	//_____ Construction / Destruction __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief	Constructor
    //-----------------------------------------------------------------------------------
	MacControllerManager();

    //-----------------------------------------------------------------------------------
    /// @brief	Destructor
    //-----------------------------------------------------------------------------------
	virtual ~MacControllerManager();


	//_____ Implementation of IControllerManager __________
public:
	//-----------------------------------------------------------------------------------
	/// @brief	Called by the Inputs Unit to initialise the manager and detect the
	///			controllers
	///	
	/// @param	mainWindowHandle	not used
	/// @return			            'true' if successful
	//-----------------------------------------------------------------------------------
	virtual bool init(void* mainWindowHandle);


	//_____ Management of the controllers __________
private:
    //-----------------------------------------------------------------------------------
    /// @brief	Initializes the keyboards
    /// @return	'true' if successful
    //-----------------------------------------------------------------------------------
	bool initKeyboards();

    //-----------------------------------------------------------------------------------
    /// @brief	Initializes the mouses
    /// return	'true' if successful
    //-----------------------------------------------------------------------------------
	bool initMouses();

    //-----------------------------------------------------------------------------------
    /// @brief	Initializes the gamepads
    /// return	'true' if successful
    //-----------------------------------------------------------------------------------
	bool initGamepads();

	io_iterator_t getDevicesIterator(int usage);
};

}
}

#endif
