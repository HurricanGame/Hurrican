// Datei : Menu.cpp

// -------------------------------------------------------------------------------------- 
//
// Menu-Klasse für das 
// Hurrican Hauptmenu
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include <sys/timeb.h>
#include <time.h>
#include <stdio.h>
#include "Main.h"
#include "Console.h"
#include "Menu.h"
#include "DX8Font.h"
#include "DX8Graphics.h"
#include "DX8Input.h"
#include "DX8Sound.h"
#include "Gameplay.h"
#include "GetKeyName.h"
#include "Logdatei.h"
#include "Player.h"
#include "PartikelSystem.h"
#include "Timer.h"

// --------------------------------------------------------------------------------------
// Die Credits
// --------------------------------------------------------------------------------------

	int CreditsCount;
	char *Credits[] =
	
	{

	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"- Hurrican- ",
	"",
	"(c) 2007",
	"poke53280",
	"",														
	"",														
	"",														
	"",
	"Jörg 'Eiswuxe' Winterstein",
	"",
	"Programmierung",
	"Gamedesign",
	"Sound & Musik",										
	"Leveldesign",												
	"",
	"",
	"",
	"Thomas 'Turri' Schreiter",
	"",
	"Grafik",
	"Gamedesign",
	"Gegnerdesign",
	"Leveldesign",											
	"",
	"",
	"",
	"Besonderer Dank geht an",
	"",
	"T.R.Schmidt und Gerhard 'ScHlAuChi' Weihrauch",
	"für zusätzliches Leveldesign und",
	"viele Verbesserungsvorschläge"
	"",
	"",
	"",
	"Spezieller Dank geht auch",
	"an unsere fleissigen Betatester",														
	"",
	"Wenkman, der Obertester",
	"Gereon 'ntroPi' Bartel",	
	"Kai 'Flanderz' Schmiegelt",
	"Michael 'Ram-Brand' Dahms",
	"Carsten 'Jazzoid' Przyluczky",
	"Heiko 'TheWanderer' Kalista",	
	"Matthias 'KingKiKapu' Klocke",	
	"Manuel 'Heimdall' Rülke",
	"Friedrich 'BadData' Volmering",	
	"Christian 'Gryphon' Kleinsteinberg",
	"Marc 'Dawn2069MS' Schmitt",
	"",
	"",
	"",
	"Zudem hier nochmal ein fettes Dankeschön an alle unsere Fans,",
	"die uns jahrelang motiviert haben und an uns glaubten.",
	"Danke, dass ihr Hurrican nie aufgegeben habt.",
	"Dieses Spiel ist für euch!",
	"",
	"",
	"",
	"Eiswuxe's Grüße gehen an",
	"",
	"Thorstein",
	"",														
	"Jens",
	"",
	"Herbie und Edi",
	"",		
	"Maroff und Anna",
	"",
	"Der Bumsmichl",
	"nebst Nina UND Linsensuppe",
	"",
	"Jarlock",	
	"",
	"Dan und Silke",
	"",														
	"Sebastian und Doro",
	"",
	"The one and only Markus Gottschalk",
	"",
	"Michael 'Skyrider' Matzka",
	"",
	"Andi, Simone und Johannes",
	"",			
	"Stef, Margot und Reinhard",
	"",
	"Dalama",
	"Manuel 'Heimdall' Rülke",
	"Lars Kranebitter",
	"",
	"Windigo-Design",
	"Friedrich 'BadData' Volmering",
	"Carsten 'Jazzoid' Przyluczky",
	"Andi, der alte Panner: *aufen!",
	"",														
	"Michael 'Gargoyle' Kreft",
	"",
	"Martin Lassahn",
	"der vielleicht einzige Mensch, der unseren",
	"Leidensweg nachvollziehen kann und auch noch teilt!",
	"Lass' dich nicht unterkriegen ;)",
	"",
	"Matthias Schindler, die alte Todesschlampe",
	"",
	"Marc Kamradt",
	"",
	"Peter Schraut",
	"",
	"Philipp Dortmann",
	"",
	"David Zaadstra",
	"",
	"Janosch Dalecke",
	"",
	"Florian Eisele",
	"",
	"Uwe Wegener mit Katrin und Vince ;)",
	"",
	"Der Rest der Zealkind Familie",
	"",
	"Nils 'Steele' Gey",
	"",														
	"Alle bei www.modarchive.com",							
	"",
	"ChrisCrusher",
	"",														
	"Alexander 'Jumperone' Krug",							
	"und Softgames",										
	"",														
	"Sechsta Sinn",
	"Haltet euch ran.... wir habens erstmal geschafft ;)",
	"",														
	"andUp",
	"",
	"Nicht zu vergessen:",
	"Steffan Kummer, der alte Hagestolz!",
	"",
	"Natürlich auch die Super Konrad Brothers,",
	"Dex, Bronko, RuvF, Aky, Contra-254 und",
	"alle anderen Turrican Fans!",
	"",
	"Pekaro Soft",
	"",	
	"Steve Harris :)",
	"",
	"Natürlich darf auch der wahre Meister nicht fehlen",
	"Einen fetten Gruß an den Manne!",
	"Wer fleissig sucht, der kann ihm",
	"Tief im Jungel seine Ehrerbietung darbringen...",
	"",	
	"Und das Beste kommt natürlich zum Schluß...",
	"Meine kleine Kathrin",
	"Ei laf juh ;)",
	"",
	"",
	"",														
	"Und jetzt ist der Turri dran:",
	"",														
	"ich grüße meine Mutti das wars...",
	"",														
	"",														
	"",														
	"",														
	"",														
	"",														
	"",
	"...noch lange nicht..",
	"",
	"ich grüße",
	"",
	"meine Schnegge, die ich über alles liebe",
	"",
	"meine Brüder - Carsten und Dirki -- der Lappen",
	"",
	"ich grüße Wenkman den Hardcore Tester",
	"",
	"alle aus dem Forum die viele Ideen zu Hurrican beigetragen haben",
	"und uns über 5 Jahre begleitet haben und uns die Treue gehalten haben."
	"",
	"ich grüße die anderen Pokes Wanderer und Skyrider und",
	"",
	"ich grüße Markus 'Snake' der sich immer saugut um unsere Webseite gekümmert hat",
	"",
	"ich grüße Peer Draeger  Wit Wit Wit",
	"",
	"dann den Rest aus meiner Arbeit",
	"Marc Schmidt, Christian Kleinsteinberg, Uwe Wegener, Tetjana Obly Wobly ;-)",
	"",
	"ich grüße Boris Triebel - Schau wir habens rausgebracht hehehe",
	"",
	"Marc Kamradt",
	"",
	"ich grüße Matthias die Todesschlampe",
	"",
	"Natürlich grüße ich auch Manfred Trenz",
	"der meine Jugend mit Turrican versüsst hat und für mich immer 'The Master' war",
	"",
	"ich grüße alle Jungs und Mädels die das hier lesen",
	"und Hurrican zocken und hoffe Ihr habt Spass :-)",
	"",
	"ich grüße Gigi und seine Katze .. (und auch Crazy)",
	"",
	"ich grüße Adnan den alten Partylöwen und guten Freund",
	"",
	"ich grüße alle aus rendering.de die mir oft mit Tipps",
	"bei meinen Grafiken geholfen haben",
	"",
	"und ich grüße den Krapfen",
	"",
	"ich grüße Thorsten Mutschall und Tanja Köhler",
	"und hoffe Sie haben Spass am spielen :-))",
	"ich grüße Micha Black Raven und seine verlorene seelen",
	"",
	"ich grüße alle vom GVC Gamevoiceclan",
	"the Best ever .. auch wenns jetzt vorbei ist ..",
	"",
	"ich grüße Velociraptor",
	"",
	"ich grüße alle die ich jetzt vergessen habe",
	"und das sind bestimmt ein paar :-)",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"#eoc"			// end of credits =)

	};

	int tempNUMPLAYERS;

// --------------------------------------------------------------------------------------
// Konstruktor : Menu Grafiken laden und Werte initialisieren
// --------------------------------------------------------------------------------------

MenuClass::MenuClass(void)
{
	MenuNebel.LoadImage("Hurrican_rund.bmp", 512, 512, 512, 512, 1, 1);
	MenuTitel.LoadImage("Hurrican-logo.bmp", 400, 90, 400, 90, 1, 1);

	Skills.LoadImage("Skills.png", 200, 200, 100, 100, 2, 2);

	MenuStar.LoadImage("Star.bmp", 5, 5, 5, 5, 1, 1);
	MenuStar.SetRect(0, 0, 5, 5);

	// Menu Werte initialisieren
	ScrollPos = 0.0f;

	xpos = (640-373) / 2;
	ypos = 80;

	Rotation    = 0.0f;
	RotationDir = 1;

	AktuellerZustand = MENUZUSTAND_MAINMENU;
	AktuellerPunkt   = MENUPUNKT_STARTGAME;

	// Sterne initialisieren
	for (int i=0; i<MAX_STARS; i++)
	{
		Stars[i].Count   = float(rand()%628) / 100.0f;
		Stars[i].Abstand = float(rand()%400 + 20);
		Stars[i].Ebene   = rand()%120+20;
	}

	// Farben für die Highscore erstellen, in der die Namen aufblinken
	int i = 0;
	HighscoreColors[i] = D3DCOLOR_RGBA( 75,  75,  75, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(105, 105, 105, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(135, 135, 135, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(165, 165, 165, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(195, 195, 195, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(225, 225, 225, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 255, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 225, 225, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 195, 195, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 165, 165, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 135, 135, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 105, 105, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,  75,  75, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,  45,  45, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,  15,  15, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,   0,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,  15,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,  45,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,  75,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 105,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 135,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 165,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 195,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 225,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 255,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(225, 255,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(195, 255,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(165, 255,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(135, 255,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(105, 255,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 75, 255,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 45, 255,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 15, 255,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255,  15, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255,  45, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255,  75, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 105, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 135, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 165, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 195, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 225, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 15, 255, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 45, 255, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 75, 255, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(105, 255, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(135, 255, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(165, 255, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(195, 255, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(225, 255, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 255, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 255, 225, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 240, 195, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 225, 165, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 210, 135, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 195, 105, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 180,  75, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 165,  45, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 165,  15, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 150,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 135,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 105,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,  75,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,  45,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,  15,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,   0,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,   0,  15, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,   0,  45, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,   0,  75, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,   0, 105, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,   0, 135, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,   0, 165, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,   0, 195, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,   0, 225, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,   0, 254, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(225,  15, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(195,  45, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(165,  75, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(135, 105, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(105, 135, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 75, 165, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 45, 195, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 15, 225, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 225, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 195, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 165, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 135, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 105, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255,  75, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255,  45, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255,  15, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(  0, 225,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 15, 195,  15, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 30, 180,  30, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 45, 165,  45, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 60, 150,  60, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 75, 135,  75, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 75, 120,  75, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 75, 105,  75, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA( 75,  90,  75, 255); i++;

	// Die ersten 20 nochmal ...

	HighscoreColors[i] = D3DCOLOR_RGBA( 75,  75,  75, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(105, 105, 105, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(135, 135, 135, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(165, 165, 165, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(195, 195, 195, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(225, 225, 225, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 255, 255, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 225, 225, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 195, 195, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 165, 165, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 135, 135, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 105, 105, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,  75,  75, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,  45,  45, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,  15,  15, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,   0,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,  15,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,  45,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255,  75,   0, 255); i++;
	HighscoreColors[i] = D3DCOLOR_RGBA(255, 105,   0, 255); i++;

	BlinkOffset  = 0;
	BlinkCounter = 0.75f;

	
	// Sprache feststellen und schauen, welche Grafik wir anzeigen müssen
	if (strcmp (TextArray[TEXT_BENUTZTE_GRAFIK], "0") == 0)
		Sprachgrafik = 0;		// deutsch ?
	else
		Sprachgrafik = 1;		// oder doch englisch ?

	// Highscore laden
	//
	LoadHighscore();

	ShowLanguageInfoCounter = 0.0f;
	locked = false;

	// creditscount rausfinden
	CreditsCount = 0;
	while (strcmp(Credits[CreditsCount], "#eoc") != 0)
		CreditsCount++;
}

// --------------------------------------------------------------------------------------
// Konstruktor : Menu Grafiken freigeben
// --------------------------------------------------------------------------------------

MenuClass::~MenuClass(void)
{
	// Aktuelle Highscoreliste speichern
	//
	SaveHighscore();
}

// --------------------------------------------------------------------------------------
// Hintergrund der Menüs anzeigen
// wird auch für den Progressbar benutzt
// --------------------------------------------------------------------------------------

void MenuClass::ShowMenuBack(void)
{
	MenuNebel.RenderSpriteScaledRotated(-80, -140, 800, 800, ScrollPos, 0xFFFFFFFF);

	DirectGraphics.SetAdditiveMode();
	DirectGraphics.SetFilterMode(true);

	MenuStar.SetRect(0, 0, 5, 5);

	// Sterne anzeigen
	for (int i=0; i<MAX_STARS; i++)
		MenuStar.RenderSpriteRotated(320 - float(sin(Stars[i].Count) * Stars[i].Abstand),
									 240 + float(cos(Stars[i].Count) * Stars[i].Abstand), 
									 Stars[i].Count, D3DCOLOR_RGBA(255, 255, 255, Stars[i].Ebene));

	DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// festlegen, welche aktionen für den spieler wählbar sind
// abhängig vom controlmode und joymode
// --------------------------------------------------------------------------------------

void MenuClass::FillPossibleKeys(void)
{
	for (int p = 0; p < 2; p++)
	{
		for (int i = 0; i < MAX_AKTIONEN; i++)
			PossibleKeys[p][i] = true;

		PlayerClass *pCurrentPlayer;

		if (p == 0)
			pCurrentPlayer = pPlayer[0];
		else
			pCurrentPlayer = pPlayer[1];
			
		// Laufen, Ducken und Hoch/Runtersehen Tasten für Joystick/Pad deaktivieren
		if (pCurrentPlayer->ControlType == JOYMODE_STICK)
		{
			//PossibleKeys[p][0] = false;
			//PossibleKeys[p][1] = false;
			//PossibleKeys[p][2] = false;
			//PossibleKeys[p][3] = false;
			//PossibleKeys[p][4] = false;

			// Springen für Joystick deaktivieren
			if (pCurrentPlayer->JoystickMode == JOYMODE_STICK)
				PossibleKeys[p][5] = false;
		}
	}								
}

// --------------------------------------------------------------------------------------
// Menu anzeigen
// --------------------------------------------------------------------------------------

void MenuClass::ShowMenu(void)
{
	D3DCOLOR menucolor;
	D3DCOLOR menucolor2;
	D3DCOLOR menucolor3;

	int alpha = int (Rotation);
	menucolor  = D3DCOLOR_RGBA(255, 255, 255, alpha);
	menucolor2 = D3DCOLOR_RGBA(255, 255, 255, alpha / 2);
	menucolor3 = D3DCOLOR_RGBA(255, 255, 255, alpha / 5);

	// Hintergrund rotieren lassen
	ScrollPos += 0.1f SYNC;

	if (ScrollPos > 360.0f)
		ScrollPos -= 360.0f;

	// Total löschen
	lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                      D3DCOLOR_XRGB(0,0,0),	1.0f, 0);

	ShowMenuBack();

	if (AktuellerZustand != MENUPUNKT_CREDITS &&
		AktuellerZustand != MENUZUSTAND_ENTERNAME)

	pDefaultFont->DrawTextCenterAlign(320.0f, 462, TextArray [TEXT_MENUE_ANLEITUNG], menucolor, 0);
	pDefaultFont->DrawText(10.0f, 462, "www.poke53280.de", menucolor, 0);
	pDefaultFont->DrawTextRightAlign(620.0f, 462, "www.hurrican-game.de", menucolor, 0);

	// graue Linien
	//
/*
	RenderRect(0, 33, 156, 1, menucolor2);
	RenderRect(0, 63, 156, 1, menucolor2);

	RenderRect(493, 33, 150, 1, menucolor2);
	RenderRect(493, 63, 150, 1, menucolor2);
*/

	RenderRect(0, 455, 640, 1, menucolor2);

	MenuTitel.RenderSprite((640 - 400) / 2 + 15, 0, menucolor);
	DirectGraphics.SetAdditiveMode();	
	

	// Hintergrund des Menus anzeigen
	/*if (AktuellerZustand != MENUPUNKT_CREDITS &&
		AktuellerZustand != MENUZUSTAND_ENTERNAME)
		MenuBackground.RenderSprite(xpos, ypos, 0xFFFFFFFF);*/	

	const int OFFSET = 80;

	// Je nach Zustand das richtige Menu anzeigen
	switch (AktuellerZustand)
	{
		case MENUZUSTAND_MAINMENU :
		{			
			for (int i=0; i<6; i++)
				if (AktuellerPunkt != i)
				{
					if (i == 1)
					{
						if (Stage == -1)
							pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + i*35, TextArray [TEXT_MENUE_SPIEL_STARTEN+i], menucolor3, 2);
						else
							pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + i*35, TextArray [TEXT_MENUE_SPIEL_STARTEN+i], menucolor2, 2);
					}
					else
						pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + i*35, TextArray [TEXT_MENUE_SPIEL_STARTEN+i], menucolor2, 2);
				}
				
			pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + AktuellerPunkt*35, TextArray [TEXT_MENUE_SPIEL_STARTEN+AktuellerPunkt], menucolor, 2);

		} break;

		case MENUZUSTAND_STARTGAME :
		{			
		} break;

		case MENUZUSTAND_VOLUMES :
		{
			const int OFFSET2 = 20;

			float d = (float)(pMenuFont->StringLength(TextArray [TEXT_MENUE_EINSTELLUNGEN], 2));
			pMenuFont->DrawText(320 - d/2.0f, ypos + OFFSET - OFFSET2, TextArray [TEXT_MENUE_EINSTELLUNGEN], menucolor, 2);			

			// Sound / Musik Lautstärke
			for (int i=0; i<2; i++)
			{
				float d = (float)(pMenuFont->StringLength(TextArray [TEXT_SOUND + i], 2));

				if (i == 0)
					LoadingBar.SetRect(0, 0, int(pSoundManager->its_GlobalSoundVolume / 100.0f * 318.0f), 19);
				else
					LoadingBar.SetRect(0, 0, int(pSoundManager->its_GlobalMusicVolume / 100.0f * 318.0f), 19);

				if (i == AktuellerPunkt)
				{					
					pMenuFont->DrawText (xpos + OFFSET - 20 - d, ypos + OFFSET - OFFSET2 + (i+2) * 35, TextArray[TEXT_SOUND + i], menucolor, 2);
					LoadingScreen.RenderSprite (220, ypos + OFFSET - OFFSET2 + (i+2) * 35 - 18, menucolor);
					LoadingBar.RenderSprite    (241, ypos + OFFSET - OFFSET2 + (i+2) * 35 +2, menucolor);
				}
				else
				{
					pMenuFont->DrawText (xpos + OFFSET - 20 - d, ypos + OFFSET - OFFSET2 + (i+2) * 35, TextArray[TEXT_SOUND + i], menucolor2, 2);
					LoadingScreen.RenderSprite (220, ypos + OFFSET - OFFSET2 + (i+2) * 35 - 18, menucolor2);
					LoadingBar.RenderSprite    (241, ypos + OFFSET - OFFSET2 + (i+2) * 35 +2, menucolor2);
				}			
			}

			pSoundManager->SetAllSongVolumes();

			//Sonstige Menu-Punkte anzeigen
			for (int i=2; i<4; i++)
			{				
				float d = (float)(pMenuFont->StringLength(TextArray [TEXT_SOUND + i], 2));				

				// Schrift anzeigen
				//
				if (i == AktuellerPunkt)
					pMenuFont->DrawText (320.0f - d / 2.0f, ypos + OFFSET - OFFSET2 + (i+2) * 35, TextArray[TEXT_SOUND + i], menucolor, 2);
				else
					pMenuFont->DrawText (320.0f - d / 2.0f, ypos + OFFSET - OFFSET2 + (i+2) * 35, TextArray[TEXT_SOUND + i], menucolor2, 2);

			}

			// Detailstufe
			d = (float)(pMenuFont->StringLength(TextArray [TEXT_DETAIL_LOW + options_Detail], 2));				
			if (4 == AktuellerPunkt)
				pMenuFont->DrawText (320.0f - d / 2.0f, ypos + OFFSET - OFFSET2 + (4+2) * 35, TextArray[TEXT_DETAIL_LOW + options_Detail], menucolor, 2);
			else
				pMenuFont->DrawText (320.0f - d / 2.0f, ypos + OFFSET - OFFSET2 + (4+2) * 35, TextArray[TEXT_DETAIL_LOW + options_Detail], menucolor2, 2);


		} break; // MENUZUSTAND_VOLUMES

		case MENUZUSTAND_LANGUAGE :
		{
			float d = (float)(pMenuFont->StringLength(TextArray [TEXT_SPRACHE], 2));
			pMenuFont->DrawText(320 - d/2.0f, ypos + OFFSET, TextArray [TEXT_SPRACHE], menucolor, 2);

			for (int i = 0; i < LanguageFileCount; i++)
			{
				pDefaultFont->DrawText (320 - pDefaultFont->StringLength (LanguageFiles[i]) / 2.0f, ypos + 120 + i * 16, LanguageFiles[i], 0x88FFFFFF);
				if (AktuellerPunkt == i)
					pDefaultFont->DrawText (320 - pDefaultFont->StringLength (LanguageFiles[i]) / 2.0f, ypos + 120 + i * 16, LanguageFiles[i], 0x88FFFFFF);
			}

			pDefaultFont->DrawText (320 - pDefaultFont->StringLength (TextArray[TEXT_ZURUECK]) / 2.0f, ypos + 136 + 16 * LanguageFileCount, (TextArray[TEXT_ZURUECK]), 0x88FFFFFF);
			if (AktuellerPunkt == LanguageFileCount)
				pDefaultFont->DrawText (320 - pDefaultFont->StringLength (TextArray[TEXT_ZURUECK]) / 2.0f, ypos + 136 + 16 * LanguageFileCount, (TextArray[TEXT_ZURUECK]), 0x88FFFFFF);

			
		} break; // MENUZUSTAND_LANGUAGE

		case MENUZUSTAND_TASTEN :
		{			
			const int TASTENLINKS = 10;
			const int SPIELER1OFFSET = 100;
			const int SPIELER2OFFSET = 200;

			FillPossibleKeys();			

			float d = (float)(pMenuFont->StringLength(TextArray [TEXT_STEUERUNG], 2));
			pMenuFont->DrawText(320 - d/2.0f, ypos + OFFSET - 60, TextArray[TEXT_STEUERUNG], menucolor, 2);

			// Überschrift "Spieler1" "Spieler2"
			if (CurrentPlayer == 0)
			{
				RenderRect(xpos + SPIELER1OFFSET - 4, ypos + 70 - 2, (float)pDefaultFont->StringLength(TextArray[TEXT_PLAYER_ONE], 1) + 8, 16, 0x30FFFFFF);
				pDefaultFont->DrawText (xpos + SPIELER1OFFSET, ypos + 70, TextArray[TEXT_PLAYER_ONE], 0xFFFFFFFF);
				pDefaultFont->DrawText (xpos + SPIELER1OFFSET + SPIELER2OFFSET, ypos + 70, TextArray[TEXT_PLAYER_TWO], 0x88FFFFFF);
			}
			else
			{
				RenderRect(xpos + SPIELER1OFFSET + SPIELER2OFFSET - 4, ypos + 70 - 2, (float)pDefaultFont->StringLength(TextArray[TEXT_PLAYER_TWO], 1) + 8, 16, 0x30FFFFFF);
				pDefaultFont->DrawText (xpos + SPIELER1OFFSET, ypos + 70, TextArray[TEXT_PLAYER_ONE], 0x88FFFFFF);
				pDefaultFont->DrawText (xpos + SPIELER1OFFSET + SPIELER2OFFSET, ypos + 70, TextArray[TEXT_PLAYER_TWO], 0xFFFFFFFF);
			}			

			// Typ, Modus und Empfindlichkeit
			if (AktuellerPunkt == 0)
				pDefaultFont->DrawTextRightAlign (xpos + TASTENLINKS, ypos + 96, TextArray[TEXT_TYP], 0xFFFFFFFF);
			else
				pDefaultFont->DrawTextRightAlign (xpos + TASTENLINKS, ypos + 96, TextArray[TEXT_TYP], 0x88FFFFFF);

			if (AktuellerPunkt == 1)
				pDefaultFont->DrawTextRightAlign (xpos + TASTENLINKS, ypos + 116, TextArray[TEXT_MODUS], 0xFFFFFFFF);
			else
				pDefaultFont->DrawTextRightAlign (xpos + TASTENLINKS, ypos + 116, TextArray[TEXT_MODUS], 0x88FFFFFF);

			if (AktuellerPunkt == 2)
				pDefaultFont->DrawTextRightAlign (xpos + TASTENLINKS, ypos + 134, TextArray[TEXT_EMPFINDLICHKEIT], 0xFFFFFFFF);			
			else
				pDefaultFont->DrawTextRightAlign (xpos + TASTENLINKS, ypos + 134, TextArray[TEXT_EMPFINDLICHKEIT], 0x88FFFFFF);			


			// Beschreibung links
			for (int i = 0; i < 12; i++)
			{
				if (PossibleKeys[CurrentPlayer][i] == false)
					continue;

				pDefaultFont->DrawTextRightAlign (xpos + TASTENLINKS, ypos + 170 + i * 16, TextArray[TEXT_TASTEN_L + i], 0x88FFFFFF);

				if (AktuellerPunkt - 3 == i)
					pDefaultFont->DrawTextRightAlign (xpos + TASTENLINKS, ypos + 170 + i * 16, TextArray[TEXT_TASTEN_L + i], 0x88FFFFFF);
			}

			float yoff;

			if (AktuellerPunkt >= 3)
				yoff = ypos + 120 + AktuellerPunkt * 16.0f;
			else
				yoff = ypos + 95 + AktuellerPunkt * 18.0f;

			RenderRect(20, yoff, 600, 16, 0x24FFFFFF);


			// Für beide Spieler den ganzen Klumbatsch anzeigen
			for (int j = 0; j < 2; j++)
			{				
				D3DCOLOR col;
				PlayerClass* pCurrentPlayer;
				
				if (j == 0)
					pCurrentPlayer = pPlayer[0];
				else
					pCurrentPlayer = pPlayer[1];

				// Typ (Tastatur, Joystick (mit Name)
				if (CurrentPlayer == j &&
					AktuellerPunkt == 0)
					col = 0xFFFFFFFF;
				else
					col = 0x88FFFFFF;
				
				if (pCurrentPlayer->ControlType == JOYMODE_KEYBOARD)
					pDefaultFont->DrawText (xpos + SPIELER1OFFSET + j * SPIELER2OFFSET, ypos + 98, TextArray[TEXT_KEYBOARD], col);
				else					
					pDefaultFont->DrawText (xpos + SPIELER1OFFSET + j * SPIELER2OFFSET, ypos + 98, DirectInput.Joysticks[pCurrentPlayer->JoystickIndex].JoystickName, col);

				// Rechteck für Empfindlichkeit
				if (pCurrentPlayer->ControlType != JOYMODE_KEYBOARD)
				{
					if (CurrentPlayer == j &&
						AktuellerPunkt == 2)
						col = 0xFFFFFFFF;
					else
						col = 0x88FFFFFF;
					
					RenderRect(xpos + SPIELER1OFFSET - 1 + j * SPIELER2OFFSET, ypos + 134, 102, 1, col);
					RenderRect(xpos + SPIELER1OFFSET - 1 + j * SPIELER2OFFSET, ypos + 145, 102, 1, col);
					RenderRect(xpos + SPIELER1OFFSET - 2 + j * SPIELER2OFFSET, ypos + 135, 1, 10, col);
					RenderRect(xpos + SPIELER1OFFSET + 101 + j * SPIELER2OFFSET, ypos + 135, 1, 10, col);			

					// Aktueller Balken
					RenderRect(xpos + SPIELER1OFFSET + j * SPIELER2OFFSET, ypos + 136, (1000.0f - (pCurrentPlayer->JoystickSchwelle + 100.0f))/8.0f, 8, col);

					// Modus
					if (CurrentPlayer == j &&
						AktuellerPunkt == 1)
						col = 0xFFFFFFFF;
					else
						col = 0x88FFFFFF;

					if (pCurrentPlayer->JoystickMode == JOYMODE_STICK)
						pDefaultFont->DrawText (xpos + SPIELER1OFFSET + j * SPIELER2OFFSET, ypos + 116, TextArray[TEXT_JOYMODE_STICK], col);
					else
						pDefaultFont->DrawText (xpos + SPIELER1OFFSET + j * SPIELER2OFFSET, ypos + 116, TextArray[TEXT_JOYMODE_PAD], col);
				}
			
				for (int i = 0; i < 12; i++)
				{
					D3DCOLOR col;

					if (AktuellerPunkt -3 == i)
						col = 0xFFFFFFFF;
					else
						col = 0x88FFFFFF;

					if (CurrentPlayer != j)
					{
						// Bestimmte Tasten werden ausgeblendet, wenn joymode = joystick oder joypad
						if (PossibleKeys[j][i] == false)
							continue;						

						// Nicht definiert?
						if (pCurrentPlayer->AktionKeyboard[i] == -1 &&
						   (pCurrentPlayer->AktionJoystick[i] == -1 ||
							JoystickFound == false))							
							pDefaultFont->DrawText (xpos + SPIELER1OFFSET + j * SPIELER2OFFSET, ypos + 170 + i * 16, TextArray[TEXT_NICHT_DEFINIERT], 0x88FFFFFF);						
						else

						// Taste anzeigen?
						if (pCurrentPlayer->AktionKeyboard[i] != -1)
							pDefaultFont->DrawText (xpos + SPIELER1OFFSET + j * SPIELER2OFFSET, ypos + 170 + i * 16, GetKeyName (pCurrentPlayer->AktionKeyboard[i]), 0x88FFFFFF);

						// oder Button
						else
						{
							char Buf[64];
							sprintf_s (Buf, "Button nr %i", pCurrentPlayer->AktionJoystick[i]);
							pDefaultFont->DrawText (xpos + SPIELER1OFFSET + j * SPIELER2OFFSET, ypos + 170 + i * 16, Buf, 0x88FFFFFF);
						}
					}
					else
					if (locked == false || AktuellerPunkt - 3 != i)
					{
						// Bestimmte Tasten werden ausgeblendet, wenn joymode = joystick oder joypad
						if (PossibleKeys[j][i] == false)
							continue;

						// Nicht definiert?
						if (pCurrentPlayer->AktionKeyboard[i] == -1 &&
						   (pCurrentPlayer->AktionJoystick[i] == -1 ||
							JoystickFound == false))							
							pDefaultFont->DrawText (xpos + SPIELER1OFFSET + j * SPIELER2OFFSET, ypos + 170 + i * 16, TextArray[TEXT_NICHT_DEFINIERT], col);
						else

						// Taste anzeigen?
						if (pCurrentPlayer->AktionKeyboard[i] != -1)
							pDefaultFont->DrawText (xpos + SPIELER1OFFSET + j * SPIELER2OFFSET, ypos + 170 + i * 16, GetKeyName (pCurrentPlayer->AktionKeyboard[i]), col);

						// oder Button
						else
						{
							char Buf[64];
							sprintf_s (Buf, "Button nr %i", pCurrentPlayer->AktionJoystick[i]);
							pDefaultFont->DrawText (xpos + SPIELER1OFFSET + j * SPIELER2OFFSET, ypos + 170 + i * 16, Buf, col);
						}						
					}
					else
					{
						if (pCurrentPlayer->ControlType == JOYMODE_KEYBOARD)
							pDefaultFont->DrawText (xpos + SPIELER1OFFSET + j * SPIELER2OFFSET, ypos + 170 + i * 16, TextArray[TEXT_TASTEN_NEU_T], col);
						else
							pDefaultFont->DrawText (xpos + SPIELER1OFFSET + j * SPIELER2OFFSET, ypos + 170 + i * 16, TextArray[TEXT_TASTEN_NEU_B], col);

					}
				}
			}
		} break; // MENUZUSTAND_TASTEN

		case MENUZUSTAND_BUTTONS :
		{
			const int OFFSET2 = 60;

			//float d = (float)(pMenuFont->StringLength(TextArray [TEXT_BUTTONS], 2));
			//pMenuFont->DrawText(320 - d/2.0f, ypos + OFFSET - OFFSET2, TextArray [TEXT_BUTTONS], menucolor, 2);

			// Einstellungen für ForceFeedback und Stickmodus
			//
			for (int i = 0; i < 3; i++)
			{
				float d = (float)(pMenuFont->StringLength(TextArray [TEXT_JOYMODE+i], 2));				

				// forcefeedback? dann freilassen fürs Kästchen
				//
				if (i == 1)
				{
					d += 20;

					// Kästchen zeichnen
					//
					if (UseForceFeedback == false)
					{
						if (i == AktuellerPunkt)
							MenuKasten[0].RenderSprite(320.0f + d / 2.0f, ypos + OFFSET - OFFSET2 + (i+2) * 35 - 12, menucolor);
						else
							MenuKasten[0].RenderSprite(320.0f + d / 2.0f, ypos + OFFSET - OFFSET2 + (i+2) * 35 - 12, menucolor2);
					}
					else
					{
						if (i == AktuellerPunkt)
							MenuKasten[1].RenderSprite(320.0f + d / 2.0f, ypos + OFFSET - OFFSET2 + (i+2) * 35 - 12, menucolor);
						else
							MenuKasten[1].RenderSprite(320.0f + d / 2.0f, ypos + OFFSET - OFFSET2 + (i+2) * 35 - 12, menucolor2);
					}
				}

				// Empfindlichkeit
				//
				if (i == 2)
				{
					d += 350;

					// Rahmen zeichnen
					//
//					LoadingBar.SetRect(0, 0, 318 - int(DirectInput.JoystickSchwelle / 1000.0f * 318.0f), 19);

					if (i == AktuellerPunkt)
					{					
						LoadingScreen.RenderSprite (260, ypos + OFFSET - OFFSET2 + (i+2) * 35 - 18, menucolor);
						LoadingBar.RenderSprite    (281, ypos + OFFSET - OFFSET2 + (i+2) * 35 +2, menucolor);
					}
					else
					{
						LoadingScreen.RenderSprite (260, ypos + OFFSET - OFFSET2 + (i+2) * 35 - 18, menucolor2);
						LoadingBar.RenderSprite    (281, ypos + OFFSET - OFFSET2 + (i+2) * 35 +2, menucolor2);
					}			
				}

				char buf[100];

				if (i != 0)
					strcpy_s(buf, strlen(TextArray [TEXT_JOYMODE + i]) + 1, TextArray [TEXT_JOYMODE + i]);
				else
				{
/*					if (DirectInput.Joysticks[pPlayer->JoystickIndex].JoystickMode == JOYMODE_STICK)
						sprintf_s (buf, "%s %s", TextArray [TEXT_JOYMODE], TextArray [TEXT_JOYMODE_STICK]);
					else
						sprintf_s (buf, "%s %s", TextArray [TEXT_JOYMODE], TextArray [TEXT_JOYMODE_PAD]);
*/

					d = (float)(pMenuFont->StringLength(buf));
				}

				if (i == AktuellerPunkt)
					pMenuFont->DrawText(320 - d/2.0f, ypos + OFFSET - OFFSET2 + (i+2) * 35, buf, menucolor, 2);
				else
					pMenuFont->DrawText(320 - d/2.0f, ypos + OFFSET - OFFSET2 + (i+2) * 35, buf, menucolor2, 2);

			}			

			// Buttons
			//
			for (int i = 5; i < 11; i++)
			{
/*
				char bnr[20];

				//if (i == 5 && DirectInput.JoystickMode == JOYMODE_STICK)
//					continue;

				pDefaultFont->DrawText (xpos +  70, ypos + 120 + i * 16, TextArray[TEXT_TASTEN_L + i], 0x88FFFFFF);

				if (locked == false || AktuellerPunkt != i)
				{
					sprintf_s (bnr, "%i", pPlayer->AktionJoystick[i]);
					pDefaultFont->DrawText (xpos + 220, ypos + 120 + i * 16, "Button nr ", 0x88FFFFFF);
					pDefaultFont->DrawText (xpos + 285, ypos + 120 + i * 16, bnr, 0x88FFFFFF);
				}
				
				if (AktuellerPunkt == i - 2)
				{
					//pDefaultFont->DrawText (xpos + 70, ypos + 120 + i * 16, TextArray[TEXT_TASTEN_L + i], 0x88FFFFFF);

					if (locked == false)
					{
						sprintf_s (bnr, "%i", pPlayer->AktionJoystick[i]);
						pDefaultFont->DrawText (xpos + 220, ypos + 120 + i * 16, "Button nr ", 0x88FFFFFF);
						pDefaultFont->DrawText (xpos + 285, ypos + 120 + i * 16, bnr, 0x88FFFFFF);
					}

					pDefaultFont->DrawText (xpos + 70, ypos + 120 + i * 16, TextArray[TEXT_TASTEN_L + i], 0x88FFFFFF);
				}
*/
			}			
		} break; // MENUZUSTAND_BUTTONS

		case MENUPUNKT_HIGHSCORES :
		{
			char Buffer[100];		// Für itoa

			float d = (float)(pMenuFont->StringLength(TextArray [TEXT_MENUE_HIGHSCORES], 2));
			pMenuFont->DrawText(320 - d/2.0f, ypos, TextArray [TEXT_MENUE_HIGHSCORES], menucolor, 2);

			//pMenuFont->DrawText(xpos-120,  ypos+55, TextArray [TEXT_HIGHSCORE_PLATZ],  0xFFFFFFFF, 2);
			pMenuFont->DrawText(xpos-120,  ypos+55, TextArray [TEXT_HIGHSCORE_NAME],   0xFFFFFFFF, 2);
			pMenuFont->DrawTextRightAlign(xpos+270, ypos+55, TextArray [TEXT_HIGHSCORE_PUNKTE], 0xFFFFFFFF, 2);
			pMenuFont->DrawTextCenterAlign(xpos+350, ypos+55, TextArray [TEXT_HIGHSCORE_STAGE],  0xFFFFFFFF, 2);			
			pMenuFont->DrawText(xpos+420, ypos+55, TextArray [TEXT_HIGHSCORE_SKILL],  0xFFFFFFFF, 2);

			for (int i=0; i<MAX_HIGHSCORES; i++)
			{
				D3DCOLOR Color;

				Color = HighscoreColors[i+BlinkOffset];

				//_itoa_s(i+1, Buffer, 10);
				//pMenuFont->DrawText(xpos-90, ypos+float(105+i*26), Buffer, Color, 2);

				pMenuFont->DrawText(xpos-120, ypos+float(105+i*26), Highscores[i].Name, Color, 2);

				_itoa_s(Highscores[i].Score, Buffer, 10);
				pMenuFont->DrawTextRightAlign(xpos+270, ypos+float(105+i*26), Buffer, Color, 2);

				_itoa_s(Highscores[i].Stage, Buffer, 10);
				pMenuFont->DrawTextCenterAlign(xpos+350, ypos+float(105+i*26), Buffer, Color, 2);

				Skills.RenderSpriteScaled(xpos+422, ypos+float(105+i*26), 30, 30, Highscores[i].Skill, 0xFFFFFFFF);				
			}

			// Highscore-Farben blinken lassen
			BlinkCounter -= SpeedFaktor;
			if (BlinkCounter < 0.0f)
			{
				BlinkCounter = 0.75f;
				BlinkOffset++;
				if (BlinkOffset > MAX_COLORS - MAX_HIGHSCORES)
					BlinkOffset = 0;
			}

		} break; // HIGHSCORE

		case MENUZUSTAND_ENTERNAME :
		{
			char Buffer[100];		// Für itoa

			pMenuFont->DrawTextCenterAlign(320, ypos + 90, TextArray[TEXT_WAHNSINN], D3DCOLOR_RGBA(255, 255, 255, 255), 2);			

			strcpy_s(Buffer, strlen(TextArray[TEXT_NEUE_HIGHSCORE]) + 1, TextArray[TEXT_NEUE_HIGHSCORE]);
			pMenuFont->DrawTextCenterAlign(320, ypos + 180, TextArray[TEXT_NAMEN_EINGEBEN], D3DCOLOR_RGBA(255, 255, 255, 255), 2);

			sprintf_s(Buffer, "%s %d", Buffer, NewRank+1);

			pMenuFont->DrawTextCenterAlign(320, ypos + 150, Buffer, D3DCOLOR_RGBA(255, 255, 255, 255), 2);
		} break; // ENTERNAME

		case MENUPUNKT_CREDITS :
		{
			// Credits anzeigen
			int i = 0;
			for (i=0; i<40; i++)
			{
				D3DCOLOR Color;

				// Transparenz richtig setzen
				int alpha = int(i*12-CreditsPosition);

				if (alpha > 360)
				{
					alpha = 255 - (alpha - 360) * 4;

					if (alpha < 0)
						alpha = 0;
				}
				else
				if (alpha < 150)
				{
					alpha = 255 - (150 - alpha) * 4;

					if (alpha < 0)
						alpha = 0;
				}
				
				else
					alpha = 255;

				Color = D3DCOLOR_RGBA(255, 255, 255, alpha);

				pDefaultFont->DrawTextCenterAlign(320.0f,
												  float(int(i*12-CreditsPosition)), 
												  Credits[CreditsOffset+i], Color, 0);
			}

			// Credits scrollen
			CreditsCounter -= 0.7f SYNC;			// Credits-Delay veringern

			// Bei Taste nach unten schneller scrollen
			//
			if (KeyDown(DIK_DOWN) ||
				KeyDown(DIK_NUMPAD2))
				CreditsCounter -= 2.0f SYNC;

			// Bei Taste nach oben langsamer werden
			//
			if (KeyDown(DIK_UP) ||
				KeyDown(DIK_NUMPAD8))
				CreditsCounter = 0.5f;

			while (CreditsCounter < 0.0f)				// Ein Pixel weiterscrollen ?
			{
				CreditsCounter += 0.5f;				// Dann neues Delay setzen
				CreditsPosition++;					// und Pixel weiterscrollen
				if (CreditsPosition >= 12)			// 12 Pixelgrenze überschritten ?
				{
					CreditsPosition = 0;			// Dann wieder zurücksetzen
					CreditsOffset++;				// Und im Char-Array eine zeile weiter

					// Credits zuende ?
					// Dann von vorne beginnen
					//
					if (strcmp ("#eoc", Credits[CreditsOffset+i]) == 0)
						CreditsOffset = 0;
				}
			}

		} break; // CREDITS

		case MENUZUSTAND_NEWGAME :
		{
			pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET, TextArray [TEXT_MENUE_SPIEL_STARTEN], menucolor, 2);

			for (int i=0; i<3; i++)
			if (AktuellerPunkt != i)
				pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + (i+2)*35, TextArray [TEXT_MENUE_TUTORIAL_SPIELEN+i], menucolor2, 2);

			pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + (AktuellerPunkt+2)*35, TextArray [TEXT_MENUE_TUTORIAL_SPIELEN+AktuellerPunkt], menucolor, 2);
			
		} break;

		case MENUZUSTAND_PLAYERCOUNT :
		{	
			pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET, TextArray [TEXT_MENUE_SPIEL_STARTEN], menucolor, 2);

			for (int i=0; i<2; i++)
			if (AktuellerPunkt != i)
				pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + (i+2)*35, TextArray [TEXT_ONE_PLAYER+i], menucolor2, 2);
			
			pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + (AktuellerPunkt+2)*35, TextArray [TEXT_ONE_PLAYER+AktuellerPunkt], menucolor, 2);
			
		} break;

		case MENUZUSTAND_SELECTSKILL :
		{

			pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET, TextArray [TEXT_MENUE_SPIEL_STARTEN], menucolor, 2);

			for (int i=0; i<4; i++)
			if (AktuellerPunkt != i)
				pMenuFont->DrawText(310, ypos + OFFSET + (i+2)*35, TextArray [TEXT_MENUE_LEICHT+i], menucolor2, 2);

			pMenuFont->DrawText(310, ypos + OFFSET + (AktuellerPunkt+2)*35, TextArray [TEXT_MENUE_LEICHT+AktuellerPunkt], menucolor, 2);

			// Skill anzeigen
			DirectGraphics.SetColorKeyMode();
			Skills.RenderSprite(190, ypos + 165, AktuellerPunkt, 0xFFFFFFFF);
			DirectGraphics.SetAdditiveMode();

		} break;

		case MENUZUSTAND_LOADGAME :
		{
			pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET, TextArray [TEXT_MENUE_ALTES_SPIEL_FORTSETZEN], menucolor, 2);

			ShowSavegames(AktuellerPunkt);			
		} break; // Load Game

		case MENUZUSTAND_SAVEGAME :
		{
			ShowSavegames(AktuellerPunkt);

			pMenuFont->DrawTextCenterAlign(320.0f, ypos + OFFSET, TextArray[TEXT_MENUE_SPIEL_SPEICHERN], 0xFFFFFFFF, 2);
			pMenuFont->DrawTextCenterAlign(320, ypos + 150 + (MAX_SAVEGAMES+1)*15, TextArray[TEXT_WEITER], 0x80FFFFFF, 2);

			// Aktuelle gewähltes Savegame heller anzeigen
			//
			if (AktuellerPunkt == MAX_SAVEGAMES)
				pMenuFont->DrawTextCenterAlign(320, ypos + 150 + (MAX_SAVEGAMES+1)*15, TextArray[TEXT_WEITER], 0xFFFFFFFF, 2);
			
		} break; // Save Game


		default : break;
	}

	DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// Alles machen, was da Menu betrifft, sprich, werte ändern und dann anzeigen
// --------------------------------------------------------------------------------------

void MenuClass::DoMenu(void)
{
	// Einheitsmatrix setzen, damit das Menu richtig angezeigt wird
	//
	D3DXMATRIX matView;
	D3DXMatrixIdentity	 (&matView);
	lpD3DDevice->SetTransform(D3DTS_VIEW, &matView);

	// Wird noch keine Menu Musik gespielt ?
	//
	if (pSoundManager->CurrentSongNr != MUSIC_MENU &&
		pSoundManager->CurrentSongNr != MUSIC_HIGHSCORE &&
		pSoundManager->CurrentSongNr != MUSIC_CREDITS)
		pSoundManager->PlaySong(MUSIC_MENU, false);

	// Logo einfaden
	if (RotationDir > 0)
		Rotation += RotationDir * 10 SYNC;

	if (Rotation >=  255.0f)
	{
		Rotation = 255.0f;
		RotationDir = 0;
	}

	// Sterne bewegen
	//
	for (int i=0; i<MAX_STARS; i++)
	{
		Stars[i].Count += Stars[i].Ebene / 20000.0f SYNC;

		if (Stars[i].Count > 2 * PI)
			Stars[i].Count -= 2 * PI;
	}

	ShowMenu();

	//LanguageFile Info Fenster Counter runterzählen und Fenster ggf anzeigen
	//
	if (ShowLanguageInfoCounter > 0.0f)
	{
		ShowLanguageInfoCounter -= 50.0f SYNC;
		ShowLanguageInfo();
	}

	if (pConsole->Showing)
		return;

//----- Tasten abfragen

	bool selected = false;
	bool anybutton = false;

	for (int i = 0; i < MAX_JOYSTICKBUTTONS; i++)
		DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickButtons[i] = false;

	DirectInput.UpdateJoysticks();

	for (int i = 0; i < MAX_JOYSTICKBUTTONS; i++)
		if (DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickButtons[i] == true)
			anybutton = true;

	if ((KeyDown(DIK_SPACE)  || 
		 KeyDown(DIK_RETURN) ||
		 anybutton == true) && AuswahlPossible == true)
		 selected = true;

	bool JoyOK, KeyOK;

	KeyOK = false;
	JoyOK = false;

	if (!KeyDown(DIK_NUMPAD4) &&
		!KeyDown(DIK_NUMPAD6) &&
		!KeyDown(DIK_NUMPAD2) &&
		!KeyDown(DIK_NUMPAD8) &&
		!KeyDown(DIK_LEFT)    &&
		!KeyDown(DIK_RIGHT)   &&
		!KeyDown(DIK_UP)      &&
		!KeyDown(DIK_DOWN)    &&
		!KeyDown(DIK_RETURN)  &&
		!KeyDown(DIK_SPACE)   &&
		!KeyDown(DIK_ESCAPE))
	KeyOK = true;

	if (JoystickFound == false)
		JoyOK = true;
	else
	{
		if (anybutton == false &&		
			DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickY > -pPlayer[0]->JoystickSchwelle &&
			DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickY <  pPlayer[0]->JoystickSchwelle &&
			DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV == -1)
			JoyOK = true;
	}

	if (KeyOK == true && 
		JoyOK == true)
		AuswahlPossible = true;

	if (KeyDown(DIK_ESCAPE) && ShowLanguageInfoCounter > 256.0f)
	{
		ShowLanguageInfoCounter = 256.0f;
		pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);
		AuswahlPossible = false;
	}	

	if (AuswahlPossible == true && locked == false &&

		(KeyDown(DIK_NUMPAD8) || KeyDown(DIK_UP)      || 

		(DirectInput.Joysticks[pPlayer[0]->JoystickIndex].Active &&

		 (DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickY < -pPlayer[0]->JoystickSchwelle || 

		 ((DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV >= 4500 * 7 || 
		   DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV <= 4500 * 1) && 
		   DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV > -1)))))
	{
		AuswahlPossible = false;
		AktuellerPunkt--;
		pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);

		// Spieler will auf "spiel fortsetzen" wechseln, aber es läuft noch kein spiel
		//
		if (Stage   == -1				     &&
			AktuellerZustand == MENUZUSTAND_MAINMENU &&
			AktuellerPunkt == 1)			 
		{
			AktuellerPunkt = 0;
		}

		// Checken, ob bei der Tastenwahl ein leeres Feld übersprungen werden muss
		//
		if (AktuellerZustand == MENUZUSTAND_TASTEN)
		{
			PlayerClass* pCurrentPlayer;

			if (CurrentPlayer == 0)
				pCurrentPlayer = pPlayer[0];
			else
				pCurrentPlayer = pPlayer[1];

			if (AktuellerPunkt == 2 &&
				pCurrentPlayer->ControlType == JOYMODE_KEYBOARD)
				AktuellerPunkt = 0;				
			else
			if (AktuellerPunkt >= 3)
			{
				while(PossibleKeys[CurrentPlayer][AktuellerPunkt-3] == false)
					AktuellerPunkt--;

			}
		}
	}

	if (AuswahlPossible == true && locked == false &&

		(KeyDown(DIK_NUMPAD2) || KeyDown(DIK_DOWN)      || 

		(DirectInput.Joysticks[pPlayer[0]->JoystickIndex].Active &&

		 (DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickY > pPlayer[0]->JoystickSchwelle || 

		 ((DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV >= 4500 * 3 || 
		   DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV <= 4500 * 5) && 
		   DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV > -1)))))
	{
		AuswahlPossible = false;
		AktuellerPunkt++;
		pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);

		// Spieler will auf "spiel fortsetzen" wechseln, aber es läuft noch kein spiel
		//
		if (Stage   == -1				     &&
			AktuellerZustand == MENUZUSTAND_MAINMENU &&
			AktuellerPunkt == 1)			 
		{
			AktuellerPunkt = 2;
		}

		// Checken, ob bei der Tastenwahl ein leeres Feld übersprungen werden muss
		//
		if (AktuellerZustand == MENUZUSTAND_TASTEN)
		{
			PlayerClass* pCurrentPlayer;

			if (CurrentPlayer == 0)
				pCurrentPlayer = pPlayer[0];
			else
				pCurrentPlayer = pPlayer[1];

			if (AktuellerPunkt == 1 &&
				pCurrentPlayer->ControlType == JOYMODE_KEYBOARD)
				AktuellerPunkt = 3;
			else

			if (AktuellerPunkt >= 3)
			{
				while(PossibleKeys[CurrentPlayer][AktuellerPunkt-3] == false)
					AktuellerPunkt++;
			}
		}
	}

//----- Je nach Menu-Zustand die Aktionen auswerten

	switch (AktuellerZustand)
	{
		// Haupt-Menu
		case MENUZUSTAND_MAINMENU :
		{		
			// zurück zum Game ?
			if (KeyDown(DIK_ESCAPE) && AuswahlPossible == true && Stage != -1)
			{
				AktuellerPunkt = MENUPUNKT_CONTINUEGAME;
				selected = true;

				while (DirectInput.AreAllKeysReleased() == false)
					DirectInput.UpdateTastatur();
			}

			if (AktuellerPunkt < MENUPUNKT_STARTGAME)
				AktuellerPunkt = MENUPUNKT_END;

			if (AktuellerPunkt > MENUPUNKT_END)
				AktuellerPunkt = MENUPUNKT_STARTGAME;

			// Menu-Punkt im Hauptmenu ausgewählt ?
			if (selected)
			{		
				AuswahlPossible = false;

				// Je nach aktuellem Menupunkt in das entsprechende Untermenu wechseln
				if (AktuellerPunkt == MENUPUNKT_STARTGAME)
				{
					AktuellerZustand = MENUZUSTAND_NEWGAME;
					AktuellerPunkt = 1;
				}
				else
				if (AktuellerPunkt == MENUPUNKT_CONTINUEGAME && Stage > -1)
				{
					AktuellerPunkt = 0;
					pSoundManager->StopSong(MUSIC_MENU, false);
					SpielZustand = GAMELOOP;

					if (FMUSIC_GetPaused(pSoundManager->its_Songs[MUSIC_BOSS]->SongData))
						pSoundManager->PlaySong(MUSIC_BOSS, true); 
					else
					if (pPlayer[0]->PunisherActive == true)
						pSoundManager->PlaySong(MUSIC_PUNISHER, true);
					else
					if (pPlayer[0]->Riding())
						pSoundManager->PlaySong(MUSIC_FLUGSACK, true); 
					else
						pSoundManager->PlaySong(MUSIC_STAGEMUSIC, true);
				
					pPlayer[0]->PowerLinePossible = false;
					pPlayer[0]->ShotDelay = 2.0f;
				}
				else
				if (AktuellerPunkt == MENUPUNKT_CREDITS)
				{
					pSoundManager->SetSongVolume(MUSIC_CREDITS, 100);
					pSoundManager->StopSong(MUSIC_MENU, false);
					pSoundManager->its_Songs[MUSIC_CREDITS]->FadingVolume = 0.0f;
					pSoundManager->PlaySong(MUSIC_CREDITS, false);
					CreditsCounter = 0.0f;
				}
				else
				if (AktuellerPunkt == MENUPUNKT_HIGHSCORES)
				{
					pSoundManager->SetSongVolume(MUSIC_HIGHSCORE, 0);
					pSoundManager->FadeSong(MUSIC_MENU, -5.0f, 0, false);
					pSoundManager->FadeSong(MUSIC_HIGHSCORE, 5.0f, 100, false);
				}

				if (AktuellerPunkt != MENUPUNKT_CONTINUEGAME &&
					AktuellerPunkt != MENUPUNKT_STARTGAME)
					AktuellerZustand = AktuellerPunkt;

				if (AktuellerPunkt == MENUPUNKT_END)
					GameRunning = false;

				if (AktuellerPunkt != MENUPUNKT_CONTINUEGAME &&
					AktuellerPunkt != MENUPUNKT_HIGHSCORES   &&
					AktuellerPunkt != MENUPUNKT_CREDITS	     &&
					AktuellerPunkt != MENUPUNKT_END)
					AktuellerPunkt = 0;

				if (AktuellerPunkt == MENUPUNKT_HIGHSCORES)
					AktuellerPunkt = MENUZUSTAND_ENTERNAME;
				
				pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);

				// Highscore wieder von Anfang an blinken lassen
				BlinkOffset = 0;

				// Credits von Anfang an scrollen lassen
				CreditsOffset   = 0;
				CreditsPosition = 0;
			}
		} break;	// MAINMENU

		// Sound / Musik Lautstärke wählen
		case MENUZUSTAND_VOLUMES :
		{
			if (AktuellerPunkt < 0)
				AktuellerPunkt = 4;

			if (AktuellerPunkt > 4)
				AktuellerPunkt = 0;

			// Volumes geändert ?
			// Leiser
			bool pressed = false;

			if (AktuellerPunkt <= 1)

			if (KeyDown(DIK_NUMPAD4) ||
				KeyDown(DIK_LEFT)    || 
			   (DirectInput.Joysticks[pPlayer[0]->JoystickIndex].Active &&
			   (DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickX < -pPlayer[0]->JoystickSchwelle ||
			   (DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV >= 4500 * 5 && DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV <= 4500 * 7))))
				pressed = true;
			else
				pressed = false;

			if(pressed)
			{
				if (AktuellerPunkt == 0)
				{
					pSoundManager->its_GlobalSoundVolume -= float(5.0f SYNC);
					if (pSoundManager->its_GlobalSoundVolume < 0.0f)
						pSoundManager->its_GlobalSoundVolume = 0.0f;
				}

				if (AktuellerPunkt == 1)
				{
					pSoundManager->its_GlobalMusicVolume -= 5.0f SYNC;
					if (pSoundManager->its_GlobalMusicVolume < 0.0f)
						pSoundManager->its_GlobalMusicVolume = 0.0f;
				}	
			}

			// Lauter
			if (KeyDown(DIK_NUMPAD6) || 
				KeyDown(DIK_RIGHT)   ||
			   (DirectInput.Joysticks[pPlayer[0]->JoystickIndex].Active &&
			   (DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickX > pPlayer[0]->JoystickSchwelle ||
			   (DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV >= 4500 * 1 && DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV <= 4500 * 3))))
				pressed = true;
			else
				pressed = false;

			if (pressed)
			{
				if (AktuellerPunkt == 0)
				{
					pSoundManager->its_GlobalSoundVolume += 5.0f SYNC;
					if (pSoundManager->its_GlobalSoundVolume > 100.0f)
						pSoundManager->its_GlobalSoundVolume = 100.0f;
				}
				if (AktuellerPunkt == 1)
				{
					pSoundManager->its_GlobalMusicVolume += 5.0f SYNC;
					if (pSoundManager->its_GlobalMusicVolume > 100.0f)
						pSoundManager->its_GlobalMusicVolume = 100.0f;
				}	
			}

			// Punkt gewählt
			//
			if (selected)
			{		
				AuswahlPossible = false;

				// Keyboard konfigurieren
				//
				if (AktuellerPunkt == 2)
				{
					CurrentPlayer	 = 0;
					AktuellerZustand = MENUZUSTAND_TASTEN;
					AktuellerPunkt   = 0;
					locked			 = false;
				}	

/*
				// Joystick Optionen
				//
				if (AktuellerPunkt == 3 && JoystickFound == true)
				{
					AktuellerZustand = MENUZUSTAND_BUTTONS;
					AktuellerPunkt   = 0;
					locked			 = false;
				}	
*/

				// Sprache wählen
				//
				if (AktuellerPunkt == 3)
				{
					AktuellerZustand = MENUZUSTAND_LANGUAGE;
					AktuellerPunkt   = 0;
				}	

				// Sprache wählen
				//
				if (AktuellerPunkt == 4)
				{
					options_Detail++;

					if (options_Detail > DETAIL_MAXIMUM)
						options_Detail = DETAIL_LOW;

					pPartikelSystem->SetParticleCount();
				}	
			}

			// zurück zum Hauptmenu ?
			if (KeyDown(DIK_ESCAPE) && AuswahlPossible == true)
			{		
				SaveConfig();							// Sound Config speichern
				AuswahlPossible = false;
				AktuellerZustand = MENUZUSTAND_MAINMENU;
				AktuellerPunkt	 = MENUPUNKT_VOLUMES;
				pSoundManager->PlayWave(100, 100, 11025, SOUND_CLICK);
			}
		} break;	// Sound / Musik Lautstärke

		case MENUZUSTAND_LANGUAGE:
		{
			if (AktuellerPunkt < 0)
				AktuellerPunkt = LanguageFileCount;

			if (AktuellerPunkt > LanguageFileCount)
				AktuellerPunkt = 0;


			// Punkt gewählt
			//
			if (selected)
			{		
				AuswahlPossible = false;

				// Language File gewählt ?
				if (AktuellerPunkt < LanguageFileCount)
				{
					strcpy_s (ActualLanguage, strlen(LanguageFiles [AktuellerPunkt]) + 1, LanguageFiles [AktuellerPunkt]);
					LoadLanguage (ActualLanguage);
					SaveConfig();
					ShowLanguageInfoCounter = 2000.0f;

					// Sprache feststellen und schauen, welche Grafik wir anzeigen müssen
					if (strcmp (TextArray[TEXT_BENUTZTE_GRAFIK], "0") == 0)
						Sprachgrafik = 0;		// deutsch ?
					else
						Sprachgrafik = 1;		// oder doch englisch ?

					AktuellerZustand = MENUZUSTAND_MAINMENU;
					AktuellerPunkt	 = MENUZUSTAND_VOLUMES;
					pSoundManager->PlayWave(100, 100, 11025, SOUND_CLICK);
				}

				// oder nur zurück ?
				else
				{
					AktuellerZustand = MENUZUSTAND_VOLUMES;
					AktuellerPunkt	 = 4;
					pSoundManager->PlayWave(100, 100, 11025, SOUND_CLICK);
				}
			}

			// zurück zum Hauptmenu ?
			if (KeyDown(DIK_ESCAPE) && AuswahlPossible == true)
			{		
				SaveConfig();							// Sound Config speichern
				AuswahlPossible = false;
				AktuellerZustand = MENUZUSTAND_MAINMENU;
				AktuellerPunkt	 = MENUPUNKT_VOLUMES;
				pSoundManager->PlayWave(100, 100, 11025, SOUND_CLICK);
			}

		}
		break; // Languages

		case MENUZUSTAND_TASTEN:
		{
			PlayerClass* pCurrentPlayer;

			if (CurrentPlayer == 0)
				pCurrentPlayer = pPlayer[0];
			else
				pCurrentPlayer = pPlayer[1];

			FillPossibleKeys();

			if (KeyDown(DIK_DELETE) ||
				KeyDown(DIK_BACK))
			{
				pCurrentPlayer->AktionJoystick[AktuellerPunkt - 3] = -1;
				pCurrentPlayer->AktionKeyboard[AktuellerPunkt - 3] = -1;
			}

			// neue Taste eingeben ?
			if (locked == true)
			{				
				if (AuswahlPossible == true)
				{
					// Neue Taste?
					DirectInput.UpdateTastatur();

					for (int i=1; i<255; i++)		// Puffer durchgehen
					if (KeyDown(i))					// ob eine Taste gedrückt wurde
					{
						pCurrentPlayer->AktionKeyboard[AktuellerPunkt - 3] = i;
						pCurrentPlayer->AktionJoystick[AktuellerPunkt - 3] = -1;
						locked		    = false;
						AuswahlPossible = false;

						while (KeyDown(i))
							DirectInput.UpdateTastatur();
							
						break;
					}

					// Neuer Button?
					if (DirectInput.Joysticks[pCurrentPlayer->JoystickIndex].Active == true)
					{
						for (int i = 0; i < MAX_JOYSTICKBUTTONS - 1; i++)
						if (DirectInput.Joysticks[pCurrentPlayer->JoystickIndex].JoystickButtons[i] == true)							
						{
							pCurrentPlayer->AktionJoystick[AktuellerPunkt - 3] = i;
							pCurrentPlayer->AktionKeyboard[AktuellerPunkt - 3] = -1;
							locked		    = false;
							AuswahlPossible = false;							

							while (DirectInput.Joysticks[pCurrentPlayer->JoystickIndex].JoystickButtons[i] == true)
								DirectInput.UpdateJoysticks();
						}
					}
				}
				else
				{				
					if (!KeyDown(DIK_DELETE) &&
						!KeyDown(DIK_NUMPAD2) &&
						!KeyDown(DIK_NUMPAD8) &&
						!KeyDown(DIK_UP)      &&
						!KeyDown(DIK_DOWN)    &&
						!KeyDown(DIK_RETURN)  &&
						!KeyDown(DIK_SPACE)   &&
						!KeyDown(DIK_ESCAPE))
						AuswahlPossible = true;
				}
			}
			else
			{
				if (AktuellerPunkt < 0)
					AktuellerPunkt = 14;

				if (AktuellerPunkt > 14)
					AktuellerPunkt = 0;

				// Spieler wechseln
				if (AuswahlPossible == true)
				{
					PlayerClass* pCurrentPlayer;
				
					if (CurrentPlayer == 0)
						pCurrentPlayer = pPlayer[0];
					else
						pCurrentPlayer = pPlayer[1];
	
					if (KeyDown(DIK_NUMPAD6) || 
						KeyDown(DIK_RIGHT)   ||
						 DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickX > pPlayer[0]->JoystickSchwelle ||
						(DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV >= 4500 * 1 && DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV <= 4500 * 3))
					{
						// Empfindlichkeit ändern?
						if (AktuellerPunkt == 2)
						{
							pCurrentPlayer->JoystickSchwelle -= 100.0f SYNC;

							if (pCurrentPlayer->JoystickSchwelle < 100.0f)
								pCurrentPlayer->JoystickSchwelle = 100.0f;	
						}
						else
						{
							AuswahlPossible = false;
							CurrentPlayer = 1;

							if (CurrentPlayer == 0)
								pCurrentPlayer = pPlayer[0];
							else
								pCurrentPlayer = pPlayer[1];

							while (PossibleKeys[CurrentPlayer][AktuellerPunkt-3] == false)
								AktuellerPunkt++;

							if (pCurrentPlayer->ControlType == JOYMODE_KEYBOARD &&
								AktuellerPunkt < 3)
								AktuellerPunkt = 0;
						}
					}

					if (KeyDown(DIK_NUMPAD4) ||
						KeyDown(DIK_LEFT)    || 
						 DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickX < -pPlayer[0]->JoystickSchwelle ||
						(DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV >= 4500 * 5 && DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV <= 4500 * 7))
					{
						// Empfindlichkeit ändern?
						if (AktuellerPunkt == 2)
						{
							pCurrentPlayer->JoystickSchwelle += 100.0f SYNC;

							if (pCurrentPlayer->JoystickSchwelle > 900.0f)
								pCurrentPlayer->JoystickSchwelle = 900.0f;							
						}
						else
						{
							AuswahlPossible = false;
							CurrentPlayer = 0;

							while (PossibleKeys[CurrentPlayer][AktuellerPunkt-3] == false)
								AktuellerPunkt++;

							if (CurrentPlayer == 0)
								pCurrentPlayer = pPlayer[0];
							else
								pCurrentPlayer = pPlayer[1];

							if (pCurrentPlayer->ControlType == JOYMODE_KEYBOARD &&
								AktuellerPunkt < 3)
								AktuellerPunkt = 0;
						}
					}
				}

				// Punkt gewählt
				//
				if (selected)
				{	
					AuswahlPossible = false;

					// Taste ändern?
					if (AktuellerPunkt >= 3)
						locked = true;		
					else
					{
						PlayerClass *pCurrentPlayer;

						if (CurrentPlayer == 0)
							pCurrentPlayer = pPlayer[0];
						else
							pCurrentPlayer = pPlayer[1];

						// Joymode ändern?
						if (AktuellerPunkt == 1)
						{
							if (pCurrentPlayer->JoystickMode == JOYMODE_STICK)
								pCurrentPlayer->JoystickMode = JOYMODE_PAD;
							else
								pCurrentPlayer->JoystickMode = JOYMODE_STICK;
						}

						// Controller Type ändern?
						if (AktuellerPunkt == 0)
						{
							// Joytick dran?
							if (JoystickFound == true)
							{								
								// Von Keyboard auf Joystick wechseln?
								if (pCurrentPlayer->ControlType == JOYMODE_KEYBOARD)
								{
									pCurrentPlayer->ControlType = JOYMODE_STICK;
									pCurrentPlayer->JoystickIndex = 0;
								}

								// Von Joystick auf Keyboard wechseln?
								else
								{	
									pCurrentPlayer->JoystickIndex++;

									if (pCurrentPlayer->JoystickIndex >= DirectInput.JoysticksFound)
									{										
										pCurrentPlayer->ControlType = JOYMODE_KEYBOARD;
										pCurrentPlayer->JoystickIndex = 0;
									}																	
								}
							}
						}
					}								
				}

				// zurück zum Hauptmenu ?
				if (KeyDown(DIK_ESCAPE) && AuswahlPossible == true)
				{		
					// Neue Tastennamen für die Tutorial-Texte übernehmen
					InitReplacers();

					// Sound Config speichern
					SaveConfig();

					AuswahlPossible = false;
					AktuellerZustand = MENUZUSTAND_VOLUMES;
					AktuellerPunkt	 = MENUPUNKT_VOLUMES;
					pSoundManager->PlayWave(100, 100, 11025, SOUND_CLICK);
				}
			}
		}
		break; // Languages

		case MENUZUSTAND_BUTTONS:
		{
			// Button gedrückt ?
			//
			if (AktuellerPunkt > 2)
			{
				for (int i = 0; i < MAX_JOYSTICKBUTTONS; i++)
					if (DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickButtons[i] == true)
						pPlayer[0]->AktionJoystick[AktuellerPunkt+2] = i;

				float d = (float)(pMenuFont->StringLength(TextArray[TEXT_TASTEN_NEU_B], 2));
				pMenuFont->DrawText (320 - d / 2.0f, ypos + 310.0f, TextArray[TEXT_TASTEN_NEU_B], 0xFFFFFFFF, 2);
			}

			// Empfindlichkeit geändert
			//
			if (AktuellerPunkt == 2)
			{
				bool pressed = false;

				if (KeyDown(DIK_NUMPAD4) ||
					KeyDown(DIK_LEFT)    || 
					DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickX < -pPlayer[0]->JoystickSchwelle ||
					(DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV >= 4500 * 5 && DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV <= 4500 * 7))
					pressed = true;
				else
					pressed = false;

				if(pressed)
				{
					pPlayer[0]->JoystickSchwelle += 50.0f SYNC;
					if (pPlayer[0]->JoystickSchwelle > 1000.0f-1)
						pPlayer[0]->JoystickSchwelle = 1000.0f-1;
				}

				// Lauter
				if (KeyDown(DIK_NUMPAD6) || 
					KeyDown(DIK_RIGHT)   ||
					DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickX > pPlayer[0]->JoystickSchwelle ||
					(DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV >= 4500 * 1 && DirectInput.Joysticks[pPlayer[0]->JoystickIndex].JoystickPOV <= 4500 * 3))
					pressed = true;
				else
					pressed = false;

				if(pressed)
				{
					pPlayer[0]->JoystickSchwelle -= 50.0f SYNC;
					if (pPlayer[0]->JoystickSchwelle < 1.0f)
						pPlayer[0]->JoystickSchwelle = 1.0f;						
				}			
			}

			if (AktuellerPunkt < 0)
				AktuellerPunkt = 8;

			if (AktuellerPunkt > 8)
				AktuellerPunkt = 0;


			// Punkt gewählt
			//
			if (selected)
			{
				AuswahlPossible = false;

				// Joypad Mode
				//
				if (AktuellerPunkt == 0)
				{
					if (pPlayer[0]->ControlType == JOYMODE_STICK)
						pPlayer[0]->ControlType = JOYMODE_PAD;
					else
						pPlayer[0]->ControlType = JOYMODE_STICK;
				}

				// ForceFeedback
				//
				if (AktuellerPunkt == 1 &&
					DirectInput.Joysticks[pPlayer[0]->JoystickIndex].CanForceFeedback == true)
				{
					UseForceFeedback = !UseForceFeedback;
				}
			}

			// zurück zum Hauptmenu ?
			if (KeyDown(DIK_ESCAPE) && AuswahlPossible == true)
			{		
				SaveConfig();							// Sound Config speichern
				AuswahlPossible = false;
				AktuellerZustand = MENUZUSTAND_MAINMENU;
				AktuellerPunkt	 = MENUPUNKT_VOLUMES;
				pSoundManager->PlayWave(100, 100, 11025, SOUND_CLICK);
			}
		}
		break;

		// Die Credits und die Highscore
		case MENUPUNKT_CREDITS  :
		case MENUPUNKT_HIGHSCORES :
		{
			// zurück zum Hauptmenu ?
			if (KeyDown(DIK_ESCAPE) && AuswahlPossible == true)
			{		
				if (AktuellerZustand == MENUPUNKT_CREDITS)
				{
					pSoundManager->SetSongVolume(MUSIC_MENU, 0);
					pSoundManager->FadeSong(MUSIC_CREDITS, -5.0f, 0, false);
					pSoundManager->FadeSong(MUSIC_MENU, 5.0f, 100, false);
					CreditsCounter = 0.0f;
					AktuellerPunkt = 4;
				}

				if (AktuellerZustand == MENUPUNKT_HIGHSCORES)
				{
					pSoundManager->SetSongVolume(MUSIC_MENU, 0);
					pSoundManager->FadeSong(MUSIC_HIGHSCORE, -5.0f, 0, false);
					pSoundManager->FadeSong(MUSIC_MENU, 5.0f, 100, false);
					AktuellerPunkt = 3;
				}

				AuswahlPossible = false;
				AktuellerZustand = MENUZUSTAND_MAINMENU;
				pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);				
			}
		} break; // Credits und Highscore

		case MENUZUSTAND_ENTERNAME :
		{
			static char Taste;
			static bool possible = false;

			if (!KeyDown(DIK_RETURN))
				AuswahlPossible = true;

			if (KeyDown(DIK_RETURN) && AuswahlPossible == true)
			{
				// Neuen Namen an die neue Position setzen
				strcpy_s(Highscores[NewRank].Name, strlen(NewName) + 1, NewName);
				Highscores[NewRank].Stage = NewStage;
				Highscores[NewRank].Skill = NewSkill;

				// Und zurück ins Hauptmenu und dort die Highscores anzeigen
				SpielZustand	 = MAINMENU;
				AktuellerZustand = MENUPUNKT_HIGHSCORES;

				pSoundManager->SetSongVolume(MUSIC_MENU, 0.0f);

				// Highscore noch sichern
				SaveHighscore();
			}

			DirectInput.UpdateTastatur();

			if (possible == false)
			{
				possible = true;

				// Testen ob die gedrückte Taste loasgelassen wurde
				if (KeyDown(Taste) || KeyDown(DIK_BACK) || KeyDown(DIK_SPACE))
					possible = false;
			}

			for (int i=0; i<256; i++)		// Puffer durchgehen
			if (KeyDown(i) && possible == true)
			{
				possible = false;

				if (KeyDown(DIK_BACK))
				{
					if (strlen(NewName) > 0)
					{
						// TODO FIX
						// String umdrehen
						//
						/*
						strrev(NewName);
						
												char buf[30];
												int  laenge;
						
												laenge = strlen(NewName);
												strcpy_s(buf, "");
						
												for (int l=0; l<laenge; l++)
													buf[l] = NewName[l+1];
						
												strrev(buf);
												strcpy_s(NewName, buf);*/
						
					}
					//strcpy_s(NewName, "");
				}
				else
				if (KeyDown(DIK_SPACE))
				{
					// TODO FIX
					/*
					if (strlen(NewName) < 16)
										strcat_s(NewName, " ");*/
					
				}
				else
				{
					Taste = i;
					if (strlen(GetKeyName(Taste)) <= 1 &&
						strlen(NewName) < 16)
						strcat_s(NewName, strlen(GetKeyName(Taste)) + 1, GetKeyName(Taste));
				}
			}

			// Aktuellen Namen azeigen
			char Buffer[20];

			sprintf_s(Buffer, "%s_", NewName);
			pMenuFont->DrawTextCenterAlign(319, ypos + 230, Buffer, D3DCOLOR_RGBA(0, 0, 255, 255), 2);
			pMenuFont->DrawTextCenterAlign(321, ypos + 230, Buffer, D3DCOLOR_RGBA(0, 0, 255, 255), 2);
			pMenuFont->DrawTextCenterAlign(320, ypos + 229, Buffer, D3DCOLOR_RGBA(0, 0, 255, 255), 2);
			pMenuFont->DrawTextCenterAlign(320, ypos + 231, Buffer, D3DCOLOR_RGBA(0, 0, 255, 255), 2);
			pMenuFont->DrawTextCenterAlign(320, ypos + 230, Buffer, D3DCOLOR_RGBA(255, 255, 255, 255), 2);

		} break; // Namen eingeben

		// Neues Spiel starten / Savegame laden
		case MENUZUSTAND_NEWGAME :
		{

			if (AktuellerPunkt < 0)
				AktuellerPunkt = 2;

			if (AktuellerPunkt > 2)
				AktuellerPunkt = 0;

			// Zurück ins Haupt Menu ?
			if (KeyDown(DIK_ESCAPE) && AuswahlPossible == true)
			{
				AktuellerZustand = MENUZUSTAND_MAINMENU;
				AktuellerPunkt	 = 0;
			}

			// Menu-Punkt ausgewählt ?
			if (selected)
			{		
				AuswahlPossible = false;

				// Tutorial spielen?
				if (AktuellerPunkt == 0)		
				{					
					pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);

					// Game starten mit Tutorial					
					NUMPLAYERS = 1;
					RunningTutorial = true;
					Skill = AktuellerPunkt;
//					pSoundManager->StopSong(MUSIC_MENU, false);
					InitNewGame();
					InitNewGameLevel(1);				
					SpielZustand = GAMELOOP;				
				} 

				// Neues Spiel starten ?
				if (AktuellerPunkt == 1)
				{
					AktuellerZustand = MENUZUSTAND_PLAYERCOUNT;
					AktuellerPunkt	 = 0;						// Auf Medium anfangen
				} 

				// altes Spiel laden ?
				else
				if (AktuellerPunkt == 2)
				{
					AktuellerZustand = MENUZUSTAND_LOADGAME;
					AktuellerPunkt	 = 0;
					LoadSavegames();
				}

				pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);
			}
		} break;	// SPIEL VERLASSEN

		case MENUZUSTAND_PLAYERCOUNT :
		{
			if (AktuellerPunkt < 0)
				AktuellerPunkt = 1;

			if (AktuellerPunkt > 1)
				AktuellerPunkt = 0;

			// Zurück ins Haupt Menu ?
			if (KeyDown(DIK_ESCAPE) && AuswahlPossible == true)
			{
				AuswahlPossible = false;
				AktuellerZustand = MENUZUSTAND_NEWGAME;
				AktuellerPunkt	 = 1;
			}

			// Menu-Punkt ausgewählt ?
			if (selected)
			{	
				tempNUMPLAYERS = AktuellerPunkt + 1;				
				AktuellerPunkt = 1;
				
				AktuellerZustand = MENUZUSTAND_SELECTSKILL;
				pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);

				AuswahlPossible = false;
			}
		} break;	// SPIEL VERLASSEN

		// Skill auswählen
		case MENUZUSTAND_SELECTSKILL :
		{
			if (AktuellerPunkt < 0)
				AktuellerPunkt = 3;

			if (AktuellerPunkt > 3)
				AktuellerPunkt = 0;

			// Zurück ins New Game Menu ?
			if (KeyDown(DIK_ESCAPE) && AuswahlPossible == true)
			{
				AuswahlPossible = false;
				AktuellerZustand = MENUZUSTAND_PLAYERCOUNT;
				AktuellerPunkt	 = tempNUMPLAYERS - 1;
			}

			// Menu-Punkt ausgewählt ?
			if (selected)
			{		
				NUMPLAYERS = tempNUMPLAYERS;
				AuswahlPossible = false;
				RunningTutorial = false;
				pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);

				// Game starten
				// mit gewähltem Skill
				Skill = AktuellerPunkt;

				// Intro initialisieren
				pSoundManager->StopAllSongs(false);
				DisplayHintNr = -1;

				StartIntro();
			}
		} break;	// Select Skill

		// Savegame zum Laden wählen
		case MENUZUSTAND_LOADGAME :
		{
			if (AktuellerPunkt < 0)
				AktuellerPunkt = MAX_SAVEGAMES-1;

			if (AktuellerPunkt > MAX_SAVEGAMES-1)
				AktuellerPunkt = 0;

			// Zurück ins New Game Menu ?
			if (KeyDown(DIK_ESCAPE) && AuswahlPossible == true)
			{
				AktuellerZustand = MENUZUSTAND_NEWGAME;
				AktuellerPunkt	 = 0;
				AuswahlPossible  = false;
			}

			// Menu-Punkt ausgewählt ?
			// Dann prüfen ob das aktuelle Savegame ein leerer Slot ist
			// und wenn das nicht der Fall ist, dann das Savegame laden und
			// das Spiel starten
			if (selected)
			{		
				RunningTutorial = false;

				AuswahlPossible = false;
				pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);

				// Aktuell gewähltes Savegame laden, wenn es existiert
				// bzw die Werte aus den schon geladenen Savegames übernehmen
				if (Savegames[AktuellerPunkt].Stage >= 0)
				{
					InitNewGame();

					NUMPLAYERS = Savegames[AktuellerPunkt].Players;

					// Werte von Spieler auf das Savegame übertragen
					pPlayer[0]->Score				= Savegames[AktuellerPunkt].Score;
					Stage = Savegames[AktuellerPunkt].Stage;
					Skill				= Savegames[AktuellerPunkt].Skill;
					NewStage = Savegames[AktuellerPunkt].NewStage;
					pPlayer[0]->CollectedDiamonds	= Savegames[AktuellerPunkt].CollectedDiamonds;					
					pPlayer[0]->PowerLines	= Savegames[AktuellerPunkt].PowerLines;
					pPlayer[0]->Grenades	= Savegames[AktuellerPunkt].Grenades;
					pPlayer[0]->SmartBombs	= Savegames[AktuellerPunkt].SmartBombs;

					for (int p = 0; p < 2; p++)
					{
						pPlayer[p]->Handlung			= STEHEN;
						pPlayer[p]->Energy				= Savegames[AktuellerPunkt].Energy[p];
						pPlayer[p]->Armour				= Savegames[AktuellerPunkt].Armour[p];
						pPlayer[p]->Shield				= Savegames[AktuellerPunkt].Shield[p];	
						pPlayer[p]->SelectedWeapon		= Savegames[AktuellerPunkt].SelectedWeapon[p];
						pPlayer[p]->BlitzLength			= Savegames[AktuellerPunkt].BlitzLength[p];
						pPlayer[p]->AutoFireExtra		= Savegames[AktuellerPunkt].AutoFire[p];
						pPlayer[p]->RiesenShotExtra		= Savegames[AktuellerPunkt].RiesenShot[p];
						pPlayer[p]->Lives				= Savegames[AktuellerPunkt].Lives[p];

						for (int i=0; i<4; i++)
							pPlayer[p]->CurrentWeaponLevel[i]	= Savegames[AktuellerPunkt].CurrentWeaponLevel[p][i];
					}

					// An der Stelle im Savegame weiterspielen
					AktuellerZustand = MAINMENU;
					AktuellerPunkt   = 0;
//					pSoundManager->StopSong(MUSIC_MENU, false);
					InitNewGameLevel(NewStage);// Neues level laden
					SpielZustand	 = GAMELOOP;	// Weiterspielen
				}
			}
		} break;	// Load Game

		// Savegame zum Speichern wählen
		case MENUZUSTAND_SAVEGAME :
		{
			if (AktuellerPunkt < 0)
				AktuellerPunkt = MAX_SAVEGAMES;

			if (AktuellerPunkt > MAX_SAVEGAMES)
				AktuellerPunkt = 0;

			// Menu-Punkt ausgewählt ?
			// Dann das Spiel an dem aktuell gewählten Slot speichern
			// oder weiterspielen ohne zu speichern (je nach Auswahl)
			if (selected)
			{		
				AuswahlPossible = false;
				pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);

				// Bestimmtes Savegame ausgewählt
				// Dann hier speichern
				if (AktuellerPunkt < MAX_SAVEGAMES)
				{
					// Werte von Spieler auf das Savegame übertragen
					// Name des Save games aus aktuellem Datum und aktueller Zeit erstelen

					char   timestr[20];

					time_t seconds= time(0);

					// TODO FIX
//					struct tm *ptm= localtime(&seconds);
					struct tm *ptm = NULL;

					sprintf_s(timestr,"%02i-%02i-%04i %02i:%02i",
						   (int)ptm->tm_mday,
						   (int)ptm->tm_mon+1,
						   (int)ptm->tm_year+1900,						   
						   
						   (int)ptm->tm_hour,
						   (int)ptm->tm_min);

					strcpy_s(Savegames[AktuellerPunkt].Name, 1, "");
					strcpy_s(Savegames[AktuellerPunkt].Name, strlen(timestr) + 1, timestr);

					Savegames[AktuellerPunkt].Players			= NUMPLAYERS;
					Savegames[AktuellerPunkt].Score				= pPlayer[0]->Score;
					Savegames[AktuellerPunkt].Stage				= Stage;
					Savegames[AktuellerPunkt].Skill				= Skill;
					Savegames[AktuellerPunkt].NewStage			= NewStage;
					Savegames[AktuellerPunkt].CollectedDiamonds = pPlayer[0]->CollectedDiamonds;					
					Savegames[AktuellerPunkt].PowerLines  = pPlayer[0]->PowerLines;
					Savegames[AktuellerPunkt].Grenades    = pPlayer[0]->Grenades;
					Savegames[AktuellerPunkt].SmartBombs  = pPlayer[0]->SmartBombs;
					
					for (int p = 0; p < 2; p++)
					{
						Savegames[AktuellerPunkt].Energy[p]			= pPlayer[0]->Energy;
						Savegames[AktuellerPunkt].Armour[p]			= pPlayer[0]->Armour;
						Savegames[AktuellerPunkt].Shield[p]			= pPlayer[0]->Shield;					
						Savegames[AktuellerPunkt].SelectedWeapon[p]	= pPlayer[0]->SelectedWeapon;					
						Savegames[AktuellerPunkt].Lives[p]			= pPlayer[p]->Lives;
						Savegames[AktuellerPunkt].AutoFire[p]		= pPlayer[p]->AutoFireExtra;
						Savegames[AktuellerPunkt].RiesenShot[p]		= pPlayer[p]->RiesenShotExtra;

						for (int i=0; i<4; i++)
							Savegames[AktuellerPunkt].CurrentWeaponLevel[p][i] = pPlayer[p]->CurrentWeaponLevel[i];

						Savegames[AktuellerPunkt].BlitzLength[p] = pPlayer[p]->BlitzLength;
					}

					// Prüfsumme gegen Savegame-Manipulation errechnen

					Savegames[AktuellerPunkt].Pruefsumme = 
						(int)pPlayer[0]->Energy + (int)pPlayer[0]->Armour + (int)pPlayer[0]->Shield +
						pPlayer[0]->Score  + Stage  + Skill  +
						NewStage + pPlayer[0]->CollectedDiamonds +
						pPlayer[0]->SelectedWeapon;

					for (int i=0; i<4; i++)
						Savegames[AktuellerPunkt].Pruefsumme +=
							pPlayer[0]->CurrentWeaponLevel[i];

					Savegames[AktuellerPunkt].Pruefsumme +=
						pPlayer[0]->BlitzLength + 
						pPlayer[0]->PowerLines +
						pPlayer[0]->Grenades + pPlayer[0]->SmartBombs +
						NUMPLAYERS;

					// Und Savegame in Datei schreiben
					char Name  [100];		// Für die Dateinamen
					char Buffer[5];			// Für _itoa
					FILE *Datei = NULL;		// Savegame Datei

					// Name des Savegames erstellen
					_itoa_s(AktuellerPunkt, Buffer, 10);
					strcpy_s(Name, strlen("Savegame") + 1, "Savegame");
					strcat_s(Name, strlen(Buffer) + 1, Buffer);
					strcat_s(Name, 4, ".sav");

					// Versuchen, die Datei zu erstellen
					// nur weitermachen falls es keinen Fehler gibt
					fopen_s(&Datei, Name, "wb");

					// Fehler beim Öffnen ? Dann leeren Slot erzeugen
					if (Datei == NULL)
					{
					}

					// Ansonsten Slot speichern
					else
						fwrite(&Savegames[AktuellerPunkt], sizeof(Savegames[AktuellerPunkt]), 1, Datei);

					// Und Datei wieder schliessen
					fclose(Datei);
				} // Select Slot Auswahl

				// oder Weiter/Continue  ?
//				if (AktuellerPunkt == MAX_SAVEGAMES)
				{
					AktuellerZustand = MAINMENU;
					AktuellerPunkt   = 0;
					InitNewGameLevel(NewStage);// Neues level laden
				}
				
			}
		} break;	// Save Game

		default : break;
	} // switch
}

// --------------------------------------------------------------------------------------
// Savegames laden
// --------------------------------------------------------------------------------------

void MenuClass::LoadSavegames(void)
{
	char Name  [100];		// Für die Dateinamen
	char Buffer[5];			// Für _itoa
	FILE *Datei;			// Savegame Datei

	// Versuchen, die einzelnen Savegames zu laden
	for (int i=0; i<MAX_SAVEGAMES; i++)
	{
		// Name des Savegames erstellen
		_itoa_s(i, Buffer, 10);
		strcpy_s(Name, "Savegame");
		strcat_s(Name, Buffer);
		strcat_s(Name, ".sav");

		// Versuchen, die Datei zu öffnen
		// falls sie nicht existiert oder es eine Fehler gibt, ist der Slot noch leer
		fopen_s(&Datei, Name, "rb");

		Protokoll.WriteText("save games loaded\n", false);

		// Fehler beim Öffnen ? Dann leeren Slot erzeugen
		//
		if (Datei == NULL)
			Savegames[i].Stage = -1;

		else
		{
			// Ansonsten Slot auslesen
			//
			fread(&Savegames[i], sizeof(Savegames[i]), 1, Datei);
			
			// Und Datei wieder schliessen
			//
			fclose(Datei);
			// Checken ob die Pruefsumme noch stimmt
			// Prüfsumme gegen Savegame-Manipulation errechnen
			// und nur wenn diese stimmt das Savegame übernehmen
			long Pruefsumme;

			Pruefsumme = 
				(int)Savegames[i].Energy[0] + (int)Savegames[i].Armour[0] + (int)Savegames[i].Shield[0] +
				Savegames[i].Score  + Savegames[i].Stage  + Savegames[i].Skill  +
				Savegames[i].NewStage + Savegames[i].CollectedDiamonds +
				Savegames[i].SelectedWeapon[0];

			for (int j=0; j<4; j++)
				Pruefsumme += 
						Savegames[i].CurrentWeaponLevel[0][j];						

			Pruefsumme +=
				Savegames[i].BlitzLength[0] +			
				Savegames[i].PowerLines +
				Savegames[i].Grenades +
				Savegames[i].SmartBombs +
				Savegames[i].Players;

			// Pruefsumme stimmt nicht mehr ? Dann Savegame zum leeren Slot machen
			//
			if (Pruefsumme != Savegames[i].Pruefsumme)
				Savegames[i].Stage = -1;
		}


	}
}

// --------------------------------------------------------------------------------------
// Savegames anzeigen
// --------------------------------------------------------------------------------------

void MenuClass::ShowSavegames(int Highlight)
{
	char buffer[100];
	D3DCOLOR col;

	pDefaultFont->DrawTextCenterAlign(xpos + 100, ypos + 120 , TextArray[TEXT_SAVE_STAGE], 0xFFFFFFFF, 0);
	pDefaultFont->DrawTextCenterAlign(xpos + 150, ypos + 120 , TextArray[TEXT_SAVE_SPIELER], 0xFFFFFFFF, 0);
	pDefaultFont->DrawTextCenterAlign(xpos + 250, ypos + 120 , TextArray[TEXT_SAVE_DATUM], 0xFFFFFFFF, 0);

	// Alle Savegames anzeigen
	for (int i=0; i<MAX_SAVEGAMES; i++)
	{
		if (i == Highlight)
			col = 0xFFFFFFFF;
		else
			col = 0x80FFFFFF
			;
		// Stage anzeigen
		//
		if (Savegames[i].Stage >= 0)
		{
			_itoa_s(Savegames[i].Stage, buffer, 10);
			pDefaultFont->DrawTextCenterAlign(xpos + 100, ypos + 150 + i*14, buffer, col, 0);
		}

		// Spieler anzeigen
		//
		if (Savegames[i].Stage >= 0)
		{
			_itoa_s(Savegames[i].Players, buffer, 10);
			pDefaultFont->DrawTextCenterAlign(xpos + 150, ypos + 150 + i*14, buffer, col, 0);
		}

		// Skill anzeigen
		//
		if (Savegames[i].Stage >= 0)
		{
			_itoa_s(Savegames[i].Players, buffer, 10);
			pDefaultFont->DrawTextCenterAlign(xpos + 150, ypos + 150 + i*14, buffer, col, 0);
		}


		// Ist ein Savegame gefunden worden ?
		if (Savegames[i].Stage >= 0)
		{
			pDefaultFont->DrawTextCenterAlign(xpos + 250, ypos + 150 + i*14, Savegames[i].Name, col, 0);
		}

		// oder ist dort noch ein leerer Slot ?
		else
		{
			pDefaultFont->DrawTextCenterAlign(320, ypos + 150 + i*14, TextArray[TEXT_SAVE_LEER], col, 0);
		}
	}
}

// --------------------------------------------------------------------------------------
// Highscore-Liste laden
// --------------------------------------------------------------------------------------

void MenuClass::LoadHighscore(void)
{
	FILE *Datei;			// Savegame Datei

	// Versuchen, die Highscore Datei zu öffnen
	// falls sie nicht existiert oder es eine Fehler gibt, wird die Standard
	// Highscore gesetzt
	fopen_s(&Datei, "Hurrican.hsl", "rb");


	// Fehler beim Öffnen ? Dann standard Highscore setzen
	//
	if (Datei == NULL)
		ResetHighscore();

	else
	{
		// Ansonsten Scores auslesen
		//
		for (int i=0; i<MAX_HIGHSCORES; i++)
			fread(&Highscores[i], sizeof(Highscores[i]), 1, Datei);
			
		// Und Datei wieder schliessen
		//
		fclose(Datei);

		// Checken ob die Pruefsumme noch stimmt
		// und nur wenn diese stimmt die Highscore übernehmen
		// ansonsten wieder standard Highscore setzen
		long Pruefsumme;

		for (int i=0; i<MAX_HIGHSCORES; i++)
		{
			Pruefsumme = 0;

			for (unsigned int j=0; j<strlen(Highscores[i].Name); j++)
				Pruefsumme += Highscores[i].Name[j];

			Pruefsumme += strlen(Highscores[i].Name) +
						  Highscores[i].Score +
						  Highscores[i].Stage +
						  Highscores[i].Skill;

			// Pruefsumme stimmt nicht mehr ?
			//
			if (Pruefsumme != Highscores[i].Pruefsumme)
			{
				strcpy_s(Highscores[i].Name, "!!! CHEATER !!!");
				Highscores[i].Stage = 0;
			}
		}
	}
}

// --------------------------------------------------------------------------------------
// Highscore-Liste speichern
// --------------------------------------------------------------------------------------

void MenuClass::SaveHighscore(void)
{
	FILE *Datei;			// Savegame Datei

	// Highscore Datei öffnen	
	fopen_s(&Datei, "Hurrican.hsl", "wb");

	// Fehler beim Öffnen ? Dann standard Highscore setzen
	//
	if (Datei == NULL)
		ResetHighscore();

	else
	{
		// Ansonsten Scores speichern
		//
		for (int i=0; i<MAX_HIGHSCORES; i++)
		{
			// Prüfsumme erstellen
			//
			Highscores[i].Pruefsumme = 0;

			for (unsigned int j=0; j<strlen(Highscores[i].Name); j++)
				Highscores[i].Pruefsumme += Highscores[i].Name[j];

			Highscores[i].Pruefsumme += strlen(Highscores[i].Name) +
						  Highscores[i].Score +
						  Highscores[i].Stage +
						  Highscores[i].Skill;

			// Und Eintrag speichern
			//
			fwrite(&Highscores[i], sizeof(Highscores[i]), 1, Datei);
		}
	}
			
	// Und Datei wieder schliessen
	//
	fclose(Datei);
}

// --------------------------------------------------------------------------------------
// Highscore-Liste mit Standardwerten resetten
// --------------------------------------------------------------------------------------

void MenuClass::ResetHighscore(void)
{
	char HighscoreNames[MAX_HIGHSCORES][30] = 
	{
	 "EISWUXE",
	 "TURRI",
	 "MANFRED",
	 "CHRIS",
	 "ANDREAS",
	 "JULIAN",
	 "RAMIRO",
	 "PETER",
	 "CELAL",
	 "MARKUS",
	};							 

	for (int i=0; i<MAX_HIGHSCORES; i++)
	{
		Highscores[i].Score = (MAX_HIGHSCORES-i)*10000;
		Highscores[i].Skill = i%4;
		Highscores[i].Stage = MAX_HIGHSCORES-i;

		if (Highscores[i].Stage > 9)
			Highscores[i].Stage = 9;

	   strcpy_s(Highscores[i].Name, HighscoreNames[i]);

		for (unsigned int j=0; j<strlen(Highscores[i].Name); j++)
			Highscores[i].Pruefsumme += Highscores[i].Name[j];

		Highscores[i].Pruefsumme += strlen(Highscores[i].Name) +
										  Highscores[i].Score +
										  Highscores[i].Stage;
	}
}

// --------------------------------------------------------------------------------------
// Infos aus der Language Datei anzeigen lassen (Zeile 1 - 8)
// --------------------------------------------------------------------------------------

void MenuClass::ShowLanguageInfo (void)
{
	int a1, a2;

	if (ShowLanguageInfoCounter < 0.0f)
		return;

	a1 = int (ShowLanguageInfoCounter);

	if (a1 > 255)
		a1 = 255;

	a2 = int (a1 / 2);

	RenderRect (320 - 152, 240 - 82, 304, 164, D3DCOLOR_RGBA(64, 128, 255, a2));
	RenderRect (320 - 151, 240 - 81, 302, 162, D3DCOLOR_RGBA(0,   0,   64,   a2));
	RenderRect (320 - 150, 240 - 80, 300, 160, D3DCOLOR_RGBA(0,   0,   64,   a2));

	pDefaultFont->DrawText (float (320 - pDefaultFont->StringLength (TextArray[TEXT_BENUTZTES_FILE]) / 2), float (240 - 64), TextArray[TEXT_BENUTZTES_FILE], D3DCOLOR_RGBA(255, 255, 255, a1));

	for (int i = 0; i < 9; i++)
	{
		int  xoff = 320 - strlen (TextArray[i])*4; 
		for (unsigned int j = 0; j < strlen (TextArray[i]); j++)
		{
			char c[2];

			c[0] = TextArray[i][j];
			c[1] = '\0';
			pDefaultFont->DrawText (float (xoff), float (240 - 40 + i * 12), c, D3DCOLOR_RGBA(255, 255, 255, a1));
			xoff += 8;
		}
	}
} // ShowLanguageInfo

// --------------------------------------------------------------------------------------
// Prüfen obs eine neue Highscore gibt
// --------------------------------------------------------------------------------------

void MenuClass::CheckForNewHighscore(void)
{
	AktuellerZustand = MENUPUNKT_HIGHSCORES;
	pSoundManager->StopAllSongs(false);
	pSoundManager->StopAllSounds();
	pSoundManager->StopAllLoopedSounds();
	pSoundManager->PlaySong(MUSIC_HIGHSCORE, false);

	// Feststellen ob wir eine neue Highscore erreicht haben
	//
	NewScore = 0;
	NewRank  = 0;
	AktuellerPunkt = 0;
	strcpy_s(NewName, "");

	// Alle Highscores durchgehen und vergleichen
	//
	bool besser = false;

	for (int j=0; j<MAX_HIGHSCORES; j++)
	if (pPlayer[0]->Score >= Highscores[j].Score && besser == false)
	{
		besser = true;

		// Ab der Stelle wo ein Namen eingefügt wird eine Zeile runterscrollen
		//
		for (int k=MAX_HIGHSCORES-1; k>j; k--)
		{
			Highscores[k].Score = Highscores[k-1].Score;
			Highscores[k].Stage = Highscores[k-1].Stage;
			Highscores[k].Skill = Highscores[k-1].Skill;
			strcpy_s(Highscores[k].Name, Highscores[k-1].Name);
		}

		Highscores[j].Score = pPlayer[0]->Score;
		NewScore = pPlayer[0]->Score;
		NewStage = Stage;
		NewSkill = Skill;
		pPlayer[0]->Score = 0;
		
		// Werte für den neuen Eintrag löschen
		NewRank = j;
		strcpy_s(NewName, "");			
		strcpy_s(Highscores[NewRank].Name, "");
		AktuellerZustand = MENUZUSTAND_ENTERNAME;
		break;
	}

	SpielZustand = MAINMENU;
}