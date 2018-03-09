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

#include "triggers/Trigger_TutorialText.hpp"            // muss oben sein, weil es <string> includiert

#if defined(PLATFORM_DIRECTX)
#include <d3dx8.h>
#endif
#include "DX8Sprite.hpp"

#include "enemies/Gegner_Auge.hpp"
#include "enemies/Gegner_BallerDrone.hpp"
#include "enemies/Gegner_BigRocket.hpp"
#include "enemies/Gegner_BlueBoulder.hpp"
#include "enemies/Gegner_BrockelRock.hpp"
#include "enemies/Gegner_Climber.hpp"
#include "enemies/Gegner_DeckenKrabbe.hpp"
#include "enemies/Gegner_Deckenturm.hpp"
#include "enemies/Gegner_Diamant.hpp"
#include "enemies/Gegner_Drone.hpp"
#include "enemies/Gegner_EierMann.hpp"
#include "enemies/Gegner_EisStachel.hpp"
#include "enemies/Gegner_Eiszapfen.hpp"
#include "enemies/Gegner_Extras.hpp"
#include "enemies/Gegner_FallBombe.hpp"
#include "enemies/Gegner_FallingRock.hpp"
#include "enemies/Gegner_FetteRakete.hpp"
#include "enemies/Gegner_FetteSpinne.hpp"
#include "enemies/Gegner_FieseDrone.hpp"
#include "enemies/Gegner_FieserFireWalker.hpp"
#include "enemies/Gegner_FieserWalker.hpp"
#include "enemies/Gegner_FireSpider.hpp"
#include "enemies/Gegner_Fledermaus.hpp"
#include "enemies/Gegner_FlugKanone.hpp"
#include "enemies/Gegner_Flugsack.hpp"
#include "enemies/Gegner_Geschuetz.hpp"
#include "enemies/Gegner_GunTower.hpp"
#include "enemies/Gegner_Jaeger.hpp"
#include "enemies/Gegner_Kettenglied.hpp"
#include "enemies/Gegner_KrabblerLinks.hpp"
#include "enemies/Gegner_KrabblerOben.hpp"
#include "enemies/Gegner_KrabblerRechts.hpp"
#include "enemies/Gegner_KugelGross.hpp"
#include "enemies/Gegner_KugelKlein.hpp"
#include "enemies/Gegner_KugelMedium.hpp"
#include "enemies/Gegner_KugelRiesig.hpp"
#include "enemies/Gegner_LaFass.hpp"
#include "enemies/Gegner_LavaBall.hpp"
#include "enemies/Gegner_LavaBallSpawner.hpp"
#include "enemies/Gegner_Lavamann.hpp"
#include "enemies/Gegner_LavaKrabbe.hpp"
#include "enemies/Gegner_Made.hpp"
#include "enemies/Gegner_Minidragon.hpp"
#include "enemies/Gegner_MiniRocket.hpp"
#include "enemies/Gegner_MittelSpinne.hpp"
#include "enemies/Gegner_Mutant.hpp"
#include "enemies/Gegner_Nest.hpp"
#include "enemies/Gegner_NeuFisch.hpp"
#include "enemies/Gegner_OneUp.hpp"
#include "enemies/Gegner_Piranha.hpp"
#include "enemies/Gegner_Pflanze.hpp"
#include "enemies/Gegner_PokeNuke.hpp"
#include "enemies/Gegner_PowerBlock.hpp"
#include "enemies/Gegner_Punisher.hpp"
#include "enemies/Gegner_Qualle.hpp"
#include "enemies/Gegner_Raketenwerfer.hpp"
#include "enemies/Gegner_ReitFlugsack.hpp"
#include "enemies/Gegner_RoboMan1.hpp"
#include "enemies/Gegner_RoboRaupe.hpp"
#include "enemies/Gegner_Rotzpott.hpp"
#include "enemies/Gegner_Schabe.hpp"
#include "enemies/Gegner_SchienenViech.hpp"
#include "enemies/Gegner_SchleimAlien.hpp"
#include "enemies/Gegner_SchleimBoller.hpp"
#include "enemies/Gegner_SchleimMaul.hpp"
#include "enemies/Gegner_Schneekoppe.hpp"
#include "enemies/Gegner_Schwabbel.hpp"
#include "enemies/Gegner_SideRocket.hpp"
#include "enemies/Gegner_SkiWalker.hpp"
#include "enemies/Gegner_SmallWespe.hpp"
#include "enemies/Gegner_SnowBomb.hpp"
#include "enemies/Gegner_SpiderBomb.hpp"
#include "enemies/Gegner_Spitter.hpp"
#include "enemies/Gegner_Spitterbombe.hpp"
#include "enemies/Gegner_Stachelbeere.hpp"
#include "enemies/Gegner_Stahlmuecke.hpp"
#include "enemies/Gegner_Stalagtit.hpp"
#include "enemies/Gegner_StarBig.hpp"
#include "enemies/Gegner_StarSmall.hpp"
#include "enemies/Gegner_StelzSack.hpp"
#include "enemies/Gegner_SwimWalker.hpp"
#include "enemies/Gegner_Walker.hpp"
#include "enemies/Gegner_WandKannone.hpp"
#include "enemies/Gegner_WandKrabbe.hpp"
#include "enemies/Gegner_WasserMine.hpp"
#include "enemies/Gegner_Zitrone.hpp"

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

#include "triggers/Trigger_Bruecke.hpp"
#include "triggers/Trigger_Column.hpp"
#include "triggers/Trigger_Column2.hpp"
#include "triggers/Trigger_EndLevel.hpp"
#include "triggers/Trigger_Fahrstuhl.hpp"
#include "triggers/Trigger_Fass.hpp"
#include "triggers/Trigger_FadeMusic.hpp"
#include "triggers/Trigger_Feuerspucker.hpp"
#include "triggers/Trigger_Floating.hpp"
#include "triggers/Trigger_Floating2.hpp"
#include "triggers/Trigger_Floating3.hpp"
#include "triggers/Trigger_Glubschi.hpp"
#include "triggers/Trigger_Glubschi2.hpp"
#include "triggers/Trigger_LaFassSpawner.hpp"
#include "triggers/Trigger_Lift.hpp"
#include "triggers/Trigger_LightFlare.hpp"
#include "triggers/Trigger_LuefterGross.hpp"
#include "triggers/Trigger_LuefterKlein.hpp"
#include "triggers/Trigger_LuefterKlein2.hpp"
#include "triggers/Trigger_Mushroom.hpp"
#include "triggers/Trigger_PartikelSpawner.hpp"
#include "triggers/Trigger_Plattform.hpp"
#include "triggers/Trigger_Presse.hpp"
#include "triggers/Trigger_Presswurst.hpp"
#include "triggers/Trigger_SchleuseH.hpp"
#include "triggers/Trigger_SchleuseV.hpp"
#include "triggers/Trigger_Secret.hpp"
#include "triggers/Trigger_ShootButton.hpp"
#include "triggers/Trigger_ShootPlattform.hpp"
#include "triggers/Trigger_Shrine.hpp"
#include "triggers/Trigger_SoundTrigger.hpp"
#include "triggers/Trigger_Spikelift.hpp"
#include "triggers/Trigger_Stampfstein.hpp"
#include "triggers/Trigger_SurfBrett.hpp"
#include "triggers/Trigger_Switch.hpp"
#include "triggers/Trigger_Tube.hpp"
#include "triggers/Trigger_Warning.hpp"

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
