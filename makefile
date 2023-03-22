cxx = "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64\cl.exe"
cflags = /I./cryptocpp/incl /I./opencv/include /I./GTLFSDK/Incl /I./json /I"C:\Users\SteveChung\AppData\Local\Programs\Python\Python310\include" /I"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\include" /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\shared" /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\ucrt" /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\um" /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\winrt"  
ldflags = /LIBPATH:./cryptocpp/lib /LIBPATH:./opencv/lib /LIBPATH:"C:\Users\SteveChung\AppData\Local\Programs\Python\Python310\libs" /LIBPATH:./GTLFSDK/lib /LIBPATH:"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\lib\x64"  /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.18362.0\ucrt\x64" /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.18362.0\um\x64" /LIBPATH:"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\lib\x64"
libs = GLTFSDK.lib cryptlib.lib python310.lib opencv_world470.lib
liber = "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64\lib.exe"


parser:
	$(cxx) $(cflags) /DBUILDPARSERAPI /std:c++17 /MD Parser.cc /c /Fo:Parser.obj
	$(cxx) $(cflags) /std:c++17 /MD SerializeBinary.cc /c /Fo:SerializeBinary.obj
	$(cxx) $(cflags) /MD CalculateJson.cc /c /Fo:CalculateJson.obj
	$(cxx) $(cflags) /MD CompressImage.cc /c /Fo:CompressImage.obj
	$(cxx) $(cflags)  /std:c++17 /MD /c CalculateHash.cc /Fo:CalculateHash.obj
	$(cxx) /LD Parser.obj CalculateHash.obj CalculateJson.obj SerializeBinary.obj CompressImage.obj ParserStream.lib  /link $(ldflags) $(libs) /OUT:Parser.dll /IMPLIB:Parser.lib

streamer:
	$(cxx) $(cflags) /std:c++17 /MD /c ParserStream.cc /Fo:ParserStream.obj
	$(liber) ParserStream.obj /OUT:ParserStream.lib

test:
	$(cxx) $(cflags) test.cc /link $(ldflags) $(libs) /OUT:test.exe