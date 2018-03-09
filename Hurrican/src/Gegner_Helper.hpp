// Datei : Gegner.h

// --------------------------------------------------------------------------------------
//
// Gegner für Hurrican
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _GEGNER_HELPER_HPP_
#define _GEGNER_HELPER_HPP_

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "Trigger_TutorialText.hpp"			// muss oben sein, weil es <string> includiert

#if defined(PLATFORM_DIRECTX)
#include <d3dx8.h>
#endif
#include "DX8Sprite.hpp"

#include "Gegner_Auge.hpp"
#include "Gegner_BallerDrone.hpp"
#include "Gegner_BigRocket.hpp"
#include "Gegner_BlueBoulder.hpp"
#include "Gegner_BrockelRock.hpp"
#include "Gegner_Climber.hpp"
#include "Gegner_DeckenKrabbe.hpp"
#include "Gegner_Deckenturm.hpp"
#include "Gegner_Diamant.hpp"
#include "Gegner_Drone.hpp"
#include "Gegner_EierMann.hpp"
#include "Gegner_EisStachel.hpp"
#include "Gegner_Eiszapfen.hpp"
#include "Gegner_Extras.hpp"
#include "Gegner_FallBombe.hpp"
#include "Gegner_FallingRock.hpp"
#include "Gegner_FetteRakete.hpp"
#include "Gegner_FetteSpinne.hpp"
#include "Gegner_FieseDrone.hpp"
#include "Gegner_FieserFireWalker.hpp"
#include "Gegner_FieserWalker.hpp"
#include "Gegner_FireSpider.hpp"
#include "Gegner_Fledermaus.hpp"
#include "Gegner_FlugKanone.hpp"
#include "Gegner_Flugsack.hpp"
#include "Gegner_Geschuetz.hpp"
#include "Gegner_GunTower.hpp"
#include "Gegner_Jaeger.hpp"
#include "Gegner_Kettenglied.hpp"
#include "Gegner_KrabblerLinks.hpp"
#include "Gegner_KrabblerOben.hpp"
#include "Gegner_KrabblerRechts.hpp"
#include "Gegner_KugelGross.hpp"
#include "Gegner_KugelKlein.hpp"
#include "Gegner_KugelMedium.hpp"
#include "Gegner_KugelRiesig.hpp"
#include "Gegner_LaFass.hpp"
#include "Gegner_LavaBall.hpp"
#include "Gegner_LavaBallSpawner.hpp"
#include "Gegner_Lavamann.hpp"
#include "Gegner_LavaKrabbe.hpp"
#include "Gegner_Made.hpp"
#include "Gegner_Minidragon.hpp"
#include "Gegner_MiniRocket.hpp"
#include "Gegner_MittelSpinne.hpp"
#include "Gegner_Mutant.hpp"
#include "Gegner_Nest.hpp"
#include "Gegner_NeuFisch.hpp"
#include "Gegner_OneUp.hpp"
#include "Gegner_Piranha.hpp"
#include "Gegner_Pflanze.hpp"
#include "Gegner_PokeNuke.hpp"
#include "Gegner_PowerBlock.hpp"
#include "Gegner_Punisher.hpp"
#include "Gegner_Qualle.hpp"
#include "Gegner_Raketenwerfer.hpp"
#include "Gegner_ReitFlugsack.hpp"
#include "Gegner_RoboMan1.hpp"
#include "Gegner_RoboRaupe.hpp"
#include "Gegner_Rotzpott.hpp"
#include "Gegner_Schabe.hpp"
#include "Gegner_SchienenViech.hpp"
#include "Gegner_SchleimAlien.hpp"
#include "Gegner_SchleimBoller.hpp"
#include "Gegner_SchleimMaul.hpp"
#include "Gegner_Schneekoppe.hpp"
#include "Gegner_Schwabbel.hpp"
#include "Gegner_SideRocket.hpp"
#include "Gegner_SkiWalker.hpp"
#include "Gegner_SmallWespe.hpp"
#include "Gegner_SnowBomb.hpp"
#include "Gegner_SpiderBomb.hpp"
#include "Gegner_Spitter.hpp"
#include "Gegner_Spitterbombe.hpp"
#include "Gegner_Stachelbeere.hpp"
#include "Gegner_Stahlmuecke.hpp"
#include "Gegner_Stalagtit.hpp"
#include "Gegner_StarBig.hpp"
#include "Gegner_StarSmall.hpp"
#include "Gegner_StelzSack.hpp"
#include "Gegner_SwimWalker.hpp"
#include "Gegner_Walker.hpp"
#include "Gegner_WandKannone.hpp"
#include "Gegner_WandKrabbe.hpp"
#include "Gegner_WasserMine.hpp"
#include "Gegner_Zitrone.hpp"

#include "bosses/Boss_BigFish.hpp"
#include "bosses/Boss_Bratklops.hpp"
#include "bosses/Boss_Drache.hpp"
#include "bosses/Boss_EisFaust.hpp"
#include "bosses/Boss_EvilHurri.hpp"
#include "bosses/Boss_FahrstuhlBoss.hpp"
#include "bosses/Boss_FlugBoss.hpp"
#include "bosses/Boss_Golem.hpp"
#include "bosses/Boss_MetalHead.hpp"
#include "bosses/Boss_PharaoKopf.hpp"
#include "bosses/Boss_RiesenPiranha.hpp"
#include "bosses/Boss_RiesenQualle.hpp"
#include "bosses/Boss_RiesenRaupe.hpp"
#include "bosses/Boss_RiesenSpinne.hpp"
#include "bosses/Boss_RiesenWespe.hpp"
#include "bosses/Boss_Rollmops.hpp"
#include "bosses/Boss_SchmidtOrgie.hpp"
#include "bosses/Boss_Schneekoenig.hpp"
#include "bosses/Boss_Skeletor.hpp"
#include "bosses/Boss_Skull.hpp"
#include "bosses/Boss_Spinnenmaschine.hpp"
#include "bosses/Boss_Stahlfaust.hpp"
#include "bosses/Boss_TheWall.hpp"
#include "bosses/Boss_Ufo.hpp"
#include "bosses/Boss_WuxeSpinnen.hpp"

#include "Trigger_Bruecke.hpp"
#include "Trigger_Column.hpp"
#include "Trigger_Column2.hpp"
#include "Trigger_EndLevel.hpp"
#include "Trigger_Fahrstuhl.hpp"
#include "Trigger_Fass.hpp"
#include "Trigger_FadeMusic.hpp"
#include "Trigger_Feuerspucker.hpp"
#include "Trigger_Floating.hpp"
#include "Trigger_Floating2.hpp"
#include "Trigger_Floating3.hpp"
#include "Trigger_Glubschi.hpp"
#include "Trigger_Glubschi2.hpp"
#include "Trigger_LaFassSpawner.hpp"
#include "Trigger_Lift.hpp"
#include "Trigger_LightFlare.hpp"
#include "Trigger_LuefterGross.hpp"
#include "Trigger_LuefterKlein.hpp"
#include "Trigger_LuefterKlein2.hpp"
#include "Trigger_Mushroom.hpp"
#include "Trigger_PartikelSpawner.hpp"
#include "Trigger_Plattform.hpp"
#include "Trigger_Presse.hpp"
#include "Trigger_Presswurst.hpp"
#include "Trigger_SchleuseH.hpp"
#include "Trigger_SchleuseV.hpp"
#include "Trigger_Secret.hpp"
#include "Trigger_ShootButton.hpp"
#include "Trigger_ShootPlattform.hpp"
#include "Trigger_Shrine.hpp"
#include "Trigger_SoundTrigger.hpp"
#include "Trigger_Spikelift.hpp"
#include "Trigger_Stampfstein.hpp"
#include "Trigger_SurfBrett.hpp"
#include "Trigger_Switch.hpp"
#include "Trigger_Tube.hpp"
#include "Trigger_Warning.hpp"

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
