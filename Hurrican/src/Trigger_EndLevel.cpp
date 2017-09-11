// --------------------------------------------------------------------------------------
// Der End Level Trigger
//
// Berührt ihn der Spieler so kommt er in
// die Stage, die bei Value1 angegeben ist
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_EndLevel.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerEndLevel::GegnerEndLevel(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_STEHEN;
    Value1			= Wert1;
    Value2			= Wert2;
    ChangeLight		= Light;
    Destroyable		= false;
    Energy			= 100;
    DontMove		= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerEndLevel::DoKI(void)
{
    // Ist der Trigger im Bild ?
    if (Energy > 0.0f &&
            (NewStage != Value1 ||
             RunningTutorial) &&
            xPos + 120 > TileEngine.XOffset	    &&
            xPos	   < TileEngine.XOffset + 640 &&
            yPos + 120 > TileEngine.YOffset	    &&
            yPos	   < TileEngine.YOffset + 480)
    {
        // Ausgang berührt ? Dann Nächsten Level laden
        for (int p = 0; p < NUMPLAYERS; p++)
            if (SpriteCollision(xPos,		   yPos,		  GegnerRect[GegnerArt],
                                Player[p].xpos,
                                Player[p].ypos,
                                Player[p].CollideRect) == true)

            {
                // Spieler zentrieren
                if (NUMPLAYERS == 0)
                    TileEngine.ScrollLevel(Player[0].xpos - 300,
                                             Player[0].ypos - 280, ZUSTAND_SCROLLTOLOCK);
                else
                    TileEngine.ScrollLevel(TileEngine.XOffset,
                                             TileEngine.YOffset, ZUSTAND_SCROLLTOLOCK);

                Energy = 0.0f;

                for (int p2 = 0; p2 < NUMPLAYERS; p2++)
                {
                    Player[p2].FesteAktion   = Value2;		// Art wie der Spieler den Screen verlässt
                    Player[p2].DoFesteAktion = true;
                }

                NewStage = Value1;

                if (RunningTutorial && Value1 <= 9)
                {
                    SoundManager.PlayWave(100, 128, 11025, SOUND_MARIO);
                    StageClear(false);
                }
                else
                    StageClear(true);
            }
    }
}

// --------------------------------------------------------------------------------------
// EndLevel explodiert (nicht *g*)
// --------------------------------------------------------------------------------------

void GegnerEndLevel::GegnerExplode(void)
{
}
