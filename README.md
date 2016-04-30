A CLR/C# wrapper for Northern Digital's Optotrak system.  One of the benefits to interface the C library to CLI is to script short Optotrak sessions with Powershell.

```Powershell
> [Reflection.Assembly]::LoadFile("U:\bin\Optotrak.dll")
> [VML.Optotrak]::initialize()
> $collector=New-Object VML.OptoCollector
> $collector.add_markers(1)
> $collector.setup_collections()
> $collector.activate()
> $collector.update_frame()
> $collector.get_position(0)
> [VML.Optotrak]::shutdown()
```
The first line loads the dll to PowerShell.  The dll has two classes within the namespace VML.  VML::Optotrak has a few static members, most important of which initialize and shutdown the system.  VML::OptoCollector does the position data collecting.  I create a collector object in the 3rd line, then add one marker to the collector, set up this collection session with default parameters (sampling frequency, etc), activate the markers, and get one frame of data (printed out to the console).  That's it.  No fluff whatsoever.  If I forget how to call, say, add_markers, I can just type the function name without parameters
```Powershell
>$collector.add_markers
```
PowerShell will give the signature of the function.  If I don't remember what's in the OptoCollector class at all,
```Powershell
>$collector | Get-Members
```
will give all the member functions in the class.

If I want to get ten frames, do
```Powershell
> [1..10] | foreach {$collector.update_frame(); $collector.get_position(0)}
```
