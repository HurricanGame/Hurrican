##### HOW TO BUILD:
##    Run 'make TARGET=xxx' where xxx is one of the following:
##    (default is linux_GL1 if TARGET= is not specified)

##### Pre-configured build targets (default is linux_GL1)
# linux_GL1   : Linux (OpenGL1)
# linux_GL2   : Linux (OpenGLES2)
# aros        : AROS Research Operating System (OpenGL1)
# rpi         : Raspberry PI (OpenGLES2)
# gcw         : GCW Zero handheld (OpenGLES2)
# rdu         : ??? (TODO: Learn from Pickle which platform this represents)
# pandora     : Pandora handheld (OpenGLES2)

ifndef TARGET
TARGET := linux_GL1
endif


DEFINES := -DPLATFORM_SDL -DENABLE_CONSOLE_COMMANDS -DUSE_MODPLUG

#####################################
#######  *** OPTIONAL DEFINES: ***  #
#####################################

####### When batching draw calls, use indexed triangle lists instead of
###     degenerate triangle strips (RECOMMENDED; fastest on most platforms.)
DEFINES += -DUSE_INDEXED_BATCH

####### Use the $HOME dir as a location for any writing (settings, high-scores, log, save-games):
#DEFINES += -DUSE_HOME_DIR

####### Specify a storage location for the read-only data files (graphics, sound, languages, etc) 
#DEFINES += -DUSE_STORAGE_PATH="\"PATH_TO_STORAGE\""

####### Simple and fast pooled memory manager that eliminates new/delete/malloc/free overhead
#DEFINES += -DUSE_MEMPOOL

####### Fast random-number-generator (can be over 100 times faster than rand() on some platforms):
#DEFINES += -DUSE_FAST_RNG

####### Fast Sin/Cos trig lookup table (can be over 15 times faster than libm on some platforms):
#DEFINES += -DUSE_TRIG_LOOKUP_TABLE

####### ETC1 texture compression support:
#DEFINES += -DUSE_ETC1

####### PVRTC texture compression support:
#DEFINES += -DUSE_PVRTC

####### FBO support (allow screen to be scaled to arbitrary dimensions)
#USE_FBO += yes

####### Use GCC compiler's link-time-optimization:
#USE_FLTO += yes

####### C++ exceptions can be turned off, reducing unnecessary code bloat and speed-hit:
#USE_NO_EXCEPTIONS := yes

####### Use the Unrarlib library, for support of packing of data files:
#USE_UNRARLIB := yes

# *** END OPTIONAL DEFINES ***


ifeq ($(TARGET),aros)

EXE := ../hurricanaros
CXX := i386-aros-g++
CC  := i386-aros-gcc
LD  := i386-aros-g++
RM  := rm -f

USE_EGL := no

INCLUDES_OS := -I/usr/local/arosv0/AROS/bin/linux-i386/AROS/Development/include/SDL/ \
               -I/usr/local/arosv0/AROS/bin/linux-i386/AROS/Development/include/libmodplug

#LDFLAGS := -g -lSDL_mixer -lvorbisfile -lvorbis -logg -lSDL_image -lpng -larosz -ljpeg -lSDL -lGL
#
#SDL_mixer in AROS contrib does not have support for MODs compiled in, so use one from archives.aros-exec.org
#LDFLAGS := -g /usr/local/arosv0/AROS/bin/linux-i386/AROS/Development/SDL_mixer-1.2.6/Development/lib/libSDL_mixer.a \
#           -lmodplug -lvorbisfile -lvorbis -logg -lSDL_image -lpng -larosz -ljpeg -lSDL -lGL
LDFLAGS := -g -lSDL_mixer \
           -lmodplug -lvorbisfile -lvorbis -logg -lSDL_image -lpng -larosz -ljpeg -lSDL -lGL

endif

ifeq ($(TARGET),rpi)

EXE := ../hurrican
CXX := g++
CC  := gcc
LD  := g++
RM  := rm -f

USE_EGL := yes
USE_GL2 := yes
USE_FBO := no

DEFINES += -DRPI -DUSE_GLES2 -DUSE_EGL_RPI
INCLUDES_OS := -ISDL_port \
               -I/usr/include/SDL/ \
               -I/usr/include/libmodplug \
               -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/GLES2 -I/opt/vc/include/EGL

LDFLAGS := -L/usr/lib -L/opt/vc/lib -g -lmodplug -lSDL_mixer -lSDL_image -lSDL -lGLESv2

endif

ifeq ($(TARGET),gcw)

GCWSDK := /opt/gcw0-toolchain/usr
EXE := ../hurrican
CXX := $(GCWSDK)/bin/mipsel-gcw0-linux-uclibc-g++
CC  := $(GCWSDK)/bin/mipsel-gcw0-linux-uclibc-gcc
LD  := $(GCWSDK)/bin/mipsel-gcw0-linux-uclibc-g++
RM  := rm -f

USE_EGL := yes
USE_GL2 := yes
USE_FLTO := yes
USE_NO_EXCEPTIONS := yes

DEFINES += -DGCW -DUSE_GLES2 -DUSE_EGL_RAW
DEFINES += -DUSE_HOME_DIR -DUSE_MEMPOOL -DUSE_FAST_RNG -DUSE_TRIG_LOOKUP_TABLE -DUSE_ETC1
INCLUDES_OS := -ISDL_port \
               -I$(GCWSDK)/mipsel-gcw0-linux-uclibc/sysroot/usr/include/SDL \
               -I$(GCWSDK)/mipsel-gcw0-linux-uclibc/sysroot/usr/include/libmodplug \
               -I$(GCWSDK)/mipsel-gcw0-linux-uclibc/sysroot/usr/include/GLES2 \
               -I$(GCWSDK)/mipsel-gcw0-linux-uclibc/sysroot/usr/include/EGL

LDFLAGS := -lpthread -lmodplug -lSDL_mixer -lSDL_image -lSDL -lGLESv2 -lEGL

endif

ifeq ($(TARGET),rdu)

EXE := ../hurrican
CXX := $(RDUSDK)/bin/armv7l-ims-linux-gnueabi-g++
CC  := $(RDUSDK)/bin/armv7l-ims-linux-gnueabi-gcc
LD  := $(RDUSDK)/bin/armv7l-ims-linux-gnueabi-g++
RM  := rm -f

USE_EGL := yes
USE_GL2 := yes
USE_FBO := yes

DEFINES += -DRDU -DUSE_GLES2 -DUSE_ETC1 -DUSE_EGL_RAW
DEFINES += -DUSE_HOME_DIR
INCLUDES_OS := -ISDL_port \
               -I$(RDUSDK)/include/SDL \
               -I$(RDUSDK)/include/libmodplug \
               -I$(RDUSDK)/include/GLES2 -I$(RDUSDK)/include/EGL

LDFLAGS := -L$(RDUSDK)/lib -g -lpthread -lmodplug -lSDL_mixer -lSDL_image -lSDL -lGLESv2 -lEGL

endif

ifeq ($(TARGET),pandora)

EXE := ../hurrican
CXX := $(PNDSDK)/bin/arm-none-linux-gnueabi-g++
CC  := $(PNDSDK)/bin/arm-none-linux-gnueabi-gcc
LD  := $(PNDSDK)/bin/arm-none-linux-gnueabi-g++
RM  := rm -f

USE_EGL := yes
USE_GL2 := yes
USE_FBO := no
USE_NO_EXCEPTIONS := yes

DEFINES += -DPANDORA -DUSE_GLES2 -DUSE_EGL_RAW -DUSE_PVRTC
DEFINES += -DUSE_MEMPOOL -DUSE_FAST_RNG -DUSE_TRIG_LOOKUP_TABLE
INCLUDES_OS := -ISDL_port \
               -I$(PNDSDK)/usr/include \
               -I$(PNDSDK)/usr/include/SDL \
               -I$(PNDSDK)/usr/include/libmodplug \
               -I$(PNDSDK)/usr/include/GLES2 -I$(PNDSDK)/usr/include/EGL

LDFLAGS := -L$(PNDSDK)/usr/lib -g -lSDL_mixer -lSDL_image -lSDL -lGLESv2 -lEGL -lIMGegl -lsrv_um -lpthread -lmodplug -lpng12 -lmad -ltiff -ljpeg -lz -lts

endif

ifeq ($(TARGET),linux_GL1)

EXE := ../hurricanlinux
CXX := g++
CC  := gcc
LD  := g++
RM  := rm -f

DEFINES += -DLINUX -DUSE_HOME_DIR -DUSE_GL1

INCLUDES_OS := -I/usr/include -I/usr/include/SDL -I/usr/include/GL -I/usr/include/libmodplug
LDFLAGS := -lSDL -lGL -lSDL_mixer -lSDL_image -lmodplug -lX11

endif

ifeq ($(TARGET),linux_GL2)

EXE := ../hurricanlinux
CXX := g++
CC  := gcc
LD  := g++
RM  := rm -f

USE_EGL := yes
USE_GL2 := yes
USE_FBO := yes

DEFINES += -DLINUX -DUSE_HOME_DIR -DUSE_GLES2 -DUSE_EGL_SDL

INCLUDES_OS := -I/usr/include -I/usr/include/SDL -I/usr/include/GLES2 -I/usr/include/libmodplug
LDFLAGS := -g -lSDL -lGLESv2 -lSDL_mixer -lSDL_image -lmodplug -lX11 -lEGL

endif



INCLUDES := -I. -I./SDLPort -I./cml $(INCLUDES_OS)

COMMON_CXXFLAGS := -Wall -fsingle-precision-constant -fno-rtti
LDFLAGS += $(COMMON_CXXFLAGS)

ifeq ($(USE_FLTO),yes)
# Don't allow flto when building debug builds:
ifndef _DEBUG
COMMON_CXXFLAGS += -flto
LDFLAGS += -flto
endif
endif

ifeq ($(USE_NO_EXCEPTIONS),yes)
COMMON_CXXFLAGS += -fno-exceptions
LDFLAGS += -fno-exceptions
DEFINES += -DUSE_NO_EXCEPTIONS
endif

ifeq ($(USE_UNRARLIB),yes)
DEFINES += -DUSE_UNRARLIB
INCLUDES += -I./unrarlib040/unrarlib
UNRAR_FILES := unrarlib040/unrarlib/unrarlib
endif

ifeq ($(USE_EGL),yes)
EGL_FILES := SDLPort/eglport
endif

ifeq ($(USE_GL2),yes)
DEFINES += -DUSE_GL2
GL2_FILES := SDLPort/cshader
endif

ifeq ($(USE_FBO),yes)
DEFINES += -DUSE_FBO
FBO_FILES := SDLPort/cfbo
endif


###### *** DETECT IF GCC VERSION IS >= 4.8 ***
##     If GCC >= 4.8 is being used, -Wno-unused-local-typedefs must be passed
##     to avoid numerous spurious warnings compiling against CML libary:
##     This will convert a version number like 4.8.1 to 4*100+8 and a version
##     number like 4.10.2 to 4*100+10, allowing direct comparison through bc.
GCC_VER_GTE48 := $(shell echo `$(CXX) -dumpversion | cut -f1-2 -d.` \>= 4.8 | sed -e 's/\./*100+/g' | bc )
ifeq ($(GCC_VER_GTE48),1)
COMMON_CXXFLAGS += -Wno-unused-local-typedefs
endif


COMMON_CXXFLAGS += $(INCLUDES) $(DEFINES)

ifdef _DEBUG
# Debug build:
CXXFLAGS := -c -ggdb3 -O0 -D_DEBUG -DDEBUG $(COMMON_CXXFLAGS)
else
# Normal CFLAGS:
CXXFLAGS := -c -O2 $(COMMON_CXXFLAGS)
ifeq ($(USE_FLTO),yes)
# If using FLTO, you must pass optimization flags to linker:
LDFLAGS += -O2
endif
endif

SDL_FILES := SDLPort/SDL_fmod \
             SDLPort/SDL_port \
             SDLPort/texture

FILES :=Boss_BigFish \
        Boss_Bratklops \
        Boss_Drache \
        Boss_EisFaust \
        Boss_EvilHurri \
        Boss_FahrstuhlBoss \
        Boss_FlugBoss \
        Boss_Golem \
        Boss_MetalHead \
        Boss_PharaoKopf \
        Boss_RiesenPiranha \
        Boss_RiesenQualle \
        Boss_RiesenRaupe \
        Boss_RiesenSpinne \
        Boss_RiesenWespe \
        Boss_Rollmops \
        Boss_SchmidtOrgie \
        Boss_Schneekoenig \
        Boss_Skeletor \
        Boss_Skull \
        Boss_Spinnenmaschine \
        Boss_Stahlfaust \
        Boss_TheWall \
        Boss_Ufo \
        Boss_WuxeSpinnen \
        CCracktro \
        CDragonHack \
        Console \
        DX8Enum \
        DX8Font \
        DX8Graphics \
        DX8Input \
        DX8Joystick \
        DX8Sound \
        DX8Sprite \
        DX8Texture \
        GUISystem \
        Gameplay \
        GegnerClass \
        Gegner_Auge \
        Gegner_BallerDrone \
        Gegner_BigRocket \
        Gegner_BlueBoulder \
        Gegner_BrockelRock \
        Gegner_Climber \
        Gegner_DeckenKrabbe \
        Gegner_Deckenturm \
        Gegner_Diamant \
        Gegner_Drone \
        Gegner_EierMann \
        Gegner_EisStachel \
        Gegner_Eiszapfen \
        Gegner_Extras \
        Gegner_FallBombe \
        Gegner_FallingRock \
        Gegner_FetteRakete \
        Gegner_FetteSpinne \
        Gegner_FieseDrone \
        Gegner_FieserFireWalker \
        Gegner_FieserWalker \
        Gegner_FireSpider \
        Gegner_Fledermaus \
        Gegner_FlugKanone \
        Gegner_Flugsack \
        Gegner_Geschuetz \
        Gegner_GunTower \
        Gegner_Helper \
        Gegner_Jaeger \
        Gegner_Kettenglied \
        Gegner_KrabblerLinks \
        Gegner_KrabblerOben \
        Gegner_KrabblerRechts \
        Gegner_KugelGross \
        Gegner_KugelKlein \
        Gegner_KugelMedium \
        Gegner_KugelRiesig \
        Gegner_LaFass \
        Gegner_LavaBall \
        Gegner_LavaBallSpawner \
        Gegner_LavaKrabbe \
        Gegner_Lavamann \
        Gegner_Made \
        Gegner_MiniRocket \
        Gegner_Minidragon \
        Gegner_MittelSpinne \
        Gegner_Mutant \
        Gegner_Nest \
        Gegner_NeuFisch \
        Gegner_OneUp \
        Gegner_PFlanze \
        Gegner_Piranha \
        Gegner_PokeNuke \
        Gegner_PowerBlock \
        Gegner_Punisher \
        Gegner_Qualle \
        Gegner_Raketenwerfer \
        Gegner_ReitFlugsack \
        Gegner_RoboMan1 \
        Gegner_RoboRaupe \
        Gegner_Rotzpott \
        Gegner_Schabe \
        Gegner_SchienenViech \
        Gegner_SchleimAlien \
        Gegner_SchleimBoller \
        Gegner_SchleimMaul \
        Gegner_Schneekoppe \
        Gegner_Schwabbel \
        Gegner_SideRocket \
        Gegner_SkiWalker \
        Gegner_SmallWespe \
        Gegner_SnowBomb \
        Gegner_SpiderBomb \
        Gegner_Spitter \
        Gegner_Spitterbombe \
        Gegner_Stachelbeere \
        Gegner_Stahlmuecke \
        Gegner_Stalagtit \
        Gegner_StarBig \
        Gegner_StarSmall \
        Gegner_StelzSack \
        Gegner_SwimWalker \
        Gegner_Walker \
        Gegner_WandKannone \
        Gegner_WandKrabbe \
        Gegner_WasserMine \
        Gegner_Zitrone \
        GetKeyName \
        HUD \
        Intro \
        Logdatei \
        Main \
        Math \
        Menu \
        Outtro \
        Partikelsystem \
        Player \
        Projectiles \
        Texts \
        Tileengine \
        Timer \
        Trigger_Bruecke \
        Trigger_Column \
        Trigger_Column2 \
        Trigger_EndLevel \
        Trigger_FadeMusic \
        Trigger_FahrStuhl \
        Trigger_Fass \
        Trigger_Feuerspucker \
        Trigger_Floating \
        Trigger_Floating2 \
        Trigger_Floating3 \
        Trigger_Glubschi \
        Trigger_Glubschi2 \
        Trigger_LaFassSpawner \
        Trigger_Lift \
        Trigger_LightFlare \
        Trigger_LuefterGross \
        Trigger_LuefterKlein \
        Trigger_LuefterKlein2 \
        Trigger_Mushroom \
        Trigger_PartikelSpawner \
        Trigger_Plattform \
        Trigger_Presse \
        Trigger_Presswurst \
        Trigger_SchleuseH \
        Trigger_SchleuseV \
        Trigger_Secret \
        Trigger_ShootButton \
        Trigger_ShootPlattform \
        Trigger_Shrine \
        Trigger_SoundTrigger \
        Trigger_Spikelift \
        Trigger_Stampfstein \
        Trigger_SurfBrett \
        Trigger_Switch \
        Trigger_Tube \
        Trigger_TutorialText \
        Trigger_Warning \
        stdafx \
        tracealloc

ALL_FILES := $(FILES) $(SDL_FILES) $(UNRAR_FILES) $(EGL_FILES) $(GL2_FILES) $(FBO_FILES)

ALL_OBJS := $(addsuffix .o, $(ALL_FILES))

$(EXE) : $(ALL_OBJS)
	$(LD) $^ $(LDFLAGS) -o $@
	
%.o : %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
	
%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY : clean

clean :
	$(RM) $(ALL_OBJS) $(EXE)
