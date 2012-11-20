/** @file   Keyboard.cpp
    @author Philip Abbet

    Implementation of the class 'Athena::Inputs::Keyboard'
*/

#include <Athena-Inputs/Keyboard.h>
#include <Athena-Inputs/IEventsListener.h>


using namespace Athena;
using namespace Athena::Inputs;
using namespace Athena::Utils;
using namespace std;


/****************************** CONSTRUCTION / DESTRUCTION ******************************/

Keyboard::Keyboard(OIS::Object* pOISObject)
: Controller(pOISObject, 1), m_bCharacterMode(false)
{
    assert(pOISObject->type() == OIS::OISKeyboard);

    static_cast<OIS::Keyboard*>(pOISObject)->setEventCallback(this);
    static_cast<OIS::Keyboard*>(pOISObject)->setTextTranslation(OIS::Keyboard::Unicode);
}

//-----------------------------------------------------------------------

Keyboard::~Keyboard()
{
}


/**************************** IMPLEMENTATION OF OIS::KeyListener ************************/

bool Keyboard::keyPressed(const OIS::KeyEvent &arg)
{
    tListenersList::iterator listenersIter, listenersIterEnd;

    if (m_bCharacterMode)
    {
        for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end();
             listenersIter != listenersIterEnd; ++listenersIter)
        {
            // (*listenersIter)->onCharacter(arg.text);
        }
    }
    else
    {
        // Push the event in the list
        tInputEvent event;

        event.pController       = this;
        event.part              = PART_KEY;
        event.partID.key        = arg.key;
        event.value.bPressed    = true;

        for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end();
             listenersIter != listenersIterEnd; ++listenersIter)
        {
            (*listenersIter)->onEvent(&event);
        }
    }

    return true;
}

//-----------------------------------------------------------------------

bool Keyboard::keyReleased(const OIS::KeyEvent &arg)
{
    if (!m_bCharacterMode)
    {
        // Push the event in the list
        tInputEvent event;

        event.pController       = this;
        event.part              = PART_KEY;
        event.partID.key        = arg.key;
        event.value.bPressed    = false;

        tListenersList::iterator listenersIter, listenersIterEnd;

        for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end();
             listenersIter != listenersIterEnd; ++listenersIter)
        {
            (*listenersIter)->onEvent(&event);
        }
    }

    return true;
}
