# Microsoft Developer Studio Project File - Name="Hurrican" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Hurrican - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "Hurrican.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Hurrican.mak" CFG="Hurrican - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "Hurrican - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "Hurrican - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Hurrican - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /WX /GX /O2 /I "d:/APIS/MemLeaks/" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 oleaut32.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib dinput8.lib winmm.lib fmodvc.lib kernel32.lib advapi32.lib odbc32.lib gdi32.lib user32.lib /nologo /subsystem:windows /machine:I386 /out:"Hurrican.exe"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "Hurrican - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /WX /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "DETECT_LEAKS" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Dxerr8.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib dinput8.lib winmm.lib fmodvc.lib kernel32.lib advapi32.lib odbc32.lib gdi32.lib user32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Hurrican.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Hurrican - Win32 Release"
# Name "Hurrican - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Gegner"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CDragonHack.cpp
# End Source File
# Begin Source File

SOURCE=.\Gegner_Auge.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_BallerDrone.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_BigRocket.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_BlueBoulder.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_BrockelRock.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Climber.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_DeckenKrabbe.cpp
# End Source File
# Begin Source File

SOURCE=.\Gegner_Deckenturm.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Diamant.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Drone.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_EierMann.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_EisStachel.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Eiszapfen.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Extras.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_FallBombe.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_FallingRock.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_FetteRakete.cpp
# End Source File
# Begin Source File

SOURCE=.\Gegner_FetteSpinne.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_FieseDrone.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_FieserFireWalker.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_FieserWalker.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_FireSpider.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Fledermaus.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_FlugKanone.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Flugsack.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Geschuetz.cpp
# End Source File
# Begin Source File

SOURCE=.\Gegner_GunTower.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Jaeger.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Kettenglied.cpp
# End Source File
# Begin Source File

SOURCE=.\Gegner_KrabblerLinks.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_KrabblerOben.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_KrabblerRechts.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_KugelGross.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_KugelKlein.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_KugelMedium.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_KugelRiesig.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_LaFass.cpp
# End Source File
# Begin Source File

SOURCE=.\Gegner_LavaBall.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_LavaBallSpawner.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_LavaKrabbe.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Lavamann.cpp
# End Source File
# Begin Source File

SOURCE=.\Gegner_Made.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Minidragon.cpp
# End Source File
# Begin Source File

SOURCE=.\Gegner_MiniRocket.cpp
# End Source File
# Begin Source File

SOURCE=.\Gegner_MittelSpinne.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Mutant.cpp
# End Source File
# Begin Source File

SOURCE=.\Gegner_Nest.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_NeuFisch.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_OneUp.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_PFlanze.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Piranha.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_PokeNuke.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_PowerBlock.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Punisher.cpp
# End Source File
# Begin Source File

SOURCE=.\Gegner_Qualle.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Raketenwerfer.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_ReitFlugsack.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_RoboMan1.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_RoboRaupe.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Rotzpott.cpp
# End Source File
# Begin Source File

SOURCE=.\Gegner_Schabe.cpp
# End Source File
# Begin Source File

SOURCE=.\Gegner_SchienenViech.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_SchleimAlien.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_SchleimBoller.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_SchleimMaul.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Schneekoppe.cpp
# End Source File
# Begin Source File

SOURCE=.\Gegner_Schwabbel.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_SideRocket.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_SkiWalker.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_SmallWespe.cpp
# End Source File
# Begin Source File

SOURCE=.\Gegner_SnowBomb.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_SpiderBomb.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Spitter.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Spitterbombe.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Stachelbeere.cpp
# End Source File
# Begin Source File

SOURCE=.\Gegner_Stahlmuecke.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Stalagtit.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_StarBig.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_StarSmall.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_StelzSack.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_SwimWalker.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Walker.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_WandKannone.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_WandKrabbe.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_WasserMine.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Gegner_Zitrone.cpp
# End Source File
# End Group
# Begin Group "DirectX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DX8Font.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\DX8Graphics.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\DX8Input.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\DX8Joystick.cpp
# End Source File
# Begin Source File

SOURCE=.\DX8Sound.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\DX8Sprite.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# End Group
# Begin Group "Engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CCracktro.cpp
# End Source File
# Begin Source File

SOURCE=.\CCutScene.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\Console.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\Gameplay.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\Gegner_Helper.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\GegnerClass.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\GetKeyName.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\GUISystem.cpp
# End Source File
# Begin Source File

SOURCE=.\HUD.CPP
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\Intro.cpp
# End Source File
# Begin Source File

SOURCE=.\lightmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Logdatei.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\Menu.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\Outtro.cpp
# End Source File
# Begin Source File

SOURCE=.\Partikelsystem.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\Player.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\Projectiles.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\Texts.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\Tileengine.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\Timer.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\tracealloc.cpp
# End Source File
# Begin Source File

SOURCE="..\..\..\..\APIs\unrar lib 0.4.0\unrarlib\unrarlib.c"
# SUBTRACT CPP /YX /Yc
# End Source File
# End Group
# Begin Group "Boss"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Boss_BigFish.cpp
# End Source File
# Begin Source File

SOURCE=.\Boss_Bratklops.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Boss_Drache.cpp
# End Source File
# Begin Source File

SOURCE=.\Boss_EisFaust.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Boss_EvilHurri.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Boss_FahrstuhlBoss.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Boss_FlugBoss.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Boss_Golem.cpp
# End Source File
# Begin Source File

SOURCE=.\Boss_MetalHead.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Boss_PharaoKopf.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Boss_RiesenPiranha.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Boss_RiesenQualle.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Boss_RiesenRaupe.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Boss_RiesenSpinne.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Boss_RiesenWespe.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Boss_Rollmops.cpp
# End Source File
# Begin Source File

SOURCE=.\Boss_SchmidtOrgie.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Boss_Schneekoenig.cpp
# End Source File
# Begin Source File

SOURCE=.\Boss_Skeletor.cpp
# End Source File
# Begin Source File

SOURCE=.\Boss_Skull.cpp
# End Source File
# Begin Source File

SOURCE=.\Boss_Spinnenmaschine.cpp
# End Source File
# Begin Source File

SOURCE=.\Boss_Stahlfaust.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Boss_TheWall.cpp
# End Source File
# Begin Source File

SOURCE=.\Boss_Ufo.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Boss_WuxeSpinnen.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# End Group
# Begin Group "Trigger"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Trigger_Bruecke.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Column.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Column2.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_EndLevel.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_FadeMusic.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_FahrStuhl.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Fass.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Feuerspucker.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Floating.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Floating2.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Floating3.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Glubschi.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Glubschi2.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_LaFassSpawner.cpp
# End Source File
# Begin Source File

SOURCE=.\Trigger_Lift.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_LightFlare.cpp
# End Source File
# Begin Source File

SOURCE=.\Trigger_LuefterGross.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_LuefterKlein.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_LuefterKlein2.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Mushroom.cpp
# End Source File
# Begin Source File

SOURCE=.\Trigger_PartikelSpawner.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Plattform.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Presse.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Presswurst.cpp
# End Source File
# Begin Source File

SOURCE=.\Trigger_SchleuseH.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_SchleuseV.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Secret.cpp
# End Source File
# Begin Source File

SOURCE=.\Trigger_ShootButton.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_ShootPlattform.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Shrine.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_SoundTrigger.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Spikelift.cpp
# End Source File
# Begin Source File

SOURCE=.\Trigger_Stampfstein.cpp
# End Source File
# Begin Source File

SOURCE=.\Trigger_SurfBrett.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Switch.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Tube.cpp
# End Source File
# Begin Source File

SOURCE=.\Trigger_TutorialText.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trigger_Warning.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# End Group
# Begin Source File

SOURCE=.\Main.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\Resource.rc
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Gegner.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CDragonHack.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Auge.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_BallerDrone.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_BigRocket.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_BlueBoulder.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_BrockelRock.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Climber.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_DeckenKrabbe.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Deckenturm.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Diamant.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Drone.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_EierMann.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_EisStachel.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Eiszapfen.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Extras.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_FallBombe.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_FallingRock.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_FetteRakete.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_FetteSpinne.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_FieseDrone.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_FieserFireWalker.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_FieserWalker.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_FireSpider.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Fledermaus.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_FlugKanone.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Flugsack.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Geschuetz.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_GunTower.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Jaeger.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Kettenglied.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_KrabblerLinks.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_KrabblerOben.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_KrabblerRechts.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_KugelGross.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_KugelKlein.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_KugelMedium.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_KugelRiesig.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_LaFass.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_LavaBall.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_LavaBallSpawner.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_LavaKrabbe.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Lavamann.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Made.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Minidragon.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_MiniRocket.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_MittelSpinne.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Mutant.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Nest.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_NeuFisch.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_OneUp.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Pflanze.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Piranha.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_PokeNuke.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_PowerBlock.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Punisher.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Qualle.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Raketenwerfer.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_ReitFlugsack.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_RoboMan1.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_RoboRaupe.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Rotzpott.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Schabe.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_SchienenViech.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_SchleimAlien.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_SchleimBoller.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_SchleimMaul.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Schneekoppe.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Schwabbel.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_SideRocket.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_SkiWalker.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_SmallWespe.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_SnowBomb.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_SpiderBomb.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Spitter.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Spitterbombe.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Stachelbeere.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Stahlmuecke.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Stalagtit.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_StarBig.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_StarSmall.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_StelzSack.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_SwimWalker.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Walker.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_WandKannone.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_WandKrabbe.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_WasserMine.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Zitrone.h
# End Source File
# End Group
# Begin Group "DirectX.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DX8Font.h
# End Source File
# Begin Source File

SOURCE=.\DX8Graphics.h
# End Source File
# Begin Source File

SOURCE=.\DX8Input.h
# End Source File
# Begin Source File

SOURCE=.\DX8Joystick.h
# End Source File
# Begin Source File

SOURCE=.\DX8Sound.h
# End Source File
# Begin Source File

SOURCE=.\DX8Sprite.h
# End Source File
# End Group
# Begin Group "Engine.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CCracktro.h
# End Source File
# Begin Source File

SOURCE=.\CCutScene.h
# End Source File
# Begin Source File

SOURCE=.\Console.h
# End Source File
# Begin Source File

SOURCE=.\Gameplay.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Helper.h
# End Source File
# Begin Source File

SOURCE=.\Gegner_Stuff.h
# End Source File
# Begin Source File

SOURCE=.\GegnerClass.h
# End Source File
# Begin Source File

SOURCE=.\GetKeyName.h
# End Source File
# Begin Source File

SOURCE=.\GUISystem.h
# End Source File
# Begin Source File

SOURCE=.\HUD.H
# End Source File
# Begin Source File

SOURCE=.\Intro.h
# End Source File
# Begin Source File

SOURCE=.\lightmap.h
# End Source File
# Begin Source File

SOURCE=.\Logdatei.h
# End Source File
# Begin Source File

SOURCE=.\Menu.h
# End Source File
# Begin Source File

SOURCE=.\mymath.h
# End Source File
# Begin Source File

SOURCE=.\Outtro.h
# End Source File
# Begin Source File

SOURCE=.\Partikelsystem.h
# End Source File
# Begin Source File

SOURCE=.\Player.h
# End Source File
# Begin Source File

SOURCE=.\Projectiles.h
# End Source File
# Begin Source File

SOURCE=.\Texts.h
# End Source File
# Begin Source File

SOURCE=.\Tileengine.h
# End Source File
# Begin Source File

SOURCE=.\Timer.h
# End Source File
# Begin Source File

SOURCE="..\..\..\..\APIs\unrar lib 0.4.0\unrarlib\unrarlib.h"
# End Source File
# End Group
# Begin Group "Boss.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Boss_BigFish.h
# End Source File
# Begin Source File

SOURCE=.\Boss_Bratklops.h
# End Source File
# Begin Source File

SOURCE=.\Boss_Drache.h
# End Source File
# Begin Source File

SOURCE=.\Boss_EisFaust.h
# End Source File
# Begin Source File

SOURCE=.\Boss_EvilHurri.h
# End Source File
# Begin Source File

SOURCE=.\Boss_FahrstuhlBoss.h
# End Source File
# Begin Source File

SOURCE=.\Boss_FlugBoss.h
# End Source File
# Begin Source File

SOURCE=.\Boss_Golem.h
# End Source File
# Begin Source File

SOURCE=.\Boss_MetalHead.h
# End Source File
# Begin Source File

SOURCE=.\Boss_PharaoKopf.h
# End Source File
# Begin Source File

SOURCE=.\Boss_RiesenPiranha.h
# End Source File
# Begin Source File

SOURCE=.\Boss_RiesenQualle.h
# End Source File
# Begin Source File

SOURCE=.\Boss_RiesenRaupe.h
# End Source File
# Begin Source File

SOURCE=.\Boss_RiesenSpinne.h
# End Source File
# Begin Source File

SOURCE=.\Boss_RiesenWespe.h
# End Source File
# Begin Source File

SOURCE=.\Boss_Rollmops.h
# End Source File
# Begin Source File

SOURCE=.\Boss_SchmidtOrgie.h
# End Source File
# Begin Source File

SOURCE=.\Boss_Schneekoenig.h
# End Source File
# Begin Source File

SOURCE=.\Boss_Skeletor.h
# End Source File
# Begin Source File

SOURCE=.\Boss_Skull.h
# End Source File
# Begin Source File

SOURCE=.\Boss_Spinnenmaschine.h
# End Source File
# Begin Source File

SOURCE=.\Boss_Stahlfaust.h
# End Source File
# Begin Source File

SOURCE=.\Boss_TheWall.h
# End Source File
# Begin Source File

SOURCE=.\Boss_Ufo.h
# End Source File
# Begin Source File

SOURCE=.\Boss_WuxeSpinnen.h
# End Source File
# End Group
# Begin Group "Trigger.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Trigger_Bruecke.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Column.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Column2.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_EndLevel.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_FadeMusic.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Fahrstuhl.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Fass.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Feuerspucker.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Floating.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Floating2.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Floating3.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Glubschi.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Glubschi2.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_LaFassSpawner.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Lift.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_LightFlare.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_LuefterGross.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_LuefterKlein.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_LuefterKlein2.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Mushroom.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_PartikelSpawner.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Plattform.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Presse.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Presswurst.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_SchleuseH.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_SchleuseV.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Secret.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_ShootButton.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_ShootPlattform.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Shrine.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_SoundTrigger.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Spikelift.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Stampfstein.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_SurfBrett.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Switch.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Tube.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_TutorialText.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_Warning.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Globals.h
# End Source File
# Begin Source File

SOURCE=.\Main.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Hurrican.ico
# End Source File
# Begin Source File

SOURCE=.\splashscreen.bmp
# End Source File
# End Group
# End Target
# End Project
