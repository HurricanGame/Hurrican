cmake_minimum_required(VERSION 3.9)
project(hurrican)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/3rdparty/" "${CMAKE_SOURCE_DIR}/3rdparty/sdl2-cmake-scripts")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options ("$<$<CONFIG:DEBUG>:-Wall;-Wextra;-pedantic;-Wcast-align;-Wcast-qual;-Wctor-dtor-privacy;-Wdisabled-optimization;-Wformat=2;-Winit-self;-Wmissing-include-dirs;-Wold-style-cast;-Woverloaded-virtual;-Wredundant-decls;-Wshadow;-Wsign-promo;-Wundef;-Wno-unused;-Wno-variadic-macros;-Wno-parentheses;-fdiagnostics-show-option>")
    #set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsingle-precision-constant -fno-rtti")
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT output)
if(result)
    message(STATUS "IPO supported by compiler, enabled for release builds")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
else()
    message(WARNING "IPO is not supported: ${output}")
endif()

set(HURRICAN_SOURCES
        src/bosses/Boss_BigFish.cpp
        src/bosses/Boss_BigFish.hpp
        src/bosses/Boss_Bratklops.cpp
        src/bosses/Boss_Bratklops.hpp
        src/bosses/Boss_Drache.cpp
        src/bosses/Boss_Drache.hpp
        src/bosses/Boss_EisFaust.cpp
        src/bosses/Boss_EisFaust.hpp
        src/bosses/Boss_EvilHurri.cpp
        src/bosses/Boss_EvilHurri.hpp
        src/bosses/Boss_FahrstuhlBoss.cpp
        src/bosses/Boss_FahrstuhlBoss.hpp
        src/bosses/Boss_FlugBoss.cpp
        src/bosses/Boss_FlugBoss.hpp
        src/bosses/Boss_Golem.cpp
        src/bosses/Boss_Golem.hpp
        src/bosses/Boss_MetalHead.cpp
        src/bosses/Boss_MetalHead.hpp
        src/bosses/Boss_PharaoKopf.cpp
        src/bosses/Boss_PharaoKopf.hpp
        src/bosses/Boss_RiesenPiranha.cpp
        src/bosses/Boss_RiesenPiranha.hpp
        src/bosses/Boss_RiesenQualle.cpp
        src/bosses/Boss_RiesenQualle.hpp
        src/bosses/Boss_RiesenRaupe.cpp
        src/bosses/Boss_RiesenRaupe.hpp
        src/bosses/Boss_RiesenSpinne.cpp
        src/bosses/Boss_RiesenSpinne.hpp
        src/bosses/Boss_RiesenWespe.cpp
        src/bosses/Boss_RiesenWespe.hpp
        src/bosses/Boss_Rollmops.cpp
        src/bosses/Boss_Rollmops.hpp
        src/bosses/Boss_SchmidtOrgie.cpp
        src/bosses/Boss_SchmidtOrgie.hpp
        src/bosses/Boss_Schneekoenig.cpp
        src/bosses/Boss_Schneekoenig.hpp
        src/bosses/Boss_Skeletor.cpp
        src/bosses/Boss_Skeletor.hpp
        src/bosses/Boss_Skull.cpp
        src/bosses/Boss_Skull.hpp
        src/bosses/Boss_Spinnenmaschine.cpp
        src/bosses/Boss_Spinnenmaschine.hpp
        src/bosses/Boss_Stahlfaust.cpp
        src/bosses/Boss_Stahlfaust.hpp
        src/bosses/Boss_TheWall.cpp
        src/bosses/Boss_TheWall.hpp
        src/bosses/Boss_Ufo.cpp
        src/bosses/Boss_Ufo.hpp
        src/bosses/Boss_WuxeSpinnen.cpp
        src/bosses/Boss_WuxeSpinnen.hpp
        src/SDLPort/keymap.hpp
        src/SDLPort/opengl.hpp
        src/SDLPort/SDL_fmod.cpp
        src/SDLPort/SDL_fmod.hpp
        src/SDLPort/SDL_port.cpp
        src/SDLPort/SDL_port.hpp
        src/SDLPort/texture.cpp
        src/SDLPort/texture.hpp
        src/CCracktro.cpp
        src/CCracktro.hpp
        src/CDragonHack.cpp
        src/CDragonHack.hpp
        src/Console.cpp
        src/Console.hpp
        src/creditStrings.hpp
        src/DataStructures.hpp
        src/DX8Font.cpp
        src/DX8Font.hpp
        src/DX8Graphics.cpp
        src/DX8Graphics.hpp
        src/DX8Input.cpp
        src/DX8Input.hpp
        src/DX8Joystick.cpp
        src/DX8Joystick.hpp
        src/DX8Sound.cpp
        src/DX8Sound.hpp
        src/DX8Sprite.cpp
        src/DX8Sprite.hpp
        src/DX8Texture.cpp
        src/DX8Texture.hpp
        src/Gameplay.cpp
        src/Gameplay.hpp
        src/enemies/Gegner_Auge.cpp
        src/enemies/Gegner_Auge.hpp
        src/enemies/Gegner_BallerDrone.cpp
        src/enemies/Gegner_BallerDrone.hpp
        src/enemies/Gegner_BigRocket.cpp
        src/enemies/Gegner_BigRocket.hpp
        src/enemies/Gegner_BlueBoulder.cpp
        src/enemies/Gegner_BlueBoulder.hpp
        src/enemies/Gegner_BrockelRock.cpp
        src/enemies/Gegner_BrockelRock.hpp
        src/enemies/Gegner_Climber.cpp
        src/enemies/Gegner_Climber.hpp
        src/enemies/Gegner_DeckenKrabbe.cpp
        src/enemies/Gegner_DeckenKrabbe.hpp
        src/enemies/Gegner_Deckenturm.cpp
        src/enemies/Gegner_Deckenturm.hpp
        src/enemies/Gegner_Diamant.cpp
        src/enemies/Gegner_Diamant.hpp
        src/enemies/Gegner_Drone.cpp
        src/enemies/Gegner_Drone.hpp
        src/enemies/Gegner_EierMann.cpp
        src/enemies/Gegner_EierMann.hpp
        src/enemies/Gegner_EisStachel.cpp
        src/enemies/Gegner_EisStachel.hpp
        src/enemies/Gegner_Eiszapfen.cpp
        src/enemies/Gegner_Eiszapfen.hpp
        src/enemies/Gegner_Extras.cpp
        src/enemies/Gegner_Extras.hpp
        src/enemies/Gegner_FallBombe.cpp
        src/enemies/Gegner_FallBombe.hpp
        src/enemies/Gegner_FallingRock.cpp
        src/enemies/Gegner_FallingRock.hpp
        src/enemies/Gegner_FetteRakete.cpp
        src/enemies/Gegner_FetteRakete.hpp
        src/enemies/Gegner_FetteSpinne.cpp
        src/enemies/Gegner_FetteSpinne.hpp
        src/enemies/Gegner_FieseDrone.cpp
        src/enemies/Gegner_FieseDrone.hpp
        src/enemies/Gegner_FieserFireWalker.cpp
        src/enemies/Gegner_FieserFireWalker.hpp
        src/enemies/Gegner_FieserWalker.cpp
        src/enemies/Gegner_FieserWalker.hpp
        src/enemies/Gegner_FireSpider.cpp
        src/enemies/Gegner_FireSpider.hpp
        src/enemies/Gegner_Fledermaus.cpp
        src/enemies/Gegner_Fledermaus.hpp
        src/enemies/Gegner_FlugKanone.cpp
        src/enemies/Gegner_FlugKanone.hpp
        src/enemies/Gegner_Flugsack.cpp
        src/enemies/Gegner_Flugsack.hpp
        src/enemies/Gegner_Geschuetz.cpp
        src/enemies/Gegner_Geschuetz.hpp
        src/enemies/Gegner_GunTower.cpp
        src/enemies/Gegner_GunTower.hpp
        src/Gegner_Helper.cpp
        src/Gegner_Helper.hpp
        src/enemies/Gegner_Jaeger.cpp
        src/enemies/Gegner_Jaeger.hpp
        src/enemies/Gegner_Kettenglied.cpp
        src/enemies/Gegner_Kettenglied.hpp
        src/enemies/Gegner_KrabblerLinks.cpp
        src/enemies/Gegner_KrabblerLinks.hpp
        src/enemies/Gegner_KrabblerOben.cpp
        src/enemies/Gegner_KrabblerOben.hpp
        src/enemies/Gegner_KrabblerRechts.cpp
        src/enemies/Gegner_KrabblerRechts.hpp
        src/enemies/Gegner_KugelGross.cpp
        src/enemies/Gegner_KugelGross.hpp
        src/enemies/Gegner_KugelKlein.cpp
        src/enemies/Gegner_KugelKlein.hpp
        src/enemies/Gegner_KugelMedium.cpp
        src/enemies/Gegner_KugelMedium.hpp
        src/enemies/Gegner_KugelRiesig.cpp
        src/enemies/Gegner_KugelRiesig.hpp
        src/enemies/Gegner_LaFass.cpp
        src/enemies/Gegner_LaFass.hpp
        src/enemies/Gegner_LavaBall.cpp
        src/enemies/Gegner_LavaBall.hpp
        src/enemies/Gegner_LavaBallSpawner.cpp
        src/enemies/Gegner_LavaBallSpawner.hpp
        src/enemies/Gegner_LavaKrabbe.cpp
        src/enemies/Gegner_LavaKrabbe.hpp
        src/enemies/Gegner_Lavamann.cpp
        src/enemies/Gegner_Lavamann.hpp
        src/enemies/Gegner_Made.cpp
        src/enemies/Gegner_Made.hpp
        src/enemies/Gegner_Minidragon.cpp
        src/enemies/Gegner_Minidragon.hpp
        src/enemies/Gegner_MiniRocket.cpp
        src/enemies/Gegner_MiniRocket.hpp
        src/enemies/Gegner_MittelSpinne.cpp
        src/enemies/Gegner_MittelSpinne.hpp
        src/enemies/Gegner_Mutant.cpp
        src/enemies/Gegner_Mutant.hpp
        src/enemies/Gegner_Nest.cpp
        src/enemies/Gegner_Nest.hpp
        src/enemies/Gegner_NeuFisch.cpp
        src/enemies/Gegner_NeuFisch.hpp
        src/enemies/Gegner_OneUp.cpp
        src/enemies/Gegner_OneUp.hpp
        src/enemies/Gegner_PFlanze.cpp
        src/enemies/Gegner_Pflanze.hpp
        src/enemies/Gegner_Piranha.cpp
        src/enemies/Gegner_Piranha.hpp
        src/enemies/Gegner_PokeNuke.cpp
        src/enemies/Gegner_PokeNuke.hpp
        src/enemies/Gegner_PowerBlock.cpp
        src/enemies/Gegner_PowerBlock.hpp
        src/enemies/Gegner_Punisher.cpp
        src/enemies/Gegner_Punisher.hpp
        src/enemies/Gegner_Qualle.cpp
        src/enemies/Gegner_Qualle.hpp
        src/enemies/Gegner_Raketenwerfer.cpp
        src/enemies/Gegner_Raketenwerfer.hpp
        src/enemies/Gegner_ReitFlugsack.cpp
        src/enemies/Gegner_ReitFlugsack.hpp
        src/enemies/Gegner_RoboMan1.cpp
        src/enemies/Gegner_RoboMan1.hpp
        src/enemies/Gegner_RoboRaupe.cpp
        src/enemies/Gegner_RoboRaupe.hpp
        src/enemies/Gegner_Rotzpott.cpp
        src/enemies/Gegner_Rotzpott.hpp
        src/enemies/Gegner_Schabe.cpp
        src/enemies/Gegner_Schabe.hpp
        src/enemies/Gegner_SchienenViech.cpp
        src/enemies/Gegner_SchienenViech.hpp
        src/enemies/Gegner_SchleimAlien.cpp
        src/enemies/Gegner_SchleimAlien.hpp
        src/enemies/Gegner_SchleimBoller.cpp
        src/enemies/Gegner_SchleimBoller.hpp
        src/enemies/Gegner_SchleimMaul.cpp
        src/enemies/Gegner_SchleimMaul.hpp
        src/enemies/Gegner_Schneekoppe.cpp
        src/enemies/Gegner_Schneekoppe.hpp
        src/enemies/Gegner_Schwabbel.cpp
        src/enemies/Gegner_Schwabbel.hpp
        src/enemies/Gegner_SideRocket.cpp
        src/enemies/Gegner_SideRocket.hpp
        src/enemies/Gegner_SkiWalker.cpp
        src/enemies/Gegner_SkiWalker.hpp
        src/enemies/Gegner_SmallWespe.cpp
        src/enemies/Gegner_SmallWespe.hpp
        src/enemies/Gegner_SnowBomb.cpp
        src/enemies/Gegner_SnowBomb.hpp
        src/enemies/Gegner_SpiderBomb.cpp
        src/enemies/Gegner_SpiderBomb.hpp
        src/enemies/Gegner_Spitter.cpp
        src/enemies/Gegner_Spitter.hpp
        src/enemies/Gegner_Spitterbombe.cpp
        src/enemies/Gegner_Spitterbombe.hpp
        src/enemies/Gegner_Stachelbeere.cpp
        src/enemies/Gegner_Stachelbeere.hpp
        src/enemies/Gegner_Stahlmuecke.cpp
        src/enemies/Gegner_Stahlmuecke.hpp
        src/enemies/Gegner_Stalagtit.cpp
        src/enemies/Gegner_Stalagtit.hpp
        src/enemies/Gegner_StarBig.cpp
        src/enemies/Gegner_StarBig.hpp
        src/enemies/Gegner_StarSmall.cpp
        src/enemies/Gegner_StarSmall.hpp
        src/enemies/Gegner_StelzSack.cpp
        src/enemies/Gegner_StelzSack.hpp
        src/enemies/Gegner_Stuff.hpp
        src/enemies/Gegner_SwimWalker.cpp
        src/enemies/Gegner_SwimWalker.hpp
        src/enemies/Gegner_Walker.cpp
        src/enemies/Gegner_Walker.hpp
        src/enemies/Gegner_WandKannone.cpp
        src/enemies/Gegner_WandKannone.hpp
        src/enemies/Gegner_WandKrabbe.cpp
        src/enemies/Gegner_WandKrabbe.hpp
        src/enemies/Gegner_WasserMine.cpp
        src/enemies/Gegner_WasserMine.hpp
        src/enemies/Gegner_Zitrone.cpp
        src/enemies/Gegner_Zitrone.hpp
        src/GegnerClass.cpp
        src/GegnerClass.hpp
        src/GetKeyName.cpp
        src/GetKeyName.hpp
        src/Globals.hpp
        src/GUISystem.cpp
        src/GUISystem.hpp
        src/HUD.cpp
        src/HUD.hpp
        src/Intro.cpp
        src/Intro.hpp
        src/Logdatei.cpp
        src/Logdatei.hpp
        src/Main.cpp
        src/Main.hpp
        src/Mathematics.cpp
        src/Mathematics.hpp
        src/MemLeaks.hpp
        src/Menu.cpp
        src/Menu.hpp
        src/Outtro.cpp
        src/Outtro.hpp
        src/Partikelsystem.cpp
        src/Partikelsystem.hpp
        src/Player.cpp
        src/Player.hpp
        src/Projectiles.cpp
        src/Projectiles.hpp
        src/stdafx.hpp
        src/Texts.cpp
        src/Texts.hpp
        src/Tileengine.cpp
        src/Tileengine.hpp
        src/Timer.cpp
        src/Timer.hpp
        src/tracealloc.cpp
        src/triggers/Trigger_Bruecke.cpp
        src/triggers/Trigger_Bruecke.hpp
        src/triggers/Trigger_Column.cpp
        src/triggers/Trigger_Column.hpp
        src/triggers/Trigger_Column2.cpp
        src/triggers/Trigger_Column2.hpp
        src/triggers/Trigger_EndLevel.cpp
        src/triggers/Trigger_EndLevel.hpp
        src/triggers/Trigger_FadeMusic.cpp
        src/triggers/Trigger_FadeMusic.hpp
        src/triggers/Trigger_FahrStuhl.cpp
        src/triggers/Trigger_Fahrstuhl.hpp
        src/triggers/Trigger_Fass.cpp
        src/triggers/Trigger_Fass.hpp
        src/triggers/Trigger_Feuerspucker.cpp
        src/triggers/Trigger_Feuerspucker.hpp
        src/triggers/Trigger_Floating.cpp
        src/triggers/Trigger_Floating.hpp
        src/triggers/Trigger_Floating2.cpp
        src/triggers/Trigger_Floating2.hpp
        src/triggers/Trigger_Floating3.cpp
        src/triggers/Trigger_Floating3.hpp
        src/triggers/Trigger_Glubschi.cpp
        src/triggers/Trigger_Glubschi.hpp
        src/triggers/Trigger_Glubschi2.cpp
        src/triggers/Trigger_Glubschi2.hpp
        src/triggers/Trigger_LaFassSpawner.cpp
        src/triggers/Trigger_LaFassSpawner.hpp
        src/triggers/Trigger_Lift.cpp
        src/triggers/Trigger_Lift.hpp
        src/triggers/Trigger_LightFlare.cpp
        src/triggers/Trigger_LightFlare.hpp
        src/triggers/Trigger_LuefterGross.cpp
        src/triggers/Trigger_LuefterGross.hpp
        src/triggers/Trigger_LuefterKlein.cpp
        src/triggers/Trigger_LuefterKlein.hpp
        src/triggers/Trigger_LuefterKlein2.cpp
        src/triggers/Trigger_LuefterKlein2.hpp
        src/triggers/Trigger_Mushroom.cpp
        src/triggers/Trigger_Mushroom.hpp
        src/triggers/Trigger_PartikelSpawner.cpp
        src/triggers/Trigger_PartikelSpawner.hpp
        src/triggers/Trigger_Plattform.cpp
        src/triggers/Trigger_Plattform.hpp
        src/triggers/Trigger_Presse.cpp
        src/triggers/Trigger_Presse.hpp
        src/triggers/Trigger_Presswurst.cpp
        src/triggers/Trigger_Presswurst.hpp
        src/triggers/Trigger_SchleuseH.cpp
        src/triggers/Trigger_SchleuseH.hpp
        src/triggers/Trigger_SchleuseV.cpp
        src/triggers/Trigger_SchleuseV.hpp
        src/triggers/Trigger_Secret.cpp
        src/triggers/Trigger_Secret.hpp
        src/triggers/Trigger_ShootButton.cpp
        src/triggers/Trigger_ShootButton.hpp
        src/triggers/Trigger_ShootPlattform.cpp
        src/triggers/Trigger_ShootPlattform.hpp
        src/triggers/Trigger_Shrine.cpp
        src/triggers/Trigger_Shrine.hpp
        src/triggers/Trigger_SoundTrigger.cpp
        src/triggers/Trigger_SoundTrigger.hpp
        src/triggers/Trigger_Spikelift.cpp
        src/triggers/Trigger_Spikelift.hpp
        src/triggers/Trigger_Stampfstein.cpp
        src/triggers/Trigger_Stampfstein.hpp
        src/triggers/Trigger_SurfBrett.cpp
        src/triggers/Trigger_SurfBrett.hpp
        src/triggers/Trigger_Switch.cpp
        src/triggers/Trigger_Switch.hpp
        src/triggers/Trigger_Tube.cpp
        src/triggers/Trigger_Tube.hpp
        src/triggers/Trigger_TutorialText.cpp
        src/triggers/Trigger_TutorialText.hpp
        src/triggers/Trigger_Warning.cpp
        src/triggers/Trigger_Warning.hpp)

if(MINGW)
    list(APPEND HURRICAN_SOURCES
        hurrican.rc
    )
endif(MINGW)

# Default to OpenGL 2
if (NOT RENDERER)
    set(RENDERER "GL2")
endif()

if (RENDERER STREQUAL "GL1")
    add_definitions(-DUSE_GL1)
    set(gl2 OFF)
elseif(RENDERER STREQUAL "GLES1")
    add_definitions(-DUSE_GL1)
    add_definitions(-DUSE_GLES1)
    set(gl2 OFF)
elseif(RENDERER STREQUAL "GL2")
    add_definitions(-DUSE_GL2)
    set(gl2 ON)
elseif(RENDERER STREQUAL "GLES2")
    add_definitions(-DUSE_GL2)
    add_definitions(-DUSE_GLES2)
    set(gl2 ON)
elseif(RENDERER STREQUAL "GL3")
    add_definitions(-DUSE_GL3)
    set(gl2 ON)
elseif(RENDERER STREQUAL "GLES3")
    add_definitions(-DUSE_GL3)
    add_definitions(-DUSE_GLES3)
    set(gl2 ON)
endif()

if(${gl2})
    list(APPEND HURRICAN_SOURCES
        src/SDLPort/cshader.cpp
        src/SDLPort/cshader.hpp
    )
endif()

include(CMakeDependentOption)

CMAKE_DEPENDENT_OPTION(FBO "FBO support (allow screen to be scaled to arbitrary dimensions)" ON ${gl2} OFF)
IF(FBO)
    ADD_DEFINITIONS(-DUSE_FBO)
    list(APPEND HURRICAN_SOURCES
        src/SDLPort/cfbo.cpp
        src/SDLPort/cfbo.hpp
    )
ENDIF(FBO)

add_definitions(-DPLATFORM_SDL)
add_definitions(-DENABLE_CONSOLE_COMMANDS) # make optional?

# TODO add options
# C++ exceptions can be turned off, reducing unnecessary code bloat and speed-hit
# add_definitions(-DUSE_NO_EXCEPTIONS)
# Bypass pooled memory manager (for debugging purposes)
# add_definitions(-DUSE_NO_MEMPOOLING)
# Use the Unrarlib library, for support of packing of data files
# add_definitions(-DUSE_UNRARLIB)
# ETC1 texture compression support
# add_definitions(-DUSE_ETC1)
# PVRTC texture compression suppor
# add_definitions(-DUSE_PVRTC)

# Fast random-number-generator (can be over 100 times faster than rand() on some platforms)
OPTION(FAST_RANDOM "Use a fast linear congruential generator for random numbers" OFF)
IF(FAST_RANDOM)
    ADD_DEFINITIONS(-DUSE_FAST_RNG)
ENDIF(FAST_RANDOM)

# Fast trig approximations (For systems with CPUS lacking trig instructions i.e. ARM/MIPS)
OPTION(FAST_TRIG "Use fast approximation for trigonometric functions" OFF)
IF(FAST_TRIG)
    ADD_DEFINITIONS(-DUSE_FAST_TRIG)
ENDIF(FAST_TRIG)

if(UNIX)
    add_definitions(-DUSE_HOME_DIR)
    add_definitions(-DUSE_STORAGE_PATH="${CMAKE_INSTALL_PREFIX}/share/hurrican/")
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/SDLPort)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/glm)
add_executable(${PROJECT_NAME} ${HURRICAN_SOURCES})

OPTION(OPENMPT "Use OPENMPT for ImpulseTracker music file decoding" OFF)
IF(OPENMPT)
    ADD_DEFINITIONS(-DUSE_OPENMPT)
    target_link_libraries(${PROJECT_NAME} openmpt)
ENDIF(OPENMPT)

set(SDL2_BUILDING_LIBRARY TRUE)
find_package(SDL2)
if (SDL2_FOUND)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
else(SDL2_FOUND)
    set(SDL_BUILDING_LIBRARY TRUE)
    find_package(SDL REQUIRED)
    INCLUDE(FindSDL_mixer REQUIRED)
    INCLUDE(FindSDL_image REQUIRED)
endif(SDL2_FOUND)

find_package(LibEpoxy 1.2 REQUIRED)

include_directories(${LibEpoxy_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${LibEpoxy_LIBRARIES})

if (SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIR}
                        ${SDL2_IMAGE_INCLUDE_DIR}
                        ${SDL2_MIXER_INCLUDE_DIR})

    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
else(SDL2_FOUND)
    include_directories(${SDL_INCLUDE_DIR}
                        ${SDL_IMAGE_INCLUDE_DIR}
                        ${SDL_MIXER_INCLUDE_DIR})

    target_link_libraries(${PROJECT_NAME} ${SDL_LIBRARY} ${SDL_MIXER_LIBRARIES} ${SDL_IMAGE_LIBRARIES})
endif(SDL2_FOUND)

if((UNIX OR MINGW) AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} stdc++fs)
endif()

if(UNIX)
    install(PROGRAMS  ${CMAKE_BINARY_DIR}/hurrican   DESTINATION bin/)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/      DESTINATION share/hurrican/data)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/lang/      DESTINATION share/hurrican/lang)
    install(FILES     ${CMAKE_SOURCE_DIR}/readme.txt DESTINATION share/doc/hurrican/)
else()
    install(PROGRAMS  ${CMAKE_BINARY_DIR}/hurrican   DESTINATION ./)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/      DESTINATION data)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/lang/      DESTINATION lang)
    install(FILES     ${CMAKE_SOURCE_DIR}/readme.txt DESTINATION ./)
endif()
