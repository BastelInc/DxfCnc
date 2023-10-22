# Microsoft Developer Studio Project File - Name="Dxf3Table" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Dxf3Table - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "Dxf3Table.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Dxf3Table.mak" CFG="Dxf3Table - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "Dxf3Table - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "Dxf3Table - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Dxf3Table - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release/Dxf3Table"
# PROP BASE Intermediate_Dir "Release/Dxf3Table"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C:\Wrk\Dxfcnc3d\DebVC6"
# PROP Intermediate_Dir "C:\Wrk\Dxfcnc3d\DebVC6"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /GX /Os /Ob2 /I "include" /I "." /I "../fltk" /I "../fltk/zlib" /I "../fltk/png" /I "../fltk/jpeg" /I "../glew-2.1.0/include" /D "SERIALCOM" /D "NDEBUG" /D "FLTK" /D "WIN32" /D "_WINDOWS" /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WIN32_LEAN_AND_MEAN" /D "VC_EXTRA_LEAN" /D "WIN32_EXTRA_LEAN" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ../fltk/lib/fltk.lib opengl32.lib glu32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd" /out:"Release/Dxf3Table.exe" /pdbtype:sept /libpath:"..\..\lib"
# SUBTRACT LINK32 /pdb:none /incremental:yes

!ELSEIF  "$(CFG)" == "Dxf3Table - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug/Dxf3Table"
# PROP BASE Intermediate_Dir "Debug/Dxf3Table"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugVC6"
# PROP Intermediate_Dir "DebugVC6"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /Gm /GX /ZI /Od /I "../include" /I "../" /I "." /I "../../fltk37" /I "../../fltk37/zlib" /I "../../fltk37/png" /I "../../fltk37/jpeg" /I "../../glew-2.1.0/include" /D "SERIALCOM" /D "_DEBUG" /D "FLTK" /D "WIN32" /D "_WINDOWS" /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WIN32_LEAN_AND_MEAN" /D "VC_EXTRA_LEAN" /D "WIN32_EXTRA_LEAN" /D "DREITABLE" /D "USE_SHADER" /D "use_int32" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../../fltk37/lib/vc6/fltk_gld.lib ../../fltk37/lib/vc6/fltkd.lib ../../glew-2.1.0/lib/glew32sd.lib opengl32.lib glu32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd" /pdbtype:sept /libpath:"..\..\vc6\lib"
# SUBTRACT LINK32 /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "Dxf3Table - Win32 Release"
# Name "Dxf3Table - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\BastelUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\src\clipperVC6.cpp
# End Source File
# Begin Source File

SOURCE=..\src\DreiTafelProjektion.cpp
# End Source File
# Begin Source File

SOURCE=..\src\DXF3DDisplayUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Dxf3Table.cpp
# End Source File
# Begin Source File

SOURCE=..\src\FingerScroll.cpp
# End Source File
# Begin Source File

SOURCE=..\src\GlDisplay.cpp
# End Source File
# Begin Source File

SOURCE=..\src\GlShaderDisplay.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Layers.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MatrixTransform.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Object.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ObjectDraw.cpp
# End Source File
# Begin Source File

SOURCE=..\src\PolygonObject.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Resource.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Settings.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Simplex.cpp
# End Source File
# Begin Source File

SOURCE=..\src\StlObject.cpp
# End Source File
# Begin Source File

SOURCE=..\src\TreeView.cpp
# End Source File
# End Group
# Begin Group "header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\clipper.h
# End Source File
# Begin Source File

SOURCE=..\include\DXF3DDisplayUtils.h
# End Source File
# Begin Source File

SOURCE=..\include\FingerScroll.h
# End Source File
# Begin Source File

SOURCE=..\include\GlDisplay.h
# End Source File
# Begin Source File

SOURCE=..\include\Layers.h
# End Source File
# Begin Source File

SOURCE=..\include\MatrixTransform.h
# End Source File
# Begin Source File

SOURCE=..\include\Object.h
# End Source File
# Begin Source File

SOURCE=..\include\Settings.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\GCodeObject.cpp
# End Source File
# End Target
# End Project
