// --------------------------------------------------------------------------------------
// Der Fade Music Trigger
//
// Fadet die Level-Musik ein oder aus
// Value1 : 0 = ausfaden, 1 = einfaden
// Value2 : Abstand zum Spieler, ab welchem der Trigger aktiviert wird
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_FadeMusic.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFadeMusic::GegnerFadeMusic(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_STEHEN;
    Value1			= Wert1;
    Value2			= SOUND_TRIGGER_START + Wert2;
    ChangeLight		= Light;
    Destroyable		= false;
    Energy			= 100;
    DontMove		= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFadeMusic::DoKI(void)
{
    // Ist der Trigger nahe genug am Spieler, dass er aktiviert wird ?
    if (PlayerAbstand() <= 150)
    {
        // Musik Ausfaden
        if (Value1 == 0)
        {
            // Spielt die Levelmusik überhaupt oder fadet nicht (oder ein) ?
            //DKS - No need to check for this, overhauled sound manager:
            //if (MUSIC_IsPlaying(SoundManager.its_Songs[MUSIC_STAGEMUSIC]->SongData))// &&
            //SoundManager.its_Songs[MUSIC_STAGEMUSIC]->FadingVolume >= 0.0f)
            SoundManager.FadeSong(MUSIC_STAGEMUSIC, -1.0f, 0, true);
        }

        // Musik Einfaden
        if (Value1 == 1)
        {
            // Ist die LevelMusik aus oder fadet nicht schon oder gerade aus ?
            //DKS - No need to check for this, overhauled sound manager:
            //if (FMUSIC_GetPaused(SoundManager.its_Songs[MUSIC_STAGEMUSIC]->SongData)) //&&
            //SoundManager.its_Songs[MUSIC_STAGEMUSIC]->FadingVolume <= 0.0f)
            SoundManager.FadeSong(MUSIC_STAGEMUSIC,  1.0f, 100, true);
        }

        // SoundTrigger Ausfaden
        if (Value1 == 2)
        {
            // Ist der Sound an und fadet noch nicht ?
            //if (SoundManager.its_Sounds[Value2]->FadeMode != FADEMODE_OUT)
            SoundManager.FadeWave(Value2, FADEMODE_OUT);
        }

        // SoundTrigger Einfaden
        if (Value1 == 3)
        {
            // Ist der Sound aus und fadet nicht?
            //if (SoundManager.its_Sounds[Value2]->FadeMode != FADEMODE_IN)
            SoundManager.FadeWave(Value2, FADEMODE_IN);
        }
    }

    //DKS - Disabled, as RenderCircle is only used this once here and what is the point?
//#ifdef _DEBUG
//    if (DebugMode)
//        RenderCircle((float)(xPos + 20 - TileEngine.XOffset),
//                     (float)(yPos + 20 - TileEngine.YOffset), 150, 0xFFFFFFFF);
//#endif //_DEBUG
}

// --------------------------------------------------------------------------------------
// FadeMusic explodiert (nicht *g*)
// --------------------------------------------------------------------------------------

void GegnerFadeMusic::GegnerExplode(void)
{
}
