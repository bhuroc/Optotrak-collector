#ifndef _OPTOTRAK_H_
#define _OPTOTRAK_H_

/**
 *@file Optotrak.h
 *@brief 
 */
#include "ndtypes.h"
#include "ndpack.h"
#include "ndopto.h"

namespace VML {

    public ref class Optotrak {
	public:

	    /**
	     * Initialize the transputer.
	     */
	    static void initialize();

	    /**
	     * Shutdown the transputer.
	     */
	    static void shutdown();

            static void sleep(int s);

    };

} // end of namespace
#endif/*_OPTOTRAK_H_*/

