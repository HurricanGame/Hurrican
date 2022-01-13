// --------------------------------------------------------------------------------------
// Der Manfred Trenz Shrine
//
// kniet man davor nieder, regnet es Extras
// schiesst man darauf, regent es Steine
// --------------------------------------------------------------------------------------

#include "Trigger_Shrine.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerShrine::GegnerShrine(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    BlickRichtung = DirectionEnum::RECHTS;
    Energy = 100;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    Value1 = 1;
    AnimCount = 0.0f;

    TileEngine.MaxOneUps += 2;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerShrine::DoKI() {
    BlickRichtung = DirectionEnum::LINKS;

    if (AnimCount > 0.0f)
        AnimCount -= Timer.sync(1.0f);
    else
        AnimCount = 0.0f;

    /*
        // Drauf geschossen ? Dann Steine runterfallen lassen
        //
        if (Energy < 100.0f)
        {
            Energy = 100.0f;

            if (AnimCount == 0.0f)
            {
                Gegner.PushGegner (Player[0].xpos,
                                     Player[0].ypos - 50,
                                     FALLINGROCK, 0, 0, true);

                if (NUMPLAYERS == 2)
                Gegner.PushGegner (Player[1].xpos,
                                     Player[1].ypos - 50,
                                     FALLINGROCK, 0, 0, true);

                SoundManager.PlayWave (100, 128, 11025, SOUND::PHARAODIE);

                AnimCount = 5.0f;
            }
        }
    */
    // Spieler kniet nieder
    //
    for (int p = 0; p < NUMPLAYERS; p++)
        if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], Player[p].xpos, Player[p].ypos, Player[p].CollideRect) ==
                true &&
            Player[p].Handlung == PlayerActionEnum::DUCKEN && Value1 == 1) {
            Value1 = 0;
            for (int i = 0; i < 2; i++)
                Gegner.PushGegner(xPos - 20.0f + static_cast<float>(i * 110),
                                  yPos - 100.0f, ONEUP, 0, 0, true);

            for (int i = 0; i < 8; i++)
                Gegner.PushGegner(xPos + static_cast<float>(random(100)),
                                  yPos - 50.0f + static_cast<float>(random(100)), EXTRAS, 3, 0, true);

            SoundManager.PlayWave(100, 128, 11025, SOUND::COLLECT);
            SoundManager.PlayWave(100, 128, 11025, SOUND::COLLECT);
            SoundManager.PlayWave(100, 128, 11025, SOUND::COLLECT);
            SoundManager.PlayWave(100, 128, 11025, SOUND::COLLECT);
            SoundManager.PlayWave(100, 128, 11025, SOUND::COLLECT);
            SoundManager.PlayWave(100, 128, 11025, SOUND::COLLECT);
        }
}

// --------------------------------------------------------------------------------------
// Shrine explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerShrine::GegnerExplode() {}
