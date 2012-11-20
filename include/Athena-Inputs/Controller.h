/** @file   Controller.h
    @author Philip Abbet

    Declaration of the class 'Athena::Inputs::Controller'
*/

#ifndef _ATHENA_INPUTS_CONTROLLER_H
#define _ATHENA_INPUTS_CONTROLLER_H

#include <Athena-Inputs/Prerequisites.h>
#include <Athena-Inputs/Declarations.h>
#include <OIS/OISObject.h>
#include <map>


namespace Athena {
namespace Inputs {

//---------------------------------------------------------------------------------------
/// @brief  Base class for all the controllers
///
/// The actual implementation for the controllers (keyboard, mouse and gamepad) is
/// platform-specific.
///
/// The Inputs Unit will register an Events listener with each controller. When the
/// inputs of a controller are read, it must use the registered listeners to
/// report them (there can be more than one listener per controller).
///
/// A controller can be activated and deactivated, in which case its inputs will
/// not be read.
//---------------------------------------------------------------------------------------
class ATHENA_INPUTS_SYMBOL Controller
{
    //_____ Construction / Destruction __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief  Constructor
    /// @param  pOISObject  The OIS controller object
    /// @param  uiIndex     The index of the controller
    //-----------------------------------------------------------------------------------
    Controller(OIS::Object* pOISObject, unsigned int uiIndex);

    //-----------------------------------------------------------------------------------
    /// @brief  Destructor
    //-----------------------------------------------------------------------------------
    virtual ~Controller();


    //_____ Management of the controller __________
public:
    //-----------------------------------------------------------------------------------
    /// @brief  Return the type of the controller (keyboard, mouse or gamepad)
    /// @return The type of the controller
    //-----------------------------------------------------------------------------------
    inline const OIS::Type getType() const { return m_pOISObject->type(); }

    //-----------------------------------------------------------------------------------
    /// @brief  Returns the index of the controller. The couple (type, index) uniquely
    ///         identifies a controller.
    /// @return The index of the controller
    //-----------------------------------------------------------------------------------
    inline const unsigned int getIndex() const { return m_uiIndex; }

    //-----------------------------------------------------------------------------------
    /// @brief  Returns the name of the controller (reported by its driver)
    /// @return The name of the controller
    //-----------------------------------------------------------------------------------
    inline const std::string& getName() const { return m_pOISObject->vendor(); }

    // //-----------------------------------------------------------------------------------
    // /// @brief  Returns the number of keys on the controller
    // /// @return The number of keys
    // //-----------------------------------------------------------------------------------
    // inline const unsigned int getNbKeys() const { return m_uiNbKeys; }
    //
    // //-----------------------------------------------------------------------------------
    // /// @brief  Returns the number of axes on the controller
    // /// @return The number of axes
    // //-----------------------------------------------------------------------------------
    // inline const unsigned int getNbAxes() const { return m_uiNbAxes; }
    //
    // //-----------------------------------------------------------------------------------
    // /// @brief  Returns the number of POVs on the controller
    // /// @return The number of POVs
    // //-----------------------------------------------------------------------------------
    // inline const unsigned int getNbPOVs() const { return m_uiNbPOVs; }
    //
    //-----------------------------------------------------------------------------------
    /// @brief  Indicates if the controller is activated
    /// @return 'true' if the controller is activated
    //-----------------------------------------------------------------------------------
    inline bool isActive() const { return m_pOISObject->buffered(); }

    //-----------------------------------------------------------------------------------
    /// @brief  Activate/Deactivate the controller
    ///
    /// @param  bActivate   Indicates if the controller must be activated
    /// @return             'true' if successful
    //-----------------------------------------------------------------------------------
    inline bool activate(bool bActivate = true) { m_pOISObject->setBuffered(bActivate); }

    //-----------------------------------------------------------------------------------
    /// @brief  Read the inputs of the controller
    ///
    /// Must be overriden by each controller
    /// @return 'true' if successful
    //-----------------------------------------------------------------------------------
    virtual void capture() { m_pOISObject->capture(); }

    //-----------------------------------------------------------------------------------
    /// @brief  Returns a string representation of the controller
    /// @return The string representation
    //-----------------------------------------------------------------------------------
    const std::string toString() const;

    // //-----------------------------------------------------------------------------------
    // /// @brief  Returns the name of a key of the controller
    // ///
    // /// Each controller should override this method to actually retrieve the name of the
    // /// key and store it in the m_strKeys list for later uses.
    // ///
    // /// The preferred implementation is to call this base method, and if the result is an
    // /// empty string, ask the driver for the name and put it in the list.
    // /// @param  key The key
    // /// @return     The name of the key
    // //-----------------------------------------------------------------------------------
    // virtual const std::string getNameOfKey(tKey key);
    //
    // //-----------------------------------------------------------------------------------
    // /// @brief  Returns the name of an axis of the controller
    // ///
    // /// Each controller should override this method to actually retrieve the name of the
    // /// axis and store it in the m_strAxes list for later uses.
    // ///
    // /// The preferred implementation is to call this base method, and if the result is an
    // /// empty string, ask the driver for the name and put it in the list.
    // /// @param  axis    The axis
    // /// @return         The name of the axis
    // //-----------------------------------------------------------------------------------
    // virtual const std::string getNameOfAxis(tAxis axis);
    //
    // //-----------------------------------------------------------------------------------
    // /// @brief  Returns the name of a POV of the controller
    // ///
    // /// Each controller should override this method to actually retrieve the name of the
    // /// POV and store it in the m_strAxes list for later uses.
    // ///
    // /// The preferred implementation is to call this base method, and if the result is an
    // /// empty string, ask the driver for the name and put it in the list.
    // /// @param  pov The POV
    // /// @return     The name of the POV
    // //-----------------------------------------------------------------------------------
    // virtual const std::string getNameOfPOV(tPOV pov);

    //-----------------------------------------------------------------------------------
    /// @brief  Register an events listener
    ///
    /// @param  pListener   The listener
    //-----------------------------------------------------------------------------------
    void registerListener(IEventsListener* pListener);

    //-----------------------------------------------------------------------------------
    /// @brief  Remove an events listener
    ///
    /// @param  pListener   The listener
    //-----------------------------------------------------------------------------------
    void removeListener(IEventsListener* pListener);


    //_____ Internal types __________
protected:
    typedef std::map<tKey, std::string>     tKeyNamesList;
    typedef std::map<tAxis, std::string>    tAxisNamesList;
    typedef std::map<tPOV, std::string>     tPOVNamesList;
    typedef std::vector<IEventsListener*>   tListenersList;


    //_____ Attributes __________
protected:
    OIS::Object*    m_pOISObject;   ///< The OIS controller object
    unsigned int    m_uiIndex;      ///< Index of the controller

    unsigned int    m_uiNbKeys;     ///< Number of keys
    unsigned int    m_uiNbAxes;     ///< Number of axes
    unsigned int    m_uiNbPOVs;     ///< Number of 'Point-of-view controllers'
    tAxis           m_axes;         ///< Available axes

    tKeyNamesList   m_strKeys;      ///< Name of the Keys
    tAxisNamesList  m_strAxes;      ///< Name of the axes
    tPOVNamesList   m_strPOVs;      ///< Name of the point-of-views

    tListenersList  m_listeners;    ///< Events listener registered
};

}
}

#endif
