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
#include "Console.h"
#include "DX8Graphics.h"
#include "GetKeyName.h"
#include "Gameplay.h"
#include "Globals.h"
#include "Logdatei.h"
#include "Projectiles.h"
#include "Player.h"
#include "Tileengine.h"
#include "Timer.h"

// --------------------------------------------------------------------------------------
// Konsolen-Klasse
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

ConsoleClass::ConsoleClass()
{
    its_Alpha		= 0.0f;
    Fade			= 0.0f;
    Activate		= true;
    Active			= false;
    Showing			= false;
    RenderBinary    = false;
    CursorChar[0]      = ' ';      
    CursorChar[1]      = '\0';      
    ConsoleLines        = MAX_LINES / pDefaultFont->GetScaleFactor();

    // Text in der Konsole leeren
    for (int i = 0; i < MAX_LINES; i++)
        strcpy_s(Text[i], 1, "");

    // Text in Konsole schreiben
    for (int i = 0; i < 9; i++)
        CONSOLE_PRINT(TextArray[TEXT_CONSOLE1+i]);

    // Lock-Puffer leeren
    for (int i = 0; i < 256; i++)
        Pressed[i] = false;

} // Konstruktor

// --------------------------------------------------------------------------------------
// Destruktor
// --------------------------------------------------------------------------------------

ConsoleClass::~ConsoleClass()
{
    //DKS - There is no need call this explicitly, it leads to a double-free:
    //ConsoleGFX.~DirectGraphicsSprite();	// Grafik freigeben
} // Destruktor

void ConsoleClass::LoadSprites(void)
{
    ConsoleGFX.LoadImage("console.png", 512, 256, 512, 256, 1, 1);	// Grafik laden
}

// --------------------------------------------------------------------------------------
// Konsole anzeigen
// --------------------------------------------------------------------------------------

void ConsoleClass::ShowConsole(void)
{
    //DKS - modified code to support scaled fonts (less lines displayed) and allow joystick input
    D3DCOLOR	Color;
    int			a;
    float		yoffset;
    static float cursorcount = 0.0f;
    int scale_factor = pDefaultFont->GetScaleFactor();
    int line_spacing = 12 * scale_factor;
    //a = int (its_Alpha * 8 / 9);  //DKS - This line was commented out in original source
    a = int (its_Alpha);

    yoffset = - 255.0f + its_Alpha - 1.0f;

    Color = D3DCOLOR_RGBA(255, 255, 255, a);
    ConsoleGFX.RenderSpriteScaled(0, yoffset, 640, 256, Color);

    // Text anzeigen
    for (int i=MAX_LINES-ConsoleLines, line_pos=0; i<MAX_LINES; i++, line_pos++)
    {
        a = int (its_Alpha * 8 / 9);
        Color = D3DCOLOR_RGBA(128, 128, 255, a);
        pDefaultFont->DrawText(26, yoffset + float(10 + line_pos * line_spacing), Text[i], Color);
    }

    char Temp[105];

    strcpy_s(Temp, 2, ">");
    strcat_s(Temp, Buffer);

    // Cursor animieren
    cursorcount += 0.3f SYNC;

    if (cursorcount >= 2.0f)
        cursorcount = 0.0f;

    Color = D3DCOLOR_RGBA(255, 255, 255, a);
    pDefaultFont->DrawText(26 - 6 * scale_factor, yoffset +  10 + ConsoleLines * line_spacing, Temp, Color);

    // Draw cursor block
    int text_width = pDefaultFont->StringLength(Temp);
    int cursor_x = 20 + text_width;
    if (scale_factor > 1) 
        cursor_x -= 6;
    int cursor_y = yoffset + 10 + ConsoleLines * line_spacing;
    int cursor_h = 12 * scale_factor;
    int cursor_w = 9 * scale_factor;

    switch (int (cursorcount))
    {
    case 0 :
        pDefaultFont->DrawText(cursor_x, cursor_y, CursorChar, Color);
        RenderRect(cursor_x, cursor_y, cursor_w, cursor_h, D3DCOLOR_RGBA(255,255,255,140));
        break;

    case 1 :
        pDefaultFont->DrawText(cursor_x, cursor_y, CursorChar, Color);
        break;

    default :
        break;
    }
    
} // ShowConsole

// --------------------------------------------------------------------------------------
// Cheat eingegeben?
// --------------------------------------------------------------------------------------

bool ConsoleClass::CONSOLE_CHEAT(char *cheat)
{
    bool result = false;

    // TODO FIX
    result = CONSOLE_COMMAND(convertText(cheat));

    if (result)
    {
        SoundManager.PlayWave(100, 128, 15000, SOUND_MESSAGE);
        Player[0].Score = 0;
        HasCheated = true;
    }

    return result;
}

// --------------------------------------------------------------------------------------
// Text checken, wenn Enter gedrückt wurde
// --------------------------------------------------------------------------------------

void ConsoleClass::CheckCommands(void)
{
    // Hilfe
    //
    //DKS - Re-enabled help, as it seems to be just fine
//#ifdef _DEBUG
    // TODO FIX
    		if (CONSOLE_COMMAND("help"))
    		{
    			CONSOLE_PRINT(" ");
    			CONSOLE_PRINT("Console Commands");
    			CONSOLE_PRINT("-------------------------");
    			CONSOLE_PRINT(" ");
    			CONSOLE_PRINT("clear - clear console");
    			CONSOLE_PRINT("levelinfo - show level information");
    			CONSOLE_PRINT("maxfps x - set maximum framerate to 'x' (0=no maximum)");
    			CONSOLE_PRINT("setspeed x - Set Game-speed (default=10)");
    			CONSOLE_PRINT("loadmap 'name' - load level 'name'");
    			CONSOLE_PRINT("minimap - save minimap to disc");
    			CONSOLE_PRINT("quit - Quit game immediately");
    		} else 

    // Konsole löschen
    if (CONSOLE_COMMAND("clear"))
    {
        for (int i=0; i<MAX_LINES; i++)
            strcpy_s(Text[i], "");

        strcpy_s(Buffer, "");
        CursorChar[0] = ' ';
    }
    else

        // Levelinfo anzeigen
        if (CONSOLE_COMMAND("levelinfo"))
        {
            char StringBuffer[100];
            char temp[50];

            CONSOLE_PRINT(" ");
            CONSOLE_PRINT("Levelinfo");
            CONSOLE_PRINT("-------------");
            CONSOLE_PRINT(" ");

            CONSOLE_PRINT(TileEngine.Beschreibung);

            _itoa_s((int)(TileEngine.XOffset), temp, 10);
            strcpy_s(StringBuffer, "Level Offset X : ");
            strcat_s(StringBuffer, temp);
            CONSOLE_PRINT(StringBuffer);

            _itoa_s((int)(TileEngine.YOffset), temp, 10);
            strcpy_s(StringBuffer, "Level Offset Y : ");
            strcat_s(StringBuffer, temp);
            CONSOLE_PRINT(StringBuffer);

            _itoa_s((int)(TileEngine.LEVELSIZE_X), temp, 10);
            strcpy_s(StringBuffer, "Level Size X : ");
            strcat_s(StringBuffer, temp);
            CONSOLE_PRINT(StringBuffer);

            _itoa_s((int)(TileEngine.LEVELSIZE_Y), temp, 10);
            strcpy_s(StringBuffer, "Level Size Y : ");
            strcat_s(StringBuffer, temp);
            CONSOLE_PRINT(StringBuffer);
        }
        else

            // Spiel sofort verlassen
            if (CONSOLE_COMMAND("quit"))
            {
                CONSOLE_PRINT("Shutting down ...");
                GameRunning = false;
            }
            else


                // Volle Extrawaffen
                if (CONSOLE_CHEAT(Cheats[CHEAT_EXTRAS]))
                {
                    for (int p = 0; p < NUMPLAYERS; p++)
                    {
                        Player[p].PowerLines = 999;
                        Player[p].Grenades   = 999;
                        Player[p].SmartBombs = 999;
                    }

                    CONSOLE_PRINT("-> Let's rock!");
                }
                else

                    // Volle Zeit
                    if (CONSOLE_CHEAT(Cheats[CHEAT_ZEIT]))
                    {
                        TileEngine.Timelimit = 999;

                        CONSOLE_PRINT("-> knowing that there's no rhyme...");
                    }
                    else

                        // Volle Waffen
                        if (CONSOLE_CHEAT(Cheats[CHEAT_WAFFEN]))
                        {
                            for (int p = 0; p < NUMPLAYERS; p++)
                            {
                                Player[p].CurrentWeaponLevel[3] = 16;

                                for (int w = 0; w <= 2; w++)
                                    Player[p].CurrentWeaponLevel[w] = 8;
                            }

                            CONSOLE_PRINT("-> Blast Off and Strike The Evil Bydo Empire!");
                        }
                        else

                            // Schild
                            if (CONSOLE_CHEAT(Cheats[CHEAT_SCHILD]))
                            {
                                for (int i = 0; i < NUMPLAYERS; i++)
                                {
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
                            }
                            else

                                // Dauerfeuer
                                if (CONSOLE_CHEAT(Cheats[CHEAT_SUPERSHOT]))
                                {
                                    for (int i = 0; i < NUMPLAYERS; i++)
                                        Player[i].RiesenShotExtra += 500;

                                    strcpy_s(Buffer, "-> Supershot");
                                }
                                else

                                    // Dauerfeuer
                                    if (CONSOLE_CHEAT(Cheats[CHEAT_AUTOFIRE]))
                                    {
                                        for (int i = 0; i < NUMPLAYERS; i++)
                                            Player[i].AutoFireExtra += 500;

                                        strcpy_s(Buffer, "-> Autofire");
                                    }
                                    else


                                        // Flammenwerfer Mode
                                        if (CONSOLE_CHEAT(Cheats[CHEAT_FLAMER]))
                                        {
                                            FlameThrower = !FlameThrower;

                                            if (FlameThrower)
                                            {
                                                CONSOLE_PRINT("-> Flamethrower on");
                                            }
                                            else
                                            {
                                                CONSOLE_PRINT("-> Flamethrower off");
                                            }
                                        } //else

    // GodMode
    if (CONSOLE_CHEAT(Cheats[CHEAT_GOD]))
    {
        if (Player[0].GodMode == false)
        {
            Player[0].GodMode = true;
            Player[1].GodMode = true;
            CONSOLE_PRINT("-> Godmode on");
        }
        else
        {
            Player[0].GodMode = false;
            Player[1].GodMode = false;
            CONSOLE_PRINT("-> Godmode off");
        }
    } //else

    // GodMode
    if (CONSOLE_CHEAT(Cheats[CHEAT_RAD]))
    {
        if (Player[0].WheelMode == false)
        {
            Player[0].WheelMode = true;
            Player[1].WheelMode = true;
            CONSOLE_PRINT("-> WheelMode on");
        }
        else
        {
            Player[0].WheelMode = false;
            Player[1].WheelMode = false;
            CONSOLE_PRINT("-> WheelMode off");
        }
    } //else

    // max FPS setzen
    if (strncmp(Buffer, "maxfps ", 7) == 0)
    {

        // Bis zu der Zahl vorgehen
        int index1 = 7;
        int g_test = 0;

        while(Buffer[index1] !='\0')
        {
            if(Buffer[index1]>=48 && Buffer[index1]<=58)
            {
                g_test*=10;
                g_test+=Buffer[index1]-48;
            }
            index1++;
        }

        // Meldung ausgeben
        char dummy[50];

        _itoa_s(g_test, dummy, 10);
        strcpy_s(Buffer, "Setting maximum Framerate to ");
        strcat_s(Buffer, dummy);
        strcat_s(Buffer, " ...");
        CONSOLE_PRINT(Buffer);

        // und FPS Setzen
        Timer.SetMaxFPS(g_test);
    }

//#ifdef _DEBUG
    // Speed setzen
    if (strncmp(Buffer, "setspeed ", 9) == 0)
    {
        // Bis zu der Zahl vorgehen
        int index1 = 9;
        int g_test = 0;

        while(Buffer[index1] !='\0')
        {
            if(Buffer[index1]>=48 && Buffer[index1]<=58)
            {
                g_test*=10;
                g_test+=Buffer[index1]-48;
            }
            index1++;
        }

        // Meldung ausgeben
        char dummy[50];

        _itoa_s(g_test, dummy, 10);
        strcpy_s(Buffer, "Setting Speed to ");
        strcat_s(Buffer, dummy);
        strcat_s(Buffer, " ...");
        CONSOLE_PRINT(Buffer);

        // und Speed Setzen
        Timer.SetMoveSpeed(float(atoi(dummy)));
    }
//#endif


    // Stage laden
    if (strncmp(Buffer, "loadmap ", 8) == 0)
    {
        char dummy[255], dummy2[255];

        strcpy_s(dummy, _strrev (Buffer));
        strncpy_s (dummy2, dummy, strlen (dummy) - 8);
        dummy2[strlen (dummy) - 8] = 0;
        strcpy_s(dummy, _strrev (dummy2));

        // Meldung ausgeben
        strcpy_s(Buffer, "Loading Level ");
        strcat_s(Buffer, dummy);
        strcat_s(Buffer, " ...");
        CONSOLE_PRINT(Buffer);

        // und Level laden
        FILE *Datei = NULL;
        char name[100];

        strcpy_s(name, g_storage_ext );
        strcat_s(name, "/data/levels/");    //DKS - Changed from /data/ to /data/levels/
        strcat_s(name, dummy);

        // .map anhängen
        if (dummy2[strlen (dummy2) - 1] != 'p' ||
                dummy2[strlen (dummy2) - 2] != 'a' ||
                dummy2[strlen (dummy2) - 3] != 'm')
        {
            strcat_s(name, ".map");
            strcat_s(dummy, ".map");
        }

        fopen_s(&Datei, name, "rb");

        // Datei gefunden? Dann laden
        if (Datei)
        {
            for (int i = 0; i < NUMPLAYERS; i++)
                Player[i].InitNewLevel();

            fclose(Datei);

            CONSOLE_PRINT("found.");
            TileEngine.LoadLevel(dummy);
            Protokoll.WriteText(false, dummy);
        }

        // Ansonsten Fehlermeldung
        else
        {
            sprintf_s(dummy2,  "Map %s not found !", name);
            CONSOLE_PRINT(dummy2);
        }
    }

    // Minimap anzeigen und Screenshot machen
    if (strncmp(Buffer, "minimap", 7) == 0)
    {
        // Darstellung beenden
#if defined(PLATFORM_DIRECTX)
        lpD3DDevice->EndScene();
#endif
        DirectGraphics.ShowBackBuffer();

        // Mit dem Darstellen beginnen
#if defined(PLATFORM_DIRECTX)
        lpD3DDevice->BeginScene();
#endif

        // Screen schwarz färben
        RenderRect(0, 0, 640, 480, 0xFF000000);

        // Mini Map anzeigen
        for (int i=0; i < TileEngine.LEVELSIZE_X; i++)
            for (int j=0; j < TileEngine.LEVELSIZE_Y; j++)
            {
                if (TileEngine.Tiles[i][j].Block & BLOCKWERT_WAND)		  RenderRect((float)i, (float)j, 1, 1, 0xFFFFFFFF);
                if (TileEngine.Tiles[i][j].Block & BLOCKWERT_DESTRUCTIBLE) RenderRect((float)i, (float)j, 1, 1, 0xFFFFFF00);
                if (TileEngine.Tiles[i][j].Block & BLOCKWERT_PLATTFORM)	  RenderRect((float)i, (float)j, 1, 1, 0xFF888888);
                if (TileEngine.Tiles[i][j].Block & BLOCKWERT_WASSER)		  RenderRect((float)i, (float)j, 1, 1, 0xFF0000FF);
            }

        char buf[100], buf2[100];

        strcpy_s(buf, "Level ");
        _itoa_s(Stage, buf2, 10);
        strcat_s(buf, buf2);

        pDefaultFont->DrawText(3, 458, buf, 0xFF00FF00);
        pDefaultFont->DrawText(3, 470, TileEngine.Beschreibung, 0xFF00FF00);

#if defined(PLATFORM_DIRECTX)
        lpD3DDevice->EndScene();						// Darstellung beenden
#endif
        DirectGraphics.ShowBackBuffer();

        // Screenshot machen
        DirectGraphics.TakeScreenshot("MiniMap", 640, 480);
        CONSOLE_PRINT("Mini Map saved !");
    }

    // Demo aufnehmen
    if (CONSOLE_COMMAND("record demo"))
    {
        NewDemo ("Demo.dem");
        CONSOLE_PRINT("Recording demo");
        this->Activate = false;
        this->Fade   = -25.0f;
        this->Active = false;
        this->Showing= false;
    } //else

    // Demo anhalten
    if (CONSOLE_COMMAND("stop demo"))
    {
        EndDemo ();
        CONSOLE_PRINT("Demo stopped");
    } //else

    // Demo laden und abspielen
    if (CONSOLE_COMMAND("load demo"))
    {
        if (LoadDemo ("Demo.dem") == false)
        {
            CONSOLE_PRINT("Error loading demo");
        }
        else
        {
            CONSOLE_PRINT("Playing demo");
            this->Activate = false;
            this->Fade   = -25.0f;
            this->Active = false;
            this->Showing= false;
        }
    } //else

    // 0 und 1 Render Mode anschalten
    if (CONSOLE_COMMAND("binary"))
    {
        if (this->RenderBinary == false)
        {
            CONSOLE_PRINT("Rendering Binary");
            this->RenderBinary = true;
        }
        else
        {
            CONSOLE_PRINT("Rendering Normal");
            this->RenderBinary = false;
        }
    } //else


#ifdef _DEBUG
    // In Level warpen
    if (strncmp(Buffer, "goto ", 5) == 0)
    {
        // Bis zu der Zahl vorgehen
        int index1 = 5;
        int g_test = 0;

        while(Buffer[index1] !='\0')
        {
            if(Buffer[index1]>=48 && Buffer[index1]<=58)
            {
                g_test*=10;
                g_test+=Buffer[index1]-48;
            }
            index1++;
        }

        // Level > 0? Dann dorthin springen
        //
        if (g_test >= MAX_LEVELS)
        {
            CONSOLE_PRINT("Level does not exist");
        }
        else if (g_test > 0)
        {
            this->Activate = false;
            this->Fade   = -25.0f;
            this->Active = false;
            this->Showing= false;

            InitNewGame ();

            for (int p = 0; p < NUMPLAYERS; p++)
            {
                for (int i=0; i<MAX_AKTIONEN; i++)
                    Player[p].Aktion[i] = false;

                Stage    = g_test;
                NewStage = g_test;
            }

            InitNewGameLevel (Stage);
        }
    } //else
#endif

    if (CONSOLE_COMMAND("light cool"))
    {
        TileEngine.ComputeCoolLight();
    }

    //DKS - ComputeShitLight() was never used in the original game except for here; disabling it.
    //if (CONSOLE_COMMAND("light shit"))
    //{
    //    TileEngine.ComputeShitLight();
    //}

    if (CONSOLE_COMMAND("lamp"))
    {
        if (TileEngine.bDrawShadow == false)
        {
            TileEngine.bDrawShadow = true;
            ShadowAlpha = 255.0f;
        }
        else
            TileEngine.bDrawShadow = false;
    }

    //strcpy_s(Buffer, "Error : Unknown Command !");

} // CheckCommands

// --------------------------------------------------------------------------------------
// Eine Zeile hochscrollen
// --------------------------------------------------------------------------------------

void ConsoleClass::ScrollUp(void)
{
    // Zeilen eins hochscrollen
    //DKS - Fixed overwriting of the lines buffer by one line:
    /*for (int i = 0; i < MAX_LINES; i++)*/
    for (int i = 0; i < MAX_LINES-1; i++)
        strcpy_s(Text[i], Text[i+1]);

    // Und unterste Zeile mit der letzten eingabe füllen
    /*strcpy_s(Text[MAX_LINES], Buffer);*/
    strcpy_s(Text[MAX_LINES-1], Buffer);
    Buffer[0] = '\0';
    CursorChar[0] = ' ';
} // ScrollUp

// --------------------------------------------------------------------------------------
// Checken ob was eingegeben wurde
// --------------------------------------------------------------------------------------

//DKS - Added joystick support:
void ConsoleClass::CheckInput(void)
{
    static float joy_counter = 0.0f;
    const float joy_delay = 30.0f;     // Only accept joy input once every time counter reaches this value

    joy_counter += 30.0f SYNC;
    if (joy_counter > joy_delay) {
        joy_counter = joy_delay;
    }

    if (joy_counter >= joy_delay && Player[0].ControlType == CONTROLTYPE_JOY && JoystickFound) {
        joy_counter = 0.0f;
        int joy_idx = Player[0].JoystickIndex;
        bool up         = false;
        bool down       = false;
        bool left       = false;
        bool right      = false;

        if (DirectInput.Joysticks[joy_idx].JoystickPOV != -1) {
            // HAT switch is pressed
            if        (DirectInput.Joysticks[joy_idx].JoystickPOV >= 4500 * 1 &&
                       DirectInput.Joysticks[joy_idx].JoystickPOV <= 4500 * 3) {
                right = true;
            } else if (DirectInput.Joysticks[joy_idx].JoystickPOV >= 4500 * 5 &&
                       DirectInput.Joysticks[joy_idx].JoystickPOV <= 4500 * 7) {
                left = true;
            } else if (DirectInput.Joysticks[joy_idx].JoystickPOV >  4500 * 3 &&
                       DirectInput.Joysticks[joy_idx].JoystickPOV <  4500 * 5) {
                down = true;
            } else if ((DirectInput.Joysticks[joy_idx].JoystickPOV >  4500 * 7 && 
                        DirectInput.Joysticks[joy_idx].JoystickPOV <= 4500 * 8) ||
                       (DirectInput.Joysticks[joy_idx].JoystickPOV >= 0        &&
                        DirectInput.Joysticks[joy_idx].JoystickPOV < 4500 * 1)) {
                up = true;
            }
        } else if (DirectInput.Joysticks[joy_idx].JoystickX >  Player[0].JoystickSchwelle) {
            right = true;
        } else if (DirectInput.Joysticks[joy_idx].JoystickX < -Player[0].JoystickSchwelle) {
            left  = true;
        } else if (DirectInput.Joysticks[joy_idx].JoystickY >  Player[0].JoystickSchwelle) {
            down  = true;
        } else if (DirectInput.Joysticks[joy_idx].JoystickY < -Player[0].JoystickSchwelle) {
            up  = true;
        }

        if (right) {
            strcat_s(Buffer, CursorChar);
        } else if (left) {
            int buf_len = strlen(Buffer);
            if (buf_len >= 1) {
                Buffer[buf_len-1] = '\0';
            }
        } else if (up) {
            // Chars 97-122 are a..z, Chars 48-57 are 0..9, Space is 32
            if (CursorChar[0] == ' ') {
                CursorChar[0] = 'a';
            } else if (CursorChar[0] == 'z') {
                CursorChar[0] = '0';    // Number '0'
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
                CursorChar[0] = ' ';    // Number '0'
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
                
    for (int i = 0; i < 256; i++)
    {
        if (KeyDown(i) &&
                Pressed[i] == false)
        {
            SoundManager.PlayWave(100, 128, 15000, SOUND_CLICK);

            Pressed[i] = true;

            // Zeichen anhängen
            if (strlen(GetKeyName(i)) <= 1 &&
                    strlen(Buffer) < MAX_CHARS)
            {
                char buf[50];
                strcpy_s(buf, GetKeyName(i));

                // unwandeln in kleinbuchstaben
                if (!KeyDown(DIK_LSHIFT))
                {
                    char c;
                    for (int i=0; i < 50; i++)
                    {
                        c = buf[i];
                        if (c >= 65 &&
                                c <= 90)
                            c += 32;
                        buf[i] = c;
                    }
                }

                strcat_s(Buffer, buf);
            }

            // Space anhängen
            if (i == DIK_SPACE)
                strcat_s(Buffer, " ");

            // Backspace
            if (i == DIK_BACK && strlen(Buffer) > 0)
            {
                char Temp[MAX_CHARS];

                strcpy_s(Temp, Buffer);
                strcpy_s(Buffer, "");
                strncat_s(Buffer, Temp, strlen(Temp) - 1);
            }

            // Neue Zeile
            if (i == DIK_RETURN)
            {
                if (strlen(Buffer) > 0)
                {
                    CheckCommands();
                    ScrollUp();
                }
            }
        }

        // Gedrückte Tasten locken
        if (!KeyDown(i))
            Pressed[i] = false;
    }
} // CheckInput

// --------------------------------------------------------------------------------------
// Konsole zeigen
// --------------------------------------------------------------------------------------

void ConsoleClass::Open(void)
{
    Fade   = 25.0f;
    Active = true;
    Showing = true;
    strcpy_s(Buffer, "");
    CursorChar[0] = ' ';
}

// --------------------------------------------------------------------------------------
// Konsole verstecken
// --------------------------------------------------------------------------------------

void ConsoleClass::Hide(void)
{
    Fade   = -25.0f;
    Active = false;
}

// --------------------------------------------------------------------------------------
// Konsole ablaufen lassen
// --------------------------------------------------------------------------------------

bool ConsoleClass::DoConsole(void)
{
#if defined(GCW)
    // On GCW Zero, the Home key (Power Slider switch) is mapped to the console as well as the Tab key.
    // Konsole ist aktivierbar
    if (!KeyDown(DIK_TAB) && !KeyDown(DIK_HOME))
        Activate = true;

    // Konsole wird aktiviert oder deaktiviert ?
    if ((KeyDown(DIK_TAB) || KeyDown(DIK_HOME)) && Activate == true)
    {
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
    if (KeyDown(DIK_TAB) && Activate == true)
    {
        Activate = false;

        if (Active == true)
            Hide();
        else

            if (Active == false)
                Open();
    }
#endif //GCW

    // Konsole fadet gerade ?
    if (Fade != 0)
        its_Alpha += 2.0f * Fade SYNC;

    // Überlauf verhindern
    if (its_Alpha > 255.0f)
    {
        its_Alpha = 255.0f;
        Fade	  =   0.0f;
    }

    // Konsole ist ausgefadet ?
    if (its_Alpha < 0.0f)
    {
        its_Alpha = 0.0f;
        Fade	  = 0.0f;
    }

    // Konsole ausgefadet? Dann aussteigen
    if (Active == false &&
            its_Alpha <= 0.0f)
    {
        Showing = false;
        return false;
    }

    // Ansonsten Eingabe checken
    CheckInput();

    // Einheitsmatrix setzen, da Konsole nicht rotiert wird
    //
    D3DXMATRIX matView;
    D3DXMatrixIdentity	 (&matView);
#if defined(PLATFORM_DIRECTX)
    lpD3DDevice->SetTransform(D3DTS_VIEW, &matView);
#elif defined(PLATFORM_SDL)
    g_matView = matView;
#endif

    // Und anzeigen
    ShowConsole();

    SetScreenShake ();

    return true;
} // DoCOnsole
