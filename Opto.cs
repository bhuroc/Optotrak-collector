using System;
using System.Diagnostics;
using VML;

class Opto {
    public static void Main() {
        Optotrak.initialize();

        OptoCollector oc = new OptoCollector();

        oc.add_markers(3);
        oc.frame_frequency = 500;
        oc.enforce_blocking();
        oc.setup_collection();
        oc.activate();
        double[] x = new double[3];
        double[] y;

        Stopwatch watch = Stopwatch.StartNew();
        double milliPerTick = 1e3/Stopwatch.Frequency;
        for(int i=0; i<10; ++i) {
            watch.Reset();
            watch.Start();
            int fn=oc.update_frame();
            watch.Stop();
            oc.get_position(x, 0);
            Console.Write(i+ " "+fn+ " x:" + x[0] + " " + x[1] + " " + x[2] + " ");
            Console.WriteLine( "at "+watch.ElapsedTicks*milliPerTick);

            y = oc.get_position(0);
            Console.WriteLine(" y:" + y[0] + " " + y[1] + " " + y[2] + " ");
        }

        Console.WriteLine("Stop");
        Optotrak.shutdown();
    }
}

