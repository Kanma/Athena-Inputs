/** @file   Prerequisites.h
    @author Philip Abbet
    
    Declaration of the types of the Athena-Inputs module
*/

#ifndef _ATHENA_INPUTS_PREREQUISITES_H_
#define _ATHENA_INPUTS_PREREQUISITES_H_

#include <Athena-Core/Prerequisites.h>
#include <Athena-Inputs/Config.h>


//----------------------------------------------------------------------------------------
/// @brief	Main namespace. All the components of the Athena engine belongs to this
///			namespace
//----------------------------------------------------------------------------------------
namespace Athena
{
	//------------------------------------------------------------------------------------
	/// @brief	Contains all the inputs-related classes
	//------------------------------------------------------------------------------------
    namespace Inputs
    {
        class Controller;
        class InputsUnit;
        class VirtualController;

        class IControllerManager;
        class IEventsListener;
        class IVirtualEventsListener;
    }
}

#endif
