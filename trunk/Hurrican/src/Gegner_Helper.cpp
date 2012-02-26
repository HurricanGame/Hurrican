// Datei : Gegner.cpp

// -------------------------------------------------------------------------------------- 
//
// Gegner für Hurrican
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "Console.h"
#include "DX8Font.h"
#include "DX8Sound.h"
#include "Gameplay.h"
#include "Gegner_Helper.h"
#include "Globals.h"
#include "HUD.h"
#include "LogDatei.h"
#include "Partikelsystem.h"
#include "Projectiles.h"
#include "Player.h"
#include "Tileengine.h"
#include "Timer.h"


// --------------------------------------------------------------------------------------
// Variablen
// --------------------------------------------------------------------------------------

DirectGraphicsSprite	LavaFlare;							// Leuchten des lava Balles
DirectGraphicsSprite	*pGegnerGrafix[MAX_GEGNERGFX];		// Grafiken der Gegner
DirectGraphicsSprite	*pDroneFlame = NULL;				// Flamme der Drone
DirectGraphicsSprite	*pDroneGun = NULL;					// Flamme der Zitrone
RECT					 GegnerRect	  [MAX_GEGNERGFX];		// Rechtecke für Kollision	
float					g_Fahrstuhl_yPos;					// yPosition des Fahrstuhls
float					g_Fahrstuhl_Offset;					// Scrollposition relativ zum Farhstuhl
float					g_Fahrstuhl_Speed;					// Fahrstuhl Geschwindigkeit

// --------------------------------------------------------------------------------------
// Bestimmte Gegner-Grafik laden
// --------------------------------------------------------------------------------------

void LoadGegnerGrafik(int Nr)
{
	pGegnerGrafix[Nr] = new DirectGraphicsSprite();

	switch (Nr)
	{
		case SPITTER:
			pGegnerGrafix[SPITTER]->LoadImage("spitter.bmp", 110,  23, 22, 23, 5,1);
		break;
	
		case WALKER:
			pGegnerGrafix[WALKER]->LoadImage("enemy-walker.bmp", 360, 180, 45, 45, 8, 4);
		break;

		case GUNTOWER:
			pGegnerGrafix[GUNTOWER]->LoadImage("enemy-guntower.bmp", 300, 120, 60, 40, 5, 3);
		break;

		case SPIDERBOMB:
			pGegnerGrafix[SPIDERBOMB]->LoadImage("spiderbomb.bmp", 350, 34, 50, 34, 7, 1);
		break;

		case PIRANHA:
			pGegnerGrafix[PIRANHA]->LoadImage("piranha.bmp", 225, 232, 45, 29, 5, 8);
		break;

		case STAHLMUECKE:
			pGegnerGrafix[STAHLMUECKE]->LoadImage("stahlmuecke.bmp", 170, 70, 34, 35, 5, 2);
		break;

		case DECKENTURM:
			pGegnerGrafix[DECKENTURM]->LoadImage("deckenturm.bmp", 190, 240, 38, 40, 5, 6);
		break;

		case DECKENKRABBE:
			pGegnerGrafix[DECKENKRABBE]->LoadImage("lavakrabbe.bmp", 280, 136, 70, 34, 4,4);
		break;

		case KUGELKLEIN:
			pGegnerGrafix[KUGELKLEIN]->LoadImage("kugel4.bmp", 150, 120, 30, 30, 5, 4);
		break;

		case KUGELMEDIUM:
			pGegnerGrafix[KUGELMEDIUM]->LoadImage("kugel3.bmp", 225, 180, 45, 45, 5, 4);
		break;

		case KUGELGROSS:
			pGegnerGrafix[KUGELGROSS]->LoadImage("kugel2.bmp", 300, 240, 60, 60, 5, 4);
		break;

		case KUGELRIESIG:
			pGegnerGrafix[KUGELRIESIG]->LoadImage("kugel1.bmp", 450, 360, 90, 90, 5, 4);
		break;

		case KRABBLEROBEN:
			pGegnerGrafix[KRABBLEROBEN]->LoadImage("krabbleroben.bmp", 250, 225, 50, 75, 5, 3);
		break;

		case KRABBLERLINKS:
			pGegnerGrafix[KRABBLERLINKS]->LoadImage("krabblerlinks.bmp", 225, 250, 75, 50, 3, 5);
		break;

		case KRABBLERRECHTS:
			pGegnerGrafix[KRABBLERRECHTS]->LoadImage("krabblerrechts.bmp", 225, 250, 75, 50, 3, 5);
		break;

		case AUGE:
			pGegnerGrafix[AUGE]->LoadImage("auge.bmp", 360, 26, 60, 26, 6, 1);
		break;

		case QUALLE:
			pGegnerGrafix[QUALLE]->LoadImage("qualle.bmp", 240, 180, 40, 60, 6, 3);
		break;

		case FALLINGROCK:
			pGegnerGrafix[FALLINGROCK]->LoadImage("fallingstone.bmp", 80, 40, 80, 40, 1, 1);
		break;

		case BROCKELROCK:
			pGegnerGrafix[BROCKELROCK]->LoadImage("fallingstone2.bmp", 160, 40, 80, 40, 2, 1);
		break;

		case ROBORAUPE:
			pGegnerGrafix[ROBORAUPE]->LoadImage("roboraupe.bmp", 360, 105, 90, 35, 4, 3);
		break;

		case STALAGTIT:
			pGegnerGrafix[STALAGTIT]->LoadImage("stalagtit.bmp", 40, 80, 40, 80, 1, 1);
		break;

		case RAKETENWERFER:
			pGegnerGrafix[RAKETENWERFER]->LoadImage("raketenwerfer.bmp", 225, 50, 45, 25, 5, 2);
		break;

		case SWIMWALKER:
			pGegnerGrafix[SWIMWALKER]->LoadImage("swimwalker.bmp", 260, 135, 65, 45, 4, 3);
		break;

		case FLUGSACK:
			pGegnerGrafix[FLUGSACK]->LoadImage("flugsack.bmp", 320, 210, 80, 70, 4, 3);
		break;

		case REITFLUGSACK:
			pGegnerGrafix[REITFLUGSACK]->LoadImage("flugsack.bmp", 320, 210, 80, 70, 4, 3);
		break;

		case EISSTACHEL:
			pGegnerGrafix[EISSTACHEL]->LoadImage("eisstachel.bmp", 420, 390, 105, 78, 4, 5);
		break;

		case SKIWALKER:
			pGegnerGrafix[SKIWALKER]->LoadImage("skiwalker.bmp", 195, 152, 65, 38, 3,4);
		break;

		case FLUGKANONE:
			pGegnerGrafix[FLUGKANONE]->LoadImage("flugkanone.bmp", 260, 225, 65, 45, 4,5);
		break;

		case SNOWBOMBE:
			pGegnerGrafix[SNOWBOMBE]->LoadImage("snowbomb.bmp", 480, 320, 80, 80, 6,4);
		break;

		case ROBOMAN1:
			pGegnerGrafix[ROBOMAN1]->LoadImage("roboman1.bmp", 450, 300, 90, 100, 5,3);
		break;

		case BOULDER:
			pGegnerGrafix[BOULDER]->LoadImage("boulder.bmp", 300, 165, 60, 55, 5,3);
		break;

		case STELZSACK:
			pGegnerGrafix[STELZSACK]->LoadImage("stelzsack.bmp", 500, 441, 100, 147, 5,4);
		break;

		case FALLBOMBE:
			pGegnerGrafix[FALLBOMBE]->LoadImage("fallbombe.bmp", 138, 59, 23, 59, 6,1);
		break;

		case POKENUKE:
			pGegnerGrafix[POKENUKE]->LoadImage("pokenuke.bmp", 81, 217, 81, 217, 1,1);
		break;

		case LAVAKRABBE:
			pGegnerGrafix[LAVAKRABBE]->LoadImage("lavakrabbe.bmp", 280, 136, 70, 34, 4,4);
		break;

		case LAVABALLSPAWNER:
			pGegnerGrafix[LAVABALLSPAWNER]->LoadImage("lavaball.bmp", 300, 240, 60, 60, 5,4);
		break;

		case LAVABALL:
		{
			pGegnerGrafix[LAVABALL]->LoadImage("lavaball.bmp", 300, 240, 60, 60, 5,4);			
		}
		break;

		case FETTESPINNE:
			pGegnerGrafix[FETTESPINNE]->LoadImage("fettespinne.png", 416, 414, 104, 69, 4,6);
		break;

		case EIERMANN:
			pGegnerGrafix[EIERMANN]->LoadImage("eiermann.bmp", 310, 132, 62, 66, 5,2);
		break;

		case WANDKANNONE:
			pGegnerGrafix[WANDKANNONE]->LoadImage("wandcannon.bmp", 141, 32, 47, 32, 3,1);
		break;

		case STARSMALL:
			pGegnerGrafix[STARSMALL]->LoadImage("starsmall.bmp", 80, 80, 40, 40, 2,2);
		break;

		case STARBIG:
			pGegnerGrafix[STARBIG]->LoadImage("starbig.bmp", 160, 160, 80, 80, 2,2);
		break;

		case MADE:
			pGegnerGrafix[MADE]->LoadImage("made.bmp", 200, 40, 20, 20, 10,2);
		break;

		case DRONE:
			pGegnerGrafix[DRONE]->LoadImage("drone.bmp", 280, 240, 70, 80, 4,3);
		break;

		case NEUFISCH:
			pGegnerGrafix[NEUFISCH]->LoadImage("neufisch.bmp", 360, 292, 90, 73, 4,4);
		break;

		case PFLANZE:
			pGegnerGrafix[PFLANZE]->LoadImage("pflanze.bmp", 440, 450, 110, 90, 4,5);
		break;

		case SPITTERBOMBE:
			pGegnerGrafix[SPITTERBOMBE]->LoadImage("spitterbombe.bmp", 174, 50, 29, 50, 6,1);
		break;

		case NEST:
			pGegnerGrafix[NEST]->LoadImage("nest.bmp", 43, 60, 43, 60, 1,1);
		break;

		case SCHIENENVIECH:
			pGegnerGrafix[SCHIENENVIECH]->LoadImage("schienenviech.bmp", 245, 40, 49, 40, 5,1);
		break;

		case WASSERMINE:
			pGegnerGrafix[WASSERMINE]->LoadImage("wassermine.bmp", 300, 190, 60, 90, 5,2);
		break;

		case FLEDERMAUS:
			pGegnerGrafix[FLEDERMAUS]->LoadImage("fledermaus.bmp", 500, 174, 50, 58, 10,3);
		break;

		case CLIMBSPIDER:
			pGegnerGrafix[CLIMBSPIDER]->LoadImage("climbspider.bmp", 250, 105, 50, 35, 5,3);
		break;

		case FIRESPIDER:
			pGegnerGrafix[FIRESPIDER]->LoadImage("feuerspinne.bmp", 480, 110, 80, 55, 6,2);
		break;

		case BALLERDRONE:
			pGegnerGrafix[BALLERDRONE]->LoadImage("ballerdrone.bmp", 450, 195, 90, 65, 5,3);
		break;

		case SCHWABBEL:
			pGegnerGrafix[SCHWABBEL]->LoadImage("schwabbel.bmp", 300, 240, 60, 60, 5,4);
		break;

		case BIGROCKET:
			pGegnerGrafix[BIGROCKET]->LoadImage("bigrocket.bmp", 578, 600, 73, 300, 8,2);
		break;

		case EISZAPFEN:
			pGegnerGrafix[EISZAPFEN]->LoadImage("eiszapfen.png", 30, 100, 30, 100, 1,1);
		break;

		case WANDKRABBE:
			pGegnerGrafix[WANDKRABBE]->LoadImage("wandkrabbe.bmp", 136, 192, 34, 64, 4,3);
		break;

		case JAEGER:
			pGegnerGrafix[JAEGER]->LoadImage("jaeger.bmp", 280, 212, 70, 53, 4,4);
		break;

		case SIDEROCKET:
			pGegnerGrafix[SIDEROCKET]->LoadImage("siderocket.bmp", 40, 18, 40, 18, 1, 1);
		break;

		case FIESEDRONE:
			pGegnerGrafix[FIESEDRONE]->LoadImage("fiesedrone.bmp", 200, 250, 50, 50, 4, 5);
		break;

		case SCHLEIMBOLLER:
			pGegnerGrafix[SCHLEIMBOLLER]->LoadImage("schleimboller.png", 240, 240, 60, 60, 4, 4);
		break;

		case SCHLEIMALIEN:
			pGegnerGrafix[SCHLEIMALIEN]->LoadImage("schleimrot.png", 240, 240, 60, 60, 4, 4);
		break;

		case SCHLEIMMAUL:
			pGegnerGrafix[SCHLEIMMAUL]->LoadImage("schleimmaul.png", 272, 272, 68, 68, 4, 4);
		break;

		case FIESERWALKER:
			pGegnerGrafix[FIESERWALKER]->LoadImage("fieserwalker.bmp", 264, 240, 66, 60, 4, 4);
		break;

		case FIESERWALKER2:
			pGegnerGrafix[FIESERWALKER2]->LoadImage("fieserwalker2.bmp", 264, 240, 66, 60, 4, 4);
		break;

		case MITTELSPINNE:
			pGegnerGrafix[MITTELSPINNE]->LoadImage("mittelspinne.png", 321, 344, 107, 86, 3, 4);
		break;

		case STAMPFSTEIN:
			pGegnerGrafix[STAMPFSTEIN]->LoadImage("stampfstein.png", 80, 256, 80, 256, 1,1);
		break;

		case LAVAMANN:
			pGegnerGrafix[LAVAMANN]->LoadImage("lavamann.bmp", 280, 150, 40, 50, 7, 3);
		break;

		case PUNISHER:
			pGegnerGrafix[PUNISHER]->LoadImage("punisher.png", 1020, 850, 170, 170, 6, 5);
		break;

		case SMALLWESPE:
			pGegnerGrafix[SMALLWESPE]->LoadImage("smallwespe.bmp", 200, 40, 50, 40, 4, 1);
		break;

		case RIESENPIRANHA:
			pGegnerGrafix[RIESENPIRANHA]->LoadImage("riesenpiranha.bmp", 1024, 420, 256, 210, 4, 2);
		break;

		case RIESENQUALLE:
			pGegnerGrafix[RIESENQUALLE]->LoadImage("riesenqualle.bmp", 390, 480, 130, 240, 3, 2);
		break;

		case RIESENRAUPE:
			pGegnerGrafix[RIESENRAUPE]->LoadImage("riesenraupe.bmp", 540, 290, 180, 58, 3, 5);
		break;

		case RIESENWASP:
			pGegnerGrafix[RIESENWASP]->LoadImage("wasp.png", 232, 300, 116, 100, 2, 3);
		break;

		case STAHLFAUST:
			pGegnerGrafix[STAHLFAUST]->LoadImage("stahlfaust.bmp", 512, 264, 256, 264, 2, 1);
		break;

		case PHARAOKOPF:
			pGegnerGrafix[PHARAOKOPF]->LoadImage("pharaokopf.bmp", 448, 448, 224, 224, 2, 2);
		break;

		case RIESENSPINNE:
			pGegnerGrafix[RIESENSPINNE]->LoadImage("spiderboss_rumpf.png", 415, 160, 415, 160, 1, 1);
		break;

		case EVILHURRI:			
		break;

		case UFO:
			pGegnerGrafix[UFO]->LoadImage("ufo.bmp",   600, 658,200, 94, 3, 7);
		break;

		case FAHRSTUHLBOSS:
			pGegnerGrafix[FAHRSTUHLBOSS]->LoadImage("fahrstuhlboss_rahmen.bmp",   420, 280,420, 280, 1, 1);
		break;

		case FLUGBOSS:
			pGegnerGrafix[FLUGBOSS]->LoadImage("flugboss_main.png",   280, 285,280, 285, 1, 1);
		break;

		case BRATKLOPS:
			pGegnerGrafix[BRATKLOPS]->LoadImage("bratklops0000.bmp",   323, 400, 323, 400, 1, 1);
		break;

		case PARTIKELSPAWN:
		break;

		case GEGNERSTURM:
		break;

		case METALHEAD:
			pGegnerGrafix[METALHEAD]->LoadImage("metalhead_birne.bmp",   204, 242, 204, 242, 1, 1);
		break;

		case SKELETOR:
			pGegnerGrafix[SKELETOR]->LoadImage("skeletor.png", 1000, 400, 125, 200, 8, 2);
		break;

		case SKULL:
			pGegnerGrafix[SKULL]->LoadImage("skull.png", 256, 123, 32, 41, 8, 3);
		break;

		case EISFAUST:
			pGegnerGrafix[EISFAUST]->LoadImage("stahlfaust_eis.bmp", 512, 264, 256, 264, 2, 1);
		break;

		case FAHRSTUHL:
			pGegnerGrafix[FAHRSTUHL]->LoadImage("fahrstuhl.bmp", 400, 134, 400, 134, 1, 1);
		break;

		case FASS:
			pGegnerGrafix[FASS]->LoadImage("fass.png", 48, 56, 48, 56, 1, 1);
		break;

		case FEUERSPUCKER:
			pGegnerGrafix[FEUERSPUCKER]->LoadImage("flammenwerfer.bmp", 120, 120, 60, 60, 2, 2);
		break;

		case FADEMUSIC:
		break;

		case ENDLEVEL:
		break;

		case PLATTFORM:
			pGegnerGrafix[PLATTFORM]->LoadImage("plattform.bmp", 128, 64, 128, 32, 1, 2);
		break;

		case PRESSE:
			pGegnerGrafix[PRESSE]->LoadImage("presse.bmp", 100, 256, 100, 256, 1, 1);
		break;

		case SHRINE:
			pGegnerGrafix[SHRINE]->LoadImage("trenzshrine.bmp", 120, 180, 120, 180, 1, 1);
		break;

		case BRUECKE:
			pGegnerGrafix[BRUECKE]->LoadImage("bruecke.bmp", 10, 6, 10, 6, 1, 1);
		break;

		case FLOATING:
			pGegnerGrafix[FLOATING]->LoadImage("floating.bmp", 39, 14, 39, 14, 1, 1);
		break;

		case FLOATING2:
			pGegnerGrafix[FLOATING2]->LoadImage("floating2.bmp", 110, 18, 110, 18, 1, 1);
		break;

		case FLOATING3:
			pGegnerGrafix[FLOATING3]->LoadImage("floating3.bmp", 100, 54, 100, 27, 1, 2);
		break;

		case SURFBRETT:
			pGegnerGrafix[SURFBRETT]->LoadImage("surfbrett.bmp", 52, 35, 52, 35, 1, 1);
		break;

		case SHOOTBUTTON:
			pGegnerGrafix[SHOOTBUTTON]->LoadImage("shootplattformbutton.bmp", 32, 9, 16, 9, 2, 1);
		break;

		case SHOOTPLATTFORM:
			pGegnerGrafix[SHOOTPLATTFORM]->LoadImage("shootplattform.bmp", 100, 54, 100, 54, 1, 1);
		break;

		case GLUBSCHI:
			pGegnerGrafix[GLUBSCHI]->LoadImage("glubschi.bmp", 510, 350, 51, 120, 10, 3);
		break;

		case GLUBSCHI2:
			pGegnerGrafix[GLUBSCHI2]->LoadImage("glubschi2.bmp", 510, 350, 51, 120, 10, 3);
		break;

		case COLUMN:
			pGegnerGrafix[COLUMN]->LoadImage("column.bmp", 40, 100, 40, 100, 1, 1);
		break;

		case COLUMN2:
			pGegnerGrafix[COLUMN2]->LoadImage("column.bmp", 40, 100, 40, 100, 1, 1);
		break;

		case LIFT:
			pGegnerGrafix[LIFT]->LoadImage("lift.bmp", 80, 230, 80, 23, 1, 10);
		break;

		case SCHLEUSEH:
			pGegnerGrafix[SCHLEUSEH]->LoadImage("schleuse.bmp", 200, 40, 200, 40, 1, 1);
		break;

		case SCHLEUSEV:
			pGegnerGrafix[SCHLEUSEV]->LoadImage("schleuse2.bmp", 40, 200, 40, 200, 1, 1);
		break;

		case SWITCH:
			pGegnerGrafix[SWITCH]->LoadImage("switch.bmp", 48, 32, 24, 32, 2, 1);
		break;

		case WARNING:
			pGegnerGrafix[WARNING]->LoadImage("warning.png", 180, 40, 180, 40, 1, 1);
		break;

		case SOUNDTRIGGER:
		break;

		case LUEFTER_GROSS:
			pGegnerGrafix[LUEFTER_GROSS]->LoadImage("luefter_gross.png", 224, 223, 224, 223, 1, 1);
		break;

		case LUEFTER_KLEIN:
			pGegnerGrafix[LUEFTER_KLEIN]->LoadImage("luefter1.png",	 453,  343, 113,114, 4,3);
		break;

		case LUEFTER_KLEIN2:
			pGegnerGrafix[LUEFTER_KLEIN2]->LoadImage("luefter2.png",	 453,  343, 113,114, 4,3);
		break;

		case TUTORIALTEXT:
		break;

		case LIGHTFLARE:
			pGegnerGrafix[LIGHTFLARE]->LoadImage("lavaflare.bmp", 120, 120, 120, 120, 1,1);
		break;

		case MUSHROOM:
			pGegnerGrafix[MUSHROOM]->LoadImage("mushroom_head.png", 135, 61, 135, 61, 1,1);
		break;

		case PRESSWURST:
			pGegnerGrafix[PRESSWURST]->LoadImage("presswurst.png", 185, 357, 185, 357, 1,1);
		break;

		case GOLEM:
			pGegnerGrafix[GOLEM]->LoadImage("golem_rumpf.png", 185, 291, 185, 291, 1,1);
		break;

		case SPINNENMASCHINE:
			pGegnerGrafix[SPINNENMASCHINE]->LoadImage("spinnenmaschine_oben.png", 317, 413, 317, 413, 1,1);
		break;

		case THEWALL:
			pGegnerGrafix[THEWALL]->LoadImage("endbossmaschine.png", 285, 480, 285, 480, 1,1);
		break;

		case LAFASS:
			pGegnerGrafix[LAFASS]->LoadImage("lafass.png", 444, 495, 74, 165, 6,3);
		break;

		case GESCHUETZ:
			pGegnerGrafix[GESCHUETZ]->LoadImage("geschuetz.bmp", 300, 300, 60, 60, 5,5);
		break;

		case LAFASSSPAWNER:
			pGegnerGrafix[LAFASSSPAWNER]->LoadImage("lafass_spawner.png", 74, 165, 74, 165, 1,1);
		break;

		case FETTERAKETE:
			pGegnerGrafix[FETTERAKETE]->LoadImage("fetterakete.png", 250, 34, 25, 34, 10,1);
		break;

		case MINIROCKET:
			pGegnerGrafix[MINIROCKET]->LoadImage("minirocket.bmp", 12, 12, 12, 12, 1,1);
		break;

		case SCHABE:
			pGegnerGrafix[SCHABE]->LoadImage("schabe.bmp", 80, 60, 20, 20, 4,3);
		break;

		case MINIDRAGON:
			pGegnerGrafix[MINIDRAGON]->LoadImage("minidragon.png", 300, 141, 60, 47, 5,3);
		break;

		case SPIKELIFT:
			pGegnerGrafix[SPIKELIFT]->LoadImage("spikelift.bmp", 460, 80, 460, 40, 1, 2);
		break;

		case DRACHE:
			pGegnerGrafix[DRACHE]->LoadImage("drache_body.png", 201, 147, 201, 147, 1, 1);
		break;

		case ROLLMOPS:
			pGegnerGrafix[ROLLMOPS]->LoadImage("snowbomb.bmp", 480, 320, 80, 80, 6,4);
		break;

		case SCHNEEKOENIG:
			pGegnerGrafix[SCHNEEKOENIG]->LoadImage("Schneekoenig.png", 800, 600, 200, 200, 4,3);
		break;

		case SCHNEEKOPPE:
			pGegnerGrafix[SCHNEEKOPPE]->LoadImage("Schneekoppe.bmp", 14, 28, 14, 28, 1, 1);
		break;

		case ZITRONE:
			pGegnerGrafix[ZITRONE]->LoadImage("zitrone.png", 460, 243, 115, 81, 4,3);
		break;

		case ROTZPOTT:
			pGegnerGrafix[ROTZPOTT]->LoadImage("rotzpott.png", 55, 41, 55, 41, 1,1);
		break;

		case BIGFISH:
			pGegnerGrafix[BIGFISH]->LoadImage("bigfish.bmp", 208, 157, 208, 157, 1,1);
		break;

		case KETTENGLIED:
			pGegnerGrafix[KETTENGLIED]->LoadImage("rollmops_kette.png", 18, 36, 18, 18, 1, 2);
		break;

		case STACHELBEERE:
			pGegnerGrafix[STACHELBEERE]->LoadImage("stachelbeere.png", 360, 360, 60, 60, 6, 6);
		break;

		case TUBE:
			pGegnerGrafix[TUBE]->LoadImage("tube.png", 400, 220, 100, 220, 4, 1);
		break;

		case MUTANT:
			pGegnerGrafix[MUTANT]->LoadImage("mutant.bmp", 500, 450, 100, 90, 5, 5);
		break;


		default:
		break;
	} // switch
}
