/** @file   MacGamepad.h
    @author Philip Abbet

    Declaration of the class 'Athena::Inputs::MacGamepad'
*/

#ifndef _ATHENA_INPUTS_MACGAMEPAD_H_
#define _ATHENA_INPUTS_MACGAMEPAD_H_

#include <Athena-Inputs/Prerequisites.h>
#include <Athena-Inputs/Declarations.h>
#include <Athena-Inputs/osx/MacController.h>

#import <IOKit/hid/IOHIDLib.h>


namespace Athena {
namespace Inputs {


//---------------------------------------------------------------------------------------
/// @brief  Represents a gamepad on MacOS X
//---------------------------------------------------------------------------------------
class MacGamepad: public MacController
{
    //_____ Construction / Destruction __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief  Constructor
    ///
    /// @param  hidDevice   HID device
    //-----------------------------------------------------------------------------------
    MacGamepad(io_object_t hidDevice, unsigned int uiNo = 1);

    //-----------------------------------------------------------------------------------
    /// @brief  Destructor
    //-----------------------------------------------------------------------------------
    virtual ~MacGamepad();


    //_____ Management of the gamepad __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief  Read the state of the gamepad's keys and axes
    ///
    /// @return 'true' if successful
    //-----------------------------------------------------------------------------------
    virtual bool readInputs();

protected:
    virtual bool acquire(bool bAcquire = true);


    //_____ Attributes __________
protected:
    IOHIDDeviceInterface**  m_ppHidDeviceInterface;
    IOHIDQueueInterface**   m_ppQueue;
};

}
}

#endif
