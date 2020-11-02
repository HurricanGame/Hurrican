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

#if !defined(__AROS__) && !defined(__MORPHOS__) && !defined(__amigaos4__)
#include <sys/timeb.h>
#endif
#include <stdio.h>
#include <time.h>
#include "Console.hpp"
#include "DX8Font.hpp"
#include "DX8Graphics.hpp"
#include "DX8Input.hpp"
#include "DX8Sound.hpp"
#include "Gameplay.hpp"
#include "GetKeyName.hpp"
#include "Logdatei.hpp"
#include "Main.hpp"
#include "Menu.hpp"
#include "Partikelsystem.hpp"
#include "Player.hpp"
#include "Timer.hpp"

// --------------------------------------------------------------------------------------
// Die Credits
// --------------------------------------------------------------------------------------

int CreditsCount;

// DKS - Made a version of the above credits text array meant for use on
//      low resolution displays that use text scaling. Long lines are split,
//      and there's less repeated blank lines.
int LowResCreditsCount;

#include "creditStrings.hpp"

int tempNUMPLAYERS;

// --------------------------------------------------------------------------------------
// Konstruktor : Menu Grafiken laden und Werte initialisieren
// --------------------------------------------------------------------------------------

MenuClass::MenuClass() {
    MenuNebel.LoadImage("hurrican_rund.png", 512, 512, 512, 512, 1, 1);
    MenuTitel.LoadImage("hurrican-logo.png", 400, 90, 400, 90, 1, 1);

    Skills.LoadImage("skills.png", 200, 200, 100, 100, 2, 2);

    MenuStar.LoadImage("star.png", 5, 5, 5, 5, 1, 1);
    MenuStar.SetRect(0, 0, 5, 5);

    // DKS - Made these members of MenuClass instead of globals in Main.cpp:
    LoadingScreen.LoadImage("loading.png", 360, 60, 360, 60, 1, 1);
    LoadingBar.LoadImage("loadingbar.png", 318, 19, 318, 19, 1, 1);
    LoadingProgress = 0;
    LoadingItemsToLoad = 1;
    LoadingItemsLoaded = 0;

    // Menu Werte initialisieren
    ScrollPos = 0.0f;

    xpos = (640 - 373) / 2;
    ypos = 80;

    Rotation = 0.0f;
    RotationDir = 1;

    AktuellerZustand = MENUZUSTAND_MAINMENU;
    AktuellerPunkt = MENUPUNKT_STARTGAME;

    // Sterne initialisieren
    for (int i = 0; i < MAX_STARS; i++) {
        Stars[i].Count = float(rand() % 628) / 100.0f;
        Stars[i].Abstand = float(rand() % 400 + 20);
        Stars[i].Ebene = rand() % 120 + 20;
    }

    // Farben für die Highscore erstellen, in der die Namen aufblinken
    int i = 0;
    HighscoreColors[i] = D3DCOLOR_RGBA(75, 75, 75, 255);
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
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 75, 75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 45, 45, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 15, 15, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 0, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 15, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 45, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 75, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 105, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 135, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 165, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 195, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 225, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 255, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(225, 255, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(195, 255, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(165, 255, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(135, 255, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(105, 255, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(75, 255, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(45, 255, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(15, 255, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 15, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 45, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 105, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 135, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 165, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 195, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 225, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(15, 255, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(45, 255, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(75, 255, 255, 255);
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
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 180, 75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 165, 45, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 165, 15, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 150, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 135, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 105, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 75, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 45, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 15, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 0, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 0, 15, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 0, 45, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 0, 75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 0, 105, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 0, 135, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 0, 165, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 0, 195, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 0, 225, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(255, 0, 254, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(225, 15, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(195, 45, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(165, 75, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(135, 105, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(105, 135, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(75, 165, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(45, 195, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(15, 225, 255, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 225, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 195, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 165, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 135, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 105, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 45, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 255, 15, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(0, 225, 0, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(15, 195, 15, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(30, 180, 30, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(45, 165, 45, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(60, 150, 60, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(75, 135, 75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(75, 120, 75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(75, 105, 75, 255);
    i++;
    HighscoreColors[i] = D3DCOLOR_RGBA(75, 90, 75, 255);
    i++;

    // Die ersten 20 nochmal ...

    HighscoreColors[i] = D3DCOLOR_RGBA(75, 75, 75, 255);
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

    BlinkOffset = 0;
    BlinkCounter = 0.75f;

    // Sprache feststellen und schauen, welche Grafik wir anzeigen müssen
    if (strcmp(TextArray[TEXT_BENUTZTE_GRAFIK], "0") == 0)
        Sprachgrafik = 0;  // deutsch ?
    else
        Sprachgrafik = 1;  // oder doch englisch ?

    // Highscore laden
    //
    LoadHighscore();

    ShowLanguageInfoCounter = 0.0f;
    control_reassignment_occuring = false;

    // creditscount rausfinden
    CreditsCount = 0;
    while (strcmp(Credits[CreditsCount], "#eoc") != 0)
        CreditsCount++;

    // DKS - Made a version of the above credits text array meant for use on
    //      low resolution displays that use text scaling. Long lines are split,
    //      and there's less repeated blank lines.
    LowResCreditsCount = 0;
    while (strcmp(LowResCredits[LowResCreditsCount], "#eoc") != 0)
        LowResCreditsCount++;

    // DKS - Initialize NewName with just the cursor char
    strcpy_s(NewName, "_");
}

// --------------------------------------------------------------------------------------
// Konstruktor : Menu Grafiken freigeben
// --------------------------------------------------------------------------------------

MenuClass::~MenuClass() {
    // Aktuelle Highscoreliste speichern
    //
    SaveHighscore();
}

// --------------------------------------------------------------------------------------
// Hintergrund der Menüs anzeigen
// wird auch für den Progressbar benutzt
// --------------------------------------------------------------------------------------

void MenuClass::ShowMenuBack() {
    MenuNebel.RenderSpriteScaledRotated(-80, -140, 800, 800, ScrollPos, 0xFFFFFFFF);

    DirectGraphics.SetAdditiveMode();
    DirectGraphics.SetFilterMode(true);

    MenuStar.SetRect(0, 0, 5, 5);

    // Sterne anzeigen
    for (int i = 0; i < MAX_STARS; i++)
        MenuStar.RenderSpriteRotated(320 - float(sin(Stars[i].Count) * Stars[i].Abstand),
                                     240 + float(cos(Stars[i].Count) * Stars[i].Abstand), Stars[i].Count,
                                     D3DCOLOR_RGBA(255, 255, 255, Stars[i].Ebene));

    DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// Menu anzeigen
// --------------------------------------------------------------------------------------

void MenuClass::ShowMenu() {
    D3DCOLOR menucolor;
    D3DCOLOR menucolor2;
    D3DCOLOR menucolor3;

    int alpha = int(Rotation);
    menucolor = D3DCOLOR_RGBA(255, 255, 255, alpha);
    menucolor2 = D3DCOLOR_RGBA(255, 255, 255, alpha / 2);
    menucolor3 = D3DCOLOR_RGBA(255, 255, 255, alpha / 5);

    // Hintergrund rotieren lassen
    ScrollPos += 0.1f SYNC;

    if (ScrollPos > 360.0f)
        ScrollPos -= 360.0f;

    // Total löschen
    DirectGraphics.ClearBackBuffer();

    ShowMenuBack();

    // DKS - Added lowres support, which has very limited room at bottom:
    if (CommandLineParams.LowRes) {
        // Under Low-res, don't display controls message at bottom and shift other text up a bit
        pDefaultFont->DrawText(10.0f, 458, "www.poke53280.de", menucolor, 0);
        pDefaultFont->DrawText(float(640 - pDefaultFont->StringLength("www.hurrican-game.de", 0) - 10), 458,
                               "www.hurrican-game.de", menucolor, 0);
#if defined(GCW)
        // Draw port credit text for Pickle & Senquack
        if (AktuellerZustand == MENUZUSTAND_MAINMENU)
            pDefaultFont->DrawTextCenterAlign(320.0f, 428, "OpenGL and GCW Zero port by Pickle & Senquack", menucolor,
                                              0);
#endif
    } else {
        if (AktuellerZustand != MENUPUNKT_CREDITS && AktuellerZustand != MENUZUSTAND_ENTERNAME)

            pDefaultFont->DrawTextCenterAlign(320.0f, 462, TextArray[TEXT_MENUE_ANLEITUNG], menucolor, 0);
        pDefaultFont->DrawText(10.0f, 462, "www.poke53280.de", menucolor, 0);
        pDefaultFont->DrawText(float(640 - pDefaultFont->StringLength("www.hurrican-game.de", 0) - 10), 462,
                               "www.hurrican-game.de", menucolor, 0);
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
    switch (AktuellerZustand) {
        case MENUZUSTAND_MAINMENU: {
            for (int i = 0; i < 6; i++)
                if (AktuellerPunkt != i) {
                    if (i == 1) {
                        if (Stage == -1)
                            pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + i * 35,
                                                           TextArray[TEXT_MENUE_SPIEL_STARTEN + i], menucolor3, 2);
                        else
                            pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + i * 35,
                                                           TextArray[TEXT_MENUE_SPIEL_STARTEN + i], menucolor2, 2);
                    } else
                        pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + i * 35,
                                                       TextArray[TEXT_MENUE_SPIEL_STARTEN + i], menucolor2, 2);
                }

            pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + AktuellerPunkt * 35,
                                           TextArray[TEXT_MENUE_SPIEL_STARTEN + AktuellerPunkt], menucolor, 2);

        } break;

        case MENUZUSTAND_STARTGAME: {
        } break;

        case MENUZUSTAND_VOLUMES: {
            const int OFFSET2 = 20;

            float d = static_cast<float>(pMenuFont->StringLength(TextArray[TEXT_MENUE_EINSTELLUNGEN], 2));
            pMenuFont->DrawText(320 - d / 2.0f, ypos + OFFSET - OFFSET2, TextArray[TEXT_MENUE_EINSTELLUNGEN], menucolor,
                                2);

            // Sound / Musik Lautstärke
            for (int i = 0; i < 2; i++) {
                float d2 = static_cast<float>(pMenuFont->StringLength(TextArray[TEXT_SOUND + i], 2));

                if (i == 0)
                    LoadingBar.SetRect(0, 0, int(SoundManager.g_sound_vol / 100.0f * 318.0f), 19);
                else
                    LoadingBar.SetRect(0, 0, int(SoundManager.g_music_vol / 100.0f * 318.0f), 19);

                if (i == AktuellerPunkt) {
                    pMenuFont->DrawText(xpos + OFFSET - 20 - d2, ypos + OFFSET - OFFSET2 + (i + 2) * 35,
                                        TextArray[TEXT_SOUND + i], menucolor, 2);
                    LoadingScreen.RenderSprite(220, ypos + OFFSET - OFFSET2 + (i + 2) * 35 - 18, menucolor);
                    LoadingBar.RenderSprite(241, ypos + OFFSET - OFFSET2 + (i + 2) * 35 + 2, menucolor);
                } else {
                    pMenuFont->DrawText(xpos + OFFSET - 20 - d2, ypos + OFFSET - OFFSET2 + (i + 2) * 35,
                                        TextArray[TEXT_SOUND + i], menucolor2, 2);
                    LoadingScreen.RenderSprite(220, ypos + OFFSET - OFFSET2 + (i + 2) * 35 - 18, menucolor2);
                    LoadingBar.RenderSprite(241, ypos + OFFSET - OFFSET2 + (i + 2) * 35 + 2, menucolor2);
                }
            }

            // DKS - Renamed:
            // SoundManager.ResetAllSongVolumes();
            SoundManager.ResetAllSoundVolumes();

            // Sonstige Menu-Punkte anzeigen
            for (int i = 2; i < 4; i++) {
                float d2 = static_cast<float>(pMenuFont->StringLength(TextArray[TEXT_SOUND + i], 2));

                // Schrift anzeigen
                //
                if (i == AktuellerPunkt)
                    pMenuFont->DrawText(320.0f - d2 / 2.0f, ypos + OFFSET - OFFSET2 + (i + 2) * 35,
                                        TextArray[TEXT_SOUND + i], menucolor, 2);
                else
                    pMenuFont->DrawText(320.0f - d2 / 2.0f, ypos + OFFSET - OFFSET2 + (i + 2) * 35,
                                        TextArray[TEXT_SOUND + i], menucolor2, 2);
            }

            // Detailstufe
            d = static_cast<float>(pMenuFont->StringLength(TextArray[TEXT_DETAIL_LOW + options_Detail], 2));
            if (4 == AktuellerPunkt)
                pMenuFont->DrawText(320.0f - d / 2.0f, ypos + OFFSET - OFFSET2 + (4 + 2) * 35,
                                    TextArray[TEXT_DETAIL_LOW + options_Detail], menucolor, 2);
            else
                pMenuFont->DrawText(320.0f - d / 2.0f, ypos + OFFSET - OFFSET2 + (4 + 2) * 35,
                                    TextArray[TEXT_DETAIL_LOW + options_Detail], menucolor2, 2);

        } break;  // MENUZUSTAND_VOLUMES

        case MENUZUSTAND_LANGUAGE: {
            // DKS - Menu now displays filenames without path or extensions and supports low-resolution
            int vertical_spacing = 16;
            if (CommandLineParams.LowRes) {
                vertical_spacing += 8;
            }

            float d = static_cast<float>(pMenuFont->StringLength(TextArray[TEXT_SPRACHE], 2));
            pMenuFont->DrawText(320 - d / 2.0f, ypos + OFFSET, TextArray[TEXT_SPRACHE], menucolor, 2);

            char lang_name[256] = "";
            for (std::size_t i = 0; i < LanguageFiles.size(); i++) {
                strcpy_s(lang_name, LanguageFiles[i].c_str());
                // Truncate the extension
                int trunc_loc = strlen(lang_name) - 4;
                lang_name[trunc_loc] = '\0';

                // Make first character upper-case
                lang_name[0] = toupper(lang_name[0]);

                pDefaultFont->DrawText(320 - pDefaultFont->StringLength(lang_name) / 2.0f,
                                       ypos + 120 + i * vertical_spacing, lang_name, 0x88FFFFFF);
                if (AktuellerPunkt == i)
                    pDefaultFont->DrawText(320 - pDefaultFont->StringLength(lang_name) / 2.0f,
                                           ypos + 120 + i * vertical_spacing, lang_name, 0x88FFFFFF);
            }

            pDefaultFont->DrawText(320 - pDefaultFont->StringLength(TextArray[TEXT_ZURUECK]) / 2.0f,
                                   ypos + 136 + vertical_spacing * LanguageFiles.size(), (TextArray[TEXT_ZURUECK]),
                                   0x88FFFFFF);
            if (AktuellerPunkt == LanguageFiles.size())
                pDefaultFont->DrawText(320 - pDefaultFont->StringLength(TextArray[TEXT_ZURUECK]) / 2.0f,
                                       ypos + 136 + vertical_spacing * LanguageFiles.size(), (TextArray[TEXT_ZURUECK]),
                                       0x88FFFFFF);

        } break;  // MENUZUSTAND_LANGUAGE

        case MENUZUSTAND_TASTEN: {
            const int scale_factor = pDefaultFont->GetScaleFactor();
            int off_x = xpos;
            int off_y = ypos;
            int col0_off_x = off_x + 30;
            int col1_off_x = off_x + 40;
            int col2_off_x = 230;
            int title_off_y = off_y + OFFSET - 60;  // Configure Controls title line

            int line_spacing = 16;         // Original line spacing for non-scaled fonts
            int line1_off_y = off_y + 70;  // Top line of menu (Force Feedback line)

            // Offsets for use with scaled-fonts on low-res devices:
            if (scale_factor > 1) {
                off_y = 5;
                title_off_y = off_y + 10;
                line_spacing = 24;  // Kinda tight using a scaled font, but it all needs to fit
                col0_off_x = 270;
                col1_off_x = col0_off_x + 10;
                col2_off_x = 180;
                line1_off_y = title_off_y;
            }

            int controls_off_y = line1_off_y + MENU_TASTEN_NUM_NON_CONTROLS * line_spacing;  // Controls lines Y-offset

            // "CONFIGURE CONTROLS" title - Do not draw when using scaled fonts, as there's no room
            if (scale_factor <= 1) {
                pMenuFont->DrawTextCenterAlign(320, title_off_y, TextArray[TEXT_STEUERUNG], menucolor, 2);
            }

            // Überschrift "Spieler1" "Spieler2"
            D3DCOLOR p1_col, p2_col;
            if (AktuellerPunkt >= MENU_TASTEN_PLAYER_LINE) {
                if (CurrentPlayer == 0) {
                    p1_col = 0xFFFFFFFF;
                    p2_col = 0x88FFFFFF;
                    RenderRect(col1_off_x - 4, line1_off_y + MENU_TASTEN_PLAYER_LINE * line_spacing - 2,
                               pDefaultFont->StringLength(TextArray[TEXT_PLAYER_ONE]) + 10, line_spacing, 0x30FFFFFF);
                } else {
                    p1_col = 0x88FFFFFF;
                    p2_col = 0xFFFFFFFF;
                    RenderRect(col1_off_x + col2_off_x - 4, line1_off_y + MENU_TASTEN_PLAYER_LINE * line_spacing - 2,
                               pDefaultFont->StringLength(TextArray[TEXT_PLAYER_TWO]) + 10, line_spacing, 0x30FFFFFF);
                }
            } else {
                p1_col = 0x88FFFFFF;
                p2_col = 0x88FFFFFF;
            }

            pDefaultFont->DrawText(col1_off_x, line1_off_y + MENU_TASTEN_PLAYER_LINE * line_spacing,
                                   TextArray[TEXT_PLAYER_ONE], p1_col);
            pDefaultFont->DrawText(col1_off_x + col2_off_x, line1_off_y + MENU_TASTEN_PLAYER_LINE * line_spacing,
                                   TextArray[TEXT_PLAYER_TWO], p2_col);

            // Force-feedback checkbox:
            {
                D3DCOLOR col;
                if (AktuellerPunkt == MENU_TASTEN_FORCEFEEDBACK_LINE)
                    col = 0xFFFFFFFF;
                else
                    col = 0x88FFFFFF;

                int bar_off_y = line1_off_y + MENU_TASTEN_FORCEFEEDBACK_LINE * line_spacing + 1;
                int bar_dim = 8;
                int border_w = 1;

                if (scale_factor > 1) {
                    bar_dim = 12;
                    border_w = 2;
                }

                RenderRect(col0_off_x - border_w - bar_dim, bar_off_y, bar_dim, border_w, col);
                RenderRect(col0_off_x - border_w - bar_dim, bar_off_y + bar_dim + border_w, bar_dim, border_w, col);
                RenderRect(col0_off_x - border_w * 2 - bar_dim, bar_off_y, border_w, bar_dim + border_w * 2, col);
                RenderRect(col0_off_x - border_w, bar_off_y, border_w, bar_dim + border_w * 2, col);

                if (UseForceFeedback)
                    RenderRect(col0_off_x - border_w - bar_dim, bar_off_y + border_w, bar_dim, bar_dim, col);

                pDefaultFont->DrawText(col1_off_x, line1_off_y + MENU_TASTEN_FORCEFEEDBACK_LINE * line_spacing,
                                       TextArray[TEXT_FORCEFEEDBACK], col);
            }

            // Defaults, type, mode, sensitivity lines
            if (AktuellerPunkt == MENU_TASTEN_DEFAULTS_LINE)
                pDefaultFont->DrawText(col1_off_x, line1_off_y + MENU_TASTEN_DEFAULTS_LINE * line_spacing,
                                       TextArray[TEXT_DEFAULTS], 0xFFFFFFFF);
            else
                pDefaultFont->DrawText(col1_off_x, line1_off_y + MENU_TASTEN_DEFAULTS_LINE * line_spacing,
                                       TextArray[TEXT_DEFAULTS], 0x88FFFFFF);

            if (AktuellerPunkt == MENU_TASTEN_TYPE_LINE)
                pDefaultFont->DrawTextRightAlign(col0_off_x, line1_off_y + MENU_TASTEN_TYPE_LINE * line_spacing,
                                                 TextArray[TEXT_TYP], 0xFFFFFFFF);
            else
                pDefaultFont->DrawTextRightAlign(col0_off_x, line1_off_y + MENU_TASTEN_TYPE_LINE * line_spacing,
                                                 TextArray[TEXT_TYP], 0x88FFFFFF);

            if (AktuellerPunkt == MENU_TASTEN_MODE_LINE)
                pDefaultFont->DrawTextRightAlign(col0_off_x, line1_off_y + MENU_TASTEN_MODE_LINE * line_spacing,
                                                 TextArray[TEXT_MODUS], 0xFFFFFFFF);
            else
                pDefaultFont->DrawTextRightAlign(col0_off_x, line1_off_y + MENU_TASTEN_MODE_LINE * line_spacing,
                                                 TextArray[TEXT_MODUS], 0x88FFFFFF);

            if (AktuellerPunkt == MENU_TASTEN_SENSITIVITY_LINE)
                pDefaultFont->DrawTextRightAlign(col0_off_x, line1_off_y + MENU_TASTEN_SENSITIVITY_LINE * line_spacing,
                                                 TextArray[TEXT_EMPFINDLICHKEIT], 0xFFFFFFFF);
            else
                pDefaultFont->DrawTextRightAlign(col0_off_x, line1_off_y + MENU_TASTEN_SENSITIVITY_LINE * line_spacing,
                                                 TextArray[TEXT_EMPFINDLICHKEIT], 0x88FFFFFF);

            for (int i = 0; i < MENU_TASTEN_NUM_CONTROLS; i++) {
                pDefaultFont->DrawTextRightAlign(col0_off_x, controls_off_y + i * line_spacing,
                                                 TextArray[TEXT_TASTEN_L + i], 0x88FFFFFF);

                if (AktuellerPunkt - MENU_TASTEN_NUM_NON_CONTROLS == i)
                    pDefaultFont->DrawTextRightAlign(col0_off_x, controls_off_y + i * line_spacing,
                                                     TextArray[TEXT_TASTEN_L + i], 0x88FFFFFF);
            }

            // Selection bar:
            RenderRect(10, line1_off_y - 2 + AktuellerPunkt * line_spacing, 620, line_spacing, 0x24FFFFFF);

            // Für beide Spieler den ganzen Klumbatsch anzeigen
            for (int j = 0; j < 2; j++) {
                D3DCOLOR col;
                PlayerClass *pCurrentPlayer;

                if (j == 0)
                    pCurrentPlayer = &Player[0];
                else
                    pCurrentPlayer = &Player[1];

                // Typ (Tastatur, Joystick (mit Name)
                if (CurrentPlayer == j && AktuellerPunkt == MENU_TASTEN_TYPE_LINE)
                    col = 0xFFFFFFFF;
                else
                    col = 0x88FFFFFF;

                if (pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD)
                    // Keyboard text
                    pDefaultFont->DrawText(col1_off_x + j * col2_off_x,
                                           line1_off_y + MENU_TASTEN_TYPE_LINE * line_spacing, TextArray[TEXT_KEYBOARD],
                                           col);
                else {
                    // Print joystick name, truncated to fit if necessary
                    char tmpbuf[120];
                    snprintf(tmpbuf, 120, "%d:%s", pCurrentPlayer->JoystickIndex,
                              DirectInput.Joysticks[pCurrentPlayer->JoystickIndex].JoystickName);
                    int cutoff = strlen(tmpbuf) - 1;
                    int max_w = col2_off_x - 10;
                    while (cutoff > 1 && pDefaultFont->StringLength(tmpbuf) > max_w) {
                        tmpbuf[cutoff] = '\0';
                        cutoff--;
                    }
                    pDefaultFont->DrawText(col1_off_x + j * col2_off_x,
                                           line1_off_y + MENU_TASTEN_TYPE_LINE * line_spacing, tmpbuf, col);
                }

                // Rechteck für Empfindlichkeit
                if (pCurrentPlayer->ControlType == CONTROLTYPE_JOY) {
                    if (CurrentPlayer == j && AktuellerPunkt == MENU_TASTEN_SENSITIVITY_LINE)
                        col = 0xFFFFFFFF;
                    else
                        col = 0x88FFFFFF;

                    int bar_off_y = line1_off_y + MENU_TASTEN_SENSITIVITY_LINE * line_spacing + 1;
                    int bar_height = 8;
                    int border_w = 1;

                    if (scale_factor > 1) {
                        bar_off_y += 2;
                        bar_height = 12;
                        border_w = 2;
                    }

                    RenderRect(col1_off_x + border_w + j * col2_off_x, bar_off_y, 100, border_w, col);
                    RenderRect(col1_off_x + border_w + j * col2_off_x, bar_off_y + bar_height + border_w, 100, border_w,
                               col);
                    RenderRect(col1_off_x + j * col2_off_x, bar_off_y, border_w, bar_height + border_w * 2, col);
                    RenderRect(col1_off_x + 100 + border_w + j * col2_off_x, bar_off_y, border_w,
                               bar_height + border_w * 2, col);

                    // Aktueller Balken
                    RenderRect(col1_off_x + j * col2_off_x + border_w, bar_off_y + border_w,
                               (1000.0f - (pCurrentPlayer->JoystickSchwelle + 100.0f)) / 8.0f, bar_height, col);

                    // Modus
                    if (CurrentPlayer == j && AktuellerPunkt == MENU_TASTEN_MODE_LINE)
                        col = 0xFFFFFFFF;
                    else
                        col = 0x88FFFFFF;

                    if (pCurrentPlayer->JoystickMode == JOYMODE_JOYSTICK)
                        pDefaultFont->DrawText(col1_off_x + j * col2_off_x,
                                               line1_off_y + MENU_TASTEN_MODE_LINE * line_spacing,
                                               TextArray[TEXT_JOYMODE_STICK], col);
                    else
                        pDefaultFont->DrawText(col1_off_x + j * col2_off_x,
                                               line1_off_y + MENU_TASTEN_MODE_LINE * line_spacing,
                                               TextArray[TEXT_JOYMODE_PAD], col);
                }

                for (int i = 0; i < MENU_TASTEN_NUM_CONTROLS; i++) {
                    bool on_move_line = (i == AKTION_LINKS || i == AKTION_RECHTS || i == AKTION_DUCKEN);
                    bool on_look_line = (i == AKTION_OBEN || i == AKTION_UNTEN);
                    bool on_jump_line = (i == AKTION_JUMP);
                    D3DCOLOR col2;
                    if (AktuellerPunkt - MENU_TASTEN_NUM_NON_CONTROLS == i && CurrentPlayer == j)
                        col2 = 0xFFFFFFFF;
                    else
                        col2 = 0x88FFFFFF;

                    if (CurrentPlayer != j || AktuellerPunkt - MENU_TASTEN_NUM_NON_CONTROLS != i ||
                        (AktuellerPunkt - MENU_TASTEN_NUM_NON_CONTROLS == i && !control_reassignment_occuring)) {
                        // Nicht definiert?
                        if ((pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD &&
                             pCurrentPlayer->AktionKeyboard[i] == -1) ||
                            (pCurrentPlayer->ControlType == CONTROLTYPE_JOY &&
                             (!on_move_line && !on_look_line &&
                              !(on_jump_line && pCurrentPlayer->JoystickMode == JOYMODE_JOYSTICK)) &&
                             pCurrentPlayer->AktionJoystick[i] == -1)) {
                            // Action is not defined
                            pDefaultFont->DrawText(col1_off_x + j * col2_off_x, controls_off_y + i * line_spacing,
                                                   TextArray[TEXT_NICHT_DEFINIERT], col2);
                        } else {
                            if (pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD) {
                                // Keyboard key
                                if (pCurrentPlayer->AktionKeyboard[i] != -1)
                                    pDefaultFont->DrawText(col1_off_x + j * col2_off_x,
                                                           controls_off_y + i * line_spacing,
                                                           GetKeyName(pCurrentPlayer->AktionKeyboard[i]), col2);
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
                                    // Similarly, the two look assignments can only be assigned to DPAD or
                                    // X/Y-analog-axis
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
                                    // Joy button
                                    snprintf(Buf, 80, "%s %s", TextArray[TEXT_BUTTON],
                                              DirectInput.MapButtonToString(pCurrentPlayer->JoystickIndex,
                                                                            pCurrentPlayer->AktionJoystick[i]));
                                }

                                pDefaultFont->DrawText(col1_off_x + j * col2_off_x, controls_off_y + i * line_spacing,
                                                       Buf, col2);
                            }
                        }
                    } else {
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

                        pDefaultFont->DrawText(col1_off_x + j * col2_off_x, controls_off_y + i * line_spacing, Buf,
                                               col2);
                    }
                }
            }
        } break;  // MENUZUSTAND_TASTEN

        case MENUPUNKT_HIGHSCORES: {
            float d = static_cast<float>(pMenuFont->StringLength(TextArray[TEXT_MENUE_HIGHSCORES], 2));
            pMenuFont->DrawText(320 - d / 2.0f, ypos, TextArray[TEXT_MENUE_HIGHSCORES], menucolor, 2);

            // pMenuFont->DrawText(xpos-120,  ypos+55, TextArray [TEXT_HIGHSCORE_PLATZ],  0xFFFFFFFF, 2);
            pMenuFont->DrawText(xpos - 120, ypos + 55, TextArray[TEXT_HIGHSCORE_NAME], 0xFFFFFFFF, 2);
            pMenuFont->DrawTextRightAlign(xpos + 270, ypos + 55, TextArray[TEXT_HIGHSCORE_PUNKTE], 0xFFFFFFFF, 2);
            pMenuFont->DrawTextCenterAlign(xpos + 350, ypos + 55, TextArray[TEXT_HIGHSCORE_STAGE], 0xFFFFFFFF, 2);
            pMenuFont->DrawText(xpos + 420, ypos + 55, TextArray[TEXT_HIGHSCORE_SKILL], 0xFFFFFFFF, 2);

            for (int i = 0; i < MAX_HIGHSCORES; i++) {
                D3DCOLOR Color;

                Color = HighscoreColors[i + BlinkOffset];

                //_itoa_s(i+1, Buffer, 10);
                // pMenuFont->DrawText(xpos-90, ypos+float(105+i*26), Buffer, Color, 2);

                pMenuFont->DrawText(xpos - 120, ypos + float(105 + i * 26), Highscores[i].Name, Color, 2);

                std::string strbuf = std::to_string(Highscores[i].Score);
                pMenuFont->DrawTextRightAlign(xpos + 270, ypos + float(105 + i * 26), strbuf.c_str(), Color, 2);

                strbuf = std::to_string(Highscores[i].Stage);
                pMenuFont->DrawTextCenterAlign(xpos + 350, ypos + float(105 + i * 26), strbuf.c_str(), Color, 2);

                Skills.RenderSpriteScaled(xpos + 422, ypos + float(105 + i * 26), 30, 30, Highscores[i].Skill,
                                          0xFFFFFFFF);
            }

            // Highscore-Farben blinken lassen
            BlinkCounter -= SpeedFaktor;
            if (BlinkCounter < 0.0f) {
                BlinkCounter = 0.75f;
                BlinkOffset++;
                if (BlinkOffset > MAX_COLORS - MAX_HIGHSCORES)
                    BlinkOffset = 0;
            }

        } break;  // HIGHSCORE

        case MENUZUSTAND_ENTERNAME: {
            char Buffer[100];  // Für itoa

            pMenuFont->DrawTextCenterAlign(320, ypos + 90, TextArray[TEXT_WAHNSINN], D3DCOLOR_RGBA(255, 255, 255, 255),
                                           2);

            strcpy_s(Buffer, strlen(TextArray[TEXT_NEUE_HIGHSCORE]) + 1, TextArray[TEXT_NEUE_HIGHSCORE]);
            pMenuFont->DrawTextCenterAlign(320, ypos + 180, TextArray[TEXT_NAMEN_EINGEBEN],
                                           D3DCOLOR_RGBA(255, 255, 255, 255), 2);

            snprintf(Buffer, 100, "%s %d", Buffer, NewRank + 1);

            pMenuFont->DrawTextCenterAlign(320, ypos + 150, Buffer, D3DCOLOR_RGBA(255, 255, 255, 255), 2);
        } break;  // ENTERNAME

        case MENUPUNKT_CREDITS: {
            // DKS - There are now two credits text arrays, one original, one meant for low-res display
            // (LowResCredits[])
            //      Altered this code to allow for displaying on low-res devices with new font-scaling:
            int i = 0;

            float scale_factor = pDefaultFont->GetScaleFactor();
            int num_lines = 40 / pDefaultFont->GetScaleFactor();
            int yoff_inc = 12 * pDefaultFont->GetScaleFactor();
            const char **credits_displayed = Credits;

            if (CommandLineParams.LowRes) {
                credits_displayed = LowResCredits;  // Use the low-res text instead
            }

            for (i = 0; i < num_lines; i++) {
                D3DCOLOR Color;

                int alpha2 = int(i * yoff_inc - CreditsPosition);

                if (alpha2 > 360) {
                    alpha2 = 255 - (alpha2 - 360) * 4;

                    if (alpha2 < 0)
                        alpha2 = 0;
                } else if (alpha2 < 150) {
                    alpha2 = 255 - (150 - alpha2) * 4;

                    if (alpha2 < 0)
                        alpha2 = 0;
                }

                else
                    alpha2 = 255;

                Color = D3DCOLOR_RGBA(255, 255, 255, alpha2);

                pDefaultFont->DrawTextCenterAlign(320.0f, float(int(i * yoff_inc - CreditsPosition)),
                                                  credits_displayed[CreditsOffset + i], Color, 0);
            }

            // Credits scrollen
            CreditsCounter -= (0.7f * scale_factor) SYNC;  // Credits-Delay veringern

            // Bei Taste nach unten schneller scrollen
            //
            if (KeyDown(DIK_DOWN) || KeyDown(DIK_NUMPAD2))
                CreditsCounter -= (2.0f * scale_factor) SYNC;

            // Bei Taste nach oben langsamer werden
            //
            if (KeyDown(DIK_UP) || KeyDown(DIK_NUMPAD8))
                CreditsCounter = (0.5f * scale_factor);

            while (CreditsCounter < 0.0f)  // Ein Pixel weiterscrollen ?
            {
                CreditsCounter += (0.5f * scale_factor);  // Dann neues Delay setzen
                CreditsPosition += 1.0f * scale_factor;   // und Pixel weiterscrollen
                if (CreditsPosition >= yoff_inc)          // 12 Pixelgrenze überschritten ?
                {
                    CreditsPosition = 0;  // Dann wieder zurücksetzen
                    CreditsOffset++;      // Und im Char-Array eine zeile weiter

                    // Credits zuende ?
                    // Dann von vorne beginnen
                    //
                    if (strcmp("#eoc", credits_displayed[CreditsOffset + i]) == 0)
                        CreditsOffset = 0;
                }
            }
        } break;  // CREDITS

        case MENUZUSTAND_NEWGAME: {
            pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET, TextArray[TEXT_MENUE_SPIEL_STARTEN], menucolor, 2);

            for (int i = 0; i < 3; i++)
                if (AktuellerPunkt != i)
                    pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + (i + 2) * 35,
                                                   TextArray[TEXT_MENUE_TUTORIAL_SPIELEN + i], menucolor2, 2);

            pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + (AktuellerPunkt + 2) * 35,
                                           TextArray[TEXT_MENUE_TUTORIAL_SPIELEN + AktuellerPunkt], menucolor, 2);

        } break;

        case MENUZUSTAND_PLAYERCOUNT: {
            pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET, TextArray[TEXT_MENUE_SPIEL_STARTEN], menucolor, 2);

            for (int i = 0; i < 2; i++)
                if (AktuellerPunkt != i)
                    pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + (i + 2) * 35, TextArray[TEXT_ONE_PLAYER + i],
                                                   menucolor2, 2);

            pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET + (AktuellerPunkt + 2) * 35,
                                           TextArray[TEXT_ONE_PLAYER + AktuellerPunkt], menucolor, 2);

        } break;

        case MENUZUSTAND_SELECTSKILL: {
            pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET, TextArray[TEXT_MENUE_SPIEL_STARTEN], menucolor, 2);

            for (int i = 0; i < 4; i++)
                if (AktuellerPunkt != i)
                    pMenuFont->DrawText(310, ypos + OFFSET + (i + 2) * 35, TextArray[TEXT_MENUE_LEICHT + i], menucolor2,
                                        2);

            pMenuFont->DrawText(310, ypos + OFFSET + (AktuellerPunkt + 2) * 35,
                                TextArray[TEXT_MENUE_LEICHT + AktuellerPunkt], menucolor, 2);

            // Skill anzeigen
            DirectGraphics.SetColorKeyMode();
            Skills.RenderSprite(190, ypos + 165, AktuellerPunkt, 0xFFFFFFFF);
            DirectGraphics.SetAdditiveMode();

        } break;

        case MENUZUSTAND_LOADGAME: {
            // "Continue saved game"
            if (pDefaultFont->GetScaleFactor() <= 1) {
                pMenuFont->DrawTextCenterAlign(320, ypos + OFFSET, TextArray[TEXT_MENUE_ALTES_SPIEL_FORTSETZEN],
                                               menucolor, 2);
            } else {
                // DKS - When using scaled fonts, display the text higher and using the small default font
                pDefaultFont->DrawTextCenterAlign(320, ypos + 20, TextArray[TEXT_MENUE_ALTES_SPIEL_FORTSETZEN],
                                                  menucolor, 2);
            }

            ShowSavegames(AktuellerPunkt);
        } break;  // Load Game

        case MENUZUSTAND_SAVEGAME: {
            // DKS - Altered to allow scaled fonts on low-resolution devices
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
                pMenuFont->DrawTextCenterAlign(320.0f, ypos + OFFSET, TextArray[TEXT_MENUE_SPIEL_SPEICHERN], 0xFFFFFFFF,
                                               2);
            } else {
                // Use the smaller default font since this we're using scaled fonts and need it to fit:
                pDefaultFont->DrawTextCenterAlign(320.0f, ypos + 20, TextArray[TEXT_MENUE_SPIEL_SPEICHERN], 0xFFFFFFFF,
                                                  2);
            }

            // Aktuelle gewähltes Savegame heller anzeigen
            // "Continue"
            if (scale_factor <= 1) {
                pMenuFont->DrawTextCenterAlign(320, continue_text_off_y + (MAX_SAVEGAMES + 1) * line_off_y,
                                               TextArray[TEXT_WEITER], 0x80FFFFFF, 2);
                if (AktuellerPunkt == MAX_SAVEGAMES)
                    pMenuFont->DrawTextCenterAlign(320, continue_text_off_y + (MAX_SAVEGAMES + 1) * line_off_y,
                                                   TextArray[TEXT_WEITER], 0xFFFFFFFF, 2);
            } else {
                // If using scaled fonts, use the smaller default font:
                pDefaultFont->DrawTextCenterAlign(320, continue_text_off_y + (MAX_SAVEGAMES + 1) * line_off_y,
                                                  TextArray[TEXT_WEITER], 0x80FFFFFF, 2);
                if (AktuellerPunkt == MAX_SAVEGAMES)
                    pDefaultFont->DrawTextCenterAlign(320, continue_text_off_y + (MAX_SAVEGAMES + 1) * line_off_y,
                                                      TextArray[TEXT_WEITER], 0xFFFFFFFF, 2);
            }
        } break;  // Save Game

        default:
            break;
    }

    DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// Alles machen, was da Menu betrifft, sprich, werte ändern und dann anzeigen
// --------------------------------------------------------------------------------------

void MenuClass::DoMenu() {
    // DKS - Modified extensively while adding joystick support and fixing navigation backwards to previous menus
    bool joy_up = false;
    bool joy_down = false;
    bool joy_left = false;
    bool joy_right = false;
    bool joy_enter = false;
    bool joy_escape = false;
    bool joy_delete = false;
    //    int joy_idx = Player[0].JoystickIndex;
    static float input_counter = 0.0f;
    const float input_delay = 40.0f;  // Only accept joy input once every time counter reaches this value
    input_counter += 30.0f SYNC;
    if (input_counter > input_delay) {
        input_counter = input_delay;
    }

    // Einheitsmatrix setzen, damit das Menu richtig angezeigt wird
    //
    glm::mat4x4 matView;
    matView = glm::mat4x4(1.0f);
    g_matView = matView;

    // Wird noch keine Menu Musik gespielt ?
    //
    // DKS - Added function SongIsPlaying() to SoundManagerClass:
    // if (SoundManager.CurrentSongNr != MUSIC_MENU &&
    //        SoundManager.CurrentSongNr != MUSIC_HIGHSCORE &&
    //        SoundManager.CurrentSongNr != MUSIC_CREDITS)
    //    SoundManager.PlaySong(MUSIC_MENU, false);
    if (!SoundManager.SongIsPlaying(MUSIC_MENU) && !SoundManager.SongIsPlaying(MUSIC_HIGHSCORE) &&
        !SoundManager.SongIsPlaying(MUSIC_CREDITS))
        SoundManager.PlaySong(MUSIC_MENU, false);

    // Logo einfaden
    if (RotationDir > 0)
        Rotation += RotationDir * 10 SYNC;

    if (Rotation >= 255.0f) {
        Rotation = 255.0f;
        RotationDir = 0;
    }

    // Sterne bewegen
    //
    for (int i = 0; i < MAX_STARS; i++) {
        Stars[i].Count += Stars[i].Ebene / 20000.0f SYNC;

        if (Stars[i].Count > 2 * PI)
            Stars[i].Count -= 2 * PI;
    }

    ShowMenu();

    if (Console.Showing)
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
        int joy_idx = Player[0].JoystickIndex;
        if (joy_idx < 0 || joy_idx >= DirectInput.JoysticksFound) {
            joy_idx = 0;
        }
#if defined(GCW)
        // However, on GCW Zero the internal controls are their own joystick
        // and we must always be sure to check them.
        if (Player[0].JoystickIndex != DirectInput.GetInternalJoystickIndex()) {
            num_joys_to_check++;
        }
#endif  // GCW

        for (int joy_ctr = 0; joy_ctr < num_joys_to_check; joy_ctr++) {
#if defined(GCW)
            if (joy_ctr > 0) {
                // After first checking Player 1, check the internal GCW Zero controls:
                joy_idx = DirectInput.GetInternalJoystickIndex();
            }
#endif  // GCW

            if (DirectInput.Joysticks[joy_idx].JoystickPOV != -1) {
                // HAT switch is pressed
                if (DirectInput.Joysticks[joy_idx].JoystickPOV >= 4500 * 1 &&
                    DirectInput.Joysticks[joy_idx].JoystickPOV <= 4500 * 3) {
                    joy_right = true;
                } else if (DirectInput.Joysticks[joy_idx].JoystickPOV >= 4500 * 5 &&
                           DirectInput.Joysticks[joy_idx].JoystickPOV <= 4500 * 7) {
                    joy_left = true;
                } else if (DirectInput.Joysticks[joy_idx].JoystickPOV > 4500 * 3 &&
                           DirectInput.Joysticks[joy_idx].JoystickPOV < 4500 * 5) {
                    joy_down = true;
                } else if ((DirectInput.Joysticks[joy_idx].JoystickPOV > 4500 * 7 &&
                            DirectInput.Joysticks[joy_idx].JoystickPOV <= 4500 * 8) ||
                           (DirectInput.Joysticks[joy_idx].JoystickPOV >= 0 &&
                            DirectInput.Joysticks[joy_idx].JoystickPOV < 4500 * 1)) {
                    joy_up = true;
                }
            } else {
#if !defined(GCW)  // On GCW Zero, only accept directional input for the menu from the DPAD
                if (DirectInput.Joysticks[joy_idx].JoystickX > Player[0].JoystickSchwelle) {
                    joy_right = true;
                } else if (DirectInput.Joysticks[joy_idx].JoystickX < -Player[0].JoystickSchwelle) {
                    joy_left = true;
                } else if (DirectInput.Joysticks[joy_idx].JoystickY > Player[0].JoystickSchwelle) {
                    joy_down = true;
                } else if (DirectInput.Joysticks[joy_idx].JoystickY < -Player[0].JoystickSchwelle) {
                    joy_up = true;
                }
#endif  // GCW
            }

            joy_enter = joy_enter || DirectInput.Joysticks[joy_idx].ButtonEnterPressed();
            joy_escape = joy_escape || DirectInput.Joysticks[joy_idx].ButtonEscapePressed();
            joy_delete = joy_delete || DirectInput.Joysticks[joy_idx].ButtonDeletePressed();
        }
    }

    if ((KeyDown(DIK_SPACE) || KeyDown(DIK_RETURN) || joy_enter) && AuswahlPossible && !control_reassignment_occuring) {
        input_counter = 0.0f;
        selected = true;
    }

    bool JoyOK, KeyOK;

    KeyOK = false;
    JoyOK = false;

    if (!KeyDown(DIK_NUMPAD4) && !KeyDown(DIK_NUMPAD6) && !KeyDown(DIK_NUMPAD2) && !KeyDown(DIK_NUMPAD8) &&
        !KeyDown(DIK_LEFT) && !KeyDown(DIK_RIGHT) && !KeyDown(DIK_UP) && !KeyDown(DIK_DOWN) && !KeyDown(DIK_RETURN) &&
        !KeyDown(DIK_SPACE) && !KeyDown(DIK_ESCAPE))
        KeyOK = true;

    // DKS - TODO: Figure out if we can get rid of JoyOK, it was here originally and I don't like or want it
    if (JoystickFound == false)
        JoyOK = true;
    else {
        if (anybutton == false &&
            DirectInput.Joysticks[Player[0].JoystickIndex].JoystickY > -Player[0].JoystickSchwelle &&
            DirectInput.Joysticks[Player[0].JoystickIndex].JoystickY < Player[0].JoystickSchwelle &&
            DirectInput.Joysticks[Player[0].JoystickIndex].JoystickPOV == -1)
            JoyOK = true;
    }

    if (KeyOK == true && JoyOK == true)
        AuswahlPossible = true;

    if ((ShowLanguageInfoCounter > 256.0f && ShowLanguageInfoCounter < 1600.0f) &&
        (KeyDown(DIK_ESCAPE) || KeyDown(DIK_RETURN) || joy_escape || joy_enter)) {
        ShowLanguageInfoCounter = 256.0f;
        SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);
        AuswahlPossible = false;
        input_counter = 0.0f;
    }

    // LanguageFile Info Fenster Counter runterzählen und Fenster ggf anzeigen
    //
    if (ShowLanguageInfoCounter > 0.0f) {
        ShowLanguageInfoCounter -= 50.0f SYNC;
        ShowLanguageInfo();
        return;  // Do not accept menu input until language counter reaches 0
    }

    if (AuswahlPossible && !control_reassignment_occuring && (KeyDown(DIK_NUMPAD8) || KeyDown(DIK_UP) || joy_up)) {
        input_counter = 0.0f;
        AuswahlPossible = false;
        AktuellerPunkt--;
        SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);

        // Spieler will auf "spiel fortsetzen" wechseln, aber es läuft noch kein spiel
        //
        if (Stage == -1 && AktuellerZustand == MENUZUSTAND_MAINMENU && AktuellerPunkt == 1) {
            AktuellerPunkt = 0;
        }

        // Checken, ob bei der Tastenwahl ein leeres Feld übersprungen werden muss
        //
        if (AktuellerZustand == MENUZUSTAND_TASTEN) {
            PlayerClass *pCurrentPlayer;

            if (CurrentPlayer == 0)
                pCurrentPlayer = &Player[0];
            else
                pCurrentPlayer = &Player[1];

            // DKS - Altered to match redesigned controls submenu:
            if ((AktuellerPunkt == MENU_TASTEN_SENSITIVITY_LINE || AktuellerPunkt == MENU_TASTEN_MODE_LINE) &&
                pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD) {
                AktuellerPunkt = MENU_TASTEN_TYPE_LINE;
            }

            if (AktuellerPunkt < 0)
                AktuellerPunkt = MENU_TASTEN_NUM_LINES - 1;
        }
    }

    if (AuswahlPossible && !control_reassignment_occuring && (KeyDown(DIK_NUMPAD2) || KeyDown(DIK_DOWN) || joy_down)) {
        input_counter = 0.0f;
        AuswahlPossible = false;
        AktuellerPunkt++;
        SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);

        // Spieler will auf "spiel fortsetzen" wechseln, aber es läuft noch kein spiel
        //
        if (Stage == -1 && AktuellerZustand == MENUZUSTAND_MAINMENU && AktuellerPunkt == 1) {
            AktuellerPunkt = 2;
        }

        // Checken, ob bei der Tastenwahl ein leeres Feld übersprungen werden muss
        //
        if (AktuellerZustand == MENUZUSTAND_TASTEN) {
            PlayerClass *pCurrentPlayer;

            if (CurrentPlayer == 0)
                pCurrentPlayer = &Player[0];
            else
                pCurrentPlayer = &Player[1];

            // DKS - Altered to match redesigned controls submenu:
            if ((AktuellerPunkt == MENU_TASTEN_MODE_LINE || AktuellerPunkt == MENU_TASTEN_SENSITIVITY_LINE) &&
                pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD) {
                AktuellerPunkt = MENU_TASTEN_NUM_NON_CONTROLS;
            }

            if (AktuellerPunkt >= MENU_TASTEN_NUM_LINES)
                AktuellerPunkt = 0;
        }
    }

    //----- Je nach Menu-Zustand die Aktionen auswerten

    switch (AktuellerZustand) {
        // Haupt-Menu
        case MENUZUSTAND_MAINMENU: {
            // zurück zum Game ?
            if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true && Stage != -1) {
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

            // Menu-Punkt im Hauptmenu ausgewählt ?
            if (selected) {
                AuswahlPossible = false;

                // Je nach aktuellem Menupunkt in das entsprechende Untermenu wechseln
                if (AktuellerPunkt == MENUPUNKT_STARTGAME) {
                    AktuellerZustand = MENUZUSTAND_NEWGAME;
                    AktuellerPunkt = 1;
                } else if (AktuellerPunkt == MENUPUNKT_CONTINUEGAME && Stage > -1) {
                    AktuellerPunkt = 0;
                    SoundManager.StopSong(MUSIC_MENU, false);
                    SpielZustand = GAMELOOP;

                    // DKS - We now use the sound manager's already-present pause functions here:
#if 0
                if (MUSIC_GetPaused(SoundManager.its_Songs[MUSIC_BOSS]->SongData))
                    SoundManager.PlaySong(MUSIC_BOSS, true);
                else if (Player[0].PunisherActive == true) {
                    SoundManager.PlaySong(MUSIC_PUNISHER, true);
                }
                else if (Player[0].Riding()) {
                    SoundManager.PlaySong(MUSIC_FLUGSACK, true);
                }
                else
                    SoundManager.PlaySong(MUSIC_STAGEMUSIC, true);
#endif  // 0
                    SoundManager.UnpauseSongs();
                    SoundManager.UnpauseSounds();

                    Player[0].PowerLinePossible = false;
                    Player[0].ShotDelay = 2.0f;
                } else if (AktuellerPunkt == MENUPUNKT_CREDITS) {
                    SoundManager.StopSong(MUSIC_MENU, false);
                    // DKS
                    // SoundManager.its_Songs[MUSIC_CREDITS]->FadingVolume = 0.0f;
                    SoundManager.PlaySong(MUSIC_CREDITS, false);
                    SoundManager.SetSongVolume(MUSIC_CREDITS, 100);
                    CreditsCounter = 0.0f;
                } else if (AktuellerPunkt == MENUPUNKT_HIGHSCORES) {
                    SoundManager.SetSongVolume(MUSIC_HIGHSCORE, 0);
                    SoundManager.FadeSong(MUSIC_MENU, -5.0f, 0, false);
                    SoundManager.FadeSong(MUSIC_HIGHSCORE, 5.0f, 100, false);
                }

                if (AktuellerPunkt != MENUPUNKT_CONTINUEGAME && AktuellerPunkt != MENUPUNKT_STARTGAME)
                    AktuellerZustand = AktuellerPunkt;

                if (AktuellerPunkt == MENUPUNKT_END)
                    GameRunning = false;

                if (AktuellerPunkt != MENUPUNKT_CONTINUEGAME && AktuellerPunkt != MENUPUNKT_HIGHSCORES &&
                    AktuellerPunkt != MENUPUNKT_CREDITS && AktuellerPunkt != MENUPUNKT_END)
                    AktuellerPunkt = 0;

                if (AktuellerPunkt == MENUPUNKT_HIGHSCORES)
                    AktuellerPunkt = MENUZUSTAND_ENTERNAME;

                SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);

                // Highscore wieder von Anfang an blinken lassen
                BlinkOffset = 0;

                // Credits von Anfang an scrollen lassen
                CreditsOffset = 0;
                CreditsPosition = 0;
            }
        } break;  // MAINMENU

        // Sound / Musik Lautstärke wählen
        case MENUZUSTAND_VOLUMES: {
            if (AktuellerPunkt < 0)
                AktuellerPunkt = 4;

            if (AktuellerPunkt > 4)
                AktuellerPunkt = 0;

            // Volumes geändert ?
            // Leiser
            bool pressed = false;

            if (AktuellerPunkt <= 1) {
                if (KeyDown(DIK_NUMPAD4) || KeyDown(DIK_LEFT) || joy_left) {
                    pressed = true;
                } else {
                    pressed = false;
                }
            }

            if (pressed) {
                if (AktuellerPunkt == 0) {
                    SoundManager.g_sound_vol -= float(5.0f SYNC);
                    if (SoundManager.g_sound_vol < 0.0f)
                        SoundManager.g_sound_vol = 0.0f;
                }

                if (AktuellerPunkt == 1) {
                    SoundManager.g_music_vol -= 5.0f SYNC;
                    if (SoundManager.g_music_vol < 0.0f)
                        SoundManager.g_music_vol = 0.0f;
                }
            }

            // Lauter
            if (KeyDown(DIK_NUMPAD6) || KeyDown(DIK_RIGHT) || joy_right)
                pressed = true;
            else
                pressed = false;

            if (pressed) {
                if (AktuellerPunkt == 0) {
                    SoundManager.g_sound_vol += 5.0f SYNC;
                    if (SoundManager.g_sound_vol > 100.0f)
                        SoundManager.g_sound_vol = 100.0f;
                }
                if (AktuellerPunkt == 1) {
                    SoundManager.g_music_vol += 5.0f SYNC;
                    if (SoundManager.g_music_vol > 100.0f)
                        SoundManager.g_music_vol = 100.0f;
                }
            }

            // Punkt gewählt
            //
            if (selected) {
                AuswahlPossible = false;

                // Keyboard konfigurieren
                //
                if (AktuellerPunkt == 2) {
                    CurrentPlayer = 0;
                    AktuellerZustand = MENUZUSTAND_TASTEN;
                    AktuellerPunkt = 0;
                    control_reassignment_occuring = false;
                }

                // DKS - This was already commented out:
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
                if (AktuellerPunkt == 3) {
                    AktuellerZustand = MENUZUSTAND_LANGUAGE;
                    AktuellerPunkt = 0;
                }

                // Sprache wählen
                //
                if (AktuellerPunkt == 4) {
                    options_Detail++;

                    if (options_Detail > DETAIL_MAXIMUM)
                        options_Detail = DETAIL_LOW;

                    PartikelSystem.SetParticleCount();
                }
            }

            // zurück zum Hauptmenu ?
            if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true) {
                input_counter = 0.0f;
                SaveConfig();  // Sound Config speichern
                AuswahlPossible = false;
                AktuellerZustand = MENUZUSTAND_MAINMENU;
                AktuellerPunkt = MENUPUNKT_VOLUMES;
                SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);
            }
        } break;  // Sound / Musik Lautstärke

        case MENUZUSTAND_LANGUAGE: {
            if (AktuellerPunkt < 0)
                AktuellerPunkt = LanguageFiles.size();

            if (AktuellerPunkt > LanguageFiles.size())
                AktuellerPunkt = 0;

            // Punkt gewählt
            //
            if (selected) {
                AuswahlPossible = false;

                // Language File gewählt ?
                if (AktuellerPunkt < LanguageFiles.size()) {
                    strcpy_s(ActualLanguage, strlen(LanguageFiles[AktuellerPunkt].c_str()) + 1,
                             LanguageFiles[AktuellerPunkt].c_str());
                    LoadLanguage(ActualLanguage);
                    SaveConfig();
                    ShowLanguageInfoCounter = 2000.0f;

                    // Sprache feststellen und schauen, welche Grafik wir anzeigen müssen
                    if (strcmp(TextArray[TEXT_BENUTZTE_GRAFIK], "0") == 0)
                        Sprachgrafik = 0;  // deutsch ?
                    else
                        Sprachgrafik = 1;  // oder doch englisch ?

                    AktuellerZustand = MENUZUSTAND_VOLUMES;
                    AktuellerPunkt = MENUPUNKT_VOLUMES_LANGUAGE;
                    SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);
                }

                // oder nur zurück ?
                else {
                    AktuellerZustand = MENUZUSTAND_VOLUMES;
                    AktuellerPunkt = MENUPUNKT_VOLUMES_LANGUAGE;
                    SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);
                }
            }

            // zurück zum Hauptmenu ?
            if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true) {
                input_counter = 0.0f;
                SaveConfig();  // Sound Config speichern
                AuswahlPossible = false;
                AktuellerZustand = MENUZUSTAND_VOLUMES;
                AktuellerPunkt = MENUPUNKT_VOLUMES_LANGUAGE;
                SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);
            }

        } break;  // Languages

        // DKS - Significantly redesigned controls submenu:
        case MENUZUSTAND_TASTEN: {
            PlayerClass *pCurrentPlayer;

            if (CurrentPlayer == 0)
                pCurrentPlayer = &Player[0];
            else
                pCurrentPlayer = &Player[1];

            int action = AktuellerPunkt - MENU_TASTEN_NUM_NON_CONTROLS;
            bool on_move_line = (action == AKTION_LINKS || action == AKTION_RECHTS || action == AKTION_DUCKEN);
            bool on_look_line = (action == AKTION_OBEN || action == AKTION_UNTEN);
            bool on_jump_line = action == AKTION_JUMP;

            // If current player is using a joystick, check if the selected joystick is available
            // and, if not, make sure controls for it are never selected unless it is changed
            if (pCurrentPlayer->ControlType == CONTROLTYPE_JOY && AktuellerPunkt > MENU_TASTEN_TYPE_LINE) {
                int joy_idx = pCurrentPlayer->JoystickIndex;
                if (joy_idx < 0 || joy_idx >= DirectInput.JoysticksFound || !DirectInput.Joysticks[joy_idx].Active) {
                    AktuellerPunkt = MENU_TASTEN_TYPE_LINE;
                }
            }

            if (AktuellerPunkt >= MENU_TASTEN_NUM_NON_CONTROLS && !control_reassignment_occuring &&
                (KeyDown(DIK_DELETE) || KeyDown(DIK_BACK) || joy_delete)) {
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
            if (control_reassignment_occuring && !selected) {
                if (AuswahlPossible && AktuellerPunkt >= MENU_TASTEN_NUM_NON_CONTROLS) {
                    if (pCurrentPlayer->ControlType == CONTROLTYPE_KEYBOARD) {
#if defined(GCW)
                        // Special abort case for GCW: If the user selects keyboard controls
                        // and tries to define a key without a keyboard actually attached,
                        // allow them to back out with the GCW button mapped to 'menu escape'

                        DirectInput.UpdateJoysticks();
                        if (DirectInput.Joysticks[DirectInput.GetInternalJoystickIndex()].ButtonEscapePressed()) {
                            control_reassignment_occuring = false;
                            AuswahlPossible = false;
                        }
#endif  // GCW
        // Neue Taste?
                        DirectInput.UpdateTastatur();

                        for (int i = 1; i < DirectInput.NumberOfKeys; i++)  // Puffer durchgehen
                        {
                            // DKS - Added exclusions for TAB and ESCAPE, as they are hard-coded to
                            //      be the console and main-menu activation keys, respectively.
                            //      Also added exclusions for keys 1,2 and 3 because they are
                            //      hard-coded in the game to be Weapon-selection keys.
                            if (KeyDown(i) && i != DIK_NUMLOCK && i != DIK_CAPITAL && i != DIK_SCROLL && i != DIK_TAB &&
                                i != DIK_ESCAPE && i != DIK_1 && i != DIK_2 &&
                                i != DIK_3)  // ob eine Taste gedrückt wurde
                            {
#if defined(GCW)
                                // DKS - Added exclusion for the Hold Switch (Mapped to home key), as it is
                                //      hard-coded to be the console-activation key for GCW Zero.
                                if (i == DIK_HOME)
                                    break;
#endif  // GCW
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
                        if (DirectInput.Joysticks[pCurrentPlayer->JoystickIndex].Active) {
                            // DKS - TODO - this seems to be where more work is needed in accepting new button input
                            for (int i = 0; i < DirectInput.Joysticks[pCurrentPlayer->JoystickIndex].NumButtons; i++) {
                                if (DirectInput.Joysticks[pCurrentPlayer->JoystickIndex].JoystickButtons[i]) {
#if defined(GCW)
                                    // DKS - Added exclusion on GCW Zero for button 5, START, as it is hard-coded
                                    //      to be the Exit-to-Main-Menu button and thus cannot be reassigned.
                                    if (i == DirectInput.GetInternalJoystickMainMenuButton())
                                        break;
#endif  // GCW
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
                } else {
                    if (!KeyDown(DIK_DELETE) && !KeyDown(DIK_NUMPAD2) && !KeyDown(DIK_NUMPAD8) && !KeyDown(DIK_UP) &&
                        !KeyDown(DIK_DOWN) && !KeyDown(DIK_RETURN) && !KeyDown(DIK_SPACE) && !KeyDown(DIK_ESCAPE))
                        AuswahlPossible = true;
                }
            } else {
                if (AktuellerPunkt < 0)
                    AktuellerPunkt = MENU_TASTEN_NUM_LINES - 1;

                if (AktuellerPunkt > MENU_TASTEN_NUM_LINES - 1)
                    AktuellerPunkt = 0;

                // Spieler wechseln
                if (AuswahlPossible == true) {
                    PlayerClass *pCurrentPlayer2;

                    if (CurrentPlayer == 0)
                        pCurrentPlayer2 = &Player[0];
                    else
                        pCurrentPlayer2 = &Player[1];

                    if (KeyDown(DIK_NUMPAD6) || KeyDown(DIK_RIGHT) || joy_right) {
                        input_counter = input_delay * 0.75f;  // Delay less than usual

                        if (AktuellerPunkt == MENU_TASTEN_SENSITIVITY_LINE) {
                            pCurrentPlayer2->JoystickSchwelle -= 100.0f SYNC;

                            if (pCurrentPlayer2->JoystickSchwelle < 100.0f)
                                pCurrentPlayer2->JoystickSchwelle = 100.0f;
                        } else if (AktuellerPunkt >= MENU_TASTEN_PLAYER_LINE) {
                            AuswahlPossible = false;
                            CurrentPlayer = 1;
                            pCurrentPlayer2 = &Player[1];

                            if (pCurrentPlayer2->ControlType == CONTROLTYPE_KEYBOARD &&
                                (AktuellerPunkt == MENU_TASTEN_MODE_LINE ||
                                 AktuellerPunkt == MENU_TASTEN_SENSITIVITY_LINE))
                                AktuellerPunkt = MENU_TASTEN_TYPE_LINE;
                        }
                    }

                    // DKS - TODO allow a second joystick to adjust this
                    if (KeyDown(DIK_NUMPAD4) || KeyDown(DIK_LEFT) || joy_left) {
                        input_counter = input_delay * 0.75f;  // Delay less than usual

                        if (AktuellerPunkt == MENU_TASTEN_SENSITIVITY_LINE) {
                            pCurrentPlayer2->JoystickSchwelle += 100.0f SYNC;

                            if (pCurrentPlayer2->JoystickSchwelle > 900.0f)
                                pCurrentPlayer2->JoystickSchwelle = 900.0f;
                        } else if (AktuellerPunkt >= MENU_TASTEN_PLAYER_LINE) {
                            AuswahlPossible = false;
                            CurrentPlayer = 0;
                            pCurrentPlayer2 = &Player[0];

                            if (pCurrentPlayer2->ControlType == CONTROLTYPE_KEYBOARD &&
                                (AktuellerPunkt == MENU_TASTEN_MODE_LINE ||
                                 AktuellerPunkt == MENU_TASTEN_SENSITIVITY_LINE))
                                AktuellerPunkt = MENU_TASTEN_TYPE_LINE;
                        }
                    }
                }

                // Punkt gewählt
                //
                if (selected) {
                    AuswahlPossible = false;
                    input_counter = 0.0f;

                    // Taste ändern?
                    if (AktuellerPunkt >= MENU_TASTEN_NUM_NON_CONTROLS) {
                        if (pCurrentPlayer->ControlType == CONTROLTYPE_JOY &&
                            (on_move_line || (on_jump_line && pCurrentPlayer->JoystickMode == JOYMODE_JOYSTICK))) {
                            // If control mode is joystick/joypad, movement and look up/down can only be
                            //  assigned to either the DPAD HAT or the analog stick:
                            pCurrentPlayer->Walk_UseAxxis = !pCurrentPlayer->Walk_UseAxxis;
                        } else if (pCurrentPlayer->ControlType == CONTROLTYPE_JOY && on_look_line) {
                            pCurrentPlayer->Look_UseAxxis = !pCurrentPlayer->Look_UseAxxis;
                        } else {
                            control_reassignment_occuring = true;
                            // Do not proceed until all keys/buttons are released:
                            while (DirectInput.AnyKeyDown() || DirectInput.AnyButtonDown()) {
                                DirectInput.UpdateTastatur();
                                DirectInput.UpdateJoysticks();
                            }
                        }
                    } else {
                        PlayerClass *pCurrentPlayer2;

                        if (CurrentPlayer == 0)
                            pCurrentPlayer2 = &Player[0];
                        else
                            pCurrentPlayer2 = &Player[1];

                        if (AktuellerPunkt == MENU_TASTEN_DEFAULTS_LINE) {
                            // Load default controls for players 1/2
                            CreateDefaultControlsConfig(0);
                            CreateDefaultControlsConfig(1);
                        }

                        // Game-wide force-feedback enable/disable
                        if (AktuellerPunkt == MENU_TASTEN_FORCEFEEDBACK_LINE) {
                            UseForceFeedback = !UseForceFeedback;
                        }

                        // Joymode ändern?
                        if (AktuellerPunkt == MENU_TASTEN_MODE_LINE) {
                            if (pCurrentPlayer2->JoystickMode == JOYMODE_JOYSTICK)
                                pCurrentPlayer2->JoystickMode = JOYMODE_JOYPAD;
                            else
                                pCurrentPlayer2->JoystickMode = JOYMODE_JOYSTICK;
                        }

                        // Controller Type ändern?
                        if (AktuellerPunkt == MENU_TASTEN_TYPE_LINE) {
                            // Joytick dran?
                            if (JoystickFound == true) {
                                // Von Keyboard auf Joystick wechseln?
                                if (pCurrentPlayer2->ControlType == CONTROLTYPE_KEYBOARD) {
                                    pCurrentPlayer2->ControlType = CONTROLTYPE_JOY;
                                    pCurrentPlayer2->JoystickIndex = 0;
                                }

                                // Von Joystick auf Keyboard wechseln?
                                else {
                                    pCurrentPlayer2->JoystickIndex++;

                                    if (pCurrentPlayer2->JoystickIndex >= DirectInput.JoysticksFound) {
                                        pCurrentPlayer2->ControlType = CONTROLTYPE_KEYBOARD;
                                        pCurrentPlayer2->JoystickIndex = 0;
                                    }
                                }
                            }
                        }
                    }
                }

                // zurück zum Hauptmenu ?
                if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true) {
                    input_counter = 0.0f;
                    // Neue Tastennamen für die Tutorial-Texte übernehmen
                    InitReplacers();

                    // Sound Config speichern
                    SaveConfig();

                    AuswahlPossible = false;
                    AktuellerZustand = MENUZUSTAND_VOLUMES;
                    AktuellerPunkt = MENUPUNKT_VOLUMES_TASTEN;
                    SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);
                }
            }
        } break;  // Tasten

        // Die Credits und die Highscore
        case MENUPUNKT_CREDITS:
        case MENUPUNKT_HIGHSCORES: {
            // zurück zum Hauptmenu ?
            if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true) {
                input_counter = 0.0f;

                if (AktuellerZustand == MENUPUNKT_CREDITS) {
                    SoundManager.SetSongVolume(MUSIC_MENU, 0);
                    SoundManager.FadeSong(MUSIC_CREDITS, -5.0f, 0, false);
                    SoundManager.FadeSong(MUSIC_MENU, 5.0f, 100, false);
                    CreditsCounter = 0.0f;
                    AktuellerPunkt = 4;
                }

                if (AktuellerZustand == MENUPUNKT_HIGHSCORES) {
                    SoundManager.SetSongVolume(MUSIC_MENU, 0);
                    SoundManager.FadeSong(MUSIC_HIGHSCORE, -5.0f, 0, false);
                    SoundManager.FadeSong(MUSIC_MENU, 5.0f, 100, false);
                    AktuellerPunkt = 3;
                }

                AuswahlPossible = false;
                AktuellerZustand = MENUZUSTAND_MAINMENU;
                SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);
            }
        } break;  // Credits und Highscore

        case MENUZUSTAND_ENTERNAME: {
            // DKS - Modified extensively to allow joystick name entry
            static unsigned char Taste;
            static bool possible = false;

            int newname_len = strlen(NewName);
            int newname_maxlen = 16;

            if (joy_right) {
                input_counter = 0.0f;
                if (newname_len < newname_maxlen && newname_len > 0 && NewName[newname_len - 1] != '_') {
                    NewName[newname_len] = '_';  // Add prompt char
                    NewName[newname_len + 1] = '\0';
                    newname_len++;
                }
            } else if (joy_left) {
                input_counter = 0.0f;
                if (newname_len > 1) {
                    NewName[newname_len - 2] = '_';
                    NewName[newname_len - 1] = '\0';
                    newname_len--;
                }
            } else if (joy_up) {
                input_counter = 0.0f;
                if (newname_len > 0) {
                    // Chars 97-122 are a..z, Chars 48-57 are 0..9, Space is 32
                    if (NewName[newname_len - 1] == 'Z') {
                        NewName[newname_len - 1] = '0';  // Number '0'
                    } else if (NewName[newname_len - 1] == '9' || NewName[newname_len - 1] == '_') {
                        NewName[newname_len - 1] = 'A';
                    } else {
                        NewName[newname_len - 1]++;
                    }
                }
            } else if (joy_down) {
                input_counter = 0.0f;
                if (newname_len > 0) {
                    // Chars 97-122 are a..z, Chars 48-57 are 0..9, Space is 32
                    if (NewName[newname_len - 1] == 'A' || NewName[newname_len - 1] == '_') {
                        NewName[newname_len - 1] = '9';
                    } else if (NewName[newname_len - 1] == '0') {
                        NewName[newname_len - 1] = 'Z';
                    } else {
                        NewName[newname_len - 1]--;
                    }
                }
            }

            if (!KeyDown(DIK_RETURN) && !joy_enter)
                AuswahlPossible = true;

            if ((KeyDown(DIK_RETURN) || joy_enter) && AuswahlPossible == true) {
                input_counter = 0.0f;
                // Strip trailing cursor char
                if (newname_len > 0 && NewName[newname_len - 1] == '_') {
                    NewName[newname_len - 1] = '\0';
                    newname_len--;
                }

                // Neuen Namen an die neue Position setzen
                strcpy_s(Highscores[NewRank].Name, strlen(NewName) + 1, NewName);

                Highscores[NewRank].Stage = NewStage;
                Highscores[NewRank].Skill = NewSkill;

                // Und zurück ins Hauptmenu und dort die Highscores anzeigen
                SpielZustand = MAINMENU;
                AktuellerZustand = MENUPUNKT_HIGHSCORES;

                SoundManager.SetSongVolume(MUSIC_MENU, 0.0f);

                // Highscore noch sichern
                SaveHighscore();
            }

            DirectInput.UpdateTastatur();

            if (possible == false) {
                possible = true;

                // Testen ob die gedrückte Taste loasgelassen wurde
                if (KeyDown(Taste) || KeyDown(DIK_BACK) || KeyDown(DIK_SPACE) || joy_left || joy_right || joy_up ||
                    joy_down)
                    possible = false;
            }

            for (int i = 0; i < 256; i++)  // Puffer durchgehen
                if (KeyDown(i) && possible == true) {
                    possible = false;

                    if (KeyDown(DIK_BACK)) {
                        // DKS - added in missing/disabled backspace support
                        if (newname_len > 1) {
                            SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);
                            NewName[newname_len - 2] = '_';
                            NewName[newname_len - 1] = '\0';
                            newname_len--;
                        }
                    } else if (KeyDown(DIK_SPACE)) {
                        // DKS - added in missing/disabled space support
                        // Only insert a space if the string is not empty
                        if (newname_len > 1 && NewName[newname_len - 1] == '_' && newname_len < newname_maxlen) {
                            SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);
                            NewName[newname_len - 1] = ' ';
                            NewName[newname_len] = '_';
                            NewName[newname_len + 1] = '\0';
                            newname_len++;
                        }

                    } else {
                        Taste = i;
                        if (strlen(GetKeyName(Taste)) == 1 && newname_len < newname_maxlen) {
                            char keyname[2];
                            strcpy_s(keyname, GetKeyName(Taste));
                            keyname[0] = toupper(keyname[0]);
                            if ((keyname[0] >= 'A' && keyname[0] <= 'Z') || (keyname[0] >= '0' && keyname[0] <= '9')) {
                                SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);
                                // NewName should already have a trailing underscore, indicating the cursor..
                                //  replace it with the new char and a new underscore
                                if (newname_len > 0) {
                                    NewName[newname_len - 1] = keyname[0];
                                    NewName[newname_len] = '_';
                                    NewName[newname_len + 1] = '\0';
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
            char Buffer[32];
            snprintf(Buffer, 32, "%s", NewName);
            pMenuFont->DrawTextCenterAlign(319, ypos + 230, Buffer, D3DCOLOR_RGBA(0, 0, 255, 255), 2);
            pMenuFont->DrawTextCenterAlign(321, ypos + 230, Buffer, D3DCOLOR_RGBA(0, 0, 255, 255), 2);
            pMenuFont->DrawTextCenterAlign(320, ypos + 229, Buffer, D3DCOLOR_RGBA(0, 0, 255, 255), 2);
            pMenuFont->DrawTextCenterAlign(320, ypos + 231, Buffer, D3DCOLOR_RGBA(0, 0, 255, 255), 2);
            pMenuFont->DrawTextCenterAlign(320, ypos + 230, Buffer, D3DCOLOR_RGBA(255, 255, 255, 255), 2);

        } break;  // Namen eingeben

        // Neues Spiel starten / Savegame laden
        case MENUZUSTAND_NEWGAME: {
            if (AktuellerPunkt < 0)
                AktuellerPunkt = 2;

            if (AktuellerPunkt > 2)
                AktuellerPunkt = 0;

            // Zurück ins Haupt Menu ?
            if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true) {
                input_counter = 0.0f;
                AktuellerZustand = MENUZUSTAND_MAINMENU;
                AktuellerPunkt = MENUPUNKT_STARTGAME;
            }

            // Menu-Punkt ausgewählt ?
            if (selected) {
                AuswahlPossible = false;

                // Tutorial spielen?
                if (AktuellerPunkt == MENUPUNKT_NEWGAME_TUTORIAL) {
                    SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);

                    // Game starten mit Tutorial
                    NUMPLAYERS = 1;
                    RunningTutorial = true;
                    Skill = AktuellerPunkt;
                    //					SoundManager.StopSong(MUSIC_MENU, false);
                    InitNewGame();
                    InitNewGameLevel(1);
                    SpielZustand = GAMELOOP;
                }

                // Neues Spiel starten ?
                if (AktuellerPunkt == MENUPUNKT_NEWGAME_STARTNEWGAME) {
                    AktuellerZustand = MENUZUSTAND_PLAYERCOUNT;
                    AktuellerPunkt = 0;  // Auf Medium anfangen
                }

                // altes Spiel laden ?
                else if (AktuellerPunkt == MENUPUNKT_NEWGAME_LOADGAME) {
                    AktuellerZustand = MENUZUSTAND_LOADGAME;
                    AktuellerPunkt = 0;
                    LoadSavegames();
                }

                SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);
            }
        } break;  // SPIEL VERLASSEN

        case MENUZUSTAND_PLAYERCOUNT: {
            if (AktuellerPunkt < 0)
                AktuellerPunkt = 1;

            if (AktuellerPunkt > 1)
                AktuellerPunkt = 0;

            // Zurück ins Haupt Menu ?
            if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true) {
                input_counter = 0.0f;
                AuswahlPossible = false;
                AktuellerZustand = MENUZUSTAND_NEWGAME;
                AktuellerPunkt = MENUPUNKT_NEWGAME_STARTNEWGAME;
            }

            // Menu-Punkt ausgewählt ?
            if (selected) {
                tempNUMPLAYERS = AktuellerPunkt + 1;
                AktuellerPunkt = 1;

                AktuellerZustand = MENUZUSTAND_SELECTSKILL;
                SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);

                AuswahlPossible = false;
            }
        } break;  // SPIEL VERLASSEN

        // Skill auswählen
        case MENUZUSTAND_SELECTSKILL: {
            if (AktuellerPunkt < 0)
                AktuellerPunkt = 3;

            if (AktuellerPunkt > 3)
                AktuellerPunkt = 0;

            // Zurück ins New Game Menu ?
            if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true) {
                input_counter = 0.0f;
                AuswahlPossible = false;
                AktuellerZustand = MENUZUSTAND_PLAYERCOUNT;
                AktuellerPunkt = tempNUMPLAYERS - 1;
            }

            // Menu-Punkt ausgewählt ?
            if (selected) {
                NUMPLAYERS = tempNUMPLAYERS;
                AuswahlPossible = false;
                RunningTutorial = false;
                SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);

                // Game starten
                // mit gewähltem Skill
                Skill = AktuellerPunkt;

                // Intro initialisieren
                SoundManager.StopSongs();
                DisplayHintNr = -1;

                StartIntro();
            }
        } break;  // Select Skill

        // Savegame zum Laden wählen
        case MENUZUSTAND_LOADGAME: {
            if (AktuellerPunkt < 0)
                AktuellerPunkt = MAX_SAVEGAMES - 1;

            if (AktuellerPunkt > MAX_SAVEGAMES - 1)
                AktuellerPunkt = 0;

            // Zurück ins New Game Menu ?
            if ((KeyDown(DIK_ESCAPE) || joy_escape) && AuswahlPossible == true) {
                input_counter = 0.0f;
                AktuellerZustand = MENUZUSTAND_NEWGAME;
                AktuellerPunkt = MENUPUNKT_NEWGAME_LOADGAME;
                AuswahlPossible = false;
            }

            // Menu-Punkt ausgewählt ?
            // Dann prüfen ob das aktuelle Savegame ein leerer Slot ist
            // und wenn das nicht der Fall ist, dann das Savegame laden und
            // das Spiel starten
            if (selected) {
                RunningTutorial = false;

                AuswahlPossible = false;
                SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);

                // Aktuell gewähltes Savegame laden, wenn es existiert
                // bzw die Werte aus den schon geladenen Savegames übernehmen
                if (Savegames[AktuellerPunkt].Stage >= 0) {
                    // DKS - Original order was like so:
                    // InitNewGame();
                    // NUMPLAYERS = Savegames[AktuellerPunkt].Players;

                    // DKS - But I have reversed it here, because InitNewGame() depended on
                    //      NUMPLAYERS being set correctly.
                    NUMPLAYERS = Savegames[AktuellerPunkt].Players;
                    InitNewGame();

                    // Werte von Spieler auf das Savegame übertragen
                    Player[0].Score = Savegames[AktuellerPunkt].Score;
                    Stage = Savegames[AktuellerPunkt].Stage;
                    Skill = Savegames[AktuellerPunkt].Skill;
                    NewStage = Savegames[AktuellerPunkt].NewStage;
                    Player[0].CollectedDiamonds = Savegames[AktuellerPunkt].CollectedDiamonds;
                    Player[0].PowerLines = Savegames[AktuellerPunkt].PowerLines;
                    Player[0].Grenades = Savegames[AktuellerPunkt].Grenades;
                    Player[0].SmartBombs = Savegames[AktuellerPunkt].SmartBombs;

                    for (int p = 0; p < 2; p++) {
                        Player[p].Handlung = STEHEN;
                        Player[p].Energy = Savegames[AktuellerPunkt].Energy[p];
                        Player[p].Armour = Savegames[AktuellerPunkt].Armour[p];
                        Player[p].Shield = Savegames[AktuellerPunkt].Shield[p];
                        Player[p].SelectedWeapon = Savegames[AktuellerPunkt].SelectedWeapon[p];
                        Player[p].BlitzLength = Savegames[AktuellerPunkt].BlitzLength[p];
                        Player[p].AutoFireExtra = Savegames[AktuellerPunkt].AutoFire[p];
                        Player[p].RiesenShotExtra = Savegames[AktuellerPunkt].RiesenShot[p];
                        Player[p].Lives = Savegames[AktuellerPunkt].Lives[p];

                        for (int i = 0; i < 4; i++)
                            Player[p].CurrentWeaponLevel[i] = Savegames[AktuellerPunkt].CurrentWeaponLevel[p][i];
                    }

                    // An der Stelle im Savegame weiterspielen
                    AktuellerZustand = MAINMENU;
                    AktuellerPunkt = 0;
                    //					SoundManager.StopSong(MUSIC_MENU, false);
                    InitNewGameLevel(NewStage);  // Neues level laden
                    SpielZustand = GAMELOOP;     // Weiterspielen
                }
            }
        } break;  // Load Game

        // Savegame zum Speichern wählen
        case MENUZUSTAND_SAVEGAME: {
            if (AktuellerPunkt < 0)
                AktuellerPunkt = MAX_SAVEGAMES;

            if (AktuellerPunkt > MAX_SAVEGAMES)
                AktuellerPunkt = 0;

            // Menu-Punkt ausgewählt ?
            // Dann das Spiel an dem aktuell gewählten Slot speichern
            // oder weiterspielen ohne zu speichern (je nach Auswahl)
            if (selected) {
                AuswahlPossible = false;
                SoundManager.PlayWave(100, 128, 11025, SOUND_CLICK);

                // Bestimmtes Savegame ausgewählt
                // Dann hier speichern
                if (AktuellerPunkt < MAX_SAVEGAMES) {
                    // Werte von Spieler auf das Savegame übertragen
                    // Name des Save games aus aktuellem Datum und aktueller Zeit erstelen

                    char timestr[20];

                    time_t seconds = time(NULL);
                    struct tm *ptm = localtime(&seconds);

                    /*
                                        sprintf_s(timestr,"%02i-%02i-%04i %02i:%02i",
                                               (int)ptm->tm_mday,
                                               (int)ptm->tm_mon+1,
                                               (int)ptm->tm_year+1900,

                                               (int)ptm->tm_hour,
                                               (int)ptm->tm_min);
                    */
                    strcpy_s(timestr, asctime(ptm));

                    strcpy_s(Savegames[AktuellerPunkt].Name, 1, "");
                    strcpy_s(Savegames[AktuellerPunkt].Name, strlen(timestr) + 1, timestr);

                    Savegames[AktuellerPunkt].Players = NUMPLAYERS;
                    Savegames[AktuellerPunkt].Score = Player[0].Score;
                    Savegames[AktuellerPunkt].Stage = Stage;
                    Savegames[AktuellerPunkt].Skill = Skill;
                    Savegames[AktuellerPunkt].NewStage = NewStage;
                    Savegames[AktuellerPunkt].CollectedDiamonds = Player[0].CollectedDiamonds;
                    Savegames[AktuellerPunkt].PowerLines = Player[0].PowerLines;
                    Savegames[AktuellerPunkt].Grenades = Player[0].Grenades;
                    Savegames[AktuellerPunkt].SmartBombs = Player[0].SmartBombs;

                    for (int p = 0; p < 2; p++) {
                        Savegames[AktuellerPunkt].Energy[p] = Player[0].Energy;
                        Savegames[AktuellerPunkt].Armour[p] = Player[0].Armour;
                        Savegames[AktuellerPunkt].Shield[p] = Player[0].Shield;
                        Savegames[AktuellerPunkt].SelectedWeapon[p] = Player[0].SelectedWeapon;
                        Savegames[AktuellerPunkt].Lives[p] = Player[p].Lives;
                        Savegames[AktuellerPunkt].AutoFire[p] = Player[p].AutoFireExtra;
                        Savegames[AktuellerPunkt].RiesenShot[p] = Player[p].RiesenShotExtra;

                        for (int i = 0; i < 4; i++)
                            Savegames[AktuellerPunkt].CurrentWeaponLevel[p][i] = Player[p].CurrentWeaponLevel[i];

                        Savegames[AktuellerPunkt].BlitzLength[p] = Player[p].BlitzLength;
                    }

                    // Prüfsumme gegen Savegame-Manipulation errechnen

                    Savegames[AktuellerPunkt].Pruefsumme =
                        static_cast<int>(Player[0].Energy) + static_cast<int>(Player[0].Armour) +
                        static_cast<int>(Player[0].Shield) + Player[0].Score + Stage + Skill + NewStage +
                        Player[0].CollectedDiamonds + Player[0].SelectedWeapon;

                    for (int i = 0; i < 4; i++)
                        Savegames[AktuellerPunkt].Pruefsumme += Player[0].CurrentWeaponLevel[i];

                    Savegames[AktuellerPunkt].Pruefsumme += Player[0].BlitzLength + Player[0].PowerLines +
                                                            Player[0].Grenades + Player[0].SmartBombs + NUMPLAYERS;

                    // Und Savegame in Datei schreiben
                    std::string Name;    // Für die Dateinamen
                    std::string Buffer;  // Für _itoa

                    // Name des Savegames erstellen
                    Buffer = std::to_string(AktuellerPunkt);

                    // Versuchen, die Datei zu erstellen
                    // nur weitermachen falls es keinen Fehler gibt
                    Name = std::string(g_save_ext) + "/Savegame" + Buffer + ".save";
                    std::ofstream Datei(Name, std::ofstream::binary);

                    // Fehler beim Öffnen ? Dann leeren Slot erzeugen
                    if (!Datei) {
                    }

                    // Ansonsten Slot speichern
                    else
                        Datei.write(reinterpret_cast<char *>(&Savegames[AktuellerPunkt]),
                                    sizeof(Savegames[AktuellerPunkt]));

                    // Und Datei wieder schliessen
                    Datei.close();
                }  // Select Slot Auswahl

                // oder Weiter/Continue  ?
                //				if (AktuellerPunkt == MAX_SAVEGAMES)
                {
                    AktuellerZustand = MAINMENU;
                    AktuellerPunkt = 0;
                    InitNewGameLevel(NewStage);  // Neues level laden
                }
            }
        } break;  // Save Game

        default:
            break;
    }  // switch
}

// --------------------------------------------------------------------------------------
// Savegames laden
// --------------------------------------------------------------------------------------

void MenuClass::LoadSavegames() {
    std::string Name;  // Für die Dateinamen

    // Versuchen, die einzelnen Savegames zu laden
    for (int i = 0; i < MAX_SAVEGAMES; i++) {
        // Name des Savegames erstellen
        Name = std::string(g_save_ext) + "/Savegame" + std::to_string(i) + ".save";

        // Versuchen, die Datei zu öffnen
        // falls sie nicht existiert oder es eine Fehler gibt, ist der Slot noch leer
        std::ifstream Datei(Name, std::ifstream::binary);

        Protokoll << "save games loaded" << std::endl;

        // Fehler beim Öffnen ? Dann leeren Slot erzeugen
        //
        if (!Datei)
            Savegames[i].Stage = -1;

        else {
            // Ansonsten Slot auslesen
            //
            Datei.read(reinterpret_cast<char *>(&Savegames[i]), sizeof(Savegames[i]));

            // Und Datei wieder schliessen
            //
            Datei.close();
            // Checken ob die Pruefsumme noch stimmt
            // Prüfsumme gegen Savegame-Manipulation errechnen
            // und nur wenn diese stimmt das Savegame übernehmen
            long Pruefsumme;

            Pruefsumme = static_cast<int>(Savegames[i].Energy[0]) + static_cast<int>(Savegames[i].Armour[0]) +
                         static_cast<int>(Savegames[i].Shield[0]) + Savegames[i].Score + Savegames[i].Stage +
                         Savegames[i].Skill + Savegames[i].NewStage + Savegames[i].CollectedDiamonds +
                         Savegames[i].SelectedWeapon[0];

            for (int j = 0; j < 4; j++)
                Pruefsumme += Savegames[i].CurrentWeaponLevel[0][j];

            Pruefsumme += Savegames[i].BlitzLength[0] + Savegames[i].PowerLines + Savegames[i].Grenades +
                          Savegames[i].SmartBombs + Savegames[i].Players;

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

// DKS - Altered to allow scaled fonts on low resolution devices
void MenuClass::ShowSavegames(int Highlight) {
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
        col1_off_x = xpos / 2 + 50;
        col2_off_x = xpos / 2 + 150;
        col3_off_x = xpos / 2 + 350;
    }

    pDefaultFont->DrawTextCenterAlign(col1_off_x, title_bar_off_y, TextArray[TEXT_SAVE_STAGE], 0xFFFFFFFF, 0);
    pDefaultFont->DrawTextCenterAlign(col2_off_x, title_bar_off_y, TextArray[TEXT_SAVE_SPIELER], 0xFFFFFFFF, 0);
    pDefaultFont->DrawTextCenterAlign(col3_off_x, title_bar_off_y, TextArray[TEXT_SAVE_DATUM], 0xFFFFFFFF, 0);

    // Alle Savegames anzeigen
    for (int i = 0; i < MAX_SAVEGAMES; i++) {
        if (i == Highlight)
            col = 0xFFFFFFFF;
        else
            col = 0x80FFFFFF;
        // Stage anzeigen
        //
        if (Savegames[i].Stage >= 0) {
            std::string buffer = std::to_string(Savegames[i].Stage);
            pDefaultFont->DrawTextCenterAlign(col1_off_x, savegames_off_y + i * line_off_y, buffer.c_str(), col, 0);
        }

        // Spieler anzeigen
        //
        if (Savegames[i].Stage >= 0) {
            std::string buffer = std::to_string(Savegames[i].Players);
            pDefaultFont->DrawTextCenterAlign(col2_off_x, savegames_off_y + i * line_off_y, buffer.c_str(), col, 0);
        }

        // DKS - This was duplicate code that I removed, it did nothing different than above.. obvious bug
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
        if (Savegames[i].Stage >= 0) {
            pDefaultFont->DrawTextCenterAlign(col3_off_x, savegames_off_y + i * line_off_y, Savegames[i].Name, col, 0);
        }

        // oder ist dort noch ein leerer Slot ?
        else {
            pDefaultFont->DrawTextCenterAlign(320, savegames_off_y + i * line_off_y, TextArray[TEXT_SAVE_LEER], col, 0);
        }
    }
}

// --------------------------------------------------------------------------------------
// Highscore-Liste laden
// --------------------------------------------------------------------------------------

void MenuClass::LoadHighscore() {
    std::string name;

    // Versuchen, die Highscore Datei zu öffnen
    // falls sie nicht existiert oder es eine Fehler gibt, wird die Standard
    // Highscore gesetzt
    name = std::string(g_save_ext) + "/Hurrican.hsl";
    std::ifstream Datei(name, std::ifstream::binary);

    // Fehler beim Öffnen ? Dann standard Highscore setzen
    //
    if (!Datei)
        ResetHighscore();

    else {
        // Ansonsten Scores auslesen
        //
        for (int i = 0; i < MAX_HIGHSCORES; i++)
            Datei.read(reinterpret_cast<char *>(&Highscores[i]), sizeof(Highscores[i]));

        // Und Datei wieder schliessen
        //
        Datei.close();

        // Checken ob die Pruefsumme noch stimmt
        // und nur wenn diese stimmt die Highscore übernehmen
        // ansonsten wieder standard Highscore setzen
        long Pruefsumme;

        for (int i = 0; i < MAX_HIGHSCORES; i++) {
            Pruefsumme = 0;

            Highscores[i].Score = FixEndian(Highscores[i].Score);
            Highscores[i].Stage = FixEndian(Highscores[i].Stage);
            Highscores[i].Skill = FixEndian(Highscores[i].Skill);
            Highscores[i].Pruefsumme = FixEndian(Highscores[i].Pruefsumme);

            for (unsigned int j = 0; j < strlen(Highscores[i].Name); j++)
                Pruefsumme += Highscores[i].Name[j];

            Pruefsumme += strlen(Highscores[i].Name) + Highscores[i].Score + Highscores[i].Stage + Highscores[i].Skill;

            // Pruefsumme stimmt nicht mehr ?
            //
            if (Pruefsumme != Highscores[i].Pruefsumme) {
                strcpy_s(Highscores[i].Name, "!!! CHEATER !!!");
                Highscores[i].Stage = 0;
            }
        }
    }
}

// --------------------------------------------------------------------------------------
// Highscore-Liste speichern
// --------------------------------------------------------------------------------------

void MenuClass::SaveHighscore() {
    std::string name;

    // Highscore Datei öffnen
    name = std::string(g_save_ext) + "/Hurrican.hsl";
    std::ofstream Datei(name, std::ofstream::binary);

    // Fehler beim Öffnen ? Dann standard Highscore setzen
    //
    if (!Datei)
        ResetHighscore();

    else {
        // Ansonsten Scores speichern
        //
        for (int i = 0; i < MAX_HIGHSCORES; i++) {
            // Prüfsumme erstellen
            //
            Highscores[i].Pruefsumme = 0;

            for (unsigned int j = 0; j < strlen(Highscores[i].Name); j++)
                Highscores[i].Pruefsumme += Highscores[i].Name[j];

            Highscores[i].Pruefsumme +=
                strlen(Highscores[i].Name) + Highscores[i].Score + Highscores[i].Stage + Highscores[i].Skill;

            // SixK - SWAP TO LITTLE ENDIAN before saving (Tweaked by DKS)
            Highscores[i].Score = FixEndian(Highscores[i].Score);
            Highscores[i].Stage = FixEndian(Highscores[i].Stage);
            Highscores[i].Skill = FixEndian(Highscores[i].Skill);
            Highscores[i].Pruefsumme = FixEndian(Highscores[i].Pruefsumme);

            // Und Eintrag speichern
            //
            Datei.write(reinterpret_cast<char *>(&Highscores[i]), sizeof(Highscores[i]));

            // SixK - SWAP TO BIG ENDIAN Again once datas have been written
            Highscores[i].Score = FixEndian(Highscores[i].Score);
            Highscores[i].Stage = FixEndian(Highscores[i].Stage);
            Highscores[i].Skill = FixEndian(Highscores[i].Skill);
            Highscores[i].Pruefsumme = FixEndian(Highscores[i].Pruefsumme);
        }
    }

    // Und Datei wieder schliessen
    //
    Datei.close();
}

// --------------------------------------------------------------------------------------
// Highscore-Liste mit Standardwerten resetten
// --------------------------------------------------------------------------------------

void MenuClass::ResetHighscore() {
    char HighscoreNames[MAX_HIGHSCORES][30] = {
        "EISWUXE", "TURRI", "MANFRED", "CHRIS", "ANDREAS", "JULIAN", "RAMIRO", "PETER", "CELAL", "MARKUS",
    };

    for (int i = 0; i < MAX_HIGHSCORES; i++) {
        Highscores[i].Score = (MAX_HIGHSCORES - i) * 10000;
        Highscores[i].Skill = i % 4;
        Highscores[i].Stage = MAX_HIGHSCORES - i;

        if (Highscores[i].Stage > 9)
            Highscores[i].Stage = 9;

        strcpy_s(Highscores[i].Name, HighscoreNames[i]);

        for (unsigned int j = 0; j < strlen(Highscores[i].Name); j++)
            Highscores[i].Pruefsumme += Highscores[i].Name[j];

        Highscores[i].Pruefsumme += strlen(Highscores[i].Name) + Highscores[i].Score + Highscores[i].Stage;
    }
}

// --------------------------------------------------------------------------------------
// Infos aus der Language Datei anzeigen lassen (Zeile 1 - 8)
// --------------------------------------------------------------------------------------

// DKS - Fixed displaying language info with scaled fonts, made rectangle drawing smarter and sized dynamically
void MenuClass::ShowLanguageInfo() {
    int a1, a2;

    if (ShowLanguageInfoCounter < 0.0f)
        return;

    a1 = int(ShowLanguageInfoCounter);

    if (a1 > 255)
        a1 = 255;

    a2 = int(a1 / 2);

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
    int rect_w = longest_line * xoff_inc + border * 2;
    int rect_h = 11 * yoff_inc + border * 2;
    int rect_x = 320 - rect_w / 2;
    int rect_y = 240 - rect_h / 2;
    RenderRect(rect_x - 2, rect_y - 2, rect_w + 4, rect_h + 4, D3DCOLOR_RGBA(64, 128, 255, a2));
    RenderRect(rect_x - 1, rect_y - 1, rect_w + 2, rect_h + 2, D3DCOLOR_RGBA(0, 0, 64, a2));
    RenderRect(rect_x, rect_y, rect_w, rect_h, D3DCOLOR_RGBA(0, 0, 64, a2));
    pDefaultFont->DrawText(float(320 - pDefaultFont->StringLength(TextArray[TEXT_BENUTZTES_FILE]) / 2),
                           float(rect_y + border), TextArray[TEXT_BENUTZTES_FILE], D3DCOLOR_RGBA(255, 255, 255, a1));

    for (int i = 0; i < 9; i++) {
        int xoff = 320 - (strlen(TextArray[i]) - 1) * xoff_inc / 2;
        for (unsigned int j = 0; j < strlen(TextArray[i]); j++) {
            char c[2];
            c[0] = TextArray[i][j];
            c[1] = '\0';
            pDefaultFont->DrawText(float(xoff), float(rect_y + border + ((2 + i) * yoff_inc)), c,
                                   D3DCOLOR_RGBA(255, 255, 255, a1));
            xoff += xoff_inc;
        }
    }
}  // ShowLanguageInfo

// --------------------------------------------------------------------------------------
// Prüfen obs eine neue Highscore gibt
// --------------------------------------------------------------------------------------

void MenuClass::CheckForNewHighscore() {
    AktuellerZustand = MENUPUNKT_HIGHSCORES;
    SoundManager.StopSongs();
    SoundManager.StopSounds();
    SoundManager.PlaySong(MUSIC_HIGHSCORE, false);

    // Feststellen ob wir eine neue Highscore erreicht haben
    //
    NewScore = 0;
    NewRank = 0;
    AktuellerPunkt = 0;
    // DKS - Initialize NewName with just the cursor char
    strcpy_s(NewName, "_");

    // Alle Highscores durchgehen und vergleichen
    //
    bool besser = false;

    for (int j = 0; j < MAX_HIGHSCORES; j++)
        if (Player[0].Score >= Highscores[j].Score && besser == false) {
            besser = true;

            // Ab der Stelle wo ein Namen eingefügt wird eine Zeile runterscrollen
            //
            for (int k = MAX_HIGHSCORES - 1; k > j; k--) {
                Highscores[k].Score = Highscores[k - 1].Score;
                Highscores[k].Stage = Highscores[k - 1].Stage;
                Highscores[k].Skill = Highscores[k - 1].Skill;
                strcpy_s(Highscores[k].Name, Highscores[k - 1].Name);
            }

            Highscores[j].Score = Player[0].Score;
            NewScore = Player[0].Score;
            NewStage = Stage;
            NewSkill = Skill;
            Player[0].Score = 0;

            // Werte für den neuen Eintrag löschen
            NewRank = j;
            // DKS - Initialize NewName with just the cursor char
            strcpy_s(NewName, "_");
            strcpy_s(Highscores[NewRank].Name, "");
            AktuellerZustand = MENUZUSTAND_ENTERNAME;
            break;
        }

    SpielZustand = MAINMENU;
}

// --------------------------------------------------------------------------------------
// Progress bar
// --------------------------------------------------------------------------------------

void MenuClass::ResetProgressBar() {
    LoadingProgress = 320.0f;
}

void MenuClass::StartProgressBar(int items) {
    LoadingProgress = 0.0f;
    LoadingItemsToLoad = static_cast<float>(items);
    LoadingItemsLoaded = 0;
}

void MenuClass::UpdateProgressBar() {
    LoadingScreen.RenderSprite((640 - 360) / 2, (480 - 60) / 2 + 5, 0x88FFFFFF);

    LoadingBar.SetRect(0, 0, int(LoadingProgress), 19);
    LoadingBar.RenderSprite((640 - 318) / 2, (480 - 19) / 2 + 5, 0x88FFFFFF);

    LoadingItemsLoaded++;

    LoadingProgress += 318.0f / LoadingItemsToLoad;
    if (LoadingProgress > 318.0f)
        LoadingProgress = 318.0f;
}
