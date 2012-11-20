/** @file   Gamepad.h
    @author Philip Abbet

    Declaration of the class 'Athena::Inputs::Gamepad'
*/

#ifndef _ATHENA_INPUTS_GAMEPAD_H
#define _ATHENA_INPUTS_GAMEPAD_H

#include <Athena-Inputs/Controller.h>
#include <OIS/OISJoyStick.h>


namespace Athena {
namespace Inputs {

//---------------------------------------------------------------------------------------
/// @brief  Represents a mouse
//---------------------------------------------------------------------------------------
class ATHENA_INPUTS_SYMBOL Gamepad: public Controller, public OIS::JoyStickListener
{
    //_____ Construction / Destruction __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief  Constructor
    /// @param  pOISObject  The OIS controller object
    //-----------------------------------------------------------------------------------
    Gamepad(OIS::Object* pOISObject);

    //-----------------------------------------------------------------------------------
    /// @brief  Destructor
    //-----------------------------------------------------------------------------------
    virtual ~Gamepad();


    //_____ Methods __________
public:
    inline OIS::JoyStick* getOISJoyStick()
    {
        return static_cast<OIS::JoyStick*>(m_pOISObject);
    }


    //_____ Implementation of OIS::MouseListener __________
public:
    virtual bool buttonPressed(const OIS::JoyStickEvent &arg, int button);
    virtual bool buttonReleased(const OIS::JoyStickEvent &arg, int button);
    virtual bool axisMoved(const OIS::JoyStickEvent &arg, int axis);
    virtual bool sliderMoved(const OIS::JoyStickEvent &arg, int index);
    virtual bool povMoved(const OIS::JoyStickEvent &arg, int index);
};

}
}

#endif
