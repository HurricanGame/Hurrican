// --------------------------------------------------------------------------------------
// Der Piranha
//
// Schwimmt von einer Beckenseite zur anderen und flitzt auf den Spieler zu, wenn
// er in der Nähe ist
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Piranha.h"

// --------------------------------------------------------------------------------------
// Piranha Konstruktor

GegnerPiranha::GegnerPiranha(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_LAUFEN;
    HitSound		= 1;
    AnimStart		= 0;
    AnimEnde		= 10;
    AnimSpeed		= 0.75f;
    if (rand()%2 == 0)
        BlickRichtung	= LINKS;
    else
        BlickRichtung	= RECHTS;
    xSpeed			= -8.0;
    ySpeed			=  0.0f;
    xAcc			=  0.0f;
    yAcc			=  0.0f;
    Energy			= 30;
    Value1			= Wert1;
    Value2			= Wert2;

    // vom bigfish ausgespuckt
    if (Value1 == 98)
    {
        Value1 = 99;
        Value2 = 0;
        Handlung = GEGNER_SPECIAL;

        xSpeed = -20.0f - rand()%20;
        xsave  = xSpeed;
        ySpeed = (float)Wert2 * 3;
        ysave  = ySpeed;

        AnimCount = 1.0f;
    }
    else

        if (Value2 != 1)
            Value2			= rand()%20 + 2;
    ChangeLight		= Light;
    Destroyable		= true;
    OwnDraw			= true;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerPiranha::DoDraw(void)
{
    // Piranha rendern
    //
    bool mirror;

    if (BlickRichtung == LINKS)
        mirror = false;
    else
        mirror = true;

    // Spieler angreifen?
    int off = 0;
    if (Value1 == 99)
        off = 0;

    // oder nur rumschwimmen
    else
        off = 20;

    pGegnerGrafix[GegnerArt]->RenderSprite ((float)(xPos-TileEngine.XOffset),
                                            (float)(yPos-TileEngine.YOffset),
                                            off+AnimPhase, 0xFFFFFFFF, mirror);
}

// --------------------------------------------------------------------------------------
// Piranha Bewegungs KI

void GegnerPiranha::DoKI(void)
{
    if (Handlung != GEGNER_SPECIAL)
        SimpleAnimation();

    // Nach links bzw rechts auf Kollision prüfen und dann ggf umkehren
    //
    if ((xSpeed < 0.0f &&
            (blockl & BLOCKWERT_WAND ||
             blockl & BLOCKWERT_GEGNERWAND)) ||
            (xSpeed > 0.0f &&
             (blockr & BLOCKWERT_WAND ||
              blockr & BLOCKWERT_GEGNERWAND)))

    {
        xSpeed	      = 0;
        Handlung	  = GEGNER_DREHEN;
        AnimPhase	  = 10;
        AnimStart	  = 0;
        AnimEnde	  = 15;
    }

    // Spieler im Wasser und in Sichtweite ? Dann verfolgen
    if (pAim->InLiquid == true &&
            Value1 == 99 &&
            (PlayerAbstand() <= 150 ||
             Value2 == 1))
    {

        if (pAim->ypos+4+Value2 < yPos &&	// Spieler oberhalb oder
                !(blocko & BLOCKWERT_WAND)		&&
                !(blocko & BLOCKWERT_GEGNERWAND) &&
                blocko & BLOCKWERT_WASSER)
            yPos -= 4.0f SYNC;

        if (pAim->ypos+4+Value2 > yPos &&	// unterhalb des Piranhas
                !(TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]) & BLOCKWERT_WAND) &&
                !(TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]) & BLOCKWERT_GEGNERWAND))
            yPos += 4.0f SYNC;			// Dann auf ihn zu schwimmen
    }

    // Je nach Handlung richtig verhalten
    //
    switch (Handlung)
    {
    case GEGNER_SPECIAL:
    {
        AnimCount -= 0.1f SYNC;

        xSpeed = xsave * AnimCount;
        ySpeed = ysave * AnimCount;

        if (AnimCount < 0.0f)
        {
            xSpeed = 0.0;
            ySpeed = 0.0f;
            Value2 = 1;
            Value1 = 99;
            Handlung = GEGNER_LAUFEN;
        }

    }
    break;

    case GEGNER_LAUFEN:						// Piranha schwimmt rum
    {
        if (pAim->InLiquid == true &&	// Spieler im Wasser und
                Value1 == 99 &&
                (PlayerAbstand() <= 150 ||
                 Value2 == 1))			// in Sichtweite ?
        {
            if (BlickRichtung == LINKS)		// Dann schneller schwimmen
                xSpeed = -14.0f;
            else
                xSpeed =  14.0f;

            // Piranha links oder rechts am Spieler vorbei ?
            // Dann umdrehen und weiter verfolgen
            if (Handlung == GEGNER_LAUFEN)
            {
                if ((BlickRichtung == LINKS &&
                        pAim->xpos > xPos + GegnerRect[GegnerArt].right-20) ||

                        (BlickRichtung == RECHTS &&
                         pAim->xpos + pAim->CollideRect.right < xPos))
                {
                    xSpeed	      = 0;
                    Handlung	  = GEGNER_DREHEN;
                    AnimPhase	  = 10;
                    AnimStart	  = 0;
                    AnimEnde	  = 15;
                }
            }
        }
        else
        {
            if (BlickRichtung == LINKS)
                xSpeed = -float (rand()%10 + 4);
            else
                xSpeed =  float (rand()%10 + 4);
        }
    }
    break;

    case GEGNER_DREHEN:						// Piranha dreht sich um
    {
        if (AnimPhase == AnimStart)
        {
            Handlung  = GEGNER_LAUFEN;
            AnimEnde  = 10;
            AnimStart = 0;
            AnimPhase = 0;

            if (BlickRichtung == LINKS)		// Ab jetzt in die andere Richtung schwimmen
            {
                BlickRichtung = RECHTS;
                xSpeed = 8.0f;
            }
            else
            {
                BlickRichtung = LINKS;
                xSpeed = -8.0f;
            }
        }
    }
    break;

    default :
        break;
    } // switch

    // Testen, ob der Spieler den Piranha berührt hat
    TestDamagePlayers(1.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Piranha explodiert

void GegnerPiranha::GegnerExplode(void)
{
    // Fetzen und Blasen erzeugen
    int i;
    for (i=0; i<3; i++)
        PartikelSystem.PushPartikel(float(xPos - 20 + rand()%45),
                                      float(yPos - 5  + rand()%30), PIRANHATEILE);

    for (i=0; i<3; i++)
        PartikelSystem.PushPartikel(float(xPos - 10  + rand()%45),
                                      float(yPos + 10  + rand()%30), BUBBLE);

    // Blutwolke dazu
    PartikelSystem.PushPartikel(float(xPos + 2),
                                  float(yPos - 5), PIRANHABLUT);

    SoundManager.PlayWave(100, 128, -rand()%2000+11025, SOUND_EXPLOSION1);	// Sound ausgeben

    Player[0].Score += 200;
}
