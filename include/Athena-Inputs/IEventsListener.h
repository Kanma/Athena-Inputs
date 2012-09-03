/** @file   IEventsListener.h
    @author Philip Abbet

    Declaration of the interface 'Athena::Inputs::IEventsListener'
*/

#ifndef _ATHENA_INPUTS_IEVENTSLISTENER_H_
#define _ATHENA_INPUTS_IEVENTSLISTENER_H_

#include <Athena-Inputs/Prerequisites.h>
#include <Athena-Inputs/Declarations.h>


namespace Athena {
namespace Inputs {

//---------------------------------------------------------------------------------------
/// @brief  Represents an events listener for a controller
//---------------------------------------------------------------------------------------
class ATHENA_INPUTS_SYMBOL IEventsListener
{
    //_____ Construction / Destruction __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief  Destructor
    //-----------------------------------------------------------------------------------
    virtual ~IEventsListener() {};


    //_____ Methods to override __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief  Called when an event occured on the controller the listener is attached to
    ///
    /// Must be overriden by each listener
    /// @param  pEvent  The event
    //-----------------------------------------------------------------------------------
    virtual void onEvent(tInputEvent* pEvent) = 0;

    //-----------------------------------------------------------------------------------
    /// @brief  Called when a character was entered on the controller the listener is
    ///         attached to
    ///
    /// Must be overriden by each listener for which it makes sense
    /// @param  strUTF8Character    The character, encoded in UTF-8
    //-----------------------------------------------------------------------------------
    virtual void onCharacter(const std::string& strUTF8Character) {}
};

}
}

#endif
