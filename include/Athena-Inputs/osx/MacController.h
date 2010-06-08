/** @file	MacController.h
	@author	Philip Abbet
 
	Declaration of the class 'Athena::Inputs::MacController'
*/

#ifndef _ATHENA_INPUTS_MACCONTROLLER_H_
#define _ATHENA_INPUTS_MACCONTROLLER_H_

#include <Athena-Inputs/Prerequisites.h>
#include <Athena-Inputs/Declarations.h>
#include <Athena-Inputs/Controller.h>

#import <IOKit/hid/IOHIDKeys.h>


namespace Athena {
namespace Inputs {


//---------------------------------------------------------------------------------------
/// @brief	Base class for the Mac controllers
//---------------------------------------------------------------------------------------
class MacController: public Controller
{
	//_____ Construction / Destruction __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief	Constructor
    ///
    /// @param	hidDevice	HID device
    /// @param	type		The type of the controller
    /// @param	uiNo		The no of the controller
    //-----------------------------------------------------------------------------------
	MacController(io_object_t hidDevice, tControllerType type, unsigned int uiNo = 1);

    //-----------------------------------------------------------------------------------
    /// @brief	Destructor
    //-----------------------------------------------------------------------------------
	virtual ~MacController();
	
	
	//_____ Management of the controller __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief	Activate or deactivate the controller
    /// 
    /// @param	bActivate	'true' to activate
    /// @return				'true' if successful
    //-----------------------------------------------------------------------------------
	virtual bool activate(bool bActivate = true);

	virtual void setExclusiveMode(bool bExclusive);

	virtual const std::string getNameOfKey(tKey key);
	virtual const std::string getNameOfAxis(tAxis axis);
	virtual const std::string getNameOfPOV(tPOV pov);
	
protected:
	virtual bool acquire(bool bAcquire = true) = 0;

	void parseDeviceProperties(CFDictionaryRef properties);

	void onPartEnumerated(tControllerPart type, tInputEventPart part,
						  CFDictionaryRef element);
	
	
	//_____ Internal types __________
protected:
	struct tCookieInfos
	{
		tControllerPart type;
		tInputEventPart part;
	};

	
	//_____ Attributes __________
protected:
	io_object_t									m_hidDevice;
	bool										m_bExclusiveMode;
	std::map<IOHIDElementCookie, tCookieInfos>	m_cookies;
};

}
}

#endif
