// --------------------------------------------------------------------------------------
// Der Bonus Block
//
// Schiesst man auf ihn, und seine Energie sinkt auf 0, so springt ein Extra heraus,
// Value 1 wird verringert und die Energie neu gesetzt, bis Value1 == 0 ist
// --------------------------------------------------------------------------------------

#include "Gegner_PowerBlock.hpp"
#include "stdafx.hpp"

//					    	Spread Laser Bounce	Blitz Shield Energy	Rad	Powerline Granate Smartbomb Autofire Supershot
//Score
int WaffenVerteilung[14] = {100, 200, 300, 500, 505, 530, 550, 600, 650, 675, 680, 685, 1000, 1000};

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPowerBlock::GegnerPowerBlock(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_STEHEN;
    AnimPhase = 0;
    Energy = 100.0f;

    for (int i = 0; i < 16; i++)
        AlreadySpawned[i] = false;
#if 0
    // Itemanzahl standard mässig auf 16 setzen, wenn keine Items drin sind (weil Turri das immer vergisst im Editor)
    if (Wert1 == 0)
        Value1 = 10;
    else
        Value1 = Wert1;

    // Je mach Skill weniger Extras
    //
    if (Skill == SKILL_EASY)
        Value1 = static_cast<int>(Value1 * 1.0f);
    if (Skill == SKILL_MEDIUM)
        Value1 = static_cast<int>(Value1 * 0.7f);
    if (Skill == SKILL_HARD)
        Value1 = static_cast<int>(Value1 * 0.5f);
    if (Skill == SKILL_HURRICAN)
        Value1 = static_cast<int>(Value1 * 0.3f);
#else
    // NOTE why has this been hardcoded?
    Value1 = 4;
#endif
    // Typ des Extras merken
    // Ist der Typ == 0, werden Extras per Zufall gespawnt
    ExtraType = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    OwnDraw = true;
    WeaponSpawned = NUMPLAYERS;  // Maximal "Anzahl Spieler" Waffen Powerup spawnen
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerPowerBlock::DoDraw() {
    DirectGraphics.SetColorKeyMode();

    // normal
    if (AnimPhase > 0) {
        pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                               yPos - TileEngine.YOffset,
                                               1 + TileEngine.DateiAppendix.UsedPowerblock * 2, 0xFFFFFFFF, false);
    }

    // nochmal leuchtend drüber (ausfadend)
    if (AnimPhase == 2) {
        D3DCOLOR col = D3DCOLOR_RGBA(255, 255, 255, static_cast<int>(DamageTaken));
        pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                               yPos - TileEngine.YOffset,
                                               AnimPhase + TileEngine.DateiAppendix.UsedPowerblock * 2, col, false);
    }
}

// --------------------------------------------------------------------------------------
// Bewegungs "KI"
// --------------------------------------------------------------------------------------

void GegnerPowerBlock::DoKI() {
    if (AnimPhase == 0)
        Value2 = static_cast<int>(yPos);  // yPos sichern

    if (AnimPhase > 0)
        PlattformTest(GegnerRect[GegnerArt]);

    // Bei Treffer erscheinen lassen
    if (DamageTaken != 0.0f && AnimPhase == 0) {
        AnimPhase = 1;
        Player[0].BlocksFullGame++;
        Player[0].BlocksThisLevel++;
        Energy = 12.0f;
    }

    // Bei Treffer andere AnimPhase setzen
    //
    if (AnimPhase != 0) {
        if (AnimPhase == 2 && DamageTaken == 0.0f)
            AnimPhase = 1;
    }

    // Spieler ist dagegengesprungen
    //
    for (int i = 0; i < NUMPLAYERS; i++)
        if (AnimPhase == 1 && Player[i].yspeed < 0.0f &&
            Player[i].ypos + Player[i].CollideRect.top <= yPos + GegnerRect[GegnerArt].bottom &&
            Player[i].ypos + Player[i].CollideRect.top >= yPos &&
            Player[i].xpos + Player[i].CollideRect.left <= xPos + GegnerRect[GegnerArt].right &&
            Player[i].xpos + Player[i].CollideRect.right >= xPos) {
            Energy = 0;  // Extra verlieren
            AnimPhase = 2;
            Handlung = GEGNER_SPRINGEN;  // Block spingen lassen

            yAcc = 4.0f;

            // Sprung-Geschwindigkeit setzen
            if (ySpeed == 0.0f)
                ySpeed = -8.0f;
            else if (ySpeed > 0.0f)
                ySpeed *= -1.0f;

            Player[i].yspeed = -Player[i].yspeed * 2 / 3;  // Spieler prallt ab
        }

    // Block nach Springen wieder unten ?
    if (ySpeed != 0.0f) {
        if (yPos > Value2)  // Alte Position erreicht ?
        {
            yPos = static_cast<float>(Value2);
            ySpeed = 0.0f;
            yAcc = 0.0f;
            Handlung = GEGNER_STEHEN;
        }
    }

    if (Energy <= 0.0f)  // Ein Extra verlieren ?
    {
        AnimPhase = 2;
        Player[0].Score += 10;  // Punkte geben

        int extra;

        // Spezielle Items ausspucken?
        //
        if (ExtraType != 0)
            extra = ExtraType;

        // Ansonsten per Zufall
        //
        else {
            // erstes Powerup immer Energie
            if (Value1 == 4)
                extra = 5;
            else {
                extra = 5;

                while (AlreadySpawned[extra]) {
                    // Per Zufallsverteilung entsprechendes Extra setzen
                    int r;
                    if (WeaponSpawned > 0)
                        r = random(675);
                    else
                        r = random(175) + 500;

                    for (int i = 0; i < 12; i++)
                        if (r < WaffenVerteilung[i]) {
                            if (i <= 2)
                                WeaponSpawned--;

                            extra = i;
                            break;
                        }
                }
            }

            AlreadySpawned[extra] = true;
        }

        Gegner.PushGegner(xPos + 10, yPos, EXTRAS, extra, 0, true);

        Value1--;            // Extra-Speicher eins verringern
        if (Value1 > 0)      // und wenn noch eins drin ist,
            Energy = 12.0f;  // dann die Energy wieder hochsetzen
        else
            Energy = 0.0f;  // ansonsten kaputt machen =)
    }
}

// --------------------------------------------------------------------------------------
// explodiert
// --------------------------------------------------------------------------------------

void GegnerPowerBlock::GegnerExplode() {
    for (int i = 0; i < NUMPLAYERS; i++)
        if (Player[i].AufPlattform == this) {
            Player[i].AufPlattform = nullptr;
            Player[i].JumpPossible = false;
            Player[i].Handlung = PlayerActionEnum::SPRINGEN;
            Player[i].AnimPhase = 0;
            Player[i].yspeed = 0.5f;
            Player[i].JumpAdd = PLAYER_JUMPADDSPEED;
        }

    // Punkte geben
    Player[0].Score += 100;

    // Explosion erzeugen
    PartikelSystem.PushPartikel(xPos - 30.0f, yPos - 30.0f, EXPLOSION_BIG);

    SoundManager.PlayWave(100, 128, -random(2000) + 11025, SOUND_EXPLOSION1);  // Sound ausgeben
}
