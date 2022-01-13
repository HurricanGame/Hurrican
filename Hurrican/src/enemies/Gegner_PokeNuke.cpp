// --------------------------------------------------------------------------------------
// Die riesen Poke Nuke
//
// kracht am Fahrstuhl auseinander und fetzt den Hurri wenn er nicht ganz am Rand steht
// --------------------------------------------------------------------------------------

#include "Gegner_PokeNuke.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPokeNuke::GegnerPokeNuke(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    Energy = 10;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    ySpeed = 27.0f;
    SmokeDelay = 0.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerPokeNuke::DoKI() {
    BlickRichtung = DirectionEnum::LINKS;

    switch (Handlung) {
        case GEGNER::STEHEN: {
            Handlung = GEGNER::FALLEN;
            yPos -= RENDERHEIGHT + 217.0f;
        } break;

        // Stein fällt runter
        case GEGNER::FALLEN: {
            if (yPos > g_Fahrstuhl_yPos - 217.0f)
                Energy = 0.0f;

            SmokeDelay -= Timer.sync(1.0f);

            if (SmokeDelay <= 0.0f) {
                SmokeDelay = 0.2f;
                PartikelSystem.PushPartikel(xPos + 24.0f - static_cast<float>(random(26)), yPos - 16.0f, SMOKEBIG);
            }
        } break;

        default:
            break;
    }  // switch

    // Testen, ob der Spieler den Stein berührt hat
    TestDamagePlayers(100.0f, true);
}

// --------------------------------------------------------------------------------------
// PokeNuke explodiert
// --------------------------------------------------------------------------------------

void GegnerPokeNuke::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION2);
    ShakeScreen(7);

    pAim = &Player[0];
    if (PlayerAbstand() < 160)
        pAim->DamagePlayer(50);

    if (NUMPLAYERS == 2) {
        pAim = &Player[1];
        if (PlayerAbstand() < 160)
            pAim->DamagePlayer(50);
    }

    for (int i = 0; i < 16; i++)
        PartikelSystem.PushPartikel(xPos - 60.0f + static_cast<float>(random(90)),
                                    yPos - 40.0f + static_cast<float>(random(200)), SMOKEBIG);
    for (int i = 0; i < 8; i++)
        PartikelSystem.PushPartikel(xPos - 50.0f + static_cast<float>(random(90)),
                                    yPos - 30.0f + static_cast<float>(random(200)), EXPLOSION_BIG);
    for (int i = 0; i < 12; i++)
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(90)),
                                    yPos - 20.0f + static_cast<float>(random(200)), EXPLOSION_MEDIUM2);
    for (int i = 0; i < 12; i++)
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(90)),
                                    yPos + 100.0f + static_cast<float>(random(100)), SPLITTER);
    for (int i = 0; i < 200; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(90)),
                                    yPos + 50.0f + static_cast<float>(random(150)), FUNKE);

    for (int i = 0; i < 48; i++)
        PartikelSystem.PushPartikel(xPos - 67.0f + static_cast<float>(random(130)),
                                    yPos - 30.0f + static_cast<float>(random(200)), EXPLOSIONFLARE);
}
