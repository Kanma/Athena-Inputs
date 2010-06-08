/** @file	VirtualController.h
	@author	Philip Abbet

	Declaration of the class 'Athena::Inputs::VirtualController'
*/

#ifndef _ATHENA_INPUTS_VIRTUALCONTROLLER_H_
#define _ATHENA_INPUTS_VIRTUALCONTROLLER_H_

#include <Athena-Inputs/Prerequisites.h>
#include <Athena-Inputs/Declarations.h>
#include <Athena-Inputs/Controller.h>
#include <vector>
#include <map>
#include <deque>


namespace Athena {
namespace Inputs {

//---------------------------------------------------------------------------------------
/// @brief	Represents a virtual controller
/// 
/// A virtual controller is used to group together some related keys, axes and POVs
/// (for instance, all the inputs used by a player).
///
/// It can be composed of virtual keys, virtual axes and virtual POVs from any controller,
/// which are identified by virtual IDs.
///
/// See tVirtualKey, tVirtualAxis and tVirtualPOV for a explaination about them.
///
/// A virtual controller can be enabled and disabled.
//---------------------------------------------------------------------------------------
class ATHENA_SYMBOL VirtualController
{
	friend class InputsUnit;


	//_____ Construction / Destruction __________
protected:
    //-----------------------------------------------------------------------------------
    /// @brief	Constructor
    //-----------------------------------------------------------------------------------
	VirtualController();

    //-----------------------------------------------------------------------------------
    /// @brief	Destructor
    //-----------------------------------------------------------------------------------
	~VirtualController();


	//_____ Management of the virtual controller __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief	Called by the Inputs Unit when events have occured on some real controllers
    ///
    /// The events will be checked to see if one of them correspond to a virtual part of the
    /// virtual controller
    /// @param	events	The list of events
    //-----------------------------------------------------------------------------------
	void process(std::deque<tInputEvent> &events);

    //-----------------------------------------------------------------------------------
    /// @brief	Set the events listener to notify when a virtual event occured
    ///
    /// @param	pEventsListener	The events listener
    //-----------------------------------------------------------------------------------
	void setEventsListener(IVirtualEventsListener* pEventsListener);

    //-----------------------------------------------------------------------------------
    /// @brief	Enable/Disable the virtual controller
    ///
    /// @param	bEnable	'true' to enable the controller
    //-----------------------------------------------------------------------------------
	void enable(bool bEnable);

    //-----------------------------------------------------------------------------------
    /// @brief	Indicates if the virtual controller is enabled
    /// @return	'true' if the virtual controller is enabled
    //-----------------------------------------------------------------------------------
	bool isEnabled();


	//_____ Management of the virtual parts __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief	Register a virtual key
    ///
    ///	@remark	The key isn't associated with a real key
    /// @param	virtualID	ID of the virtual key
    /// @param	strShortcut	Shortcut of the virtual key
    //-----------------------------------------------------------------------------------
	void registerVirtualKey(tVirtualID virtualID, const std::string& strShortcut = "");

    //-----------------------------------------------------------------------------------
    /// @brief	Register a virtual axis
    ///
    ///	@remark	The axis isn't associated with a real axis
    /// @param	virtualID	ID of the virtual axis
    //-----------------------------------------------------------------------------------
	void registerVirtualAxis(tVirtualID virtualID);

    //-----------------------------------------------------------------------------------
    /// @brief	Register a virtual POV
    ///
    ///	@remark	The POV isn't associated with a real POV
    /// @param	virtualID			ID of the virtual POV
    /// @param	strShortcutUp		Shortcut for the UP position
    /// @param	strShortcutDown		Shortcut for the DOWN position
    /// @param	strShortcutLeft		Shortcut for the LEFT position
    /// @param	strShortcutRight	Shortcut for the RIGHT position
    //-----------------------------------------------------------------------------------
	void registerVirtualPOV(tVirtualID virtualID, const std::string& strShortcutUp = "",
							const std::string& strShortcutDown = "",
							const std::string& strShortcutLeft = "",
							const std::string& strShortcutRight = "");

    //-----------------------------------------------------------------------------------
    /// @brief	Add a virtual key
    ///
    /// @param	virtualID	ID of the virtual key
    /// @param	pController	Controller on which is the real key
    /// @param	key			The real key
    /// @param	strShortcut	Shortcut of the virtual key
    //-----------------------------------------------------------------------------------
	void addVirtualKey(tVirtualID virtualID, Controller* pController, tKey key,
					   const std::string& strShortcut = "");

    //-----------------------------------------------------------------------------------
    /// @brief	Add a virtual axis
    ///
    /// @param	virtualID	ID of the virtual axis
    /// @param	pController	Controller on which is the real axis
    /// @param	axis		The real axis
    //-----------------------------------------------------------------------------------
	void addVirtualAxis(tVirtualID virtualID, Controller* pController, tAxis axis);

    //-----------------------------------------------------------------------------------
    /// @brief	Add a virtual axis created from a real POV

    /// @param	virtualID	ID of the virtual axis
    /// @param	pController	Controller on which is the real POV
    /// @param	pov			The real POV
    /// @param	bUpDown		Indicates if the axis is on the Up/Down part of the POV
    //-----------------------------------------------------------------------------------
	void addVirtualAxis(tVirtualID virtualID, Controller* pController, tPOV pov, bool bUpDown);

    //-----------------------------------------------------------------------------------
    /// @brief	Add a virtual axis created from two real keys

    /// @param	virtualID	ID of the virtual axis
    /// @param	pController	Controller on which are the real keys
    /// @param	keyMin		The minimum real key
    /// @param	keyMax		The maximum real key
    //-----------------------------------------------------------------------------------
	void addVirtualAxis(tVirtualID virtualID, Controller* pController, tKey keyMin,
						tKey keyMax);

    //-----------------------------------------------------------------------------------
    /// @brief	Add a virtual POV
    ///
    /// @param	virtualID			ID of the virtual POV
    /// @param	pController			Controller on which is the real POV
    /// @param	pov					The real POV
    /// @param	strShortcutUp		Shortcut for the UP position
    /// @param	strShortcutDown		Shortcut for the DOWN position
    /// @param	strShortcutLeft		Shortcut for the LEFT position
    /// @param	strShortcutRight	Shortcut for the RIGHT position
    //-----------------------------------------------------------------------------------
	void addVirtualPOV(tVirtualID virtualID, Controller* pController, tPOV pov,
					   const std::string& strShortcutUp = "",
					   const std::string& strShortcutDown = "",
					   const std::string& strShortcutLeft = "",
					   const std::string& strShortcutRight = "");

    //-----------------------------------------------------------------------------------
    /// @brief	Add a virtual POV from four real keys
    ///
    /// @param	virtualID			ID of the virtual POV
    /// @param	pController			Controller on which are the real keys
    /// @param	keyUp				The real UP key
    /// @param	keyDown				The real DOWN key
    /// @param	keyLeft				The real LEFT key
    /// @param	keyRight			The real RIGHT key
    /// @param	strShortcutUp		Shortcut for the UP position
    /// @param	strShortcutDown		Shortcut for the DOWN position
    /// @param	strShortcutLeft		Shortcut for the LEFT position
    /// @param	strShortcutRight	Shortcut for the RIGHT position
    //-----------------------------------------------------------------------------------
	void addVirtualPOV(tVirtualID virtualID, Controller* pController,
					   tKey keyUp, tKey keyDown, tKey keyLeft, tKey keyRight,
					   const std::string& strShortcutUp = "",
					   const std::string& strShortcutDown = "",
					   const std::string& strShortcutLeft = "",
					   const std::string& strShortcutRight = "");

    //-----------------------------------------------------------------------------------
    /// @brief	Add a virtual POV from two real axes
    ///
    /// @param	virtualID			ID of the virtual POV
    /// @param	pController			Controller on which are the real axes
    /// @param	axisUpDown			The real UP/DOWN axis
    /// @param	axisLeftRight		The real LEFT/RIGHT axis
    /// @param	strShortcutUp		Shortcut for the UP position
    /// @param	strShortcutDown		Shortcut for the DOWN position
    /// @param	strShortcutLeft		Shortcut for the LEFT position
    /// @param	strShortcutRight	Shortcut for the RIGHT position
    //-----------------------------------------------------------------------------------
	void addVirtualPOV(tVirtualID virtualID, Controller* pController,
					   tAxis axisUpDown, tAxis axisLeftRight,
					   const std::string& strShortcutUp = "",
					   const std::string& strShortcutDown = "",
					   const std::string& strShortcutLeft = "", 
					   const std::string& strShortcutRight = "");

    //-----------------------------------------------------------------------------------
    /// @brief	Indicates if a virtual key is pressed
    ///
    /// @param	virtualKey	The virtual key
    /// @return				'true' if the virtual key is pressed
    //-----------------------------------------------------------------------------------
	bool isKeyPressed(tVirtualID virtualKey);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Indicates if a virtual key was just toggled
    ///
    /// @param	virtualKey	The virtual key
    /// @return				'true' if the virtual was toggled
    //-----------------------------------------------------------------------------------
	bool wasKeyToggled(tVirtualID virtualKey);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Indicates if a virtual key was just pressed
    ///
    /// @param	virtualKey	The virtual key
    /// @return				'true' if the virtual was pressed
    //-----------------------------------------------------------------------------------
	bool wasKeyPressed(tVirtualID virtualKey);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Indicates if a virtual key was just released
    ///
    /// @param	virtualKey	The virtual key
    /// @return				'true' if the virtual was released
    //-----------------------------------------------------------------------------------
	bool wasKeyReleased(tVirtualID virtualKey);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Returns the value of a virtual axis
    ///
    /// @param	virtualAxis	The virtual axis
    /// @return				The value
    //-----------------------------------------------------------------------------------
	int	getAxisValue(tVirtualID virtualAxis);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Indicates if the value of a virtual axis has changed since the last frame
    ///
    /// @param	virtualAxis	The virtual axis
    /// @return				'true' if the value has changed
    //-----------------------------------------------------------------------------------
	bool wasAxisChanged(tVirtualID virtualAxis);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Returns the duration of the press of a virtual key
    ///
    /// @param	virtualKey	The virtual key
    /// @return				The duration
    //-----------------------------------------------------------------------------------
	unsigned int getKeyPressedDuration(tVirtualID virtualKey);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Returns the position of a virtual POV
    ///
    /// @param	virtualPOV	The virtual POV
    /// @return				The position
    //-----------------------------------------------------------------------------------
	tPOVPosition getPOVPosition(tVirtualID virtualPOV);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Returns the previous position of a virtual POV
    ///
    /// @param	virtualPOV	The virtual POV
    /// @return				The previous position
    //-----------------------------------------------------------------------------------
	tPOVPosition getPOVPreviousPosition(tVirtualID virtualPOV);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Indicates if the position of a virtual POV has changed since the last frame
    ///
    /// @param	virtualPOV	The virtual POV
    /// @return				'true' if the position has changed
    //-----------------------------------------------------------------------------------
	bool wasPOVChanged(tVirtualID virtualPOV);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Returns the POV position corresponding to the specified shortcut
    ///
    /// @param	virtualPOV	The virtual POV
    /// @param	strShortcut	The shortcut
    /// @return				The POV position corresponding to the shortcut
    //-----------------------------------------------------------------------------------
	tPOVPosition getPOVPositionFromShortcut(tVirtualID virtualPOV, const std::string& strShortcut);

    //-----------------------------------------------------------------------------------
    /// @brief	Returns the duration of the press of a virtual POV in its current position
    ///
    /// @param	virtualPOV	The virtual POV
    /// @return				The duration
    //-----------------------------------------------------------------------------------
	unsigned int getPOVPressedDuration(tVirtualID virtualPOV);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Indicates if a virtual ID correspond to a virtual key
    ///
    /// @param	virtualID	The virtual ID
    /// @return				'true' if the virtual ID correspond to a virtual key
    //-----------------------------------------------------------------------------------
	bool isKey(tVirtualID virtualID);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Indicates if a virtual ID correspond to a virtual POV
    ///
    /// @param	virtualID	The virtual ID
    /// @return				'true' if the virtual ID correspond to a virtual POV
    //-----------------------------------------------------------------------------------
	bool isPOV(tVirtualID virtualID);

    //-----------------------------------------------------------------------------------
    /// @brief	Returns the number of virtual keys
    /// @return	The number of virtual keys
    //-----------------------------------------------------------------------------------
	unsigned int getNbVirtualKeys();

    //-----------------------------------------------------------------------------------
    /// @brief	Returns the number of virtual axes
    /// @return	The number of virtual axes
    //-----------------------------------------------------------------------------------
	unsigned int getNbVirtualAxes();

    //-----------------------------------------------------------------------------------
    /// @brief	Returns the number of virtual POVs
    /// @return	The number of virtual POVs
    //-----------------------------------------------------------------------------------
	unsigned int getNbVirtualPOVs();

    //-----------------------------------------------------------------------------------
    /// @brief	Return a virtual key
    ///
    /// @param	uiIndex	Index of the virtual key
    /// @retval	id		The virtual ID of the virtual key
    /// @return			The virtual key
    //-----------------------------------------------------------------------------------
	const tVirtualKey& getVirtualKey(unsigned int uiIndex, tVirtualID &id);

    //-----------------------------------------------------------------------------------
    /// @brief	Return a virtual axis
    ///
    /// @param	uiIndex	Index of the virtual axis
    /// @retval	id		The virtual ID of the virtual axis
    /// @return			The virtual axis
    //-----------------------------------------------------------------------------------
	const tVirtualAxis& getVirtualAxis(unsigned int uiIndex, tVirtualID &id);

    //-----------------------------------------------------------------------------------
    /// @brief	Return a virtual POV
    ///
    /// @param	uiIndex	Index of the virtual POV
    /// @retval	id		The virtual ID of the virtual POV
    /// @return			The virtual POV
    //-----------------------------------------------------------------------------------
	const tVirtualPOV& getVirtualPOV(unsigned int uiIndex, tVirtualID &id);

    //-----------------------------------------------------------------------------------
    /// @brief	Return a virtual key
    ///
    /// @param	id		The virtual ID
    /// @return			The virtual key, 0 if not a key
    //-----------------------------------------------------------------------------------
	tVirtualKey* getVirtualKey(tVirtualID id);

    //-----------------------------------------------------------------------------------
    /// @brief	Return a virtual axis
    ///
    /// @param	id		The virtual ID
    /// @return			The virtual axis, 0 if not a axis
    //-----------------------------------------------------------------------------------
	tVirtualAxis* getVirtualAxis(tVirtualID id);

    //-----------------------------------------------------------------------------------
    /// @brief	Return a virtual POV
    ///
    /// @param	id		The virtual ID
    /// @return			The virtual POV, 0 if not a POV
    //-----------------------------------------------------------------------------------
	tVirtualPOV* getVirtualPOV(tVirtualID id);


	//_____ Attributes __________
private:
	std::map<tVirtualID, tVirtualKey>	m_virtualKeys;				///< List of the virtual keys
	std::map<tVirtualID, tVirtualAxis>	m_virtualAxes;				///< List of the virtual axes
	std::map<tVirtualID, tVirtualPOV>	m_virtualPOVs;				///< List of the virtual POVs

	IVirtualEventsListener*				m_pEventsListener;			///< Virtual events listener to use when an event occurs
	bool								m_bEnabled;					///< Indicates if the virtual controller is enabled or not
};

}
}

#endif
