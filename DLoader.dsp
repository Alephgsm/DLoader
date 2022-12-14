# Microsoft Developer Studio Project File - Name="DLoader" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DLoader - Win32 Cust Factory Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DLoader.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DLoader.mak" CFG="DLoader - Win32 Cust Factory Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DLoader - Win32 Cust Debug" (based on "Win32 (x86) Application")
!MESSAGE "DLoader - Win32 Cust Release" (based on "Win32 (x86) Application")
!MESSAGE "DLoader - Win32 Cust Upgrade Debug" (based on "Win32 (x86) Application")
!MESSAGE "DLoader - Win32 Cust Upgrade Release" (based on "Win32 (x86) Application")
!MESSAGE "DLoader - Win32 Cust Factory Debug" (based on "Win32 (x86) Application")
!MESSAGE "DLoader - Win32 Cust Factory Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "DLoader"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DLoader - Win32 Cust Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DLoader___Win32_Cust_Debug"
# PROP BASE Intermediate_Dir "DLoader___Win32_Cust_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DLoader___Win32_Cust_Debug"
# PROP Intermediate_Dir "DLoader___Win32_Cust_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MTd /W4 /Gm /GX /ZI /Od /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MTd /W4 /Gm /GX /ZI /Od /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_RESEARCH" /D "_VC6" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib BMAFramework.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"..\..\..\common\bin/DLoaderR.exe" /pdbtype:sept /libpath:"..\..\..\common\lib"
# ADD LINK32 version.lib ProcessFlow.lib PortHound.lib SprdMesApp.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"..\..\bin\ResearchDownload.exe" /pdbtype:sept /libpath:"..\lib"

!ELSEIF  "$(CFG)" == "DLoader - Win32 Cust Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DLoader___Win32_Cust_Release"
# PROP BASE Intermediate_Dir "DLoader___Win32_Cust_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DLoader___Win32_Cust_Release"
# PROP Intermediate_Dir "DLoader___Win32_Cust_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MT /W4 /GX /O2 /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MT /W4 /GX /O2 /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_RESEARCH" /D "_VC6" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib BMAFramework.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"..\..\..\common\bin/DLoaderR.exe" /libpath:"..\..\..\common\lib"
# ADD LINK32 version.lib ProcessFlow.lib PortHound.lib SprdMesApp.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"..\..\bin\ResearchDownload.exe" /libpath:"..\lib"

!ELSEIF  "$(CFG)" == "DLoader - Win32 Cust Upgrade Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DLoader___Win32_Cust_Upgrade_Debug"
# PROP BASE Intermediate_Dir "DLoader___Win32_Cust_Upgrade_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DLoader___Win32_Cust_Upgrade_Debug"
# PROP Intermediate_Dir "DLoader___Win32_Cust_Upgrade_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MTd /W4 /Gm /GX /ZI /Od /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_SPUPGRADE" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MTd /W4 /Gm /GX /ZI /Od /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_SPUPGRADE" /D "_VC6" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib BMAFrame.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"..\..\bin\UpgradeDownload.exe" /pdbtype:sept /libpath:"..\lib"
# ADD LINK32 version.lib ProcessFlow.lib PortHound.lib SprdMesApp.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"../../../UpgradeDownload/bin/UpgradeDownload.exe" /pdbtype:sept /libpath:"..\lib"

!ELSEIF  "$(CFG)" == "DLoader - Win32 Cust Upgrade Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DLoader___Win32_Cust_Upgrade_Release"
# PROP BASE Intermediate_Dir "DLoader___Win32_Cust_Upgrade_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DLoader___Win32_Cust_Upgrade_Release"
# PROP Intermediate_Dir "DLoader___Win32_Cust_Upgrade_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MT /W4 /GX /O2 /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_SPUPGRADE" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MT /W4 /GX /O2 /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_SPUPGRADE" /D "_VC6" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib BMAFrame.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"..\..\bin\UpgradeDownload.exe" /libpath:"..\lib"
# ADD LINK32 version.lib ProcessFlow.lib PortHound.lib SprdMesApp.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"../../../UpgradeDownload/bin/UpgradeDownload.exe" /libpath:"..\lib"

!ELSEIF  "$(CFG)" == "DLoader - Win32 Cust Factory Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DLoader___Win32_Cust_Factory_Debug"
# PROP BASE Intermediate_Dir "DLoader___Win32_Cust_Factory_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DLoader___Win32_Cust_Factory_Debug"
# PROP Intermediate_Dir "DLoader___Win32_Cust_Factory_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MTd /W4 /Gm /GX /ZI /Od /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_DOWNLOAD_FOR_PRODUCTION" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MTd /W4 /Gm /GX /ZI /Od /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_FACTORY" /D "_VC6" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib BMAFrame.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"..\..\bin\FactoryDownload.exe" /pdbtype:sept /libpath:"..\lib"
# ADD LINK32 version.lib ProcessFlow.lib PortHound.lib SprdMesApp.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"../../../FactoryDownload/bin/FactoryDownload.exe" /pdbtype:sept /libpath:"..\lib"

!ELSEIF  "$(CFG)" == "DLoader - Win32 Cust Factory Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DLoader___Win32_Cust_Factory_Release"
# PROP BASE Intermediate_Dir "DLoader___Win32_Cust_Factory_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DLoader___Win32_Cust_Factory_Release"
# PROP Intermediate_Dir "DLoader___Win32_Cust_Factory_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MT /W4 /GX /O2 /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_DOWNLOAD_FOR_PRODUCTION" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MT /W4 /GX /O2 /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_FACTORY" /D "_VC6" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib BMAFrame.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"..\..\bin\FactoryDownload.exe" /libpath:"..\lib"
# ADD LINK32 version.lib ProcessFlow.lib PortHound.lib SprdMesApp.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"../../../FactoryDownload/bin/FactoryDownload.exe" /libpath:"..\lib"

!ENDIF 

# Begin Target

# Name "DLoader - Win32 Cust Debug"
# Name "DLoader - Win32 Cust Release"
# Name "DLoader - Win32 Cust Upgrade Debug"
# Name "DLoader - Win32 Cust Upgrade Release"
# Name "DLoader - Win32 Cust Factory Debug"
# Name "DLoader - Win32 Cust Factory Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BarcodeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BinPack.cpp
# End Source File
# Begin Source File

SOURCE=.\Calibration.cpp
# End Source File
# Begin Source File

SOURCE=.\CalibrationPage.cpp
# End Source File
# Begin Source File

SOURCE=.\CombCtrlDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolStatusBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\crc16.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\DlgPacketSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPassword.cpp
# End Source File
# Begin Source File

SOURCE=.\DLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\DLoader.rc
# End Source File
# Begin Source File

SOURCE=.\DLoaderDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\DLoaderView.cpp
# End Source File
# Begin Source File

SOURCE=.\EditEx.cpp
# End Source File
# Begin Source File

SOURCE=.\EdtBtnCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\FileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FilterEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\FlashOptPage.cpp
# End Source File
# Begin Source File

SOURCE=.\listeditctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainPage.cpp
# End Source File
# Begin Source File

SOURCE=.\MasterImgGen.cpp
# End Source File
# Begin Source File

SOURCE=.\NumericEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\NvBackupOptList.cpp
# End Source File
# Begin Source File

SOURCE=.\PageLcdConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\PageMcp.cpp
# End Source File
# Begin Source File

SOURCE=.\PageMultiLang.cpp
# End Source File
# Begin Source File

SOURCE=.\PageOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\PageVolFreq.cpp
# End Source File
# Begin Source File

SOURCE=.\phasecheck\PhaseCheckBuild.cpp
# End Source File
# Begin Source File

SOURCE=.\PipeServer.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressCtrlST.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\supergridctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\XAboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\XListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\XRandom.cpp
# End Source File
# Begin Source File

SOURCE=.\XStatic.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BarcodeDlg.h
# End Source File
# Begin Source File

SOURCE=.\BinPack.h
# End Source File
# Begin Source File

SOURCE=.\Calibration.h
# End Source File
# Begin Source File

SOURCE=.\CalibrationPage.h
# End Source File
# Begin Source File

SOURCE=.\CombCtrlDlg.h
# End Source File
# Begin Source File

SOURCE=.\CoolListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\CoolStatic.h
# End Source File
# Begin Source File

SOURCE=.\CoolStatusBar.h
# End Source File
# Begin Source File

SOURCE=.\CoolToolBar.h
# End Source File
# Begin Source File

SOURCE=.\crc16.h
# End Source File
# Begin Source File

SOURCE=.\DlgPacketSetting.h
# End Source File
# Begin Source File

SOURCE=.\DlgPassword.h
# End Source File
# Begin Source File

SOURCE=.\DLoader.h
# End Source File
# Begin Source File

SOURCE=.\DLoaderDoc.h
# End Source File
# Begin Source File

SOURCE=.\DLoaderView.h
# End Source File
# Begin Source File

SOURCE=.\EditEx.h
# End Source File
# Begin Source File

SOURCE=.\EdtBtnCtrl.h
# End Source File
# Begin Source File

SOURCE=.\FileDlg.h
# End Source File
# Begin Source File

SOURCE=.\FilterEdit.h
# End Source File
# Begin Source File

SOURCE=.\FlashOptPage.h
# End Source File
# Begin Source File

SOURCE=.\listeditctrl.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MainPage.h
# End Source File
# Begin Source File

SOURCE=.\MasterImgGen.h
# End Source File
# Begin Source File

SOURCE=.\NumericEdit.h
# End Source File
# Begin Source File

SOURCE=.\NvBackupOptList.h
# End Source File
# Begin Source File

SOURCE=.\PageLcdConfig.h
# End Source File
# Begin Source File

SOURCE=.\PageMcp.h
# End Source File
# Begin Source File

SOURCE=.\PageMultiLang.h
# End Source File
# Begin Source File

SOURCE=.\PageOptions.h
# End Source File
# Begin Source File

SOURCE=.\PageVolFreq.h
# End Source File
# Begin Source File

SOURCE=.\phasecheck\PhaseCheckBuild.h
# End Source File
# Begin Source File

SOURCE=.\PipeServer.h
# End Source File
# Begin Source File

SOURCE=.\ProcessMgr.h
# End Source File
# Begin Source File

SOURCE=.\ProgressCtrlST.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h

!IF  "$(CFG)" == "DLoader - Win32 Cust Debug"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=ResearchDownload
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLoader - Win32 Cust Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=ResearchDownload
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLoader - Win32 Cust Upgrade Debug"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=UpgradeDownload
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLoader - Win32 Cust Upgrade Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=UpgradeDownload
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLoader - Win32 Cust Factory Debug"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=FactoryDownload
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLoader - Win32 Cust Factory Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=FactoryDownload
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SettingsSheet.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\supergridctrl.h
# End Source File
# Begin Source File

SOURCE=.\XAboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\XListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\XRandom.h
# End Source File
# Begin Source File

SOURCE=.\XStatic.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bmp_lang.bmp
# End Source File
# Begin Source File

SOURCE=.\res\DLoader.ico
# End Source File
# Begin Source File

SOURCE=.\res\DLoader.rc2
# End Source File
# Begin Source File

SOURCE=.\res\DLoaderDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\folders.bmp
# End Source File
# Begin Source File

SOURCE=.\res\OK.ico
# End Source File
# Begin Source File

SOURCE=.\res\Pause.ico
# End Source File
# Begin Source File

SOURCE=.\res\ProgBlock.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProgBlockBrown.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProgBlockRed.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Start.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tb_bkg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolbarBmp.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolbarBmp_green.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolbarBmp_yellow.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolbarDis.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolbarDis_green.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolbarDis_yellow.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolbarHot.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolbarHot_green.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolbarHot_yellow.bmp
# End Source File
# Begin Source File

SOURCE=.\res\trademark.bmp
# End Source File
# End Group
# Begin Group "hash"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hash\sha256.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\hash\sha256.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
