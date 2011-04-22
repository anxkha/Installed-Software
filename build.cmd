@ECHO OFF

SET LIB=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Lib;C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\lib
cl.exe /nologo /EHsc /c /D "_UNICODE" /D "UNICODE" /Zc:wchar_t /W3 /O2 /Oi /Fo"instsoft.obj" instsoft.cpp
link.exe /SUBSYSTEM:CONSOLE /NOLOGO /INCREMENTAL:NO /OUT:instsoft.exe advapi32.lib instsoft.obj

SET LIB=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Lib\x64;C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\lib\amd64
"C:\program files (x86)\microsoft visual studio 10.0\vc\bin\amd64\cl.exe" /nologo /EHsc /c /D "_UNICODE" /D "UNICODE" /Zc:wchar_t /W3 /O2 /Oi /Fo"instsoft64.obj" instsoft.cpp
"C:\program files (x86)\microsoft visual studio 10.0\vc\bin\amd64\link.exe" /SUBSYSTEM:CONSOLE /NOLOGO /MACHINE:X64 /INCREMENTAL:NO /OUT:instsoft64.exe advapi32.lib kernel32.lib instsoft64.obj