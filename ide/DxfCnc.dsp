# Microsoft Developer Studio Project File - Name="DxfCnc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DxfCnc - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "DxfCnc.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "DxfCnc.mak" CFG="DxfCnc - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "DxfCnc - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "DxfCnc - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DxfCnc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release/DxfCnc"
# PROP BASE Intermediate_Dir "Release/DxfCnc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C:\Wrk\Dxfcnc3d\DebVC6"
# PROP Intermediate_Dir "C:\Wrk\Dxfcnc3d\DebVC6"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /GX /Os /Ob2 /I "include" /I "NeuronComm" /I "." /I "../fltk" /I "../fltk/zlib" /I "../fltk/png" /I "../fltk/jpeg" /I "../glew-2.1.0/include" /D "SERIALCOM" /D "NDEBUG" /D "FLTK" /D "WIN32" /D "_WINDOWS" /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WIN32_LEAN_AND_MEAN" /D "VC_EXTRA_LEAN" /D "WIN32_EXTRA_LEAN" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ../fltk/lib/fltk.lib opengl32.lib glu32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd" /out:"Release/DxfCnc.exe" /pdbtype:sept /libpath:"..\..\lib"
# SUBTRACT LINK32 /pdb:none /incremental:yes

!ELSEIF  "$(CFG)" == "DxfCnc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug/DxfCnc"
# PROP BASE Intermediate_Dir "Debug/DxfCnc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /Gm /GX /ZI /Od /I "../include" /I "../" /I "../../fltk" /I "../../fltk/zlib" /I "../../fltk/png" /I "../../fltk/jpeg" /I "../../glew-2.1.0/include" /D "BLOCKSIMULATION" /D "SERIALCOM" /D "_DEBUG" /D "FLTK" /D "WIN32" /D "_WINDOWS" /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WIN32_LEAN_AND_MEAN" /D "VC_EXTRA_LEAN" /D "WIN32_EXTRA_LEAN" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../../fltk/lib/vc6/fltk_gld.lib  ../../fltk/lib/vc6/fltkd.lib ../../glew-2.1.0/lib/glew32sd.lib opengl32.lib glu32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\..\vc6\lib"
# SUBTRACT LINK32 /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "DxfCnc - Win32 Release"
# Name "DxfCnc - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\BastelUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BlockModel.cpp
# End Source File
# Begin Source File

SOURCE=..\src\clipperVC6.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CncSetup.cpp
# End Source File
# Begin Source File

SOURCE=..\src\DxfCnc.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Edit.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Execute.cpp
# End Source File
# Begin Source File

SOURCE=..\src\FileFolder.cpp
# End Source File
# Begin Source File

SOURCE=..\src\FingerScroll.cpp
# End Source File
# Begin Source File

SOURCE=..\src\FltkDialogBox.cpp
# End Source File
# Begin Source File

SOURCE=..\src\FltkWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\src\GCodeObject.cpp
# End Source File
# Begin Source File

SOURCE=..\src\GlDisplay.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Layers.cpp
# End Source File
# Begin Source File

SOURCE=..\src\LonMessage.cpp
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

SOURCE=..\src\ProgressStatus.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Resource.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Serialcom.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Settings.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Simplex.cpp
# End Source File
# Begin Source File

SOURCE=..\src\TreeView.cpp
# End Source File
# End Group
# Begin Group "header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\basteltype.h
# End Source File
# Begin Source File

SOURCE=..\include\BastelUtils.h
# End Source File
# Begin Source File

SOURCE=..\include\BlockModel.h
# End Source File
# Begin Source File

SOURCE=..\include\BlockModelTiefen_und_Vertext.h
# End Source File
# Begin Source File

SOURCE=..\include\BlockModelVersuchAbbruch.h
# End Source File
# Begin Source File

SOURCE=..\include\clipper.h
# End Source File
# Begin Source File

SOURCE=..\include\CncSetup.h
# End Source File
# Begin Source File

SOURCE=..\include\CommSnapMaker.h
# End Source File
# Begin Source File

SOURCE=..\include\CVisual_Pad.h
# End Source File
# Begin Source File

SOURCE=..\include\DXF3DDisplayUtils.h
# End Source File
# Begin Source File

SOURCE=..\include\DxfCnc.h
# End Source File
# Begin Source File

SOURCE=..\include\FileFolder.h
# End Source File
# Begin Source File

SOURCE=..\include\FingerScroll.h
# End Source File
# Begin Source File

SOURCE=..\include\FltkDialogBox.h
# End Source File
# Begin Source File

SOURCE=..\include\FltkWindow.h
# End Source File
# Begin Source File

SOURCE=..\include\GCodeDown.h
# End Source File
# Begin Source File

SOURCE=..\include\GCodeObject.h
# End Source File
# Begin Source File

SOURCE=..\include\GlDialogBox.h
# End Source File
# Begin Source File

SOURCE=..\include\GlDisplay.h
# End Source File
# Begin Source File

SOURCE=..\include\glew.h
# End Source File
# Begin Source File

SOURCE=..\include\gpc.h
# End Source File
# Begin Source File

SOURCE=..\include\Layers.h
# End Source File
# Begin Source File

SOURCE=..\include\LonMessage.h
# End Source File
# Begin Source File

SOURCE=..\include\MatrixTransform.h
# End Source File
# Begin Source File

SOURCE=..\include\NNetmgmt.h
# End Source File
# Begin Source File

SOURCE=..\include\Object.h
# End Source File
# Begin Source File

SOURCE=..\include\ProgressStatus.h
# End Source File
# Begin Source File

SOURCE=..\include\Resource.h
# End Source File
# Begin Source File

SOURCE=..\include\Serialcom.h
# End Source File
# Begin Source File

SOURCE=..\include\Settings.h
# End Source File
# Begin Source File

SOURCE=..\include\TouchLib.h
# End Source File
# Begin Source File

SOURCE=..\include\trackball.h
# End Source File
# Begin Source File

SOURCE=..\include\Tree.h
# End Source File
# Begin Source File

SOURCE=..\include\TreeView.h
# End Source File
# End Group
# End Target
# End Project
