// Datei : Menu.cpp

// --------------------------------------------------------------------------------------
//
// Menu-Klasse f�r das
// Hurrican Hauptmenu
//
// (c) 2002 J�rg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#if !defined(__AROS__) && !defined(__MORPHOS__) && !defined(__amigaos4__)
#include <sys/timeb.h>
#endif
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
#include "Partikelsystem.h"
#include "Timer.h"

// --------------------------------------------------------------------------------------
// Endianess handling
// We will Swap values only for Big_Endian, Little_Endian should be unchanged
// --------------------------------------------------------------------------------------
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define SWAP32(X) (X)
#else
#define SWAP32(X) SDL_Swap32(X)
#endif

// --------------------------------------------------------------------------------------
// Die Credits
// --------------------------------------------------------------------------------------

int CreditsCount;
const char *Credits[] =
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
    "- Hurrican -",
    "",
    "(c) 2007",
    "poke53280",
    "",
    "",
    "",
    "",
    "J�rg 'Eiswuxe' Winterstein",
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
    "f�r zus�tzliches Leveldesign und",
    "viele Verbesserungsvorschl�ge"
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
    "Manuel 'Heimdall' R�lke",
    "Friedrich 'BadData' Volmering",
    "Christian 'Gryphon' Kleinsteinberg",
    "Marc 'Dawn2069MS' Schmitt",
    "",
    "",
    "",
    "Zudem hier nochmal ein fettes Dankesch�n an alle unsere Fans,",
    "die uns jahrelang motiviert haben und an uns glaubten.",
    "Danke, dass ihr Hurrican nie aufgegeben habt.",
    "Dieses Spiel ist f�r euch!",
    "",
    "",
    "",
    "Eiswuxe's Gr��e gehen an",
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
    "Manuel 'Heimdall' R�lke",
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
    "Nat�rlich auch die Super Konrad Brothers,",
    "Dex, Bronko, RuvF, Aky, Contra-254 und",
    "alle anderen Turrican Fans!",
    "",
    "Pekaro Soft",
    "",
    "Steve Harris :)",
    "",
    "Nat�rlich darf auch der wahre Meister nicht fehlen",
    "Einen fetten Gru� an den Manne!",
    "Wer fleissig sucht, der kann ihm",
    "Tief im Jungel seine Ehrerbietung darbringen...",
    "",
    "Und das Beste kommt nat�rlich zum Schlu�...",
    "Meine kleine Kathrin",
    "Ei laf juh ;)",
    "",
    "",
    "",
    "Und jetzt ist der Turri dran:",
    "",
    "ich gr��e meine Mutti das wars...",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "...noch lange nicht..",
    "",
    "ich gr��e",
    "",
    "meine Schnegge, die ich �ber alles liebe",
    "",
    "meine Br�der - Carsten und Dirki -- der Lappen",
    "",
    "ich gr��e Wenkman den Hardcore Tester",
    "",
    "alle aus dem Forum die viele Ideen zu Hurrican beigetragen haben",
    "und uns �ber 5 Jahre begleitet haben und uns die Treue gehalten haben."
    "",
    "ich gr��e die anderen Pokes Wanderer und Skyrider und",
    "",
    "ich gr��e Markus 'Snake' der sich immer saugut um unsere Webseite gek�mmert hat",
    "",
    "ich gr��e Peer Draeger  Wit Wit Wit",
    "",
    "dann den Rest aus meiner Arbeit",
    "Marc Schmidt, Christian Kleinsteinberg, Uwe Wegener, Tetjana Obly Wobly ;-)",
    "",
    "ich gr��e Boris Triebel - Schau wir habens rausgebracht hehehe",
    "",
    "Marc Kamradt",
    "",
    "ich gr��e Matthias die Todesschlampe",
    "",
    "Nat�rlich gr��e ich auch Manfred Trenz",
    "der meine Jugend mit Turrican vers�sst hat und f�r mich immer 'The Master' war",
    "",
    "ich gr��e alle Jungs und M�dels die das hier lesen",
    "und Hurrican zocken und hoffe Ihr habt Spass :-)",
    "",
    "ich gr��e Gigi und seine Katze .. (und auch Crazy)",
    "",
    "ich gr��e Adnan den alten Partyl�wen und guten Freund",
    "",
    "ich gr��e alle aus rendering.de die mir oft mit Tipps",
    "bei meinen Grafiken geholfen haben",
    "",
    "und ich gr��e den Krapfen",
    "",
    "ich gr��e Thorsten Mutschall und Tanja K�hler",
    "und hoffe Sie haben Spass am spielen :-))",
    "ich gr��e Micha Black Raven und seine verlorene seelen",
    "",
    "ich gr��e alle vom GVC Gamevoiceclan",
    "the Best ever .. auch wenns jetzt vorbei ist ..",
    "",
    "ich gr��e Velociraptor",
    "",
    "ich gr��e alle die ich jetzt vergessen habe",
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

//DKS - Made a version of the above credits text array meant for use on
//      low resolution displays that use text scaling. Long lines are split,
//      and there's less repeated blank lines.
int LowResCreditsCount;
const char *LowResCredits[] =
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
    "- Hurrican -",
    "",
    "(c) 2007",
    "poke53280",
    "",
    "",
    "J�rg 'Eiswuxe' Winterstein",
    "",
    "Programmierung",
    "Gamedesign",
    "Sound & Musik",
    "Leveldesign",
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
    "Besonderer Dank geht an",
    "",
    "T.R.Schmidt und Gerhard 'ScHlAuChi' Weihrauch",
    "f�r zus�tzliches Leveldesign und",
    "viele Verbesserungsvorschl�ge"
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
    "Manuel 'Heimdall' R�lke",
    "Friedrich 'BadData' Volmering",
    "Christian 'Gryphon' Kleinsteinberg",
    "Marc 'Dawn2069MS' Schmitt",
    "",
    "",
    "Zudem hier nochmal ein fettes Dankesch�n an alle",
    "unsere Fans, die uns jahrelang motiviert haben",
    "und an uns glaubten. Danke, dass ihr Hurrican nie",
    "aufgegeben habt. Dieses Spiel ist f�r euch!",
    "",
    "",
    "Eiswuxe's Gr��e gehen an",
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
    "Manuel 'Heimdall' R�lke",
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
    "Nat�rlich auch die Super Konrad Brothers,",
    "Dex, Bronko, RuvF, Aky, Contra-254 und",
    "alle anderen Turrican Fans!",
    "",
    "Pekaro Soft",
    "",
    "Steve Harris :)",
    "",
    "Nat�rlich darf auch der wahre Meister nicht fehlen",
    "Einen fetten Gru� an den Manne!",
    "Wer fleissig sucht, der kann ihm",
    "Tief im Jungel seine Ehrerbietung darbringen...",
    "",
    "Und das Beste kommt nat�rlich zum Schlu�...",
    "Meine kleine Kathrin",
    "Ei laf juh ;)",
    "",
    "",
    "Und jetzt ist der Turri dran:",
    "",
    "ich gr��e meine Mutti das wars...",
    "",
    "",
    "",
    "...noch lange nicht..",
    "",
    "ich gr��e",
    "",
    "meine Schnegge, die ich �ber alles liebe",
    "",
    "meine Br�der - Carsten und Dirki -- der Lappen",
    "",
    "ich gr��e Wenkman den Hardcore Tester",
    "",
    "alle aus dem Forum die viele Ideen zu Hurrican",
    "beigetragen haben und uns �ber 5 Jahre begleitet",
    "haben und uns die Treue gehalten haben.",
    "",
    "ich gr��e die anderen Pokes Wanderer",
    "und Skyrider und",
    "",
    "ich gr��e Markus 'Snake' der sich immer saugut um",
    "unsere Webseite gek�mmert hat",
    "",
    "ich gr��e Peer Draeger  Wit Wit Wit",
    "",
    "dann den Rest aus meiner Arbeit",
    "Marc Schmidt, Christian Kleinsteinberg,",
    "Uwe Wegener, Tetjana Obly Wobly ;-)",
    "",
    "ich gr��e Boris Triebel - Schau wir habens",
    "rausgebracht hehehe",
    "",
    "Marc Kamradt",
    "",
    "ich gr��e Matthias die Todesschlampe",
    "",
    "Nat�rlich gr��e ich auch Manfred Trenz",
    "der meine Jugend mit Turrican vers�sst hat",
    "und f�r mich immer 'The Master' war",
    "",
    "ich gr��e alle Jungs und M�dels die das hier lesen",
    "und Hurrican zocken und hoffe Ihr habt Spass :-)",
    "",
    "ich gr��e Gigi und seine Katze .. (und auch Crazy)",
    "",
    "ich gr��e Adnan den alten Partyl�wen",
    "und guten Freund",
    "",
    "ich gr��e alle aus rendering.de die mir oft mit Tipps",
    "bei meinen Grafiken geholfen haben",
    "",
    "und ich gr��e den Krapfen",
    "",
    "ich gr��e Thorsten Mutschall und Tanja K�hler",
    "und hoffe Sie haben Spass am spielen :-))",
    "ich gr��e Micha Black Raven",
    "und seine verlorene seelen",
    "",
    "ich gr��e alle vom GVC Gamevoiceclan",
    "the Best ever .. auch wenns jetzt vorbei ist ..",
    "",
    "ich gr��e Velociraptor",
    "",
    "ich gr��e alle die ich jetzt vergessen habe",
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
    "#eoc"			// end of credits =)

};

int tempNUMPLAYERS;

// --------------------------------------------------------------------------------------
// Konstruktor : Menu Grafiken laden und Werte initialisieren
// --------------------------------------------------------------------------------------

MenuClass::MenuClass(void)
{
    MenuNebel.LoadImage("hurrican_rund.bmp", 512, 512, 512, 512, 1, 1);
    MenuTitel.LoadImage("hurrican-logo.bmp", 400, 90, 400, 90, 1, 1);

    Skills.LoadImage("skills.png", 200, 200, 100, 100, 2, 2);

    MenuStar.LoadImage("star.bmp", 5, 5, 5, 5, 1, 1);
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

    // Farben f�r die Highscore erstellen, in der die Namen aufblinken
    int i = 0;
    HighscoreColors[i] = D3DCOLOR_RGBA( 75,  75,  75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(105, 105, 105, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(135, 135, 135, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(165, 165, 165, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(195, 195, 195, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(225, 225, 225, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 255, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 225, 225, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 195, 195, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 165, 165, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 135, 135, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 105, 105, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,  75,  75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,  45,  45, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,  15,  15, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,   0,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,  15,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,  45,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,  75,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 105,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 135,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 165,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 195,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 225,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 255,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(225, 255,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(195, 255,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(165, 255,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(135, 255,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(105, 255,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 75, 255,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 45, 255,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 15, 255,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255,  15, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255,  45, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255,  75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 105, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 135, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 165, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 195, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 225, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 15, 255, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 45, 255, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 75, 255, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(105, 255, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(135, 255, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(165, 255, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(195, 255, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(225, 255, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 255, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 255, 225, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 240, 195, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 225, 165, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 210, 135, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 195, 105, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 180,  75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 165,  45, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 165,  15, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 150,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 135,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 105,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,  75,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,  45,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,  15,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,   0,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,   0,  15, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,   0,  45, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,   0,  75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,   0, 105, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,   0, 135, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,   0, 165, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,   0, 195, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,   0, 225, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,   0, 254, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(225,  15, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(195,  45, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(165,  75, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(135, 105, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(105, 135, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 75, 165, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 45, 195, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 15, 225, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 225, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 195, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 165, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 135, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255, 105, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255,  75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255,  45, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 255,  15, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(  0, 225,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 15, 195,  15, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 30, 180,  30, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 45, 165,  45, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 60, 150,  60, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 75, 135,  75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 75, 120,  75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 75, 105,  75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA( 75,  90,  75, 255);
    i++;

    // Die ersten 20 nochmal ...

    HighscoreColors[i] = D3DCOLOR_RGBA( 75,  75,  75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(105, 105, 105, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(135, 135, 135, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(165, 165, 165, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(195, 195, 195, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(225, 225, 225, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 255, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 225, 225, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 195, 195, 255);
    /* DKS - Commented out the rest of this, as it is writing past the end of the array: */
    /*
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 165, 165, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 135, 135, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 105, 105, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,  75,  75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,  45,  45, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,  15,  15, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,   0,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,  15,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,  45,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255,  75,   0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 105,   0, 255);
    i++;
    */

    BlinkOffset  = 0;
    BlinkCounter = 0.75f;


    // Sprache feststellen und schauen, welche Grafik wir anzeigen m�ssen
    if (strcmp (TextArray[TEXT_BENUTZTE_GRAFIK], "0") == 0)
        Sprachgrafik = 0;		// deutsch ?
    else
        Sprachgrafik = 1;		// oder doch englisch ?

    // Highscore laden
    //
    LoadHighscore();

    ShowLanguageInfoCounter = 0.0f;
    control_reassignment_occuring = false;

    // creditscount rausfinden
    CreditsCount = 0;
    while (strcmp(Credits[CreditsCount], "#eoc") != 0)
        CreditsCount++;

    //DKS - Made a version of the above credits text array meant for use on
    //      low resolution displays that use text scaling. Long lines are split,
    //      and there's less repeated blank lines.
    LowResCreditsCount = 0;
    while (strcmp(LowResCredits[LowResCreditsCount], "#eoc") != 0)
        LowResCreditsCount++;

    //DKS - Initialize NewName with just the cursor char
    strcpy_s(NewName, "_");
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
// Hintergrund der Men�s anzeigen
// wird auch f�r den Progressbar benutzt
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

    // Total l�schen

#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                       D3DCOLOR_XRGB(0,0,0),	1.0f, 0);
#elif defined(PLATFORM_SDL)
    DirectGraphics.ClearBackBuffer();
#endif

    ShowMenuBack();

    //DKS - Added lowres support, which has very limited room at bottom:
    if (CommandLineParams.LowRes) {
        // Under Low-res, don't display controls message at bottom and shift other text up a bit
        pDefaultFont->DrawText(10.0f, 458, "www.poke53280.de", menucolor, 0);
        pDefaultFont->DrawText(640-pDefaultFont->StringLength("www.hurrican-game.de", 0)-10,
                                            458, "www.hurrican-game.de", menucolor, 0);
#if defined(GCW)
        // Draw port credit text for Pickle & Senquack
        if (AktuellerZustand == MENUZUSTAND_MAINMENU)
            pDefaultFont->DrawTextCenterAlign(320.0f, 428, "OpenGL and GCW Zero port by Pickle & Senquack", menucolor, 0);
#endif
    } else {
        if (AktuellerZustand != MENUPUNKT_CREDITS &&
                AktuellerZustand != MENUZUSTAND_ENTERNAME)

            pDefaultFont->DrawTextCenterAlign(320.0f, 462, TextArray [TEXT_MENUE_ANLEITUNG], menucolor, 0);
        pDefaultFont->DrawText(10.0f, 462, "www.poke53280.de", menucolor, 0);
        pDefaultFont->DrawText(640-pDefaultFont->StringLength("www.hurrican-game.de", 0)-10,
                                            462, "www.hurrican-game.de", menucolor, 0);
    }

    // graue Linien
    //
    /*
    	RenderRect(0, 33, 156, 1, menucolor2);
    	RenderRect(0, 63, 156, 1, menucolor2);

    	RenderRect(493, 33, 150, 1, menucolor2);
    	RenderRect(493, 63, 150, 1, menucolor2);
    */

    RenderRect(0, 455, 640, 1, menucolor2);

    // DKS - Do not draw the large "HURRICAN" title graphic when using scaled fonts and the
    //       controls reconfiguration menu is active - there is not enough room otherwise
    if (pDefaultFont->GetScaleFactor() <= 1 || AktuellerZustand != MENUZUSTAND_TASTEN) {
        MenuTitel.RenderSprite((640 - 400) / 2 + 15, 0, menucolor);
    }

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

    }
    break;

    case MENUZUSTAND_STARTGAME :
    {
    } break;

    case MENUZUSTAND_VOLUMES :
    {
        const int OFFSET2 = 20;

        float d = (float)(pMenuFont->StringLength(TextArray [TEXT_MENUE_EINSTELLUNGEN], 2));
        pMenuFont->DrawText(320 - d/2.0f, ypos + OFFSET - OFFSET2, TextArray [TEXT_MENUE_EINSTELLUNGEN], menucolor, 2);

        // Sound / Musik Lautst�rke
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


    }
    break; // MENUZUSTAND_VOLUMES

    case MENUZUSTAND_LANGUAGE :
    {
        //DKS - Menu now displays filenames without path or extensions and supports low-resolution
        int vertical_spacing = 16;
        if (CommandLineParams.LowRes) {
            vertical_spacing += 8;
        }

        float d = (float)(pMenuFont->StringLength(TextArray [TEXT_SPRACHE], 2));
        pMenuFont->DrawText(320 - d/2.0f, ypos + OFFSET, TextArray [TEXT_SPRACHE], menucolor, 2);

        char lang_name[256] = "";
        for (int i = 0; i < LanguageFileCount; i++)
        {

            strcpy_s(lang_name, LanguageFiles[i]);
            // Truncate the extension
            int trunc_loc = strlen(lang_name) - 4;
            lang_name[trunc_loc] = '\0';
            
            // Make first character upper-case
            lang_name[0] = toupper( lang_name[0] );

            pDefaultFont->DrawText (320 - pDefaultFont->StringLength(lang_name) / 2.0f, 
                    ypos + 120 + i * vertical_spacing, lang_name, 0x88FFFFFF);
            if (AktuellerPunkt == i)
                pDefaultFont->DrawText (320 - pDefaultFont->StringLength(lang_name) / 2.0f, 
                        ypos + 120 + i * vertical_spacing, lang_name, 0x88FFFFFF);
        }

        pDefaultFont->DrawText (320 - pDefaultFont->StringLength (TextArray[TEXT_ZURUECK]) / 2.0f, 
                ypos + 136 + vertical_spacing * LanguageFileCount, (TextArray[TEXT_ZURUECK]), 0x88FFFFFF);
        if (AktuellerPunkt == LanguageFileCount)
            pDefaultFont->DrawText (320 - pDefaultFont->StringLength (TextArray[TEXT_ZURUECK]) / 2.0f, 
                    ypos + 136 + vertical_spacing * LanguageFileCount, (TextArray[TEXT_ZURUECK]), 0x88FFFFFF);


    }
    break; // MENUZUSTAND_LANGUAGE

    case MENUZUSTAND_TASTEN :
    {
        const int scale_factor = pDefaultFont->GetScaleFactor();
        int off_x = xpos;
        int off_y = ypos;
        int col0_off_x = off_x + 30;
        int col1_off_x = off_x + 40;
        int col2_off_x = 230;
        int title_off_y = off_y + OFFSET - 60;  // Configure Controls title line

        int line_spacing = 16;               // Original line spacing for non-scaled fonts
        int line1_off_y = off_y + 70;        // Top line of menu (Force Feedback line)

        // Offsets for use with scaled-fonts on low-res devices:
        if (scale_factor > 1) {
            off_y = 5;
            title_off_y = off_y + 10;
            line_spacing = 24;    // Kinda tight using a scaled font, but it all needs to fit
            col0_off_x = 270;
            col1_off_x = col0_off_x + 10;
            col2_off_x = 180;
            line1_off_y = title_off_y;
        }

        int controls_off_y = line1_off_y + MENU_TASTEN_NUM_NON_CONTROLS * line_spacing;    // Controls lines Y-offset

        // "CONFIGURE CONTROLS" title - Do not draw when using scaled fonts, as there's no room
        if (scale_factor <= 1) {
            pMenuFont->DrawTextCenterAlign(320, title_off_y, TextArray[TEXT_STEUERUNG], menucolor, 2);
        }

        // �berschrift "Spieler1" "Spieler2"
        D3DCOLOR p1_col, p2_col;
        if (AktuellerPunkt >= MENU_TASTEN_PLAYER_LINE) {
            if (CurrentPlayer == 0) {
                p1_col = 0xFFFFFFFF;
                p2_col = 0x88FFFFFF;
                RenderRect(col1_off_x - 4, line1_off_y + MENU_TASTEN_PLAYER_LINE*line_spacing - 2, 
                        (float)pDefaultFont->StringLength(TextArray[TEXT_PLAYER_ONE]) + 10, line_spacing, 0x30FFFFFF);
            } else {
                p1_col = 0x88FFFFFF;
                p2_col = 0xFFFFFFFF;
                RenderRect(col1_off_x + col2_off_x - 4, line1_off_y+MENU_TASTEN_PLAYER_LINE*line_spacing - 2, 
                        (float)pDefaultFont->StringLength(TextArray[TEXT_PLAYER_TWO]) + 10, line_spacing, 0x30FFFFFF);
            }
        } else {
            p1_col = 0x88FFFFFF;
            p2_col = 0x88FFFFFF;
        }

        pDefaultFont->DrawText (col1_off_x, line1_off_y+MENU_TASTEN_PLAYER_LINE*line_spacing, 
                TextArray[TEXT_PLAYER_ONE], p1_col);
        pDefaultFont->DrawText (col1_off_x + col2_off_x, line1_off_y+MENU_TASTEN_PLAYER_LINE*line_spacing, 
                TextArray[TEXT_PLAYER_TWO], p2_col);

        // Force-feedback checkbox:
        {
            D3DCOLOR col;
            if (AktuellerPunkt == MENU_TASTEN_FORCEFEEDBACK_LINE)
                col = 0xFFFFFFFF;
            else
                col = 0x88FFFFFF;

            int bar_off_y = line1_off_y + MENU_TASTEN_FORCEFEEDBACK_LINE*line_spacing + 1;
            int bar_dim = 8;
            int border_w = 1;

            if (scale_factor > 1) {
                bar_dim = 12;
                border_w = 2;
            }

            RenderRect(col0_off_x-border_w-bar_dim, bar_off_y, bar_dim, border_w, col);
            RenderRect(col0_off_x-border_w-bar_dim, bar_off_y+bar_dim+border_w, bar_dim, border_w, col);
            RenderRect(col0_off_x-border_w*2-bar_dim, bar_off_y, border_w, bar_dim+border_w*2, col);
            RenderRect(col0_off_x-border_w, bar_off_y, border_w, bar_dim+border_w*2, col);

            if (UseForceFeedback)
                RenderRect(col0_off_x-border_w-bar_dim, bar_off_y+border_w, bar_dim, bar_dim, col);

            pDefaultFont->DrawText(col1_off_x, line1_off_y+MENU_TASTEN_FORCEFEEDBACK_LINE*line_spacing,
                    TextArray[TEXT_FORCEFEEDBACK], col);
        }

        // Defaults, type, mode, sensitivity lines
        if (AktuellerPunkt == MENU_TASTEN_DEFAULTS_LINE)
            pDefaultFont->DrawText(col1_off_x, line1_off_y+MENU_TASTEN_DEFAULTS_LINE*line_spacing, 
                    TextArray[TEXT_DEFAULTS], 0xFFFFFFFF);
        else
            pDefaultFont->DrawText(col1_off_x, line1_off_y+MENU_TASTEN_DEFAULTS_LINE*line_spacing, 
                    TextArray[TEXT_DEFAULTS], 0x88FFFFFF);

        if (AktuellerPunkt == MENU_TASTEN_TYPE_LINE)
            pDefaultFont->DrawTextRightAlign(col0_off_x, line1_off_y+MENU_TASTEN_TYPE_LINE*line_spacing, 
                    TextArray[TEXT_TYP], 0xFFFFFFFF);
        else
            pDefaultFont->DrawTextRightAlign(col0_off_x, line1_off_y+MENU_TASTEN_TYPE_LINE*line_spacing, 
                    TextArray[TEXT_TYP], 0x88FFFFFF);

        if (AktuellerPunkt == MENU_TASTEN_MODE_LINE)
            pDefaultFont->DrawTextRightAlign(col0_off_x, line1_off_y+MENU_TASTEN_MODE_LINE*line_spacing, 
                    TextArray[TEXT_MODUS], 0xFFFFFFFF);
        else
            pDefaultFont->DrawTextRightAlign(col0_off_x, line1_off_y+MENU_TASTEN_MODE_LINE*line_spacing, 
                    TextArray[TEXT_MODUS], 0x88FFFFFF);

        if (AktuellerPunkt == MENU_TASTEN_SENSITIVITY_LINE)
            pDefaultFont->DrawTextRightAlign(col0_off_x, line1_off_y+MENU_TASTEN_SENSITIVITY_LINE*line_spacing, 
                    TextArray[TEXT_EMPFINDLICHKEIT], 0xFFFFFFFF);
        else
            pDefaultFont->DrawTextRightAlign(col0_off_x, line1_off_y+MENU_TASTEN_SENSITIVITY_LINE*line_spacing, 
                    TextArray[TEXT_EMPFINDLICHKEIT], 0x88FFFFFF);

        for (int i = 0; i < MENU_TASTEN_NUM_CONTROLS; i++)
        {
            pDefaultFont->DrawTextRightAlign (col0_off_x, controls_off_y + i * line_spacing, 
                    TextArray[TEXT_TASTEN_L + i], 0x88FFFFFF);

            if (AktuellerPunkt - MENU_TASTEN_NUM_NON_CONTROLS == i)
                pDefaultFont->DrawTextRightAlign (col0_off_x, controls_off_y + i * line_spacing, 
                        TextArray[TEXT_TASTEN_L + i], 0x88FFFFFF);
        }

        // Selection bar:
        RenderRect(10, line1_off_y-2 + AktuellerPunkt * line_spacing, 620, line_spacing, 0x24FFFFFF);

        // F�r beide Spieler den ganzen Klumbatsch anzeigen
        for (int j = 0; j < 2; j++)
        {
            D3DCOLOR col;
            PlayerClass* pCurrentPlayer;

            if (j == 0)
                pCurrentPlayer = pPlayer[0];
            else
                pCurrentPlayer = pPlayer[1];

            // Typ (Tastatur, Joystick (mit Name)
            if (CurrentPlayer == j && AktuellerPunkt == MENU_TASTEN_TYPE_LINE)
                col = 0xFFFFFFFF;
            else
                col = 0x88FFFFFF;

            if (pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD)
                // Keyboard text
                pDefaultFont->DrawText (col1_off_x + j * col2_off_x, line1_off_y+MENU_TASTEN_TYPE_LINE*line_spacing, 
                        TextArray[TEXT_KEYBOARD], col);
            else {
                // Print joystick name, truncated to fit if necessary
                char tmpbuf[120];
                sprintf_s(tmpbuf, "%d:%s", pCurrentPlayer->JoystickIndex,
                                              DirectInput.Joysticks[pCurrentPlayer->JoystickIndex].JoystickName);
                int cutoff = strlen(tmpbuf) - 1;
                int max_w = col2_off_x - 10;
                while (cutoff > 1 && pDefaultFont->StringLength(tmpbuf) > max_w) {
                    tmpbuf[cutoff] = '\0';
                    cutoff--;
                }
                pDefaultFont->DrawText (col1_off_x + j * col2_off_x, line1_off_y+MENU_TASTEN_TYPE_LINE*line_spacing, 
                        tmpbuf, col);
            }

            // Rechteck f�r Empfindlichkeit
            if (pCurrentPlayer->ControlType == CONTROLTYPE_JOY)
            {
                if (CurrentPlayer == j && AktuellerPunkt == MENU_TASTEN_SENSITIVITY_LINE)
                    col = 0xFFFFFFFF;
                else
                    col = 0x88FFFFFF;

                int bar_off_y = line1_off_y + MENU_TASTEN_SENSITIVITY_LINE*line_spacing + 1;
                int bar_height = 8;
                int border_w = 1;

                if (scale_factor > 1) {
                    bar_off_y += 2;
                    bar_height = 12;
                    border_w = 2;
                }

                RenderRect(col1_off_x + border_w + j * col2_off_x, bar_off_y, 100, border_w, col);
                RenderRect(col1_off_x + border_w + j * col2_off_x, bar_off_y + bar_height+border_w, 100, border_w, col);
                RenderRect(col1_off_x + j * col2_off_x, bar_off_y, border_w, bar_height + border_w*2, col);
                RenderRect(col1_off_x + 100 + border_w + j * col2_off_x, bar_off_y, border_w, bar_height + border_w*2, col);

                // Aktueller Balken
                RenderRect(col1_off_x + j * col2_off_x + border_w, bar_off_y + border_w, 
                        (1000.0f - (pCurrentPlayer->JoystickSchwelle + 100.0f))/8.0f, bar_height, col);

                // Modus
                if (CurrentPlayer == j && AktuellerPunkt == MENU_TASTEN_MODE_LINE)
                    col = 0xFFFFFFFF;
                else
                    col = 0x88FFFFFF;

                if (pCurrentPlayer->JoystickMode == JOYMODE_JOYSTICK)
                    pDefaultFont->DrawText(col1_off_x + j * col2_off_x, line1_off_y+MENU_TASTEN_MODE_LINE*line_spacing, 
                            TextArray[TEXT_JOYMODE_STICK], col);
                else
                    pDefaultFont->DrawText(col1_off_x + j * col2_off_x, line1_off_y+MENU_TASTEN_MODE_LINE*line_spacing,
                            TextArray[TEXT_JOYMODE_PAD], col);
            }

            for (int i = 0; i < MENU_TASTEN_NUM_CONTROLS; i++)
            {
                bool on_move_line = (i==AKTION_LINKS || i==AKTION_RECHTS || i==AKTION_DUCKEN);
                bool on_look_line = (i==AKTION_OBEN || i==AKTION_UNTEN);
                bool on_jump_line = (i==AKTION_JUMP);
                D3DCOLOR col;
                if (AktuellerPunkt - MENU_TASTEN_NUM_NON_CONTROLS  == i && CurrentPlayer == j)
                    col = 0xFFFFFFFF;
                else
                    col = 0x88FFFFFF;

                if (CurrentPlayer != j || 
                        AktuellerPunkt - MENU_TASTEN_NUM_NON_CONTROLS != i ||
                        (AktuellerPunkt - MENU_TASTEN_NUM_NON_CONTROLS == i && !control_reassignment_occuring))
                {
                    // Nicht definiert?
                    if ((pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD && 
                                pCurrentPlayer->AktionKeyboard[i] == -1) ||
                        (pCurrentPlayer->ControlType == CONTROLTYPE_JOY && 
                         (!on_move_line && !on_look_line &&
                             !(on_jump_line && pCurrentPlayer->JoystickMode == JOYMODE_JOYSTICK)) &&
                                pCurrentPlayer->AktionJoystick[i] == -1)) 
                    {
                            // Action is not defined
                            pDefaultFont->DrawText (col1_off_x + j * col2_off_x, controls_off_y + i * line_spacing,
                                TextArray[TEXT_NICHT_DEFINIERT], col);
                    } else {
                        if (pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD) {
                            // Keyboard key
                            if (pCurrentPlayer->AktionKeyboard[i] != -1)
                                pDefaultFont->DrawText (col1_off_x + j * col2_off_x, controls_off_y + i * line_spacing,
                                        GetKeyName (pCurrentPlayer->AktionKeyboard[i]), col);
                        } else {
                            // Joy axis/hat/button
                            char Buf[80];
                            if (on_move_line) {
                                // The three movement assignments (Walk Left, Walk Right, Crouch) can only be 
                                //  assigned to DPAD or X/Y-analog-axis and are a special case:
                                if (pCurrentPlayer->Walk_UseAxxis) {
                                    // Movement is assigned to analog stick
                                    strcpy_s(Buf, TextArray[TEXT_JOY_ACHSE]);
                                } else {
                                    // Movement is assigned to D-PAD HAT
                                    strcpy_s(Buf, TextArray[TEXT_JOY_COOLIE]);
                                }
                            } else if (on_look_line) {
                                // Similarly, the two look assignments can only be assigned to DPAD or X/Y-analog-axis
                                if (pCurrentPlayer->Look_UseAxxis) {
                                    // Look up/down is assigned to analog stick
                                    strcpy_s(Buf, TextArray[TEXT_JOY_ACHSE]);
                                } else {
                                    // Look up/down is assigned to DPAD HAT
                                    strcpy_s(Buf, TextArray[TEXT_JOY_COOLIE]);
                                }
                            } else if (on_jump_line && pCurrentPlayer->JoystickMode == JOYMODE_JOYSTICK) {
                                // When in joystick mode, jump is handled by movement (DPAD or analog axis)
                                if (pCurrentPlayer->Walk_UseAxxis) {
                                    // Jump/Movement is assigned to analog stick
                                    strcpy_s(Buf, TextArray[TEXT_JOY_ACHSE]);
                                } else {
                                    // Jump/Movement is assigned to DPAD HAT
                                    strcpy_s(Buf, TextArray[TEXT_JOY_COOLIE]);
                                }
                            } else {
                                //Joy button
                                sprintf_s (Buf, "%s %s", TextArray[TEXT_BUTTON], 
                                        DirectInput.MapButtonToString(pCurrentPlayer->JoystickIndex, 
                                            pCurrentPlayer->AktionJoystick[i]));
                            }

                            pDefaultFont->DrawText (col1_off_x + j * col2_off_x, controls_off_y + i * line_spacing, 
                                    Buf, col);
                        }
                    }
                } else
                {
                    char Buf[80];
                    // Redefinition prompt
                    if (scale_factor <= 1) {
                        if (pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD)
                            // Ask for new key
                            strcpy_s(Buf, TextArray[TEXT_TASTEN_NEU_T]);
                        else
                            // Ask for new button
                            strcpy_s(Buf, TextArray[TEXT_TASTEN_NEU_B]);
                    } else {
                        // When using scaled fonts (low-res device), there's not enough room for the text prompt
                        strcpy_s(Buf, "???");
                    }

                    pDefaultFont->DrawText(col1_off_x + j * col2_off_x, controls_off_y + i * line_spacing, 
                            Buf, col);
                }
            }
        }
    }
    break; // MENUZUSTAND_TASTEN

    case MENUPUNKT_HIGHSCORES :
    {
        char Buffer[100];		// F�r itoa

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

    }
    break; // HIGHSCORE

    case MENUZUSTAND_ENTERNAME :
    {
        char Buffer[100];		// F�r itoa

        pMenuFont->DrawTextCenterAlign(320, ypos + 90, TextArray[TEXT_WAHNSINN], D3DCOLOR_RGBA(255, 255, 255, 255), 2);

        strcpy_s(Buffer, strlen(TextArray[TEXT_NEUE_HIGHSCORE]) + 1, TextArray[TEXT_NEUE_HIGHSCORE]);
        pMenuFont->DrawTextCenterAlign(320, ypos + 180, TextArray[TEXT_NAMEN_EINGEBEN], D3DCOLOR_RGBA(255, 255, 255, 255), 2);

        sprintf_s(Buffer, "%s %d", Buffer, NewRank+1);

        pMenuFont->DrawTextCenterAlign(320, ypos + 150, Buffer, D3DCOLOR_RGBA(255, 255, 255, 255), 2);
    }
    break; // ENTERNAME

    case MENUPUNKT_CREDITS :
    {
        //DKS - There are now two credits text arrays, one original, one meant for low-res display (LowResCredits[])
        //      Altered this code to allow for displaying on low-res devices with new font-scaling:
        int i = 0;

        float scale_factor = pDefaultFont->GetScaleFactor();
        int num_lines = 40 / pDefaultFont->GetScaleFactor();
        int yoff_inc = 12 * pDefaultFont->GetScaleFactor();
        const char **credits_displayed = Credits;

        if (CommandLineParams.LowRes) {
            credits_displayed = LowResCredits;  // Use the low-res text instead
        }

        for (i=0; i<num_lines; i++)
        {
            D3DCOLOR Color;

            int alpha = int(i*yoff_inc-CreditsPosition);

            if (alpha > 360)
            {
                alpha = 255 - (alpha - 360) * 4;

                if (alpha < 0)
                    alpha = 0;
            }
            else if (alpha < 150)
            {
                alpha = 255 - (150 - alpha) * 4;

                if (alpha < 0)
                    alpha = 0;
            }

            else
                alpha = 255;

            Color = D3DCOLOR_RGBA(255, 255, 255, alpha);

            pDefaultFont->DrawTextCenterAlign(320.0f,
                                              float(int(i*yoff_inc-CreditsPosition)),
                                              credits_displayed[CreditsOffset+i], Color, 0);
        }

        // Credits scrollen
        CreditsCounter -= (0.7f * scale_factor) SYNC;			// Credits-Delay veringern

        // Bei Taste nach unten schneller scrollen
        //
        if (KeyDown(DIK_DOWN) ||
                KeyDown(DIK_NUMPAD2))
            CreditsCounter -= (2.0f * scale_factor) SYNC;

        // Bei Taste nach oben langsamer werden
        //
        if (KeyDown(DIK_UP) ||
                KeyDown(DIK_NUMPAD8))
            CreditsCounter = (0.5f * scale_factor);

        while (CreditsCounter < 0.0f)				// Ein Pixel weiterscrollen ?
        {
            CreditsCounter += (0.5f * scale_factor);				// Dann neues Delay setzen
            CreditsPosition += 1.0f * scale_factor;					// und Pixel weiterscrollen
            if (CreditsPosition >= yoff_inc)			// 12 Pixelgrenze �berschritten ?
            {
                CreditsPosition = 0;			// Dann wieder zur�cksetzen
                CreditsOffset++;				// Und im Char-Array eine zeile weiter

                // Credits zuende ?
                // Dann von vorne beginnen
                //
                if (strcmp ("#eoc", credits_displayed[CreditsOffset+i]) == 0)
                    CreditsOffset = 0;
            }
        }
    }
    break; // CREDITS

    case MENUZUSTAND_NEWGAME :
    {
        pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET, TextArray [TEXT_MENUE_SPIEL_STARTEN], menucolor, 2);

        for (int i=0; i<3; i++)
            if (AktuellerPunkt != i)
                pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + (i+2)*35, TextArray [TEXT_MENUE_TUTORIAL_SPIELEN+i], menucolor2, 2);

        pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + (AktuellerPunkt+2)*35, TextArray [TEXT_MENUE_TUTORIAL_SPIELEN+AktuellerPunkt], menucolor, 2);

    }
    break;

    case MENUZUSTAND_PLAYERCOUNT :
    {
        pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET, TextArray [TEXT_MENUE_SPIEL_STARTEN], menucolor, 2);

        for (int i=0; i<2; i++)
            if (AktuellerPunkt != i)
                pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + (i+2)*35, TextArray [TEXT_ONE_PLAYER+i], menucolor2, 2);

        pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + (AktuellerPunkt+2)*35, TextArray [TEXT_ONE_PLAYER+AktuellerPunkt], menucolor, 2);

    }
    break;

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

    }
    break;

    case MENUZUSTAND_LOADGAME :
    {
        // "Continue saved game"
        if (pDefaultFont->GetScaleFactor() <= 1) {
            pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET, 
                    TextArray [TEXT_MENUE_ALTES_SPIEL_FORTSETZEN], menucolor, 2);
        } else {
            //DKS - When using scaled fonts, display the text higher and using the small default font
            pDefaultFont->DrawTextCenterAlign(320, ypos + 20,
                    TextArray [TEXT_MENUE_ALTES_SPIEL_FORTSETZEN], menucolor, 2);
        }

        ShowSavegames(AktuellerPunkt);
    }
    break; // Load Game

    case MENUZUSTAND_SAVEGAME :
    {
        //DKS - Altered to allow scaled fonts on low-resolution devices
        const int scale_factor = pDefaultFont->GetScaleFactor();
        int line_off_y = 15;
        int continue_text_off_y = ypos + 150;

        if (scale_factor > 1) {
            continue_text_off_y = ypos + 50 + 20;
            line_off_y = 24;
        }

        ShowSavegames(AktuellerPunkt);

        // "Save Game"
        if (scale_factor <= 1) {
            pMenuFont->DrawTextCenterAlign(320.0f, ypos + OFFSET, TextArray[TEXT_MENUE_SPIEL_SPEICHERN], 0xFFFFFFFF, 2);
        } else {
            // Use the smaller default font since this we're using scaled fonts and need it to fit:
            pDefaultFont->DrawTextCenterAlign(320.0f, ypos + 20, TextArray[TEXT_MENUE_SPIEL_SPEICHERN], 0xFFFFFFFF, 2);
        }

        // Aktuelle gew�hltes Savegame heller anzeigen
        // "Continue"
        if (scale_factor <= 1) {
            pMenuFont->DrawTextCenterAlign(320, continue_text_off_y + (MAX_SAVEGAMES+1)*line_off_y, 
                    TextArray[TEXT_WEITER], 0x80FFFFFF, 2);
            if (AktuellerPunkt == MAX_SAVEGAMES)
                pMenuFont->DrawTextCenterAlign(320, continue_text_off_y + (MAX_SAVEGAMES+1)*line_off_y, 
                        TextArray[TEXT_WEITER], 0xFFFFFFFF, 2);
        } else {
            // If using scaled fonts, use the smaller default font:
            pDefaultFont->DrawTextCenterAlign(320, continue_text_off_y + (MAX_SAVEGAMES+1)*line_off_y, 
                    TextArray[TEXT_WEITER], 0x80FFFFFF, 2);
            if (AktuellerPunkt == MAX_SAVEGAMES)
                pDefaultFont->DrawTextCenterAlign(320, continue_text_off_y + (MAX_SAVEGAMES+1)*line_off_y, 
                        TextArray[TEXT_WEITER], 0xFFFFFFFF, 2);
        }
    }
    break; // Save Game


    default :
        break;
    }

    DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// Alles machen, was da Menu betrifft, sprich, werte �ndern und dann anzeigen
// --------------------------------------------------------------------------------------

void MenuClass::DoMenu(void)
{
    //DKS - Modified extensively while adding joystick support and fixing navigation backwards to previous menus
    bool joy_up         = false;
    bool joy_down       = false;
    bool joy_left       = false;
    bool joy_right      = false;
    bool joy_enter      = false;
    bool joy_escape     = false;
    bool joy_delete     = false;
//    int joy_idx = pPlayer[0]->JoystickIndex;
    static float input_counter = 0.0f;
    const float input_delay = 40.0f;     // Only accept joy input once every time counter reaches this value
    input_counter += 30.0f SYNC;
    if (input_counter > input_delay) {
        input_counter = input_delay;
    }

    // Einheitsmatrix setzen, damit das Menu richtig angezeigt wird
    //
    D3DXMATRIX matView;
    D3DXMatrixIdentity	 (&matView);
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTransform(D3DTS_VIEW, &matView);
#elif defined(PLATFORM_SDL)
    g_matView = matView;
#endif

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

    if (pConsole->Showing)
        return;

//----- Tasten abfragen

    bool selected = false;
    bool anybutton = false;

    DirectInput.UpdateJoysticks();

    if (DirectInput.AnyButtonDown())
        anybutton = true;

    if (input_counter >= input_delay && JoystickFound == true) {
        // Normally, for menu input we only check Player 1's joystick.
        int num_joys_to_check = 1;
        int joy_idx = pPlayer[0]->JoystickIndex;
        if (joy_idx < 0 || joy_idx >= DirectInput.JoysticksFound) {
            joy_idx = 0;
        }
#if defined(GCW)
        // However, on GCW Zero the internal controls are their own joystick
        // and we must always be sure to check them.
        if (pPlayer[0]->JoystickIndex != DirectInput.GetInternalJoystickIndex()) {
            num_joys_to_check++;
        }
#endif // GCW

        for (int joy_ctr=0; joy_ctr<num_joys_to_check; joy_ctr++) {
#if defined(GCW)
            if (joy_ctr > 0) {
                // After first checking Player 1, check the internal GCW Zero controls:
                joy_idx = DirectInput.GetInternalJoystickIndex();
            }
#endif // GCW

            if (DirectInput.Joysticks[joy_idx].JoystickPOV != -1) {
                // HAT switch is pressed
                if        (DirectInput.Joysticks[joy_idx].JoystickPOV >= 4500 * 1 &&
                        DirectInput.Joysticks[joy_idx].JoystickPOV <= 4500 * 3) {
                    joy_right = true;
                } else if (DirectInput.Joysticks[joy_idx].JoystickPOV >= 4500 * 5 &&
                        DirectInput.Joysticks[joy_idx].JoystickPOV <= 4500 * 7) {
                    joy_left = true;
                } else if (DirectInput.Joysticks[joy_idx].JoystickPOV >  4500 * 3 &&
                        DirectInput.Joysticks[joy_idx].JoystickPOV <  4500 * 5) {
                    joy_down = true;
                } else if ((DirectInput.Joysticks[joy_idx].JoystickPOV >  4500 * 7 && 
                            DirectInput.Joysticks[joy_idx].JoystickPOV <= 4500 * 8) ||
                        (DirectInput.Joysticks[joy_idx].JoystickPOV >= 0        &&
                         DirectInput.Joysticks[joy_idx].JoystickPOV < 4500 * 1)) {
                    joy_up = true;
                }
            } else {
#if !defined(GCW)   // On GCW Zero, only accept directional input for the menu from the DPAD
                if (DirectInput.Joysticks[joy_idx].JoystickX >  pPlayer[0]->JoystickSchwelle) {
                    joy_right = true;
                } else if (DirectInput.Joysticks[joy_idx].JoystickX < -pPlayer[0]->JoystickSchwelle) {
                    joy_left  = true;
                } else if (DirectInput.Joysticks[joy_idx].JoystickY >  pPlayer[0]->JoystickSchwelle) {
                    joy_down  = true;
                } else if (DirectInput.Joysticks[joy_idx].JoystickY < -pPlayer[0]->JoystickSchwelle) {
                    joy_up  = true;
                }
#endif //GCW
            }

            joy_enter  = joy_enter  || DirectInput.Joysticks[joy_idx].ButtonEnterPressed();
            joy_escape = joy_escape || DirectInput.Joysticks[joy_idx].ButtonEscapePressed();
            joy_delete = joy_delete || DirectInput.Joysticks[joy_idx].ButtonDeletePressed();
        }
    }

    if ((KeyDown(DIK_SPACE)  || KeyDown(DIK_RETURN) || joy_enter) && 
            AuswahlPossible && !control_reassignment_occuring) {
        input_counter = 0.0f;
        selected = true;
    }

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

    //DKS - TODO: Figure out if we can get rid of JoyOK, it was here originally and I don't like or want it
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

    if (KeyOK == true && JoyOK == true)
        AuswahlPossible = true;

    if ((ShowLanguageInfoCounter > 256.0f && ShowLanguageInfoCounter < 1600.0f) && 
            (KeyDown(DIK_ESCAPE) || KeyDown(DIK_RETURN) || joy_escape || joy_enter)) {
        ShowLanguageInfoCounter = 256.0f;
        pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);
        AuswahlPossible = false;
        input_counter = 0.0f;
    }

    //LanguageFile Info Fenster Counter runterz�hlen und Fenster ggf anzeigen
    //
    if (ShowLanguageInfoCounter > 0.0f) {
        ShowLanguageInfoCounter -= 50.0f SYNC;
        ShowLanguageInfo();
        return; // Do not accept menu input until language counter reaches 0
    }

    if (AuswahlPossible && !control_reassignment_occuring &&
            (KeyDown(DIK_NUMPAD8) || KeyDown(DIK_UP) || joy_up))
    {
        input_counter = 0.0f;
        AuswahlPossible = false;
        AktuellerPunkt--;
        pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);

        // Spieler will auf "spiel fortsetzen" wechseln, aber es l�uft noch kein spiel
        //
        if (Stage   == -1				     &&
                AktuellerZustand == MENUZUSTAND_MAINMENU &&
                AktuellerPunkt == 1)
        {
            AktuellerPunkt = 0;
        }

        // Checken, ob bei der Tastenwahl ein leeres Feld �bersprungen werden muss
        //
        if (AktuellerZustand == MENUZUSTAND_TASTEN)
        {
            PlayerClass* pCurrentPlayer;

            if (CurrentPlayer == 0)
                pCurrentPlayer = pPlayer[0];
            else
                pCurrentPlayer = pPlayer[1];

            //DKS - Altered to match redesigned controls submenu:
            if ((AktuellerPunkt == MENU_TASTEN_SENSITIVITY_LINE || AktuellerPunkt == MENU_TASTEN_MODE_LINE) &&
                    pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD)
            {
                AktuellerPunkt = MENU_TASTEN_TYPE_LINE;
            } 

            if (AktuellerPunkt < 0)
                AktuellerPunkt = MENU_TASTEN_NUM_LINES-1;
        }
    }

    if (AuswahlPossible && !control_reassignment_occuring &&
            (KeyDown(DIK_NUMPAD2) || KeyDown(DIK_DOWN) || joy_down))
    {
        input_counter = 0.0f;
        AuswahlPossible = false;
        AktuellerPunkt++;
        pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);

        // Spieler will auf "spiel fortsetzen" wechseln, aber es l�uft noch kein spiel
        //
        if (Stage   == -1				     &&
                AktuellerZustand == MENUZUSTAND_MAINMENU &&
                AktuellerPunkt == 1)
        {
            AktuellerPunkt = 2;
        }

        // Checken, ob bei der Tastenwahl ein leeres Feld �bersprungen werden muss
        //
        if (AktuellerZustand == MENUZUSTAND_TASTEN)
        {
            PlayerClass* pCurrentPlayer;

            if (CurrentPlayer == 0)
                pCurrentPlayer = pPlayer[0];
            else
                pCurrentPlayer = pPlayer[1];

            //DKS - Altered to match redesigned controls submenu:
            if ((AktuellerPunkt == MENU_TASTEN_MODE_LINE || AktuellerPunkt == MENU_TASTEN_SENSITIVITY_LINE) &&
                    pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD) {
                AktuellerPunkt = MENU_TASTEN_NUM_NON_CONTROLS;
            }

            if (AktuellerPunkt >= MENU_TASTEN_NUM_LINES)
                AktuellerPunkt = 0;
        }
    }

//----- Je nach Menu-Zustand die Aktionen auswerten

    switch (AktuellerZustand)
    {
    // Haupt-Menu
    case MENUZUSTAND_MAINMENU :
    {
        // zur�ck zum Game ?
        if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true && Stage != -1)
        {
            input_counter = 0.0f;
            AktuellerPunkt = MENUPUNKT_CONTINUEGAME;
            selected = true;

            while (DirectInput.AreAllKeysReleased() == false)
                DirectInput.UpdateTastatur();
        }

        if (AktuellerPunkt < MENUPUNKT_STARTGAME)
            AktuellerPunkt = MENUPUNKT_END;

        if (AktuellerPunkt > MENUPUNKT_END)
            AktuellerPunkt = MENUPUNKT_STARTGAME;

        // Menu-Punkt im Hauptmenu ausgew�hlt ?
        if (selected)
        {
            AuswahlPossible = false;

            // Je nach aktuellem Menupunkt in das entsprechende Untermenu wechseln
            if (AktuellerPunkt == MENUPUNKT_STARTGAME)
            {
                AktuellerZustand = MENUZUSTAND_NEWGAME;
                AktuellerPunkt = 1;
            }
            else if (AktuellerPunkt == MENUPUNKT_CONTINUEGAME && Stage > -1)
            {
                AktuellerPunkt = 0;
                pSoundManager->StopSong(MUSIC_MENU, false);
                SpielZustand = GAMELOOP;

                if (MUSIC_GetPaused(pSoundManager->its_Songs[MUSIC_BOSS]->SongData))
                    pSoundManager->PlaySong(MUSIC_BOSS, true);
                else if (pPlayer[0]->PunisherActive == true)
                    pSoundManager->PlaySong(MUSIC_PUNISHER, true);
                else if (pPlayer[0]->Riding())
                    pSoundManager->PlaySong(MUSIC_FLUGSACK, true);
                else
                    pSoundManager->PlaySong(MUSIC_STAGEMUSIC, true);

                pPlayer[0]->PowerLinePossible = false;
                pPlayer[0]->ShotDelay = 2.0f;
            }
            else if (AktuellerPunkt == MENUPUNKT_CREDITS)
            {
                pSoundManager->SetSongVolume(MUSIC_CREDITS, 100);
                pSoundManager->StopSong(MUSIC_MENU, false);
                pSoundManager->its_Songs[MUSIC_CREDITS]->FadingVolume = 0.0f;
                pSoundManager->PlaySong(MUSIC_CREDITS, false);
                CreditsCounter = 0.0f;
            }
            else if (AktuellerPunkt == MENUPUNKT_HIGHSCORES)
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
    }
    break;	// MAINMENU

    // Sound / Musik Lautst�rke w�hlen
    case MENUZUSTAND_VOLUMES :
    {
        if (AktuellerPunkt < 0)
            AktuellerPunkt = 4;

        if (AktuellerPunkt > 4)
            AktuellerPunkt = 0;

        // Volumes ge�ndert ?
        // Leiser
        bool pressed = false;

        if (AktuellerPunkt <= 1) {

            if (KeyDown(DIK_NUMPAD4) || KeyDown(DIK_LEFT) || joy_left) {
                pressed = true;
            } else {
                pressed = false;
            }
        }

        if (pressed)
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
        if (KeyDown(DIK_NUMPAD6) || KeyDown(DIK_RIGHT) || joy_right)
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

        // Punkt gew�hlt
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
                control_reassignment_occuring = false;
            }

            //DKS - This was already commented out:
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

            // Sprache w�hlen
            //
            if (AktuellerPunkt == 3)
            {
                AktuellerZustand = MENUZUSTAND_LANGUAGE;
                AktuellerPunkt   = 0;
            }

            // Sprache w�hlen
            //
            if (AktuellerPunkt == 4)
            {
                options_Detail++;

                if (options_Detail > DETAIL_MAXIMUM)
                    options_Detail = DETAIL_LOW;

                pPartikelSystem->SetParticleCount();
            }
        }

        // zur�ck zum Hauptmenu ?
        if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true)
        {
            input_counter = 0.0f;
            SaveConfig();							// Sound Config speichern
            AuswahlPossible = false;
            AktuellerZustand = MENUZUSTAND_MAINMENU;
            AktuellerPunkt	 = MENUPUNKT_VOLUMES;
            pSoundManager->PlayWave(100, 100, 11025, SOUND_CLICK);
        }
    }
    break;	// Sound / Musik Lautst�rke

    case MENUZUSTAND_LANGUAGE:
    {
        if (AktuellerPunkt < 0)
            AktuellerPunkt = LanguageFileCount;

        if (AktuellerPunkt > LanguageFileCount)
            AktuellerPunkt = 0;


        // Punkt gew�hlt
        //
        if (selected)
        {
            AuswahlPossible = false;

            // Language File gew�hlt ?
            if (AktuellerPunkt < LanguageFileCount)
            {
                strcpy_s (ActualLanguage, strlen(LanguageFiles [AktuellerPunkt]) + 1, LanguageFiles [AktuellerPunkt]);
                LoadLanguage (ActualLanguage);
                SaveConfig();
                ShowLanguageInfoCounter = 2000.0f;

                // Sprache feststellen und schauen, welche Grafik wir anzeigen m�ssen
                if (strcmp (TextArray[TEXT_BENUTZTE_GRAFIK], "0") == 0)
                    Sprachgrafik = 0;		// deutsch ?
                else
                    Sprachgrafik = 1;		// oder doch englisch ?

                AktuellerZustand = MENUZUSTAND_VOLUMES;
                AktuellerPunkt	 = MENUPUNKT_VOLUMES_LANGUAGE;
                pSoundManager->PlayWave(100, 100, 11025, SOUND_CLICK);
            }

            // oder nur zur�ck ?
            else
            {
                AktuellerZustand = MENUZUSTAND_VOLUMES;
                AktuellerPunkt	 = MENUPUNKT_VOLUMES_LANGUAGE;
                pSoundManager->PlayWave(100, 100, 11025, SOUND_CLICK);
            }
        }

        // zur�ck zum Hauptmenu ?
        if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true)
        {
            input_counter = 0.0f;
            SaveConfig();							// Sound Config speichern
            AuswahlPossible = false;
            AktuellerZustand = MENUZUSTAND_VOLUMES;
            AktuellerPunkt	 = MENUPUNKT_VOLUMES_LANGUAGE;
            pSoundManager->PlayWave(100, 100, 11025, SOUND_CLICK);
        }

    }
    break; // Languages

    //DKS - Significantly redesigned controls submenu:
    case MENUZUSTAND_TASTEN:
    {
        PlayerClass* pCurrentPlayer;

        if (CurrentPlayer == 0)
            pCurrentPlayer = pPlayer[0];
        else
            pCurrentPlayer = pPlayer[1];
        
        int action = AktuellerPunkt - MENU_TASTEN_NUM_NON_CONTROLS;
        bool on_move_line = (action==AKTION_LINKS || action==AKTION_RECHTS || action==AKTION_DUCKEN);
        bool on_look_line = (action==AKTION_OBEN || action==AKTION_UNTEN);
        bool on_jump_line = action==AKTION_JUMP;

        // If current player is using a joystick, check if the selected joystick is available
        // and, if not, make sure controls for it are never selected unless it is changed
        if (pCurrentPlayer->ControlType == CONTROLTYPE_JOY && AktuellerPunkt > MENU_TASTEN_TYPE_LINE) {
            int joy_idx = pCurrentPlayer->JoystickIndex;
            if (joy_idx < 0 || joy_idx >= DirectInput.JoysticksFound || !DirectInput.Joysticks[joy_idx].Active) {
                AktuellerPunkt = MENU_TASTEN_TYPE_LINE;
            }
        }

        if (AktuellerPunkt >= MENU_TASTEN_NUM_NON_CONTROLS && !control_reassignment_occuring &&
                (KeyDown(DIK_DELETE) || KeyDown(DIK_BACK) || joy_delete))
        {
            input_counter = 0.0f;
            if (pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD) {
                pCurrentPlayer->AktionKeyboard[action] = -1;
            } else if (!on_move_line && !on_look_line &&
                    !(on_jump_line && pCurrentPlayer->JoystickMode == JOYMODE_JOYSTICK)) {
                // Only non-look and non-movement actions are assignable to buttons on joysticks
                pCurrentPlayer->AktionJoystick[action] = -1;
            }
        }

        // neue Taste eingeben ?
        if (control_reassignment_occuring && !selected)
        {
            if (AuswahlPossible && AktuellerPunkt >= MENU_TASTEN_NUM_NON_CONTROLS)
            {
                if (pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD) 
                {
#if defined(GCW)
                    // Special abort case for GCW: If the user selects keyboard controls
                    // and tries to define a key without a keyboard actually attached,
                    // allow them to back out with the GCW button mapped to 'menu escape'

                    DirectInput.UpdateJoysticks();
                    if (DirectInput.Joysticks[DirectInput.GetInternalJoystickIndex()].ButtonEscapePressed()) {
                        control_reassignment_occuring = false;
                        AuswahlPossible = false;
                    }
#endif // GCW
                    // Neue Taste?
                    DirectInput.UpdateTastatur();

                    for (int i=1; i<DirectInput.NumberOfKeys; i++)		// Puffer durchgehen
                    {
                        //DKS - Added exclusions for TAB and ESCAPE, as they are hard-coded to 
                        //      be the console and main-menu activation keys, respectively.
                        //      Also added exclusions for keys 1,2 and 3 because they are
                        //      hard-coded in the game to be Weapon-selection keys.
                        if (KeyDown(i)  && i!=DIK_NUMLOCK && i!=DIK_CAPITAL && i!=DIK_SCROLL
                                && i!=DIK_TAB && i!=DIK_ESCAPE
                                && i!=DIK_1 && i!=DIK_2 && i!=DIK_3)					// ob eine Taste gedr�ckt wurde
                        {
#if defined(GCW)
                            //DKS - Added exclusion for the Hold Switch (Mapped to home key), as it is
                            //      hard-coded to be the console-activation key for GCW Zero.
                            if (i==DIK_HOME)
                                break;
#endif // GCW
                            pCurrentPlayer->AktionKeyboard[action] = i;
                            control_reassignment_occuring = false;
                            AuswahlPossible = false;

                            while (KeyDown(i))
                                DirectInput.UpdateTastatur();

                            input_counter = 0.0f;
                            break;
                        }
                    }
                } else {
                    // Neuer Button?
                    if (DirectInput.Joysticks[pCurrentPlayer->JoystickIndex].Active)
                    {
                        //DKS - TODO - this seems to be where more work is needed in accepting new button input
                        for (int i = 0; i < DirectInput.Joysticks[pCurrentPlayer->JoystickIndex].NumButtons; i++)
                        {
                            if (DirectInput.Joysticks[pCurrentPlayer->JoystickIndex].JoystickButtons[i])
                            {
#if defined(GCW)
                                //DKS - Added exclusion on GCW Zero for button 5, START, as it is hard-coded
                                //      to be the Exit-to-Main-Menu button and thus cannot be reassigned.
                                if (i == DirectInput.GetInternalJoystickMainMenuButton())
                                    break;
#endif //GCW
                                pCurrentPlayer->AktionJoystick[action] = i;
                                control_reassignment_occuring = false;
                                AuswahlPossible = false;

                                while (DirectInput.Joysticks[pCurrentPlayer->JoystickIndex].JoystickButtons[i])
                                    DirectInput.UpdateJoysticks();

                                input_counter = 0.0f;
                            }
                        }
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
                AktuellerPunkt = MENU_TASTEN_NUM_LINES-1;

            if (AktuellerPunkt > MENU_TASTEN_NUM_LINES-1)
                AktuellerPunkt = 0;

            // Spieler wechseln
            if (AuswahlPossible == true)
            {
                PlayerClass* pCurrentPlayer;

                if (CurrentPlayer == 0)
                    pCurrentPlayer = pPlayer[0];
                else
                    pCurrentPlayer = pPlayer[1];

                if (KeyDown(DIK_NUMPAD6) || KeyDown(DIK_RIGHT) || joy_right)
                {
                    input_counter = input_delay * 0.75f;     // Delay less than usual

                    if (AktuellerPunkt == MENU_TASTEN_SENSITIVITY_LINE)
                    {
                        pCurrentPlayer->JoystickSchwelle -= 100.0f SYNC;

                        if (pCurrentPlayer->JoystickSchwelle < 100.0f)
                            pCurrentPlayer->JoystickSchwelle = 100.0f;
                    } else if (AktuellerPunkt >= MENU_TASTEN_PLAYER_LINE)
                    {
                        AuswahlPossible = false;
                        CurrentPlayer = 1;
                        pCurrentPlayer = pPlayer[1];

                        if (pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD &&
                                (AktuellerPunkt == MENU_TASTEN_MODE_LINE    ||
                                 AktuellerPunkt == MENU_TASTEN_SENSITIVITY_LINE))
                            AktuellerPunkt = MENU_TASTEN_TYPE_LINE;
                    }
                }

                //DKS - TODO allow a second joystick to adjust this
                if (KeyDown(DIK_NUMPAD4) || KeyDown(DIK_LEFT) || joy_left)
                {
                    input_counter = input_delay * 0.75f;     // Delay less than usual

                    if (AktuellerPunkt == MENU_TASTEN_SENSITIVITY_LINE)
                    {
                        pCurrentPlayer->JoystickSchwelle += 100.0f SYNC;

                        if (pCurrentPlayer->JoystickSchwelle > 900.0f)
                            pCurrentPlayer->JoystickSchwelle = 900.0f;
                    } else if (AktuellerPunkt >= MENU_TASTEN_PLAYER_LINE)
                    {
                        AuswahlPossible = false;
                        CurrentPlayer = 0;
                        pCurrentPlayer = pPlayer[0];

                        if (pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD &&
                                (AktuellerPunkt == MENU_TASTEN_MODE_LINE  ||
                                 AktuellerPunkt == MENU_TASTEN_SENSITIVITY_LINE))
                            AktuellerPunkt = MENU_TASTEN_TYPE_LINE;
                    }
                }
            }

            // Punkt gew�hlt
            //
            if (selected)
            {
                AuswahlPossible = false;
                input_counter = 0.0f;

                // Taste �ndern?
                if (AktuellerPunkt >= MENU_TASTEN_NUM_NON_CONTROLS)
                {
                    if (pCurrentPlayer->ControlType == CONTROLTYPE_JOY && 
                            (on_move_line || on_look_line || 
                             (on_jump_line && pCurrentPlayer->JoystickMode == JOYMODE_JOYSTICK))) {
                        // If control mode is joystick/joypad, movement and look up/down can only be
                        //  assigned to either the DPAD HAT or the analog stick:
                        pCurrentPlayer->Walk_UseAxxis = !pCurrentPlayer->Walk_UseAxxis;
                        pCurrentPlayer->Look_UseAxxis = !pCurrentPlayer->Look_UseAxxis;
                        // Make sure the walk and look assignments remain different from one another
                        //  (normally this always be the case if defaults are being assigned correctly)
                        if (pCurrentPlayer->Walk_UseAxxis == pCurrentPlayer->Look_UseAxxis)
                            pCurrentPlayer->Walk_UseAxxis = !pCurrentPlayer->Look_UseAxxis;
                    } else {
                        control_reassignment_occuring = true;
                        // Do not proceed until all keys/buttons are released:
                        while (DirectInput.AnyKeyDown() || DirectInput.AnyButtonDown()) {
                            DirectInput.UpdateTastatur();
                            DirectInput.UpdateJoysticks();
                        }
                    }
                } else {
                    PlayerClass *pCurrentPlayer;

                    if (CurrentPlayer == 0)
                        pCurrentPlayer = pPlayer[0];
                    else
                        pCurrentPlayer = pPlayer[1];

                    if (AktuellerPunkt == MENU_TASTEN_DEFAULTS_LINE) {
                        // Load default controls for players 1/2
                       CreateDefaultControlsConfig(0);
                       CreateDefaultControlsConfig(1);
                    }

                    // Game-wide force-feedback enable/disable
                    if (AktuellerPunkt == MENU_TASTEN_FORCEFEEDBACK_LINE) {
                       UseForceFeedback = !UseForceFeedback;
                    } 

                    // Joymode �ndern?
                    if (AktuellerPunkt == MENU_TASTEN_MODE_LINE)
                    {
                        if (pCurrentPlayer->JoystickMode == JOYMODE_JOYSTICK)
                            pCurrentPlayer->JoystickMode = JOYMODE_JOYPAD;
                        else
                            pCurrentPlayer->JoystickMode = JOYMODE_JOYSTICK;
                    }

                    // Controller Type �ndern?
                    if (AktuellerPunkt == MENU_TASTEN_TYPE_LINE)
                    {
                        // Joytick dran?
                        if (JoystickFound == true)
                        {
                            // Von Keyboard auf Joystick wechseln?
                            if (pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD)
                            {
                                pCurrentPlayer->ControlType = CONTROLTYPE_JOY;
                                pCurrentPlayer->JoystickIndex = 0;
                            }

                            // Von Joystick auf Keyboard wechseln?
                            else
                            {
                                pCurrentPlayer->JoystickIndex++;

                                if (pCurrentPlayer->JoystickIndex >= DirectInput.JoysticksFound)
                                {
                                    pCurrentPlayer->ControlType = CONTROLTYPE_KEYBOARD;
                                    pCurrentPlayer->JoystickIndex = 0;
                                }
                            }
                        }
                    }
                }
            }

            // zur�ck zum Hauptmenu ?
            if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true)
            {
                input_counter = 0.0f;
                // Neue Tastennamen f�r die Tutorial-Texte �bernehmen
                InitReplacers();

                // Sound Config speichern
                SaveConfig();

                AuswahlPossible = false;
                AktuellerZustand = MENUZUSTAND_VOLUMES;
                AktuellerPunkt	 = MENUPUNKT_VOLUMES_TASTEN;
                pSoundManager->PlayWave(100, 100, 11025, SOUND_CLICK);
            }
        }
    }
    break; // Tasten

    // Die Credits und die Highscore
    case MENUPUNKT_CREDITS  :
    case MENUPUNKT_HIGHSCORES :
    {
        // zur�ck zum Hauptmenu ?
        if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true)
        {
            input_counter = 0.0f;
            
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
    }
    break; // Credits und Highscore

    case MENUZUSTAND_ENTERNAME :
    {
        //DKS - Modified extensively to allow joystick name entry
        static unsigned char Taste;
        static bool possible = false;

        int newname_len = strlen(NewName);
        int newname_maxlen = 16;

        
        if (joy_right) {
            input_counter = 0.0f;
            if (newname_len < newname_maxlen && newname_len > 0 && NewName[newname_len-1] != '_') {
                NewName[newname_len] = '_';  // Add prompt char
                NewName[newname_len+1] = '\0';
                newname_len++;
            }
        } else if (joy_left) {
            input_counter = 0.0f;
            if (newname_len > 1) {
                NewName[newname_len-2] = '_';
                NewName[newname_len-1] = '\0';
                newname_len--;
            }
        } else if (joy_up) {
            input_counter = 0.0f;
            if (newname_len > 0) {
                // Chars 97-122 are a..z, Chars 48-57 are 0..9, Space is 32
                if (NewName[newname_len-1] == 'Z') {
                    NewName[newname_len-1] = '0';    // Number '0'
                } else if (NewName[newname_len-1] == '9' || NewName[newname_len-1] == '_') {
                    NewName[newname_len-1] = 'A';
                } else {
                    NewName[newname_len-1]++;
                }
            }
        } else if (joy_down) {
            input_counter = 0.0f;
            if (newname_len > 0) {
                // Chars 97-122 are a..z, Chars 48-57 are 0..9, Space is 32
                if (NewName[newname_len-1] == 'A' || NewName[newname_len-1] == '_') {
                    NewName[newname_len-1] = '9';    
                } else if (NewName[newname_len-1] == '0') {
                    NewName[newname_len-1] = 'Z';
                } else {
                    NewName[newname_len-1]--;
                }
            }
        }

        if (!KeyDown(DIK_RETURN) && !joy_enter)
            AuswahlPossible = true;

        if ((KeyDown(DIK_RETURN) || joy_enter) && AuswahlPossible == true)
        {
            input_counter = 0.0f;
            //Strip trailing cursor char
            if (newname_len > 0 && NewName[newname_len-1] == '_') {
                NewName[newname_len-1] = '\0';
                newname_len--;
            }

            // Neuen Namen an die neue Position setzen
            strcpy_s(Highscores[NewRank].Name, strlen(NewName) + 1, NewName);

            Highscores[NewRank].Stage = NewStage;
            Highscores[NewRank].Skill = NewSkill;

            // Und zur�ck ins Hauptmenu und dort die Highscores anzeigen
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

            // Testen ob die gedr�ckte Taste loasgelassen wurde
            if (KeyDown(Taste) || KeyDown(DIK_BACK) || KeyDown(DIK_SPACE) 
                    || joy_left || joy_right || joy_up || joy_down)
                possible = false;
        }

        for (int i=0; i<256; i++)		// Puffer durchgehen
            if (KeyDown(i) && possible == true)
            {
                possible = false;

                if (KeyDown(DIK_BACK))
                {
                    //DKS - added in missing/disabled backspace support
                    if (newname_len > 1) {
                        pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);
                        NewName[newname_len-2] = '_';
                        NewName[newname_len-1] = '\0';
                        newname_len--;
                    }
                }
                else if (KeyDown(DIK_SPACE))
                {
                    //DKS - added in missing/disabled space support
                    // Only insert a space if the string is not empty
                    if (newname_len > 1 && NewName[newname_len-1] == '_' && newname_len < newname_maxlen) {
                        pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);
                        NewName[newname_len-1] = ' ';
                        NewName[newname_len]   = '_';
                        NewName[newname_len+1] = '\0';
                        newname_len++;
                    }
                       

                }
                else
                {
                    Taste = i;
                    if (strlen(GetKeyName(Taste)) == 1 && newname_len < newname_maxlen) {
                        char keyname[2];
                        strcpy_s(keyname, GetKeyName(Taste));
                        keyname[0] = toupper(keyname[0]); 
                        if ((keyname[0] >= 'A' && keyname[0] <= 'Z') ||
                            (keyname[0] >= '0' && keyname[0] <= '9'))
                        {
                            pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);
                            // NewName should already have a trailing underscore, indicating the cursor..
                            //  replace it with the new char and a new underscore
                            if (newname_len > 0) {
                                NewName[newname_len-1] = keyname[0];
                                NewName[newname_len]   = '_';
                                NewName[newname_len+1] = '\0';
                                newname_len++;
                            } else {
                                NewName[0] = keyname[0];
                                NewName[1] = '_';
                                NewName[2] = '\0';
                                newname_len = 2;
                            }
                        }
                    }
                }
            }

        // Aktuellen Namen azeigen
        char Buffer[20];
        sprintf_s(Buffer, "%s", NewName);
        pMenuFont->DrawTextCenterAlign(319, ypos + 230, Buffer, D3DCOLOR_RGBA(0, 0, 255, 255), 2);
        pMenuFont->DrawTextCenterAlign(321, ypos + 230, Buffer, D3DCOLOR_RGBA(0, 0, 255, 255), 2);
        pMenuFont->DrawTextCenterAlign(320, ypos + 229, Buffer, D3DCOLOR_RGBA(0, 0, 255, 255), 2);
        pMenuFont->DrawTextCenterAlign(320, ypos + 231, Buffer, D3DCOLOR_RGBA(0, 0, 255, 255), 2);
        pMenuFont->DrawTextCenterAlign(320, ypos + 230, Buffer, D3DCOLOR_RGBA(255, 255, 255, 255), 2);

    }
    break; // Namen eingeben

    // Neues Spiel starten / Savegame laden
    case MENUZUSTAND_NEWGAME :
    {
        if (AktuellerPunkt < 0)
            AktuellerPunkt = 2;

        if (AktuellerPunkt > 2)
            AktuellerPunkt = 0;

        // Zur�ck ins Haupt Menu ?
        if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true)
        {
            input_counter = 0.0f;
            AktuellerZustand = MENUZUSTAND_MAINMENU;
            AktuellerPunkt	 = MENUPUNKT_STARTGAME;
        }

        // Menu-Punkt ausgew�hlt ?
        if (selected)
        {
            AuswahlPossible = false;

            // Tutorial spielen?
            if (AktuellerPunkt == MENUPUNKT_NEWGAME_TUTORIAL)
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
            if (AktuellerPunkt == MENUPUNKT_NEWGAME_STARTNEWGAME)
            {
                AktuellerZustand = MENUZUSTAND_PLAYERCOUNT;
                AktuellerPunkt	 = 0;						// Auf Medium anfangen
            }

            // altes Spiel laden ?
            else if (AktuellerPunkt == MENUPUNKT_NEWGAME_LOADGAME)
            {
                AktuellerZustand = MENUZUSTAND_LOADGAME;
                AktuellerPunkt	 = 0;
                LoadSavegames();
            }

            pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);
        }
    }
    break;	// SPIEL VERLASSEN

    case MENUZUSTAND_PLAYERCOUNT :
    {
        if (AktuellerPunkt < 0)
            AktuellerPunkt = 1;

        if (AktuellerPunkt > 1)
            AktuellerPunkt = 0;

        // Zur�ck ins Haupt Menu ?
        if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true)
        {
            input_counter = 0.0f;
            AuswahlPossible = false;
            AktuellerZustand = MENUZUSTAND_NEWGAME;
            AktuellerPunkt	 = MENUPUNKT_NEWGAME_STARTNEWGAME;
        }

        // Menu-Punkt ausgew�hlt ?
        if (selected)
        {
            tempNUMPLAYERS = AktuellerPunkt + 1;
            AktuellerPunkt = 1;

            AktuellerZustand = MENUZUSTAND_SELECTSKILL;
            pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);

            AuswahlPossible = false;
        }
    }
    break;	// SPIEL VERLASSEN

    // Skill ausw�hlen
    case MENUZUSTAND_SELECTSKILL :
    {
        if (AktuellerPunkt < 0)
            AktuellerPunkt = 3;

        if (AktuellerPunkt > 3)
            AktuellerPunkt = 0;

        // Zur�ck ins New Game Menu ?
        if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true)
        {
            input_counter = 0.0f;
            AuswahlPossible = false;
            AktuellerZustand = MENUZUSTAND_PLAYERCOUNT;
            AktuellerPunkt	 = tempNUMPLAYERS - 1;
        }

        // Menu-Punkt ausgew�hlt ?
        if (selected)
        {
            NUMPLAYERS = tempNUMPLAYERS;
            AuswahlPossible = false;
            RunningTutorial = false;
            pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);

            // Game starten
            // mit gew�hltem Skill
            Skill = AktuellerPunkt;

            // Intro initialisieren
            pSoundManager->StopAllSongs(false);
            DisplayHintNr = -1;

            StartIntro();
        }
    }
    break;	// Select Skill

    // Savegame zum Laden w�hlen
    case MENUZUSTAND_LOADGAME :
    {
        if (AktuellerPunkt < 0)
            AktuellerPunkt = MAX_SAVEGAMES-1;

        if (AktuellerPunkt > MAX_SAVEGAMES-1)
            AktuellerPunkt = 0;

        // Zur�ck ins New Game Menu ?
        if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true)
        {
            input_counter = 0.0f;
            AktuellerZustand = MENUZUSTAND_NEWGAME;
            AktuellerPunkt	 = MENUPUNKT_NEWGAME_LOADGAME;
            AuswahlPossible  = false;
        }

        // Menu-Punkt ausgew�hlt ?
        // Dann pr�fen ob das aktuelle Savegame ein leerer Slot ist
        // und wenn das nicht der Fall ist, dann das Savegame laden und
        // das Spiel starten
        if (selected)
        {
            RunningTutorial = false;

            AuswahlPossible = false;
            pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);

            // Aktuell gew�hltes Savegame laden, wenn es existiert
            // bzw die Werte aus den schon geladenen Savegames �bernehmen
            if (Savegames[AktuellerPunkt].Stage >= 0)
            {
                InitNewGame();

                NUMPLAYERS = Savegames[AktuellerPunkt].Players;

                // Werte von Spieler auf das Savegame �bertragen
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
    }
    break;	// Load Game

    // Savegame zum Speichern w�hlen
    case MENUZUSTAND_SAVEGAME :
    {
        if (AktuellerPunkt < 0)
            AktuellerPunkt = MAX_SAVEGAMES;

        if (AktuellerPunkt > MAX_SAVEGAMES)
            AktuellerPunkt = 0;

        // Menu-Punkt ausgew�hlt ?
        // Dann das Spiel an dem aktuell gew�hlten Slot speichern
        // oder weiterspielen ohne zu speichern (je nach Auswahl)
        if (selected)
        {
            AuswahlPossible = false;
            pSoundManager->PlayWave(100, 128, 11025, SOUND_CLICK);

            // Bestimmtes Savegame ausgew�hlt
            // Dann hier speichern
            if (AktuellerPunkt < MAX_SAVEGAMES)
            {
                // Werte von Spieler auf das Savegame �bertragen
                // Name des Save games aus aktuellem Datum und aktueller Zeit erstelen

                char   timestr[20];

                time_t seconds= time(NULL);
                struct tm *ptm= localtime(&seconds);

                /*
                					sprintf_s(timestr,"%02i-%02i-%04i %02i:%02i",
                						   (int)ptm->tm_mday,
                						   (int)ptm->tm_mon+1,
                						   (int)ptm->tm_year+1900,

                						   (int)ptm->tm_hour,
                						   (int)ptm->tm_min);
                */
                strcpy_s( timestr, asctime(ptm) );

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

                // Pr�fsumme gegen Savegame-Manipulation errechnen

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
                char Name  [100];		// F�r die Dateinamen
                char Buffer[5];			// F�r _itoa
                FILE *Datei = NULL;		// Savegame Datei

                // Name des Savegames erstellen
                _itoa_s(AktuellerPunkt, Buffer, 10);

                // Versuchen, die Datei zu erstellen
                // nur weitermachen falls es keinen Fehler gibt
                snprintf( Name, sizeof(Name), "%s/Savegame%s.save", g_storage_ext, Buffer );
                fopen_s(&Datei, Name, "wb");

                // Fehler beim �ffnen ? Dann leeren Slot erzeugen
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
    }
    break;	// Save Game

    default :
        break;
    } // switch
}

// --------------------------------------------------------------------------------------
// Savegames laden
// --------------------------------------------------------------------------------------

void MenuClass::LoadSavegames(void)
{
    char Name  [100];		// F�r die Dateinamen
    char Buffer[5];			// F�r _itoa
    FILE *Datei;			// Savegame Datei

    // Versuchen, die einzelnen Savegames zu laden
    for (int i=0; i<MAX_SAVEGAMES; i++)
    {
        // Name des Savegames erstellen
        _itoa_s(i, Buffer, 10);
        snprintf( Name, sizeof(Name), "%s/Savegame%s.save", g_storage_ext, Buffer );

        // Versuchen, die Datei zu �ffnen
        // falls sie nicht existiert oder es eine Fehler gibt, ist der Slot noch leer
        fopen_s(&Datei, Name, "rb");

        Protokoll.WriteText(  false, "save games loaded\n" );

        // Fehler beim �ffnen ? Dann leeren Slot erzeugen
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
            // Pr�fsumme gegen Savegame-Manipulation errechnen
            // und nur wenn diese stimmt das Savegame �bernehmen
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

//DKS - Altered to allow scaled fonts on low resolution devices
void MenuClass::ShowSavegames(int Highlight)
{
    char buffer[100];
    D3DCOLOR col;
    const int scale_factor = pDefaultFont->GetScaleFactor();
    int line_off_y = 14;
    int title_bar_off_y = ypos + 120;
    int savegames_off_y = ypos + 150;
    int col1_off_x = xpos + 100;
    int col2_off_x = xpos + 150;
    int col3_off_x = xpos + 250;

    if (scale_factor > 1) {
        // Things get shifted higher and spaced farther when using a scaled font, so there's room for everything
        line_off_y = 24;
        title_bar_off_y = ypos + 50;
        savegames_off_y = title_bar_off_y + 30;
        col1_off_x = xpos/2 + 50;
        col2_off_x = xpos/2 + 150;
        col3_off_x = xpos/2 + 350;
    }

    pDefaultFont->DrawTextCenterAlign(col1_off_x, title_bar_off_y, TextArray[TEXT_SAVE_STAGE], 0xFFFFFFFF, 0);
    pDefaultFont->DrawTextCenterAlign(col2_off_x, title_bar_off_y, TextArray[TEXT_SAVE_SPIELER], 0xFFFFFFFF, 0);
    pDefaultFont->DrawTextCenterAlign(col3_off_x, title_bar_off_y, TextArray[TEXT_SAVE_DATUM], 0xFFFFFFFF, 0);


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
            pDefaultFont->DrawTextCenterAlign(col1_off_x, savegames_off_y + i*line_off_y, buffer, col, 0);
        }

        // Spieler anzeigen
        //
        if (Savegames[i].Stage >= 0)
        {
            _itoa_s(Savegames[i].Players, buffer, 10);
            pDefaultFont->DrawTextCenterAlign(col2_off_x, savegames_off_y + i*line_off_y, buffer, col, 0);
        }

        //DKS - This was duplicate code that I removed, it did nothing different than above.. obvious bug
        /*
        // Skill anzeigen
        //
        if (Savegames[i].Stage >= 0)
        {
            _itoa_s(Savegames[i].Players, buffer, 10);
            pDefaultFont->DrawTextCenterAlign(xpos + 150, ypos + 150 + i*14, buffer, col, 0);
        }
        */


        // Ist ein Savegame gefunden worden ?
        if (Savegames[i].Stage >= 0)
        {
            pDefaultFont->DrawTextCenterAlign(col3_off_x, savegames_off_y + i*line_off_y, Savegames[i].Name, col, 0);
        }

        // oder ist dort noch ein leerer Slot ?
        else
        {
            pDefaultFont->DrawTextCenterAlign(320, savegames_off_y + i*line_off_y, TextArray[TEXT_SAVE_LEER], col, 0);
        }
    }
}

// --------------------------------------------------------------------------------------
// Highscore-Liste laden
// --------------------------------------------------------------------------------------

void MenuClass::LoadHighscore(void)
{
    FILE *Datei;			// Savegame Datei
    char name[256];

    // Versuchen, die Highscore Datei zu �ffnen
    // falls sie nicht existiert oder es eine Fehler gibt, wird die Standard
    // Highscore gesetzt
    sprintf_s(name, "%s/Hurrican.hsl", g_storage_ext);
    fopen_s(&Datei, name, "rb");


    // Fehler beim �ffnen ? Dann standard Highscore setzen
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
        // und nur wenn diese stimmt die Highscore �bernehmen
        // ansonsten wieder standard Highscore setzen
        long Pruefsumme;

        for (int i=0; i<MAX_HIGHSCORES; i++)
        {
            Pruefsumme = 0;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            Highscores[i].Score=SWAP32(Highscores[i].Score);
            Highscores[i].Stage=SWAP32(Highscores[i].Stage);
            Highscores[i].Skill=SWAP32(Highscores[i].Skill);
            Highscores[i].Pruefsumme=SWAP32(Highscores[i].Pruefsumme);
#endif

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
    char name[256];

    // Highscore Datei �ffnen
    sprintf_s(name, "%s/Hurrican.hsl", g_storage_ext);
    fopen_s(&Datei, name, "wb");

    // Fehler beim �ffnen ? Dann standard Highscore setzen
    //
    if (Datei == NULL)
        ResetHighscore();

    else
    {
        // Ansonsten Scores speichern
        //
        for (int i=0; i<MAX_HIGHSCORES; i++)
        {
            // Pr�fsumme erstellen
            //
            Highscores[i].Pruefsumme = 0;

            for (unsigned int j=0; j<strlen(Highscores[i].Name); j++)
                Highscores[i].Pruefsumme += Highscores[i].Name[j];

            Highscores[i].Pruefsumme += strlen(Highscores[i].Name) +
                                        Highscores[i].Score +
                                        Highscores[i].Stage +
                                        Highscores[i].Skill;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            // SixK - SWAP TO LITTLE ENDIAN before saving
            Highscores[i].Score=SWAP32(Highscores[i].Score);
            Highscores[i].Stage=SWAP32(Highscores[i].Stage);
            Highscores[i].Skill=SWAP32(Highscores[i].Skill);
            Highscores[i].Pruefsumme=SWAP32(Highscores[i].Pruefsumme);
#endif

            // Und Eintrag speichern
            //
            fwrite(&Highscores[i], sizeof(Highscores[i]), 1, Datei);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            // SixK - SWAP TO BIG ENDIAN Again once datas have been written
            Highscores[i].Score=SWAP32(Highscores[i].Score);
            Highscores[i].Stage=SWAP32(Highscores[i].Stage);
            Highscores[i].Skill=SWAP32(Highscores[i].Skill);
            Highscores[i].Pruefsumme=SWAP32(Highscores[i].Pruefsumme);
#endif
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

//DKS - Fixed displaying language info with scaled fonts, made rectangle drawing smarter and sized dynamically
void MenuClass::ShowLanguageInfo (void)
{
    int a1, a2;

    if (ShowLanguageInfoCounter < 0.0f)
        return;

    a1 = int (ShowLanguageInfoCounter);

    if (a1 > 255)
        a1 = 255;

    a2 = int (a1 / 2);

    // Determine size and location of background rectangle:
    unsigned int longest_line = 0;
    for (int i = 0; i < 9; i++) {
        if (strlen(TextArray[i]) > longest_line) {
            longest_line = strlen(TextArray[i]);
        }
    }
    int xoff_inc = 8 * pDefaultFont->GetScaleFactor();
    int yoff_inc = 12 * pDefaultFont->GetScaleFactor();
    int border = 20 / pDefaultFont->GetScaleFactor();
    int rect_w = longest_line * xoff_inc + border*2;      
    int rect_h = 11 * yoff_inc + border*2;                 
    int rect_x = 320 - rect_w / 2;
    int rect_y = 240 - rect_h / 2;
    RenderRect (rect_x-2, rect_y-2, rect_w+4, rect_h+4, D3DCOLOR_RGBA(64, 128, 255, a2));
    RenderRect (rect_x-1, rect_y-1, rect_w+2, rect_h+2, D3DCOLOR_RGBA(0,   0,   64,   a2));
    RenderRect (rect_x,   rect_y,   rect_w,   rect_h,   D3DCOLOR_RGBA(0,   0,   64,   a2));
    pDefaultFont->DrawText (float(320 - pDefaultFont->StringLength (TextArray[TEXT_BENUTZTES_FILE]) / 2), 
                            float(rect_y+border), TextArray[TEXT_BENUTZTES_FILE], D3DCOLOR_RGBA(255, 255, 255, a1));

    for (int i = 0; i < 9; i++)
    {
        int  xoff = 320 - (strlen (TextArray[i])-1) * xoff_inc / 2;
        for (unsigned int j = 0; j < strlen (TextArray[i]); j++)
        {
            char c[2];
            c[0] = TextArray[i][j];
            c[1] = '\0';
            pDefaultFont->DrawText (float (xoff), float (rect_y+border+((2+i)*yoff_inc)), c, D3DCOLOR_RGBA(255, 255, 255, a1));
            xoff += xoff_inc;
        }
    }
} // ShowLanguageInfo

// --------------------------------------------------------------------------------------
// Pr�fen obs eine neue Highscore gibt
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
    //DKS - Initialize NewName with just the cursor char
    strcpy_s(NewName, "_");

    // Alle Highscores durchgehen und vergleichen
    //
    bool besser = false;

    for (int j=0; j<MAX_HIGHSCORES; j++)
        if (pPlayer[0]->Score >= Highscores[j].Score && besser == false)
        {
            besser = true;

            // Ab der Stelle wo ein Namen eingef�gt wird eine Zeile runterscrollen
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

            // Werte f�r den neuen Eintrag l�schen
            NewRank = j;
            //DKS - Initialize NewName with just the cursor char
            strcpy_s(NewName, "_");
            strcpy_s(Highscores[NewRank].Name, "");
            AktuellerZustand = MENUZUSTAND_ENTERNAME;
            break;
        }

    SpielZustand = MAINMENU;
}
