/** @file   Controller.cpp
    @author Philip Abbet

    Implementation of the class 'Athena::Inputs::Controller'
*/

#include <Athena-Inputs/Controller.h>
#include <Athena-Core/Utils/StringConverter.h>
#include <OIS/OISInputManager.h>


using namespace Athena;
using namespace Athena::Inputs;
using namespace Athena::Utils;
using namespace std;


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

Controller::Controller(OIS::Object* pOISObject, unsigned int uiIndex)
: m_pOISObject(pOISObject), m_uiIndex(uiIndex)
{
}

//-----------------------------------------------------------------------

Controller::~Controller()
{
    m_pOISObject->getCreator()->destroyInputObject(m_pOISObject);
}


/**************************** MANAGEMENT OF THE CONTROLLER *****************************/

const string Controller::toString() const
{
    switch (m_pOISObject->type())
    {
    case OIS::OISKeyboard:
        return "Keyboard";

    case OIS::OISMouse:
        return  "Mouse";

    case OIS::OISJoyStick:
        return "Gamepad #" + StringConverter::toString(m_uiIndex);

    default:
        return "Unknown #" + StringConverter::toString(m_uiIndex);
    }
}

//-----------------------------------------------------------------------

// const string Controller::getNameOfKey(tKey key)
// {
//     // Declarations
//     tKeyNamesList::iterator iter;
//
//     // Search if the name is already in the map
//     iter = m_strKeys.find(key);
//     if (iter != m_strKeys.end())
//         return iter->second;
//
//     return "";
// }
//
// //-----------------------------------------------------------------------
//
// const string Controller::getNameOfAxis(tAxis axis)
// {
//     // Declarations
//     tAxisNamesList::iterator iter;
//
//     // Search if the name is already in the map
//     iter = m_strAxes.find(axis);
//     if (iter != m_strAxes.end()) return iter->second;
//
//     return "";
// }
//
// //-----------------------------------------------------------------------
//
// const string Controller::getNameOfPOV(tPOV pov)
// {
//     // Declarations
//     tPOVNamesList::iterator iter;
//
//     // Search if the name is already in the map
//     iter = m_strPOVs.find(pov);
//     if (iter != m_strPOVs.end()) return iter->second;
//
//     return "";
// }

//-----------------------------------------------------------------------

void Controller::registerListener(IEventsListener* pListener)
{
    m_listeners.push_back(pListener);
}

//-----------------------------------------------------------------------

void Controller::removeListener(IEventsListener* pListener)
{
    // Declarations
    tListenersList::iterator iter, iterEnd;

    for (iter = m_listeners.begin(), iterEnd = m_listeners.end(); iter != iterEnd; ++iter)
    {
        if (*iter == pListener)
        {
            m_listeners.erase(iter);
            break;
        }
    }
}
