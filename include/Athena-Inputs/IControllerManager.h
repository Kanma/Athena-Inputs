/** @file       IControllerManager.h
    @author     Philip Abbet

    Declaration of the interface 'Athena::Inputs::IControllerManager'
*/

#ifndef _ATHENA_INPUTS_ICONTROLLERMANAGER_H_
#define _ATHENA_INPUTS_ICONTROLLERMANAGER_H_

#include <Athena-Inputs/Prerequisites.h>


namespace Athena {
namespace Inputs {

//---------------------------------------------------------------------------------------
/// @brief  Base class for the Controller Manager (the implementation is plaform-specific)
///
/// A Controller Manager is responsible to detect all the controllers connected to the
/// computer, and to tell the Inputs Unit about them by calling InputsUnit::_addController().
//---------------------------------------------------------------------------------------
class ATHENA_INPUTS_SYMBOL IControllerManager
{
    //_____ Construction / Destruction __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief  Destructor
    //-----------------------------------------------------------------------------------
    virtual ~IControllerManager() {};


    //_____ Methods __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief  Called by the Inputs Unit to initialise the manager and detect the
    ///         controllers
    ///
    /// Must be overriden by each controller manager
    /// @param  mainWindowHandle    Handle of the window to use for capturing the events
    ///                             (Windows: the HWND, MacOS X: not used)
    /// @return                     'true' if successful
    //-----------------------------------------------------------------------------------
    virtual bool init(void* mainWindowHandle) = 0;
};

}
}

#endif
