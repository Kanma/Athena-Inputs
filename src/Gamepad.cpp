/** @file   Gamepad.cpp
    @author Philip Abbet

    Implementation of the class 'Athena::Inputs::Gamepad'
*/

#include <Athena-Inputs/Gamepad.h>
#include <Athena-Inputs/IEventsListener.h>


using namespace Athena;
using namespace Athena::Inputs;
using namespace Athena::Utils;
using namespace std;


/****************************** CONSTRUCTION / DESTRUCTION ******************************/

Gamepad::Gamepad(OIS::Object* pOISObject)
: Controller(pOISObject, 1)
{
    assert(pOISObject->type() == OIS::OISJoyStick);

    static_cast<OIS::JoyStick*>(pOISObject)->setEventCallback(this);
}

//-----------------------------------------------------------------------

Gamepad::~Gamepad()
{
}


/************************* IMPLEMENTATION OF OIS::JoyStickListener **********************/

bool Gamepad::buttonPressed(const OIS::JoyStickEvent &arg, int button)
{
    // Push the event in the list
    tListenersList::iterator listenersIter, listenersIterEnd;
    tInputEvent event;

    event.pController       = this;
    event.part              = PART_KEY;
    event.partID.key        = button;
    event.value.bPressed    = true;

    for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end();
         listenersIter != listenersIterEnd; ++listenersIter)
    {
        (*listenersIter)->onEvent(&event);
    }

    return true;
}

//-----------------------------------------------------------------------

bool Gamepad::buttonReleased(const OIS::JoyStickEvent &arg, int button)
{
    // Push the event in the list
    tListenersList::iterator listenersIter, listenersIterEnd;
    tInputEvent event;

    event.pController       = this;
    event.part              = PART_KEY;
    event.partID.key        = button;
    event.value.bPressed    = false;

    for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end();
         listenersIter != listenersIterEnd; ++listenersIter)
    {
        (*listenersIter)->onEvent(&event);
    }

    return true;
}

//-----------------------------------------------------------------------

bool Gamepad::axisMoved(const OIS::JoyStickEvent &arg, int axis)
{
    // Push the event in the list
    tListenersList::iterator listenersIter, listenersIterEnd;
    tInputEvent event;

    event.pController   = this;
    event.part          = PART_AXIS;
    event.partID.axis   = (AXIS_X << axis);
    event.value.iValue  = arg.state.mAxes[axis].abs;

    for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end();
         listenersIter != listenersIterEnd; ++listenersIter)
    {
        (*listenersIter)->onEvent(&event);
    }

    return true;
}

//-----------------------------------------------------------------------

bool Gamepad::sliderMoved(const OIS::JoyStickEvent &arg, int index)
{
    // // Push the event in the list
    // tListenersList::iterator listenersIter, listenersIterEnd;
    // tInputEvent event;
    //
    // event.pController   = this;
    // event.part          = PART_AXIS;
    // event.partID.axis   = (SLIDER_0 << index);
    // event.value.iValue  = arg.state.mSliders[index].abs;
    //
    // for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end();
    //      listenersIter != listenersIterEnd; ++listenersIter)
    // {
    //     (*listenersIter)->onEvent(&event);
    // }
    //
    return true;
}

//-----------------------------------------------------------------------

bool Gamepad::povMoved(const OIS::JoyStickEvent &arg, int index)
{
    // Push the event in the list
    tListenersList::iterator listenersIter, listenersIterEnd;
    tInputEvent event;

    event.pController   = this;
    event.part          = PART_POV;
    event.partID.pov    = index;
    event.value.iValue  = arg.state.mPOV[index].direction;

    for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end();
         listenersIter != listenersIterEnd; ++listenersIter)
    {
        (*listenersIter)->onEvent(&event);
    }

    return true;
}
