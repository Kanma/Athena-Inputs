/** @file   Mouse.h
    @author Philip Abbet

    Declaration of the class 'Athena::Inputs::Mouse'
*/

#ifndef _ATHENA_INPUTS_MOUSE_H
#define _ATHENA_INPUTS_MOUSE_H

#include <Athena-Inputs/Controller.h>
#include <OIS/OISMouse.h>


namespace Athena {
namespace Inputs {

//---------------------------------------------------------------------------------------
/// @brief  Represents a mouse
//---------------------------------------------------------------------------------------
class ATHENA_INPUTS_SYMBOL Mouse: public Controller, public OIS::MouseListener
{
    //_____ Construction / Destruction __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief  Constructor
    /// @param  pOISObject  The OIS controller object
    //-----------------------------------------------------------------------------------
    Mouse(OIS::Object* pOISObject);

    //-----------------------------------------------------------------------------------
    /// @brief  Destructor
    //-----------------------------------------------------------------------------------
    virtual ~Mouse();


    //_____ Methods __________
public:
    inline OIS::Mouse* getOISMouse()
    {
        return static_cast<OIS::Mouse*>(m_pOISObject);
    }


    //_____ Implementation of OIS::MouseListener __________
public:
    virtual bool mouseMoved(const OIS::MouseEvent &arg);
    virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
};

}
}

#endif
