// --------------------------------------------------------------------------------------
// Der Fahrstuhl
//
// Fährt nach unten, sobald der Spieler drauftritt
// Value1 == 0 : Fahrstuhl für das Fahrstuhl Level, sprich, wechselt das Tempo
// Value1 >  0 : Fahrtsuhl fährt ganz normal nach unten (Tempelausgang)
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_Fahrstuhl.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFahrstuhl::GegnerFahrstuhl(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_STEHEN;
    BlickRichtung	= LINKS;
    Energy			= 100;
    Value1			= Wert1;
    Value2			= Wert2;
    AnimPhase		= 0;
    ChangeLight		= Light;
    Destroyable		= false;
    g_Fahrstuhl_Offset = 70.0f;
    new_ySpeed		= 0.0f;
    TestBlock		= false;
    OwnDraw			= true;

    if (Value1 == 0)
    {
        TileEngine.ParallaxLayer[2].LoadImage("fahrstuhl_parallax.png", 400, 480, 400, 480, 1, 1);
        TileEngine.IsElevatorLevel = true;
    }
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerFahrstuhl::DoDraw(void)
{
    DirectGraphics.SetFilterMode (true);

    // Schatten rendern
    float l = (float) (g_Fahrstuhl_yPos - TileEngine.YOffset) - 80;
    pGegnerGrafix[GegnerArt]->RenderSpriteScaled((float)(xPos-TileEngine.XOffset - 10),
            (float)(g_Fahrstuhl_yPos-TileEngine.YOffset),
            GegnerRect[GegnerArt].right  + 20,
            int (GegnerRect[GegnerArt].bottom + l / 5.0f),
            AnimPhase, 0x99000000);

    DirectGraphics.SetFilterMode (false);

    // Fahrstuhl rendern
    pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-TileEngine.XOffset),
                                           (float)(g_Fahrstuhl_yPos-TileEngine.YOffset),
                                           AnimPhase, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFahrstuhl::DoKI(void)
{
    if (Value1 == 0)
        TileEngine.XOffset = xPos + 200 - 320.0f;

    // Feststellen ob der Hurri auf dem Fahrstuhl steht
    PlattformTest(GegnerRect[GegnerArt]);

    for (int p = 0; p < NUMPLAYERS; p++)
        if (Player[p].AufPlattform == this)
        {
            if (Handlung != GEGNER_FALLEN)
            {
                if (Value1 == 0)
                {
                    TileEngine.Zustand = ZUSTAND_LOCKED;
                }
                else if (TileEngine.Zustand != ZUSTAND_SCROLLTOPLAYER)
                    TileEngine.ScrollLevel (xPos - 50, yPos - 400, ZUSTAND_SCROLLTOLOCK);
            }

            Handlung = GEGNER_FALLEN;				// Fahrstuhl fährt los
        }

// Je nach Handlung richtig verhalten
    switch (Handlung)
    {

    // Warten auf den Hurri =)
    case GEGNER_STEHEN:
    {
    } break;

    // Fahrstuhl fährt nach unten
    case GEGNER_FALLEN:
    {
        if (Value1 == 0)
            TileEngine.NewYOffset = yPos-320.0f-g_Fahrstuhl_Offset;

        // Spieler "auf" den Fahrstuhl setzen
        if (Value1 != 0)
        {
            for (int p = 0; p < NUMPLAYERS; p++)
            {
                Player[p].JumpPossible = false;
                Player[p].ypos = yPos - Player[p].CollideRect.bottom;
            }

            if (TileEngine.Zustand == ZUSTAND_LOCKED)
                new_ySpeed =  8.0f;
        }
        else
            new_ySpeed = Player[0].AutoScrollspeed;

        // Speed angleichen
        if (ySpeed < new_ySpeed) ySpeed += 0.3f SYNC;
        if (ySpeed > new_ySpeed) ySpeed -= 0.3f SYNC;

        // Funken sprühen
        static float sparkcount = 0.0f;

        sparkcount -= 1.0f SYNC;

        if (sparkcount < 0.0f)
        {
            sparkcount = 0.05f;

            pPartikelSystem->PushPartikel(xPos + rand()%5, yPos + rand()%5, FUNKE);
            pPartikelSystem->PushPartikel(xPos + rand()%5+GegnerRect[GegnerArt].right, yPos + rand()%5, FUNKE);

            if (rand()%20 == 0) pPartikelSystem->PushPartikel(xPos - 20,	yPos - 10, SMOKEBIG);
            if (rand()%20 == 0) pPartikelSystem->PushPartikel(xPos - 20 +GegnerRect[GegnerArt].right, yPos - 10, SMOKEBIG);
        }

    }
    break;

    default :
        break;
    } // switch

    // Endposition erreicht ? Dann explodieren (nur im Fahrstuhllevel)
    //
    if (Value1 == 0 &&
            yPos >= Value2)
        Energy = 0.0f;

    g_Fahrstuhl_yPos  = yPos;
    g_Fahrstuhl_Speed = ySpeed;
}

// --------------------------------------------------------------------------------------
// Fahrstuhl explodiert
// --------------------------------------------------------------------------------------

void GegnerFahrstuhl::GegnerExplode(void)
{
    FahrstuhlPos = -1.0f;

    ShakeScreen (10);
    SoundManager.PlayWave (100, 128, 11025, SOUND_EXPLOSION2);

    for (int j=0; j < 100; j++)
        pPartikelSystem->PushPartikel(xPos + rand()%400, yPos + rand()%100, SPIDERSPLITTER);

    for (int j=0; j < 50; j++)
    {
        pPartikelSystem->PushPartikel(xPos + rand()%400, yPos + rand()%100, ROCKETSMOKE);
        pPartikelSystem->PushPartikel(xPos - 30 + rand()%400, yPos - 30 + rand()%100, EXPLOSION_MEDIUM2);
    }

    for (int j=0; j < 20; j++)
        pPartikelSystem->PushPartikel(xPos + rand()%400, yPos + rand()%100, SPLITTER);

    TileEngine.Zustand = ZUSTAND_SCROLLBAR;

    // Spieler springt ab
    //
    for (int p = 0; p < NUMPLAYERS; p++)
    {
        Player[p].JumpPossible = false;
        Player[p].AnimPhase = 0;
        Player[p].Handlung  = SPRINGEN;
        Player[p].JumpStart = Player[p].ypos;
        Player[p].yspeed    = -PLAYER_MAXJUMPSPEED;
        Player[p].JumpAdd	  =  0.0f;
        Player[p].AufPlattform = NULL;
    }
}
