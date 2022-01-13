// --------------------------------------------------------------------------------------
// Der Lava Golem
//
// wirft Steinbroken und lässt die Höhle schaukeln
// --------------------------------------------------------------------------------------

#include "Boss_Golem.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

constexpr float WACKELMAX = 0.1f;
constexpr float ARMX = -55.0f;
constexpr float ARMY = 20.0f;

constexpr int ARMSIZEX = 193;
constexpr int ARMSIZEY = 179;

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerGolem::GegnerGolem(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::NOTVISIBLE;
    BlickRichtung = DirectionEnum::LINKS;
    Energy = 8000;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimPhase = 0;
    ChangeLight = Light;
    Destroyable = true;
    OwnDraw = true;

    Wackel = 0.0f;
    WackelDir = 1.0f;
    yoff = 0.0f;

    StoneCount = 0;

    rotarm1 = 0.0f;
    rotarm2 = 0.0f;

    last = 0;

    arm[0].LoadImage("golemarm_hinten.png", 224, 241, 224, 241, 1, 1);
    arm[1].LoadImage("golemarm_vorne.png", 205, 266, 205, 266, 1, 1);
}

// --------------------------------------------------------------------------------------
// Winkel zum Spieler
// --------------------------------------------------------------------------------------

float GegnerGolem::WinkelToPlayer() {

    float ydiv = (pAim->ypos + 40) - (yPos + yoff + 50);
    if (ydiv == 0.0f)
        ydiv = 0.00001f;

    float xdiv = (pAim->xpos + 35) - (xPos + 50);
    // DKS - converted to float, used new macros:
    // w = 90.0f + (float)atan(ydiv / xdiv) * 180.0f / PI;

    return 90.0f + RadToDeg(atanf(ydiv / xdiv));
}

// --------------------------------------------------------------------------------------
// Golem beim laufen wackeln lassen
// --------------------------------------------------------------------------------------

void GegnerGolem::Wackeln() {
    // yoff bewegen
    if (Wackel > 0.0f)
        yoff = Wackel * 8.0f;
    else
        yoff = -Wackel * 8.0f;

    // grenze überschritten?
    if ((WackelDir > 0.0f && Wackel > WACKELMAX) || (WackelDir < 0.0f && Wackel < -WACKELMAX)) {
        WackelDir *= -1.0f;
        Wackel = WACKELMAX * -WackelDir;

        if (Handlung == GEGNER::EINFLIEGEN || Handlung == GEGNER::LAUFEN || Handlung == GEGNER::LAUFEN2 ||
            Handlung == GEGNER::LAUFEN3 || Handlung == GEGNER::LAUFEN4) {
            ShakeScreen(4.0f);
            SoundManager.PlayWave(100, 128, 15000 + random(2000), SOUND::DOORSTOP);
        }
    }
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerGolem::DoDraw() {

    float wackel = 0.0f;

    if (Handlung == GEGNER::EXPLODIEREN)
        wackel = static_cast<float>(static_cast<int>(ShotDelay) % 2);

    DirectGraphics.SetFilterMode(true);

    int Wert = 255 - (static_cast<int>(DamageTaken));
    D3DCOLOR Color = D3DCOLOR_RGBA(255, Wert, Wert, 255);

    // Wert /= 2;
    // Color2 = D3DCOLOR_RGBA(128, Wert, Wert, 255);

    // Rotationsmatrizen erstellen
    RECT_struct itsRect;
    glm::mat4x4 matRotBody, matRotArm, matTransBody, matTransArm, matTrans2Body, matTrans2Arm;

    // Körper
    int offx = 0;
    int offy = 80;
    float x = xPos - TileEngine.XOffset;
    float y = yPos - TileEngine.YOffset;

    itsRect = GegnerRect[GegnerArt];
    matRotBody = glm::rotate(glm::mat4x4(1.0f), Wackel, glm::vec3(0.0f, 0.0f, 1.0f));
    D3DXMatrixTranslation(&matTransBody, -x - (itsRect.right - itsRect.left) / 2.0f - offx,
                          -y - (itsRect.bottom - itsRect.top) / 2.0f - offy, 0.0f);

    // Transformation wieder zurück
    D3DXMatrixTranslation(&matTrans2Body, x + (itsRect.right - itsRect.left) / 2.0f + offx,
                          y + (itsRect.bottom - itsRect.top) / 2.0f + offy, 0.0f);

    matRotArm = glm::rotate(glm::mat4x4(1.0f), rotarm2 - 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));

    // arm bewegen
    offx = 0;
    offy = 80;
    x = xPos + ARMX - TileEngine.XOffset;
    y = yPos + ARMY - TileEngine.YOffset;
    D3DXMatrixTranslation(&matTransArm, -x - 205, -y - 10, 0.0f);

    // Transformation wieder zurück
    D3DXMatrixTranslation(&matTrans2Arm, x + 120, y + 80, 0.0f);

    matWorld = glm::mat4x4(1.0f);
    matWorld = matTransArm * matWorld;    // rotieren (Körper)
    matWorld = matRotArm * matWorld;      // rotieren (Arm)
    matWorld = matTrans2Arm * matWorld;   // und wieder zurück verschieben
    matWorld = matTransBody * matWorld;   // rotieren (Körper)
    matWorld = matRotBody * matWorld;     // rotieren (Arm)
    matWorld = matTrans2Body * matWorld;  // und wieder zurück verschieben
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

    // Arm hinten zeichnen
    arm[0].RenderSprite(xPos - TileEngine.XOffset + wackel,
                        yPos - TileEngine.YOffset + yoff, Color);

    // Körper zeichnen
    matWorld = glm::mat4x4(1.0f);
    matWorld = matTransBody * matWorld;   // rotieren (Körper)
    matWorld = matRotBody * matWorld;     // rotieren (Arm)
    matWorld = matTrans2Body * matWorld;  // und wieder zurück verschieben
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset + wackel,
                                           yPos - TileEngine.YOffset + yoff, Color);

    // Arm
    offx = 0;
    offy = 0;
    x = xPos + ARMX - TileEngine.XOffset;
    y = yPos + ARMY - TileEngine.YOffset;

    itsRect.left = 0;
    itsRect.top = 0;
    itsRect.right = 193;
    itsRect.bottom = 179;

    matRotArm = glm::rotate(glm::mat4x4(1.0f), rotarm1 - 0.3f, glm::vec3(0.0f, 0.0f, 1.0f));

    // arm bewegen
    D3DXMatrixTranslation(&matTransArm, -x - 180, -y - 50, 0.0f);

    // Transformation wieder zurück
    D3DXMatrixTranslation(&matTrans2Arm, x + 175, y + 95, 0.0f);

    matWorld = glm::mat4x4(1.0f);
    matWorld = matTransArm * matWorld;    // rotieren (Körper)
    matWorld = matRotArm * matWorld;      // rotieren (Arm)
    matWorld = matTrans2Arm * matWorld;   // und wieder zurück verschieben
    matWorld = matTransBody * matWorld;   // rotieren (Körper)
    matWorld = matRotBody * matWorld;     // rotieren (Arm)
    matWorld = matTrans2Body * matWorld;  // und wieder zurück verschieben
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

    // Arm zeichnen
    arm[1].RenderSprite(xPos - TileEngine.XOffset + wackel,
                        yPos - TileEngine.YOffset + yoff, Color);

    // Normale Projektions-Matrix wieder herstellen
    matWorld = glm::mat4x4(1.0f);
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

    // Blockrect setzen
    GegnerRect[GOLEM].left = 42;
    GegnerRect[GOLEM].right = 138 + static_cast<int>(yoff);
    GegnerRect[GOLEM].top = 29;
    GegnerRect[GOLEM].bottom = 300;

    DirectGraphics.SetFilterMode(false);
    DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// Wert gegen Null gehen lassen mit speed
// --------------------------------------------------------------------------------------
bool GegnerGolem::GoToZero(float &Value, float speed) {
    bool fertig = true;

    if (Value > 0.0f) {
        fertig = false;
        Value -= Timer.sync(speed);
        if (Value < 0.0f)
            Value = 0.0f;
    }

    if (Value < 0.0f) {
        fertig = false;
        Value += Timer.sync(speed);
        if (Value > 0.0f)
            Value = 0.0f;
    }

    return fertig;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerGolem::DoKI() {
    // Energie anzeigen
    if (Handlung != GEGNER::NOTVISIBLE && Handlung != GEGNER::EXPLODIEREN)
        HUD.ShowBossHUD(8000, Energy);

    // Levelausschnitt auf den Golem zentrieren, sobald dieser sichtbar wird
    if (Active == true && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        // verstecken
        xPos += 400;

        // hinscrollen
        TileEngine.ScrollLevel(static_cast<float>(Value1),
                               static_cast<float>(Value2), TileStateEnum::SCROLLTOLOCK);

        // Ausfaden und pausieren
        SoundManager.FadeSong(MUSIC::STAGEMUSIC, -2.0f, 0, true);
    }

    // Zwischenboss blinkt nicht so lange wie die restlichen Gegner
    if (DamageTaken > 0.0f)
        DamageTaken -= Timer.sync(100.0f);  // Rotwerden langsam ausfaden lassen
    else
        DamageTaken = 0.0f;  // oder ganz anhalten

    // Hat der Golem keine Energie mehr ? Dann explodiert er
    if (Energy <= 100.0f && Handlung != GEGNER::EXPLODIEREN) {
        Destroyable = false;
        Handlung = GEGNER::EXPLODIEREN;
        xSpeed = 0.0f;
        ySpeed = 0.0f;
        xAcc = 0.0f;
        yAcc = 0.0f;
        AnimCount = 1.0f;
        ShotDelay = 30.0f;

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC::BOSS, -2.0f, 0, false);
    }

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::NOTVISIBLE:  // Warten bis der Screen zentriert wurde
        {
            if (TileEngine.Zustand == TileStateEnum::LOCKED) {
                // Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
                // DKS - Added function SongIsPlaying() to SoundManagerClass:
                if (!SoundManager.SongIsPlaying(MUSIC::BOSS))
                    SoundManager.PlaySong(MUSIC::BOSS, false);

                // Und Boss erscheinen lassen
                Handlung = GEGNER::EINFLIEGEN;
                SoundManager.PlayWave(100, 128, 7000, SOUND::MUTANT);
            }
        } break;

        // Gegner läuft am Anfang langsam nach links
        case GEGNER::EINFLIEGEN: {
            Energy = 8000;
            DamageTaken = 0.0f;
            xPos -= Timer.sync(10.0f);  // Faust nach unten bewegen
            Wackel += Timer.sync(WackelDir * 0.025f);
            rotarm1 -= Timer.sync(WackelDir * 0.2f);
            rotarm2 += Timer.sync(WackelDir * 0.2f);
            Wackeln();
            if (xPos <= TileEngine.ScrolltoX + 380)  // Weit genug oben ?
            {
                xPos = static_cast<float>(TileEngine.ScrolltoX + 380);
                Handlung = GEGNER::SPECIAL;
            }
        } break;

        // Gegner läuft am Anfang langsam nach links
        case GEGNER::LAUFEN: {
            xPos -= Timer.sync(10.0f);
            Wackel += Timer.sync(WackelDir * 0.025f);
            rotarm1 -= Timer.sync(WackelDir * 0.2f);
            rotarm2 += Timer.sync(WackelDir * 0.2f);
            Wackeln();
            if (xPos <= TileEngine.ScrolltoX + 380.0f)  // Weit genug in Mitte  ?
            {
                xPos = TileEngine.ScrolltoX + 380.0f;
                Handlung = GEGNER::SPECIAL;
            }
        } break;

        // Gegner läuft nach rechts
        case GEGNER::LAUFEN2: {
            xPos += Timer.sync(10.0f);
            Wackel += Timer.sync(WackelDir * 0.025f);
            rotarm1 -= Timer.sync(WackelDir * 0.2f);
            rotarm2 += Timer.sync(WackelDir * 0.2f);
            Wackeln();
            if (xPos >= TileEngine.ScrolltoX + 540.0f)  // Weit genug rechts ?
            {
                // zurücklaufen?
                xPos = TileEngine.ScrolltoX + 540.0f;
                Handlung = GEGNER::SPECIAL;
            }
        } break;

        // Gegner läuft ganz nach links zur mauer
        case GEGNER::LAUFEN3: {
            xPos -= Timer.sync(10.0f);
            Wackel += Timer.sync(WackelDir * 0.025f);
            rotarm1 -= Timer.sync(WackelDir * 0.2f);
            rotarm2 += Timer.sync(WackelDir * 0.2f);
            Wackeln();
            if (xPos <= TileEngine.ScrolltoX + 100.0f)  // Weit genug links ?
            {
                xPos = TileEngine.ScrolltoX + 100.0f;
                Handlung = GEGNER::SPECIAL2;
                last = GEGNER::SPECIAL2;
                state2 = ArmState::SENKEN;
                StoneCount = 3 + random(2);
            }
        } break;

        // von links zur mitte laufen
        case GEGNER::LAUFEN4: {
            xPos += Timer.sync(10.0f);
            Wackel += Timer.sync(WackelDir * 0.025f);
            rotarm1 -= Timer.sync(WackelDir * 0.2f);
            rotarm2 += Timer.sync(WackelDir * 0.2f);
            Wackeln();
            if (xPos >= TileEngine.ScrolltoX + 380.0f)  // Weit genug in Mitte  ?
            {
                xPos = TileEngine.ScrolltoX + 380.0f;
                Handlung = GEGNER::SPECIAL;
            }
        } break;

        // gegen die wand bollern und lavabälle spawnen
        case GEGNER::SPECIAL2: {
            switch (state2) {
                case ArmState::SENKEN: {
                    if (Wackel > -0.2f)
                        Wackel -= Timer.sync(0.03f);
                    else
                        Wackel = -0.2f;

                    rotarm1 -= Timer.sync(0.25f);
                    rotarm2 += Timer.sync(0.25f);

                    if (rotarm2 > 1.0f) {
                        state2 = ArmState::HEBEN;
                        ShakeScreen(3.0f);
                        SoundManager.PlayWave(75, 128, 15000 + random(2000), SOUND::DOORSTOP);

                        Gegner.PushGegner(TileEngine.XOffset + 100.0f + static_cast<float>(random(540)),
                                          TileEngine.YOffset + 480.0f, LAVABALL, 50 + random(10), 0,
                                          false);
                    }
                } break;

                case ArmState::HEBEN: {
                    if (Wackel > -0.2f)
                        Wackel -= Timer.sync(0.03f);
                    else
                        Wackel = -0.2f;

                    rotarm1 += Timer.sync(0.25f);
                    rotarm2 -= Timer.sync(0.25f);

                    if (rotarm1 > 1.0f) {
                        state2 = ArmState::SENKEN;
                        ShakeScreen(3.0f);
                        SoundManager.PlayWave(75, 128, 15000 + random(2000), SOUND::DOORSTOP);

                        Gegner.PushGegner(pAim->xpos - 20.0f + static_cast<float>(random(70)),
                                          TileEngine.YOffset + 480.0f, LAVABALL, 50 + random(10), 0,
                                          false);

                        StoneCount--;

                        if (StoneCount <= 0)
                            Handlung = GEGNER::SPECIAL;
                    }
                } break;
            }
        } break;

        // Ausgangsposition für Aktionen herstellen
        case GEGNER::SPECIAL: {

            // zuende wackeln
            Wackeln();

            bool fertig0 = GoToZero(Wackel);
            bool fertig1 = GoToZero(rotarm1);
            bool fertig2 = GoToZero(rotarm2);

            if (fertig0 && fertig1 && fertig2) {
                // int j = 0; // PICKLE not used

                // Golem steht gerade rechts?
                if (xPos > TileEngine.ScrolltoX + 400.0f) {
                    // Dann Steine werfen
                    if (random(2) == 0) {
                        Handlung = GEGNER::CRUSHEN;
                        last = GEGNER::CRUSHEN;
                        state2 = ArmState::SENKEN;
                        StoneCount = random(2) + 2;
                    }

                    // oder auf die lava dreschen
                    else {
                        Handlung = GEGNER::BOMBARDIEREN;
                        state2 = ArmState::HEBEN;
                        count = static_cast<float>(random(3) + 2);
                    }
                }

                // Golem steht gerade in der Mitte?
                else if (xPos > TileEngine.ScrolltoX + 120.0f) {
                    // Weiterlaufen?
                    if (random(3) > 0) {
                        // Die Aktion ausführen, die eben nicht dran war
                        if (last == GEGNER::SPECIAL2)
                            Handlung = GEGNER::LAUFEN2;
                        else
                            Handlung = GEGNER::LAUFEN3;
                    }

                    // oder Suschuss schiessen
                    else {
                        Handlung = GEGNER::AUSSPUCKEN;
                        ShotDelay = 80.0f;
                        count = 4.0f + random(4);
                        Wackel = 0.0f;
                    }
                }

                // Golem steht schon links? Dann zurück zur Mitte
                else if (xPos <= TileEngine.ScrolltoX + 120.0f) {
                    Handlung = GEGNER::LAUFEN4;
                }
            }

        } break;

        // Grüne Rotzbobel schiessen
        case GEGNER::AUSSPUCKEN: {
            if (ShotDelay > 10.0f) {
                Wackel += Timer.sync(0.03f);
                rotarm1 -= Timer.sync(0.054f);
                rotarm2 -= Timer.sync(0.054f);
            } else {
                Wackel -= Timer.sync(0.2f);
                rotarm1 += Timer.sync(0.4f);
                rotarm2 += Timer.sync(0.4f);
            }

            ShotDelay -= Timer.sync(10.0f);

            if (ShotDelay < 0.0f) {
                // Suchschuss erzeugen
                WinkelUebergabe = WinkelToPlayer() + 180.0f;
                Projectiles.PushProjectile(xPos + 35.0f, yPos + yoff + 35.0f, FIREBALL_BIG, pAim);

                PartikelSystem.PushPartikel(xPos - 60.0f, yPos + yoff - 70.0f, GRENADEFLARE);

                // Sound ausgeben
                SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::FIREBALL);

                ShotDelay = 80.0f;

                // Schusszahl verringern
                count -= 1.0f;

                // kein schuss mehr? dann neue aktion
                if (count <= 0.0f) {
                    Handlung = GEGNER::WARTEN;
                    ShotDelay = 10.0f;
                }

                Wackel = 0.0f;
                rotarm1 = 0.0f;
                rotarm2 = 0.0f;
            }
        } break;

        // nach dem Schiessen kurz warten bis zur nächsten Aktion
        case GEGNER::WARTEN: {
            ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay <= 0.0f)
                Handlung = GEGNER::SPECIAL;
        } break;

        // stein raufholen und werfen
        case GEGNER::CRUSHEN: {
            switch (state2) {
                // absenken und stein holen
                case ArmState::SENKEN: {
                    Wackel -= Timer.sync(0.05f);
                    rotarm1 += Timer.sync(0.1f);
                    yoff += Timer.sync(5.0f);

                    if (Wackel < -0.5f)
                        state2 = ArmState::HEBEN;
                } break;

                // wieder aufrichten
                case ArmState::HEBEN: {
                    yoff -= Timer.sync(5.0f);
                    Wackel += Timer.sync(0.05f);
                    rotarm1 -= Timer.sync(0.1f);

                    if (Wackel >= 0.0f) {
                        state2 = ArmState::UEBERKOPF;
                        Wackel = 0.0f;
                        rotarm1 = 0.0f;
                        yoff = 0.0f;
                    }
                } break;

                // Stein nach hinten nehmen
                case ArmState::UEBERKOPF: {
                    // yoff += Timer.sync(5.0f);
                    Wackel += Timer.sync(0.04f);
                    rotarm1 += Timer.sync(0.4f);

                    if (Wackel >= 0.35f) {
                        state2 = ArmState::WERFEN;
                        SoundManager.PlayWave(100, 128, 6000 + random(2000), SOUND::MUTANT);
                    }
                } break;

                // Stein werfen
                case ArmState::WERFEN: {
                    if (Wackel > 0.0f)
                        Wackel -= Timer.sync(0.1f);
                    else
                        Wackel = 0.0f;

                    rotarm1 -= Timer.sync(0.6f);

                    if (rotarm1 <= 2.0f) {
                        Gegner.PushGegner(xPos - 40.0f, yPos + 30.0f, BOULDER, -(random(80) + 10), -(random(20) + 10),
                                          true);
                        Gegner.PushGegner(xPos - 40.0f, yPos + 30.0f, BOULDER, -(random(40) + 10), -(random(10) + 10),
                                          true);
                        SoundManager.PlayWave(100, 128, 14000 + random(4000), SOUND::STONEFALL);
                        state2 = ArmState::SENKEN2;
                    }
                } break;

                // wieder in wurf ausgangsposition
                case ArmState::SENKEN2: {

                    // zuende wackeln
                    Wackeln();

                    bool fertig0 = GoToZero(Wackel, 0.5f);
                    bool fertig1 = GoToZero(rotarm1, 0.5f);
                    bool fertig2 = GoToZero(rotarm2, 0.5f);

                    if (fertig0 && fertig1 && fertig2) {
                        StoneCount--;

                        if (StoneCount == 0)
                            Handlung = GEGNER::LAUFEN;
                        else
                            state2 = ArmState::SENKEN;
                    }
                } break;
            }
        } break;

        // Feuer spucken
        case GEGNER::SCHIESSEN: {
            count += Timer.sync(1.0f);

            // vorbeugen
            if (count < 10.0f) {
                Wackel += Timer.sync(1.0f);
            }

            // schiessen
            else {
                // einmalig Sound abspielen
                if (ShotDelay == 3.0f)
                    SoundManager.PlayWave(100, 128, 11025, SOUND::FEUERFALLE);

                Wackel = 10.0f;
                ShotDelay -= Timer.sync(1.0f);
            }

            if (ShotDelay <= 0.0f) {
                ShotDelay = 0.5f;
                Projectiles.PushProjectile(xPos - 10.0f, yPos + 80.0f, WALKERFIRE, pAim);
            }

            if (count > 30.0f)
                Handlung = GEGNER::SPECIAL;

        } break;

        //
        case GEGNER::BOMBARDIEREN: {
            // arm heben
            switch (state2) {
                // schnell heben
                case ArmState::HEBEN: {
                    rotarm1 += Timer.sync(0.3f);
                    rotarm2 += Timer.sync(0.2f);

                    if (rotarm1 > 3.0f) {
                        rotarm1 = 3.0f;
                        state2 = ArmState::SENKEN;
                    }
                } break;

                // schnell absenken
                case ArmState::SENKEN: {
                    rotarm1 -= Timer.sync(0.8f);
                    rotarm2 -= Timer.sync(0.7f);

                    // Lava berührt?
                    if (rotarm1 < 0.7f) {
                        rotarm1 = 0.7f;
                        rotarm2 = 0.7f;
                        state2 = ArmState::SENKEN2;

                        // Spritzer
                        for (int i = 0; i < 48; i++)
                            PartikelSystem.PushPartikel(xPos - 80.0f + static_cast<float>(random(50)),
                                                        yPos + 245.0f, LAVA_SPRITZER2);

                        SoundManager.PlayWave3D(static_cast<int>(xPos) + 30, static_cast<int>(yPos) + 30,
                                                10000 + random(2050), SOUND::WATERIN);

                        Projectiles.PushProjectile(xPos - 90.0f, yPos + 300.0f, GOLEMSAEULE);
                    }

                } break;

                // langsam wieder ein stück hoch federn
                case ArmState::SENKEN2: {
                    rotarm1 += Timer.sync(0.25f);
                    rotarm2 += Timer.sync(0.25f);

                    if (rotarm1 > 1.5f) {
                        rotarm1 = 1.5f;
                        rotarm2 = 1.5f;

                        count -= 1.0f;

                        if (count < 0.0f)
                            Handlung = GEGNER::SPECIAL;
                        else
                            state2 = ArmState::HEBEN;
                    }
                } break;
            }
        } break;

        case GEGNER::EXPLODIEREN: {
            Energy = 100.0f;

            AnimCount -= SpeedFaktor;
            if (AnimCount <= 0.0f) {
                AnimCount = 0.5f;

                // brodeln lassen
                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(160)),
                                            yPos + static_cast<float>(random(300)) + yoff, EXPLOSION_MEDIUM2);
                SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);

                if (random(2) == 0)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(160)),
                                                yPos + static_cast<float>(random(300)) + yoff, SMOKEBIG);
            }

            ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay <= 0.0f)
                Energy = 0.0f;
        } break;

        default:
            break;
    }  // switch

    // Hat der Golem den Hurri berührt ?
    if (Handlung != GEGNER::EXPLODIEREN)
        TestDamagePlayers(Timer.sync(5.0f));
}

// --------------------------------------------------------------------------------------
// Golem explodiert
// --------------------------------------------------------------------------------------

void GegnerGolem::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION2);

    for (int p = 0; p < NUMPLAYERS; p++)
        DirectInput.Joysticks[Player[p].JoystickIndex].ForceFeedbackEffect(FFE_BIGRUMBLE);

    int i;

    for (i = 0; i < 100; i++)
        PartikelSystem.PushPartikel(xPos - 50.0f + static_cast<float>(random(200)),
                                    yPos + yoff + static_cast<float>(random(300)), ROCKSPLITTER);

    for (i = 0; i < 80; i++)
        PartikelSystem.PushPartikel(xPos - 50.0f + static_cast<float>(random(200)),
                                    yPos + yoff + static_cast<float>(random(300)), SMOKEBIG);

    for (i = 0; i < 40; i++)
        PartikelSystem.PushPartikel(xPos - 50.0f + static_cast<float>(random(200)),
                                    yPos + yoff + static_cast<float>(random(300)), EXPLOSION_MEDIUM2);

    for (i = 0; i < 10; i++)
        PartikelSystem.PushPartikel(xPos - 50.0f + static_cast<float>(random(200)),
                                    yPos + yoff + static_cast<float>(random(300)), EXPLOSION_BIG);

    ShakeScreen(4);

    Player[0].Score += 8000;

    // Plattform spawnen, damit der Spieler weiterkommt =)
    Gegner.PushGegner(6760, 2500, PLATTFORM, 6, 0, false);

    ScrolltoPlayeAfterBoss();
}
