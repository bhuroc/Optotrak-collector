SystemDrive=C:
ND_LIB=/libpath:$(SystemDrive)\\NDIoapi\\ndlib\\msvc oapi.lib NDItb.lib
ND_INCLUDE=/I$(SystemDrive)\\NDIoapi\\ndlib\\include

CXXFLAGS=$(ND_INCLUDE) /clr /c /LD

%.obj:%.cc
	cl $(CXXFLAGS) /Fo$@ $<

Optotrak.dll:Optotrak.obj OptoCollector.obj
	link /DLL /out:$@ $^ $(ND_LIB)
	mt -nologo -manifest $@.manifest -outputresource:$@\;2

Opto:Opto.cs
	csc $< /debug+ /r:Optotrak.dll

clean:
	rm -rf Optotrak.dll *.obj
