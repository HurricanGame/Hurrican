// --------------------------------------------------------------------------------------
// Die riesen Piranha
//
// im Tempel Level
// Spuckt kleine Piranhas aus
// --------------------------------------------------------------------------------------

#include "Boss_BigFish.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerBigFish::GegnerBigFish(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::NOTVISIBLE;
    BlickRichtung = DirectionEnum::LINKS;
    Energy = 2000;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimPhase = 0;
    ChangeLight = Light;
    Destroyable = true;
    TestBlock = false;
    OwnDraw = true;
    SinOff = 0.0f;
    MaulWinkel = 0.0f;
    ShotCount = 0;
    Moving = false;
    NewX = 0.0f;
    NewY = 0.0f;
    rotz = 0.0f;
    KugelCount = 150.0f;

    // Zusatzgrafiken laden
    Maul.LoadImage("bigfishmaul.png", 106, 55, 106, 55, 1, 1);
    FlosseKlein.LoadImage("bigfishflosseklein.png", 66, 38, 66, 38, 1, 1);
    FlosseGross.LoadImage("bigfishflossebig.png", 88, 157, 88, 157, 1, 1);
    FlosseOben.LoadImage("bigfishflosseoben.png", 74, 59, 74, 59, 1, 1);
    FlosseUnten.LoadImage("bigfishflosseunten.png", 97, 72, 97, 72, 1, 1);
}

// --------------------------------------------------------------------------------------
// An neue Position moven
// --------------------------------------------------------------------------------------

void GegnerBigFish::MoveTo(float x, float y) {
    Moving = true;

    NewX = x;
    NewY = y;

    xSpeed = ySpeed = 0.0f;

    if (xPos < x)
        xSpeed = 10.0f;
    if (xPos > x)
        xSpeed = -10.0f;
    if (yPos < y)
        ySpeed = 10.0f;
    if (yPos > y)
        ySpeed = -10.0f;
}

void GegnerBigFish::DoMove() {
    if (xSpeed < 0.0f && NewX > xPos) {
        xSpeed += Timer.sync(1.0f);

        if (xSpeed > 0.0f)
            xSpeed = 0.0f;
    }

    if (xSpeed > 0.0f && NewX < xPos) {
        xSpeed -= Timer.sync(1.0f);

        if (xSpeed < 0.0f)
            xSpeed = 0.0f;
    }

    if (ySpeed < 0.0f && NewY > yPos) {
        ySpeed += Timer.sync(1.0f);

        if (ySpeed > 0.0f)
            ySpeed = 0.0f;
    }

    if (ySpeed > 0.0f && NewY < yPos) {
        ySpeed -= Timer.sync(1.0f);

        if (ySpeed < 0.0f)
            ySpeed = 0.0f;
    }

    if (xSpeed == 0.0f && ySpeed == 0.0f) {
        Moving = false;
    }
}

// --------------------------------------------------------------------------------------
// Neue Aktion starten
// --------------------------------------------------------------------------------------

void GegnerBigFish::NewAction() {
    static int lastaction = -1;

    // Spieler rechts vom Fisch? Dann den Fisch bewegen
    if (pAim->xpos > xPos) {
        float newx = pAim->xpos;

        if (newx > Value1 + 400)
            newx = static_cast<float>(Value1) + 400.0f;

        MoveTo(newx, static_cast<float>(Value2) + 200.0f);
    }

    // neue Aktion festlegen
    int j = random(3);

    while (lastaction == j)
        j = random(3);

    lastaction = j;

    switch (j) {
        case 0: {
            ShotCount = 1;
            Handlung = GEGNER::SPECIAL;
        } break;

        case 1: {
            ShotCount = 5;
            Handlung = GEGNER::SCHIESSEN;
        } break;

        case 2: {
            ShotCount = 3;
            Handlung = GEGNER::LAUFEN;
        } break;
    }
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerBigFish::DoDraw() {
    static float FlossenSin = 0.0f;
    static float FlossenSin2 = 0.0f;
    float const mw = sin(-HALF_PI + MaulWinkel) * 35.0f;
    float const fs = sin(FlossenSin) * 40.0f;
    int const fs2 = static_cast<int>(sin(FlossenSin2) * 10.0f);
    float const YOff = sin(SinOff) * 5.0f;

    // Maul rendern
    Maul.RenderSpriteRotatedOffset(xPos - TileEngine.XOffset + 51.0f,
                                   yPos - TileEngine.YOffset + YOff + 94.0f, mw,
                                   12, -17, 0xFFFFFFFF, false);

    // Schwanzflosse rendern
    FlosseGross.RenderSpriteScaled(xPos - TileEngine.XOffset + 203.0f,
                                   yPos - TileEngine.YOffset + YOff - 9.0f,
                                   88 + fs2, 157, 0, 0xFFFFFFFF);

    // obere Floße rendern
    FlosseOben.RenderSpriteScaled(xPos - TileEngine.XOffset + 151.0f,
                                  yPos - TileEngine.YOffset + YOff - 20.0f + fs2, 74,
                                  59 - fs2, 0, 0xFFFFFFFF);

    // untere Floße rendern
    FlosseUnten.RenderSpriteScaled(xPos - TileEngine.XOffset + 140.0f,
                                   yPos - TileEngine.YOffset + YOff + 120.0f,
                                   97, 72 + fs2, 0, 0xFFFFFFFF);

    // Fisch rendern
    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                           yPos - TileEngine.YOffset + YOff, 0,
                                           0xFFFFFFFF, false);

    // kleine Floße rendern
    FlosseKlein.RenderSpriteRotatedOffset(xPos - TileEngine.XOffset + 184.0f,
                                          yPos - TileEngine.YOffset + YOff + 80.0f, fs, -28, -9, 0xFFFFFFFF, false);

    if (!AlreadyDrawn) {
        DirectGraphics.SetAdditiveMode();

        // Leuchtflare rendern

        int a = static_cast<int>(Energy / 2000.0f * 255.0f);

        D3DCOLOR col = D3DCOLOR_RGBA(255, 224, 80, a);

        for (int i = 0; i < 2; i++)
            Projectiles.LavaFlare.RenderSprite(xPos - TileEngine.XOffset - 49.0f,
                                               yPos - TileEngine.YOffset - 22.0f + YOff, 0,
                                               col, false);

        // Flossen bewegen
        float fact = (xSpeed * xSpeed + ySpeed * ySpeed + 20.0f) / 200.0f;

        FlossenSin += Timer.sync(fact);
        if (FlossenSin > TWO_PI)
            FlossenSin = 0.0f;

        FlossenSin2 += Timer.sync(fact);
        if (FlossenSin2 > TWO_PI)
            FlossenSin2 = 0.0f;
    }

    // DirectGraphics.SetColorKeyMode();
    AlreadyDrawn = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerBigFish::DoKI() {
    static bool IsKugel = false;

    KugelCount -= Timer.sync(1.0f);

    if (KugelCount <= 0.0f) {
        int LeftOrRight = 1;
        int Art = 2;
        KugelCount = 80.0f;

        if (Energy > 500)
            Art = 1;

        if (Energy > 1500)
            Art = 0;

        if (random(2) == 0)
            LeftOrRight *= -1;

        if (IsKugel)
            Gegner.PushGegner(TileEngine.XOffset + 300.0f + static_cast<float>(300 * LeftOrRight),
                              TileEngine.YOffset + 200.0f,
                              KUGELKLEIN + Art, 5, 0, false);
        else
            Gegner.PushGegner(TileEngine.XOffset + 300.0f + static_cast<float>(320 * LeftOrRight),
                              TileEngine.YOffset + 350.0f,
                              SWIMWALKER, 5, 0, false);
        IsKugel = !IsKugel;
    }

    // Schwimmbewegung
    SinOff += Timer.sync(0.2f);

    if (SinOff > TWO_PI)
        SinOff = 0.0f;

    // Energie anzeigen
    if (Handlung != GEGNER::INIT && Handlung != GEGNER::EXPLODIEREN)
        HUD.ShowBossHUD(2000, Energy);

    // Levelausschnitt auf den Boss zentrieren, sobald dieser sichtbar wird
    if (Active && Handlung != GEGNER::EXPLODIEREN && TileEngine.Zustand == TileStateEnum::SCROLLBAR && Energy > 0.0f) {
        TileEngine.ScrollLevel(Value1, Value2, TileStateEnum::SCROLLTOLOCK);  // Level auf die Faust zentrieren
        SoundManager.FadeSong(MUSIC::STAGEMUSIC, -2.0f, 0, true);       // Ausfaden und pausieren
        xPos += 400;
    }

    // Zwischenboss blinkt nicht so lange wie die restlichen Gegner
    if (DamageTaken > 0.0f)
        DamageTaken -= Timer.sync(100.0f);  // Rotwerden langsam ausfaden lassen
    else
        DamageTaken = 0.0f;  // oder ganz anhalten

    // Schon schwer angeschlagen ? Dann blutet der Fish
    if (Energy < 1000 && random(20) == 0)
        PartikelSystem.PushPartikel(xPos + 40.0f + static_cast<float>(random(200)),
                                    yPos + 50.0f + static_cast<float>(random(100)), PIRANHABLUT);

    // Hat der Boss keine Energie mehr ? Dann explodiert er
    if (Energy <= 100.0f && Handlung != GEGNER::EXPLODIEREN) {
        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC::BOSS, -2.0f, 0, false);

        // zum Spieler scrollen
        ScrolltoPlayeAfterBoss();

        Handlung = GEGNER::EXPLODIEREN;
        xSpeed = 0.0f;
        ySpeed = 2.0f;
        xAcc = 0.0f;
        yAcc = 1.0f;
        AnimCount = 50.0f;

        Moving = false;
    }

    // Je nach Handlung richtig verhalten
    if (Moving)
        DoMove();
    else
        switch (Handlung) {
            case GEGNER::NOTVISIBLE:  // Warten bis der Screen zentriert wurde
            {
                if (TileEngine.Zustand == TileStateEnum::LOCKED) {
                    // Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
                    if (!SoundManager.SongIsPlaying(MUSIC::BOSS))
                        SoundManager.PlaySong(MUSIC::BOSS, false);

                    // Und Boss erscheinen lassen
                    Handlung = GEGNER::EINFLIEGEN;
                }
            } break;

            case GEGNER::EINFLIEGEN:  // Gegner kommt DAS ERSTE MAL in den Screen geflogen
            {
                Energy = 2000;
                DamageTaken = 0.0f;

                NewAction();
                MoveTo(static_cast<float>(Value1 + 450), yPos);
            } break;

            case GEGNER::LAUFEN: {
                MoveTo(static_cast<float>(Value1 + 50 + random(300)),
                       static_cast<float>(Value2 + 50 + random(200)));
                ShotCount--;

                if (ShotCount <= 0)
                    NewAction();
            } break;

            // einzelne Fische ausspucken
            case GEGNER::SCHIESSEN: {
                static bool shot = false;

                // und Maulbewegung
                MaulWinkel += Timer.sync(0.6f);

                if (MaulWinkel > PI && !shot) {
                    shot = true;
                    Gegner.PushGegner(xPos + 80.0f, yPos + 90.0f, PIRANHA, 99, 1, true);
                    ShotCount--;
                }

                if (MaulWinkel > TWO_PI) {
                    shot = false;
                    MaulWinkel = 0.0f;
                    SoundManager.PlayWave(50, 128, 6000 + random(500), SOUND::KLONG);

                    if (ShotCount <= 0)
                        NewAction();
                }

            } break;

            // mehrere Fische ausspucken
            case GEGNER::SPECIAL: {
                static bool shot = false;

                // und Maulbewegung
                MaulWinkel += Timer.sync(0.1f);

                if (MaulWinkel > PI && !shot) {
                    shot = true;

                    for (int i = 0; i < 9; i++)
                        Gegner.PushGegner(xPos + 80.0f, yPos + 90.0f, PIRANHA, 98, i - 4, true);

                    ShotCount--;
                }

                if (MaulWinkel > TWO_PI) {
                    shot = false;
                    MaulWinkel = 0.0f;
                    SoundManager.PlayWave(50, 128, 6000 + random(500), SOUND::KLONG);

                    if (ShotCount <= 0)
                        NewAction();
                }

            } break;

            // Boss fliegt in die Luft
            case GEGNER::EXPLODIEREN: {
                Energy = 100;
                if (yPos > Value2 + 300.0f)
                    Energy = 0.0f;
            } break;

            default:
                break;
        }  // switch

    // Testen, ob der Fisch den Spieler berührt hat
    GegnerRect[BIGFISH].left = 60;
    GegnerRect[BIGFISH].right = 280;
    GegnerRect[BIGFISH].top = 28 + static_cast<int>(sin(SinOff) * 5.0f);
    GegnerRect[BIGFISH].bottom = 156 + static_cast<int>(sin(SinOff) * 5.0f);

    TestDamagePlayers(Timer.sync(8.0f));

    // Normales Cliprect setzen
    GegnerRect[BIGFISH].left = 0;
    GegnerRect[BIGFISH].right = 17;
    GegnerRect[BIGFISH].top = 28 + static_cast<int>(sin(SinOff) * 5.0f);
    GegnerRect[BIGFISH].bottom = 50 + static_cast<int>(sin(SinOff) * 5.0f);
}

// --------------------------------------------------------------------------------------
// Explodieren
// --------------------------------------------------------------------------------------

void GegnerBigFish::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION2);

    // Blut
    for (int i = 0; i < 80; i++)
        PartikelSystem.PushPartikel(xPos + 40.0f + static_cast<float>(random(200)),
                                    yPos + 50.0f + static_cast<float>(random(100)), PIRANHABLUT);

    for (int i = 0; i < 20; i++)
        Gegner.PushGegner(xPos + 40.0f + static_cast<float>(random(200)),
                          yPos + 30.0f + static_cast<float>(random(80)), PIRANHA, 99, 0, false);

    for (int i = 0; i < NUMPLAYERS; i++)
        DirectInput.Joysticks[Player[i].JoystickIndex].ForceFeedbackEffect(FFE_BIGRUMBLE);

    ShakeScreen(4);

    Player[0].Score += 5000;
}
