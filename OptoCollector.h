#ifndef _OPTOCOLLECTOR_H_
#define _OPTOCOLLECTOR_H_

/**
 *@file OptoCollector.h
 *@brief 
 */
#include <vector>
#include "ndtypes.h"
#include "ndpack.h"
#include "ndopto.h"

namespace VML {
    enum RotationFormat {
        UNDEFINED,
        EULER,
        QUATERNION,
        MATRIX
    };


    /**
     * Optotrak collector container.  Possible combinations include
     * <ul>
     * <li> A single marker </li>
     * <li> A group of markers </li>
     * <li> A single rigid body </li>
     * <li> Several rigid bodies </li>
     * <li> Markers and rigid bodies </li>
     * </ul>
     * A marker or a rigid body is called an <it>element</it>.
     *
     * Don't inherit this class for extra functionalities.  Decorate it
     * (syntactically still inheritance, but no semantically).  It's 
     * easier that way to have a FilteredCollector, a BufferedCollector
     * and a FilteredBufferedCollector.
     *
     * How the memory is arranged with different strober and maker 
     * combintations isn't clear from the manual though.  Here are some
     * common situations and their correct settings.
     *
     * Case 1
     * We have a rigid body ("laser1") with 6 markers and another
     * marker.
     * The strober of laser1 is connected to port 1 and the single marker
     * is on its own strober and daisy-chained to laser1's strober.
     * Solution
     * Add the rigid body first, first marker set to 1. Add the single marker
     * next.  Everything works.
     *
     * Case 2
     * The single marker is connected to port 1 and the laser1 strober is
     * daisy chained.
     * Add the marker and then add laser1, first marker set to 2.  Won't work.
     * Solution
     * If we use DataGetLeastTransform, the marker_data will show the first
     * marker position all right, but the next five are missing.  The 7th
     * marker is actually the 1st of laser1.  So the right way is to use 12
     * as the total number of markers when calling SetupCollection.  Set the 
     * 1st marker be 7 when calling add_rigid_body().  The five empty sockets
     * of strober 1 will be missing, as if there were markers connected to 
     * them.  Obviously we should allocate space for 12 markers.  We just
     * ignore the middle 5 (non-exist) markers.
     *
     * Case 3
     * Connect two strobers to the first two ports.
     * Add rigid body first or add the marker first.  Won't work.  Because, 
     * I think, by default, optotrak assumes all markers are at port 1.
     * Solution
     * Use OptotrakSetStroberTable.  If laser1 is at port 1, it's (6,1,0,0)
     * and 1st marker is 1.  
     * If laser1 is at port 2, it's (1,6,0,0) and the 1st marker of laser1
     * is 2, not 7.
     * In both cases, we only need to allocate space for 7 markers.
     *
     * Case 4
     * Two rigid bodies to one port
     * If the first has 6, it's the case in sample11.c.  If the first has 3
     * markers (cuby1), then we need to set the 1st marker of laser1 to 7 and
     * use 12 markers to call SetupCollection.  The same reason in case 2.
     *
     */

    public ref class OptoCollector {

        public:
	    /**
	     * Default ructor sets default values for OptotrakSetupCollection.
	     */
	    OptoCollector();

	    /**
	     * Destructor.
	     */
	    virtual ~OptoCollector();

            /**
             * Add n markers on port number p;
             */
            void add_markers(int n, int p);
            void add_markers(int n) {
                add_markers(n, 0);
            }


            /**
             * Prepare for the collection.
             */
            void setup_collection();

	    /**
	     * Activate the markers.  It looks like a good idea to put the
             * functionalities of activate() and setup_collection() together.
             * But we may deactivate the markers and then re-activate them
             * again.  Although it's unlikely we'll do this, but let's keep
             * the interface stable.
	     */
	    void activate();
	    void deactivate();

	    /**
	     * Get the number of elements (markers or ridig bodies) in this collector.
	     */
	    int get_num_elements()  {
		return num_elements;
	    }

	    /**
	     * @brief Retrieve the 3-D position of the n-th element.
	     * @param n Which element(marker or rigid body) to read from. Default is the 1st one.
	     * @param x,y,z The position.
	     * @return frame number if data are valid.
	     */
	    int get_position(array<double> ^x, int n) ;
	    int get_position(array<double> ^x) {
                return get_position(x,0);
            }

            array<double> ^get_position(int n);

            /**
             * @brief Ask the optotrak system for a new frame of data.
             * @return frame number. -1 if anything's wrong or data unavailable (non-blocking update).
             */
	    int update_frame();
	    int update_frame_blocking();
	    int update_frame_nonblocking();


	    void set_nonblocking() { nonblocking = true; }
	    void set_blocking() { nonblocking = false; }
	    bool is_nonblocking()  { return nonblocking; }

            /**
             * Without this flag, optotrak doesn't really do a 
             * blocking retrieval with GetLatestData().
             */
            void enforce_blocking() {
                collect_flags |=OPTOTRAK_GET_NEXT_FRAME_FLAG;
            }

	private:
	    int num_marker_elements;//< Number of markers or rigid bodies
            int num_rigid_body_elements;
	    int num_elements;
            int total_num_markers;
            int collect_flags;

            literal int NUM_PORTS=4;
	    bool nonblocking;
	    int frame_number, nelements, flags;

	    Position3d *marker_data;

        public:
            property float frame_frequency; //< Frequency to collect data frames (120).
            property float marker_frequency;//< Marker frequency for marker maximum on-time (2500).
            property int threshold; // Dynamic or Static Threshold value to use (30).
            property int gain; //< Minimum gain code amplification to use (160).
            property int stream_mode; //< Stream mode for the data buffers (1).
            property float cycle; // Marker Duty Cycle to use (0.4).
            property float voltage; //< Voltage to use when turning on markers (7.0).
            property float collect_time; //< Number of seconds for buffered data collections.
            property float trigger_time;   //< Number of seconds to pre-trigger data by.
    };

} // end of namespace


#endif/*_OPTOCOLLECTOR_H_*/

