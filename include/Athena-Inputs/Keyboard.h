/** @file   Keyboard.h
    @author Philip Abbet

    Declaration of the class 'Athena::Inputs::Keyboard'
*/

#ifndef _ATHENA_INPUTS_KEYBOARD_H
#define _ATHENA_INPUTS_KEYBOARD_H

#include <Athena-Inputs/Controller.h>
#include <OIS/OISKeyboard.h>


namespace Athena {
namespace Inputs {

//---------------------------------------------------------------------------------------
/// @brief  Represents a keyboard
//---------------------------------------------------------------------------------------
class ATHENA_INPUTS_SYMBOL Keyboard: public Controller, public OIS::KeyListener
{
    //_____ Construction / Destruction __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief  Constructor
    /// @param  pOISObject  The OIS controller object
    //-----------------------------------------------------------------------------------
    Keyboard(OIS::Object* pOISObject);

    //-----------------------------------------------------------------------------------
    /// @brief  Destructor
    //-----------------------------------------------------------------------------------
    virtual ~Keyboard();


    //_____ Methods __________
public:
    inline OIS::Keyboard* getOISKeyboard()
    {
        return static_cast<OIS::Keyboard*>(m_pOISObject);
    }

    inline void setCharacterMode(bool enabled)
    {
        m_bCharacterMode = enabled;
    }


    //_____ Implementation of OIS::KeyListener __________
public:
    virtual bool keyPressed(const OIS::KeyEvent &arg);
    virtual bool keyReleased(const OIS::KeyEvent &arg);


    //_____ Attributes __________
protected:
    bool m_bCharacterMode;
};

}
}

#endif
