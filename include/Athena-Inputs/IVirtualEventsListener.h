/** @file	IVirtualEventsListener.h
	@author	Philip Abbet

	Declaration of the interface 'Athena::Inputs::IVirtualEventsListener'
*/

#ifndef _ATHENA_INPUTS_IVIRTUALEVENTSLISTENER_H_
#define _ATHENA_INPUTS_IVIRTUALEVENTSLISTENER_H_

#include <Athena-Inputs/Prerequisites.h>
#include <Athena-Inputs/Declarations.h>


namespace Athena {
namespace Inputs {

//---------------------------------------------------------------------------------------
/// @brief	Represents an event's listener for a virtual controller
//---------------------------------------------------------------------------------------
class ATHENA_SYMBOL IVirtualEventsListener
{
	//_____ Construction / Destruction __________
public:
	//-----------------------------------------------------------------------------------
	/// @brief	Destructor
	//-----------------------------------------------------------------------------------
	virtual ~IVirtualEventsListener() {};


	//_____ Methods to override __________
public:
	//-----------------------------------------------------------------------------------
	/// @brief	Called when an event occured on the virtual controller the listener is
	///			attached to
	///
	/// Must be overriden by each listener
	/// @param	pEvent	The event
	//-----------------------------------------------------------------------------------
	virtual void onEvent(tVirtualEvent* pEvent) = 0;
};

}
}

#endif
