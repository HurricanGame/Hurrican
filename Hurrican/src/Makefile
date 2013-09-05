DEFINES := -DPLATFORM_SDL -DENABLE_CONSOLE_COMMANDS -DUSE_MODPLUG -DDEBUG

TARGET = rpi

ifeq ($(TARGET),aros)

EXE := ../hurricanaros
CCP := i386-aros-gcc
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
CCP := g++
CC  := gcc
LD  := g++
RM  := rm -f

USE_EGL := yes
USE_GL2 := yes

DEFINES += -DRPI -DUSE_GL2 -DUSE_GLES2 -DUSE_EGL_RPI -DRGBA_5551

INCLUDES_OS := -ISDL_port \
               -I/usr/include/SDL/ \
               -I/usr/include/libmodplug \
               -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/GLES2 -I/opt/vc/include/EGL

LDFLAGS := -L/usr/lib -L/opt/vc/lib -g -lmodplug -lSDL_mixer -lSDL_image -lSDL -lGLESv2

endif

ifeq ($(TARGET),gcw)

EXE := ../hurrican
CCP := $(GCWSDK)/bin/mipsel-gcw0-linux-uclibc-g++
CC  := $(GCWSDK)/bin/mipsel-gcw0-linux-uclibc-gcc
LD  := $(GCWSDK)/bin/mipsel-gcw0-linux-uclibc-g++
RM  := rm -f

USE_EGL := yes
USE_GL2 := yes

DEFINES += -DGCW -DUSE_GL2 -DUSE_GLES2 -DRGBA_5551 -DUSE_EGL_RAW -DUSE_320_240

INCLUDES_OS := -ISDL_port \
               -I$(GCWSDK)/mipsel-gcw0-linux-uclibc/sysroot/usr/include/SDL \
               -I$(GCWSDK)/mipsel-gcw0-linux-uclibc/sysroot/usr/include/libmodplug \
               -I$(GCWSDK)/mipsel-gcw0-linux-uclibc/sysroot/usr/include/GLES2 -I$(GCWSDK)/include/mipsel-gcw0-linux-uclibc/sysroot/usr/include/EGL

LDFLAGS := -L$(GCWSDK)/lib -g -lpthread -lmodplug -lSDL_mixer -lSDL_image -lSDL -lGLESv2 -lEGL

endif


ifeq ($(TARGET),linux)

EXE := ../hurricanlinux
CCP := gcc
CC  := gcc
LD  := g++
RM  := rm -f

USE_EGL := yes
USE_GL2 := yes

DEFINES += -DLINUX -DUSE_GL2 -DUSE_GLES2 -DUSE_EGL_SDL -DRGBA_5551

INCLUDES_OS := -I/usr/include -I/usr/include/SDL -I/usr/include/GLES2 -I/usr/include/libmodplug

LDFLAGS := -g -lSDL -lGLESv2 -lSDL_mixer -lSDL_image -lmodplug -lX11 -lEGL

endif

INCLUDES := -I. -I./SDLPort -Icml -I./unrarlib040/unrarlib $(INCLUDES_OS)

CFLAGS := -g -Wall -O0 -c $(INCLUDES) $(DEFINES)

SDL_FILES := SDLPort/SDL_fmod \
             SDLPort/SDL_port \
             SDLPort/texture

EGL_FILES := SDLPort/eglport

GL2_FILES := SDLPort/cshader

#SDLPort/cshader

UNRAR_FILES := unrarlib040/unrarlib/unrarlib

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
        CCutScene \
        CDragonHack \
        Console \
        DX8Enum \
        DX8Font \
        DX8Graphics \
        DX8Input \
        DX8Joystick \
        DX8Sound \
        DX8Sprite \
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
        lightmap \
        stdafx \
        tracealloc

ALL_FILES := $(FILES) $(SDL_FILES) $(UNRAR_FILES)

ifeq ($(USE_EGL),yes)
ALL_FILES += $(EGL_FILES)
endif

ifeq ($(USE_GL2),yes)
ALL_FILES += $(GL2_FILES)
endif

ALL_OBJS := $(addsuffix .o, $(ALL_FILES))

$(EXE) : $(ALL_OBJS)
	$(LD) $^ $(LDFLAGS) -o $@
	
%.o : %.cpp
	$(CCP) $(CFLAGS) $< -o $@
	
%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY : clean

clean :
	$(RM) $(ALL_OBJS) $(EXE)
