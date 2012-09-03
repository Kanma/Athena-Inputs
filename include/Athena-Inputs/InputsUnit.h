/** @file	Athena_Inputs_InputsUnit.h
	@author	Philip Abbet

	Declaration of the class 'Athena::Inputs::InputsUnit'
*/

#ifndef _ATHENA_INPUTS_INPUTSUNIT_H_
#define _ATHENA_INPUTS_INPUTSUNIT_H_

#include <Athena-Inputs/Prerequisites.h>
#include <Athena-Inputs/Declarations.h>
#include <Athena-Inputs/Controller.h>
#include <Athena-Inputs/VirtualController.h>
#include <Athena-Inputs/IEventsListener.h>
#include <vector>
#include <map>
#include <deque>


namespace Athena {
namespace Inputs {

//---------------------------------------------------------------------------------------
/// @brief	Represents the Inputs Unit
///
/// This unit is responsible of the creation/destruction of the controllers and of the
/// virtual controllers.
///
/// A controller can be a keyboard, a mouse or a gamepad. It is uniquely identified by
/// a (type, index) pair. For the keyboard and the mouse, 'index' can be left at 1,
/// since only one keyboard and mouse are supported in the operating system.
/// 
/// A virtual controller is a convenient way to group together inputs logically related
/// (for instance, all the inputs used by a player). It can be composed of virtual keys,
/// virtual axes and virtual POVs from any controller, and identified by virtual IDs.
/// The Inputs Unit is the repository for those virtual IDs (registration, conversion
/// ID <-> name).
///
/// In the future, a 'combo system' will be implemented. It will rely on 'shortcuts'
/// registered for each virtual key, axis and POV with the Inputs Unit.
///
/// The Inputs Unit doesn't create the controllers itself. It's the job of a
/// platform-specific Controller Manager (@see IControllerManager).
///
/// At shutdown, the destruction of the remaining controllers and virtual controllers is
/// handled by the unit.
///
/// This class is a singleton.
//---------------------------------------------------------------------------------------
class ATHENA_INPUTS_SYMBOL InputsUnit: public Utils::Singleton<InputsUnit>, public IEventsListener
{
	//_____ Construction / Destruction __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief	Constructor
    //-----------------------------------------------------------------------------------
	InputsUnit();

    //-----------------------------------------------------------------------------------
    /// @brief	Destructor
    //-----------------------------------------------------------------------------------
	virtual ~InputsUnit();

    //-----------------------------------------------------------------------------------
    /// @brief	Return a pointer to the instance of the Inputs Unit
    /// @return	A pointer to the Inputs Unit
    //-----------------------------------------------------------------------------------
	static InputsUnit& getSingleton();

    //-----------------------------------------------------------------------------------
    /// @brief	Return a reference to the instance of the Inputs Unit
    /// @return	A reference to the Inputs Unit
    //-----------------------------------------------------------------------------------
	static InputsUnit* getSingletonPtr();


	//_____ Management of the unit __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief	Initialise the Inputs Unit
    ///
    /// @param	mainWindowHandle    Platform-specific handle of the main window of the
    ///                             application (Windows: the HWND, MacOS X: not used)
    /// @return				        'true' if successful
    //-----------------------------------------------------------------------------------
	bool init(void* mainWindowHandle);

    //-----------------------------------------------------------------------------------
    /// @brief	Read the inputs on each active controller, and transmit them to the
    ///			virtual controllers
    //-----------------------------------------------------------------------------------
	void process();

    //-----------------------------------------------------------------------------------
    /// @brief	Scan the inputs state of all the controllers. The purpose of this fonction
    ///			is to implement an 'Inputs configuration' screen
    ///
    /// @param	pListener	The input events will be reported to this listener instead of
    ///						going through the standard path to the virtual controllers
    //-----------------------------------------------------------------------------------
	void scan(IEventsListener* pListener);


	//_____ Implementation of IEventsListener __________
public:
//-----------------------------------------------------------------------------------
    /// @brief	Called when an event occured on one of the controllers
    ///
    /// This happens when the InputsUnit::process() method is called
    /// @param	pEvent	The event
    //-----------------------------------------------------------------------------------
	void onEvent(tInputEvent* pEvent);


	//_____ Management of the controllers __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief	Add a controller in the list
    ///
    /// @remark	Called by the Controllers manager
    /// @param	pController		The new controller
    //-----------------------------------------------------------------------------------
	void _addController(Controller* pController);

    //-----------------------------------------------------------------------------------
    /// @brief	Remove a controller from the list
    ///
    /// @remark	Called by the Controllers manager
    /// @param	pController		The controller
    //-----------------------------------------------------------------------------------
	void _removeController(Controller* pController);

    //-----------------------------------------------------------------------------------
    /// @brief	Returns a controller
    ///
    /// @param	type	Type of the controller
    /// @param	uiIndex	Index of the controller
    /// @return			The controller, 0 if it doesn't exist
    //-----------------------------------------------------------------------------------
	Controller* getController(tControllerType type, unsigned int uiIndex = 1);

    //-----------------------------------------------------------------------------------
    /// @brief	Returns a controller
    ///
    /// @param	uiIndex	Index of the controller (absolute, not type-based)
    /// @return			The controller
    //-----------------------------------------------------------------------------------
	Controller* getController(unsigned int uiIndex = 1);
	
	//-----------------------------------------------------------------------------------
	/// @brief	Returns the number of controllers
	/// @return	The number of controllers
	//-----------------------------------------------------------------------------------
	inline unsigned int getNbControllers() const { return (unsigned int) m_controllers.size(); }

	//-----------------------------------------------------------------------------------
	/// @brief	Returns the number of gamepads
	/// @return	The number of gamepads
	//-----------------------------------------------------------------------------------
	inline unsigned int getNbGamepads() const {	return m_uiNbGamepads; }


	//_____ Management of the virtual controllers __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief	Create a virtual controller
    ///
    /// @param	strName		The name of the virtual controller
    /// @return				The virtual controller, 0 if failed
    //-----------------------------------------------------------------------------------
	VirtualController* createVirtualController(const std::string& strName);

    //-----------------------------------------------------------------------------------
    /// @brief	Returns a virtual controller
    ///
    /// @param	strName		The name of the virtual controller
    /// @return				The virtual controller, 0 if the name isn't valid
    //-----------------------------------------------------------------------------------
	VirtualController* getVirtualController(const std::string& strName);

    //-----------------------------------------------------------------------------------
    /// @brief	Returns the name of a virtual controller
    ///
    /// @param	pVirtualController	The virtual controller
    /// @return						The name
    //-----------------------------------------------------------------------------------
	const std::string getVirtualControllerName(VirtualController* pVirtualController);

    //-----------------------------------------------------------------------------------
    /// @brief	Destroys a virtual controller
    ///
    /// @param	strName		The name of the virtual controller
    //-----------------------------------------------------------------------------------
	void destroyVirtualController(const std::string& strName);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Destroys a virtual controller
    ///
    /// @param	pVirtualController	The virtual controller
    //-----------------------------------------------------------------------------------
	void destroyVirtualController(VirtualController* pVirtualController);

    //-----------------------------------------------------------------------------------
    /// @brief	Register a virtual ID in the list
    ///
    /// @param	strName		The name of the ID
    /// @param	virtualID	The ID
    /// @return				'true' if successful
    //-----------------------------------------------------------------------------------
	bool registerVirtualID(const std::string& strName, tVirtualID virtualID = 0);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Returns a virtual ID
    ///
    /// @param	strName	The name of the ID
    /// @return			The ID, 0 if the name isn't associated with an ID
    //-----------------------------------------------------------------------------------
	tVirtualID getVirtualID(const std::string& strName);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Returns the name associated with a virtual ID
    ///
    /// @param	virtualID	The ID
    /// @return				The name
    //-----------------------------------------------------------------------------------
	const std::string getVirtualName(tVirtualID virtualID);

    //-----------------------------------------------------------------------------------
    /// @brief	Register a shortcut
    ///
    /// @param	strShortcut		The shortcut
    /// @param	virtualID		The virtual ID associated with the shortcut
    /// @return					'true' if successful
    //-----------------------------------------------------------------------------------
	bool registerShortcut(const std::string& strShortcut, tVirtualID virtualID);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Returns the virtual ID associated with a shortcut
    ///
    /// @param	strShortcut		The shortcut
    /// @return					The virtual ID associated with the shortcut, 0 if none
    //-----------------------------------------------------------------------------------
	tVirtualID getVirtualIDFromShortcut(const std::string& strShortcut);
	
    //-----------------------------------------------------------------------------------
    /// @brief	Returns the shortcut associated with a virtual ID
    ///
    /// @param	virtualID	The virtual ID
    /// @return				The shortcut associated with the virtual ID, an empty string if
    ///						none
    //-----------------------------------------------------------------------------------
	const std::string getShortcutFromVirtualID(tVirtualID virtualID);

    //-----------------------------------------------------------------------------------
    /// @brief	Load some virtual controllers from an XML-based file
    ///
    /// @param	strFile	    The name of the file
    /// @return				'true' if successful
    //-----------------------------------------------------------------------------------
    // bool loadVirtualControllers(const std::string& strFile);
    
    //-----------------------------------------------------------------------------------
    /// @brief	Save all the virtual controllers to an XML-based file
    ///
    /// @param	strFile		The name of the file
    /// @return				'true' if successful
    //-----------------------------------------------------------------------------------
    // bool saveVirtualControllers(const std::string& strFile);


	//_____ Attributes __________
private:
    IControllerManager*                         m_pControllerManager;
    std::vector<Controller*>					m_controllers;			///< List of the connected controllers
	std::map<std::string, VirtualController*>	m_virtualControllers;	///< List of the virtual controllers
	std::map<std::string, tVirtualID>			m_virtualIDs;			///< List of the virtual IDs
	std::map<std::string, tVirtualID>			m_shortcuts;			///< List of the shortcuts
	unsigned int								m_uiNbGamepads;			///< Number of gamepads
	std::deque<tInputEvent>						m_events;				///< List of input events (used when reading the inputs)
};

}
}

#endif
