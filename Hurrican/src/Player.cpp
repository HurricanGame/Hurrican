// Datei : Player.cpp

// --------------------------------------------------------------------------------------
//
// Funktionen für den Spieler
// Tasteneingaben bearbeiten und Spieler entsprechend animieren
// usw
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "Player.hpp"
#include <stdio.h>
#include <string>
#include "Console.hpp"
#include "DX8Font.hpp"
#include "DX8Graphics.hpp"
#include "DX8Input.hpp"
#include "DX8Sound.hpp"
#include "GUISystem.hpp"
#include "Gameplay.hpp"
#include "Gegner_Helper.hpp"
#include "Globals.hpp"
#include "Logdatei.hpp"
#include "Main.hpp"
#include "Menu.hpp"
#include "Outtro.hpp"
#include "Partikelsystem.hpp"
#include "Projectiles.hpp"
#include "Tileengine.hpp"
#include "Timer.hpp"

extern bool DEMORecording;
extern bool DEMOPlaying;

int Stage;     // Aktuelles Level
int NewStage;  // Neues Level
int Skill;     // 0 = easy, 1 = medium, 2 = hard, 3 = Hurrican

constexpr int TUTORIAL_END = 99;

// --------------------------------------------------------------------------------------
// Player-Klasse
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor : Spieler initialisieren und Grafiken laden
// --------------------------------------------------------------------------------------

PlayerClass::PlayerClass() {
    // DKS - New variable PlayerNumber represents which player a class instance
    //      represents. PlayerNumber will be set to 1 if InitPlayer(1) is called,
    //      or will remain 0 as it is here when InitPlayer(0) is called.
    PlayerNumber = 0;  // This will be set to 1 if InitPlayer(1) is called on a class instance.
    SoundOff = 0;      // This will be set to 1 if InitPlayer(1) is called on a class instance.

    // DKS - Added initialization of member vars here, since Main.cpp was just doing a memset to
    //      fill the entire class with 0's and that won't do now that we have the sprites as
    //      member vars.
    AnimCount = 0;
    BlitzStart = 0;
    FlameTime = 0;
    AustrittX = AustrittY = 0;
    AustrittAnim = 0;
    FlameAnim = 0;
    randomwert = 0;
    changecount = 0;
    weaponswitchlock = false;
    GegnerDran = false;
    AlreadyDrawn = false;
    memset(Aktion, 0, sizeof(Aktion));
    memset(AktionKeyboard, 0, sizeof(AktionKeyboard));
    memset(AktionJoystick, 0, sizeof(AktionJoystick));
    Walk_UseAxxis = false;
    Look_UseAxxis = false;
    CanBeDamaged = false;
    GodMode = false;
    WheelMode = false;
    WasDamaged = false;
    memset(&CollideRect, 0, sizeof(CollideRect));
    AufPlattform = nullptr;
    InLiquid = false;
    JumpPossible = false;
    JumpedOnSurfboard = false;
    PowerLinePossible = false;
    PunisherActive = false;
    WalkLock = false;
    AnimPhase = 0;
    AnimEnde = 0;
    GameOverTimer = 0;
    ShotDelay = 0;
    AutoFireCount = 0;
    AutoFireExtra = 0;
    RiesenShotExtra = 0;
    xposold = yposold = 0;
    xpos = ypos = 0;
    xspeed = yspeed = 0;
    xadd = yadd = 0;
    BeamX = BeamY = 0;
    BeamCount = 0;
    AutoScrollspeed = 0;
    JumpySave = 0;
    JumpxSave = 0;
    JumpStart = 0;
    JumpAdd = 0;
    SmokeCount = 0;
    BlitzWinkel = 0;
    BlitzCount = 0;
    BlitzAnim = 0;
    Handlung = PlayerActionEnum::STEHEN;
    Blickrichtung = 0;
    Energy = 0;
    Armour = 0;
    Shield = 0;
    DamageCounter = 0;
    BlinkCounter = 0;
    BlinkColor = 0;
    CurrentColor = 0;
    Score = 0;
    CollectedDiamonds = 0;
    DiamondsThisLevel = 0;
    DiamondsFullGame = 0;
    LivesThisLevel = 0;
    LivesFullGame = 0;
    BlocksThisLevel = 0;
    BlocksFullGame = 0;
    Lives = 0;
    SelectedWeapon = 0;
    memset(CurrentWeaponLevel, 0, sizeof(CurrentWeaponLevel));
    BlitzLength = 0;
    PowerLines = 0;
    Grenades = 0;
    SmartBombs = 0;
    SecretThisLevel = 0;
    SecretFullGame = 0;
    BronsonCounter = 0;
    FesteAktion = 0;
    DoFesteAktion = false;
    StageClearRunning = false;
    JoystickIndex = 0;
    JoystickSchwelle = 0;
    ControlType = 0;
    JoystickMode = 0;
    ExplodingTimer = 0;

    SpritesLoaded = false;  // DKS - Added this bool so sprites can be loaded on-demand

    // DKS - All the rest of the following were already present in the constructor, but
    //      likely having no effect at all because the game was memsetting each player
    //      class to 0 after allocating each player with new(). I hope there are no
    //      unintended consequences of leaving these at the bottom of the constructor,
    //      where they will now have an effect.
    Stage = -1;  // Es läuft noch kein Spiel
    NewStage = -1;
    StageClearRunning = false;
    JoystickSchwelle = 500.0f;
    ControlType = CONTROLTYPE_KEYBOARD;
    JoystickMode = JOYMODE_JOYPAD;
    JoystickIndex = 0;
    GodMode = false;
    WheelMode = false;
    ExplodingTimer = 0.0f;
    BeamCount = 0.0f;
    changecount = 0.0f;
    memset(&strahlen, 0, sizeof(strahlen));
}

// --------------------------------------------------------------------------------------
// Spieler auf neues Spiel vorbereiten
// --------------------------------------------------------------------------------------

// DKS - InitPlayer now takes an argument the class uses to set which player number
//      a specific class instance is. 0 for Player1, 1 for Player2.
void PlayerClass::InitPlayer(int player_num) {
    if (player_num == 1) {
        PlayerNumber = 1;
        SoundOff = 1;
    } else {
        PlayerNumber = 0;
        SoundOff = 0;
    }

    // DKS - Load each player's individual set of sprites (Player 1's has red highlights,
    //      Player 2 has blue highlights).
    LoadSprites();

    BronsonCounter = 0.0f;
    Score = 0;
    Stage = 1;
    NewStage = 1;
    Lives = 3;
    Armour = MAX_ARMOUR;
    Energy = MAX_ENERGY;
    yspeed = 0.0f;
    yadd = 0.0f;

    // Waffen initialisieren
    PowerLines = 3;
    Grenades = 3;
    SmartBombs = 1;
    SelectedWeapon = 0;

    CollectedDiamonds = 0;

    int i = 0;
    /*
        for(i=0; i<4; i++)
            CollectedPowerUps[i] = 0;
    */

    for (i = 0; i < 4; i++) {
        CurrentWeaponLevel[i] = 0;
        //		NextWeaponLevel	  [i] = 3;
    }

    CurrentWeaponLevel[0] = 1;

    // Blitz
    BlitzLength = 3;            // Aktuelle
    CurrentWeaponLevel[3] = 2;  // Länge
    // NextWeaponLevel	  [3] = 3;			// Wieviel PowerUps zum nächsten Level ?

    AutoFireCount = 0.0f;
    AutoFireExtra = 0.0f;
    RiesenShotExtra = 0.0f;

    // Spielt der Spieler das TutorialLevel? Dann alle Extra-Waffen auf Null setzen
    //
    if (RunningTutorial) {
        Skill = SKILL_EASY;
        PowerLines = 0;
        Grenades = 0;
        SmartBombs = 0;
    }

    GodMode = false;
    WheelMode = false;
    InitNewLevel();
    AutoScrollspeed = 0.0f;
}

// --------------------------------------------------------------------------------------
// Spieler auf neues Level vorbereiten
// --------------------------------------------------------------------------------------

void PlayerClass::InitNewLevel() {
    if (Handlung == PlayerActionEnum::TOT)
        return;

    BronsonCounter = 0.0f;
    Handlung = PlayerActionEnum::STEHEN;
    JumpAdd = 0.0f;
    AnimPhase = 0;
    AnimCount = 0.0f;
    xspeed = 0.0f;
    yspeed = 0.0f;
    xadd = 0.0f;
    yadd = 0.0f;
    Blickrichtung = RECHTS;
    JumpPossible = true;
    CollideRect.left = 20;
    CollideRect.top = 12;
    CollideRect.right = 50;
    CollideRect.bottom = 80;
    Energy = MAX_ENERGY;
    Armour = MAX_ARMOUR;

    // Waffen initialisieren
    ShotDelay = 0.0f;
    BlitzWinkel = 0.0f;
    BlitzCount = 0.0f;
    BlitzAnim = 0;
    PowerLinePossible = true;

    AufPlattform = nullptr;
    InLiquid = false;
    JumpedOnSurfboard = false;

    DamageCounter = 0.0f;
    Shield = 0.0f;

    GameOverTimer = 0.0f;

    FlameTime = 0.0f;

    SecretThisLevel = 0;
    DiamondsThisLevel = 0;
    LivesThisLevel = 0;
    BlocksThisLevel = 0;

    CanBeDamaged = true;
    WalkLock = true;

    FesteAktion = -1;
    DoFesteAktion = false;
    StageClearRunning = false;

    // Zu Beginn des Levels werden alle Listen mit Gegner, Schüssen und Partikeln gelöscht
    PartikelSystem.ClearAll();
    Gegner.ClearAll();
    Projectiles.ClearAll();
}

// --------------------------------------------------------------------------------------
// Spieler explodiert gerade
// --------------------------------------------------------------------------------------

void PlayerClass::runExplode() {
    ExplodingTimer -= 1.0f SYNC;

    // Zuende explodiert?
    //
    if (ExplodingTimer <= 0.0f) {
        // Rechteck wieder herstellen
        //
        CollideRect.left = 20;
        CollideRect.top = 12;
        CollideRect.right = 50;
        CollideRect.bottom = 79;

        // Dann ein Leben abziehen
        Lives--;
        if (Lives < 0)
            Handlung = PlayerActionEnum::TOT;

        // Aktuell eingestellte Waffe um eine Stufe verringern
        //
        if (CurrentWeaponLevel[SelectedWeapon] > 1)
            CurrentWeaponLevel[SelectedWeapon]--;

        // Blitz um eine Stufe verkürzen
        //
        if (CurrentWeaponLevel[3] > 2)
            CurrentWeaponLevel[3]--;

        // Powerlines wieder minimal auf Spieleranzahl setzen
        if (PowerLines < NUMPLAYERS)
            PowerLines = NUMPLAYERS;

        // Granaten wieder minimal auf Spieleranzahl setzen
        if (Grenades < NUMPLAYERS)
            Grenades = NUMPLAYERS;

        // DKS - This was commented out in original source
        // SmartBombs wieder minimal auf 1 setzen
        // if (SmartBombs < 1)
        //	SmartBombs = 1;

        if (TileEngine.Zustand == TileStateEnum::SCROLLBAR &&
                Handlung != PlayerActionEnum::SACKREITEN &&
                Handlung != PlayerActionEnum::DREHEN) {
            xpos = JumpxSave;  // Alte Position wieder herstellen, wenn der
            ypos = JumpySave;  // der Spieler zB einen Abgrund runterfiel
            xposold = JumpxSave;
            yposold = JumpySave;
        }

        // Spieler hat Game Over ?
        if (Player[0].Lives < 0 && (NUMPLAYERS == 1 || Player[1].Lives < 0)) {
            Player[0].Lives = -1;
            Player[1].Lives = -1;
            Player[0].GameOverTimer = 50.0f;

            // DKS - We should really just stop all songs (it was missing MUSIC_PUNISHER from this list anyway)
            // SoundManager.StopSong(MUSIC_STAGEMUSIC, false);
            // SoundManager.StopSong(MUSIC_FLUGSACK, false);
            // SoundManager.StopSong(MUSIC_BOSS, false);
            SoundManager.StopSongs();  // DKS - Added this to replace above 3 lines

            SoundManager.PlaySong(MUSIC_GAMEOVER, false);
            pMenu->AktuellerZustand = 0;
        }

        // oder wird wieder gespanwed
        else if (Lives >= 0) {
            // Spieler hat kurze Zeit lang Schild
            // if (Shield <= 0.0f)
            Projectiles.PushProjectile(xpos, ypos, SHIELDSPAWNER, this);
            Projectiles.PushProjectile(xpos, ypos, SHIELDSPAWNER2, this);

            Shield += 20.0f;

            // Energie und Rad-Energie wieder auf voll setzen
            Energy = MAX_ENERGY;
            Armour = MAX_ARMOUR;

            // Im Fahrstuhllevel? Dann auf dem Fahrstuhl wieder anfangen
            //
            if (g_Fahrstuhl_yPos > -1.0f) {
                xpos = TileEngine.XOffset + 320.0f - 35;
                ypos = g_Fahrstuhl_yPos - CollideRect.bottom - 1;
                xposold = xpos;  // Alte Position wieder herstellen, wenn der
                yposold = ypos;  // der Spieler zB einen Abgrund runterfiel
                JumpxSave = xpos;
                JumpySave = ypos;
            }
        }

        if (Lives < -1)
            Lives = -1;

        if (Handlung != PlayerActionEnum::SACKREITEN &&
                Handlung != PlayerActionEnum::DREHEN &&
                Handlung != PlayerActionEnum::TOT)
            Handlung = PlayerActionEnum::SPRINGEN;

        yspeed = 0.0f;
        JumpAdd = 0.0f;
    }
}

// --------------------------------------------------------------------------------------
// will der Spieler schiessen?
// --------------------------------------------------------------------------------------

void PlayerClass::checkShoot() {
    // DKS - No need to check if sound is playing, just ask to stop it:
    // if (!Aktion[AKTION_SHOOT] &&
    //        FlameThrower == true &&
    //        SoundManager.its_Sounds[SOUND_FLAMETHROWER + SoundOff]->isPlaying)
    //    SoundManager.StopWave(SOUND_FLAMETHROWER + SoundOff);
    if (!Aktion[AKTION_SHOOT] && FlameThrower)
        SoundManager.StopWave(SOUND_FLAMETHROWER + SoundOff);

    // Normaler Schuss mit Primärwaffe?
    //
    if (Aktion[AKTION_SHOOT] && Handlung != PlayerActionEnum::BLITZEN &&
            Handlung != PlayerActionEnum::BEAMLADEN &&
            Handlung != PlayerActionEnum::DREHEN &&
            AutoFireCount > 0.0f && ShotDelay <= 0.0f) {
        ShotDelay = PLAYER_SHOTDELAY;
        PlayerShoot();

        if (Handlung != PlayerActionEnum::LAUFEN &&
                Handlung != PlayerActionEnum::SPRINGEN &&
                Handlung != PlayerActionEnum::SACKREITEN &&
                Handlung != PlayerActionEnum::RADELN &&
                Handlung != PlayerActionEnum::RADELN_FALL && WalkLock)
            AnimPhase = 1;

        // TODO immer Dauerfeuer?
        /*// Nur bei "Easy" Dauerfeuer
        // ansonsten Autofire Count verringern
        //
        if (Skill > SKILL_EASY)
            AutoFireCount -= 1.0f;*/

        if (AutoFireExtra > 0.0f)
            ShotDelay /= 2.25f;

        // Beim Sackreiten den Spieler durch den Rückschlag noch bewegen
        //
        if (Handlung == PlayerActionEnum::SACKREITEN) {
            if (Blickrichtung == LINKS)
                xadd += 2.0f;
            else
                xadd -= 2.0f;
        }
    }

    // Granate abfeuern ?
    //
    if (Aktion[AKTION_GRANATE] && PowerLinePossible && Grenades > 0 &&
            Handlung != PlayerActionEnum::RADELN &&
            Handlung != PlayerActionEnum::RADELN_FALL &&
            Handlung != PlayerActionEnum::BLITZEN &&
            Handlung != PlayerActionEnum::DREHEN &&
            Handlung != PlayerActionEnum::BEAMLADEN) {
        PlayerGrenadeShoot();
    }

    // Kann man wieder schiessen ?
    //
    if (!Aktion[AKTION_SHOOT]) {
        ShotDelay = 0.0f;

        if (AutoFireCount < 3.0f)
            AutoFireCount = 3.0f;
    }

    if (ShotDelay > 0.0f)
        ShotDelay -= static_cast<float>(1.0 SYNC);
    else
        ShotDelay = 0.0f;
}

// --------------------------------------------------------------------------------------
// Autofire abhandeln
// --------------------------------------------------------------------------------------

void PlayerClass::handleAutoFire() {
    // Eingesammeltes Autofire runterzählen
    //
    if (AutoFireExtra > 0.0f) {
        AutoFireExtra -= 0.5f SYNC;
        AutoFireCount = 3;
    } else {
        AutoFireExtra = 0.0f;
    }

    // Eingesammeltes RiesenShotExtra runterzählen
    //
    if (RiesenShotExtra > 0.0f)
        RiesenShotExtra -= 0.5f SYNC;
    else
        RiesenShotExtra = 0.0f;
}

// --------------------------------------------------------------------------------------
// Eingabegeräte abfragen und Spieler entsprechend reagieren lassen
// --------------------------------------------------------------------------------------

bool PlayerClass::GetPlayerInput() {
    // tot?
    if (Handlung == PlayerActionEnum::TOT)
        return false;

    // explodiert der Spieler noch?
    //
    if (Handlung == PlayerActionEnum::EXPLODIEREN) {
        runExplode();
        return false;
    }

    // Zuerst alle Aktionen auf false setzen
    //
    for (bool &i : Aktion) {
        i = false;
    }

    // und Bewegungsgeschwindigkeit für den nächsten Frame auf 0 setzen,
    // es sei denn, man läuft auf Eis
    if (TileEngine.BlockUntenNormal(xpos, ypos, xposold, yposold, CollideRect) & BLOCKWERT_EIS) {
        xspeed *= PLAYER_ICESSLOWDOWN;
    } else {
        xspeed = 0.0f;
    }

    // Aktionen nach Keyboard-Input abfragen falls der
    // Spieler sich gerade bewegen darf =)
    //
    //	if (TileEngine.Zustand != ZUSTAND_SCROLLTOPLAYER)
    {
        // Demo läuft ?
        if (DEMOPlaying)
            PlayDemo();

        // oder feste Aktion?
        else if (DoFesteAktion) {
            if (FesteAktion >= 0)
                Aktion[FesteAktion] = true;
        }
        // oder Spieler spielt ?
        else {
            // DKS - Cleaned a lot of this up and made the joystick support more complete,
            //      especially in regards to Walk_UseAxxis and Look_UseAxxis,
            //      along with safeguarding against checking button inputs on actions that
            //      are undefined, and not checking the keyboard for actions unless the
            //      player is set to use it as the control method.

            // Keyboard
            if (ControlType == CONTROLTYPE_KEYBOARD) {
                for (int i = 0; i < MAX_AKTIONEN; i++)
                    if (KeyDown(AktionKeyboard[i])) {
                        Aktion[i] = true;
                        BronsonCounter = 0.0f;
                    }
            } else if (JoystickIndex >= 0 && JoystickIndex < DirectInput.JoysticksFound &&
                       DirectInput.Joysticks[JoystickIndex].Active) {
                bool stick_right = DirectInput.Joysticks[JoystickIndex].JoystickX > JoystickSchwelle;
                bool stick_left = DirectInput.Joysticks[JoystickIndex].JoystickX < -JoystickSchwelle;
                bool stick_up = DirectInput.Joysticks[JoystickIndex].JoystickY < -JoystickSchwelle;
                bool stick_down = DirectInput.Joysticks[JoystickIndex].JoystickY > JoystickSchwelle;
                bool hat_right = false;
                bool hat_left = false;
                bool hat_up = false;
                bool hat_down = false;

                if (DirectInput.Joysticks[JoystickIndex].JoystickPOV != -1) {
                    // HAT switch is pressed
                    if (DirectInput.Joysticks[JoystickIndex].JoystickPOV >= 4500 * 1 &&
                        DirectInput.Joysticks[JoystickIndex].JoystickPOV <= 4500 * 3) {
                        hat_right = true;
                    } else if (DirectInput.Joysticks[JoystickIndex].JoystickPOV >= 4500 * 5 &&
                               DirectInput.Joysticks[JoystickIndex].JoystickPOV <= 4500 * 7) {
                        hat_left = true;
                    }

                    if (DirectInput.Joysticks[JoystickIndex].JoystickPOV >= 4500 * 3 &&
                        DirectInput.Joysticks[JoystickIndex].JoystickPOV <= 4500 * 5) {
                        hat_down = true;
                    } else if ((DirectInput.Joysticks[JoystickIndex].JoystickPOV >= 4500 * 7 &&
                                DirectInput.Joysticks[JoystickIndex].JoystickPOV <= 4500 * 8) ||
                               (DirectInput.Joysticks[JoystickIndex].JoystickPOV >= 0 &&
                                DirectInput.Joysticks[JoystickIndex].JoystickPOV <= 4500 * 1)) {
                        hat_up = true;
                    }
                }

                // Zum laufen die Achse links/rechts auslesen
                if (Walk_UseAxxis) {
                    // Analog stick input used for walking:
                    if (stick_right)
                        Aktion[AKTION_RECHTS] = true;
                    if (stick_left)
                        Aktion[AKTION_LINKS] = true;
                    if (stick_down)
                        Aktion[AKTION_DUCKEN] = true;

                    if (stick_up) {
                        if (JoystickMode == JOYMODE_JOYSTICK) {
                            // Analog stick up-input used for jumping (when in "joystick" mode):
                            // Spieler kann im Joystick Mode nicht mit dem Button springen
                            //
                            Aktion[AKTION_JUMP] = true;
                        } else {
                            // Analog stick up-input used for looking up (when in "joypad" mode)
                            Aktion[AKTION_OBEN] = true;
                        }
                    }
                } else {
                    // HAT/DPAD used for walking:
                    if (hat_right)
                        Aktion[AKTION_RECHTS] = true;
                    if (hat_left)
                        Aktion[AKTION_LINKS] = true;
                    if (hat_down)
                        Aktion[AKTION_DUCKEN] = true;

                    if (hat_up) {
                        if (JoystickMode == JOYMODE_JOYSTICK) {
                            // HAT/DPAD up-input used for jumping (when in "joystick" mode):
                            // Spieler kann im Joystick Mode nicht mit dem Button springen
                            //
                            Aktion[AKTION_JUMP] = true;
                        } else {
                            // HAT/DPAD up-input used for looking up (when in "joypad" mode)
                            Aktion[AKTION_OBEN] = true;
                        }
                    }
                }

                if (Look_UseAxxis) {
                    // Analog stick used for looking:
                    if (stick_down)
                        Aktion[AKTION_UNTEN] = true;
                    else if (stick_up)
                        Aktion[AKTION_OBEN] = true;
                } else {
                    // HAT/DPAD used for looking:
                    if (hat_down)
                        Aktion[AKTION_UNTEN] = true;
                    else if (hat_up)
                        Aktion[AKTION_OBEN] = true;
                }

                // Joystick buttons auslesen
                for (int i = AKTION_LINKS; i <= AKTION_WAFFEN_CYCLE; i++) {
                    if (AktionJoystick[i] != -1 &&
                        DirectInput.Joysticks[JoystickIndex].JoystickButtons[AktionJoystick[i]]) {
                        if (!(i == AKTION_JUMP && JoystickMode == JOYMODE_JOYSTICK))
                            Aktion[i] = true;
                    }
                }
            }

            // Bewegungen aufnehmen ?
            if (DEMORecording)
                RecordDemo();
        }
    }

    // Spieler läuft ins Exit?

    if (Player[0].StageClearRunning)
        RunPlayerExit();

    // Spieler schiesst?
    //
    checkShoot();

    // Autofire und Riesenshot Extra abgandeln
    //
    handleAutoFire();

    // waffe wechseln?
    checkWeaponSwitch();

    for (bool i : Aktion) {
        if (i) {
            if (Handlung == PlayerActionEnum::PISSEN) {
                GUI.HideBoxFast();
                BronsonCounter = 0.0f;
                Handlung = PlayerActionEnum::STEHEN;
            }
        }
    }

    // Bronson-Counter erhöhen
    if ((Handlung == PlayerActionEnum::STEHEN || Handlung == PlayerActionEnum::PISSEN) &&
            !RunningTutorial) {
        BronsonCounter += 1.0f SYNC;
    } else {
        BronsonCounter = 0.0f;

        if (Handlung == PlayerActionEnum::PISSEN)
            Handlung = PlayerActionEnum::STEHEN;
    }

    // und evtl auch mit Bronson loslegen =)
    //
    if (BronsonCounter > 200.0f) {
        if (Handlung != PlayerActionEnum::PISSEN) {
            AnimCount = 0.0f;
            AnimPhase = 0;
            Handlung = PlayerActionEnum::PISSEN;
        }
    }

    return true;
}

// --------------------------------------------------------------------------------------
// Funken und Rauch erzeugen, wenn der Spieler angeschlagen ist
// --------------------------------------------------------------------------------------

void PlayerClass::DoStuffWhenDamaged() {
    if (Console.Showing || Handlung == PlayerActionEnum::EXPLODIEREN)
        return;

    static float sparkcount = 0.0f;
    static float smokecount = 0.0f;

    // Noch viel Energie? Dann gleich wieder raus
    //
    if (Energy > MAX_ENERGY / 2)
        return;

    // Funkenzähler runterzählen
    //
    if (sparkcount > 0.0f)
        sparkcount -= 1.0f SYNC;
    else {
        sparkcount = static_cast<float>(rand() % static_cast<int>(Energy / 2 + 2)) + 5;

        // ein Funken "Schadenseffekt" per Zufall einbauen
        //
        int effect = random(3);

        switch (effect) {
            // Normale Funken
            //
            case 0: {
                float x = xpos + static_cast<float>(20 + random(40));
                float y = ypos + static_cast<float>(20 + random(40));

                for (int i = 0; i < 5; i++)
                    PartikelSystem.PushPartikel(x + random(4), y + random(4), FUNKE);

                PartikelSystem.PushPartikel(x - 20, y - 20, LASERFLAME);
                SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND_FUNKE);
            } break;

            // Lange Funken
            //
            case 1: {
                float x = xpos + static_cast<float>(20 + random(40));
                float y = ypos + static_cast<float>(20 + random(40));

                for (int i = 0; i < 5; i++)
                    PartikelSystem.PushPartikel(x + random(4), y + random(4), LONGFUNKE);

                PartikelSystem.PushPartikel(x - 20, y - 20, LASERFLAME);
                SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND_FUNKE);
            } break;
        }
    }

    // Rauchzähler runterzählen
    //
    if (smokecount > 0.0f)
        smokecount -= 1.0f SYNC;
    else {
        smokecount = 0.8f;

        // Rauch per Zufall
        //
        if (Energy < MAX_ENERGY / 3 + 5.0f)
            if (random(2) == 0)
                PartikelSystem.PushPartikel(xpos + 10 + random(30), ypos + 20 + random(40), SMOKE2);

        // Rauchsäule
        //
        if (Energy < MAX_ENERGY / 4 + 5.0f)
            PartikelSystem.PushPartikel(xpos + 26 + Blickrichtung * 4 + random(4), ypos + 20 + random(4), SMOKE3);
    }
}

// --------------------------------------------------------------------------------------
// Energie testen, ob <= 0. Dann explodieren lassen
// --------------------------------------------------------------------------------------

void PlayerClass::CheckForExplode() {
    if (Energy <= 0.0f &&
            Handlung != PlayerActionEnum::EXPLODIEREN &&
            GameOverTimer == 0.0f) {
        // DKS - No need to check if the sounds are playing, just request they stop:
        // if (SoundManager.its_Sounds[SOUND_ABZUG + SoundOff]->isPlaying == true)
        //    SoundManager.StopWave(SOUND_ABZUG + SoundOff);

        // if (SoundManager.its_Sounds[SOUND_BLITZ + SoundOff]->isPlaying == true)
        //    SoundManager.StopWave(SOUND_BLITZ + SoundOff);

        // if (SoundManager.its_Sounds[SOUND_BEAMLOAD + SoundOff]->isPlaying == true)
        //    SoundManager.StopWave(SOUND_BEAMLOAD + SoundOff);
        SoundManager.StopWave(SOUND_ABZUG + SoundOff);
        SoundManager.StopWave(SOUND_BLITZ + SoundOff);
        SoundManager.StopWave(SOUND_BEAMLOAD + SoundOff);

        DirectInput.Joysticks[JoystickIndex].StopForceFeedbackEffect(FFE_BLITZ);

        // Screen bei evtl. Ruckeln begradigen
        glm::mat4x4 matRot;  // Rotationsmatrix
        WackelMaximum = 0.0f;
        WackelValue = 0.0f;
        matRot = glm::mat4x4(1.0f);
        g_matModelView = matRot * g_matView;
#if defined(USE_GL1)
        load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

        CollideRect.left = 0;
        CollideRect.right = 0;
        CollideRect.top = 0;
        CollideRect.bottom = 0;

        // Spieler explodieren lassen und Gegnern dabei Schaden zufügen
        //
        SoundManager.PlayWave(100, 128, 11025, SOUND_EXPLOSION2);
        Gegner.DamageEnemiesonScreen(xpos + 35, ypos + 40, 400);

        // Piss-Meldung verschwinden lassen
        GUI.HideBoxFast();
        BronsonCounter = 0.0f;

        Shield = 0.0f;

        ShakeScreen(5);

        for (int i = 0; i < 3; i++)
            PartikelSystem.PushPartikel(xpos + 10 + random(50), ypos + 10 + random(50), SPLITTER);

        for (int i = 0; i < 10; i++)
            PartikelSystem.PushPartikel(xpos + 10 + random(50), ypos + 10 + random(50), SPIDERSPLITTER);

        PartikelSystem.PushPartikel(xpos + 35 - 90, ypos + 40 - 90, EXPLOSION_GIGA);
        PartikelSystem.PushPartikel(xpos + 35, ypos + 40, SHOCKEXPLOSION);
        PartikelSystem.PushPartikel(xpos - 20, ypos - 20, EXPLOSIONFLARE);

        // DKS - Player 2 sprite is blue, so I added separate particles and particle art for them
        //      that are colored blue, and PlayerClass now tracks which player it is assigned to.
        if (PlayerNumber == 0) {
            PartikelSystem.PushPartikel(xpos + 20, ypos + 10, HURRITEILE_KOPF);
            PartikelSystem.PushPartikel(xpos + 20, ypos + 10, HURRITEILE_ARM1);
            PartikelSystem.PushPartikel(xpos + 20, ypos + 10, HURRITEILE_ARM2);
            PartikelSystem.PushPartikel(xpos + 20, ypos + 10, HURRITEILE_BEIN1);
            PartikelSystem.PushPartikel(xpos + 20, ypos + 10, HURRITEILE_BEIN2);
            PartikelSystem.PushPartikel(xpos + 20, ypos + 10, HURRITEILE_WAFFE);
            PartikelSystem.PushPartikel(xpos + 20, ypos + 10, HURRITEILE_TORSO);
        } else {
            PartikelSystem.PushPartikel(xpos + 20, ypos + 10, HURRITEILE_P2_KOPF);
            PartikelSystem.PushPartikel(xpos + 20, ypos + 10, HURRITEILE_P2_ARM1);
            PartikelSystem.PushPartikel(xpos + 20, ypos + 10, HURRITEILE_P2_ARM2);
            PartikelSystem.PushPartikel(xpos + 20, ypos + 10, HURRITEILE_P2_BEIN1);
            PartikelSystem.PushPartikel(xpos + 20, ypos + 10, HURRITEILE_P2_BEIN2);
            PartikelSystem.PushPartikel(xpos + 20, ypos + 10, HURRITEILE_P2_WAFFE);
            PartikelSystem.PushPartikel(xpos + 20, ypos + 10, HURRITEILE_P2_TORSO);
        }

        PartikelSystem.PushPartikel(xpos - 88, ypos - 88, GRENADEFLARE);

        ExplodingTimer = 30.0f;

        // Punisher verschwinden lassen
        GegnerClass *pTemp;
        GegnerPunisher *pPunisher;

        pTemp = Gegner.pStart;

        while (pTemp != nullptr) {
            if (pTemp->GegnerArt == PUNISHER) {
                pPunisher = reinterpret_cast<GegnerPunisher *>(pTemp);
                pPunisher->Vanish();
            }

            pTemp = pTemp->pNext;
        }

        if (Handlung == PlayerActionEnum::SACKREITEN || Handlung == PlayerActionEnum::DREHEN) {
            // Von unten wieder hochkommen ?
            if (ypos >= TileEngine.YOffset + 475.0f) {
                ypos = TileEngine.YOffset + 470.0f;
                yadd = -25.0f;
            }

            ExplodingTimer = 0.0f;
            SoundManager.PlayWave(100, 128, 11025, SOUND_EXPLOSION2);

            for (int i = 0; i < 15; i++)
                PartikelSystem.PushPartikel(xpos - 10 + random(80), ypos - 10 + random(80), EXPLOSION_MEDIUM2);

            for (int i = 0; i < 40; i++)
                PartikelSystem.PushPartikel(xpos + random(90), ypos + random(90), LONGFUNKE);
            runExplode();
        } else if (Handlung != PlayerActionEnum::TOT) {
            Handlung = PlayerActionEnum::EXPLODIEREN;
        }

        DirectInput.Joysticks[JoystickIndex].ForceFeedbackEffect(FFE_BIGRUMBLE);
        InLiquid = false;
    }
}

// --------------------------------------------------------------------------------------
// Spieler animieren
// --------------------------------------------------------------------------------------

void PlayerClass::AnimatePlayer() {
    // Pause beim Explodieren
    //
    if (Handlung == PlayerActionEnum::EXPLODIEREN || Handlung == PlayerActionEnum::TOT)
        return;

    static float look;  // hoch / runter sehen

    // DKS - Added so we can limit bounds of accesses to Tiles[][] array here
    int levelsize_x = TileEngine.LEVELSIZE_X;
    int levelsize_y = TileEngine.LEVELSIZE_Y;

    bu = 0;

    bl = TileEngine.BlockLinks(xpos, ypos, xposold, yposold, CollideRect, yspeed >= 0.0f);
    br = TileEngine.BlockRechts(xpos, ypos, xposold, yposold, CollideRect, yspeed >= 0.0f);

    // auf schrägen laufen/gelandet?
    //
    if (yspeed >= 0.0f)
        // DKS - Rewrote BlockSlopes function to only take the parameters it needs:
        // bu = TileEngine.BlockSlopes     (xpos, ypos, xposold, yposold, CollideRect, yspeed);
        bu = TileEngine.BlockSlopes(xpos, ypos, CollideRect, yspeed);

    if (Handlung == PlayerActionEnum::RADELN) {
        if ((Blickrichtung == LINKS && bu & BLOCKWERT_SCHRAEGE_R) ||
            (Blickrichtung == RECHTS && bu & BLOCKWERT_SCHRAEGE_L)) {
            bu = TileEngine.BlockUnten(xpos, ypos, xposold, yposold, CollideRect, yspeed >= 0.0f);
            yspeed = -2.0f;
            ypos -= 1.0f;
        }
    }

    // normaler Boden
    //
    if (!(bu & BLOCKWERT_SCHRAEGE_L) && !(bu & BLOCKWERT_SCHRAEGE_R)) {
        bu = TileEngine.BlockUnten(xpos, ypos, xposold, yposold, CollideRect, yspeed >= 0.0f);
        bu = TileEngine.BlockUntenNormal(xpos, ypos, xposold, yposold, CollideRect);
    } else {
        if (Handlung == PlayerActionEnum::SPRINGEN) {
            Handlung = PlayerActionEnum::STEHEN;
            yspeed = 0.0f;
            JumpAdd = 0.0f;
        }
    }

    bo = TileEngine.BlockOben(xpos, ypos, xposold, yposold, CollideRect, true);

    // Spieler unter Wasserfall ? Dann Wassertropfen entstehen lassen
    // DKS - Added bounds check for access to Tiles[][] array, eliminated divisions while I was here:
#if 0
    for (int i = int (xpos + CollideRect.left) + 5; i < int (xpos + CollideRect.right) - 5; i+=2)
    {
        if (TileEngine.TileAt(int (i / 20), int (ypos + CollideRect.top + 10) / 20).Block & BLOCKWERT_WASSERFALL)
        {
            if (rand()%50 == 0)
                PartikelSystem.PushPartikel (float (i) + rand()%4, ypos + CollideRect.top + rand()%4 + 10, WASSERTROPFEN);

            if (rand()%200 == 0)
                PartikelSystem.PushPartikel (float (i) + rand()%4, ypos + CollideRect.top + rand()%4 + 10, WATERFUNKE);

            if (rand()%200 == 0)
                PartikelSystem.PushPartikel (float (i) + rand()%4 - 16, ypos + CollideRect.top + rand()%4 + 10 - 16, WATERFLUSH);
        }
    }
#endif  // 0
    // DKS - New version of above with bounds checks, divisions converted to multiplies:
    {
        float tmp_y = ypos + (CollideRect.top + 10);
        int tile_y = tmp_y * (1.0f / TILESIZE_Y);
        if (tile_y >= 0 && tile_y < levelsize_y) {
            for (int i = static_cast<int>(xpos) + CollideRect.left + 5; i < static_cast<int>(xpos) + CollideRect.right - 5; i += 2) {
                float tmp_x = i;
                int tile_x = static_cast<float>(i) * (1.0f / TILESIZE_X);
                if (tile_x < 0)
                    continue;
                else if (tile_x >= levelsize_x)
                    break;

                if (TileEngine.TileAt(tile_x, tile_y).Block & BLOCKWERT_WASSERFALL) {
                    if (random(50) == 0)
                        PartikelSystem.PushPartikel(tmp_x + random(4), tmp_y + random(4), WASSERTROPFEN);

                    if (random(200) == 0)
                        PartikelSystem.PushPartikel(tmp_x + random(4), tmp_y + random(4), WATERFUNKE);

                    if (random(200) == 0)
                        PartikelSystem.PushPartikel(tmp_x + (random(4) - 16), tmp_y + (random(4) - 16), WATERFLUSH);
                }
            }
        }
    }

    // Alte Position für Kollisionsabfrage sichern
    xposold = xpos;
    yposold = ypos;

    // Auf Fliessband ?
    if ((bo & BLOCKWERT_FLIESSBANDL || bu & BLOCKWERT_FLIESSBANDL) && !(bl & BLOCKWERT_WAND))
        xpos -= 11.0f SYNC;

    if ((bo & BLOCKWERT_FLIESSBANDR || bu & BLOCKWERT_FLIESSBANDR) && !(br & BLOCKWERT_WAND))
        xpos += 11.0f SYNC;

    //---------------------------------------------------------------------------
    // Spieler im normalen Modus, also kein Rad und nicht auf dem FlugSack reiten
    //---------------------------------------------------------------------------

    if (Handlung != PlayerActionEnum::RADELN &&
            Handlung != PlayerActionEnum::RADELN_FALL &&
            Handlung != PlayerActionEnum::SACKREITEN &&
            Handlung != PlayerActionEnum::SURFEN &&
            Handlung != PlayerActionEnum::SURFENCROUCH &&
            Handlung != PlayerActionEnum::SURFENJUMP &&
            Handlung != PlayerActionEnum::DREHEN) {
        // Stehen animieren?
        if (Handlung == PlayerActionEnum::STEHEN) {
            AnimCount += 1.0f SYNC;

            if (AnimCount > 1.0f) {
                AnimCount -= 1.0f;
                AnimPhase++;

                if (AnimPhase >= (FRAMES_IDLE * 3) + 44 - 1)
                    AnimPhase = 0;
            }
        }

        // Langsam die Rad-Energie wieder auffüllen ?
        if (Armour < 0.0f)
            Armour = 0.0f;

        Armour += 0.2f SYNC;

        // Oder schneller?
        if (Handlung == PlayerActionEnum::STEHEN ||
                Handlung == PlayerActionEnum::DUCKEN ||
                Handlung == PlayerActionEnum::SCHIESSEN_O)
            Armour += 1.0f SYNC;

        if (Armour > MAX_ARMOUR)
            Armour = MAX_ARMOUR;

        // Richtung umkehren wenn an die Decke gestossen ?
        if (bo & BLOCKWERT_WAND || (TileEngine.Zustand == TileStateEnum::LOCKED && ypos <= TileEngine.YOffset)) {
            if (yspeed < 0.0f)
                yspeed = 0.0f;
            JumpAdd = PLAYER_JUMPADDSPEED;
        }

        // Nach Links laufen/springen oder blitzen
        if (Aktion[AKTION_LINKS] &&  // Links gedrückt ?
            !Aktion[AKTION_RECHTS])  // und Rechts nicht ?
        {
            if (Handlung == PlayerActionEnum::BLITZEN)  // Blitzen und dabei den Blitz bewegen ?
            {
                if (BlitzStart >= PLAYER_BLITZ_START)  // Bewegen schon möglich ?
                    BlitzWinkel -= 20 SYNC;
            } else if (Handlung == PlayerActionEnum::BEAMLADEN)  // Rundum bewegen und den Beam aufladen ?
            {
                BlitzWinkel -= 20 SYNC;
            } else if (!(bl & BLOCKWERT_WAND))  // Keine Wand im Weg ?
            {
                Blickrichtung = LINKS;   // nach links kucken
                if (Handlung == PlayerActionEnum::STEHEN)  // Aus dem Stehen heraus
                    // Aktion[AKTION_OBEN]  == false &&	// und nicht nach oben zielen ?
                    // Aktion[AKTION_UNTEN] == false)		// und nicht nach unten zielen ?
                    AnimPhase = 0;  // das laufen starten

                if (Handlung != PlayerActionEnum::SPRINGEN &&  // Nicht in der Luft
                    Handlung != PlayerActionEnum::DUCKEN)      // und nicht ducken ?
                    // Aktion[AKTION_OBEN]  == false &&	// und nicht nach oben zielen ?
                    // Aktion[AKTION_UNTEN] == false)		// und nicht nach unten zielen ?
                    Handlung = PlayerActionEnum::LAUFEN;  // Dann laufen wir =)

                if (Handlung != PlayerActionEnum::DUCKEN) {
                    if (!InLiquid) {
                        // auf Eis?
                        if (bu & BLOCKWERT_EIS) {
                            xspeed -= PLAYER_ICESPEED SYNC;

                            if (xspeed < -PLAYER_MOVESPEED)
                                xspeed = -PLAYER_MOVESPEED;
                        } else

                            // oder normal
                            xspeed = -PLAYER_MOVESPEED;
                    } else
                        xspeed = -PLAYER_MOVESPEED * 2 / 3;  // im Wasser langsamer

                    if (GegnerDran)
                        xspeed *= 0.5f;
                }

                if (Handlung != PlayerActionEnum::SPRINGEN && Aktion[AKTION_OBEN] && WalkLock)

                    xspeed = 0.0f;
            }
        }
        // Nach Rechts laufen/springen oder blitzen
        if (Aktion[AKTION_RECHTS] &&  // Rechts gedrückt ?
            !Aktion[AKTION_LINKS])    // und Links nicht ?
        {
            if (Handlung == PlayerActionEnum::BLITZEN)  // Blitzen und dabei den Blitz bewegen ?
            {
                if (BlitzStart >= PLAYER_BLITZ_START)  // Bewegen schon möglich ?
                    BlitzWinkel += 20 SYNC;
            } else if (Handlung == PlayerActionEnum::BEAMLADEN)  // Rundum bewegen und den Beam aufladen ?
            {
                BlitzWinkel += 20 SYNC;
            } else if (br | BLOCKWERT_WAND)  // Keine Wand im Weg ?
            {
                Blickrichtung = RECHTS;  // nach rechts kucken
                if (Handlung == PlayerActionEnum::STEHEN)  // Aus dem Stehen heraus
                    // Aktion[AKTION_OBEN]  == false &&	// nicht nach oben zielen ?
                    // Aktion[AKTION_UNTEN] == false)		// und nicht nach unten zielen ?
                    AnimPhase = 0;  // das laufen starten

                if (Handlung != PlayerActionEnum::SPRINGEN &&  // Nicht in der Luft
                    Handlung != PlayerActionEnum::DUCKEN)      // und nicht ducken ?
                    // Aktion[AKTION_OBEN]  == false &&		// nicht nach oben zielen ?
                    // Aktion[AKTION_UNTEN] == false)		// und nicht nach unten zielen ?
                    Handlung = PlayerActionEnum::LAUFEN;  // Dann laufen wir =)

                if (Handlung != PlayerActionEnum::DUCKEN) {
                    if (!InLiquid) {
                        // auf Eis?
                        if (bu & BLOCKWERT_EIS) {
                            xspeed += PLAYER_ICESPEED SYNC;

                            if (xspeed > PLAYER_MOVESPEED)
                                xspeed = PLAYER_MOVESPEED;
                        } else

                            // oder normal
                            xspeed = PLAYER_MOVESPEED;
                    } else
                        xspeed = PLAYER_MOVESPEED * 2 / 3;  // im Wasser langsamer

                    if (GegnerDran)
                        xspeed *= 0.5f;
                }

                if (Handlung != PlayerActionEnum::SPRINGEN && Aktion[AKTION_OBEN] && WalkLock)
                    xspeed = 0.0f;
            }
        }

        // Wand im Weg ? Dann stehenbleiben
        if (((Aktion[AKTION_LINKS] && (bl & BLOCKWERT_WAND)) || (Aktion[AKTION_RECHTS] && (br & BLOCKWERT_WAND))) &&
            !(bu & BLOCKWERT_SCHRAEGE_L) && !(bu & BLOCKWERT_SCHRAEGE_R))

        {
            if (Handlung == PlayerActionEnum::LAUFEN) {
                Handlung = PlayerActionEnum::STEHEN;
                AnimPhase = 0;
            }
            xspeed = 0.0f;
        }

        // Rechts/Links gedrückt ? stehenbleiben, da man ja nich rückwärts UND vorwärts gehen kann =)
        if (Aktion[AKTION_LINKS] && Aktion[AKTION_RECHTS]) {
            if ((Handlung != PlayerActionEnum::SPRINGEN || JumpPossible) &&
                    Handlung != PlayerActionEnum::BLITZEN &&
                    Handlung != PlayerActionEnum::BEAMLADEN) {
                AnimPhase = 0;
                Handlung = PlayerActionEnum::STEHEN;
            }
        }

        // Bei keiner Tastatureingabe steht der Spieler ruhig da
        if (!Aktion[AKTION_LINKS] && !Aktion[AKTION_RECHTS] &&
                Handlung != PlayerActionEnum::SPRINGEN &&
                Handlung != PlayerActionEnum::BLITZEN &&
                Handlung != PlayerActionEnum::PISSEN &&
                Handlung != PlayerActionEnum::BEAMLADEN && ShotDelay <= 0.5f) {
            if (Handlung != PlayerActionEnum::SPRINGEN)
                Handlung = PlayerActionEnum::STEHEN;
            // AnimPhase = 0;
        }

        // im Sumpf langsam einsinken
        uint32_t busumpf = TileEngine.BlockUnten(xpos, ypos, xposold, yposold, CollideRect, yspeed >= 0.0f);
        if (busumpf & BLOCKWERT_SUMPF || bl & BLOCKWERT_SUMPF || br & BLOCKWERT_SUMPF) {
            if (!Aktion[AKTION_JUMP])
                JumpPossible = true;

            if (!(busumpf & BLOCKWERT_WAND) && yspeed >= 0.0f)
                ypos += 4.0f SYNC;

            if (Handlung == PlayerActionEnum::SPRINGEN && yspeed >= 0.0f)
                Handlung = PlayerActionEnum::LAUFEN;
        }

        // Springen (nur, wenn nicht schon in der Luft)
        if (Aktion[AKTION_JUMP] && JumpPossible) {
            int save = AnimPhase;  // Für Sumpf

            SoundManager.StopWave(SOUND_BEAMLOAD + SoundOff);
            JumpPossible = false;
            AnimPhase = 2;  // das springen starten
            Handlung = PlayerActionEnum::SPRINGEN;
            JumpStart = ypos;
            yspeed = -PLAYER_MAXJUMPSPEED;

            JumpAdd = 0.0f;

            // Gegner dran festgebissen? Dann kann man nicht so hoch springen
            if (GegnerDran) {
                yspeed *= 0.5f;
                JumpAdd = PLAYER_JUMPADDSPEED;
            }

            if (AufPlattform != nullptr) {
                GegnerMushroom *pTemp;

                if (AufPlattform->GegnerArt == MUSHROOM) {
                    pTemp = reinterpret_cast<GegnerMushroom *>(AufPlattform);
                    pTemp->PlayerJumps(this);
                }

                AufPlattform = nullptr;
            }

            // Blubbersound noch beim Springen im Wasser
            // DKS - Added function WaveIsPlaying() to SoundManagerClass:
            // if (InLiquid == true &&
            //        SoundManager.its_Sounds[SOUND_DIVE]->isPlaying == false)
            //    SoundManager.PlayWave(100, rand()%255, 8000 + rand()%4000, SOUND_DIVE);
            if (InLiquid && !SoundManager.WaveIsPlaying(SOUND_DIVE))
                SoundManager.PlayWave(100, random(255), 8000 + random(4000), SOUND_DIVE);

            // Im Sumpf? Dann ein paar Pixel anheben und wieder runterfallen
            if (bu & BLOCKWERT_SUMPF) {
                ypos -= 15.0f;

                // noch nicht aus dem Sumpf draussen? Dann gleich wieder runterfallen
                // DKS - Rewrote BlockSlopes function to only take the parameters it uses:
                // bu = TileEngine.BlockSlopes     (xpos, ypos, xposold, yposold, CollideRect, yspeed);
                bu = TileEngine.BlockSlopes(xpos, ypos, CollideRect, yspeed);
                if (bu & BLOCKWERT_SUMPF) {
                    // yspeed = 0.0f;
                    JumpPossible = false;
                    AnimPhase = save;
                    // Handlung = PlayerActionEnum::STEHEN;
                }
            }
        }

        // Ist ein Sprung möglich ? (Wenn der Spieler wieder auf den Boden aufkommt)
        if ((bu & BLOCKWERT_SUMPF) || (bu & BLOCKWERT_WAND) || (bu & BLOCKWERT_PLATTFORM) ||
            (bu & BLOCKWERT_SCHRAEGE_L) || (bu & BLOCKWERT_SCHRAEGE_R) || AufPlattform != nullptr) {
            if (!(bu & BLOCKWERT_SCHRAEGE_L) && !(bu & BLOCKWERT_SCHRAEGE_R)) {
                if (yspeed > 0.0f) {
                    // Beim Blitz weiterblitzen, ansonsten auf "STEHEN" schalten
                    //
                    if (Handlung != PlayerActionEnum::BLITZEN) {
                        Handlung = PlayerActionEnum::STEHEN;
                        AnimPhase = 0;
                    }

                    // stehen bleiben
                    yspeed = 0.0f;
                    SoundManager.PlayWave(100, 128, 11025, SOUND_LANDEN);
                    PartikelSystem.PushPartikel(xpos + 20, ypos + 60, SMOKE);
                }
            } else
                // DKS - Rewrote BlockSlopes function to only take the parameters it uses:
                // bu = TileEngine.BlockSlopes     (xpos, ypos, xposold, yposold, CollideRect, yspeed);
                bu = TileEngine.BlockSlopes(xpos, ypos, CollideRect, yspeed);

            if (!JumpPossible && !Aktion[AKTION_JUMP] && yspeed >= 0.0f)
                JumpPossible = true;
        }

        // Kein Block unter dem Spieler und kein Lift? Dann fällt er runter
        if (AufPlattform == nullptr &&
                Handlung != PlayerActionEnum::SPRINGEN &&
                yspeed >= 0.0f && (!(bu & BLOCKWERT_WAND)) &&
                (!(bu & BLOCKWERT_PLATTFORM)) && (!(bu & BLOCKWERT_SUMPF)) &&
                (!(bl & BLOCKWERT_SCHRAEGE_L)) && (!(bl & BLOCKWERT_SCHRAEGE_R)) &&
                (!(br & BLOCKWERT_SCHRAEGE_L)) && (!(br & BLOCKWERT_SCHRAEGE_R)) &&
                (!(bu & BLOCKWERT_SCHRAEGE_L)) && (!(bu & BLOCKWERT_SCHRAEGE_R))) {
            JumpPossible = false;

            if (Handlung != PlayerActionEnum::BLITZEN) {
                Handlung = PlayerActionEnum::SPRINGEN;
                AnimPhase = 0;
                // yspeed    = 0.5f;
            }

            if ((Handlung == PlayerActionEnum::BLITZEN && yspeed == 0.0f))
                yspeed = 0.5f;

            JumpAdd = PLAYER_JUMPADDSPEED;
        }

        // Am Boden und nicht im Sumpf oder Damage-Tile? Dann die Position sichern
        if (Handlung != PlayerActionEnum::SPRINGEN &&
                AufPlattform == nullptr && !(bo & BLOCKWERT_SUMPF) && !(bu & BLOCKWERT_SUMPF) &&
                !(bl & BLOCKWERT_SUMPF) && !(br & BLOCKWERT_SUMPF) && !(bo & BLOCKWERT_SCHADEN) &&
                !(bu & BLOCKWERT_SCHADEN) && !(bl & BLOCKWERT_SCHADEN) && !(br & BLOCKWERT_SCHADEN)) {
            JumpxSave = xpos;
            JumpySave = ypos;
        }

        // Vom Ducken aufstehen
        // nur, wenn keine Decke über dem Spieler (wenn er grad mit ner Plattform wo drunter durchfährt z.B.)
        if (Handlung == PlayerActionEnum::DUCKEN) {
            float ypos2 = ypos - 48.0f;
            uint32_t bo2 = TileEngine.BlockOben(xpos, ypos2, xposold, yposold, CollideRect, true);

            if (!(bo & BLOCKWERT_WAND) && !(bo2 & BLOCKWERT_WAND))
                Handlung = PlayerActionEnum::STEHEN;
        }

        CollideRect.top = 12;  // Blockierung wieder wie im Stehen

        // oder gleich wieder geduckt bleiben ?
        if (Aktion[AKTION_DUCKEN] &&
                // Aktion[AKTION_LINKS]  == false &&
                // Aktion[AKTION_RECHTS] == false &&
                JumpPossible &&
                Handlung != PlayerActionEnum::BEAMLADEN &&
                Handlung != PlayerActionEnum::BLITZEN) {
            Handlung = PlayerActionEnum::DUCKEN;
            CollideRect.top = 32;  // Im Ducken liegt die obere Blockgrenze tiefer
        }

        // Blitz schiessen ? (nur, wenn am Boden)
        if (Aktion[AKTION_BLITZ] && JumpPossible) {
            // Blitz aufladen?
            if (Aktion[AKTION_SHOOT] && Handlung != PlayerActionEnum::BEAMLADEN) {
                SoundManager.PlayWave(100, 128, 9000, SOUND_BEAMLOAD + SoundOff);

                DirectInput.Joysticks[JoystickIndex].ForceFeedbackEffect(FFE_BLITZ);

                Handlung = PlayerActionEnum::BEAMLADEN;
                BlitzStart = 5.0f;
                AnimPhase = 0;
                if (Blickrichtung == LINKS)  // Blitz je nach Blickrichtung neu
                    BlitzWinkel = 270;       // geradeaus richten
                else
                    BlitzWinkel = 90;

                if (Aktion[AKTION_OBEN]) {
                    Blickrichtung *= -1;
                    BlitzWinkel = 0;
                }
            }

            else

                if (Handlung != PlayerActionEnum::BLITZEN &&
                    Handlung != PlayerActionEnum::BEAMLADEN)  // Blitz wird erst gestartet ?
                {
                    BlitzStart = 0.0f;
                    BeamCount = 0.1f;
                    AnimPhase = 0;
                    Handlung = PlayerActionEnum::BLITZEN;
                    changecount = 0.0f;

                    // Blitz Startsound ausgeben
                    SoundManager.PlayWave(100, 128, random(500) + 18025, SOUND_BLITZSTART + SoundOff);

                    if (Blickrichtung == LINKS)  // Blitz je nach Blickrichtung neu
                        BlitzWinkel = 270;       // geradeaus richten
                    else
                        BlitzWinkel = 90;
                }
        }

        if (!Aktion[AKTION_BLITZ] && Handlung == PlayerActionEnum::BLITZEN) {
            // Im Fallen? Dann in Sprunganimation wechseln
            //
            if (yspeed > 0.0f) {
                Handlung = PlayerActionEnum::SPRINGEN;
            }

            // Im Stehen? Dann je nach aktueller Blickrichtung
            // richtig hinstellen
            //
            else {
                if (AnimPhase >= 9 && AnimPhase <= 18)
                    Blickrichtung *= -1;

                Handlung = PlayerActionEnum::STEHEN;
            }

            AnimPhase = 0;
        }

        // In das Rad verwandeln ?
        if (JumpPossible && Aktion[AKTION_POWERLINE] && Aktion[AKTION_DUCKEN] && Armour > 5.0f) {
            Handlung = PlayerActionEnum::RADELN;
            AnimPhase = 0;
            AnimCount = 0.0f;
            CollideRect.left = 20;  // Neue Abmessungen für das Rad
            CollideRect.top = 40;
            CollideRect.right = 50;
            CollideRect.bottom = 79;
            PowerLinePossible = false;
        }

        // Geloopten Blitzsound ausgeben
        if (Handlung == PlayerActionEnum::BLITZEN) {
            // Blitz Sound abspielen, wenn er nicht schon abgespielt wird
            // DKS - Added function WaveIsPlaying() to SoundManagerClass:
            // if (BlitzStart >= PLAYER_BLITZ_START &&
            //        true == SoundManager.InitSuccessfull &&
            //        SoundManager.its_Sounds[SOUND_BLITZ + SoundOff]->isPlaying == false)
            if (BlitzStart >= PLAYER_BLITZ_START && !SoundManager.WaveIsPlaying(SOUND_BLITZ + SoundOff)) {
                SoundManager.PlayWave(100, 128, 11025, SOUND_BLITZ + SoundOff);
                DirectInput.Joysticks[JoystickIndex].ForceFeedbackEffect(FFE_BLITZ);
            }
        }
        // DKS - Added function WaveIsPlaying() to SoundManagerClass:
        // else if (true == SoundManager.InitSuccessfull &&
        //         SoundManager.its_Sounds[SOUND_BLITZ + SoundOff]->isPlaying == true)
        else if (SoundManager.WaveIsPlaying(SOUND_BLITZ + SoundOff)) {
            SoundManager.StopWave(SOUND_BLITZ + SoundOff);
            SoundManager.PlayWave(100, 128, random(1000) + 11025, SOUND_BLITZENDE + SoundOff);
            DirectInput.Joysticks[JoystickIndex].StopForceFeedbackEffect(FFE_BLITZ);
        }

        // Spieler nach BlitzRichtung ausrichten
        if (Handlung == PlayerActionEnum::BLITZEN) {
            // Verzögerung beim Blitzen
            if (BlitzStart < PLAYER_BLITZ_START)
                BlitzStart += 1.0f SYNC;
            else {
                float Winkel;

                Winkel = BlitzWinkel - 270;  // 270° beim nach links kucken = Animphase 0
                if (Winkel < 0.0f)
                    Winkel += 360.0f;

                // Je nach Richtung Winkel umdrehen
                if (Blickrichtung == LINKS)
                    Winkel = 360.0f - Winkel;
                else
                    Winkel = 180 + Winkel;

                AnimPhase = static_cast<int>((Winkel + 0.0f) / 10.0f) % FRAMES_SURROUND;
            }
        }

        // Beam Laden und richtige Animationsphase setzen
        //
        if (Handlung == PlayerActionEnum::BEAMLADEN) {
            // Beam aufladen. Je länger der Blitz desto schneller lädt der Beam
            if (BlitzStart < PLAYER_BEAM_MAX)
                BlitzStart += CurrentWeaponLevel[3] * 1.0f SYNC;

            float Winkel;

            Winkel = BlitzWinkel - 270;  // 270° beim nach links kucken = Animphase 0
            if (Winkel < 0.0f)
                Winkel += 360.0f;

            // Je nach Richtung Winkel umdrehen
            if (Blickrichtung == LINKS)
                Winkel = 360.0f - Winkel;
            else
                Winkel = 180 + Winkel;

            AnimPhase = static_cast<int>((Winkel + 0.0f) / 10.0f) % FRAMES_SURROUND;

            // Beam abfeuern
            if (!Aktion[AKTION_BLITZ] || !Aktion[AKTION_SHOOT]) {
                Handlung = PlayerActionEnum::STEHEN;
                SoundManager.StopWave(SOUND_BEAMLOAD + SoundOff);
                DirectInput.Joysticks[JoystickIndex].StopForceFeedbackEffect(FFE_BLITZ);

                if (BlitzStart >= 20.0f) {
                    Projectiles.PushBlitzBeam(static_cast<int>(BlitzStart), BlitzWinkel, this);
                    SoundManager.PlayWave(100, 128, 7000, SOUND_BLITZENDE + SoundOff);
                    SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND_BLITZENDE + SoundOff);
                    SoundManager.PlayWave(100, 128, 10000, SOUND_EXPLOSION3);
                }

                // je nach Ausrichtung der Waffe in die richtige Richtung kucken
                //
                if (AnimPhase >= 12 && AnimPhase <= 24)
                    Blickrichtung *= -1;
            }
        }

        // Piss-Animation
        //
        if (Handlung == PlayerActionEnum::PISSEN) {
            AnimCount += 1.0f SYNC;

            if (AnimCount > 1.0f) {
                AnimCount = 0.0f;
                AnimPhase++;

                if (AnimPhase > 2)
                    AnimCount = -1.0f;

                if (AnimPhase == 11)
                    AnimCount = -2.0f;
            }

            if (AnimPhase == 16)
                AnimPhase = 3;
        }
    }

    else

        //--------------------
        // Als Rad rumkullern
        //--------------------

        if (Handlung == PlayerActionEnum::RADELN || Handlung == PlayerActionEnum::RADELN_FALL)

    {
        // Rad-Energie abziehen
        if (!WheelMode)
            Armour -= static_cast<float>(2.5 SYNC);

        if (Armour < 0.0f)
            Armour = 0.0f;

        // Powerline schiessen ?
        if (Aktion[AKTION_POWERLINE] && PowerLinePossible && PowerLines > 0) {
            SoundManager.PlayWave(100, 128, 11025, SOUND_POWERLINE);
            PowerLinePossible = false;  // Taste einrasten
            PowerLines--;               // Powerlines verringern

            for (int i = -1; i < 25; i++)  // Powerlines schiessen
            {
                Projectiles.PushProjectile(xpos + 20, static_cast<float>(static_cast<int>(TileEngine.YOffset / 20) * 20 + i * 20), POWERLINE,
                                           this);
                Projectiles.PushProjectile(xpos + 20, static_cast<float>(static_cast<int>(TileEngine.YOffset / 20) * 20 + i * 20), POWERLINE2,
                                           this);
            }
        }

        // SmartBomb abfeuern ?
        if (Aktion[AKTION_SMARTBOMB] && PowerLinePossible && SmartBombs > 0) {
            SoundManager.PlayWave(100, 128, 8000, SOUND_POWERLINE);
            PowerLinePossible = false;  // Taste einrasten
            SmartBombs--;               // SmartBombs verringern

            Projectiles.PushProjectile(xpos + 40 - 32, ypos + 55 - 32, SMARTBOMB, this);
        }

        // Taste losgelassen ? Dann können wir wieder eine Powerline schiessen
        if (!Aktion[AKTION_POWERLINE] && !Aktion[AKTION_SMARTBOMB])
            PowerLinePossible = true;

        // Ja nach Blickrichtung in die richtige Richtung scrollen
        if (Blickrichtung == LINKS && (!(bl & BLOCKWERT_WAND))) {
            if (!InLiquid)
                xspeed = -40.0f;
            else
                xspeed = -30.0f;
        } else if (Blickrichtung == RECHTS && (!(br & BLOCKWERT_WAND))) {
            if (!InLiquid)
                xspeed = 40.0f;
            else
                xspeed = 30.0f;
        }

        if (Aktion[AKTION_LINKS] &&  // Nach Links rollen ?
            (Handlung == PlayerActionEnum::RADELN ||   // Nur wenn man Boden unter den Füßen hat
             AufPlattform != nullptr || bu & BLOCKWERT_WAND || bu & BLOCKWERT_PLATTFORM))
            Blickrichtung = LINKS;

        if (Aktion[AKTION_RECHTS] &&  // Nach Rechts rollen ?
            (Handlung == PlayerActionEnum::RADELN ||    // Nur wenn man Boden unter den Füßen hat
             AufPlattform != nullptr || bu & BLOCKWERT_WAND || bu & BLOCKWERT_PLATTFORM))
            Blickrichtung = RECHTS;

        if (Handlung != PlayerActionEnum::RADELN_FALL &&
                (!(bu & BLOCKWERT_SCHRAEGE_R)) && (!(bu & BLOCKWERT_SCHRAEGE_L)) &&
                (!(bu & BLOCKWERT_WAND)) && (!(bu & BLOCKWERT_PLATTFORM))) {
            Handlung = PlayerActionEnum::RADELN_FALL;
            yspeed = 0.5f;
            JumpAdd = PLAYER_JUMPADDSPEED;
        }

        // Wieder am Boden aufgekommen ?
        if (Handlung == PlayerActionEnum::RADELN_FALL &&
            ((AufPlattform != nullptr) || (bu & BLOCKWERT_WAND) || (bu & BLOCKWERT_PLATTFORM))) {
            if (yspeed > 2.0f)
                PartikelSystem.PushPartikel(xpos + 20, ypos + 60, SMOKE);

            if (yspeed > 0.0f) {
                yspeed = -yspeed * 2 / 3;  // Abhopfen

                if (yspeed > -5.0f)  // oder ggf wieder
                {
                    // Normal weiterkullern
                    yspeed = 0.0f;
                    JumpAdd = 0.0f;
                    Handlung = PlayerActionEnum::RADELN;
                    TileEngine.BlockUnten(xpos, ypos, xposold, yposold, CollideRect, true);
                } else {
                    AufPlattform = nullptr;
                    SoundManager.PlayWave(100, 128, 11025, SOUND_LANDEN);
                }
            }
        }

        // An die Wand gestossen ? Dann Richtung umkehren
        if (!(bu & BLOCKWERT_SCHRAEGE_R) && !(bu & BLOCKWERT_SCHRAEGE_L)) {
            if (Blickrichtung == RECHTS && (br & BLOCKWERT_WAND))
                Blickrichtung = LINKS;

            if (Blickrichtung == LINKS && (bl & BLOCKWERT_WAND))
                Blickrichtung = RECHTS;
        }

        // Aufhören zu kullern und zurückverwandeln, wenn man springt oder keine Energie mehr hat
        // funktiomiert nur, wenn über einem keine Wand ist. Dabei wird noch ein Teil mehr gecheckt, als
        // das Rad hoch ist, damit man den Zustand prüft, als wenn der Spieler wieder stehen würde

        float ypos2 = ypos - 20.0f;
        uint32_t bo2 = TileEngine.BlockOben(xpos, ypos2, xposold, yposold, CollideRect, true);

        if ((Armour <= 0.0f || Aktion[AKTION_JUMP]) && !(bo & BLOCKWERT_WAND) && !(bo2 & BLOCKWERT_WAND)) {
            AnimPhase = 0;

            // Am Boden zurückverwandelt ?
            if (Armour <= 0.0f && ((bu & BLOCKWERT_WAND) || (bu & BLOCKWERT_PLATTFORM)))
                Handlung = PlayerActionEnum::STEHEN;

            // oder in der Luft bzw durch Sprung
            else
                Handlung = PlayerActionEnum::SPRINGEN;  // Nein, dann in der Luft

            CollideRect.left = 20;
            CollideRect.top = 12;
            CollideRect.right = 50;
            CollideRect.bottom = 79;
            if (Armour < 0.0f)
                Armour = 0.0f;
        }
    }

    else

        //------------------------
        // Auf dem FlugSack reiten
        //------------------------

        if (Handlung == PlayerActionEnum::SACKREITEN || Handlung == PlayerActionEnum::DREHEN) {
        // Spieler und Level bewegen (wenn nicht beim Endboss bzw wenn Level scrollbar)
        // Nur, wenn alle Spieler auf einem Flugsack sitzen
        //
        bool BeideFrei = true;

        for (int p = 0; p < NUMPLAYERS; p++)
            if (Player[p].FesteAktion > -1)
                BeideFrei = false;

        if (FlugsackFliesFree == false && Riding() && BeideFrei == true)
            ypos -= PLAYER_FLUGSACKSPEED SYNC;

        JumpySave = ypos;
        JumpxSave = xpos;

        // unten rausgeflogen ? Dann wieder zurücksetzen
        if (ypos > TileEngine.YOffset + 475.0f) {
            SoundManager.PlayWave(100, 128, 11025, SOUND_EXPLOSION2);
            xpos = JumpxSave;  // Alte Position wieder herstellen, wenn der
            ypos = JumpySave;  // der Spieler zB einen Abgrund runterfiel
            xposold = JumpxSave;
            yposold = JumpySave;
            Energy = 0.0f;
        }

        // An der Decke anstoßen
        if (ypos < TileEngine.YOffset && BeideFrei) {
            if (yadd < 0.0f)
                yadd *= -1.0f;
        }

        // Wände checken
        if (bl & BLOCKWERT_WAND)
            if (xadd < 0.0f)
                xadd = -xadd / 2.0f;
        if (br & BLOCKWERT_WAND)
            if (xadd > 0.0f)
                xadd = -xadd / 2.0f;
        if (bo & BLOCKWERT_WAND)
            if (yadd < 0.0f)
                yadd = -yadd / 2.0f;
        if (bu & BLOCKWERT_WAND)
            if (yadd > 0.0f)
                yadd = -yadd / 2.0f;

        // Rauch am Flugsack erzeugen
        SmokeCount -= 1.0f SYNC;
        if (SmokeCount < 0.0f) {
            SmokeCount += 0.1f;

            // Beim Reiten
            if (Handlung == PlayerActionEnum::SACKREITEN) {
                if (Blickrichtung == LINKS)
                    PartikelSystem.PushPartikel(xpos + 72, ypos + 100, FLUGSACKSMOKE2);
                else
                    PartikelSystem.PushPartikel(xpos - 2, ypos + 100, FLUGSACKSMOKE);
            }

            // oder beim Drehen
            if (Handlung == PlayerActionEnum::DREHEN) {
                if (Blickrichtung == LINKS) {
                    PartikelSystem.PushPartikel(xpos + 70 - AnimPhase * 10, ypos + 100, FLUGSACKSMOKE2);
                    PartikelSystem.PushPartikel(xpos + 87 - AnimPhase * 10, ypos + 100, FLUGSACKSMOKE);
                } else {
                    PartikelSystem.PushPartikel(xpos + 4 + (AnimPhase - 10) * 10, ypos + 100, FLUGSACKSMOKE);
                    PartikelSystem.PushPartikel(xpos - 20 + (AnimPhase - 10) * 10, ypos + 100, FLUGSACKSMOKE2);
                }
            }
        }

        // Umdrehen
        if (Handlung == PlayerActionEnum::DREHEN) {
            AnimCount += 1.2f SYNC;

            if (AnimCount > 0.6f) {
                AnimCount -= 0.6f;
                AnimPhase++;

                // Zuende gedreht ?
                if (AnimPhase == AnimEnde) {
                    AnimEnde = 0;
                    Handlung = PlayerActionEnum::SACKREITEN;

                    if (Blickrichtung == RECHTS) {
                        AnimPhase = 0;
                        Blickrichtung = LINKS;
                    } else {
                        AnimPhase = 10;
                        Blickrichtung = RECHTS;
                    }
                }
            }
        }

        // Links fliegen
        if (Aktion[AKTION_LINKS] == true && Aktion[AKTION_RECHTS] == false) {
            xadd -= 10.0f SYNC;

            // Drehen ?
            if (Aktion[AKTION_SHOOT] == false && Handlung == PlayerActionEnum::SACKREITEN &&
                    Blickrichtung == RECHTS) {
                Handlung = PlayerActionEnum::DREHEN;
                AnimEnde = 19;
                AnimCount = 0.0f;
                AnimPhase = 10;
            }
        }

        // Rechts fliegen
        if (Aktion[AKTION_RECHTS] == true && Aktion[AKTION_LINKS] == false) {
            xadd += 10.0f SYNC;

            // Drehen ?
            if (Aktion[AKTION_SHOOT] == false && Handlung == PlayerActionEnum::SACKREITEN &&
                    Blickrichtung == LINKS) {
                Handlung = PlayerActionEnum::DREHEN;
                AnimEnde = 9;
                AnimCount = 0.0f;
                AnimPhase = 0;
            }
        }

        // Hoch fliegen
        if ((Aktion[AKTION_OBEN] == true || Aktion[AKTION_JUMP] == true) && Aktion[AKTION_DUCKEN] == false)
            yadd -= 10.0f SYNC;

        // Runter fliegen
        if (Aktion[AKTION_DUCKEN] == true && Aktion[AKTION_OBEN] == false)
            yadd += 10.0f SYNC;

        // Bewegung abbremsen
        if (!Aktion[AKTION_LINKS] && !Aktion[AKTION_RECHTS]) {
            if (xadd < 0.0f) {
                xadd += 4.0f SYNC;
                if (xadd > 0.0f)
                    xadd = 0.0f;
            }

            if (xadd > 0.0f) {
                xadd -= 4.0f SYNC;
                if (xadd < 0.0f)
                    xadd = 0.0f;
            }
        }

        if (!Aktion[AKTION_OBEN] && !Aktion[AKTION_DUCKEN]) {
            if (yadd < 0.0f) {
                yadd += 4.0f SYNC;
                if (yadd > 0.0f)
                    yadd = 0.0f;
            }

            if (yadd > 0.0f) {
                yadd -= 4.0f SYNC;
                if (yadd < 0.0f)
                    yadd = 0.0f;
            }
        }

        // Geschwindigkeitsgrenzen checken
        if (xadd > 25.0f)
            xadd = 25.0f;
        if (xadd < -25.0f)
            xadd = -25.0f;
        if (yadd > 25.0f)
            yadd = 25.0f;
        if (yadd < -25.0f)
            yadd = -25.0f;

        xspeed = xadd;
        ypos += yadd SYNC;

        // Wendepunkt erreicht? Dann automatisch abspringen
        float ytemp = ypos - 1;
        bu = TileEngine.BlockUntenNormal(xpos, ypos, xposold, ytemp, CollideRect);
        if (bu & BLOCKWERT_WENDEPUNKT) {
            Handlung = PlayerActionEnum::SPRINGEN;
            AnimPhase = 0;
            yspeed = -PLAYER_MAXJUMPSPEED;
            JumpAdd = PLAYER_JUMPADDSPEED;
            AufPlattform = nullptr;

            // abstürzenden Flugsack adden
            Gegner.PushGegner(xpos, ypos + 20, FLUGSACK, 99, 0, false);

            FlugsackFliesFree = true;
            TileEngine.Zustand = TileStateEnum::SCROLLBAR;

            SoundManager.FadeSong(MUSIC_FLUGSACK, -2.0f, 0, true);
            SoundManager.FadeSong(MUSIC_STAGEMUSIC, 2.0f, 100, true);
        }
    }

    //-------------------------
    // Auf dem SurfBrett stehen
    //-------------------------

    // DKS - This appears never to have been implemented (no image for it) so disabled it:
#if 0
    if (Handlung == PlayerActionEnum::SURFEN     ||
            Handlung == PlayerActionEnum::SURFENJUMP ||
            Handlung == PlayerActionEnum::SURFENCROUCH)
    {
        static float Antrieb = 0.0f;

        // Beim ersten Draufspringen geht das Board kurz ab
        if (JumpedOnSurfboard == true)
        {
            JumpedOnSurfboard = false;
            xadd = 30.0f;
        }

        // Spieler und Level bewegen (wenn nicht beim Endboss bzw wenn Level scrollbar)
        //
        xpos			     += AutoScrollspeed SYNC;
        TileEngine.XOffset += AutoScrollspeed SYNC;

        if (Handlung == PlayerActionEnum::SURFENCROUCH)
            Handlung = PlayerActionEnum::SURFEN;

        // Ducken
        if (Aktion[AKTION_DUCKEN] == true  &&
                Aktion[AKTION_LINKS]  == false &&
                Aktion[AKTION_RECHTS] == false &&
                JumpPossible == true           &&
                Handlung !=PlayerActionEnum:: BLITZEN)
        {
            Handlung = PlayerActionEnum::SURFENCROUCH;
            CollideRect.top = 32;	// Im Ducken liegt die obere Blockgrenze tiefer
        }

        // Springen (nur, wenn nich schon in der Luft)
        if (Aktion[AKTION_JUMP] && JumpPossible == true)
        {
            Handlung = PlayerActionEnum::SURFENJUMP;
        }

        // Links surfen
        if (Aktion[AKTION_LINKS] == true && Aktion[AKTION_RECHTS]== false)
            xadd -= 12.0f SYNC;

        // Rechts surfen
        if (Aktion[AKTION_RECHTS] == true && Aktion[AKTION_LINKS]== false)
        {
            xadd += 12.0f SYNC;
            Antrieb -= 1.0f SYNC;

            // Antriebsflamme erzeugen
            //
            if (Antrieb < 0.0f)
            {
                PartikelSystem.PushPartikel (xpos - 10, ypos + 60 + rand()%6, ROCKETSMOKE);
                Antrieb += 0.1f;
            }
        }

        // Bewegung abbremsen
        if (!Aktion[AKTION_LINKS] && !Aktion[AKTION_RECHTS]) xadd *= 0.99f;

        // Geschwindigkeitsgrenzen checken
        if (xadd >  25.0f) xadd =  25.0f;
        if (xadd < -25.0f) xadd = -25.0f;

        xspeed = xadd;
    }
#endif  // 0

    //----------------------
    // Powerline schiessen ?
    //----------------------

    if (Aktion[AKTION_POWERLINE] == true && Aktion[AKTION_DUCKEN] == false && PowerLinePossible == true &&
        PowerLines > 0) {
        SoundManager.PlayWave(100, 128, 11025, SOUND_POWERLINE);
        PowerLinePossible = false;  // Taste einrasten
        PowerLines--;               // Powerlines verringern

        for (int i = -1; i < 25; i++)  // Powerlines schiessen
        {
            Projectiles.PushProjectile(xpos + 20, static_cast<float>(static_cast<int>(TileEngine.YOffset / 20) * 20 + i * 20), POWERLINE, this);
            Projectiles.PushProjectile(xpos + 20, static_cast<float>(static_cast<int>(TileEngine.YOffset / 20) * 20 + i * 20), POWERLINE2, this);
        }
    }

    //---------------------
    // SmartBomb abfeuern ?
    //---------------------

    if (Aktion[AKTION_SMARTBOMB] == true && PowerLinePossible == true && SmartBombs > 0) {
        SoundManager.PlayWave(100, 128, 8000, SOUND_POWERLINE);
        PowerLinePossible = false;  // Taste einrasten
        SmartBombs--;               // SmartBombs verringern

        Projectiles.PushProjectile(xpos + 40 - 32, ypos + 35 - 32, SMARTBOMB, this);
    }

    //--------------------------------------------------------------------
    // Taste losgelassen ? Dann können wir wieder eine Powerline schiessen
    //--------------------------------------------------------------------

    if (Aktion[AKTION_POWERLINE] == false && Aktion[AKTION_GRANATE] == false && Aktion[AKTION_SMARTBOMB] == false)
        PowerLinePossible = true;

    // --------------------------------
    // Hoch / runter kucken bzw. zielen
    // --------------------------------

    // nach oben zielen
    if (Aktion[AKTION_OBEN]) {
        if (Handlung == PlayerActionEnum::STEHEN ||
                Handlung == PlayerActionEnum::SCHIESSEN_O ||
                Handlung == PlayerActionEnum::SCHIESSEN_LO ||
                Handlung == PlayerActionEnum::SCHIESSEN_RO ||
                Handlung == PlayerActionEnum::LAUFEN) {
            if (Aktion[AKTION_LINKS]) {
                Handlung = PlayerActionEnum::SCHIESSEN_LO;
                Blickrichtung = LINKS;
            } else if (Aktion[AKTION_RECHTS]) {
                Handlung = PlayerActionEnum::SCHIESSEN_RO;
                Blickrichtung = RECHTS;
            } else
                Handlung = PlayerActionEnum::SCHIESSEN_O;
        }
    }

    if ((!Aktion[AKTION_OBEN] && !Aktion[AKTION_UNTEN]) ||
            NUMPLAYERS == 2 || TileEngine.Zustand != TileStateEnum::SCROLLBAR)
        look = 0.0f;

    if (TileEngine.Zustand == TileStateEnum::SCROLLBAR && NUMPLAYERS < 2 &&
            Handlung != PlayerActionEnum::SACKREITEN &&
            Handlung != PlayerActionEnum::DREHEN &&
            Handlung != PlayerActionEnum::SURFEN &&
            Handlung != PlayerActionEnum::SURFENJUMP &&
            Handlung != PlayerActionEnum::SURFENCROUCH) {
        // Spieler steht am Boden ?
        //

        if (Handlung != PlayerActionEnum::SPRINGEN) {
            // Hoch scrollen / nach oben zielen
            if (Aktion[AKTION_OBEN]) {
                look += 1.0f SYNC;

                if (look > 5.0f) {
                    if (TileEngine.YOffset > ypos - 400.0f)
                        TileEngine.YOffset -= 19.0f SYNC;
                    else
                        TileEngine.YOffset = ypos - 400.0f;
                }
            }

            // Runter scrollen bzw. runter zielen
            if (Aktion[AKTION_UNTEN]) {
                look += 1.0f SYNC;

                if (look > 5.0f || AktionKeyboard[AKTION_UNTEN] != AktionKeyboard[AKTION_DUCKEN]) {
                    if (TileEngine.YOffset < ypos - 40.0f)
                        TileEngine.YOffset += 19.0f SYNC;
                    else
                        TileEngine.YOffset = ypos - 40.0f;
                }
            }
        }

        else {
            if (ypos - TileEngine.YOffset < SCROLL_BORDER_EXTREME_TOP)
                TileEngine.YOffset = ypos - SCROLL_BORDER_EXTREME_TOP;

            if (ypos - TileEngine.YOffset > SCROLL_BORDER_EXTREME_BOTTOM && yspeed > 0.0f)
                TileEngine.YOffset = ypos - SCROLL_BORDER_EXTREME_BOTTOM;
        }

        // extrem Rand trotzdem nochmal checken
        if (Handlung != PlayerActionEnum::SACKREITEN && Handlung != PlayerActionEnum::DREHEN) {
            if (ypos - TileEngine.YOffset < 40)
                TileEngine.YOffset = ypos - 40;
            if (ypos - TileEngine.YOffset > 380)
                TileEngine.YOffset = ypos - 380;
        }
    }

    // Fahrstuhl hat andere Grenzen
    if (Riding() == false && TileEngine.IsElevatorLevel == true && ypos - TileEngine.YOffset > 300)
        TileEngine.YOffset = ypos - 300;

    // -----------------
    // Spieler animieren
    // -----------------

    // Hat der Spieler ein Schutzschild ?

    if (Shield > 0.0f)
        Shield -= 0.4f SYNC;

    //-----------------------------------------------
    // Energie abziehen

    if ((bo & BLOCKWERT_SCHADEN) || (bu & BLOCKWERT_SCHADEN) || (bl & BLOCKWERT_SCHADEN) || (br & BLOCKWERT_SCHADEN)) {
        switch (Skill) {
            case SKILL_EASY:     DamagePlayer(10.0f SYNC); break;
            case SKILL_MEDIUM:   DamagePlayer(20.0f SYNC); break;
            case SKILL_HARD:     DamagePlayer(30.0f SYNC); break;
            case SKILL_HURRICAN: DamagePlayer(40.0f SYNC); break;
        }

        if ((Handlung == PlayerActionEnum::RADELN || Handlung == PlayerActionEnum::RADELN_FALL) && WheelMode == false)
            Armour -= 5.0f SYNC;
    }

    //-----------------------------------------------
    // Testen, ob sich der Spieler im Wasser befindet

    // DKS - Added bounds check to Tiles[][] array, eliminated divisions while I was here:
    // uint32_t middle = TileEngine.TileAt(static_cast<int>(xpos + 35) / TILESIZE_X, static_cast<int>(ypos + 40) /
    // TILESIZE_Y).Block;
    uint32_t middle = 0;
    {
        int tile_x = (xpos + 35.0f) * (1.0f / TILESIZE_X);
        int tile_y = (ypos + 40.0f) * (1.0f / TILESIZE_Y);
        if (tile_x >= 0 && tile_x < levelsize_x && tile_y >= 0 && tile_y < levelsize_y)
            middle = TileEngine.TileAt(tile_x, tile_y).Block;
    }

    int spritzertype = 0;
    if ((bu & BLOCKWERT_LIQUID) || (br & BLOCKWERT_LIQUID) || (bl & BLOCKWERT_LIQUID) || (bo & BLOCKWERT_LIQUID) ||
        middle & BLOCKWERT_LIQUID) {
        spritzertype = WASSER_SPRITZER2;

        // Gerade erst in Flüssigkeit gesprungen ?
        if (InLiquid == false) {
            if (Handlung == PlayerActionEnum::SPRINGEN || Handlung == PlayerActionEnum::RADELN_FALL) {
                for (int i = 0; i < 12; i++)
                    PartikelSystem.PushPartikel(xpos + 18 + random(20), ypos + CollideRect.bottom - 25, spritzertype);

                WinkelUebergabe = -1.0f;
                for (int i = 0; i < 15; i++)
                    PartikelSystem.PushPartikel(xpos + 10 + random(40), ypos + CollideRect.bottom + random(20),
                                                BUBBLE);
            }

            InLiquid = true;
            SoundManager.PlayWave(100, 128, 10000 + random(2050), SOUND_WATERIN);
        }
    } else {
        // Aus dem Wasser heraus gehopst ?
        if (InLiquid == true) {
            // Welcher Liquid Type?
            // if (bu & BLOCKWERT_WASSER) spritzertype = WASSER_SPRITZER;
            // if (bu & BLOCKWERT_LAVA)   spritzertype = LAVA_SPRITZER;
            spritzertype = WASSER_SPRITZER2;

            if (Handlung == PlayerActionEnum::SPRINGEN || Handlung == PlayerActionEnum::RADELN_FALL) {
                for (int i = 0; i < 12; i++)
                    PartikelSystem.PushPartikel(xpos + 10 + random(20), ypos + CollideRect.bottom - 25, spritzertype);
            }

            SoundManager.PlayWave(100, 128, 10000 + random(2050), SOUND_WATEROUT);
            InLiquid = false;
        }
    }

    // Spieler im Wasser
    if (InLiquid == true) {
        // Blubberblasen entstehen lassen
        // DKS - No need to do two separate rand()%500 calculations, also added
        //      function WaveIsPlaying to SoundManagerClass:
        // if (rand()%500 == 0)
        //    PartikelSystem.PushPartikel(xpos+30, ypos+20, BUBBLE);

        //// ggf noch Tauchgeräusche abspielen
        // if (rand()%500 == 0 &&
        //        SoundManager.its_Sounds[SOUND_DIVE]->isPlaying == false)
        //    SoundManager.PlayWave(100, rand()%255, 8000 + rand()%4000, SOUND_DIVE);
        if (random(500) == 0) {
            PartikelSystem.PushPartikel(xpos + 30, ypos + 20, BUBBLE);
            // ggf noch Tauchgeräusche abspielen
            if (!SoundManager.WaveIsPlaying(SOUND_DIVE))
                SoundManager.PlayWave(100, random(255), 8000 + random(4000), SOUND_DIVE);
        }
    }

    // schräg laufen?
    if ((Handlung == PlayerActionEnum::SCHIESSEN_LO || Handlung == PlayerActionEnum::SCHIESSEN_RO) && WalkLock == false) {
        // Nächste Animations-Phase ?
        AnimCount += 1.0f SYNC;
        while (AnimCount > PLAYER_ANIMSPEED) {
            AnimCount = AnimCount - PLAYER_ANIMSPEED;

            AnimPhase++;

            if (AnimPhase >= FRAMES_RUN)
                AnimPhase = 0;  // Loop
        }
    }

    // Normal laufen oder radeln?
    if (Handlung == PlayerActionEnum::LAUFEN ||       // Spieler läuft oder
            Handlung == PlayerActionEnum::RADELN ||       // oder kullert
            Handlung == PlayerActionEnum::RADELN_FALL ||  // oder fällt als Rad
            (Handlung == PlayerActionEnum::SPRINGEN &&    // springt, sprich, wird animiert ?
            yspeed > -PLAYER_MAXJUMPSPEED / 1.5f)) {
        if (InLiquid == false)
            AnimCount += 1.0f SYNC;  // Dann animieren, je nachdem, ob man im
        else                         // Wasser ist oder nicht
            AnimCount += 0.5f SYNC;  // verschieden schnell animieren

        if (Handlung == PlayerActionEnum::RADELN ||
                Handlung == PlayerActionEnum::RADELN_FALL)  // Als Rad schneller animieren, also
            AnimCount += 4.0f SYNC;                         // einfach nochmal den Wert viermal dazu

        while (AnimCount > PLAYER_ANIMSPEED) {
            AnimCount = AnimCount - PLAYER_ANIMSPEED;

            switch (Handlung)  // Je nach Handlung anders animieren
            {
                case PlayerActionEnum::LAUFEN:  // Laufen animieren
                {
                    AnimPhase++;

                    if (AnimPhase >= FRAMES_RUN)
                        AnimPhase = 0;  // Loop
                } break;

                case PlayerActionEnum::SPRINGEN:  // Springen animieren wobei erst animiert
                {
                    // wird wenn der Spieler wieder am Fallen ist
                    if (AnimPhase < FRAMES_JUMP - 1)
                        AnimPhase++;  // Kein Loop
                } break;

                case PlayerActionEnum::RADELN:
                case PlayerActionEnum::RADELN_FALL: {
                    if (AnimPhase < PLAYERANIM_RAD_ENDE)
                        AnimPhase++;
                    else
                        AnimPhase = 0;
                } break;

                default:
                    break;
            }
        }
    }

    // Springen
    if (Handlung == PlayerActionEnum::SPRINGEN || (Handlung == PlayerActionEnum::BLITZEN && yspeed > 0.0f)) {
        // Sprung-Geschwindigkeit manipulieren
        if (InLiquid == false)
            yspeed += JumpAdd SYNC;
        else
            yspeed += JumpAdd * 2 / 3 SYNC;

        // y-Position manipulieren, wenn oben frei ist
        if (InLiquid == false)
            ypos += yspeed SYNC;
        else
            ypos += yspeed * 2 / 3 SYNC;

        if (yspeed > PLAYER_MAXJUMPSPEED)  // Schnellste "Fall-Geschwindigkeit" erreicht ?
            yspeed = PLAYER_MAXJUMPSPEED;

        if (InLiquid == false) {
            if (ypos < JumpStart - PLAYER_MAXJUMP ||  // Höchster Punkt erreicht ? Dann den Fall
                Aktion[AKTION_JUMP] == false)         // dazu addieren
            {
                // Der Wert, wie schnell man wieder runterkommt, wird jetzt aus der Sprunghöhe errechnet, sodass
                // man bei einem kleinen Sprung viel schneller wieder runterkommt
                //
                JumpAdd = PLAYER_JUMPADDSPEED;
                JumpAdd = PLAYER_JUMPADDSPEED + abs(static_cast<int>((160 - (JumpySave - ypos)) / 10.0f));

                if (JumpAdd > 18.0f)
                    JumpAdd = 18.0f;
            }
        } else {
            if (ypos < JumpStart - (PLAYER_MAXJUMP + 50) ||  // Höchster Punkt im Wasser liegt anders
                Aktion[AKTION_JUMP] == false)                // dazu addieren
                JumpAdd = PLAYER_JUMPADDSPEED;
        }
    }

    // Als Rad runterfallen
    if (Handlung == PlayerActionEnum::RADELN_FALL) {
        // Sprung-Geschwindigkeit manipulieren
        if (InLiquid == false)
            yspeed += JumpAdd SYNC;
        else
            yspeed += JumpAdd * 2 / 3 SYNC;

        // y-Position manipulieren, wenn oben frei ist
        if (InLiquid == false)
            ypos += yspeed SYNC;
        else
            ypos += yspeed * 2 / 3 SYNC;

        if (yspeed > PLAYER_MAXJUMPSPEED)  // Schnellste "Fall-Geschwindigkeit" erreicht ?
            yspeed = PLAYER_MAXJUMPSPEED;

        if (bo & BLOCKWERT_WAND)  // An die Decke gestossen ?
        {
            if (yspeed < 0.0f)  // Richtung umkehren
                yspeed = -yspeed / 3;
            JumpAdd = PLAYER_JUMPADDSPEED;
        }
    }

    // Spieler "rutscht" weg, weil Level schräg steht?
    if (bu & BLOCKWERT_WAND || bu & BLOCKWERT_PLATTFORM)
        xspeed += ScreenWinkel * 7.0f SYNC;

    // Level abhängig von der Spieler-Position scrollen
    if (xspeed != 0.0f) {
        // nur bewegen, wenn keine Wand im Weg ist
        if ((xspeed < 0.0f && !(bl & BLOCKWERT_WAND)) || (xspeed > 0.0f && !(br & BLOCKWERT_WAND)))
            xpos += xspeed SYNC;  // Spieler bewegen*/

        // Zwei Spieler Mode? Dann auf Screen beschränken
        if (NUMPLAYERS == 2 && StageClearRunning == false) {
            if (xpos < TileEngine.XOffset)
                xpos = static_cast<float>(TileEngine.XOffset);
            if (xpos > TileEngine.XOffset + 570)
                xpos = static_cast<float>(TileEngine.XOffset) + 570;
        }
    }

    /*if (Handlung != PlayerActionEnum::SPRINGEN	&&
        Handlung != PlayerActionEnum::RADELN_FALL &&
        yspeed <= 0.0f)
        TileEngine.YOffset -= PLAYER_MAXJUMPSPEED SYNC;*/

    if (NUMPLAYERS == 1 && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        if (xpos - TileEngine.XOffset < 20)
            TileEngine.XOffset = xpos - 20;
        if (xpos - TileEngine.XOffset > 550)
            TileEngine.XOffset = xpos - 550;
    }

    // DKS - By using a bounds-checked array for itsPreCalcedRects[] when debugging,
    //      I found AnimPhase was going out of bounds of the number of frames when
    //      Handlung==PlayerActionEnum::LAUFEN (running), at times. So, I've added some checks here
    //      to ensure that never happens:
    switch (Handlung) {
        case PlayerActionEnum::RADELN:
        case PlayerActionEnum::RADELN_FALL:
            if (AnimPhase >= 8)
                AnimPhase = 0;
            break;
        case PlayerActionEnum::LAUFEN:
            // DKS - This was the problematic one causing out-of-bounds access to itsPreCalcedRects
            if (AnimPhase >= FRAMES_RUN)
                AnimPhase = 0;
            break;
        case PlayerActionEnum::BLITZEN:
        case PlayerActionEnum::BEAMLADEN:
            if (AnimPhase >= 36)
                AnimPhase = 0;
            break;
        case PlayerActionEnum::DREHEN:
            if (AnimPhase >= 20)
                AnimPhase = 0;
            break;
        case PlayerActionEnum::SPRINGEN:
            if (AnimPhase >= 12)
                AnimPhase = 0;
            break;
        case PlayerActionEnum::PISSEN:
            if (AnimPhase >= 18)
                AnimPhase = 0;
            break;
        default:
            break;
    }
}

// --------------------------------------------------------------------------------------
// Aktionen, wenn der Spieler auf einer Plattform steht
// --------------------------------------------------------------------------------------

void PlayerClass::DoPlattformStuff() {
    if (AufPlattform == nullptr)
        return;

    if (AufPlattform->GegnerArt == REITFLUGSACK)
        return;

    if (Handlung == PlayerActionEnum::SPRINGEN) {
        Handlung = PlayerActionEnum::STEHEN;
        yspeed = 0.0f;
    }

    if (Aktion[AKTION_JUMP] == false)
        JumpPossible = true;

    // Fahrstuhl über Boden gefahren? Dann bleibt der Spieler daran hängen
    if (bu & BLOCKWERT_WAND || bu & BLOCKWERT_PLATTFORM) {
        AufPlattform = nullptr;
    } else {
        if (AufPlattform->GegnerArt == FAHRSTUHL)
            ypos = g_Fahrstuhl_yPos + GegnerRect[AufPlattform->GegnerArt].top - CollideRect.bottom;
        else
            ypos = AufPlattform->yPos + GegnerRect[AufPlattform->GegnerArt].top - CollideRect.bottom;
    }
}

// --------------------------------------------------------------------------------------
// Spieler an der aktuellen Position zeichnen, je nach Zustand
//
//	leuchten : Spieler additiv rendern
//  farbe    : mit Farbe "CurrentColor" rendern
// --------------------------------------------------------------------------------------

bool PlayerClass::DrawPlayer(bool leuchten, bool farbe) {
    if (Energy <= 0.0f && Handlung == PlayerActionEnum::TOT)
        return false;

    float xdraw, ydraw;
    D3DCOLOR Color;

    if (leuchten)
        DirectGraphics.SetAdditiveMode();

    xdraw = static_cast<float>(static_cast<int>(xpos) - static_cast<int>(TileEngine.XOffset));
    ydraw = ypos - TileEngine.YOffset;

    // Im Wasser? Dann schwabbeln lassen
    // DKS - This original block of code never had an effect because there are no platforms
    //      underwater that I know of. Also, when I had it also execute when standing on
    //      a BLOCKWERT_WAND tile, I discovered the left/right swaying was very ugly
    //      and not even in coordination with other artwork lying in water. I've disabled
    //      it entirely:
#if 0
    if (InLiquid == true &&
            bo & BLOCKWERT_LIQUID &&
            bu & BLOCKWERT_PLATTFORM)
    {
        int yLevel = static_cast<int>(ypos + 80.0f) / 20;
        int off = static_cast<int>(TileEngine.SinPos2 + yLevel) % 40;

        xdraw -= TileEngine.SinList2[off];
    }
#endif  // 0

    // Schaden genommen ? Dann Spieler blinken lassen
    if (DamageCounter > 0.0f)
        DamageCounter -= 5.0f SYNC;
    else
        DamageCounter = 0.0f;

    // DKS - No need to check if it is playing, just ask it to stop:
    // if (DamageCounter == 0.0f &&
    //        true == SoundManager.InitSuccessfull &&
    //        SoundManager.its_Sounds[SOUND_ABZUG + SoundOff]->isPlaying == true)
    //    SoundManager.StopWave(SOUND_ABZUG + SoundOff);
    if (DamageCounter == 0.0f)
        SoundManager.StopWave(SOUND_ABZUG + SoundOff);

    Color = TileEngine.LightValue(xpos, ypos, CollideRect, false);

    if (Handlung == PlayerActionEnum::BLITZEN)
        Color = Color | 0xFF0000FF;

    if (ShotDelay > 1.0f)
        Color = 0xFFFFFFFF;

    // Spieler nicht normal rendern (zB wenn er blinkt nach dem Waffeneinsammeln)
    // dann entsprechende Farbe setzen
    if (farbe)
        Color = CurrentColor;

    //	if (this == Player[1])
    //		Color = 0xFFBBFFBB;

    bool blick = false;
    if (Blickrichtung == RECHTS)
        blick = true;

    //----- Spieler anzeigen
    switch (Handlung) {
            // Spieler kullert (oder fällt) als Rad durch die Gegend
        case PlayerActionEnum::RADELN:
        case PlayerActionEnum::RADELN_FALL: {
            PlayerRad.RenderSprite(xdraw + 17, ydraw + 45, AnimPhase, Color, blick);
        } break;

            // Spieler steht und macht nichts
        case PlayerActionEnum::STEHEN: {
            if (Aktion[AKTION_UNTEN] && !Aktion[AKTION_SHOOT] && !Aktion[AKTION_GRANATE])
                PlayerKucken.RenderSprite(xdraw, ydraw, 1, Color, !blick);
            else {
                int a;

                if (AnimPhase < FRAMES_IDLE * 3) {
                    a = AnimPhase % FRAMES_IDLE;

                    if (ShotDelay > 0.25f && FlameThrower == false)
                        PlayerIdle.RenderSprite(xdraw, ydraw, FRAMES_IDLE, Color, !blick);
                    else
                        PlayerIdle.RenderSprite(xdraw, ydraw, a, Color, !blick);
                } else {
                    a = AnimPhase - FRAMES_IDLE * 3;
                    PlayerIdle2.RenderSprite(xdraw, ydraw, a, Color, !blick);
                }
            }

        } break;

            // Spieler zielt nach oben
        case PlayerActionEnum::SCHIESSEN_O: {
            if (Aktion[AKTION_SHOOT] || Aktion[AKTION_GRANATE]) {
                if (ShotDelay > 0.25f && FlameThrower == false)
                    PlayerOben.RenderSprite(xdraw, ydraw, 1, Color, !blick);
                else
                    PlayerOben.RenderSprite(xdraw, ydraw, 0, Color, !blick);
            } else
                PlayerKucken.RenderSprite(xdraw, ydraw, 0, Color, !blick);
        } break;

            // Spieler zielt nach links oder rechts oben
        case PlayerActionEnum::SCHIESSEN_LO:
        case PlayerActionEnum::SCHIESSEN_RO: {
            // im Stehen schiessen?
            if (WalkLock == true) {
                if (ShotDelay > 0.25f && FlameThrower == false)
                    PlayerDiagonal.RenderSprite(xdraw, ydraw, 1, Color, !blick);
                else
                    PlayerDiagonal.RenderSprite(xdraw, ydraw, 0, Color, !blick);
            }

            /*
                        // oder im Laufen?
                        else
                        {
                            if (ShotDelay > 0.25f)
                                PlayerRunDiagonal[AnimPhase].RenderSprite(xdraw, ydraw, 0, Color, !blick);
                            else
                                PlayerRunDiagonal[AnimPhase].RenderSprite(xdraw, ydraw, 0, Color, !blick);
                        }
            */
        } break;

        // Spieler zielt nach links oder rechts unten
        /*		case SCHIESSEN_LU:
                case SCHIESSEN_RU:
                {
                    PlayerBlitz.RenderSprite(xdraw, ydraw, 4, Color, blick);
                } break;*/

        // DKS - This "surfing" appears never to have been implemented (no image for it) so disabled it:
#if 0
// Spieler surft
    case PlayerActionEnum::SURFEN :
    {
        PlayerSurf.RenderSprite(xdraw, ydraw, 0, Color, false);
    }
    break;

// Spieler springt bei surfen
    case PlayerActionEnum::SURFENJUMP :
    {
        PlayerSurf.RenderSprite(xdraw, ydraw, 0, Color, false);
    }
    break;

// Spieler surft im Kien
    case PlayerActionEnum::SURFENCROUCH :
    {
        PlayerSurf.RenderSprite(xdraw, ydraw, 1, Color, false);
    }
    break;
#endif  // 0

            // Spieler springt
        case PlayerActionEnum::SPRINGEN: {
            if (Aktion[AKTION_OBEN]) {
                if (Aktion[AKTION_LINKS] || Aktion[AKTION_RECHTS])
                    PlayerJumpDiagonal.RenderSprite(xdraw, ydraw, AnimPhase, Color, !blick);
                else
                    PlayerJumpUp.RenderSprite(xdraw, ydraw, AnimPhase, Color, !blick);
            } else
                PlayerJump.RenderSprite(xdraw, ydraw, AnimPhase, Color, !blick);

        } break;

            // Spieler läuft
        case PlayerActionEnum::LAUFEN: {
            PlayerRun.RenderSprite(xdraw, ydraw, AnimPhase, Color, !blick);
        } break;

            // Spieler muss brutal pullern ?
        case PlayerActionEnum::PISSEN: {
            PlayerPiss.RenderSprite(xdraw, ydraw, AnimPhase, Color, !blick);
        } break;

            // Spieler schiesst Blitz oder lädt den Beam
        case PlayerActionEnum::BLITZEN:
        case PlayerActionEnum::BEAMLADEN: {
            PlayerBlitz.RenderSprite(xdraw, ydraw, AnimPhase, Color, !blick);
        } break;

            // Spieler duckt sich
        case PlayerActionEnum::DUCKEN: {
            if (ShotDelay > 0.25f)
                PlayerCrouch.RenderSprite(xdraw, ydraw, 1, Color, !blick);
            else
                PlayerCrouch.RenderSprite(xdraw, ydraw, 0, Color, !blick);
        } break;

            // Spieler reitet auf dem FlugSack
        case PlayerActionEnum::SACKREITEN: {
            if (Blickrichtung == LINKS)
                PlayerRide.RenderSprite(xdraw, ydraw, 0, Color);
            else if (Blickrichtung == RECHTS)
                PlayerRide.RenderSprite(xdraw, ydraw, 10, Color);
        } break;

        case PlayerActionEnum::DREHEN: {
            PlayerRide.RenderSprite(xdraw, ydraw, AnimPhase, Color);
        } break;

        default:
            break;
    }

    //----- Schuss-Flamme anzeigen
    if (FlameTime > 0.0f && leuchten == false && farbe == false)

    {
        // Zeit der Darstellung verringern
        //
        FlameTime -= SpeedFaktor;

        DirectGraphics.SetAdditiveMode();
        CalcFlamePos();

        int FlameOff = SelectedWeapon;

        if (FlameThrower)
            FlameOff = 1;

        // DKS - Added check that AustrittAnim was between 0-2, because in the original code,
        //      SchussFlamme[] had 4 elements, but only the first 3 elements actually were
        //      ever loaded with textures.
        if (AustrittAnim >= 0 && AustrittAnim < 3) {
            if (Blickrichtung == RECHTS)
                Projectiles.SchussFlamme[AustrittAnim].RenderSprite(xpos + AustrittX, ypos + AustrittY,
                                                                    FlameAnim + FlameOff * 2, 0xFFFFFFFF);
            else
                Projectiles.SchussFlamme[AustrittAnim].RenderSprite(xpos + AustrittX, ypos + AustrittY,
                                                                    FlameAnim + FlameOff * 2, 0xFFFFFFFF, true);
        }

        CalcAustrittsPunkt();

        if (options_Detail >= DETAIL_MEDIUM) {
            FlameOff = SelectedWeapon;
            if (FlameThrower)
                FlameOff = 0;

            switch (FlameOff) {
                case 0:
                    Projectiles.SchussFlammeFlare.RenderSprite(
                        xpos + AustrittX - 70 - static_cast<float>(TileEngine.XOffset),
                        ypos + AustrittY - 70 - static_cast<float>(TileEngine.YOffset), 0, 0x88FFCC99);
                    break;

                case 1:
                    Projectiles.SchussFlammeFlare.RenderSprite(
                        xpos + AustrittX - 70 - static_cast<float>(TileEngine.XOffset),
                        ypos + AustrittY - 70 - static_cast<float>(TileEngine.YOffset), 0, 0x8899CCFF);
                    break;

                case 2:
                    Projectiles.SchussFlammeFlare.RenderSprite(
                        xpos + AustrittX - 70 - static_cast<float>(TileEngine.XOffset),
                        ypos + AustrittY - 70 - static_cast<float>(TileEngine.YOffset), 0, 0x8899FFCC);
                    break;
            }
        }

        DirectGraphics.SetColorKeyMode();
    }

    // Blitz anzeigen oder Beam aufladen
    //
    if (Handlung == PlayerActionEnum::BLITZEN && AlreadyDrawn == false)
        DoLightning();

    if (Handlung == PlayerActionEnum::BEAMLADEN && AlreadyDrawn == false)
        LoadBeam();

    if (leuchten)
        DirectGraphics.SetColorKeyMode();

    // Funken und so Kram, wenn der Spieler verwundet ist
    //
    if (Console.Showing == false)
        DoStuffWhenDamaged();

    CurrentShotTexture = 0;
    CurrentPartikelTexture = -1;

    AlreadyDrawn = true;

    return true;
}

// --------------------------------------------------------------------------------------
// Spieler mit aktueller Speed bewegen
// --------------------------------------------------------------------------------------

void PlayerClass::MovePlayer() {
    // wenn der Spieler gerade fernegesteuert das Level verlässt dann gleich wieder raus
    if (DoFesteAktion == true || Handlung == PlayerActionEnum::TOT)
        return;

    // Ränder überprüfen
    // links raus
    if (xpos < 0.0f)
        xpos = 0.0f;

    // rechts raus
    if (xpos > TileEngine.LEVELPIXELSIZE_X - 80.0f)
        xpos = static_cast<float>(TileEngine.LEVELPIXELSIZE_X - 80.0f);

    // unten raus
    if (ypos > TileEngine.LEVELPIXELSIZE_Y)
        Energy = 0.0f;

    // im 2 Spieler-Mode: Ein Spieler fällt aus dem Screen?
    // Geht nur, wenn beide noch leben
    //
    if (NUMPLAYERS == 2 && Player[0].Handlung !=PlayerActionEnum:: TOT &&
            Player[1].Handlung != PlayerActionEnum::TOT) {
        PlayerClass *pVictim = nullptr;
        PlayerClass *pSurvivor = nullptr;

        // Spieler 1 fällt unten raus?
        if (Player[0].ypos > Player[1].ypos + 480.0f) {
            pVictim = &Player[0];
            pSurvivor = &Player[1];
        }

        // Spieler 2 fällt unten raus?
        if (Player[1].ypos > Player[0].ypos + 480.0f) {
            pVictim = &Player[1];
            pSurvivor = &Player[0];
        }

        // Einer der Spieler fällt raus
        if (pVictim != nullptr) {
            pVictim->xpos = pSurvivor->xpos;
            pVictim->ypos = pSurvivor->ypos - 20.0f;
        }
    }

    // an die Decke
    if (ypos < 0.0f) {
        if (yspeed < 0.0f)
            yspeed = -yspeed * 2 / 3;
        else
            ypos = 0.0f;
    }

    // Ränder für gelockten Screen prüfen
    if (TileEngine.Zustand != TileStateEnum::SCROLLBAR) {
        if (xpos < TileEngine.XOffset)
            xpos = static_cast<float>(TileEngine.XOffset);
        if (xpos > TileEngine.XOffset + 580)
            xpos = static_cast<float>(TileEngine.XOffset + 580);

        // Im Fahrstuhllevel?
        //
        if (g_Fahrstuhl_yPos > -1.0f) {
            // ja, dann checken ob Spieler ausserhalb des Screens, und wenn ja, dann
            // lassen wir ihn halt mal einfach so sterben. Das gehört sich ja auch nicht ;)
            //
            if (ypos + CollideRect.bottom < TileEngine.YOffset || ypos > TileEngine.YOffset + 480.0f)
                Energy = 0.0f;
        } else {
            if (ypos < TileEngine.YOffset)
                ypos = static_cast<float>(TileEngine.YOffset);
        }
    }
}

// --------------------------------------------------------------------------------------
// Spieler schiesst mit Primär Waffe
// --------------------------------------------------------------------------------------

void PlayerClass::PlayerShoot() {
    float wadd = 0.0f;
    int yoff;

    CalcAustrittsPunkt();

    //----- Y-Offset der Patronenhülse ausrechnen

    // DKS - This appears never to have been implemented (no image for it) so disabled it:
#if 0
    // surfen
    if (Handlung == PlayerActionEnum::SURFEN ||
            Handlung == PlayerActionEnum::SURFENJUMP)
        yoff = -13;
    else if (Handlung == PlayerActionEnum::SURFENCROUCH)
        yoff = -1;

    // andere Handlung
    else
#endif  // 0
    {
        if (Handlung == PlayerActionEnum::DUCKEN)
            yoff = 23;
        else
            yoff = 0;
    }

    //----- Richung des Schusses ermitteln

    // Hurri zielt nach oben
    //
    if (Handlung == PlayerActionEnum::SCHIESSEN_O ||
        (Handlung == PlayerActionEnum::SPRINGEN && Aktion[AKTION_OBEN] &&
        !Aktion[AKTION_LINKS] && !Aktion[AKTION_RECHTS])) {
        if (Blickrichtung == LINKS)
            wadd = 90.0f;
        else
            wadd = -90.0f;
    } else

        // Hurri zielt nach links oben
        //
        if (Handlung == PlayerActionEnum::SCHIESSEN_LO ||
            (Handlung == PlayerActionEnum::SPRINGEN && Aktion[AKTION_OBEN] && Aktion[AKTION_LINKS]))
        wadd = 45.0f;
    else

        // Hurri zielt nach rechts oben
        //
        if (Handlung == PlayerActionEnum::SCHIESSEN_RO ||
            (Handlung == PlayerActionEnum::SPRINGEN && Aktion[AKTION_OBEN] && Aktion[AKTION_RECHTS]))
        wadd = -45.0f;

    if (Handlung != PlayerActionEnum::RADELN &&  // Normal schiessen ?
        Handlung != PlayerActionEnum::RADELN_FALL) {
        int tempadd = 0;
        int tempshot = 0;

        if (FlameThrower) {
            WinkelUebergabe = 90.0f + wadd;
            Projectiles.PushProjectile(xpos - tempadd + AustrittX - 29, ypos - tempadd + AustrittY - 33, PLAYERFIRE,
                                       this);
            ShotDelay = PLAYER_SHOTDELAY / 5.0f;

            // DKS - Added function WaveIsPlaying() to SoundManagerClass:
            if (!SoundManager.WaveIsPlaying(SOUND_FLAMETHROWER + SoundOff))
                SoundManager.PlayWave(100, 128, 11025, SOUND_FLAMETHROWER + SoundOff);
        } else
            switch (SelectedWeapon) {
                    //----- Spread Shot

                case 0: {
                    // Normale Schüsse
                    //
                    if (RiesenShotExtra <= 0.0f) {
                        tempadd = 8;
                        tempshot = SPREADSHOT;
                        SoundManager.PlayWave(100, 128, 11025, SOUND_SPREADSHOT);
                    } else {
                        tempadd = 16;
                        tempshot = SPREADSHOTBIG;
                        SoundManager.PlayWave(100, 128, 8000, SOUND_SPREADSHOT);
                    }

                    WinkelUebergabe = 90.0f + wadd;

                    if (CurrentWeaponLevel[SelectedWeapon] < 5)
                        Projectiles.PushProjectile(xpos - tempadd + AustrittX, ypos - tempadd + AustrittY, tempshot,
                                                   this);

                    if (CurrentWeaponLevel[SelectedWeapon] >= 5)
                        Projectiles.PushProjectile(xpos - tempadd + AustrittX, ypos - tempadd + AustrittY, tempshot + 1,
                                                   this);

                    if (CurrentWeaponLevel[SelectedWeapon] > 1 && CurrentWeaponLevel[SelectedWeapon] < 6) {
                        WinkelUebergabe = 82.0f + wadd;
                        Projectiles.PushProjectile(xpos - tempadd + AustrittX, ypos - tempadd + AustrittY, tempshot,
                                                   this);

                        WinkelUebergabe = 98.0f + wadd;
                        Projectiles.PushProjectile(xpos - tempadd + AustrittX, ypos - tempadd + AustrittY, tempshot,
                                                   this);
                    }

                    if (CurrentWeaponLevel[SelectedWeapon] >= 6) {
                        WinkelUebergabe = 82.0f + wadd;
                        Projectiles.PushProjectile(xpos - tempadd + AustrittX, ypos - tempadd + AustrittY, tempshot + 1,
                                                   this);

                        WinkelUebergabe = 98.0f + wadd;
                        Projectiles.PushProjectile(xpos - tempadd + AustrittX, ypos - tempadd + AustrittY, tempshot + 1,
                                                   this);
                    }

                    if (CurrentWeaponLevel[SelectedWeapon] > 2 && CurrentWeaponLevel[SelectedWeapon] < 7) {
                        WinkelUebergabe = 74.0f + wadd;
                        Projectiles.PushProjectile(xpos - tempadd + AustrittX, ypos - tempadd + AustrittY, tempshot,
                                                   this);

                        WinkelUebergabe = 106.0f + wadd;
                        Projectiles.PushProjectile(xpos - tempadd + AustrittX, ypos - tempadd + AustrittY, tempshot,
                                                   this);
                    }

                    if (CurrentWeaponLevel[SelectedWeapon] >= 7) {
                        WinkelUebergabe = 74.0f + wadd;
                        Projectiles.PushProjectile(xpos - tempadd + AustrittX, ypos - tempadd + AustrittY, tempshot + 1,
                                                   this);

                        WinkelUebergabe = 106.0f + wadd;
                        Projectiles.PushProjectile(xpos - tempadd + AustrittX, ypos - tempadd + AustrittY, tempshot + 1,
                                                   this);
                    }

                    if (CurrentWeaponLevel[SelectedWeapon] > 3 && CurrentWeaponLevel[SelectedWeapon] < 8) {
                        WinkelUebergabe = 66.0f + wadd;
                        Projectiles.PushProjectile(xpos - tempadd + AustrittX, ypos - tempadd + AustrittY, tempshot,
                                                   this);

                        WinkelUebergabe = 114.0f + wadd;
                        Projectiles.PushProjectile(xpos - tempadd + AustrittX, ypos - tempadd + AustrittY, tempshot,
                                                   this);
                    }

                    if (CurrentWeaponLevel[SelectedWeapon] >= 8) {
                        WinkelUebergabe = 66.0f + wadd;
                        Projectiles.PushProjectile(xpos - tempadd + AustrittX, ypos - tempadd + AustrittY, tempshot + 1,
                                                   this);

                        WinkelUebergabe = 114.0f + wadd;
                        Projectiles.PushProjectile(xpos - tempadd + AustrittX, ypos - tempadd + AustrittY, tempshot + 1,
                                                   this);
                    }
                } break;

                    //----- LaserShot

                case 1: {
                    int tempaddx, tempaddy;
                    float mul1 = 7.0f / 8.0f;

                    // Normale Schüsse
                    //
                    if (RiesenShotExtra <= 0.0f) {
                        tempaddx = 8;
                        tempaddy = 24;
                        tempshot = LASERSHOT;
                        SoundManager.PlayWave(100, 128, 11025, SOUND_LASERSHOT);
                    } else {
                        tempaddx = 12;
                        tempaddy = 40;
                        tempshot = LASERSHOTBIG;
                        SoundManager.PlayWave(100, 128, 8000, SOUND_LASERSHOT);
                    }

                    WinkelUebergabe = 90.0f + wadd;

                    if (CurrentWeaponLevel[SelectedWeapon] == 4 || CurrentWeaponLevel[SelectedWeapon] == 6)
                        Projectiles.PushProjectile(xpos - tempaddx + AustrittX - 4, ypos - tempaddy + AustrittY,
                                                   tempshot + 1, this);

                    if (CurrentWeaponLevel[SelectedWeapon] == 1 || CurrentWeaponLevel[SelectedWeapon] == 3 ||
                        CurrentWeaponLevel[SelectedWeapon] == 5)
                        Projectiles.PushProjectile(xpos - tempaddx + AustrittX, ypos - tempaddy + AustrittY, tempshot,
                                                   this);

                    if (CurrentWeaponLevel[SelectedWeapon] == 2) {
                        if (wadd == 0.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX,
                                                       ypos - tempaddy + AustrittY - tempaddx * mul1, tempshot, this);
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX,
                                                       ypos - tempaddy + AustrittY + tempaddx * mul1, tempshot, this);
                        }

                        if (wadd == 45.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx + tempaddx * mul1 + AustrittX - 2,
                                                       ypos - tempaddy + AustrittY - tempaddx * mul1, tempshot, this);
                            Projectiles.PushProjectile(xpos - tempaddx - tempaddx * mul1 + AustrittX + 2,
                                                       ypos - tempaddy + AustrittY + tempaddx * mul1, tempshot, this);
                        }

                        if (wadd == -45.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx - tempaddx * mul1 + AustrittX + 6,
                                                       ypos - tempaddy + AustrittY - tempaddx * mul1, tempshot, this);
                            Projectiles.PushProjectile(xpos - tempaddx + tempaddx * mul1 + AustrittX + 2,
                                                       ypos - tempaddy + AustrittY + tempaddx * mul1, tempshot, this);
                        }

                        if (wadd == 90.0f || wadd == -90.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX - tempaddx * mul1,
                                                       ypos - tempaddy + AustrittY, tempshot, this);
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX + tempaddx * mul1,
                                                       ypos - tempaddy + AustrittY, tempshot, this);
                        }
                    }

                    if (CurrentWeaponLevel[SelectedWeapon] >= 7) {
                        if (wadd == 0.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX,
                                                       ypos - tempaddy + AustrittY - tempaddx * mul1, tempshot + 1,
                                                       this);
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX,
                                                       ypos - tempaddy + AustrittY + tempaddx * mul1, tempshot + 1,
                                                       this);
                        }

                        if (wadd == 45.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx + tempaddx * mul1 * 0.75f + AustrittX,
                                                       ypos - tempaddy + AustrittY - tempaddx * mul1 * 0.75f,
                                                       tempshot + 1, this);
                            Projectiles.PushProjectile(xpos - tempaddx - tempaddx * mul1 * 0.75f + AustrittX,
                                                       ypos - tempaddy + AustrittY + tempaddx * mul1 * 0.75f,
                                                       tempshot + 1, this);
                        }

                        if (wadd == -45.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx - tempaddx * mul1 * 0.75f + AustrittX,
                                                       ypos - tempaddy + AustrittY - tempaddx * mul1 * 0.75f,
                                                       tempshot + 1, this);
                            Projectiles.PushProjectile(xpos - tempaddx + tempaddx * mul1 * 0.75f + AustrittX,
                                                       ypos - tempaddy + AustrittY + tempaddx * mul1 * 0.75f,
                                                       tempshot + 1, this);
                        }

                        if (wadd == 90.0f || wadd == -90.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX - tempaddx * mul1,
                                                       ypos - tempaddy + AustrittY, tempshot + 1, this);
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX + tempaddx * mul1,
                                                       ypos - tempaddy + AustrittY, tempshot + 1, this);
                        }
                    }

                    if (CurrentWeaponLevel[SelectedWeapon] >= 3 && CurrentWeaponLevel[SelectedWeapon] <= 4) {
                        if (wadd == 0.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX - tempaddx * Blickrichtung,
                                                       ypos - tempaddy - tempaddx * mul1 * 2.3f + AustrittY, tempshot,
                                                       this);
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX - tempaddx * Blickrichtung,
                                                       ypos - tempaddy + tempaddx * mul1 * 2.3f + AustrittY, tempshot,
                                                       this);
                        }

                        if (wadd == 90.0f || wadd == -90.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx - tempaddx * mul1 * 2.3f + AustrittX,
                                                       ypos - tempaddy / 2 + AustrittY, tempshot, this);
                            Projectiles.PushProjectile(xpos - tempaddx + tempaddx * mul1 * 2.3f + AustrittX,
                                                       ypos - tempaddy / 2 + AustrittY, tempshot, this);
                        }

                        if (wadd == 45.0f) {
                            Projectiles.PushProjectile(xpos + tempaddx * mul1 * 2.3f + AustrittX,
                                                       ypos - tempaddy + AustrittY, tempshot, this);
                            Projectiles.PushProjectile(xpos + tempaddx - tempaddx * mul1 * 2.3f + AustrittX,
                                                       ypos - tempaddy + tempaddx * 3 + AustrittY, tempshot, this);
                        }

                        if (wadd == -45.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx * mul1 * 4 + AustrittX,
                                                       ypos - tempaddy + AustrittY, tempshot, this);
                            Projectiles.PushProjectile(xpos - tempaddx * 2 + tempaddx * mul1 * 2.3f + AustrittX,
                                                       ypos - tempaddy + tempaddx * 2 + AustrittY, tempshot, this);
                        }
                    }

                    if (CurrentWeaponLevel[SelectedWeapon] == 5 || CurrentWeaponLevel[SelectedWeapon] == 6) {
                        if (wadd == 0.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX - tempaddx * Blickrichtung * 2,
                                                       ypos - tempaddy + AustrittY - tempaddx * mul1 * 2, tempshot + 1,
                                                       this);
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX - tempaddx * Blickrichtung * 2,
                                                       ypos - tempaddy + AustrittY + tempaddx * mul1 * 2, tempshot + 1,
                                                       this);
                        }

                        if (wadd == 90.0f || wadd == -90.0f) {
                            Projectiles.PushProjectile(xpos + tempaddx * mul1 * 2 - tempaddx + AustrittX - 4,
                                                       ypos - tempaddy + AustrittY + tempaddx, tempshot + 1, this);
                            Projectiles.PushProjectile(xpos - tempaddx * mul1 * 2 - tempaddx + AustrittX - 4,
                                                       ypos - tempaddy + AustrittY + tempaddx, tempshot + 1, this);
                        }

                        if (wadd == 45.0f) {
                            Projectiles.PushProjectile(xpos + tempaddx + AustrittX - 4, ypos - tempaddy + AustrittY,
                                                       tempshot + 1, this);
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX - 4,
                                                       ypos - tempaddy + tempaddx * 2 + AustrittY, tempshot + 1, this);
                        }

                        if (wadd == -45.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx * 3 + AustrittX - 6, ypos - tempaddy + AustrittY,
                                                       tempshot + 1, this);
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX - 4,
                                                       ypos - tempaddy + tempaddx * 2 + AustrittY, tempshot + 1, this);
                        }
                    }

                    if (CurrentWeaponLevel[SelectedWeapon] == 7) {
                        if (wadd == 0.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX - tempaddx * Blickrichtung * 2,
                                                       ypos - tempaddy + AustrittY - tempaddx * mul1 * 3, tempshot,
                                                       this);
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX - tempaddx * Blickrichtung * 2,
                                                       ypos - tempaddy + AustrittY + tempaddx * mul1 * 3, tempshot,
                                                       this);
                        }

                        if (wadd == 45.0f) {
                            Projectiles.PushProjectile(xpos + tempaddx * 2 + AustrittX + 6, ypos - tempaddy + AustrittY,
                                                       tempshot, this);
                            Projectiles.PushProjectile(xpos - tempaddx * 2 + AustrittX + 4,
                                                       ypos - tempaddy + tempaddx * 5 / 2 + AustrittY, tempshot, this);
                        }

                        if (wadd == -45.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx * 4 + AustrittX + 2, ypos - tempaddy + AustrittY,
                                                       tempshot, this);
                            Projectiles.PushProjectile(xpos + AustrittX + 4,
                                                       ypos - tempaddy + tempaddx * 5 / 2 + AustrittY, tempshot, this);
                        }

                        if (wadd == 90.0f || wadd == -90.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx - tempaddy * mul1 + AustrittX + 3,
                                                       ypos - tempaddy / 2 + AustrittY, tempshot, this);
                            Projectiles.PushProjectile(xpos - tempaddx + tempaddy * mul1 + AustrittX + 4,
                                                       ypos - tempaddy / 2 + AustrittY, tempshot, this);
                        }
                    }

                    if (CurrentWeaponLevel[SelectedWeapon] == 8) {
                        if (wadd == 0.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX - tempaddx * Blickrichtung * 2,
                                                       ypos - tempaddy + AustrittY - tempaddx * 3, tempshot + 1, this);
                            Projectiles.PushProjectile(xpos - tempaddx + AustrittX - tempaddx * Blickrichtung * 2,
                                                       ypos - tempaddy + AustrittY + tempaddx * 3, tempshot + 1, this);
                        }

                        if (wadd == 45.0f) {
                            Projectiles.PushProjectile(xpos + tempaddx * 3 + AustrittX, ypos - tempaddy + AustrittY,
                                                       tempshot + 1, this);
                            Projectiles.PushProjectile(xpos - tempaddx * 2 + AustrittX,
                                                       ypos - tempaddy + tempaddx * 3 + AustrittY, tempshot + 1, this);
                        }

                        if (wadd == -45.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx * 4.5f + AustrittX, ypos - tempaddy + AustrittY,
                                                       tempshot + 1, this);
                            Projectiles.PushProjectile(xpos + AustrittX, ypos - tempaddy + tempaddx * 3 + AustrittY,
                                                       tempshot + 1, this);
                        }

                        if (wadd == 90.0f || wadd == -90.0f) {
                            Projectiles.PushProjectile(xpos - tempaddx - tempaddy * mul1 + AustrittX,
                                                       ypos - tempaddy / 2 + AustrittY, tempshot + 1, this);
                            Projectiles.PushProjectile(xpos - tempaddx + tempaddy * mul1 + AustrittX,
                                                       ypos - tempaddy / 2 + AustrittY, tempshot + 1, this);
                        }
                    }

                } break;

                    //----- BounceShot

                case 2: {
                    int tempadd2;

                    // Normale Schüsse
                    //
                    if (RiesenShotExtra <= 0.0f) {
                        tempadd2 = 12;
                        tempshot = BOUNCESHOT1;

                        SoundManager.PlayWave(100, 128, 11025, SOUND_BOUNCESHOT);
                    } else {
                        tempadd2 = 24;
                        tempshot = BOUNCESHOTBIG1;
                        SoundManager.PlayWave(100, 128, 8000, SOUND_BOUNCESHOT);
                    }

                    WinkelUebergabe = 90.0f + wadd;

                    // if (CurrentWeaponLevel[SelectedWeapon] <= 5)
                    Projectiles.PushProjectile(xpos - tempadd2 + AustrittX, ypos - tempadd2 + AustrittY, tempshot, this);

                    /*
                    if (CurrentWeaponLevel[SelectedWeapon] == 5)
                    {
                        if (wadd == 0.0f)
                        {
                            Projectiles.PushProjectile(xpos-tempadd2+AustrittX + 4, ypos-tempadd2+AustrittY-11,
                    tempshot+1, this); Projectiles.PushProjectile(xpos-tempadd2+AustrittX + 4, ypos-tempadd2+AustrittY+19,
                    tempshot+1, this);
                        }
                        else
                        if (wadd ==  90.0f ||
                            wadd == -90.0f)
                        {
                            Projectiles.PushProjectile(xpos-tempadd2+AustrittX-11, ypos-tempadd2+AustrittY+4, tempshot+1,
                    this); Projectiles.PushProjectile(xpos-tempadd2+AustrittX+19, ypos-tempadd2+AustrittY+4, tempshot+1,
                    this);
                        }
                        else
                        if (wadd == 45.0f)
                        {
                            Projectiles.PushProjectile(xpos-tempadd2+AustrittX-7, ypos-tempadd2+AustrittY+17, tempshot+1,
                    this); Projectiles.PushProjectile(xpos-tempadd2+AustrittX+17, ypos-tempadd2+AustrittY-7, tempshot+1,
                    this);
                        }
                        else
                        if (wadd == -45.0f)
                        {
                            Projectiles.PushProjectile(xpos-tempadd2+AustrittX-7, ypos-tempadd2+AustrittY-7, tempshot+1,
                    this); Projectiles.PushProjectile(xpos-tempadd2+AustrittX+17, ypos-tempadd2+AustrittY+15, tempshot+1,
                    this);
                        }
                    }

                    if (CurrentWeaponLevel[SelectedWeapon] > 5)
                    {
                        if (wadd ==  0.0f)
                        {
                            Projectiles.PushProjectile(xpos-tempadd2+AustrittX, ypos-tempadd2+AustrittY-10, tempshot,
                    this); Projectiles.PushProjectile(xpos-tempadd2+AustrittX, ypos-tempadd2+AustrittY+10, tempshot,
                    this);
                        }
                        else
                        if (wadd ==  90.0f ||
                            wadd == -90.0f)
                        {
                            Projectiles.PushProjectile(xpos-tempadd2+AustrittX-10, ypos-tempadd2+AustrittY, tempshot,
                    this); Projectiles.PushProjectile(xpos-tempadd2+AustrittX+10, ypos-tempadd2+AustrittY, tempshot,
                    this);
                        }

                        if (wadd == -45.0f)
                        {
                            Projectiles.PushProjectile(xpos-tempadd2+AustrittX-7, ypos-tempadd2+AustrittY-7, tempshot,
                    this); Projectiles.PushProjectile(xpos-tempadd2+AustrittX+7, ypos-tempadd2+AustrittY+7, tempshot,
                    this);
                        }
                        else
                        if (wadd == 45.0f)
                        {
                            Projectiles.PushProjectile(xpos-tempadd2+AustrittX+7, ypos-tempadd2+AustrittY-7, tempshot,
                    this); Projectiles.PushProjectile(xpos-tempadd2+AustrittX-7, ypos-tempadd2+AustrittY+7, tempshot,
                    this);
                        }
                    }

                    if (CurrentWeaponLevel[SelectedWeapon] == 7)
                    {
                        WinkelUebergabe = 60.0f + wadd;
                        Projectiles.PushProjectile(xpos-tempadd2+AustrittX, ypos-tempadd2+AustrittY, tempshot+2, this);

                        WinkelUebergabe = 120.0f + wadd;
                        Projectiles.PushProjectile(xpos-tempadd2+AustrittX, ypos-tempadd2+AustrittY, tempshot+2, this);
                    }

                    if (CurrentWeaponLevel[SelectedWeapon] == 8)
                    {
                        WinkelUebergabe = 60.0f + wadd;
                        Projectiles.PushProjectile(xpos-tempadd2+AustrittX, ypos-tempadd2+AustrittY, tempshot+1, this);

                        WinkelUebergabe = 120.0f + wadd;
                        Projectiles.PushProjectile(xpos-tempadd2+AustrittX, ypos-tempadd2+AustrittY, tempshot+1, this);
                    }
                    */

                } break;

                default:
                    break;
            }  // switch

        if (!FlameThrower) {
            // Patronenhülse auswerfen bei SpreadShot
            if (SelectedWeapon == 0)
                PartikelSystem.PushPartikel(xpos + 30, ypos + 24 + yoff, BULLET, this);
            else

                // Rauchwolke bei Laser
                if (SelectedWeapon == 1)
                PartikelSystem.PushPartikel(xpos + 30, ypos + 24 + yoff, SMOKE3);
            else

                // Grüne Funken beim Bounce
                if (SelectedWeapon == 2)
                for (int i = 0; i < 2; i++)
                    PartikelSystem.PushPartikel(xpos + 30 + random(4), ypos + 28 + yoff + random(4), FUNKE2);
        }

        // Schussflamme
        FlameTime = 1.0f;
        FlameAnim = random(2);
    }

    else  // oder Bombe legen
    {
        // Riesen Bombe?
        if (RiesenShotExtra > 0.0f)
            Projectiles.PushProjectile(xpos + 26, ypos + 46, BOMBEBIG, this);

        // oder normal
        else
            Projectiles.PushProjectile(xpos + 30, ypos + 56, BOMBE, this);
    }
}

// --------------------------------------------------------------------------------------
// Spieler Granate
// --------------------------------------------------------------------------------------

void PlayerClass::PlayerGrenadeShoot() {
    CalcAustrittsPunkt();

    //----- Richung des Schusses ermitteln

    float wadd = 0.0f;

    // Hurri zielt nach oben
    //
    if (Handlung == PlayerActionEnum::SCHIESSEN_O ||
            (Handlung == PlayerActionEnum::SPRINGEN &&
            Aktion[AKTION_OBEN] && !Aktion[AKTION_LINKS] && !Aktion[AKTION_RECHTS])) {
        if (Blickrichtung == LINKS)
            wadd = 90.0f;
        else
            wadd = -90.0f;
    } else

        // Hurri zielt nach links oben
        //
        if (Handlung == PlayerActionEnum::SCHIESSEN_LO ||
            (Handlung == PlayerActionEnum::SPRINGEN && Aktion[AKTION_OBEN] && Aktion[AKTION_LINKS]))
        wadd = 45.0f;
    else

        // Hurri zielt nach rechts oben
        //
        if (Handlung == PlayerActionEnum::SCHIESSEN_RO ||
            (Handlung == PlayerActionEnum::SPRINGEN && Aktion[AKTION_OBEN] && Aktion[AKTION_RECHTS]))
        wadd = -45.0f;

    WinkelUebergabe = 90.0f + wadd;
    Projectiles.PushProjectile(xpos + AustrittX - 4, ypos + AustrittY - 4, GRENADE, this);

    SoundManager.PlayWave(100, 128, 11025, SOUND_GRANATE);
    PowerLinePossible = false;  // Taste einrasten
    Grenades--;                 // Granaten verringern

    // Schussflamme
    FlameTime = 1.0f;
    FlameAnim = random(2);
}

// --------------------------------------------------------------------------------------
// Den alten, billigen Blitz rendern
// --------------------------------------------------------------------------------------

void PlayerClass::DrawNormalLightning(int DrawLength) {
    //----- Strahl des Blitzes anzeigen

    float l, r, o, u;      // Vertice Koordinaten
    float tl, tr, to, tu;  // Textur Koordinaten
    float x, y;

    x = static_cast<float>(xpos - TileEngine.XOffset + 60);  // Position errechnen
    y = static_cast<float>(ypos - TileEngine.YOffset + 36);

    if (Blickrichtung == LINKS)
        x -= 56;

    l = x - 0.5f;                    // Links
    o = y - DrawLength * 32 - 0.5f;  // Oben
    r = x + 31 + 0.5f;               // Rechts
    u = y + 31 + 0.5f;               // Unten

    tl = 0.0f;
    tr = 1.0f;
    to = 0.0f;
    tu = 1.0f * DrawLength;
    if (tu == 0.0f)
        tu = 1.0f;

    TriangleStrip[0].x = l;  // Links oben
    TriangleStrip[0].y = o;
    TriangleStrip[0].tu = tl;
    TriangleStrip[0].tv = to;

    TriangleStrip[1].x = r;  // Rechts oben
    TriangleStrip[1].y = o;
    TriangleStrip[1].tu = tr;
    TriangleStrip[1].tv = to;

    TriangleStrip[2].x = l;  // Links unten
    TriangleStrip[2].y = u;
    TriangleStrip[2].tu = tl;
    TriangleStrip[2].tv = tu;

    TriangleStrip[3].x = r;  // Rechts unten
    TriangleStrip[3].y = u;
    TriangleStrip[3].tu = tr;
    TriangleStrip[3].tv = tu;

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = 0xFFFFFFFF;

    DirectGraphics.SetTexture(Projectiles.Blitzstrahl[BlitzAnim].itsTexIdx);

    // Blitz rotieren lassen
    glm::mat4x4 matRot, matTrans, matTrans2;

    matRot = glm::rotate(glm::mat4x4(1.0f), DegreetoRad[static_cast<int>(BlitzWinkel)], glm::vec3(0.0f, 0.0f, 1.0f));  // Rotationsmatrix
    D3DXMatrixTranslation(&matTrans, -x - 16, -y - 56, 0.0f);     // Transformation zum Ursprung

    if (Blickrichtung == RECHTS)
        D3DXMatrixTranslation(&matTrans2, x - 23, y, 0.0f);  // Transformation wieder zurück
    else
        D3DXMatrixTranslation(&matTrans2, x + 31, y, 0.0f);  // Transformation wieder zurück

    matWorld = matRot * matTrans;     // Verschieben und rotieren
    matWorld = matTrans2 * matWorld;  // und wieder zurück
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

    DirectGraphics.SetFilterMode(true);

    // Blitzstrahl zeichnen
    v1.color = v2.color = v3.color = v4.color = 0xFFFFFFFF;
    DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip[0]);

    // nochmal rendern bei supershot?
    if (RiesenShotExtra > 0.0f)
        DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip[0]);

    DirectGraphics.SetFilterMode(false);

    // Normale Projektions-Matrix wieder herstellen, wenn der Screen nicht gerade gewackelt wird
    if (WackelMaximum <= 0.0f) {
        matWorld = glm::mat4x4(1.0f);
        g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
        load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif
    } else {
        int Winkel = static_cast<int>(WackelValue + 500) - 500;  // +500 und -500 damit er von -1.0 bis +1.0
        // nich stehen bleibt, weil -0.99 bis +0.99
        // auf 0 gerundet wird

        // Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
        clampAngle(Winkel);
        matRot = glm::rotate(glm::mat4x4(1.0f), DegreetoRad[Winkel], glm::vec3(0.0f, 0.0f, 1.0f));

        if (WackelMaximum <= 0.0f)  // Wackeln zuende ?
            WackelMaximum = 0.0f;   // Dann aufhören damit

        // rotierte Matrix setzen
        g_matModelView = matRot * g_matView;
#if defined(USE_GL1)
        load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif
    }

    DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// Den neuen, mega geilen Blitz rendern
// bestehend aus mehreren, zusammengesetzten vertice-strichen
// Drawlength gibt an, wie lang der Blitz ist
// --------------------------------------------------------------------------------------

void PlayerClass::DrawCoolLightning(int DrawLength, float mul) {
    int r = 0;
    int zufall = 0;
    int ystrahl = 0;
    int xstrahl = 0;
    float xstart = 0.0f;
    float ystart = 0.0f;
    int yoff = 0;
    int size = 0;
    int maxintersections = 0;  // anzahl zwischenschritte im blitz
    D3DCOLOR col;

    ystrahl = 0;
    xstrahl = 0;

    xstart = static_cast<float>(18);
    ystart = static_cast<float>(4);

    maxintersections = DrawLength + 2;

    // Blitze neu zusammensetzen?
    if (Console.Showing == false)
        changecount -= 1.0f SYNC;

    if (changecount <= 0.0f) {
        changecount = static_cast<float>(random(10) + 1) / 10.0f;

        for (int n = 0; n < 12; n++) {
            // Farbe festlegen, hell oder dunkel oder fast weiss
            zufall = random(6);
            if (zufall < 3) {
                r = random(64);
                col = D3DCOLOR_RGBA(r / 2, r, random(32) + 224, random(64) + 192);
            } else if (zufall < 5) {
                r = random(32);
                col = D3DCOLOR_RGBA(r / 2, r, random(24) + 64, random(24) + 192);
            } else {
                r = random(128) + 128;
                col = D3DCOLOR_RGBA(r, r, 255, random(16) + 128);
            }

            // Startpunkt auf 0 setzen
            yoff = 0;
            size = random(4) + 1;

            for (int i = 0; i < maxintersections * 2; i += 2) {
                // zwei neue Punkte zwischen letztem Punkt und Endpunkt per Zufall setzen
                xstrahl = static_cast<int>((xpos - TileEngine.XOffset + xstart) + (random(32) - 16) * mul);
                ystrahl = static_cast<int>(ypos - TileEngine.YOffset + ystart - yoff);

                // Am End- und Austrittspunkt gebündelt
                if (i == 0) {
                    xstrahl = static_cast<int>(xpos - TileEngine.XOffset + xstart) + random(6) - 2;
                    ystrahl = static_cast<int>(ypos - TileEngine.YOffset + ystart);
                }

                if (i >= (maxintersections - 1) * 2) {
                    xstrahl = static_cast<int>(xpos - TileEngine.XOffset + xstart) + random(6) - 2;
                    ystrahl = static_cast<int>(ypos - TileEngine.YOffset + ystart - (DrawLength + 1) * 32);
                }

                // Position setzen
                strahlen[n][i + 0].x = static_cast<float>(xstrahl - size + xstart);
                strahlen[n][i + 1].x = static_cast<float>(xstrahl + size + xstart);
                strahlen[n][i + 0].y = static_cast<float>(ystrahl + ystart);
                strahlen[n][i + 1].y = static_cast<float>(ystrahl + ystart);

                // texturkoordinaten setzen
                strahlen[n][i + 0].tu = 0.0f;
                strahlen[n][i + 0].tv = 0.0f;
                strahlen[n][i + 1].tu = 1.0f;
                strahlen[n][i + 1].tv = 0.0f;

                // Farbe setzen
                strahlen[n][i + 0].color = col;
                strahlen[n][i + 1].color = col;

                yoff += random(21) + 24;
            }
        }
    }

    // Strahlen rendern
    DirectGraphics.SetTexture(Projectiles.BlitzTexture.itsTexIdx);

    for (int n = 0; n < 12; n++) {
        DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, (maxintersections - 1) * 2 - 1, &strahlen[n][0]);
    }

    return;
}

// --------------------------------------------------------------------------------------
// Blitz animieren und zeichnen
//
// Ist die Blitztaste schon lange genug gedrückt worden, wird der ganze Blitz angezeigt
// Andernfalls nur der Blitzursprung beim Spieler
// --------------------------------------------------------------------------------------

bool PlayerClass::DoLightning() {
    int DrawLength;       // Länge des Blitze mit berücksichtigten Wänden im Weg
    GegnerClass *pEnemy;  // Für die Blitz/Gegner Kollision

    DrawLength = BlitzLength - 1;  // Vom Maximum ausgehen (das wird später "gekürzt")

    // Einmal im Kreis rumgedreht ? Dann wieder
    // von vorne beginnen mit der Rotation
    // und zwar im und gegen den Uhrzeigersinn
    clampAngle(BlitzWinkel);

    // Ende des Blitzes beim Spieler leuchten lassen, falls er ihn grade noch auflädt

    float x, y;

    x = xpos - TileEngine.XOffset + 35;  // Position errechnen
    y = ypos - TileEngine.YOffset + 35;

    // if (Blickrichtung == LINKS)
    //	x -= 56;

    DirectGraphics.SetAdditiveMode();

    //----- Blitz animieren

    if (!Console.Showing)
        BlitzCount += SpeedFaktor;  // Counter erhöhen

    if (BlitzCount > PLAYER_BLITZ_SPEED)  // Animationsgrenze überschritten ?
    {
        BlitzCount = 0.0f;   // Dann Counter wieder auf Null setzen und
        BlitzAnim++;         // Nächste Animationsphase
        if (BlitzAnim >= 4)  // Animation von vorne beginnen ?
            BlitzAnim = 0;
    }

    //----- Wird der Blitz schon angezeigt ?
    //----- wenn nicht, dann das Aufladen anzeigen

    float xstart = xpos + 20;
    float ystart = ypos + 21;

    // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
    // xstart += static_cast<float>(36*cos(PI * (BlitzWinkel-90) / 180));
    // ystart += static_cast<float>(36*sin(PI * (BlitzWinkel-90) / 180));
    xstart += 36.0f * cos_deg(BlitzWinkel - 90.0f);
    ystart += 36.0f * sin_deg(BlitzWinkel - 90.0f);

    if (BlitzStart < PLAYER_BLITZ_START) {
        D3DCOLOR Color, Color2;

        if (BlitzStart < PLAYER_BLITZ_START) {
            Color = D3DCOLOR_RGBA(255, 255, 255, static_cast<int>(BlitzStart * 25));
            Color2 = D3DCOLOR_RGBA(255, 255, 255, static_cast<int>(BlitzStart * 25) / 5);
        } else {
            Color = D3DCOLOR_RGBA(255, 255, 255, 255);
            Color2 = D3DCOLOR_RGBA(255, 255, 255, 48);
        }

        Projectiles.Blitzflash[BlitzAnim].RenderSprite(xstart - 18 - TileEngine.XOffset,
                                                       ystart - 18 - TileEngine.YOffset, Color);

        // noch glow um die blitzenden?
        if (options_Detail >= DETAIL_HIGH)
            Projectiles.Blitzflash[3 - BlitzAnim].RenderSpriteScaled(
                xstart - 58 - TileEngine.XOffset, ystart - 58 - TileEngine.YOffset, 144, 144, 0, Color2);

        DirectGraphics.SetColorKeyMode();
        return true;
    }

    //----- Blitz anzeigen - zuerst wird festgestellt, wie lang der Blitz ist,
    //		d.h., ob er eine Wand getroffen hat

    // Anfang des Blitzes leuchten lassen
    Projectiles.Blitzflash[BlitzAnim].RenderSprite(xstart - 18 - TileEngine.XOffset, ystart - 18 - TileEngine.YOffset,
                                                   0xFFFFFFFF);

    // noch glow um die blitzenden?
    if (options_Detail >= DETAIL_HIGH) {
        Projectiles.Blitzflash[3 - BlitzAnim].RenderSpriteScaled(
            xstart - 58 - TileEngine.XOffset, ystart - 58 - TileEngine.YOffset, 144, 144, 0, 0x30FFFFFF);
    }

    // Startpunkt der Kollisionsabfrage auch schon mit ein wenig Abstand zum Spieler
    // xstart -= static_cast<float>(20*cos(PI * (BlitzWinkel-90) / 180));
    // ystart -= static_cast<float>(20*sin(PI * (BlitzWinkel-90) / 180));
    xstart -= 20.0f * cos_deg(BlitzWinkel - 90);
    ystart -= 20.0f * sin_deg(BlitzWinkel - 90);

    RECT_struct Rect;  // Rechteck für die Kollisionserkennung
    // ein Blitz-Stück wird grob durch
    Rect.left = 0;  // ein 32x32 Rechteck abgeschätzt
    Rect.top = 0;
    Rect.right = 31;
    Rect.bottom = 31;

    float xs, ys;

    // Rechtecke für die Kollisionsabfrage rotieren lassen
    for (int i = 0; i < BlitzLength + 1; i++) {
        // DKS - #ifdef'd this check
#ifndef NDEBUG
        // Zum anzeigen der Rects, die geprüft werden
        if (DebugMode == true)
            RenderRect(static_cast<float>(xstart - TileEngine.XOffset), static_cast<float>(ystart - TileEngine.YOffset), 31, 31, 0x80FFFFFF);
#endif  // NDEBUG

        xs = xstart;
        ys = ystart;

        // Blitz auf Kollision mit den Gegnern prüfen
        pEnemy = Gegner.pStart;    // Anfang der Gegnerliste
        while (pEnemy != nullptr)  // Noch nicht alle durch ?
        {
            if (pEnemy->Active &&     // Ist der Gegner überhaupt aktiv ?
                pEnemy->Destroyable)  // und zerstörbar ?

            {
                // Überschneiden sich die Rechtecke ?
                if (xstart + 31 > pEnemy->xPos && xstart < pEnemy->xPos + GegnerRect[pEnemy->GegnerArt].right &&
                    ystart + 31 > pEnemy->yPos && ystart < pEnemy->yPos + GegnerRect[pEnemy->GegnerArt].bottom) {
                    // Funken sprühen
                    if (BlitzCount == 0.0f && BlitzAnim % 2 == 0)
                        PartikelSystem.PushPartikel(xs + 12, ys + 12, LASERFUNKE);

                    // Gegner blinken lassen
                    pEnemy->DamageTaken = 255;

                    // Energy abziehen
                    pEnemy->Energy = pEnemy->Energy - BLITZ_POWER SYNC;

                    // Hit Sound
                    // DKS - Added function WaveIsPlaying() to SoundManagerClass:
                    if (!SoundManager.WaveIsPlaying(SOUND_HIT + pEnemy->HitSound))
                        SoundManager.PlayWave(100, 128, 21000 + random(1000), SOUND_HIT + pEnemy->HitSound);

                    // PlattForm ShootButton getroffen ? Dann Blitz kürzen
                    //
                    if (pEnemy->GegnerArt == SHOOTBUTTON) {
                        if (BlitzCount == 0.0f && BlitzAnim % 2 == 0)
                            PartikelSystem.PushPartikel(xs + 12, ys + 12, LASERFUNKE);  // Funken sprühen
                        DrawLength = i - 1;                                             // Blitz "kürzen"
                        i = BlitzLength + 2;
                        break;  // Und Schleife verlassen
                    }
                }
            }

            pEnemy = pEnemy->pNext;  // Nächsten Gegner testen
        }

        // Zerstörbare Wände ?
        TileEngine.CheckDestroyableWalls(xs, ys, 0, 0, Rect);
        TileEngine.BlockDestroyRechts(xs, ys, xs, ys, Rect);
        TileEngine.BlockDestroyLinks(xs, ys, xs, ys, Rect);
        TileEngine.BlockDestroyOben(xs, ys, xs, ys, Rect);
        TileEngine.BlockDestroyUnten(xs, ys, xs, ys, Rect);

        if (TileEngine.BlockLinks(xs, ys, xs, ys, Rect) & BLOCKWERT_WASSER &&  // Im Wasser blitzen ?
            TileEngine.BlockRechts(xs, ys, xs, ys, Rect) & BLOCKWERT_WASSER &&
            TileEngine.BlockOben(xs, ys, xs, ys, Rect) & BLOCKWERT_WASSER &&
            TileEngine.BlockUnten(xs, ys, xs, ys, Rect) & BLOCKWERT_WASSER && random(80) == 0)
            PartikelSystem.PushPartikel(xs + random(32), ys + random(32), BUBBLE);  // Dann blubbern

        if (TileEngine.BlockLinks(xs, ys, xs, ys, Rect) & BLOCKWERT_WAND ||  // Eine Wand getroffen ?
            TileEngine.BlockRechts(xs, ys, xs, ys, Rect) & BLOCKWERT_WAND ||
            TileEngine.BlockOben(xs, ys, xs, ys, Rect) & BLOCKWERT_WAND ||
            TileEngine.BlockUnten(xs, ys, xs, ys, Rect) & BLOCKWERT_WAND) {
            if (BlitzCount == 0.0f && BlitzAnim % 2 == 0)
                PartikelSystem.PushPartikel(xs + 12, ys + 12, LASERFUNKE);  // Funken sprühen
            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xstart += static_cast<float>(32*cos(PI * (BlitzWinkel-90) / 180));
            // ystart += static_cast<float>(32*sin(PI * (BlitzWinkel-90) / 180));
            xstart += 32.0f * cos_deg(BlitzWinkel - 90);
            ystart += 32.0f * sin_deg(BlitzWinkel - 90);
            DrawLength = i - 1;  // Blitz "kürzen"
            break;               // und Schleife verlassen
        }

        // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
        // xstart += static_cast<float>(32*cos(PI * (BlitzWinkel-90) / 180));
        // ystart += static_cast<float>(32*sin(PI * (BlitzWinkel-90) / 180));
        xstart += 32.0f * cos_deg(BlitzWinkel - 90);
        ystart += 32.0f * sin_deg(BlitzWinkel - 90);
    }

    // Position für das Ende des Blitzes wieder ein wenig zurückverschieben
    // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
    // xstart -= static_cast<float>(16*cos(PI * (BlitzWinkel-90) / 180));
    // ystart -= static_cast<float>(16*sin(PI * (BlitzWinkel-90) / 180));
    xstart -= 16.0f * cos_deg(BlitzWinkel - 90);
    ystart -= 16.0f * sin_deg(BlitzWinkel - 90);

    // Ende des Blitzes leuchten lassen
    Projectiles.Blitzflash[BlitzAnim].RenderSprite(static_cast<float>(xstart - 18 - TileEngine.XOffset),
                                                   static_cast<float>(ystart - 18 - TileEngine.YOffset), 0xFFFFFFFF);

    // noch glow um die blitzenden?
    if (options_Detail >= DETAIL_HIGH) {
        Projectiles.Blitzflash[3 - BlitzAnim].RenderSpriteScaled(
            static_cast<float>(xstart - 58 - TileEngine.XOffset), static_cast<float>(ystart - 58 - TileEngine.YOffset), 144, 144, 0, 0x30FFFFFF);
    }

    // Blitz rotieren lassen
    glm::mat4x4 matRot, matTrans, matTrans2;

    matRot = glm::rotate(glm::mat4x4(1.0f), DegreetoRad[static_cast<int>(BlitzWinkel)], glm::vec3(0.0f, 0.0f, 1.0f));  // Rotationsmatrix
    D3DXMatrixTranslation(&matTrans, -x, -y, 0.0f);               // Transformation zum Ursprung
    if (Blickrichtung == RECHTS)
        D3DXMatrixTranslation(&matTrans2, x, y, 0.0f);  // Transformation wieder zurück
    else
        D3DXMatrixTranslation(&matTrans2, x, y, 0.0f);     // Transformation wieder zurück
    matWorld = matRot * matTrans;     // Verschieben und rotieren
    matWorld = matTrans2 * matWorld;  // und wieder zurück
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

    DirectGraphics.SetFilterMode(true);
    if (DrawLength < 0)
        DrawLength = 0;

    if (options_Detail >= DETAIL_HIGH)
        DrawCoolLightning(DrawLength);
    else
        DrawNormalLightning(DrawLength);

    // nochmal rendern bei supershot?
    if (RiesenShotExtra > 0.0f) {
        if (options_Detail >= DETAIL_HIGH)
            DrawCoolLightning(DrawLength, 1.2f);
        else
            DrawNormalLightning(DrawLength);
    }

    DirectGraphics.SetFilterMode(false);

    // Normale Projektions-Matrix wieder herstellen, wenn der Screen nicht gerade gewackelt wird
    if (WackelMaximum <= 0.0f) {
        matWorld = glm::mat4x4(1.0f);
        g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
        load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif
    } else {
        int Winkel = static_cast<int>(WackelValue + 500) - 500;  // +500 und -500 damit er von -1.0 bis +1.0
        // nich stehen bleibt, weil -0.99 bis +0.99
        // auf 0 gerundet wird

        // Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
        clampAngle(Winkel);
        matRot = glm::rotate(glm::mat4x4(1.0f), DegreetoRad[Winkel], glm::vec3(0.0f, 0.0f, 1.0f));

        if (WackelMaximum <= 0.0f)  // Wackeln zuende ?
            WackelMaximum = 0.0f;   // Dann aufhören damit

        // rotierte Matrix setzen
        g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
        load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif
    }

    DirectGraphics.SetColorKeyMode();

    return true;
}

// --------------------------------------------------------------------------------------
// BeamBlitz aufladen
//
// Der Beam lädt sich langsam auf, je nach Blitzlänge unterschiedlich schnell
// --------------------------------------------------------------------------------------

bool PlayerClass::LoadBeam() {
    // Einmal im Kreis rumgedreht ? Dann wieder
    // von vorne beginnen mit der Rotation
    // und zwar im und gegen den Uhrzeigersinn
    clampAngle(BlitzWinkel);

    //----- Blitz animieren

    BlitzCount += SpeedFaktor;            // Counter erhöhen
    if (BlitzCount > PLAYER_BLITZ_SPEED)  // Animationsgrenze überschritten ?
    {
        BlitzCount = 0.0f;   // Dann Counter wieder auf Null setzen und
        BlitzAnim++;         // Nächste Animationsphase
        if (BlitzAnim >= 4)  // Animation von vorne beginnen ?
            BlitzAnim = 0;
    }

    // Ende des Blitzes beim Spieler leuchten lassen, falls er ihn grade noch auflädt
    //
    float x;
    // float y;

    x = xpos - TileEngine.XOffset + 60;  // Position errechnen
    // y = static_cast<float>(ypos - TileEngine.YOffset+36);

    if (Blickrichtung == LINKS)
        x -= 56;

    DirectGraphics.SetAdditiveMode();

    //----- Wird der Blitz schon angezeigt ?
    //----- wenn nicht, dann das Aufladen anzeigen

    D3DCOLOR Color;
    int a;

    a = static_cast<int>(BlitzStart * 255 / 160);

    if (BlitzStart < PLAYER_BEAM_MAX)
        Color = D3DCOLOR_RGBA(255, 255, 255, a);
    else
        Color = D3DCOLOR_RGBA(255, 255, 255, 255);

    float xstart = xpos + 20;
    float ystart = ypos + 21;

    // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
    // xstart += static_cast<float>(28*cos(PI * (BlitzWinkel-90) / 180));
    // ystart += static_cast<float>(28*sin(PI * (BlitzWinkel-90) / 180));
    xstart += 28.0f * cos_deg(BlitzWinkel - 90);
    ystart += 28.0f * sin_deg(BlitzWinkel - 90);

    // Ende des Blitzes leuchten lassen
    Projectiles.Blitzflash[BlitzAnim].RenderSprite(static_cast<float>(xstart - 18 - TileEngine.XOffset),
                                                   static_cast<float>(ystart - 18 - TileEngine.YOffset), Color);

    BeamX = xstart + 12;
    BeamY = ystart + 12;

    // Auflade Partikel erzeugen?
    //
    if (BlitzStart < PLAYER_BEAM_MAX) {
        BeamCount -= static_cast<float>(1.0 SYNC);

        if (BeamCount < 0.0f) {
            BeamCount = 0.1f;

            int j = random(360);
            // DKS - pretty obviously a bug, they mean to convert to degrees before calling sin (which takes radians)
            //      When I fixed this, I went ahead and added support for trig lookup table, and support for
            //      rad/deg versions of sin/cos
            // PartikelSystem.PushPartikel (float (BeamX + sin (static_cast<float>(j)) * 50),
            //                               float (BeamY + cos (static_cast<float>(j)) * 50), BEAMSMOKE2, this);
            PartikelSystem.PushPartikel(BeamX + sin_deg(j) * 50.0f, BeamY + cos_deg(j) * 50.0f, BEAMSMOKE2, this);
        }
    }

    DirectGraphics.SetColorKeyMode();

    return true;
}

// --------------------------------------------------------------------------------------
// Spieler erleidet Schaden
// ist Override == true dann auch mit Shield oder als Rad
// --------------------------------------------------------------------------------------

void PlayerClass::DamagePlayer(float amount, bool Override) {
    if (GodMode || !CanBeDamaged || (Shield > 0.0f && !Override))
        return;

    // Skill Level berücksichtigen
    switch (Skill) {
        case SKILL_EASY:     amount *= 0.3f; break;
        case SKILL_MEDIUM:   amount *= 0.6f; break;
        case SKILL_HARD:     amount *= 1.0f; break;
        case SKILL_HURRICAN: amount *= 2.2f; break;
    }

    // Sound starten
    // DKS - Added function WaveIsPlaying() to SoundManagerClass:
    if (DamageCounter == 0.0f &&
            Handlung != PlayerActionEnum::RADELN && Handlung != PlayerActionEnum::RADELN_FALL &&
            !SoundManager.WaveIsPlaying(SOUND_ABZUG + SoundOff))
        SoundManager.PlayWave(100, 128, 11025, SOUND_ABZUG + SoundOff);

    // Nur verwunden, wenn kein Rad, denn als Rad ist man unverwundbar
    if ((Handlung != PlayerActionEnum::RADELN &&
            Handlung != PlayerActionEnum::RADELN_FALL) || Override) {
        Energy -= amount;
        if (Energy < 0.0f)
            Energy = 0.0f;

        DamageCounter = 5.0f;

        DrawPlayer(true, false);
        WasDamaged = true;
    }
}

// --------------------------------------------------------------------------------------
// Level zentrieren, damit der Spieler wieder im Mittelpunkt steht
// --------------------------------------------------------------------------------------

void PlayerClass::CenterLevel() {
    TileEngine.XOffset = xpos - 300;
    TileEngine.YOffset = ypos - 280;
}

// --------------------------------------------------------------------------------------
// Waffenlevels für die nächste Stufe berechnen
// --------------------------------------------------------------------------------------

void PlayerClass::CalcWeaponLevels() {
    /*
        for (int i = 0; i < 3; i++)
            pPlayer->NextWeaponLevel[i] = 3 + (pPlayer->CurrentWeaponLevel[i] - 1) * 2;

        pPlayer->NextWeaponLevel[3] = 3;
    */
}

// --------------------------------------------------------------------------------------
// Position der Schussflamme errechnen
// --------------------------------------------------------------------------------------

void PlayerClass::CalcFlamePos() {
    float xoff, yoff;

    // auf dem FlugSack reiten
    if (Handlung == PlayerActionEnum::SACKREITEN) {
        yoff = 1;
        if (Blickrichtung == RECHTS)
            xoff = 80;
        else
            xoff = -35;
    }

    // alle anderen Handlugen
    else {
        if (Handlung == PlayerActionEnum::DUCKEN)
            yoff = 23;
        else
            yoff = 0;

        if (Blickrichtung == RECHTS)
            xoff = 66;
        else
            xoff = -33;
    }

    // X-Offset richtig berechnen
    xoff -= static_cast<int>(TileEngine.XOffset);
    yoff -= static_cast<int>(TileEngine.YOffset);

    // Und dann je nach Blickrichtung die Flamme und den Flare dazu setzen
    if (Blickrichtung == RECHTS) {
        if (Handlung == PlayerActionEnum::STEHEN ||
                Handlung == PlayerActionEnum::SACKREITEN ||
                Handlung == PlayerActionEnum::DUCKEN) {
            AustrittAnim = 0;
            AustrittX = xoff - 10;
            AustrittY = yoff + 20;
        } else

            // im Sprung?
            if (Handlung == PlayerActionEnum::SPRINGEN) {
            // dabei hochzielen?
            if (Aktion[AKTION_OBEN]) {
                // nur noch?
                if (!(Aktion[AKTION_LINKS] || Aktion[AKTION_RECHTS])) {
                    AustrittAnim = 2;
                    AustrittX = xoff - 47;
                    AustrittY = yoff - 34;
                }

                // oder schräg?
                else {
                    AustrittAnim = 1;
                    AustrittX = xoff - 21;
                    AustrittY = yoff - 12;
                }
            }

            // normal springen
            else {
                AustrittAnim = 0;
                AustrittX = xoff - 10;
                AustrittY = yoff + 15;
            }
        }

        else if (Handlung == PlayerActionEnum::LAUFEN) {
            AustrittAnim = 0;
            AustrittX = xoff - 2;
            AustrittY = yoff + 19;
        }

        else if (Handlung == PlayerActionEnum::SCHIESSEN_RO) {
            // im Stehen?
            if (WalkLock) {
                AustrittAnim = 1;
                AustrittX = xoff - 25;
                AustrittY = yoff - 8;
            }

            // oder Laufen?
            else {
                AustrittAnim = 1;
                AustrittX = xoff - 21;
                AustrittY = yoff - 12;
            }
        }

        else if (Handlung == PlayerActionEnum::SCHIESSEN_O) {
            AustrittAnim = 2;
            AustrittX = xoff - 49;
            AustrittY = yoff - 33;
        }
    } else {
        if (Handlung == PlayerActionEnum::STEHEN ||
                Handlung == PlayerActionEnum::SACKREITEN ||
                Handlung == PlayerActionEnum::DUCKEN) {
            AustrittAnim = 0;
            AustrittX = xoff + 10;
            AustrittY = yoff + 20;
        } else

            // im Sprung?
            if (Handlung == PlayerActionEnum::SPRINGEN) {
            // dabei hochzielen?
            if (Aktion[AKTION_OBEN]) {
                // nur noch?
                if (!(Aktion[AKTION_LINKS] || Aktion[AKTION_RECHTS])) {
                    AustrittAnim = 2;
                    AustrittX = xoff + 62;
                    AustrittY = yoff - 34;
                }

                // oder schräg?
                else {
                    AustrittAnim = 1;
                    AustrittX = xoff + 21;
                    AustrittY = yoff - 12;
                }
            }

            // normal springen
            else {
                AustrittAnim = 0;
                AustrittX = xoff + 6;
                AustrittY = yoff + 15;
            }
        }

        else if (Handlung == PlayerActionEnum::LAUFEN) {
            AustrittAnim = 0;
            AustrittX = xoff;
            AustrittY = yoff + 19;
        }

        else if (Handlung == PlayerActionEnum::SCHIESSEN_LO) {
            // im Stehen?
            if (WalkLock) {
                AustrittAnim = 1;
                AustrittX = xoff + 21;
                AustrittY = yoff - 8;
            }

            // oder im Laufen
            else {
                AustrittAnim = 1;
                AustrittX = xoff + 8;
                AustrittY = yoff - 10;
            }
        } else if (Handlung == PlayerActionEnum::SCHIESSEN_O) {
            AustrittAnim = 2;
            AustrittX = xoff + 62;
            AustrittY = yoff - 33;
        }
    }
}

// --------------------------------------------------------------------------------------
// Austrittspunkt des Schusses errechnen
// --------------------------------------------------------------------------------------

void PlayerClass::CalcAustrittsPunkt() {
    // float xver = 0.0f;	// Verschiebung des Austrittspunktes, wenn man vor einer Wandsteht

    //----- Genauen Pixel am Anfang derFlamme finden
    //
    CalcFlamePos();
    AustrittX += static_cast<float>(TileEngine.XOffset);
    AustrittY += static_cast<float>(TileEngine.YOffset);
    switch (AustrittAnim) {
        // gerade flamme
        case 0: {
            if (Blickrichtung == LINKS)
                AustrittX += 34;
            else
                AustrittX += 4;

            AustrittY += 24 / 2 - 1;
        } break;

        // schräg oben
        case 1: {
            if (Blickrichtung == LINKS)
                AustrittX += 32;
            else
                AustrittX += 8;

            AustrittY += 26;
        } break;

        // oben
        case 2: {
            AustrittX += 24 / 2;
            AustrittY += 38;
        } break;

        case 3: {
            AustrittX += 40 / 2;
            AustrittY += 36 / 2;
        } break;
    }
}

// --------------------------------------------------------------------------------------
// Spieler aus dem Level rausgelaufen?
// --------------------------------------------------------------------------------------

bool PlayerClass::CheckLevelExit() const {
    // Spieler aus Level draussen?
    return xpos + Player[0].CollideRect.right < TileEngine.XOffset ||
           xpos + Player[0].CollideRect.left > TileEngine.XOffset + 640 ||
           ypos + Player[0].CollideRect.bottom < TileEngine.YOffset ||
           ypos + Player[0].CollideRect.top > TileEngine.YOffset + 480;
}

// --------------------------------------------------------------------------------------
// Spieler im Ausgang
// --------------------------------------------------------------------------------------

void PlayerClass::PlayerInExit() {
    StageClearRunning = false;

    // Summary Screen zeigen, wenn nicht im Tutorial Mode
    if (!RunningTutorial)
        SummaryScreen();

    Stage = NewStage;  // Anpassen dass wir im neuen Level sind

    SpielZustand = GameStateEnum::MAINMENU;
    Console.Hide();

    // Tutorial Level zuende? Dann normal zurück ins Hauptmenu
    //

    if (Stage == TUTORIAL_END) {
        NewStage = -1;
        Stage = -1;
        pMenu->AktuellerZustand = MENUZUSTAND_MAINMENU;
        GUI.HideBoxFast();
        SoundManager.StopSong(MUSIC_STAGECLEAR, false);
    }

    // Alle Levels durch? Dann Outtro starten
    //

    else if (Stage > MAX_LEVELS)
        StartOuttro();

    // Ansonsten Savegame Menu
    //

    else if (!RunningTutorial) {
        pMenu->LoadSavegames();  // Slots mit aktuellen Savegames füllen
        pMenu->AktuellerPunkt = 0;
        pMenu->AktuellerZustand = MENUZUSTAND_SAVEGAME;
    } else {
        for (int p = 0; p < NUMPLAYERS; p++)
            Player[p].GodMode = false;

        Grenades = 3;
        PowerLines = 3;
        SmartBombs = 1;

        RunningTutorial = false;
        InitNewGameLevel();  // Neues level laden
    }

    RunningTutorial = false;
}

// --------------------------------------------------------------------------------------
// Spieler läuft grad ins Exit
// --------------------------------------------------------------------------------------

void PlayerClass::RunPlayerExit() {
    // DKS - Added support for non-looped music, so this is no longer necessary..
    //      This was causing stage-clear music to never be heard in the SDL port.
    //// Musik zuende ?
    // if (MUSIC_IsFinished(SoundManager.its_Songs[MUSIC_STAGECLEAR]->SongData))
    //    SoundManager.StopSong(MUSIC_STAGECLEAR, false);

    if (CheckLevelExit())
        PlayerInExit();
}

// --------------------------------------------------------------------------------------
// Alle Spieler auf einem Flugsack?
// --------------------------------------------------------------------------------------

bool PlayerClass::Riding() const {
    for (int p = 0; p < NUMPLAYERS; p++)
        if (Player[p].Handlung != PlayerActionEnum::SACKREITEN &&
                Player[p].Handlung != PlayerActionEnum::DREHEN &&
                Player[p].Handlung != PlayerActionEnum::TOT)
            return false;

    return true;
}

// --------------------------------------------------------------------------------------
// Level bei Flugsack Passage scrollen
// --------------------------------------------------------------------------------------

void PlayerClass::ScrollFlugsack() {
    bool BeideFrei = true;

    for (int p = 0; p < NUMPLAYERS; p++)
        if (Player[p].FesteAktion > -1)
            BeideFrei = false;
    if (!FlugsackFliesFree && Riding() && BeideFrei)
        TileEngine.YOffset -= static_cast<float>(PLAYER_FLUGSACKSPEED SYNC);
}

// --------------------------------------------------------------------------------------
// Waffen durchcyclen
// --------------------------------------------------------------------------------------

void PlayerClass::checkWeaponSwitch() {
    if (!Aktion[AKTION_WAFFEN_CYCLE])
        weaponswitchlock = false;

    if (Aktion[AKTION_WAFFEN_CYCLE] && !weaponswitchlock) {
        weaponswitchlock = true;

        do {
            SelectedWeapon++;
        } while (CurrentWeaponLevel[SelectedWeapon] == 0);

        if (SelectedWeapon >= 3)
            SelectedWeapon = 0;
    }
}

// DKS - Player sprites are no longer static globals, but instead class member vars:
//      This function will load the sprites specific to each player. If
//      player_num is 0, it will load Player 1's sprites, or Player 2's otherwise.
void PlayerClass::LoadSprites() {
    if (SpritesLoaded)
        return;

    SpritesLoaded = true;
    std::string prefix;

    if (PlayerNumber == 1)
        prefix = "p2_";
    else
        prefix = "p1_";

    // Idle
    PlayerIdle.LoadImage(prefix + "hurri_idle.png", 350, 320, 70, 80, 5, 4);

    // Kucken
    PlayerKucken.LoadImage(prefix + "hurri_kucken.png", 140, 80, 70, 80, 2, 1);

    // Umkucken
    PlayerIdle2.LoadImage(prefix + "hurri_idleumkuck.png", 980, 320, 70, 80, 14, 4);

    // Laufen
    PlayerRun.LoadImage(prefix + "hurri_laufen.png", 350, 320, 70, 80, 5, 4);

    // Diagonal schauen/schiessen
    PlayerDiagonal.LoadImage(prefix + "hurri_shootdiagonal.png", 140, 80, 70, 80, 2, 1);

    // Hoch schauen/schiessen
    PlayerOben.LoadImage(prefix + "hurri_shootup.png", 140, 80, 70, 80, 2, 1);

    // Ducken
    PlayerCrouch.LoadImage(prefix + "hurri_crouch.png", 140, 80, 70, 80, 2, 1);

    // Springen normal, diagonal und nach oben
    PlayerJump.LoadImage(prefix + "hurri_jump.png", 280, 240, 70, 80, 4, 3);
    PlayerJumpDiagonal.LoadImage(prefix + "hurri_jumpschraeg.png", 280, 240, 70, 80, 4, 3);
    PlayerJumpUp.LoadImage(prefix + "hurri_jumpup.png", 280, 240, 70, 80, 4, 3);

    // Rundumschuss
    PlayerBlitz.LoadImage(prefix + "hurri_rundumschuss.png", 490, 480, 70, 80, 7, 6);

    // Pissen
    PlayerPiss.LoadImage(prefix + "hurri_pissen.png", 490, 240, 70, 80, 7, 3);

    // Flugsack
    PlayerRide.LoadImage(prefix + "hurri_ride.png", 450, 480, 90, 120, 5, 4);

    // Stachelrad
    PlayerRad.LoadImage(prefix + "hurri_rad.png", 140, 70, 35, 35, 4, 2);
}
