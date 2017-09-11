// Datei : Gegner.h

// --------------------------------------------------------------------------------------
//
// Gegner für Hurrican
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _GEGNER_HELPER_H_
#define _GEGNER_HELPER_H_

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "Trigger_TutorialText.h"			// muss oben sein, weil es <string> includiert

#if defined(PLATFORM_DIRECTX)
#include <d3dx8.h>
#endif
#include "DX8Sprite.h"

#include "Gegner_Auge.h"
#include "Gegner_BallerDrone.h"
#include "Gegner_BigRocket.h"
#include "Gegner_BlueBoulder.h"
#include "Gegner_BrockelRock.h"
#include "Gegner_Climber.h"
#include "Gegner_DeckenKrabbe.h"
#include "Gegner_Deckenturm.h"
#include "Gegner_Diamant.h"
#include "Gegner_Drone.h"
#include "Gegner_EierMann.h"
#include "Gegner_EisStachel.h"
#include "Gegner_Eiszapfen.h"
#include "Gegner_Extras.h"
#include "Gegner_FallBombe.h"
#include "Gegner_FallingRock.h"
#include "Gegner_FetteRakete.h"
#include "Gegner_FetteSpinne.h"
#include "Gegner_FieseDrone.h"
#include "Gegner_FieserFireWalker.h"
#include "Gegner_FieserWalker.h"
#include "Gegner_FireSpider.h"
#include "Gegner_Fledermaus.h"
#include "Gegner_FlugKanone.h"
#include "Gegner_Flugsack.h"
#include "Gegner_Geschuetz.h"
#include "Gegner_GunTower.h"
#include "Gegner_Jaeger.h"
#include "Gegner_Kettenglied.h"
#include "Gegner_KrabblerLinks.h"
#include "Gegner_KrabblerOben.h"
#include "Gegner_KrabblerRechts.h"
#include "Gegner_KugelGross.h"
#include "Gegner_KugelKlein.h"
#include "Gegner_KugelMedium.h"
#include "Gegner_KugelRiesig.h"
#include "Gegner_LaFass.h"
#include "Gegner_LavaBall.h"
#include "Gegner_LavaBallSpawner.h"
#include "Gegner_Lavamann.h"
#include "Gegner_LavaKrabbe.h"
#include "Gegner_Made.h"
#include "Gegner_Minidragon.h"
#include "Gegner_MiniRocket.h"
#include "Gegner_MittelSpinne.h"
#include "Gegner_Mutant.h"
#include "Gegner_Nest.h"
#include "Gegner_NeuFisch.h"
#include "Gegner_OneUp.h"
#include "Gegner_Piranha.h"
#include "Gegner_Pflanze.h"
#include "Gegner_PokeNuke.h"
#include "Gegner_PowerBlock.h"
#include "Gegner_Punisher.h"
#include "Gegner_Qualle.h"
#include "Gegner_Raketenwerfer.h"
#include "Gegner_ReitFlugsack.h"
#include "Gegner_RoboMan1.h"
#include "Gegner_RoboRaupe.h"
#include "Gegner_Rotzpott.h"
#include "Gegner_Schabe.h"
#include "Gegner_SchienenViech.h"
#include "Gegner_SchleimAlien.h"
#include "Gegner_SchleimBoller.h"
#include "Gegner_SchleimMaul.h"
#include "Gegner_Schneekoppe.h"
#include "Gegner_Schwabbel.h"
#include "Gegner_SideRocket.h"
#include "Gegner_SkiWalker.h"
#include "Gegner_SmallWespe.h"
#include "Gegner_SnowBomb.h"
#include "Gegner_SpiderBomb.h"
#include "Gegner_Spitter.h"
#include "Gegner_Spitterbombe.h"
#include "Gegner_Stachelbeere.h"
#include "Gegner_Stahlmuecke.h"
#include "Gegner_Stalagtit.h"
#include "Gegner_StarBig.h"
#include "Gegner_StarSmall.h"
#include "Gegner_StelzSack.h"
#include "Gegner_SwimWalker.h"
#include "Gegner_Walker.h"
#include "Gegner_WandKannone.h"
#include "Gegner_WandKrabbe.h"
#include "Gegner_WasserMine.h"
#include "Gegner_Zitrone.h"

#include "Boss_BigFish.h"
#include "Boss_Bratklops.h"
#include "Boss_Drache.h"
#include "Boss_EisFaust.h"
#include "Boss_EvilHurri.h"
#include "Boss_FahrstuhlBoss.h"
#include "Boss_FlugBoss.h"
#include "Boss_Golem.h"
#include "Boss_MetalHead.h"
#include "Boss_PharaoKopf.h"
#include "Boss_RiesenPiranha.h"
#include "Boss_RiesenQualle.h"
#include "Boss_RiesenRaupe.h"
#include "Boss_RiesenSpinne.h"
#include "Boss_RiesenWespe.h"
#include "Boss_Rollmops.h"
#include "Boss_SchmidtOrgie.h"
#include "Boss_Schneekoenig.h"
#include "Boss_Skeletor.h"
#include "Boss_Skull.h"
#include "Boss_Spinnenmaschine.h"
#include "Boss_Stahlfaust.h"
#include "Boss_TheWall.h"
#include "Boss_Ufo.h"
#include "Boss_WuxeSpinnen.h"

#include "Trigger_Bruecke.h"
#include "Trigger_Column.h"
#include "Trigger_Column2.h"
#include "Trigger_EndLevel.h"
#include "Trigger_Fahrstuhl.h"
#include "Trigger_Fass.h"
#include "Trigger_FadeMusic.h"
#include "Trigger_Feuerspucker.h"
#include "Trigger_Floating.h"
#include "Trigger_Floating2.h"
#include "Trigger_Floating3.h"
#include "Trigger_Glubschi.h"
#include "Trigger_Glubschi2.h"
#include "Trigger_LaFassSpawner.h"
#include "Trigger_Lift.h"
#include "Trigger_LightFlare.h"
#include "Trigger_LuefterGross.h"
#include "Trigger_LuefterKlein.h"
#include "Trigger_LuefterKlein2.h"
#include "Trigger_Mushroom.h"
#include "Trigger_PartikelSpawner.h"
#include "Trigger_Plattform.h"
#include "Trigger_Presse.h"
#include "Trigger_Presswurst.h"
#include "Trigger_SchleuseH.h"
#include "Trigger_SchleuseV.h"
#include "Trigger_Secret.h"
#include "Trigger_ShootButton.h"
#include "Trigger_ShootPlattform.h"
#include "Trigger_Shrine.h"
#include "Trigger_SoundTrigger.h"
#include "Trigger_Spikelift.h"
#include "Trigger_Stampfstein.h"
#include "Trigger_SurfBrett.h"
#include "Trigger_Switch.h"
#include "Trigger_Tube.h"
#include "Trigger_Warning.h"

// --------------------------------------------------------------------------------------
// Funktions-Prototypen
// --------------------------------------------------------------------------------------

void LoadGegnerGrafik(int Nr);							// Bestimmten Gegner laden

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern GegnerListClass		Gegner;
extern DirectGraphicsSprite	*pGegnerGrafix[MAX_GEGNERGFX];		// Grafiken der Gegner
extern RECT					 GegnerRect	  [MAX_GEGNERGFX];		// Rechtecke für Kollision
extern float				 g_Fahrstuhl_yPos;					// yPosition des Fahrstuhls
extern float				 g_Fahrstuhl_Offset;				// Scrollposition relativ zum Farhstuhl
extern float				 g_Fahrstuhl_Speed;					// Fahrstuhl Geschwindigkeit


#endif
