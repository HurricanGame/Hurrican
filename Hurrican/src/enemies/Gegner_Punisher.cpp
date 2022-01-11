// --------------------------------------------------------------------------------------
// Der Punisher
//
// Erscheint, wenn die Zeit abgelaufen ist.
// Der Punisher verfolgt den Spieler ständig, dafür recht langsam.
// Er ist nahezu unbesiegbar, und bei Berührung verliert der Spieler ein Leben und alle
// Waffen-Powerups, erhält dafür aber wieder volle Zeit
// Besiegt der Spieler den Punisher, so erhält er dafür ein Extraleben, voll Zeit
// und massenhaft Extras (muss ich mir noch ausdenken, welche)
// --------------------------------------------------------------------------------------

#include "Gegner_Punisher.hpp"
#include "HUD.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPunisher::GegnerPunisher(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::INIT;
    Energy = 20000;

    ChangeLight = Light;
    Destroyable = false;
    Value1 = Wert1;
    Value2 = Wert2;
    OwnDraw = true;
    TestBlock = false;
    Active = true;
    alpha = 0.0f;
    ShotDelay = 0.0f;

    AnimPhase = 15;
    AnimCount = 15.0f;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerPunisher::DoDraw() {
    // Je nach Handlung entsprechend anders rendern
    //
    switch (Handlung) {
        // in Schwabbelstreifen rendern
        case GEGNER::INIT2:
        case GEGNER::SPECIAL: {
            for (int i = 0; i < 170; i++) {
                pGegnerGrafix[GegnerArt]->SetRect(3 * 170, 2 * 170 + i, 4 * 170, 2 * 170 + i + 1);
                pGegnerGrafix[GegnerArt]->RenderSprite(
                        xPos - TileEngine.XOffset +
                        static_cast<float>(sin((alpha / 20.0f) + i / 10.0f) * ((255.0f - alpha) / 255.0f * 200.0f)),
                    yPos - TileEngine.YOffset + static_cast<float>(i),
                    D3DCOLOR_RGBA(255, 255, 255, static_cast<int>(alpha)));
            }
        } break;

        // normal rendern
        case GEGNER::LAUFEN: {
            pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                                   yPos - TileEngine.YOffset, AnimPhase,
                                                   D3DCOLOR_RGBA(255, 255, 255, 255), false);
        } break;
    }
}

// --------------------------------------------------------------------------------------
// Spieler verfolgen
// --------------------------------------------------------------------------------------

void GegnerPunisher::FollowPlayer() {
    if (xPos + 50 < pAim->xpos + 35)
        xAcc = 2.0f;
    if (xPos + 50 > pAim->xpos + 35)
        xAcc = -2.0f;

    if (yPos + 100 < pAim->ypos + 40)
        yAcc = 2.0f;
    if (yPos + 100 > pAim->ypos + 40)
        yAcc = -2.0f;

    xSpeed = std::clamp(xSpeed, -15.0f, 15.0f);
    ySpeed = std::clamp(ySpeed, -15.0f, 15.0f);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerPunisher::DoKI() {
    switch (Handlung) {
        // initialisieren
        case GEGNER::INIT: {
            // zentrieren
            xPos = static_cast<float>(TileEngine.XOffset + 320.0f - 100.0f / 2.0f);
            yPos = static_cast<float>(TileEngine.YOffset + 240.0f - 95.0f / 2.0f);

            alpha = 0.0f;
            Handlung = GEGNER::INIT2;

            SoundManager.StopSong(MUSIC::STAGEMUSIC, true);

            // DKS - Punisher music is now loaded on-demand:
            SoundManager.LoadSong("Punisher.it", MUSIC::PUNISHER);

            SoundManager.PlaySong(MUSIC::PUNISHER, false);

        } break;

        // einfaden
        case GEGNER::INIT2: {
            // einfaden
            alpha += 5.0f SYNC;

            if (alpha > 255.0f) {
                alpha = 255.0f;
                Handlung = GEGNER::LAUFEN;
                Destroyable = true;
            }

        } break;

        // Spieler verfolgen
        case GEGNER::LAUFEN: {
            // Spieler verfolgen
            FollowPlayer();

            // Testen, ob der Spieler den Punisher berührt hat
            for (int p = 0; p < NUMPLAYERS; p++)
                if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], Player[p].xpos, Player[p].ypos,
                                    Player[p].CollideRect) == true) {
                    // Spieler stirbt
                    Player[p].Energy = 0.0f;

                    // und verliert Waffen
                    for (int i = 0; i < 4; i++)
                        if (Player[p].CurrentWeaponLevel[i] > 1)
                            Player[p].CurrentWeaponLevel[i]--;

                    Player[p].CalcWeaponLevels();

                    // bekommt dafür aber wieder Zeit
                    TileEngine.Timelimit = TileEngine.TimelimitSave;

                    // ausfaden
                    Energy = 0.0f;
                }

            // Animphase setzen
            if (xAcc < 0.0f) {
                if (AnimCount > 20.0f)
                    AnimCount = 20.0f;
                AnimCount -= 1.0f SYNC;
                if (AnimCount < 0.0f)
                    AnimCount = 10.0f;
            } else {
                if (AnimCount < 10.0f)
                    AnimCount = 10.0f;
                AnimCount += 1.0f SYNC;
                if (AnimCount > 30.0f)
                    AnimCount = 20.0f;
            }

            AnimPhase = static_cast<int>(AnimCount);
        } break;

        // ausfaden
        case GEGNER::SPECIAL: {
            xAcc = 0.0f;
            yAcc = 0.0f;
            xSpeed = 0.0f;
            ySpeed = 0.0f;

            // ausfaden
            alpha -= 5.0f SYNC;

            if (alpha < 0.0f) {
                alpha = 0.0f;
                Energy = 0.0f;
            }

        } break;

        default:
            break;
    }  // switch

    // Boss oder SummaryScreen ? Dann verschwinden
    if (HUD.BossHUDActive != 0.0f || ShowSummary == true)
        Energy = 0.0f;

    if (Energy <= 0.0f && alpha > 0.0f && Handlung != GEGNER::SPECIAL)
        Vanish();
}

// --------------------------------------------------------------------------------------
// Punisher verschwindet
// --------------------------------------------------------------------------------------

void GegnerPunisher::Vanish() {
    if (ShowSummary == true)
        SoundManager.StopSong(MUSIC::PUNISHER, false);
    else
        SoundManager.FadeSong(MUSIC::PUNISHER, -2.0f, 0, false);

    Handlung = GEGNER::SPECIAL;
    Energy = 1.0f;
    Destroyable = false;
}

// --------------------------------------------------------------------------------------
// Punisher explodiert
// --------------------------------------------------------------------------------------

void GegnerPunisher::GegnerExplode() {
    TileEngine.Timelimit += 100;

    Player[0].PunisherActive = false;
    Player[1].PunisherActive = false;

    SoundManager.SetSongVolume(MUSIC::STAGEMUSIC, 0);
    SoundManager.FadeSong(MUSIC::STAGEMUSIC, 2.0f, 100, true);
}
