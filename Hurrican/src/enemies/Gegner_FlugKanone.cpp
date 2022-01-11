// --------------------------------------------------------------------------------------
// Die FlugKanone
//
// Umkreist den Spieler und Ballert auf ihn runter
// --------------------------------------------------------------------------------------

#include "Gegner_FlugKanone.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFlugKanone::GegnerFlugKanone(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 30;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    xSpeed = 0.0f;
    ySpeed = 0.0f;
    yAcc = 0.0f;
    AnimSpeed = 1.0f;
    AnimEnde = 19;
    ShotDelay = 0.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFlugKanone::DoKI() {
    // Animieren
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

    // in Richtung Spieler fliegen
    if (xPos + 30 < pAim->xpos + 35)
        xAcc = 2.5f;
    if (xPos + 30 > pAim->xpos + 35)
        xAcc = -2.5f;

    if (yPos < pAim->ypos - 100)
        yAcc = 2.5f;
    if (yPos > pAim->ypos - 100)
        yAcc = -2.5f;

    xSpeed = std::clamp(xSpeed, -20.0f, 20.0f);
    ySpeed = std::clamp(ySpeed, -10.0f, 10.0f);

    if (ShotDelay > 0.0f)
        ShotDelay -= Timer.sync(1.0f);
    if (ShotDelay <= 0.0f) {
        if (Skill == SKILL_EASY)
            ShotDelay = 10.0f;
        if (Skill == SKILL_MEDIUM)
            ShotDelay = 7.5f;
        if (Skill >= SKILL_HARD)
            ShotDelay = 5.0f;

        if (abs(static_cast<int>(pAim->xpos - xPos)) < 100 && PlayerAbstand() < 300) {
            Projectiles.PushProjectile(xPos + 30.0f, yPos + 40.0f, FLUGLASER);
            PartikelSystem.PushPartikel(xPos + 24.0f, yPos + 30.0f, SMOKE);
            SoundManager.PlayWave(50, 128, 30000, SOUND::LASERSHOT);
        }
    }

    if ((blocko & BLOCKWERT_WAND || blocko & BLOCKWERT_GEGNERWAND) && ySpeed < 0.0f)
        ySpeed *= 1.0f;
    if ((blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_GEGNERWAND) && ySpeed > 0.0f)
        ySpeed *= 1.0f;
    if ((blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND) && xSpeed < 0.0f)
        xSpeed *= 1.0f;
    if ((blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND) && xSpeed > 0.0f)
        xSpeed *= 1.0f;
#if 0
    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::LAUFEN:  // Durch die Gegend rasen
        {
        } break;

        default:
            break;
    }  // switch
#endif
    // Testen, ob der Spieler die FlugKanone berührt hat
    TestDamagePlayers(Timer.sync(1.0f));
}

// --------------------------------------------------------------------------------------
// FlugKanone explodiert
// --------------------------------------------------------------------------------------

void GegnerFlugKanone::GegnerExplode() {
    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(xPos - 20.0f + static_cast<float>(random(45)),
                                    yPos - 20.0f + static_cast<float>(random(45)), EXPLOSION_MEDIUM2);
    for (int i = 0; i < 20; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(random(65)),
                                    yPos + static_cast<float>(random(35)), LASERFUNKE2);

    SoundManager.PlayWave(100, 128, -random(2000) + 11025, SOUND::EXPLOSION1);  // Sound ausgeben

    Player[0].Score += 150;
}
