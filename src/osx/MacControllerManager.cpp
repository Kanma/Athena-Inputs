/**	@file	MacControllerManager.cpp
	@author	Philip Abbet

	Implementation of the class 'Athena::Inputs::MacControllerManager'
*/

#import <CoreFoundation/CoreFoundation.h>
#import <IOKit/IOKitLib.h>
#import <IOKit/IOCFPlugIn.h>
#import <IOKit/hid/IOHIDLib.h>
#import <IOKit/hid/IOHIDKeys.h>
#import <Kernel/IOKit/hidsystem/IOHIDUsageTables.h>

#include <Athena-Inputs/osx/MacControllerManager.h>
#include <Athena-Inputs/osx/MacMouse.h>
#include <Athena-Inputs/osx/MacKeyboard.h>
#include <Athena-Inputs/osx/MacGamepad.h>
#include <Athena-Inputs/InputsUnit.h>
#include <Athena-Core/Log/LogManager.h>


using namespace Athena;
using namespace Athena::Inputs;
using namespace Athena::Log;
using namespace std;


static const char* __CONTEXT__ = "MacInput plugin";


/****************************** CONSTRUCTION / DESTRUCTION *****************************/

MacControllerManager::MacControllerManager()
{
	ATHENA_LOG_EVENT("Creation of the Controllers' manager");
}

//-----------------------------------------------------------------------

MacControllerManager::~MacControllerManager()
{
	ATHENA_LOG_EVENT("Destruction of the Controllers' manager");
}


/************************* IMPLEMENTATION OF IControllerManager ************************/

bool MacControllerManager::init(void* mainWindowHandle)
{
	ATHENA_LOG_EVENT("Initialization of the Controller's manager");

	// Initialization of the controllers
	ATHENA_LOG_EVENT("Initialization of the controllers");
	initKeyboards();
	initMouses();
	initGamepads();

	return true;
}


/***************************** MANAGEMENT OF THE CONTROLLERS ***************************/

bool MacControllerManager::initKeyboards()
{
	// Assertions
	assert(InputsUnit::getSingletonPtr() && "There isn't an instance of the Inputs Unit");

	// Declarations
	MacKeyboard*	pKeyboard;
	io_object_t		hidDevice = 0;

	ATHENA_LOG_EVENT("Initialization of the keyboard");

	// Retrieves an enumerator over the keyboards
	io_iterator_t iterator = getDevicesIterator(kHIDUsage_GD_Keyboard);
	if (!iterator)
	{
		ATHENA_LOG_ERROR("Failed to retrieve the keyboard");
		return false;
	}
	
	// Create the keyboard
	if ((hidDevice = IOIteratorNext(iterator)) != 0)
	{
		pKeyboard = new MacKeyboard(hidDevice);
		if (pKeyboard->isInitialized())
		{
			// Add it to the Inputs' unit's list
			InputsUnit::getSingletonPtr()->_addController(pKeyboard);
			IOObjectRelease(iterator);
			return true;
		}
		
		delete pKeyboard;
	}		
	
	ATHENA_LOG_ERROR("Failed to initialize the keyboard");
	
	IOObjectRelease(iterator);
	
	return true;
}

//-----------------------------------------------------------------------

bool MacControllerManager::initMouses()
{
	// Assertions
	assert(InputsUnit::getSingletonPtr() && "There isn't an instance of the Inputs Unit");

	// Declarations
	MacMouse*	pMouse;
	io_object_t hidDevice = 0;

	ATHENA_LOG_EVENT("Initialization of the mouse");

	// Retrieves an enumerator over the mouses
	io_iterator_t iterator = getDevicesIterator(kHIDUsage_GD_Mouse);
	if (!iterator)
	{
		ATHENA_LOG_ERROR("Failed to retrieve the mouse");
		return false;
	}
	
	// Create the mouse
	if ((hidDevice = IOIteratorNext(iterator)) != 0)
	{
		pMouse = new MacMouse(hidDevice);
		if (pMouse->isInitialized())
		{
			// Add it to the Inputs' unit's list
			InputsUnit::getSingletonPtr()->_addController(pMouse);
			IOObjectRelease(iterator);
			return true;
		}

		delete pMouse;
	}		
	
	ATHENA_LOG_ERROR("Failed to initialize the mouse");

	IOObjectRelease(iterator);

	return false;
}

//-----------------------------------------------------------------------

bool MacControllerManager::initGamepads()
{
	// Assertions
	assert(InputsUnit::getSingletonPtr() && "There isn't an instance of the Inputs Unit");

	// Declarations
	MacGamepad*	pGamepad;
	io_object_t		hidDevice = 0;
	unsigned int	counter = 1;

	ATHENA_LOG_EVENT("Initialization of the gamepads");

	// Retrieves an enumerator over the gamepads
	io_iterator_t iterator = getDevicesIterator(kHIDUsage_GD_GamePad);
	if (!iterator)
	{
		ATHENA_LOG_ERROR("Failed to retrieve the gamepads");
		return false;
	}

	// Create the gamepads
	if ((hidDevice = IOIteratorNext(iterator)) != 0)
	{
		pGamepad = new MacGamepad(hidDevice, counter);
		if (pGamepad->isInitialized())
		{
			// Add it to the Inputs' unit's list
			InputsUnit::getSingletonPtr()->_addController(pGamepad);
			++counter;
		}
		else
		{
			delete pGamepad;
		}
	}		
	
	if (counter == 0)
		ATHENA_LOG_ERROR("Failed to initialize the gamepads");
	
	IOObjectRelease(iterator);
	
	return (counter != 0);
}

//-----------------------------------------------------------------------

io_iterator_t MacControllerManager::getDevicesIterator(int usage)
{
	// Retrieve the device lookup map 
	CFMutableDictionaryRef deviceLookupMap = IOServiceMatching(kIOHIDDeviceKey);
	if (!deviceLookupMap)
	{
		ATHENA_LOG_ERROR("Failed to retrieve the device lookup map");
		return 0;
	}
	
	
	// Specify the parameters of the search
	int page = kHIDPage_GenericDesktop;
	
	CFNumberRef usageRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage);
	CFNumberRef pageRef  = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &page);
	
	CFDictionarySetValue(deviceLookupMap, CFSTR(kIOHIDPrimaryUsageKey), usageRef);
	CFDictionarySetValue(deviceLookupMap, CFSTR(kIOHIDPrimaryUsagePageKey), pageRef);

	CFRelease(usageRef);
	CFRelease(pageRef);


	// Retrieves the iterator
	// Note: IOServiceGetMatchingServices consumes the map so we do not have to release it ourself
	io_iterator_t iterator = 0;
	IOReturn result = IOServiceGetMatchingServices(kIOMasterPortDefault, deviceLookupMap, &iterator);
	if (result != kIOReturnSuccess)
	{
		ATHENA_LOG_ERROR("Failed to retrieve an iterator over the devices");
		return 0;
	}

	return iterator;
}
