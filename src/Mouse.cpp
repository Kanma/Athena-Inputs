/** @file   Mouse.cpp
    @author Philip Abbet

    Implementation of the class 'Athena::Inputs::Mouse'
*/

#include <Athena-Inputs/Mouse.h>
#include <Athena-Inputs/IEventsListener.h>


using namespace Athena;
using namespace Athena::Inputs;
using namespace Athena::Utils;
using namespace std;


/****************************** CONSTRUCTION / DESTRUCTION ******************************/

Mouse::Mouse(OIS::Object* pOISObject)
: Controller(pOISObject, 1)
{
    assert(pOISObject->type() == OIS::OISMouse);

    static_cast<OIS::Mouse*>(pOISObject)->setEventCallback(this);
}

//-----------------------------------------------------------------------

Mouse::~Mouse()
{
}


/*************************** IMPLEMENTATION OF OIS::MouseListener ***********************/

bool Mouse::mouseMoved(const OIS::MouseEvent &arg)
{
    // Push the events in the list
    tListenersList::iterator listenersIter, listenersIterEnd;
    tInputEvent event;

    event.pController = this;
    event.part        = PART_AXIS;

    if (arg.state.X.rel != 0)
    {
        event.partID.axis  = AXIS_X;
        event.value.iValue = arg.state.X.rel;

        for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end();
             listenersIter != listenersIterEnd; ++listenersIter)
        {
            (*listenersIter)->onEvent(&event);
        }
    }

    if (arg.state.Y.rel != 0)
    {
        event.partID.axis  = AXIS_Y;
        event.value.iValue = arg.state.Y.rel;

        for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end();
             listenersIter != listenersIterEnd; ++listenersIter)
        {
            (*listenersIter)->onEvent(&event);
        }
    }

    if (arg.state.Z.rel != 0)
    {
        event.partID.axis  = AXIS_Z;
        event.value.iValue = arg.state.Z.rel;

        for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end();
             listenersIter != listenersIterEnd; ++listenersIter)
        {
            (*listenersIter)->onEvent(&event);
        }
    }

    return true;
}

//-----------------------------------------------------------------------

bool Mouse::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    // Push the event in the list
    tListenersList::iterator listenersIter, listenersIterEnd;
    tInputEvent event;

    event.pController       = this;
    event.part              = PART_KEY;
    event.partID.key        = id;
    event.value.bPressed    = true;

    for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end();
         listenersIter != listenersIterEnd; ++listenersIter)
    {
        (*listenersIter)->onEvent(&event);
    }

    return true;
}

//-----------------------------------------------------------------------

bool Mouse::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    // Push the event in the list
    tListenersList::iterator listenersIter, listenersIterEnd;
    tInputEvent event;

    event.pController       = this;
    event.part              = PART_KEY;
    event.partID.key        = id;
    event.value.bPressed    = false;

    for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end();
         listenersIter != listenersIterEnd; ++listenersIter)
    {
        (*listenersIter)->onEvent(&event);
    }

    return true;
}
