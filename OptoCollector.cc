/**
 *@file OptoCollector.cc
 *@brief 
 */
#include <assert.h>
#include "Optotrak.h"
#include "OptoCollector.h"

using namespace System;

namespace VML {

    OptoCollector::OptoCollector()
        :num_elements(0),
        total_num_markers(0),
        marker_data(0)
    {
        frame_frequency = 120.f;
        marker_frequency = 2500.f;
        threshold = 30; 
        gain = 160;
        stream_mode = 0;
        cycle = 0.4f;
        voltage = 7.0f;
        collect_time = 1.f;
        trigger_time = 0.f;

        nonblocking = false;
        collect_flags = OPTOTRAK_NO_FIRE_MARKERS_FLAG | OPTOTRAK_BUFFER_RAW_FLAG ;
    }

    OptoCollector::~OptoCollector()
    {
        // The first 2 if's are necessary because of the collector
        // can be either pure marker or pure rigid body.
        if(marker_data)
            free(marker_data);
    }

    void OptoCollector::add_markers(int n, int port)
    {
        assert(port < NUM_PORTS);
        num_elements += n;
        total_num_markers += n;
    }


    void OptoCollector::setup_collection()
    {
        if(marker_data) {
            throw gcnew System::Exception("Setup_collection can be called only once.");
        }

        /////////////////////////////////////////////////
        //
        // allocate space
        // 
        /////////////////////////////////////////////////

        // allocate space for markers
        marker_data = (Position3d*)malloc(sizeof(Position3d)*total_num_markers);
        if(marker_data == 0)
            throw gcnew System::Exception("Can't alloc marker data");

        /////////////////////////////////////////////////
        //
        // Set up collection
        // 
        /////////////////////////////////////////////////
	if(OptotrakSetupCollection(
	    total_num_markers,    // Number of markers in the collection. 
	    frame_frequency,// Frequency to collect rigid_data frames at. 
	    marker_frequency, // Marker frequency for marker maximum on-time. 
	    threshold,        // Dynamic or Static Threshold value to use. 
	    gain,             // Minimum gain code amplification to use. 
	    stream_mode,      // Stream mode for the rigid_data buffers. 
	    cycle,            // Marker Duty Cycle to use. 
	    voltage,          // Voltage to use when turning on markers. 
	    collect_time,     // Number of seconds of rigid_data to collect. 
	    trigger_time,     // Seconds to pre-trigger rigid_data by. 
            collect_flags) ) {

	    throw gcnew System::Exception("OptotrakSetupCollection returns error.");
	}

	Optotrak::sleep(1);
    }

    void OptoCollector::activate()
    {
	if( OptotrakActivateMarkers() ) {
	    throw gcnew System::Exception("Can't activate markers.");
	} 

	Optotrak::sleep(1);
    }

    void OptoCollector::deactivate()
    {
	OptotrakDeActivateMarkers();
    }


    int OptoCollector::update_frame()
    {
        int fnumber;
	if(nonblocking) {
	    fnumber = update_frame_nonblocking();
	}else  {
	    fnumber = update_frame_blocking();
	}

        return fnumber;
    }

    int OptoCollector::update_frame_blocking()
    {
        //assert(num_elements > 0);
        // do the markers
        unsigned int fn, ne, f;
        if(DataGetLatest3D(&fn, &ne, &f, marker_data)) {
            return -1;
        }
        frame_number = fn;
        nelements = ne;
        flags = f;

        if(nelements != total_num_markers) {
            Console::WriteLine("Shouldn't happen: missing marker elements.");
            return -1;
        }


	return frame_number;
    }

    int OptoCollector::update_frame_nonblocking()
    {
        assert(num_elements > 0);
        if(RequestLatest3D()) {
            return -1;
        }

        if(!DataIsReady()){
            return -1;
        }

        unsigned int fn, ne, f;
        if(DataReceiveLatest3D(&fn, &ne, &f, marker_data)) {
            return -1;
        }
        frame_number = fn;
        nelements = ne;
        flags = f;

        if(nelements != total_num_markers) {
            Console::WriteLine("Shouldn't happen: missing marker elements.");
            return -1;
        }

	return frame_number;
    }

    int OptoCollector::get_position(array<double> ^p, int n) 
    {
        assert( n < num_elements);
        p[0] = marker_data[n].x;
        p[1] = marker_data[n].y;
        p[2] = marker_data[n].z;
        return frame_number;
    }

    array<double> ^OptoCollector::get_position(int n)
    {
        assert( n < num_elements);
        array<double> ^p=gcnew array<double>(3);
        p[0] = marker_data[n].x;
        p[1] = marker_data[n].y;
        p[2] = marker_data[n].z;
        return p;
    }

} // end of namespace



