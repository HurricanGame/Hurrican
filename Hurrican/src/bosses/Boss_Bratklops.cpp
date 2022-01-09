// --------------------------------------------------------------------------------------
// Der eklige Bratklops
//
// kommt von der Seite reingefahren und kotzt maden
// schiesst ab und zu grünen laser
// --------------------------------------------------------------------------------------

#include "Boss_Bratklops.hpp"
#include "stdafx.hpp"

// DKS - Note: I am not sure if this boss ever actually made it into the final game.
//      I recall encountering it on my test play-throughs. Perhaps this is the reason
//      the bratklopslaser.png sprite below had unusual dimensions compared to its
//      on-disk dimensions (16x16 vs. 30x60 on-disk). Furthermore, bratklopsshot had
//      two frames in its sprite in code, but on-disk only one frame. Even further,
//      in Gegner_Helper.cpp, bratklops0000.png is loaded wi.hpp 323x400 dimensions
//      but its on-disk dimensions are 232x400. All in all, I think this points to
//      the entire Bratklops enemy being a candidate for removal from the game (TODO).

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerBratklops::GegnerBratklops(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_NOTVISIBLE;
    BlickRichtung = LINKS;
    Energy = 8000;
    ChangeLight = Light;
    Destroyable = true;
    AnimPhase = 0;
    AnimStart = 0;
    AnimEnde = 10;
    AnimCount = 0.0f;
    AnimSpeed = 1.0f;
    Value1 = Wert1;
    Value2 = Wert2;
    ShotDelay = 0.0f;
    ActionDelay = 0.0f;
    FlareDelay = 0.0f;
    TestBlock = false;
    OwnDraw = true;

    // Zusätzliche Grafiken laden
    //
    for (int i = 0; i < 6; i++)
        pGfx[i] = new DirectGraphicsSprite();

    pLaser = new DirectGraphicsSprite();
    pFlare = new DirectGraphicsSprite();

    pGfx[0]->LoadImage("bratklops0000.png", 232, 400, 232, 400, 1, 1);
    pGfx[1]->LoadImage("bratklops0001.png", 232, 400, 232, 400, 1, 1);
    pGfx[2]->LoadImage("bratklops0002.png", 232, 400, 232, 400, 1, 1);
    pGfx[3]->LoadImage("bratklops0003.png", 232, 400, 232, 400, 1, 1);
    pGfx[4]->LoadImage("bratklops0004.png", 232, 400, 232, 400, 1, 1);
    pGfx[5]->LoadImage("bratklops0005.png", 232, 400, 232, 400, 1, 1);

    // DKS - Corrected dimensions from 16x16 to 30x60, to match actual image file.
    //      Also see my notes in comments at top of this file.
    pLaser->LoadImage("bratklopslaser.png", 16, 16, 16, 16, 1, 1);
    pFlare->LoadImage("bratklopsshot2.png", 180, 180, 180, 180, 1, 1);

    pFlare->SetRect(0, 0, 180, 180);
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerBratklops::DoDraw() {
    // Gegner rendern
    // Animation existiert nur von 1-5, danach läuft sie rückwärts
    //

    int a;

    a = AnimPhase;
    if (a > 5)
        a = 10 - a;

    pGfx[a]->RenderSprite(xPos - TileEngine.XOffset,
                          yPos - TileEngine.YOffset, 0xFFFFFFFF);

    // Laser rendern ?
    //
    if (FlareDelay > 0.0f) {
        int c = static_cast<int>(FlareDelay);

        if (c > 128.0f)
            c = 128;

        if (FlareDelay > 544)
            c = static_cast<int>(128.0f - (FlareDelay - 544.0f) / 2.0f);

        DirectGraphics.SetAdditiveMode();
        D3DCOLOR Color = D3DCOLOR_RGBA(255, 255, 255, c);
        pFlare->RenderSpriteRotated(xPos - TileEngine.XOffset + 64.0f,
                                    yPos - TileEngine.YOffset + 122.0f,
                                    FlareDelay * 2, Color);
        pFlare->RenderSpriteRotated(xPos - TileEngine.XOffset + 64.0f,
                                    yPos - TileEngine.YOffset + 122.0f,
                                    FlareDelay * 2, Color);

        // Laser rendern
        //
        if (FlareDelay > 150.0f &&
            ((Handlung == GEGNER_SPECIAL && static_cast<int>(FlareDelay) % 90 < 60) || Handlung == GEGNER_SPECIAL3)) {
            // DKS - Added function WaveIsPlaying() to SoundManagerClass:
            if (!SoundManager.WaveIsPlaying(SOUND_BRATLASER))
                SoundManager.PlayWave(100, 128, 11025, SOUND_BRATLASER);

            VERTEX2D TriangleStrip[4];  // Strip für ein Sprite
            int Winkel;
            Winkel = static_cast<int>((FlareDelay - 128.0f) / 4.5f) - 20;

            while (Winkel < 0)
                Winkel += 360;

            float l, r, o, u;      // Vertice Koordinaten
            float tl, tr, to, tu;  // Textur Koordinaten

            l = xPos - TileEngine.XOffset + 140.0f - 0.5f;  // Links
            o = yPos - TileEngine.YOffset + 215.0f - 0.5f;  // Oben
            r = xPos - TileEngine.XOffset + 170.0f + 0.5f;  // Rechts
            u = yPos - TileEngine.YOffset + 800.0f + 0.5f;  // Unten

            tl = 0.0f;
            tr = 1.0f;
            to = 0.0f;
            tu = 1.0f;

            TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color =
                0xFFFFFFFF;
            TriangleStrip[0].x = l;
            TriangleStrip[0].y = o;
            TriangleStrip[0].tu = tl;
            TriangleStrip[0].tv = to;
            TriangleStrip[1].x = r;
            TriangleStrip[1].y = o;
            TriangleStrip[1].tu = tr;
            TriangleStrip[1].tv = to;
            TriangleStrip[2].x = l;
            TriangleStrip[2].y = u;
            TriangleStrip[2].tu = tl;
            TriangleStrip[2].tv = tu;
            TriangleStrip[3].x = r;
            TriangleStrip[3].y = u;
            TriangleStrip[3].tu = tr;
            TriangleStrip[3].tv = tu;

            // Textur setzen
            DirectGraphics.SetTexture(pLaser->itsTexIdx);

            // Blitz rotieren lassen
            glm::mat4x4 matRot, matTrans, matTrans2;

            matRot = glm::rotate(glm::mat4x4(1.0f), DegreetoRad[360 - Winkel], glm::vec3(0.0f, 0.0f, 1.0f));  // Rotationsmatrix
            D3DXMatrixTranslation(&matTrans, -l - 15, -o, 0.0f);      // Transformation zum Ursprung
            D3DXMatrixTranslation(&matTrans2, l + 15, o, 0.0f);       // Transformation wieder zurück
            matWorld = matRot * matTrans;        // Verschieben und rotieren
            matWorld = matTrans2 * matWorld;     // und wieder zurück
            g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
            load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

            DirectGraphics.SetFilterMode(true);

            // Blitzstrahl zeichnen
            //
            DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip[0]);

            DirectGraphics.SetFilterMode(false);

            // Normale Projektions-Matrix wieder herstellen
            //
            matWorld = glm::mat4x4(1.0f);
            g_matModelView = matWorld * g_matView;
#if defined(GL1)
            load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

            DirectGraphics.SetColorKeyMode();

            // Kollisionsabfrage mit Spieler durch rotierte Rechtecke (wie beim Spielerblitz)
            //
            RECT_struct Rect;  // Rechteck für die Kollisionserkennung
            // ein Laser-Stück wird grob durch
            Rect.left = 0;  // ein 24x24 Rechteck abgeschätzt
            Rect.top = 0;
            Rect.right = 24;
            Rect.bottom = 24;

            float xs, ys;
            float xstart = xPos + 145.0f;
            float ystart = yPos + 203.0f;

            // Rechtecke für die Kollisionsabfrage rotieren lassen
            for (int i = 0; i < 25; i++) {
            // Zum anzeigen der Rects, die geprüft werden
#ifndef NDEBUG
                if (DebugMode == true)
                    RenderRect(xstart - TileEngine.XOffset, ystart - TileEngine.YOffset, 24, 24,
                               0x80FFFFFF);
#endif  //NDEBUG

                // Laser auf Kollision mit dem Spieler prüfen
                //

                xs = xstart;
                ys = ystart;

                for (int j = 0; j < NUMPLAYERS; j++)
                    if (SpriteCollision(Player[j].xpos, Player[j].ypos, Player[j].CollideRect, xs, ys, Rect) == true) {
                        Player[j].DamagePlayer(10.0f SYNC);
                    }

                // Und nächstes Rechteck
                //
                // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
                // xstart += static_cast<float>(24*cos(PI * (360 - Winkel + 90) / 180));
                // ystart += static_cast<float>(24*sin(PI * (360 - Winkel + 90) / 180));
                xstart += 24.0f * cos_deg(360 - Winkel + 90);
                ystart += 24.0f * sin_deg(360 - Winkel + 90);

                if (TileEngine.BlockUnten(xs, ys, xs, ys, Rect) & BLOCKWERT_WAND) {
                    // Funken und Rauch am Boden
                    //
                    if (random(2) == 0)
                        PartikelSystem.PushPartikel(xs + static_cast<float>(random(24)),
                                                    ys + static_cast<float>(random(24)), FUNKE2);
                    if (random(2) == 0)
                        PartikelSystem.PushPartikel(xs - 15.0f + static_cast<float>(random(24)),
                                                    ys - 40.0f + static_cast<float>(random(8)), SMOKE2);
                }
            }
        } else
            SoundManager.StopWave(SOUND_BRATLASER);

        if (FlareDelay > 800.0f) {
            FlareDelay = 0.0f;
            Handlung = GEGNER_STEHEN;
        }
    }
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerBratklops::DoKI() {
    //glm::mat4x4 swap;

    /*// TODO
    static float c = 480.0f;
    static float d = 40.0f;

    c += d SYNC;
    if ((d > 0.0f && c > 480.0f)||
        (d < 0.0f && c < 0.0f))
        d *= -1.0f;

    D3DXMatrixOrthoOffCenterLH(&swap, 0, RENDERWIDTH, c, 480 - c, 0.0f, 1.0f);
    lpD3DDevice->SetTransform(D3DTS_PROJECTION, &swap); */

    // Schneller wabbeln
    //
    AnimSpeed = Energy / 8000.0f * 1.8f;

    if (AnimEnde > 0)  // Soll überhaupt anmiert werden ?
    {
        AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;              // Dann wieder auf Null setzen
            AnimPhase++;                // Und nächste Animationsphase
            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
                AnimPhase = AnimStart;  // Dann wieder von vorne beginnen
        }
    }  // animieren

    // Energie anzeigen
    if (Handlung != GEGNER_NOTVISIBLE && Handlung != GEGNER_EXPLODIEREN)
        HUD.ShowBossHUD(8000, Energy);

    // Boss aktivieren und Mucke laufen lassen
    //
    if (Active == true && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        TileEngine.ScrollLevel(static_cast<float>(Value1), static_cast<float>(Value2),
                               TileStateEnum::SCROLLTOLOCK);  // Level auf den Boss zentrieren
        xPos -= 232;                                   // und Boss aus dem Screen setzen

        SoundManager.FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
    }

    // Zwischenboss blinkt nicht so lange wie die restlichen Gegner
    if (DamageTaken > 0.0f)
        DamageTaken -= 50 SYNC;  // Rotwerden langsam ausfaden lassen
    else
        DamageTaken = 0.0f;  // oder ganz anhalten

    // Hat der Boss keine Energie mehr ? Dann explodiert er
    if (Energy <= 100.0f && Handlung != GEGNER_EXPLODIEREN) {
        Handlung = GEGNER_EXPLODIEREN;
        FlareDelay = 0.0f;

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC_BOSS, -2.0f, 0, false);
    }

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER_NOTVISIBLE:  // Warten bis der Screen zentriert wurde
        {
            // xPos = Value1 - 232;

            if (TileEngine.Zustand == TileStateEnum::LOCKED) {
                // Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
                //
                // DKS - Added function SongIsPlaying() to SoundManagerClass:
                if (!SoundManager.SongIsPlaying(MUSIC_BOSS)) {
                    SoundManager.PlaySong(MUSIC_BOSS, false);

                    // Und Boss erscheinen lassen
                    //
                    Handlung = GEGNER_EINFLIEGEN;
                }
            }
        } break;

        case GEGNER_EINFLIEGEN:  // Gegner kommt in den Screen geflogen
        {
            Energy = 8000;
            DamageTaken = 0.0f;

            xPos += 4.0f SYNC;

            if (xPos > Value1) {
                xPos = static_cast<float>(Value1);
                Handlung = GEGNER_STEHEN;
            }
        } break;

        case GEGNER_STEHEN: {
            static int oldaction = 0;

            // Auf nächste Aktion warten
            //
            ActionDelay -= 1.0f SYNC;
            if (ActionDelay <= 0.0f) {
                int j;

                // Keine Aktion doppelt
                //
                do {
                    j = random(6);
                } while (j == oldaction);

                oldaction = j;

                // Ballern
                //
                if (j == 0) {
                    Handlung = GEGNER_BOMBARDIEREN;
                    ActionDelay = 8.0f;
                    Projectiles.PushProjectile(xPos + 146.0f, yPos + 186.0f, BRATKLOPSSHOT);
                    SoundManager.PlayWave(100, 128, 8000, SOUND_GRANATE);
                    Shots = random(3) + 3;
                }

                // Kotzen
                //
                else if (j == 1) {
                    Handlung = GEGNER_SCHIESSEN;
                    ShotDelay = 1.0f;
                    SoundManager.PlayWave(100, 128, 11025, SOUND_KOTZEN);
                } else

                    // Laser
                    //
                    if (j == 2) {
                    Handlung = GEGNER_SPECIAL;
                    FlareDelay = 0.0f;
                    SoundManager.PlayWave(100, 128, 8000, SOUND_SPIDERSCREAM);
                }
                // FettBoller
                //
                else if (j == 3) {
                    Handlung = GEGNER_SPECIAL2;
                    FlareDelay = 0.0f;
                    SoundManager.PlayWave(100, 128, 8000, SOUND_SPIDERSCREAM);
                }

                // Laser von Rechts nach Links
                //
                else if (j == 4) {
                    Handlung = GEGNER_SPECIAL3;
                    FlareDelay = 800.0f;
                    SoundManager.PlayWave(100, 128, 8000, SOUND_SPIDERSCREAM);
                }

                // Pause
                //
                else if (j == 5) {
                    ActionDelay = 16.0f;
                }
            }
        } break;

        // Gegner ballert grüne Rotzbollen
        //
        case GEGNER_BOMBARDIEREN: {
            ActionDelay -= 1.0f SYNC;

            if (ActionDelay <= 0.0f) {
                ActionDelay = 8.0f;
                Projectiles.PushProjectile(xPos + 146.0f, yPos + 186.0f, BRATKLOPSSHOT);
                SoundManager.PlayWave(100, 128, 8000, SOUND_GRANATE);
                Shots--;
                if (Shots == 0)
                    Handlung = GEGNER_STEHEN;
            }
        } break;

        // Laser schiessen, der am Boden entlang wandert (von links nach rechts)
        //
        case GEGNER_SPECIAL: {
            FlareDelay += (10000.0f - Energy) / 1000.0f SYNC;
        } break;

        // Laser schiessen, der am Boden entlang wandert (von rechts nach links)
        //
        case GEGNER_SPECIAL3: {
            FlareDelay -= (10000.0f - Energy) / 750.0f SYNC;
            if (FlareDelay < 210.0f) {
                FlareDelay = 0.0f;
                Handlung = GEGNER_STEHEN;
                ActionDelay = 8.0f;
            }
        } break;

        // Laser aufladen und dann drei Fett Boller schiessen
        //
        case GEGNER_SPECIAL2: {
            FlareDelay += (10000.0f - Energy) / 1000.0f SYNC;
            if (FlareDelay > 255.0f) {
                FlareDelay = 0.0f;
                Handlung = GEGNER_STEHEN;
                ActionDelay = 8.0f;

                Projectiles.PushProjectile(xPos + 60.0f, yPos + 50.0f, BRATKLOPSSHOT);
                Projectiles.PushProjectile(xPos + 84.0f, yPos + 80.0f, BRATKLOPSSHOT);
                Projectiles.PushProjectile(xPos + 108.0f, yPos + 115.0f, BRATKLOPSSHOT);
                Projectiles.PushProjectile(xPos + 130.0f, yPos + 150.0f, BRATKLOPSSHOT);
                Projectiles.PushProjectile(xPos + 145.0f, yPos + 185.0f, BRATKLOPSSHOT);
                Projectiles.PushProjectile(xPos + 130.0f, yPos + 220.0f, BRATKLOPSSHOT);
                Projectiles.PushProjectile(xPos + 108.0f, yPos + 255.0f, BRATKLOPSSHOT);
                Projectiles.PushProjectile(xPos + 84.0f, yPos + 290.0f, BRATKLOPSSHOT);
                Projectiles.PushProjectile(xPos + 60.0f, yPos + 320.0f, BRATKLOPSSHOT);

                SoundManager.PlayWave(100, 128, 8000, SOUND_GRANATE);
                SoundManager.PlayWave(100, 128, 11025, SOUND_GRANATE);
                SoundManager.PlayWave(100, 128, 11025, SOUND_BRATLASER);
            }
        } break;

        case GEGNER_EXPLODIEREN: {
            Energy = 100.0f;

            // Explodieren
            //
            if (random(5) == 0)
                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(180)),
                                            yPos + static_cast<float>(random(500)), EXPLOSION_GREEN);
            if (random(3) == 0)
                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(150)),
                                            yPos + 100.0f + static_cast<float>(random(200)) , MADEBLUT);
            if (random(8) == 0)
                SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND_EXPLOSION1);

            xPos -= 3.0f SYNC;

            // Fertig explodiert ? Dann wird der Spacko ganz zerlegt
            //
            if (xPos - TileEngine.XOffset <= -300)
                Energy = 0.0f;
        } break;

        case GEGNER_SCHIESSEN: {
            // Aktion vorbei ?
            //
            // DKS - Added function WaveIsPlaying() to SoundManagerClass:
            // if (SoundManager.its_Sounds [SOUND_KOTZEN]->isPlaying == false)
            if (!SoundManager.WaveIsPlaying(SOUND_KOTZEN)) {
                ActionDelay = 10.0f;
                Handlung = GEGNER_STEHEN;
            }

            // Made kotzen
            //
            ShotDelay -= 1.0f SYNC;
            if (ShotDelay <= 0.0f) {
                if (Skill == SKILL_EASY)
                    ShotDelay = 0.40f;
                else if (Skill == SKILL_MEDIUM)
                    ShotDelay = 0.30f;
                else
                    ShotDelay = 0.20f;

                Gegner.PushGegner(xPos + 121.0f + static_cast<float>(random(6)),
                                  yPos + 105.0f + static_cast<float>(random(6)), MADE, 0, 0, false);
            }
        } break;

        default:
            break;
    }  // switch

    // Testen, ob der Spieler den Bratklops berührt hat
    // dafür nehmen wir ein anderes Rect, weil das normale GegnerRect nur das Grüne Auge ist, wo man den Gegner treffen
    // kann
    //
    // RECT_struct rect;

    // rect.top    = 100;
    // rect.left   = 0;
    // rect.bottom = 340;
    // rect.right  = 60;

    TestDamagePlayers(4.0f SYNC);

    // rect.top    = 100;
    // rect.left   = 0;
    // rect.bottom = 300;
    // rect.right  = 120;

    TestDamagePlayers(4.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Braktklops explodiert
// --------------------------------------------------------------------------------------

void GegnerBratklops::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 11025, SOUND_EXPLOSION2);

    // Zusäzliche Grafiken freigeben
    //
    for (int i = 0; i < 6; i++)
        delete (pGfx[i]);

    delete (pLaser);
    delete (pFlare);

    ScrolltoPlayeAfterBoss();
}
