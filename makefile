libpath = /LIBPATH:"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Lib" /LIBPATH:"C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\lib"
libpath64 = /LIBPATH:"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Lib\x64" /LIBPATH:"C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\lib\amd64"
cl = "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\cl.exe"
cl64 = "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\amd64\cl.exe"
link = "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\link.exe"
link64 = "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\amd64\link.exe"
csharp = csc.exe
csparam = /nologo /target:exe
clparam = /nologo /EHsc /c /D "_CONSOLE" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Zc:wchar_t /Zc:forScope /Gd /GS /Gy /GL /W3 /O2 /Oi
linkparam = /LTCG /RELEASE /SUBSYSTEM:CONSOLE /NOLOGO /MACHINE:X86 /DYNAMICBASE /NXCOMPAT /OPT:REF /OPT:ICF /ALLOWISOLATION /INCREMENTAL:NO $(libpath)
linkparam64 = /LTCG /RELEASE /SUBSYSTEM:CONSOLE /NOLOGO /MACHINE:X64 /DYNAMICBASE /NXCOMPAT /OPT:REF /OPT:ICF /ALLOWISOLATION /INCREMENTAL:NO $(libpath64)
objs = instsoft.obj
objs64 = instsoft64.obj
src = instsoft.cpp
srcdotnet = instsoft.cs
libs = kernel32.lib advapi32.lib
newtarget = instsoft.exe
newtarget64 = instsoft64.exe
target = instsoft.old.exe
target64 = instsoft64.old.exe

all: $(target) $(target64) $(newtarget) $(newtarget64)

x64: $(target64) $(newtarget64)

clean:
	del $(objs) $(objs64) $(target) $(target64) $(newtarget) $(newtarget64)

instsoft.obj: instsoft.cpp
	$(cl) $(clparam) /Fo"$@" instsoft.cpp

instsoft64.obj: instsoft.cpp
	$(cl64) $(clparam) /Fo"$@" instsoft.cpp

$(target): $(objs)
	$(link) $(linkparam) /OUT:$(target) $(libs) $**

$(target64): $(objs64)
	$(link64) $(linkparam64) /OUT:$(target64) $(libs) $**

$(newtarget): $(srcdotnet)
	$(csharp) $(csparam) /platform:anycpu /OUT:$@ $**

$(newtarget64): $(srcdotnet)
	$(csharp) $(csparam) /platform:x64 /OUT:$@ $**
