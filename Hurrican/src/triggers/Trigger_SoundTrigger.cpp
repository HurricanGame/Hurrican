// --------------------------------------------------------------------------------------
// Der Sound Trigger
//
// Lädt Sound mit Namen "Value1.wav" (bei Value1 == 1 z.B. also "1.wav")
// Value2 : 0 = Sound ständig loopen und Lautstärke vom Spieler abhängig regulieren
//			1 = Bei SpielerAbstand < 100 Sound einmalig abspielen
//			2 = Bei SpielerAbstand < 100 Sound ständig abspielen
// --------------------------------------------------------------------------------------

#include "Trigger_SoundTrigger.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSoundTrigger::GegnerSoundTrigger(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_STEHEN;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    Energy = 100;
    DontMove = true;
    Active = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSoundTrigger::DoKI() {
    switch (Value2) {
        // Sound loopen, Lautstärke anhängig von SpielerAbstand
        //
        case 0: {
            // Sound auf die richtige Lautstärke und den Pitch setzen
            //
            if (PlayerAbstand() < 700) {
                int vol = 0, pan = 128;
                float xdiff, ydiff, Abstand;

                xdiff = ((Player[0].xpos + 45) - xPos);
                ydiff = ((Player[0].ypos + 45) - yPos);

                // DKS - Converted to float:
                Abstand = sqrtf((xdiff * xdiff) + (ydiff * ydiff));

                // DKS - Sound is twice as loud here as it should be, and volume is
                //      adjusted against the global sound volume in the sound manager itself:
                // vol = static_cast<int>(100-static_cast<float>(Abstand/6.0f)) * 2;
                // vol = int (vol * SoundManager.g_sound_vol /100.0f);
                vol = static_cast<int>(100.0f - Abstand / 6.0f);
                if (vol < 0) {
                    vol = 0;
                } else {
                    // Sound links oder rechts vom Spieler ?
                    //
                    if (xPos < Player[0].xpos + 45) {
                        pan = 128 - (100 - vol);
                        if (pan < 0)
                            pan = 0;
                    } else {
                        pan = 128 + (100 - vol);
                        if (pan > 255)
                            pan = 255;
                    }
                }

                // Sound abspielen, wenn er noch nicht läuft bzw im Menu gestoppt wurde
                //
                // DKS - Added function GetChannelWaveIsPlayingOn() to SoundManagerClass:
                int channel = SoundManager.GetChannelWaveIsPlayingOn(SOUND_TRIGGER_START + Value1);

                if (channel == -1 && vol > 0)
                    channel = SoundManager.PlayWave(vol, pan, 11025, SOUND_TRIGGER_START + Value1);

                // DKS - There are often multiple sound triggers close to one another and they
                //      often interfere with one another. I added a function
                //      SetPendingChannelVolumeAndPanning() that will accept these multiple
                //      requests and create a single result from all of them.
                // int channel = SoundManager.its_Sounds [SOUND_TRIGGER_START + Value1]->Channel;
                // SOUND_SetVolume (channel, vol);
                // SOUND_SetPan	 (channel, 128);
                if (channel >= 0)
                    SoundManager.SetPendingChannelVolumeAndPanning(channel, vol, pan);
            }

        } break;

        // Sound einmalig abspielen bei bestimmtem Abstand
        //
        case 1: {
            if (PlayerAbstand() < 100) {
                // DKS - added check for WaveIsPlaying:
                if (!SoundManager.WaveIsPlaying(SOUND_TRIGGER_START + Value1))
                    SoundManager.PlayWave(100, 128, 11025, SOUND_TRIGGER_START + Value1);
                Energy = 0.0f;
            }
        } break;

        // Sound immer abspielen bei bestimmtem Abstand
        //
        case 2: {
            if (PlayerAbstand() < 100) {
                // DKS - added check for WaveIsPlaying:
                if (!SoundManager.WaveIsPlaying(SOUND_TRIGGER_START + Value1))
                    SoundManager.PlayWave(100, 128, 11025, SOUND_TRIGGER_START + Value1);
                Energy = 0.0f;
            }
        } break;
    }
}

// --------------------------------------------------------------------------------------
// SoundTrigger explodiert (nicht *g*)
// --------------------------------------------------------------------------------------

void GegnerSoundTrigger::GegnerExplode() {}
