/**	@file	InputsUnit.cpp
	@author	Philip Abbet

	Implementation of the class 'Athena::Inputs::InputsUnit'
*/

#include <Athena-Inputs/InputsUnit.h>
#include <Athena-Inputs/IControllerManager.h>
#include <Athena-Core/Log/LogManager.h>
// #include <tinyxml.h>
#include <sstream>

#if ATHENA_PLATFORM == ATHENA_PLATFORM_WIN32
    #include <Athena-Inputs/win32/DI8ControllerManager.h>
#elif ATHENA_PLATFORM == ATHENA_PLATFORM_APPLE
    #include <Athena-Inputs/osx/MacControllerManager.h>
#endif

using namespace Athena;
using namespace Athena::Inputs;
using namespace Athena::Log;
using namespace std;


/************************************** CONSTANTS **************************************/

/// Context used for logging
static const char* __CONTEXT__ = "Inputs unit";


/********************************** STATIC ATTRIBUTES **********************************/

// The instance of the singleton
template<> InputsUnit* Utils::Singleton<InputsUnit>::ms_Singleton = 0;


/****************************** CONSTRUCTION / DESTRUCTION *****************************/

InputsUnit::InputsUnit()
: m_pControllerManager(0), m_uiNbGamepads(0)
{
	ATHENA_LOG_EVENT("Creation");

#if ATHENA_PLATFORM == ATHENA_PLATFORM_WIN32
    m_pControllerManager = new DI8ControllerManager();
#elif ATHENA_PLATFORM == ATHENA_PLATFORM_APPLE
    m_pControllerManager = new MacControllerManager();
#else
    #error Unsupported platform
#endif
}

//-----------------------------------------------------------------------

InputsUnit::~InputsUnit()
{
	ATHENA_LOG_EVENT("Destruction");

	ATHENA_LOG_EVENT("Destruction of the virtual controllers");

	// Destroy the virtual controllers
	while (!m_virtualControllers.empty())
	{
		destroyVirtualController(m_virtualControllers.begin()->first);
	}

	ATHENA_LOG_EVENT("Destruction of the controllers");

	// Destroy the controllers
	while (!m_controllers.empty())
	{
		_removeController(m_controllers.front());
	}

	// Destroy the virtual IDs
	m_virtualIDs.clear();

	// Destroy the shortcuts
	m_shortcuts.clear();
	
	// Destroy the controller manager
    delete m_pControllerManager;
}

//-----------------------------------------------------------------------

InputsUnit* InputsUnit::getSingletonPtr()
{
    return ms_Singleton;
}

//-----------------------------------------------------------------------

InputsUnit& InputsUnit::getSingleton()
{
	assert(ms_Singleton);
	return *ms_Singleton;
}


/********************************* MANAGEMENT OF THE UNIT ******************************/

bool InputsUnit::init(void* mainWindowHandle)
{
	assert(m_pControllerManager && "There isn't an instance of the Controllers manager");

	ATHENA_LOG_EVENT("Initialization");

	if (!m_pControllerManager->init(mainWindowHandle))
	{
		ATHENA_LOG_ERROR("Failed to initialize");
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------

void InputsUnit::process()
{
	// Declarations
	vector<Controller*>::iterator				iter, iterEnd;
	map<string, VirtualController*>::iterator	iter2, iterEnd2;
	Controller*								    pController;

	// Read the inputs of all the active controllers
	for (iter = m_controllers.begin(), iterEnd = m_controllers.end(); iter != iterEnd; ++iter)
	{
		pController = *iter;
		if (pController->isActive())
			pController->readInputs();
	}

	// Update the virtual controllers
	for (iter2 = m_virtualControllers.begin(), iterEnd2 = m_virtualControllers.end(); iter2 != iterEnd2; ++iter2)
	{
		iter2->second->process(m_events);
	}

	m_events.clear();
}

//-----------------------------------------------------------------------

void InputsUnit::scan(IEventsListener* pListener)
{
	// Declarations
	vector<Controller*>::iterator	iter, iterEnd;
	Controller*						pController;

	// Read the inputs of all the controllers
	for (iter = m_controllers.begin(), iterEnd = m_controllers.end(); iter != iterEnd; ++iter)
	{
		pController = *iter;

		pController->removeListener(this);
		pController->registerListener(pListener);
        pController->readInputs();
		pController->removeListener(pListener);
		pController->registerListener(this);
	}
}


/**************************** IMPLEMENTATION OF IEVENTSLISTENER ************************/

void InputsUnit::onEvent(tInputEvent* pEvent)
{
	// Push the event in the list
	m_events.push_back(*pEvent);
}


/****************************** MANAGEMENT OF THE CONTROLLERS **************************/

Controller* InputsUnit::getController(tControllerType type, unsigned int uiIndex)
{
	// Declarations
	vector<Controller*>::iterator	iter, iterEnd;
	Controller*					    pController;

	// Search the controller
	for (iter = m_controllers.begin(), iterEnd = m_controllers.end(); iter != iterEnd; ++iter)
	{
		pController = *iter;
		if ((pController->getType() == type) && (pController->getIndex() == uiIndex))
			return pController;
	}

	return 0;
}

//-----------------------------------------------------------------------

Controller* InputsUnit::getController(unsigned int uiIndex)
{
	assert(uiIndex > 0);
	assert(uiIndex <= m_controllers.size());

	return m_controllers[uiIndex - 1];
}

//-----------------------------------------------------------------------

void InputsUnit::_addController(Controller* pController)
{
	if (pController->getType() == CONTROLLER_GAMEPAD)
		++m_uiNbGamepads;

	ATHENA_LOG_EVENT("Adding the controller '" + pController->getName() + "'");

	m_controllers.push_back(pController);

	pController->registerListener(this);
}

//-----------------------------------------------------------------------

void InputsUnit::_removeController(Controller* pController)
{
	// Declarations
	std::vector<Controller*>::iterator iter, iterEnd;

	if (pController->getType() == CONTROLLER_GAMEPAD)
		--m_uiNbGamepads;

	// Search the controller
	for (iter = m_controllers.begin(), iterEnd = m_controllers.end();
		 iter != iterEnd; ++iter)
	{
		if (*iter == pController)
		{
			m_controllers.erase(iter);
			break;
		}
	}

	delete pController;
}


/************************** MANAGEMENT OF THE VIRTUAL CONTROLLERS **********************/

VirtualController* InputsUnit::createVirtualController(const std::string& strName)
{
	// Declarations
	VirtualController* pController;

	// Check that the name doesn't already exists
	if (m_virtualControllers.find(strName) != m_virtualControllers.end())
	{
		ATHENA_LOG_ERROR("Can't create a virtual controller, the name " + strName + " already exists");
		return 0;
	}

	pController = new VirtualController();

	m_virtualControllers[strName] = pController;

	return pController;
}

//-----------------------------------------------------------------------

VirtualController* InputsUnit::getVirtualController(const std::string& strName)
{
	// Declarations
	map<string, VirtualController*>::iterator iter;

	iter = m_virtualControllers.find(strName);
	if (iter != m_virtualControllers.end())
		return iter->second;
	else
		return 0;
}

//-----------------------------------------------------------------------

const std::string InputsUnit::getVirtualControllerName(VirtualController* pVirtualController)
{
	// Declarations
	map<string, VirtualController*>::iterator iter, iterEnd;

	for (iter = m_virtualControllers.begin(), iterEnd = m_virtualControllers.end();
		 iter != iterEnd; ++iter)
	{
		if (iter->second == pVirtualController)
			return iter->first;
	}

	return "";
}

//-----------------------------------------------------------------------

void InputsUnit::destroyVirtualController(const std::string& strName)
{
	// Declarations
	map<string, VirtualController*>::iterator iter, iterEnd;

	for (iter = m_virtualControllers.begin(), iterEnd = m_virtualControllers.end();
		 iter != iterEnd; ++iter)
	{
		if (iter->first == strName)
		{
			delete iter->second;
			m_virtualControllers.erase(iter);
			break;
		}
	}
}

//-----------------------------------------------------------------------

void InputsUnit::destroyVirtualController(VirtualController* pVirtualController)
{
	// Declarations
	map<string, VirtualController*>::iterator iter, iterEnd;

	for (iter = m_virtualControllers.begin(), iterEnd = m_virtualControllers.end();
		 iter != iterEnd; ++iter)
	{
		if (iter->second == pVirtualController)
		{
			delete pVirtualController;
			m_virtualControllers.erase(iter);
			break;
		}
	}
}

//-----------------------------------------------------------------------

bool InputsUnit::registerVirtualID(const std::string& strName, tVirtualID virtualID)
{
	// Declarations
	map<std::string, tVirtualID>::iterator	iter, iterEnd;
	stringstream str;

	// Check that the name doesn't already exists
	if (m_virtualIDs.find(strName) != m_virtualIDs.end())
	{
		if (virtualID != 0)
		{
			ATHENA_LOG_ERROR("Can't create a virtual ID, the name '" + strName + "' already exists");
			return false;
		}

		return true;
	}

	if (virtualID != 0)
	{
		// Check that the provided ID is free
		for (iter = m_virtualIDs.begin(), iterEnd = m_virtualIDs.end(); iter != iterEnd; ++iter)
		{
			if (iter->second == virtualID)
			{
				str << "Can't create a virtual ID, the supplied ID "
					<< virtualID << " already exists";
				ATHENA_LOG_ERROR(str.str());
				return false;
			}
		}
	}
	else
	{
		// Search the next free ID
		virtualID = 1;
		for (iter = m_virtualIDs.begin(), iterEnd = m_virtualIDs.end(); iter != iterEnd; ++iter)
		{
			if (iter->second >= virtualID)
				virtualID = (tVirtualID) iter->second + 1;
		}
	}

	m_virtualIDs[strName] = virtualID;

	str << "Virtual ID '" << strName << "' registered with the value " << virtualID;
	ATHENA_LOG_EVENT(str.str());

	return true;
}

//-----------------------------------------------------------------------

tVirtualID InputsUnit::getVirtualID(const std::string& strName)
{
	// Declarations
	map<string, tVirtualID>::iterator iter;

	iter = m_virtualIDs.find(strName);
	if (iter != m_virtualIDs.end())
		return (tVirtualID) iter->second;
	else
		return 0;
}

//-----------------------------------------------------------------------

const std::string InputsUnit::getVirtualName(tVirtualID virtualID)
{
	// Declarations
	map<string, tVirtualID>::iterator iter, iterEnd;

	for (iter = m_virtualIDs.begin(), iterEnd = m_virtualIDs.end(); iter != iterEnd; ++iter)
	{
		if (iter->second == virtualID)
			return iter->first;
	}

	return "";
}

//-----------------------------------------------------------------------

bool InputsUnit::registerShortcut(const std::string& strShortcut, tVirtualID virtualID)
{
	// Declarations
	map<std::string, tVirtualID>::iterator	iter, iterEnd;
	stringstream str;

	// Check that the name doesn't already exists
	if (m_shortcuts.find(strShortcut) != m_shortcuts.end())
	{
		ATHENA_LOG_ERROR("Can't register a shortcut, the name '" + strShortcut + "' already exists");
		return false;
	}

	m_shortcuts[strShortcut] = virtualID;

	str << "Shortcut '" << strShortcut << "' registered for the virtual ID " << virtualID;
	ATHENA_LOG_EVENT(str.str());

	return true;
}

//-----------------------------------------------------------------------

tVirtualID InputsUnit::getVirtualIDFromShortcut(const std::string& strShortcut)
{
	// Declarations
	map<string, tVirtualID>::iterator iter;

	iter = m_shortcuts.find(strShortcut);
	if (iter != m_shortcuts.end())
		return (tVirtualID) iter->second;
	else
		return 0;
}

//-----------------------------------------------------------------------

const string InputsUnit::getShortcutFromVirtualID(tVirtualID virtualID)
{
	// Declarations
	map<string, tVirtualID>::iterator iter, iterEnd;

	for (iter = m_shortcuts.begin(), iterEnd = m_shortcuts.end(); iter != iterEnd; ++iter)
	{
		if (iter->second == virtualID)
			return iter->first;
	}

	return "";
}

//-----------------------------------------------------------------------

// bool InputsUnit::loadVirtualControllers(const std::string& strFile)
// {
//  // Declarations
//  String              strFileFullName;
//  TiXmlDocument*      pXMLDocument;
//  TiXmlElement*       pXMLRoot;
//  TiXmlElement*       pXMLGamepad;
//  TiXmlElement*       pXMLVirtualController;
//  TiXmlElement*       pXMLVirtualPartsList;
//  TiXmlElement*       pXMLVirtualPart;
//  TiXmlElement*       pXMLController;
//  TiXmlElement*       pXMLShortcuts;
//  TiXmlElement*       pXMLKey;
//  VirtualController*  pVirtualController;
//  map<int, Controller*>   gamepads;
// 
//  ATHENA_LOG_EVENT("Loading the virtual controllers from the file '" + strFile + "'");
// 
//  // Parse the file that contains the definition of the virtual controllers
//  FileInfoListPtr list = ResourceGroupManager::getSingleton().findResourceFileInfo(strResourceGroup, strFile);
//  if (!list->empty())
//      strFileFullName = list->front().archive->getName() + ATHENA_PATH_SEPARATOR + strFile;
//  else
//      strFileFullName = strFile;
//  pXMLDocument = new TiXmlDocument(strFileFullName.c_str());
//  pXMLDocument->LoadFile();
// 
//  // Retrieve the root element
//  pXMLRoot = pXMLDocument->RootElement();
//  if (!pXMLRoot)
//  {
//      delete pXMLDocument;
//      return false;
//  }
// 
//  // Build the list of gamepads
//  if (pXMLRoot->FirstChildElement("gamepads"))
//  {
//      for (pXMLGamepad = pXMLRoot->FirstChildElement("gamepads")->FirstChildElement("gamepad");
//           pXMLGamepad; pXMLGamepad = pXMLGamepad->NextSiblingElement("gamepad"))
//      {
//             const char* strName = pXMLGamepad->Attribute("name");
//          int iNo;
//          int iIndex;
//          std::vector<Controller*>::iterator iter, iterEnd;
// 
//          pXMLGamepad->QueryIntAttribute("no", &iNo);
//          if (pXMLGamepad->QueryIntAttribute("index", &iIndex) == 0)
//              iIndex = 1;
// 
//          gamepads[iNo] = 0;
// 
//          int counter = 0;
//          for (iter = m_controllers.begin(), iterEnd = m_controllers.end(); iter != iterEnd; ++iter)
//          {
//              if ((*iter)->getName() == strName)
//              {
//                  ++counter;
//                  if (counter == iIndex)
//                  {
//                      gamepads[iNo] = *iter;
//                      break;
//                  }
//              }
//          }
//      }
//  }
// 
//  // For each virtual controller defined in the file
//  for (pXMLVirtualController = pXMLRoot->FirstChildElement("virtualController");
//       pXMLVirtualController;
//       pXMLVirtualController = pXMLVirtualController->NextSiblingElement("virtualController"))
//  {
//      pVirtualController = createVirtualController(pXMLVirtualController->Attribute("name"));
//      if (!pVirtualController)
//          continue;
// 
//      // Parse the list of virtual keys
//      pXMLVirtualPartsList = pXMLVirtualController->FirstChildElement("virtualKeys");
//      if (pXMLVirtualPartsList)
//      {
//          for (pXMLVirtualPart = pXMLVirtualPartsList->FirstChildElement("virtualKey");
//              pXMLVirtualPart;
//              pXMLVirtualPart = pXMLVirtualPart->NextSiblingElement("virtualKey"))
//          {
//              tVirtualID      virtualKey;
//              Controller* pController = 0;
//              int             iNo = 1;
//              tKey            key;
// 
//              if (!registerVirtualID(pXMLVirtualPart->Attribute("name")))
//                  continue;
// 
//              virtualKey = getVirtualID(pXMLVirtualPart->Attribute("name"));
//              const char* strShortcut = pXMLVirtualPart->Attribute("shortcut");
// 
//              pXMLController = pXMLVirtualPart->FirstChildElement("controller");
//              if (!pXMLController)
//              {
//                  pVirtualController->registerVirtualKey(virtualKey, (strShortcut ? strShortcut : ""));
//                  continue;
//              }
// 
//              if (strcmp(pXMLController->Attribute("type"), "KEYBOARD") == 0)
//              {
//                  pController = getController(CONTROLLER_KEYBOARD);
//              }
//              else if (strcmp(pXMLController->Attribute("type"), "MOUSE") == 0)
//              {
//                  pController = getController(CONTROLLER_MOUSE);
//              }
//              else if (strcmp(pXMLController->Attribute("type"), "GAMEPAD") == 0)
//              {
//                  if (pXMLController->Attribute("no", &iNo) == 0)
//                      continue;
//                  pController = gamepads[iNo];
//              }
//              
//              if (!pController)
//                  continue;
//              
//              if (!pController->isActive()) pController->activate(true);
// 
//              pXMLKey = pXMLVirtualPart->FirstChildElement("key");
//              if (!pXMLKey)
//                  continue;
// 
//              key = (tKey) StringConverter::parseUnsignedInt(pXMLKey->FirstChild()->Value());
// 
//              pVirtualController->addVirtualKey(virtualKey, pController, key, (strShortcut ? strShortcut : ""));
//          }
//      }
// 
//      // Parse the list of virtual POVs
//      pXMLVirtualPartsList = pXMLVirtualController->FirstChildElement("virtualPOVs");
//      if (pXMLVirtualPartsList)
//      {
//          for (pXMLVirtualPart = pXMLVirtualPartsList->FirstChildElement("virtualPOV");
//              pXMLVirtualPart;
//              pXMLVirtualPart = pXMLVirtualPart->NextSiblingElement("virtualPOV"))
//          {
//              tVirtualID      virtualPOV;
//              Controller* pController = 0;
//              int             iNo = 1;
//              tPOV            pov;
//              tKey            upKey;
//              tKey            downKey;
//              tKey            leftKey;
//              tKey            rightKey;
//              tAxis           vertAxis;
//              tAxis           horAxis;
//              String          strShortcutUp;
//              String          strShortcutDown;    
//              String          strShortcutLeft;
//              String          strShortcutRight;
// 
//              if (!registerVirtualID(pXMLVirtualPart->Attribute("name")))
//                  continue;
// 
//              virtualPOV = getVirtualID(pXMLVirtualPart->Attribute("name"));
// 
//              pXMLShortcuts = pXMLVirtualPart->FirstChildElement("shortcuts");
//              if (pXMLShortcuts)
//              {
//                  strShortcutUp       = pXMLShortcuts->Attribute("up");
//                  strShortcutDown     = pXMLShortcuts->Attribute("down");
//                  strShortcutLeft     = pXMLShortcuts->Attribute("left");
//                  strShortcutRight    = pXMLShortcuts->Attribute("right");
//              }
// 
//              pXMLController = pXMLVirtualPart->FirstChildElement("controller");
//              if (!pXMLController)
//              {
//                  if (pXMLShortcuts)
//                  {
//                      pVirtualController->registerVirtualPOV(virtualPOV, strShortcutUp, strShortcutDown,
//                                                             strShortcutLeft, strShortcutRight);
//                  }
//                  else
//                  {
//                      pVirtualController->registerVirtualPOV(virtualPOV);
//                  }
//                  continue;
//              }
//              
//              pXMLKey = pXMLVirtualPart->FirstChildElement("pov");
//              if (pXMLKey)
//              {
//                  pov = (tPOV) StringConverter::parseUnsignedInt(pXMLKey->FirstChild()->Value());
// 
//                  if (strcmp(pXMLController->Attribute("type"), "GAMEPAD") == 0)
//                  {
//                      if (pXMLController->Attribute("no", &iNo) == 0)
//                          continue;
//                      pController = gamepads[iNo];
//                  }
//                  else
//                  {
//                      ATHENA_LOG_ERROR("You can't define a POV like that on something else than a gamepad!");
//                      continue;
//                  }
//                  
//                  if (!pController)
//                      continue;
//                  
//                  if (!pController->isActive()) pController->activate(true);
// 
//                  pVirtualController->addVirtualPOV(virtualPOV, pController, pov, strShortcutUp,
//                                                    strShortcutDown, strShortcutLeft, strShortcutRight);
//              }   
//              else
//              {
//                  pXMLKey = pXMLVirtualPart->FirstChildElement("upKey");
//                  if (pXMLKey)
//                  {
//                      upKey = (tKey) StringConverter::parseUnsignedInt(pXMLKey->FirstChild()->Value());
// 
//                      pXMLKey = pXMLVirtualPart->FirstChildElement("downKey");
//                      if (!pXMLKey)
//                          continue;
//                      downKey = (tKey) StringConverter::parseUnsignedInt(pXMLKey->FirstChild()->Value());
// 
//                      pXMLKey = pXMLVirtualPart->FirstChildElement("leftKey");
//                      if (!pXMLKey)
//                          continue;
//                      leftKey = (tKey) StringConverter::parseUnsignedInt(pXMLKey->FirstChild()->Value());
// 
//                      pXMLKey = pXMLVirtualPart->FirstChildElement("rightKey");
//                      if (!pXMLKey)
//                          continue;
//                      rightKey = (tKey) StringConverter::parseUnsignedInt(pXMLKey->FirstChild()->Value());
// 
//                      if (strcmp(pXMLController->Attribute("type"), "KEYBOARD") == 0)
//                      {
//                          pController = getController(CONTROLLER_KEYBOARD);
//                      }
//                      else if (strcmp(pXMLController->Attribute("type"), "MOUSE") == 0)
//                      {
//                          pController = getController(CONTROLLER_MOUSE);
//                      }
//                      else if (strcmp(pXMLController->Attribute("type"), "GAMEPAD") == 0)
//                      {
//                          if (pXMLController->Attribute("no", &iNo) == 0)
//                              continue;
//                          pController = gamepads[iNo];
//                      }
//                      
//                      if (!pController)
//                          continue;
//                      
//                      if (!pController->isActive()) pController->activate(true);
// 
//                      pVirtualController->addVirtualPOV(virtualPOV, pController, upKey, downKey, leftKey, rightKey,
//                                                      strShortcutUp, strShortcutDown, strShortcutLeft, strShortcutRight);
//                  }
//                  else
//                  {
//                      pXMLKey = pXMLVirtualPart->FirstChildElement("vertAxis");
//                      if (!pXMLKey)
//                          continue;
//                      vertAxis = (tAxis) StringConverter::parseUnsignedInt(pXMLKey->FirstChild()->Value());
// 
//                      pXMLKey = pXMLVirtualPart->FirstChildElement("horAxis");
//                      if (!pXMLKey)
//                          continue;
//                      horAxis = (tAxis) StringConverter::parseUnsignedInt(pXMLKey->FirstChild()->Value());
// 
//                      if (strcmp(pXMLController->Attribute("type"), "GAMEPAD") == 0)
//                      {
//                          if (pXMLController->Attribute("no", &iNo) == 0)
//                              continue;
//                          pController = gamepads[iNo];
//                      }
//                      else
//                      {
//                          ATHENA_LOG_ERROR("You can't define a POV like that on something else than a gamepad!");
//                          continue;
//                      }
//                      
//                      if (!pController)
//                          continue;
//                      
//                      if (!pController->isActive()) pController->activate(true);
// 
//                      pVirtualController->addVirtualPOV(virtualPOV, pController, vertAxis, horAxis,
//                                                        strShortcutUp, strShortcutDown, strShortcutLeft,
//                                                        strShortcutRight);
//                  }
//              }
//          }
//      }
// 
//      // Parse the list of virtual axes
//      pXMLVirtualPartsList = pXMLVirtualController->FirstChildElement("virtualAxes");
//      if (pXMLVirtualPartsList)
//      {
//          for (pXMLVirtualPart = pXMLVirtualPartsList->FirstChildElement("virtualAxis");
//              pXMLVirtualPart;
//              pXMLVirtualPart = pXMLVirtualPart->NextSiblingElement("virtualAxis"))
//          {
//              tVirtualID      virtualAxis;
//              Controller* pController = 0;
//              int             iNo = 1;
//              tAxis           axis;
//              tPOV            pov;
//              bool            bUpDown;
//              tKey            minKey;
//              tKey            maxKey;
// 
//              if (!registerVirtualID(pXMLVirtualPart->Attribute("name")))
//                  continue;
// 
//              virtualAxis = getVirtualID(pXMLVirtualPart->Attribute("name"));
// 
//              pXMLController = pXMLVirtualPart->FirstChildElement("controller");
//              if (!pXMLController)
//              {
//                  pVirtualController->registerVirtualAxis(virtualAxis);
//                  continue;
//              }
//              
//              pXMLKey = pXMLVirtualPart->FirstChildElement("axis");
//              if (pXMLKey)
//              {
//                  axis = (tAxis) StringConverter::parseUnsignedInt(pXMLKey->FirstChild()->Value());
// 
//                  if (strcmp(pXMLController->Attribute("type"), "GAMEPAD") == 0)
//                  {
//                      if (pXMLController->Attribute("no", &iNo) == 0)
//                          continue;
//                      pController = gamepads[iNo];
//                  }
//                  else if (strcmp(pXMLController->Attribute("type"), "MOUSE") == 0)
//                  {
//                      pController = getController(CONTROLLER_MOUSE);
//                  }
//                  else
//                  {
//                      ATHENA_LOG_ERROR("You can't define an axis like that on something else than a gamepad or a mouse!");
//                      continue;
//                  }
//                  
//                  if (!pController)
//                      continue;
//                  
//                  if (!pController->isActive()) pController->activate(true);
// 
//                  pVirtualController->addVirtualAxis(virtualAxis, pController, axis);
//              }   
//              else
//              {
//                  pXMLKey = pXMLVirtualPart->FirstChildElement("minKey");
//                  if (pXMLKey)
//                  {
//                      minKey = (tKey) StringConverter::parseUnsignedInt(pXMLKey->FirstChild()->Value());
// 
//                      pXMLKey = pXMLVirtualPart->FirstChildElement("maxKey");
//                      if (!pXMLKey)
//                          continue;
//                      maxKey = (tKey) StringConverter::parseUnsignedInt(pXMLKey->FirstChild()->Value());
// 
//                      if (strcmp(pXMLController->Attribute("type"), "KEYBOARD") == 0)
//                      {
//                          pController = getController(CONTROLLER_KEYBOARD);
//                      }
//                      else if (strcmp(pXMLController->Attribute("type"), "MOUSE") == 0)
//                      {
//                          pController = getController(CONTROLLER_MOUSE);
//                      }
//                      else if (strcmp(pXMLController->Attribute("type"), "GAMEPAD") == 0)
//                      {
//                          if (pXMLController->Attribute("no", &iNo) == 0)
//                              continue;
//                          pController = gamepads[iNo];
//                      }
//                      
//                      if (!pController)
//                          continue;
//                      
//                      if (!pController->isActive()) pController->activate(true);
// 
//                      pVirtualController->addVirtualAxis(virtualAxis, pController, minKey, maxKey);
//                  }
//                  else
//                  {
//                      pXMLKey = pXMLVirtualPart->FirstChildElement("pov");
//                      if (!pXMLKey)
//                          continue;
//                      pov = (tPOV) StringConverter::parseUnsignedInt(pXMLKey->FirstChild()->Value());
// 
//                      bUpDown = (strcmp(pXMLKey->Attribute("upDown"), "true") == 0);
// 
//                      if (strcmp(pXMLController->Attribute("type"), "GAMEPAD") == 0)
//                      {
//                          if (pXMLController->Attribute("no", &iNo) == 0)
//                              continue;
//                          pController = gamepads[iNo];
//                      }
//                      else
//                      {
//                          ATHENA_LOG_ERROR("You can't define an axis like that on something else than a gamepad!");
//                          continue;
//                      }
//                      
//                      if (!pController)
//                          continue;
//                      
//                      if (!pController->isActive()) pController->activate(true);
// 
//                      pVirtualController->addVirtualAxis(virtualAxis, pController, pov, bUpDown);
//                  }
//              }
//          }
//      }
//  }
// 
//  delete pXMLDocument;
// 
//  return true;
// }

//-----------------------------------------------------------------------

// bool InputsUnit::saveVirtualControllers(const std::string& strFile)
// {
//  // Assertions
//  assert(!strFile.empty());
// 
//  // Declarations
//  String              strFullPath;
//  TiXmlDocument*      pXMLDocument;
//  TiXmlElement*       pXMLRoot;
//  TiXmlElement*       pXMLGamepads;
//  TiXmlElement*       pXMLVirtualController;
//  TiXmlElement*       pXMLVirtualPartsList;
//  TiXmlElement*       pXMLVirtualPart;
//  TiXmlElement*       pXMLController;
//  TiXmlElement*       pXMLShortcuts;
//  TiXmlElement*       pXMLKey;
//  TiXmlElement*       pXMLPOV;
//  TiXmlElement*       pXMLAxis;
//  map<String, VirtualController*>::iterator contrIter, contrIterEnd;
//  map<int, Controller*>   gamepads;
//  Controller*     pController;
//  int                 iNo = 1;
// 
//  // Retrieve the full path of the file
//  if ((strFile.substr(1, 2) == ":\\") || (strFile.substr(1, 2) == ":/"))
//  {
//      strFullPath = strFile;
//  }
//  else
//  {
//      FileInfoListPtr list = ResourceGroupManager::getSingleton().findResourceFileInfo(
//          ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, strFile);
//      
//      if (!list->empty())
//          strFullPath = list->front().archive->getName() + ATHENA_PATH_SEPARATOR + strFile;
//      else
//          strFullPath = strFile;
//  }
// 
//  ATHENA_LOG_EVENT("Saving the virtual controllers to the file '" + strFullPath + "'");
// 
//  // Create the xml document
//  pXMLDocument = new TiXmlDocument(strFullPath.c_str());
//  pXMLDocument->InsertEndChild(TiXmlDeclaration("1.0", "utf-8", ""));
// 
//  pXMLRoot = (TiXmlElement*) pXMLDocument->InsertEndChild(TiXmlElement("virtualControllers"));
// 
//  // Build the list of gamepads
//  pXMLGamepads = (TiXmlElement*) pXMLRoot->InsertEndChild(TiXmlElement("gamepads"));
//  while (pController = this->getController(CONTROLLER_GAMEPAD, iNo))
//  {
//      int iIndex = 1;
//      map<int, Controller*>::iterator iter, iterEnd;
// 
//      for (iter = gamepads.begin(), iterEnd = gamepads.end(); iter != iterEnd; ++iter)
//      {
//          if (iter->second->getName() == pController->getName())
//              ++iIndex;
//      }
// 
//      TiXmlElement* pXMLGamepad = (TiXmlElement*) pXMLGamepads->InsertEndChild(TiXmlElement("gamepad"));
//      pXMLGamepad->SetAttribute("no", iNo);
//      pXMLGamepad->SetAttribute("name", pController->getName().c_str());
//      pXMLGamepad->SetAttribute("index", iIndex);
// 
//      gamepads[iNo] = pController;
// 
//      ++iNo;
//  }
// 
//  // Add the virtual controllers
//  for (contrIter = m_virtualControllers.begin(), contrIterEnd = m_virtualControllers.end();
//       contrIter != contrIterEnd; ++contrIter)
//  {
//      VirtualController*  pVirtualController = contrIter->second;
//      pXMLVirtualController = (TiXmlElement*) pXMLRoot->InsertEndChild(TiXmlElement("virtualController"));
//      pXMLVirtualController->SetAttribute("name", getVirtualControllerName(pVirtualController).c_str());
// 
//      // Add the virtual keys
//      pXMLVirtualPartsList = (TiXmlElement*) pXMLVirtualController->InsertEndChild(TiXmlElement("virtualKeys"));
//      for (unsigned int i = 0; i < pVirtualController->getNbVirtualKeys(); ++i)
//      {
//          tVirtualID id;
//          tVirtualKey key = pVirtualController->getVirtualKey(i, id);
// 
//          pXMLVirtualPart = (TiXmlElement*) pXMLVirtualPartsList->InsertEndChild(TiXmlElement("virtualKey"));
//          pXMLVirtualPart->SetAttribute("name", getVirtualName(id).c_str());
// 
//          if (key.bHasShortcut)
//              pXMLVirtualPart->SetAttribute("shortcut", getShortcutFromVirtualID(id).c_str());
// 
//          if (!key.pController)
//              continue;
// 
//          pXMLController = (TiXmlElement*) pXMLVirtualPart->InsertEndChild(TiXmlElement("controller"));
//          switch (key.pController->getType())
//          {
//          case CONTROLLER_KEYBOARD:
//              pXMLController->SetAttribute("type", "KEYBOARD");
//              break;
// 
//          case CONTROLLER_MOUSE:
//              pXMLController->SetAttribute("type", "MOUSE");
//              break;
// 
//          case CONTROLLER_GAMEPAD:
//              pXMLController->SetAttribute("type", "GAMEPAD");
//              break;
//          }
//          pXMLController->SetAttribute("no", key.pController->getIndex());
// 
//          pXMLKey = (TiXmlElement*) pXMLVirtualPart->InsertEndChild(TiXmlElement("key"));
//          pXMLKey->InsertEndChild(TiXmlText(StringConverter::toString(key.key).c_str()));
//      }
// 
//      // Add the virtual POVs
//      pXMLVirtualPartsList = (TiXmlElement*) pXMLVirtualController->InsertEndChild(TiXmlElement("virtualPOVs"));
//      for (unsigned int i = 0; i < pVirtualController->getNbVirtualPOVs(); ++i)
//      {
//          tVirtualID id;
//          tVirtualPOV pov = pVirtualController->getVirtualPOV(i, id);
// 
//          pXMLVirtualPart = (TiXmlElement*) pXMLVirtualPartsList->InsertEndChild(TiXmlElement("virtualPOV"));
//          pXMLVirtualPart->SetAttribute("name", getVirtualName(id).c_str());
// 
//          if (!pov.shortcuts.strShortcutUp.empty() || !pov.shortcuts.strShortcutDown.empty() ||
//              !pov.shortcuts.strShortcutLeft.empty() || !pov.shortcuts.strShortcutRight.empty())
//          {
//              pXMLShortcuts = (TiXmlElement*) pXMLVirtualPart->InsertEndChild(TiXmlElement("shortcuts"));
//              pXMLShortcuts->SetAttribute("up", pov.shortcuts.strShortcutUp.c_str());
//              pXMLShortcuts->SetAttribute("down", pov.shortcuts.strShortcutDown.c_str());
//              pXMLShortcuts->SetAttribute("left", pov.shortcuts.strShortcutLeft.c_str());
//              pXMLShortcuts->SetAttribute("right", pov.shortcuts.strShortcutRight.c_str());
//          }
// 
//          if (!pov.pController)
//              continue;
// 
//          pXMLController = (TiXmlElement*) pXMLVirtualPart->InsertEndChild(TiXmlElement("controller"));
//          switch (pov.pController->getType())
//          {
//          case CONTROLLER_KEYBOARD:
//              pXMLController->SetAttribute("type", "KEYBOARD");
//              break;
// 
//          case CONTROLLER_MOUSE:
//              pXMLController->SetAttribute("type", "MOUSE");
//              break;
// 
//          case CONTROLLER_GAMEPAD:
//              pXMLController->SetAttribute("type", "GAMEPAD");
//              break;
//          }
//          pXMLController->SetAttribute("no", pov.pController->getIndex());
// 
//          switch (pov.part)
//          {
//          case PART_POV:
//              pXMLPOV = (TiXmlElement*) pXMLVirtualPart->InsertEndChild(TiXmlElement("pov"));
//              pXMLPOV->InsertEndChild(TiXmlText(StringConverter::toString(pov.realPart.pov).c_str()));
//              break;
// 
//          case PART_AXIS:
//              pXMLAxis = (TiXmlElement*) pXMLVirtualPart->InsertEndChild(TiXmlElement("vertAxis"));
//              pXMLAxis->InsertEndChild(TiXmlText(StringConverter::toString(pov.realPart.axes.axisUpDown).c_str()));
//              pXMLAxis = (TiXmlElement*) pXMLVirtualPart->InsertEndChild(TiXmlElement("horAxis"));
//              pXMLAxis->InsertEndChild(TiXmlText(StringConverter::toString(pov.realPart.axes.axisLeftRight).c_str()));
//              break;
// 
//          case PART_KEY:
//              pXMLKey = (TiXmlElement*) pXMLVirtualPart->InsertEndChild(TiXmlElement("upKey"));
//              pXMLKey->InsertEndChild(TiXmlText(StringConverter::toString(pov.realPart.keys.keyUp).c_str()));
//              pXMLKey = (TiXmlElement*) pXMLVirtualPart->InsertEndChild(TiXmlElement("downKey"));
//              pXMLKey->InsertEndChild(TiXmlText(StringConverter::toString(pov.realPart.keys.keyDown).c_str()));
//              pXMLKey = (TiXmlElement*) pXMLVirtualPart->InsertEndChild(TiXmlElement("leftKey"));
//              pXMLKey->InsertEndChild(TiXmlText(StringConverter::toString(pov.realPart.keys.keyLeft).c_str()));
//              pXMLKey = (TiXmlElement*) pXMLVirtualPart->InsertEndChild(TiXmlElement("rightKey"));
//              pXMLKey->InsertEndChild(TiXmlText(StringConverter::toString(pov.realPart.keys.keyRight).c_str()));
//              break;
//          }
//      }
// 
//      // Add the virtual axes
//      pXMLVirtualPartsList = (TiXmlElement*) pXMLVirtualController->InsertEndChild(TiXmlElement("virtualAxes"));
//      for (unsigned int i = 0; i < pVirtualController->getNbVirtualAxes(); ++i)
//      {
//          tVirtualID id;
//          tVirtualAxis axis = pVirtualController->getVirtualAxis(i, id);
// 
//          pXMLVirtualPart = (TiXmlElement*) pXMLVirtualPartsList->InsertEndChild(TiXmlElement("virtualAxis"));
//          pXMLVirtualPart->SetAttribute("name", getVirtualName(id).c_str());
// 
//          if (!axis.pController)
//              continue;
// 
//          pXMLController = (TiXmlElement*) pXMLVirtualPart->InsertEndChild(TiXmlElement("controller"));
//          switch (axis.pController->getType())
//          {
//          case CONTROLLER_KEYBOARD:
//              pXMLController->SetAttribute("type", "KEYBOARD");
//              break;
// 
//          case CONTROLLER_MOUSE:
//              pXMLController->SetAttribute("type", "MOUSE");
//              break;
// 
//          case CONTROLLER_GAMEPAD:
//              pXMLController->SetAttribute("type", "GAMEPAD");
//              break;
//          }
//          pXMLController->SetAttribute("no", axis.pController->getIndex());
// 
//          switch (axis.part)
//          {
//          case PART_POV:
//              pXMLPOV = (TiXmlElement*) pXMLVirtualPart->InsertEndChild(TiXmlElement("pov"));
//              pXMLPOV->SetAttribute("upDown", (axis.realPart.pov.bUpDown ? "true" : "false"));
//              pXMLPOV->InsertEndChild(TiXmlText(StringConverter::toString(axis.realPart.pov.pov).c_str()));
//              break;
// 
//          case PART_AXIS:
//              pXMLAxis = (TiXmlElement*) pXMLVirtualPart->InsertEndChild(TiXmlElement("axis"));
//              pXMLAxis->InsertEndChild(TiXmlText(StringConverter::toString(axis.realPart.axis).c_str()));
//              break;
// 
//          case PART_KEY:
//              pXMLKey = (TiXmlElement*) pXMLVirtualPart->InsertEndChild(TiXmlElement("minKey"));
//              pXMLKey->InsertEndChild(TiXmlText(StringConverter::toString(axis.realPart.keys.keyMin).c_str()));
//              pXMLKey = (TiXmlElement*) pXMLVirtualPart->InsertEndChild(TiXmlElement("maxKey"));
//              pXMLKey->InsertEndChild(TiXmlText(StringConverter::toString(axis.realPart.keys.keyMax).c_str()));
//              break;
//          }
//      }
//  }
// 
//  // Save the file
//  if (pXMLDocument->SaveFile())
//  {
//      ATHENA_LOG_EVENT("File '" + strFullPath + "' successfully saved");
//      return true;
//  }
//  else
//  {
//      ATHENA_LOG_ERROR("Failed to save the file '" + strFullPath + "'");
//      return false;
//  }
// }
