// --------------------------------------------------------------------------------------
// Der Riesen-Spinnen Endboss
//
// --------------------------------------------------------------------------------------

#include "Boss_RiesenSpinne.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerRiesenSpinne::GegnerRiesenSpinne(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::NOTVISIBLE;
    BlickRichtung = DirectionEnum::LINKS;

    Energy = 100000;
    LastEnergy = 100000;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    OwnDraw = true;
    TestBlock = false;

    ShotMode = 0;
    HeadWinkel = 0.0f;
    HeadXOffset = 0.0f;
    WalkCount = 0.0f;
    DamageWackel = 0.0f;
    WalkDir = -5.0f;
    yBody = 0.0f;
    ShotCount = 0;

    // Zusätzliche Grafiken für die Beine
    Head.LoadImage("spiderboss_kopf.png", 127, 92, 127, 92, 1, 1);
    Legs[0].LoadImage("spiderboss_foot.png", 305, 388, 61, 97, 5, 4);
    Legs[1].LoadImage("spiderboss_leg2.png", 320, 228, 32, 114, 10, 2);
    Legs[2].LoadImage("spiderboss_leg1.png", 340, 308, 34, 154, 10, 2);
}

// --------------------------------------------------------------------------------------
//
// --------------------------------------------------------------------------------------

void GegnerRiesenSpinne::StopCurrentAction() {}

// --------------------------------------------------------------------------------------
// Bein zeichnen
// x,y: Position
// winkel: Drehwinkel, geht von 0 bis 2 PI , wobei 0 = 2 PI = Ausgangsposition
// anim: Grafikausschnitt
// --------------------------------------------------------------------------------------

void GegnerRiesenSpinne::DrawLeg(float x, float y, float winkel, int anim, int off, D3DCOLOR col) {

    x += 120;

    float xp, yp;

    // Je nach "winkel" die beine anderes positionieren
    if (winkel < PI) {
        xp = x + (static_cast<float>(sin(winkel + HALF_PI) * 18.0f));
        yp = y + (static_cast<float>(cos(winkel + HALF_PI) * 30.0f));
    } else {
        //		xp = x + (static_cast<float>(sin(winkel + PI / 2.0f) * 20.0f));  //DKS - Was already commented out in original source
        xp = x + (winkel - PI) * 10.0f - 16;
        yp = y;
    }

    float tempdamage = sin(DamageWackel * TWO_PI) * 10.0f;

    //----- Oberteil des Beins rendern

    float yoff = sin(WalkCount * 2.0f);
    float yoff2 = sin(winkel - PI - 0.5f) * 5.0f;
    Legs[2].itsRect = Legs[2].itsPreCalcedRects[anim * 3 + off];

    // Vertice Koordinaten
    float l = xp - 0.5f;                                                                          // Links
    float r = xp + (Legs[2].itsRect.right - Legs[2].itsRect.left - 1) + 0.5f;                     // Rechts
    float o = y + 55.0f - 0.5f - (Legs[2].itsRect.bottom - Legs[2].itsRect.top) + yoff2 + yBody;  // Oben
    float u = y + 30.0f + 0.5f + yoff + tempdamage + yBody;                                       // unten

    // Textur Koordinaten
    float tl = Legs[2].itsRect.left * Legs[2].itsXTexScale;    // Links
    float tr = Legs[2].itsRect.right * Legs[2].itsXTexScale;   // Rechts
    float to = Legs[2].itsRect.top * Legs[2].itsYTexScale;     // Oben
    float tu = Legs[2].itsRect.bottom * Legs[2].itsYTexScale;  // Unten

    VERTEX2D TriangleStrip[4];  // Strip für einen Schuss

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = col;

    TriangleStrip[0].x = l;  // Links oben
    TriangleStrip[0].y = o + yoff;
    TriangleStrip[0].tu = tl;
    TriangleStrip[0].tv = to;

    TriangleStrip[1].x = r;  // Rechts oben
    TriangleStrip[1].y = o + yoff;
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

    // TriangleStrip[0].x = xPos + 100 + anim*40;
    // TriangleStrip[1].x = xPos + 100 + anim*40 + 34;

    // float winkel2 = -static_cast<float>(static_cast<int>(winkel + 320.0f) % 360); // Pickle not used
    float temp = sin(winkel) * 10.0f;

    switch (anim) {
        case 0:
            TriangleStrip[0].x = temp + x + 40;
            TriangleStrip[1].x = temp + x + 32 + 40;
            TriangleStrip[2].x = x + 120;
            TriangleStrip[3].x = x + 34 + 120;
            break;

        case 1:
            TriangleStrip[0].x = temp + x + 24;
            TriangleStrip[1].x = temp + x + 32 + 24;
            TriangleStrip[2].x = x + 40;
            TriangleStrip[3].x = x + 34 + 40;
            break;

        case 2:
            TriangleStrip[0].x = temp + x - 10;
            TriangleStrip[1].x = temp + x + 32 - 10;
            TriangleStrip[2].x = x - 40;
            TriangleStrip[3].x = x + 34 - 40;
            break;

        case 3:
            TriangleStrip[0].x = temp + x - 20;
            TriangleStrip[1].x = temp + x + 32 - 20;
            TriangleStrip[2].x = x - 120;
            TriangleStrip[3].x = x + 34 - 120;
            break;
    }

    DirectGraphics.SetTexture(Legs[2].itsTexIdx);
    DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip[0]);

    //----- Unterteil des Beines rendern

    Legs[1].itsRect = Legs[1].itsPreCalcedRects[anim * 3 + off];

    l = xp - 0.5f;                                                                          // Links
    r = xp + (Legs[1].itsRect.right - Legs[1].itsRect.left - 1) + 0.5f;                     // Rechts
    o = y + 20.0f - 0.5f - (Legs[1].itsRect.bottom - Legs[1].itsRect.top) + yoff2 + yBody;  // Oben
    u = yp + +20.0f + 0.5f;                                                                 // unten

    tl = Legs[1].itsRect.left * Legs[1].itsXTexScale;    // Links
    tr = Legs[1].itsRect.right * Legs[1].itsXTexScale;   // Rechts
    to = Legs[1].itsRect.top * Legs[1].itsYTexScale;     // Oben
    tu = Legs[1].itsRect.bottom * Legs[1].itsYTexScale;  // Unten

    TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = col;

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

    switch (anim) {
        case 0:
            TriangleStrip[0].x = temp + x + 40;
            TriangleStrip[1].x = temp + x + 32 + 40;
            TriangleStrip[2].x += 20;
            TriangleStrip[3].x += 20;
            break;

        case 1:
            TriangleStrip[0].x = temp + x + 20;
            TriangleStrip[1].x = temp + x + 32 + 20;
            TriangleStrip[2].x += 20;
            TriangleStrip[3].x += 20;
            break;

        case 2:
            TriangleStrip[0].x = temp + x - 20;
            TriangleStrip[1].x = temp + x + 32 - 20;
            break;

        case 3:
            TriangleStrip[0].x = temp + x - 40;
            TriangleStrip[1].x = temp + x + 32 - 40;
            break;
    }

    DirectGraphics.SetTexture(Legs[1].itsTexIdx);
    DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip[0]);

    //----- Fuß rendern
    Legs[0].RenderSpriteRotated(xp, yp, 0, anim * 3 + off, col);
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerRiesenSpinne::DoDraw() {
    D3DCOLOR Color = 0xFFFFFFFF;

    DirectGraphics.SetFilterMode(true);

    // Beine hinter dem Körper
    for (int i = 0; i < 4; i++) {
        int a;

        if (i == 0)
            a = 0;
        if (i == 1)
            a = 1;
        if (i == 2)
            a = 3;
        if (i == 3)
            a = 2;

        float w = LegsAnim[a] + PI;

        if (w > TWO_PI)
            w -= TWO_PI;

        DrawLeg(xPos - TileEngine.XOffset - 70.0f + static_cast<float>(a * 110),
                yPos - TileEngine.YOffset + 130.0f, w, a, 10, 0xFF888888);
    }

    float tempdamage = sin(DamageWackel * TWO_PI) * 10.0f;
    float tempwinkel = HeadWinkel - sin(DamageWackel) * 12.0f;

    clampAngle(tempwinkel);

    // Rumpf
    pGegnerGrafix[GegnerArt]->RenderSprite(
        xPos - TileEngine.XOffset,
        yPos - TileEngine.YOffset + static_cast<float>(sin(WalkCount) * 2.0f) + tempdamage + yBody,
        0, Color);

    // Kopf
    Head.RenderSpriteRotatedOffset(
        xPos - TileEngine.XOffset - 90.0f + 120.0f + static_cast<float>(sin(HeadXOffset) * 10.0f),
        yPos - TileEngine.YOffset + 60.0f + static_cast<float>(sin(WalkCount) * 2.0f) + tempdamage +
            yBody,
        tempwinkel, 127, 0, Color);

    // Beine vor dem Körper
    for (int i = 0; i < 4; i++) {
        int a;

        if (i == 0)
            a = 0;
        if (i == 1)
            a = 1;
        if (i == 2)
            a = 3;
        if (i == 3)
            a = 2;

        DrawLeg(xPos - TileEngine.XOffset - 70.0f + static_cast<float>(a * 110),
                yPos - TileEngine.YOffset + 130.0f, LegsAnim[a], a, 0, 0xFFFFFFFF);
    }

    // Evtl. Lila Leuchten vor kopf
    if (Handlung == GEGNER::LAUFEN_LINKS && Energy < 75000.0f) {
        int a = static_cast<int>(255.0f - ShotDelay / 15.0f * 255.0f);

        DirectGraphics.SetAdditiveMode();

        Projectiles.LavaFlare.RenderSprite(
            xPos - TileEngine.XOffset - 10.0f + static_cast<float>(sin(HeadXOffset) * 10.0f),
            yPos - TileEngine.YOffset + 50.0f + static_cast<float>(sin(WalkCount) * 2.0f) +
                tempdamage + yBody,
            D3DCOLOR_RGBA(255, 0, 255, a));

        Projectiles.LavaFlare.RenderSpriteScaled(
            xPos - TileEngine.XOffset + 30.0f + static_cast<float>(sin(HeadXOffset) * 10.0f),
            yPos - TileEngine.YOffset + 90.0f + static_cast<float>(sin(WalkCount) * 2.0f) +
                tempdamage + yBody,
            40, 40, D3DCOLOR_RGBA(255, 255, 255, a));
    }

    // Evtl. Lila Leuchten vor kopf
    if (Handlung == GEGNER::SPECIAL2) {
        int a = static_cast<int>(ShotDelay);

        DirectGraphics.SetAdditiveMode();

        Projectiles.LavaFlare.RenderSpriteScaled(
            xPos - TileEngine.XOffset - 20.0f + static_cast<float>(sin(HeadXOffset) * 10.0f),
            yPos - TileEngine.YOffset + 60.0f + static_cast<float>(sin(WalkCount) * 2.0f) +
                tempdamage + yBody,
            100, 100, D3DCOLOR_RGBA(255, 255, 255, a));

        Projectiles.LavaFlare.RenderSpriteScaled(
            xPos - TileEngine.XOffset - 90.0f + static_cast<float>(sin(HeadXOffset) * 10.0f),
            yPos - TileEngine.YOffset - 10.0f + static_cast<float>(sin(WalkCount) * 2.0f) +
                tempdamage + yBody,
            240, 240, D3DCOLOR_RGBA(255, 255, 255, a));
    }

    DirectGraphics.SetFilterMode(false);
    DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// Neue Handlung, wenn sie links steht
// --------------------------------------------------------------------------------------

void GegnerRiesenSpinne::RandomHandlung() {
    ShotDelay = 4.0f;
    xSpeed = 0.0f;

    static int last = -1;
    int j;

    // Per Zufall neue Aktion, die eben noch nicht dran war
    do {
        j = random(6);

    } while (j == last);

    // letzte Aktion merken
    last = j;

    switch (j) {
        case 0: {
            ShotCount = 10 + random(10);
            Handlung = GEGNER::SCHIESSEN;
        } break;

        case 1: {
            Handlung = GEGNER::ABSENKEN;
        } break;

        case 2: {
            ShotCount = 20 + random(10);
            Handlung = GEGNER::SPECIAL;
        } break;

        case 3: {
            Handlung = GEGNER::ABSENKENZWEI;
        } break;

        case 4: {
            Handlung = GEGNER::LAUFEN_LINKS;
            ShotDelay = 1.0f;
            ShotMode = random(2);
        } break;

        case 5: {
            Handlung = GEGNER::SPECIAL2;
            Projectiles.PushProjectile(static_cast<float>(Value1), yPos + yBody + 110.0f, SPIDERLASER);
            SoundManager.PlayWave(100, 128, 11025, SOUND::BEAMLOAD2);
            AnimCount = 140;
            ShotDelay = 0.0f;
        } break;
    }
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerRiesenSpinne::DoKI() {
    // Energie anzeigen
    if (Handlung != GEGNER::NOTVISIBLE && Handlung != GEGNER::EXPLODIEREN)
        HUD.ShowBossHUD(100000, Energy);

    // Levelausschnitt auf die RiesenSpinne zentrieren, sobald diese sichtbar wird
    if (Active == true && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        TileEngine.ScrollLevel(static_cast<float>(Value1), static_cast<float>(Value2),
                               TileStateEnum::SCROLLTOLOCK);  // Level auf die Spinne zentrieren

        SoundManager.FadeSong(MUSIC::STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
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
            }
        } break;

        case GEGNER::EINFLIEGEN:  // Spinne läuft ins Level ein
        {
            yPos -= 55;
            Energy = 100000;
            DamageTaken = 0.0f;

            //			if (xPos <= Value1 + 460)			// Weit genug eingelaufen ?
            {
                SoundManager.PlayWave(100, 128, 11025, SOUND::SPIDERSCREAM);
                Handlung = GEGNER::LAUFEN_LINKS;
                ShotDelay = 1.0f;
                ShotMode = random(2);
            }
        } break;

        // Spinne läuft nach links oder rechts
        case GEGNER::CRUSHEN:
        case GEGNER::LAUFEN_LINKS:
        case GEGNER::LAUFEN_RECHTS:
        case GEGNER::EXPLODIEREN: {
            // Kopf gerade richten
            if (HeadWinkel > 0.0f)
                HeadWinkel -= Timer.sync(1.0f);

            if (HeadWinkel < 0.0f)
                HeadWinkel += Timer.sync(1.0f);

            xSpeed = WalkDir;
            WalkCount -= Timer.sync(WalkDir / 10.0f);

            if (WalkCount > TWO_PI)
                WalkCount -= TWO_PI;
            if (WalkCount < 0.0f)
                WalkCount += TWO_PI;

            if (Handlung == GEGNER::CRUSHEN) {
                if (HeadWinkel < 45.0f)
                    HeadWinkel += Timer.sync(1.0f);

                WalkDir -= Timer.sync(10.0f);

                if (WalkDir < -35.0f)
                    WalkDir = -35.0f;

                if (xPos < Value1 - 50) {
                    Handlung = GEGNER::LAUFEN_RECHTS;
                    ShotDelay = 15.0f;
                }

                // Spieler wegschieben?

                RECT_struct coll;

                coll.left = 50;
                coll.right = 120;
                coll.top = 40 + static_cast<int>(yBody);
                coll.bottom = 140 + static_cast<int>(yBody);
                Wegschieben(coll, 50.0f);
            }

            if (Handlung == GEGNER::LAUFEN_LINKS) {
                WalkDir -= Timer.sync(1.0f);

                if (WalkDir < -8.0f)
                    WalkDir = -8.0f;

                if (xPos < Value1 + 100) {
                    Handlung = GEGNER::LAUFEN_RECHTS;
                    ShotDelay = 15.0f;
                }

                // evtl schiessen
                if (Energy < 75000.0f) {
                    ShotDelay -= Timer.sync(1.0f);

                    if (DamageWackel > 0.0f)
                        ShotDelay = 15.0f;

                    if (ShotDelay < 0.0f) {
                        ShotDelay = 12.0f;

                        SoundManager.PlayWave(100, 128, 8000 + random(1000), SOUND::LILA);
                        PartikelSystem.PushPartikel(xPos + 30.0f - 50.0f, yPos + yBody + 85.0f - 35.0f,
                                                    EXPLOSIONFLARE2);

                        if (ShotMode == 0)
                            Projectiles.PushProjectile(xPos + 30.0f, yPos + yBody + 85.0f, SPIDERSLOW);
                        else {
                            for (int i = 0; i < 5; i++) {
                                WinkelUebergabe = 212 - i * 15.0f;
                                Projectiles.PushProjectile(xPos, yPos + yBody + 85.0f, SPIDERSHOTLANGSAM);
                            }
                        }
                    }
                }
            }

            if (Handlung == GEGNER::LAUFEN_RECHTS || Handlung == GEGNER::EXPLODIEREN) {
                WalkDir += Timer.sync(1.0f);

                // Körper hoch, wenn er grade unten ist
                if (yBody > 0.0f) {
                    yBody -= Timer.sync(1.0f);

                    if (yBody < 0.0f)
                        yBody -= 0.0f;
                }

                if (Handlung == GEGNER::LAUFEN_RECHTS) {
                    // Spinnen hinten rauslassen
                    ShotDelay -= Timer.sync(1.0f);
                    if (ShotDelay < 0.0f) {
                        ShotDelay = 15.0f;

                        Gegner.PushGegner(xPos + 400, yPos + 100.0f + yBody, SPIDERBOMB, 0, 0, false);
                    }
                }

                // Nach crushen kurz schneller laufen
                if (xPos < Value1 - 550)
                    WalkDir = 30.0f;

                else {
                    if (Handlung == GEGNER::LAUFEN_RECHTS)
                        if (WalkDir > 8.0f)
                            WalkDir = 8.0f;

                    if (Handlung == GEGNER::EXPLODIEREN)
                        if (WalkDir > 4.0f)
                            WalkDir = 4.0f;
                }

                if (Handlung == GEGNER::LAUFEN_RECHTS)
                    if (xPos > Value1 + 400)
                        RandomHandlung();
            }

            if (Handlung == GEGNER::EXPLODIEREN) {
                Energy = 100.0f;

                AnimCount -= Timer.sync(1.0f);

                if (AnimCount < 0.0f) {
                    AnimCount = 0.4f;

                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(400)),
                                                yPos + static_cast<float>(random(100)),
                                                EXPLOSION_MEDIUM + random(2));
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(random(400)),
                                                yPos + static_cast<float>(random(100)), SMOKEBIG);

                    if (random(5) == 0)
                        SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION3 + random(2));
                }

                if (xPos > Value1 + 640)
                    Energy = 0.0f;
            }

            // AnimStufen für die einzelnen Beine setzen
            for (int i = 0; i < 4; i++) {
                LegsAnim[i] = WalkCount + (PI / 3.0f) * static_cast<float>(i);

                while (LegsAnim[i] > TWO_PI)
                    LegsAnim[i] -= TWO_PI;
            }

        } break;

        // Raketen abschiessen
        case GEGNER::SCHIESSEN: {
            ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay < 0.0f) {
                ShotDelay = 4.0f;

                // Rakete abschiessen
                SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND::ROCKET);
                Gegner.PushGegner(xPos + 35.0f, yPos + 95.0f, FETTERAKETE, random(60) - 130, 0, false);

                // Kopf zurückschnellen lassen
                HeadXOffset = TWO_PI;

                // Shusscounter runterzählen
                ShotCount--;
                if (ShotCount == 0 || pAim->xpos > xPos - 50) {
                    Handlung = GEGNER::LAUFEN_LINKS;
                    ShotMode = random(2);
                    ShotDelay = 5.0f;
                }
                // RandomHandlung();
            }
        } break;

        // Kopf bewegen und Schüsse abgeben
        case GEGNER::SPECIAL: {
            static float headdir = 1.0f;

            HeadWinkel += Timer.sync(headdir);

            if (HeadWinkel > 15.0f)
                headdir = -1.0f;
            if (HeadWinkel < -10.0f)
                headdir = 1.0f;

            ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay < 0.0f) {
                ShotDelay = 3.5f;

                // Böller abschiessen
                SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND::LILA);
                WinkelUebergabe = 180 - HeadWinkel * 1.5f;
                // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
                // Projectiles.PushProjectile(xPos - (float)cos(HeadWinkel * PI / 180.0f) * 10.0f,
                //                             yPos + (float)sin(HeadWinkel * PI / 180.0f) * 140.0f + 90.0f + yBody,
                //                             SPIDERSHOT);
                Projectiles.PushProjectile(xPos - cos_deg(HeadWinkel) * 10.0f,
                                           yPos + sin_deg(HeadWinkel) * 140.0f + 90.0f + yBody, SPIDERSHOT);

                // Kopf zurückschnellen lassen
                HeadXOffset = TWO_PI;

                // Shusscounter runterzählen
                ShotCount--;
                if (ShotCount == 0) {
                    Handlung = GEGNER::LAUFEN_LINKS;
                    ShotMode = random(2);
                    ShotDelay = 5.0f;
                }
                // RandomHandlung();
            }
        } break;

        case GEGNER::SPECIAL2: {
            // Flare einfaden
            if (ShotDelay < 255.0f)
                ShotDelay += Timer.sync(5.8f);
            else
                ShotDelay = 255.0f;

            // Counter runterzählen
            AnimCount -= Timer.sync(2.5f);
            if (AnimCount < 0.0f) {
                Handlung = GEGNER::LAUFEN_LINKS;
                ShotMode = random(2);
                ShotDelay = 5.0f;
            }
        } break;

        case GEGNER::ABSENKEN: {
            yBody += Timer.sync(1.0f);

            // ganz unten?
            if (yBody > 50.0f) {
                Handlung = GEGNER::AUFRICHTEN;
            }

            // schiessen
            ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay < 0.0f) {
                ShotDelay = 10.0f;

                // Böller abschiessen
                SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND::LILA);
                Projectiles.PushProjectile(xPos + 20.0f, yPos + 80.0f + yBody, SPIDERSHOT2);
                Projectiles.PushProjectile(xPos + 20.0f, yPos + 100.0f + yBody, SPIDERSHOT2);

                // Kopf zurückschnellen lassen
                HeadXOffset = static_cast<float>(M_PI * 3.0);
            }
        } break;

        case GEGNER::ABSENKENZWEI: {
            yBody += Timer.sync(8.0f);

            // ganz unten? Dann nach links losrennen
            if (yBody > 50.0f) {
                Handlung = GEGNER::CRUSHEN;
                SoundManager.PlayWave(100, 128, 11025, SOUND::SPIDERSCREAM);
            }
        } break;

        case GEGNER::AUFRICHTEN: {
            yBody -= Timer.sync(1.0f);

            if (yBody <= 0.0f) {
                yBody = 0.0f;
                Handlung = GEGNER::LAUFEN_LINKS;
                ShotMode = random(2);
                ShotDelay = 5.0f;
                // RandomHandlung();
            }

            // schiessen
            ShotDelay -= Timer.sync(1.0f);

            if (ShotDelay < 0.0f) {
                ShotDelay = 10.0f;

                // Böller abschiessen
                SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND::LILA);
                WinkelUebergabe = PlayerAbstandHoriz() / 10.0f;
                Projectiles.PushProjectile(xPos + 20.0f, yPos + 80.0f + yBody, SPIDERSHOT2);
                Projectiles.PushProjectile(xPos + 20.0f, yPos + 100.0f + yBody, SPIDERSHOT2);

                // Kopf zurückschnellen lassen
                HeadXOffset = static_cast<float>(M_PI * 3.0);
            }
        } break;

        default:
            break;
    }  // switch

    // Kopf zurückgeschnellt?
    if (HeadXOffset > PI)
        HeadXOffset -= Timer.sync(0.8f);
    else
        HeadXOffset = PI;

    // Spinne hat Schaden genommen?
    if (DamageTaken > 0.0f) {
        // War es ein Fass? Dann den Kopf heben
        if (LastEnergy - Energy > 5000) {
            if (Handlung != GEGNER::SPECIAL2)
                DamageWackel = PI;

            SoundManager.PlayWave(100, 128, 15000, SOUND::SPIDERSCREAM);

            if (Handlung == GEGNER::SCHIESSEN || Handlung == GEGNER::SPECIAL || Handlung == GEGNER::ABSENKEN ||
                Handlung == GEGNER::AUFRICHTEN)
                Handlung = GEGNER::LAUFEN_LINKS;
        } else
            DamageTaken = 0.0f;

        LastEnergy = Energy;
    }

    // Spinne wackelt gerade mit dem Kopf wegen Schaden?
    if (DamageWackel > 0.0f) {
        DamageWackel -= Timer.sync(0.2f);
    } else
        DamageWackel = 0.0f;

    GegnerRect[RIESENSPINNE].left = 60;
    GegnerRect[RIESENSPINNE].right = 360;
    GegnerRect[RIESENSPINNE].top = 40 + static_cast<int>(yBody);
    GegnerRect[RIESENSPINNE].bottom = 140 + static_cast<int>(yBody);

    // Testen, ob der Spieler den Boss berührt hat
    TestDamagePlayers(Timer.sync(4.0f));

    // Hat die Spinnen keine Energie mehr ? Dann explodiert sie
    if (Energy <= 100.0f && Handlung != GEGNER::EXPLODIEREN) {
        Energy = 100.0f;
        Handlung = GEGNER::EXPLODIEREN;
        AnimCount = 1.0f;
        Projectiles.ClearType(SPIDERLASER);

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC::BOSS, -2.0f, 0, false);
    }
}

// --------------------------------------------------------------------------------------
// RiesenSpinne explodiert
// --------------------------------------------------------------------------------------

void GegnerRiesenSpinne::GegnerExplode() {
    Player[0].Score += 15000;
    SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION2);
    ShakeScreen(5.0f);

    ScrolltoPlayeAfterBoss();
}
