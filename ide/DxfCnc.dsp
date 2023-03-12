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
# ADD CPP /nologo /MTd /Gm /GX /ZI /Od /I "include" /I "NeuronComm" /I "." /I "../fltk" /I "../fltk/zlib" /I "../fltk/png" /I "../fltk/jpeg" /I "../glew-2.1.0/include" /D "BLOCKSIMULATION" /D "SERIALCOM" /D "_DEBUG" /D "FLTK" /D "WIN32" /D "_WINDOWS" /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WIN32_LEAN_AND_MEAN" /D "VC_EXTRA_LEAN" /D "WIN32_EXTRA_LEAN" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glu32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\..\vc6\lib"
# SUBTRACT LINK32 /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "DxfCnc - Win32 Release"
# Name "DxfCnc - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BastelUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\BlockModel.cpp
# End Source File
# Begin Source File

SOURCE=.\clipperVC6.cpp
# End Source File
# Begin Source File

SOURCE=.\CncSetup.cpp
# End Source File
# Begin Source File

SOURCE=.\DxfCnc.cpp
# End Source File
# Begin Source File

SOURCE=.\Edit.cpp
# End Source File
# Begin Source File

SOURCE=.\Execute.cpp
# End Source File
# Begin Source File

SOURCE=.\FileFolder.cpp
# End Source File
# Begin Source File

SOURCE=.\FingerScroll.cpp
# End Source File
# Begin Source File

SOURCE=.\FltkDialogBox.cpp
# End Source File
# Begin Source File

SOURCE=.\FltkWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\GCodeObject.cpp
# End Source File
# Begin Source File

SOURCE=.\GlDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\GlDisplay.h
# End Source File
# Begin Source File

SOURCE=.\Layers.cpp
# End Source File
# Begin Source File

SOURCE=.\LonMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\MatrixTransform.cpp
# End Source File
# Begin Source File

SOURCE=.\Object.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\PolygonObject.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\Resource.cpp
# End Source File
# Begin Source File

SOURCE=.\Serialcom.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings.cpp
# End Source File
# Begin Source File

SOURCE=.\Simplex.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeView.cpp
# End Source File
# End Group
# Begin Group "header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\include\basteltype.h
# End Source File
# Begin Source File

SOURCE=.\include\BastelUtils.h
# End Source File
# Begin Source File

SOURCE=.\BlockModel.h
# End Source File
# Begin Source File

SOURCE=.\clipper.h
# End Source File
# Begin Source File

SOURCE=.\CncSetup.h
# End Source File
# Begin Source File

SOURCE=.\DXF3DDisplay.h
# End Source File
# Begin Source File

SOURCE=.\DXF_CncSetup.h
# End Source File
# Begin Source File

SOURCE=.\DXF_Object.h
# End Source File
# Begin Source File

SOURCE=.\DXF_Tree.h
# End Source File
# Begin Source File

SOURCE=.\DxfCnc.h
# End Source File
# Begin Source File

SOURCE=.\FileFolder.h
# End Source File
# Begin Source File

SOURCE=.\FingerScroll.h
# End Source File
# Begin Source File

SOURCE=.\FltkDialogBox.h
# End Source File
# Begin Source File

SOURCE=.\FltkWindow.h
# End Source File
# Begin Source File

SOURCE=.\GCodeObject.h
# End Source File
# Begin Source File

SOURCE=.\Layers.h
# End Source File
# Begin Source File

SOURCE=.\LonMessage.h
# End Source File
# Begin Source File

SOURCE=.\MatrixTransform.h
# End Source File
# Begin Source File

SOURCE=.\NNetmgmt.h
# End Source File
# Begin Source File

SOURCE=.\Object.h
# End Source File
# Begin Source File

SOURCE=.\ProgressStatus.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Serialcom.h
# End Source File
# Begin Source File

SOURCE=.\Settings.h
# End Source File
# Begin Source File

SOURCE=.\TreeView.h
# End Source File
# End Group
# End Target
# End Project
