/**
 *@file Optotrak.cc
 *@brief 
 */
#include "ndtypes.h"
#include "ndpack.h"
#include "ndopto.h"
#include "Optotrak.h"

namespace VML {

    void Optotrak::initialize()
    {
        using namespace System;
	if( TransputerLoadSystem( "system" ) != OPTO_NO_ERROR_CODE ){
	    throw gcnew System::Exception("TransputerLoadSystem returns error");
	}

	Optotrak::sleep(1);

	if( TransputerInitializeSystem(OPTO_LOG_ERRORS_FLAG|OPTO_LOG_MESSAGES_FLAG)){
	    throw gcnew System::Exception("TransputerInitializeSystem returns error");
	}
	Optotrak::sleep(1);

        // on-host conversions have to be enabled for rigid body processing
        if( OptotrakSetProcessingFlags( OPTO_LIB_POLL_REAL_DATA |
                    OPTO_CONVERT_ON_HOST |
                    OPTO_RIGID_ON_HOST ) ) {
            throw gcnew System::Exception("OptotrakSetProcessingFlags returns error.");
        }

	if( OptotrakLoadCameraParameters( "standard" ) ){
	    throw gcnew System::Exception("OptotrakLoadCameraParameters returns error");
	}

    }

    void Optotrak::shutdown()
    {

	OptotrakDeActivateMarkers();
	Optotrak::sleep(1);

	if( TransputerShutdownSystem() ){
	    throw gcnew System::Exception("TransputerShutdownSystem returns error");
	}
    }

    void Optotrak::sleep(int s)
    {
        using namespace System::Threading;
        Thread::Sleep(s*1000);
    }

} // end of namespace
