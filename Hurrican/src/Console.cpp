// Datei : Console.cpp

// --------------------------------------------------------------------------------------
//
// Konsolen-Klasse für Hurrican
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;
#include "Console.hpp"
#include "DX8Graphics.hpp"
#include "Gameplay.hpp"
#include "GetKeyName.hpp"
#include "Globals.hpp"
#include "Logdatei.hpp"
#include "Player.hpp"
#include "Projectiles.hpp"
#include "Tileengine.hpp"
#include "Timer.hpp"

// --------------------------------------------------------------------------------------
// Konsolen-Klasse
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

ConsoleClass::ConsoleClass() {
    its_Alpha = 0.0f;
    Fade = 0.0f;
    Activate = true;
    Active = false;
    Showing = false;
    RenderBinary = false;
    CursorChar[0] = ' ';
    CursorChar[1] = '\0';

    // Text in der Konsole leeren
    for (int i = 0; i < MAX_LINES; i++)
        strcpy_s(Text[i], 1, "");

    // Text in Konsole schreiben
    for (int i = 0; i < 9; i++)
        this->print(TextArray[TEXT::CONSOLE1 + i]);

    // Lock-Puffer leeren
    Pressed.reset();

}  // Konstruktor

// --------------------------------------------------------------------------------------
// Destruktor
// --------------------------------------------------------------------------------------

ConsoleClass::~ConsoleClass() {
    // DKS - There is no need call this explicitly, it leads to a double-free:
    // ConsoleGFX.~DirectGraphicsSprite();	// Grafik freigeben
}  // Destruktor

void ConsoleClass::LoadSprites() {
    ConsoleGFX.LoadImage("console.png", 512, 256, 512, 256, 1, 1);  // Grafik laden
}

// --------------------------------------------------------------------------------------
// Konsole anzeigen
// --------------------------------------------------------------------------------------

void ConsoleClass::ShowConsole() {
    // DKS - modified code to support scaled fonts (less lines displayed) and allow joystick input
    static float cursorcount = 0.0f;
    int scale_factor = pDefaultFont->GetScaleFactor();               // DKS - added font scaling support
    int console_lines = MAX_LINES / pDefaultFont->GetScaleFactor();  // DKS - ditto
    int line_spacing = 12 * scale_factor;
    // a = int (its_Alpha * 8 / 9);  //DKS - This line was commented out in original source
    int a = static_cast<int>(its_Alpha);

    float yoffset = -255.0f + its_Alpha - 1.0f;

    D3DCOLOR Color = D3DCOLOR_RGBA(255, 255, 255, a);
    ConsoleGFX.RenderSpriteScaled(0, yoffset, 640, 256, Color);

    // DKS- Fixed for low-res devices using scaled fonts:
    //// Text anzeigen
    // for (int i=MAX_LINES-ConsoleLines, line_pos=0; i<MAX_LINES; i++, line_pos++)
    //{
    //    a = int (its_Alpha * 8 / 9);
    //    Color = D3DCOLOR_RGBA(128, 128, 255, a);
    //    pDefaultFont->DrawText(26, yoffset + static_cast<float>(10 + line_pos * line_spacing), Text[i], Color);
    //}

    // DKS-Draw array of lines of text, but only draw the lines that will fit on the screen
    //     using the current font scaling. Low-res devices like 320x240can only display
    //     8 lines instead of 16, for example:
    Color = D3DCOLOR_RGBA(128, 128, 255, a);
    for (int i = 0; i < console_lines; i++) {
        int array_off = MAX_LINES - console_lines + i;
        // DKS - Do a bounds check here just to be extra safe:
        if (array_off >= 0 && array_off < MAX_LINES)
            pDefaultFont->DrawText(26, yoffset + static_cast<float>(10 + i * line_spacing), Text[array_off], Color);
    }

    std::string Temp(">");
    Temp.append(Buffer);

    // Cursor animieren
    cursorcount += Timer.sync(0.3f);

    if (cursorcount >= 2.0f)
        cursorcount = 0.0f;

    Color = D3DCOLOR_RGBA(255, 255, 255, a);
    pDefaultFont->DrawText(26 - 6 * scale_factor, yoffset + 10 + console_lines * line_spacing, Temp.c_str(), Color);

    // Draw cursor block
    int text_width = pDefaultFont->StringLength(Temp.c_str());
    int cursor_x = 20 + text_width;
    if (scale_factor > 1)
        cursor_x -= 6;
    int cursor_y = yoffset + 10 + console_lines * line_spacing;
    int cursor_h = 12 * scale_factor;
    int cursor_w = 9 * scale_factor;

    switch (static_cast<int>(cursorcount)) {
        case 0:
            pDefaultFont->DrawText(cursor_x, cursor_y, CursorChar, Color);
            RenderRect(cursor_x, cursor_y, cursor_w, cursor_h, D3DCOLOR_RGBA(255, 255, 255, 140));
            break;

        case 1:
            pDefaultFont->DrawText(cursor_x, cursor_y, CursorChar, Color);
            break;

        default:
            break;
    }

}  // ShowConsole

// --------------------------------------------------------------------------------------
// Cheat eingegeben?
// --------------------------------------------------------------------------------------

bool ConsoleClass::CONSOLE_CHEAT(char *cheat) {

    // TODO FIX
    bool result = CONSOLE_COMMAND(convertText(cheat));

    if (result) {
        SoundManager.PlayWave(100, 128, 15000, SOUND::MESSAGE);
        Player[0].Score = 0;
        HasCheated = true;
    }

    return result;
}

// --------------------------------------------------------------------------------------
// Text checken, wenn Enter gedrückt wurde
// --------------------------------------------------------------------------------------

void ConsoleClass::CheckCommands() {
    // Hilfe
    //
    // DKS - Re-enabled help, as it seems to be just fine
    //#ifndef NDEBUG
    // TODO FIX
    if (CONSOLE_COMMAND("help")) {
        this->print(" ");
        this->print("Console Commands");
        this->print("-------------------------");
        this->print(" ");
        this->print("clear - clear console");
        this->print("levelinfo - show level information");
        this->print("maxfps x - set maximum framerate to 'x' (0=no maximum)");
        this->print("setspeed x - Set Game-speed (default=10)");
        this->print("loadmap 'name' - load level 'name'");
        this->print("minimap - save minimap to disc");
        this->print("quit - Quit game immediately");
    } else

        // Konsole löschen
        if (CONSOLE_COMMAND("clear")) {
        for (int i = 0; i < MAX_LINES; i++)
            strcpy_s(Text[i], "");

        strcpy_s(Buffer, "");
        CursorChar[0] = ' ';
    } else

        // Levelinfo anzeigen
        if (CONSOLE_COMMAND("levelinfo")) {
        std::string StringBuffer;

        this->print(" ");
        this->print("Levelinfo");
        this->print("-------------");
        this->print(" ");

        this->print(TileEngine.Beschreibung);

        StringBuffer = "Level Offset X : ";
        StringBuffer += std::to_string(TileEngine.XOffset);
        this->print(StringBuffer);

        StringBuffer = "Level Offset Y : ";
        StringBuffer += std::to_string(TileEngine.XOffset);
        this->print(StringBuffer);

        StringBuffer = "Level Size X : ";
        StringBuffer += std::to_string(TileEngine.LEVELSIZE_X);
        this->print(StringBuffer);

        StringBuffer = "Level Size Y : ";
        StringBuffer += std::to_string(TileEngine.LEVELSIZE_Y);
        this->print(StringBuffer);
    } else

        // Spiel sofort verlassen
        if (CONSOLE_COMMAND("quit")) {
        this->print("Shutting down ...");
        GameRunning = false;
    } else

        // Volle Extrawaffen
        if (CONSOLE_CHEAT(Cheats[CHEAT::EXTRAS])) {
        for (int p = 0; p < NUMPLAYERS; p++) {
            Player[p].PowerLines = 999;
            Player[p].Grenades = 999;
            Player[p].SmartBombs = 999;
        }

        this->print("-> Let's rock!");
    } else

        // Volle Zeit
        if (CONSOLE_CHEAT(Cheats[CHEAT::ZEIT])) {
        TileEngine.Timelimit = 999.0f;

        this->print("-> knowing that there's no rhyme...");
    } else

        // Volle Waffen
        if (CONSOLE_CHEAT(Cheats[CHEAT::WAFFEN])) {
        for (int p = 0; p < NUMPLAYERS; p++) {
            Player[p].CurrentWeaponLevel[3] = 16;

            for (int w = 0; w <= 2; w++)
                Player[p].CurrentWeaponLevel[w] = 8;
        }

        this->print("-> Blast Off and Strike The Evil Bydo Empire!");
    } else

        // Schild
        if (CONSOLE_CHEAT(Cheats[CHEAT::SCHILD])) {
        for (int i = 0; i < NUMPLAYERS; i++) {
            /*
            if (Player[i].Shield <= 0.0f)
            {
                Projectiles.PushProjectile (Player[i].xpos, Player[i].ypos, SHIELDSPAWNER, Player[i]);
                Projectiles.PushProjectile (Player[i].xpos, Player[i].ypos, SHIELDSPAWNER2, Player[i]);
            }

            // Schild setzen
            Player[i].Shield = 500.0f;*/

            Player[i].Lives = 99;
        }

        strcpy_s(Buffer, "-> Live long and prosper!");
    } else

        // Dauerfeuer
        if (CONSOLE_CHEAT(Cheats[CHEAT::SUPERSHOT])) {
        for (int i = 0; i < NUMPLAYERS; i++)
            Player[i].RiesenShotExtra += 500;

        strcpy_s(Buffer, "-> Supershot");
    } else

        // Dauerfeuer
        if (CONSOLE_CHEAT(Cheats[CHEAT::AUTOFIRE])) {
        for (int i = 0; i < NUMPLAYERS; i++)
            Player[i].AutoFireExtra += 500;

        strcpy_s(Buffer, "-> Autofire");
    } else

        // Flammenwerfer Mode
        if (CONSOLE_CHEAT(Cheats[CHEAT::FLAMER])) {
        FlameThrower = !FlameThrower;

        if (FlameThrower) {
            this->print("-> Flamethrower on");
        } else {
            this->print("-> Flamethrower off");
        }
    }  // else

    // GodMode
    if (CONSOLE_CHEAT(Cheats[CHEAT::GOD])) {
        if (Player[0].GodMode == false) {
            Player[0].GodMode = true;
            Player[1].GodMode = true;
            this->print("-> Godmode on");
        } else {
            Player[0].GodMode = false;
            Player[1].GodMode = false;
            this->print("-> Godmode off");
        }
    }  // else

    // GodMode
    if (CONSOLE_CHEAT(Cheats[CHEAT::RAD])) {
        if (Player[0].WheelMode == false) {
            Player[0].WheelMode = true;
            Player[1].WheelMode = true;
            this->print("-> WheelMode on");
        } else {
            Player[0].WheelMode = false;
            Player[1].WheelMode = false;
            this->print("-> WheelMode off");
        }
    }  // else

    // max FPS setzen
    if (strncmp(Buffer, "maxfps ", 7) == 0) {
        // Bis zu der Zahl vorgehen
        int index1 = 7;
        int g_test = 0;

        while (Buffer[index1] != '\0') {
            if (Buffer[index1] >= 48 && Buffer[index1] <= 58) {
                g_test *= 10;
                g_test += Buffer[index1] - 48;
            }
            index1++;
        }

        std::string StringBuffer = "Setting maximum Framerate to ";
        StringBuffer += std::to_string(g_test);
        StringBuffer += " ...";
        this->print(StringBuffer);

        // und FPS Setzen
        Timer.SetMaxFPS(g_test);
    }

    //#ifndef NDEBUG
    // Speed setzen
    if (strncmp(Buffer, "setspeed ", 9) == 0) {
        // Bis zu der Zahl vorgehen
        int index1 = 9;
        int g_test = 0;

        while (Buffer[index1] != '\0') {
            if (Buffer[index1] >= 48 && Buffer[index1] <= 58) {
                g_test *= 10;
                g_test += Buffer[index1] - 48;
            }
            index1++;
        }

        std::string StringBuffer = "Setting Speed to ";
        StringBuffer += std::to_string(g_test);
        StringBuffer += " ...";
        this->print(StringBuffer);

        // und Speed Setzen
        Timer.SetMoveSpeed(static_cast<float>(g_test));
    }
    //#endif

    // Stage laden
    if (strncmp(Buffer, "loadmap ", 8) == 0) {
        std::string mapname = std::string(Buffer).substr(8);

        // Meldung ausgeben
        std::string msg = std::string("Loading Level ") + mapname + " ...";
        this->print(msg.c_str());

        // und Level laden
        if (mapname.substr(mapname.length() - 4) != ".map")
            mapname += ".map";

        std::string filename = g_storage_ext + "/data/levels/" + mapname;

        // Datei gefunden? Dann laden
        if (fs::exists(filename) && fs::is_regular_file(filename)) {
            for (int i = 0; i < NUMPLAYERS; i++)
                Player[i].InitNewLevel();

            this->print("found.");
            TileEngine.LoadLevel(mapname);
            Protokoll << mapname << std::endl;
        }

        // Ansonsten Fehlermeldung
        else {
            msg = std::string("Map ") + mapname + " not found!";
            this->print(msg.c_str());
        }
    }

    // Minimap anzeigen und Screenshot machen
    if (strncmp(Buffer, "minimap", 7) == 0) {
        DirectGraphics.ShowBackBuffer();

        // Screen schwarz färben
        RenderRect(0, 0, RENDERWIDTH, RENDERHEIGHT, 0xFF000000);

        // Mini Map anzeigen
        for (int i = 0; i < TileEngine.LEVELSIZE_X; i++)
            for (int j = 0; j < TileEngine.LEVELSIZE_Y; j++) {
                if (TileEngine.TileAt(i, j).Block & BLOCKWERT_WAND)
                    RenderRect(static_cast<float>(i), static_cast<float>(j), 1, 1, 0xFFFFFFFF);
                if (TileEngine.TileAt(i, j).Block & BLOCKWERT_DESTRUCTIBLE)
                    RenderRect(static_cast<float>(i), static_cast<float>(j), 1, 1, 0xFFFFFF00);
                if (TileEngine.TileAt(i, j).Block & BLOCKWERT_PLATTFORM)
                    RenderRect(static_cast<float>(i), static_cast<float>(j), 1, 1, 0xFF888888);
                if (TileEngine.TileAt(i, j).Block & BLOCKWERT_WASSER)
                    RenderRect(static_cast<float>(i), static_cast<float>(j), 1, 1, 0xFF0000FF);
            }

        std::string buf = "Level " + std::to_string(Stage);
        pDefaultFont->DrawText(3, 458, buf.c_str(), 0xFF00FF00);
        pDefaultFont->DrawText(3, 470, TileEngine.Beschreibung, 0xFF00FF00);

        DirectGraphics.ShowBackBuffer();

        // Screenshot machen
        DirectGraphics.TakeScreenshot("MiniMap", RENDERWIDTH, RENDERHEIGHT);
        this->print("Mini Map saved !");
    }

    // Demo aufnehmen
    if (CONSOLE_COMMAND("record demo")) {
        NewDemo("Demo.dem");
        this->print("Recording demo");
        this->Activate = false;
        this->Fade = -25.0f;
        this->Active = false;
        this->Showing = false;
    }  // else

    // Demo anhalten
    if (CONSOLE_COMMAND("stop demo")) {
        EndDemo();
        this->print("Demo stopped");
    }  // else

    // Demo laden und abspielen
    if (CONSOLE_COMMAND("load demo")) {
        if (LoadDemo("Demo.dem") == false) {
            this->print("Error loading demo");
        } else {
            this->print("Playing demo");
            this->Activate = false;
            this->Fade = -25.0f;
            this->Active = false;
            this->Showing = false;
        }
    }  // else

    // 0 und 1 Render Mode anschalten
    if (CONSOLE_COMMAND("binary")) {
        if (this->RenderBinary == false) {
            this->print("Rendering Binary");
            this->RenderBinary = true;
        } else {
            this->print("Rendering Normal");
            this->RenderBinary = false;
        }
    }  // else

#ifndef NDEBUG
    // In Level warpen
    if (strncmp(Buffer, "goto ", 5) == 0) {
        // Bis zu der Zahl vorgehen
        int index1 = 5;
        int g_test = 0;

        while (Buffer[index1] != '\0') {
            if (Buffer[index1] >= 48 && Buffer[index1] <= 58) {
                g_test *= 10;
                g_test += Buffer[index1] - 48;
            }
            index1++;
        }

        // Level > 0? Dann dorthin springen
        //
        if (g_test >= MAX_LEVELS) {
            this->print("Level does not exist");
        } else if (g_test > 0) {
            this->Activate = false;
            this->Fade = -25.0f;
            this->Active = false;
            this->Showing = false;

            InitNewGame();

            for (int p = 0; p < NUMPLAYERS; p++) {
                Player[p].Aktion.reset();

                Stage = g_test;
                NewStage = g_test;
            }

            InitNewGameLevel();
        }
    }  // else
#endif

    if (CONSOLE_COMMAND("light cool")) {
        TileEngine.ComputeCoolLight();
    }

    if (CONSOLE_COMMAND("lamp")) {
        if (TileEngine.bDrawShadow == false) {
            TileEngine.bDrawShadow = true;
            ShadowAlpha = 255.0f;
        } else
            TileEngine.bDrawShadow = false;
    }

    // strcpy_s(Buffer, "Error : Unknown Command !");

}  // CheckCommands

// --------------------------------------------------------------------------------------
// Eine Zeile hochscrollen
// --------------------------------------------------------------------------------------

void ConsoleClass::ScrollUp() {
    // Zeilen eins hochscrollen
    // DKS - Fixed overwriting of the lines buffer by one line:
    /*for (int i = 0; i < MAX_LINES; i++)*/
    for (int i = 0; i < MAX_LINES - 1; i++)
        strcpy_s(Text[i], Text[i + 1]);

    // Und unterste Zeile mit der letzten eingabe füllen
    /*strcpy_s(Text[MAX_LINES], Buffer);*/
    strcpy_s(Text[MAX_LINES - 1], Buffer);
    Buffer[0] = '\0';
    CursorChar[0] = ' ';
}  // ScrollUp

// --------------------------------------------------------------------------------------
// Checken ob was eingegeben wurde
// --------------------------------------------------------------------------------------

// DKS - Added joystick support:
void ConsoleClass::CheckInput() {
    static float joy_counter = 0.0f;
    constexpr float JOY_DELAY = 30.0f;  // Only accept joy input once every time counter reaches this value

    joy_counter += Timer.sync(30.0f);
    if (joy_counter > JOY_DELAY) {
        joy_counter = JOY_DELAY;
    }

    if (joy_counter >= JOY_DELAY && Player[0].ControlType == CONTROLTYPE_JOY && JoystickFound) {
        joy_counter = 0.0f;
        int joy_idx = Player[0].JoystickIndex;
        bool up = false;
        bool down = false;
        bool left = false;
        bool right = false;

        if (DirectInput.Joysticks[joy_idx].JoystickPOV != -1) {
            // HAT switch is pressed
            if (DirectInput.Joysticks[joy_idx].JoystickPOV >= 4500 * 1 &&
                DirectInput.Joysticks[joy_idx].JoystickPOV <= 4500 * 3) {
                right = true;
            } else if (DirectInput.Joysticks[joy_idx].JoystickPOV >= 4500 * 5 &&
                       DirectInput.Joysticks[joy_idx].JoystickPOV <= 4500 * 7) {
                left = true;
            } else if (DirectInput.Joysticks[joy_idx].JoystickPOV > 4500 * 3 &&
                       DirectInput.Joysticks[joy_idx].JoystickPOV < 4500 * 5) {
                down = true;
            } else if ((DirectInput.Joysticks[joy_idx].JoystickPOV > 4500 * 7 &&
                        DirectInput.Joysticks[joy_idx].JoystickPOV <= 4500 * 8) ||
                       (DirectInput.Joysticks[joy_idx].JoystickPOV >= 0 &&
                        DirectInput.Joysticks[joy_idx].JoystickPOV < 4500 * 1)) {
                up = true;
            }
        } else if (DirectInput.Joysticks[joy_idx].JoystickX > Player[0].JoystickSchwelle) {
            right = true;
        } else if (DirectInput.Joysticks[joy_idx].JoystickX < -Player[0].JoystickSchwelle) {
            left = true;
        } else if (DirectInput.Joysticks[joy_idx].JoystickY > Player[0].JoystickSchwelle) {
            down = true;
        } else if (DirectInput.Joysticks[joy_idx].JoystickY < -Player[0].JoystickSchwelle) {
            up = true;
        }

        if (right) {
            strcat_s(Buffer, CursorChar);
        } else if (left) {
            std::size_t buf_len = strlen(Buffer);
            if (buf_len >= 1) {
                Buffer[buf_len - 1] = '\0';
            }
        } else if (up) {
            // Chars 97-122 are a..z, Chars 48-57 are 0..9, Space is 32
            if (CursorChar[0] == ' ') {
                CursorChar[0] = 'a';
            } else if (CursorChar[0] == 'z') {
                CursorChar[0] = '0';  // Number '0'
            } else if (CursorChar[0] == '9') {
                CursorChar[0] = ' ';
            } else {
                CursorChar[0]++;
            }
        } else if (down) {
            // Chars 97-122 are a..z, Chars 48-57 are 0..9, Space is 32
            if (CursorChar[0] == ' ') {
                CursorChar[0] = '9';
            } else if (CursorChar[0] == 'a') {
                CursorChar[0] = ' ';  // Number '0'
            } else if (CursorChar[0] == '0') {
                CursorChar[0] = 'z';
            } else {
                CursorChar[0]--;
            }
        }

        // Now, handle enter/return (Button 0)
        if (DirectInput.Joysticks[joy_idx].JoystickButtons[0]) {
            if (strlen(Buffer) > 0) {
                CheckCommands();
                ScrollUp();
            }
        }
    }

    for (int i = 0; i < 256; i++) {
        if (KeyDown(i) && !Pressed[i]) {
            SoundManager.PlayWave(100, 128, 15000, SOUND::CLICK);

            Pressed[i] = true;

            // Zeichen anhängen
            // DKS - Fixed buffer overflow by adding strlen check that encompasses both
            //      single characters and spaces. Previously, only single chars had a
            //      check and, worse, it had an off-by-one error. (Caught with valgrind)
            //      Also: added check of string's on-screen length drawn with current
            //      font, to ensure line never goes off right edge of console, especially
            //      in low-res modes using new font scaling support.
            if (strlen(Buffer) < MAX_CHARS - 1 && pDefaultFont->StringLength(Buffer) < 570) {
                // Space anhängen
                if (i == DIK_SPACE) {
                    strcat_s(Buffer, " ");
                } else if (strlen(GetKeyName(i)) ==
                           1)  // DKS - Replacement line that also has saner strlen(GetKeyName) check
                {
                    // Single characters:

                    // DKS - This original code is a bit obtuse. It was brought to my attention by
                    //      valgrind  complaining of a conditional on uninitialized data. It
                    //      wasn't a true problem here, but might as well get valgrind to quiet down
                    //      while I'm here:
                    // DKS - We already know GetKeyName(i)'s strlen was <= 1, why use a huge buffer?
                    // char buf[50];
                    // strcpy_s(buf, GetKeyName(i));
                    // unwandeln in kleinbuchstaben
                    // if (!KeyDown(DIK_LSHIFT)) {
                    //    char c;
                    //    for (int i=0; i < 50; i++) {
                    //        c = buf[i];
                    //        if (c >= 65 && c <= 90)
                    //            c += 32;
                    //        buf[i] = c;
                    //    }
                    //}
                    // strcat_s(Buffer, buf);

                    char buf[2];
                    buf[0] = *GetKeyName(i);
                    buf[1] = '\0';
                    if (!KeyDown(DIK_LSHIFT))
                        buf[0] = tolower(buf[0]);
                    strcat_s(Buffer, buf);
                }
            }

            // Backspace
            if (i == DIK_BACK && strlen(Buffer) > 0) {
                std::string Temp(Buffer, strlen(Buffer) - 1);

                strcpy_s(Buffer, "");
                strncat_s(Buffer, Temp.c_str(), MAX_CHARS-1);
            }

            // Neue Zeile
            if (i == DIK_RETURN) {
                if (strlen(Buffer) > 0) {
                    CheckCommands();
                    ScrollUp();
                }
            }
        }

        // Gedrückte Tasten locken
        if (!KeyDown(i))
            Pressed[i] = false;
    }
}  // CheckInput

// --------------------------------------------------------------------------------------
// Konsole zeigen
// --------------------------------------------------------------------------------------

void ConsoleClass::Open() {
    Fade = 25.0f;
    Active = true;
    Showing = true;
    strcpy_s(Buffer, "");
    CursorChar[0] = ' ';
}

// --------------------------------------------------------------------------------------
// Konsole verstecken
// --------------------------------------------------------------------------------------

void ConsoleClass::Hide() {
    Fade = -25.0f;
    Active = false;
}

// --------------------------------------------------------------------------------------
// Konsole ablaufen lassen
// --------------------------------------------------------------------------------------

bool ConsoleClass::DoConsole() {
#if defined(GCW)
    // On GCW Zero, the Home key (Power Slider switch) is mapped to the console as well as the Tab key.
    // Konsole ist aktivierbar
    if (!KeyDown(DIK_TAB) && !KeyDown(DIK_HOME))
        Activate = true;

    // Konsole wird aktiviert oder deaktiviert ?
    if ((KeyDown(DIK_TAB) || KeyDown(DIK_HOME)) && Activate == true) {
        Activate = false;

        if (Active == true)
            Hide();
        else

            if (Active == false)
            Open();
    }
#else
    // Konsole ist aktivierbar
    if (!KeyDown(DIK_TAB))
        Activate = true;

    // Konsole wird aktiviert oder deaktiviert ?
    if (KeyDown(DIK_TAB) && Activate) {
        Activate = false;

        if (Active)
            Hide();
        else

            if (!Active)
            Open();
    }
#endif  // GCW

    // Konsole fadet gerade ?
    if (Fade != 0.0f)
        its_Alpha += Timer.sync(2.0f * Fade);

    // Überlauf verhindern
    if (its_Alpha > 255.0f) {
        its_Alpha = 255.0f;
        Fade = 0.0f;
    }

    // Konsole ist ausgefadet ?
    if (its_Alpha < 0.0f) {
        its_Alpha = 0.0f;
        Fade = 0.0f;
    }

    // Konsole ausgefadet? Dann aussteigen
    if (!Active && its_Alpha <= 0.0f) {
        Showing = false;
        return false;
    }

    // Ansonsten Eingabe checken
    CheckInput();

    // Einheitsmatrix setzen, da Konsole nicht rotiert wird
    //
    glm::mat4x4 matView;
    matView = glm::mat4x4(1.0f);
    g_matView = matView;

    // Und anzeigen
    ShowConsole();

    SetScreenShake();

    return true;
}  // DoCOnsole

void ConsoleClass::print(const std::string &output) {
    strcpy_s(Buffer, MAX_CHARS-1, output.c_str());
    ScrollUp();
}

void ConsoleClass::print(const char *output) {
    strcpy_s(Buffer, MAX_CHARS-1, output);
    ScrollUp();
}
