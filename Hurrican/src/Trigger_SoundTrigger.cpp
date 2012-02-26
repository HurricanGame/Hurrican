// --------------------------------------------------------------------------------------
// Der Sound Trigger
//
// Lädt Sound mit Namen "Value1.wav" (bei Value1 == 1 z.B. also "1.wav")
// Value2 : 0 = Sound ständig loopen und Lautstärke vom Spieler abhängig regulieren
//			1 = Bei SpielerAbstand < 100 Sound einmalig abspielen
//			2 = Bei SpielerAbstand < 100 Sound ständig abspielen
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_SoundTrigger.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSoundTrigger::GegnerSoundTrigger(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= false;
	Energy			= 100;
	DontMove		= true;
	Active			= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerSoundTrigger::DoKI(void)
{
	switch (Value2)
	{
		// Sound loopen, Lautstärke anhängig von SpielerAbstand
		//
		case 0 :
		{
			// Sound auf die richtige Lautstärke und den Pitch setzen
			//
			if (PlayerAbstand() < 700)
			{
				int   vol, pan;
				float xdiff, ydiff, Abstand;

				xdiff = ((pPlayer[0]->xpos + 45)  - xPos);
				ydiff = ((pPlayer[0]->ypos + 45)  - yPos);

				Abstand = float(sqrt((xdiff * xdiff) + (ydiff * ydiff)));

				vol = int(100-float(Abstand/6.0f)) * 2;
				vol = int (vol * pSoundManager->its_GlobalSoundVolume /100.0f);
				if (vol < 0)
					vol = 0;
				else
				{
					// Sound links oder rechts vom Spieler ?
					//
					if (xPos < pPlayer[0]->xpos + 45)
					{
						pan = 128 - (100 - vol);
						if (pan < 0)
							pan = 0;
					}
					else
					{
						pan = 128 + (100 - vol);
						if (pan > 255)
							pan = 255;
					}

					// Sound abspielen, wenn er noch nicht läuft bzw im Menu gestoppt wurde
					//
						if (pSoundManager->its_Sounds [SOUND_TRIGGER_START + Value1]->isPlaying == false)
							pSoundManager->PlayWave(100, 128, 11025, SOUND_TRIGGER_START + Value1);

					int channel = pSoundManager->its_Sounds [SOUND_TRIGGER_START + Value1]->Channel;
					FSOUND_SetVolume (channel, vol);
					FSOUND_SetPan	 (channel, 128);
				}
			}

		} break;

		// Sound einmalig abspielen bei bestimmtem Abstand
		//
		case 1 :
		{
			if (PlayerAbstand() < 100)
			{
				pSoundManager->PlayWave (100, 128, 11025, SOUND_TRIGGER_START + Value1);
				Energy = 0.0f;
			}
		} break;

		// Sound immer abspielen bei bestimmtem Abstand
		//
		case 2 :
		{
			if (PlayerAbstand() < 100)
			{
				pSoundManager->PlayWave (100, 128, 11025, SOUND_TRIGGER_START + Value1);
				Energy = 0.0f;
			}
		} break;
	}
}

// --------------------------------------------------------------------------------------
// SoundTrigger explodiert (nicht *g*)
// --------------------------------------------------------------------------------------

void GegnerSoundTrigger::GegnerExplode(void)
{
}
