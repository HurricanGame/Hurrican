// --------------------------------------------------------------------------------------
// Der Tutorial Text
//
// Befindet sich der Spieler nahe genug an dem Trigger, so wird
// der TutorialText mit der ID "Value1" eingefadet. Ist der Spieler wieder weit genug
// entfernt, wird dieser wieder ausgeblendet.
// --------------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "stdafx.h"
using namespace std;

#include "Gegner_Powerblock.h"
#include "Trigger_TutorialText.h"
#include "GetKeyName.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerTutorialText::GegnerTutorialText(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	BlickRichtung	= LINKS;
	Energy			= 100;
	Value1			= Wert1;
	Value2			= Wert2;	
	AnimPhase		= 0;
	ChangeLight		= Light;
	Destroyable		= false;
	Active		    = true;
	DontMove		= true;	
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerTutorialText::DoKI(void)
{
	// Spieler nahe genug? Dann einfaden
	if (PlayerAbstand() < 120 &&
		Value1 != 9 &&
		(pGUI->m_FadeMode == INVISIBLE ||
		 pGUI->m_TextID != Value1) &&
		 !(Value1 == 20 && pPlayer[0]->PowerLines == 5))
	{
		int    pos;				// Position, an der das zu ersetzende Wort steht
		string newWord = "";			// neuer Text für das zu ersetzende Wort
		string s_text = "";			// Alter Text, in dem ersetzt wird
		char   c_text[512];		// Neuer Text, nachdem alles ersetzt wurde		

		// Bei Trigger 7 Anzahl der Diamanten auf fünf setzen (falls die ersten fünf nicht richtig
		// eingesammelt wurden)
		//
		if (Value1 == 7 &&
			pPlayer[0]->CollectedDiamonds < 5)
			Value2 = pPlayer[0]->CollectedDiamonds = 5;

		// Bei Trigger 28 dem Spieler eine SmartBomb geben
		//
		if (Value1 == 28 &&
			pPlayer[0]->SmartBombs == 0)
			pPlayer[0]->SmartBombs = 1;

		s_text = TextArray[TEXT_TUTORIAL1 + Value1];

		// die Makros für die Aktionstasten mit den Namen der aktuell eingestellten Tasten ersetzen
		//

		for (int i = 0; i < MAX_AKTIONEN*2; i+= 2)
		{
			do
			{				
				pos = s_text.find (s_Replacers[i]);
				if (pos != string::npos)
				{
					newWord = s_Replacers[i+1];
					newWord = "'" + newWord + "'";
					s_text.replace(pos, strlen(s_Replacers[i]), newWord);
				}

			} while (pos != string::npos);
		}

		// aktualisierten Text übernehmen
		//

		int i = 0;
		int len = s_text.length();
		for (i = 0; i < len; i++)
			c_text[i] = s_text[i];

		c_text[i] = '\0';

		pGUI->ShowBox(c_text, 100);
		pGUI->m_TextID = Value1;

		pSoundManager->PlayWave (100, 128, 11025, SOUND_MESSAGE);
	}

	// Spieler weg und Trigger auf dem Screen? Dann ausfaden
	if (pGUI->m_TextID == Value1 &&
		IsOnScreen() &&
		pGUI->m_FadeMode == VISIBLE &&
		PlayerAbstand() > 120)
		pGUI->HideBox();

	bool stillAlive = false;

	// Einzelne Trigger bei bestimmter Bedingung zum nächsten Text umschalten
	if (PlayerAbstand() < 120)
	switch (Value1)
	{
		// die ersten 5 Diamanten eingesammelt?
		//
		case 2:
			if (pPlayer[0]->CollectedDiamonds >= 5)
			{
				Value1 = 3;
			}
		break;

		// die nächsten 3 Diamanten eingesammelt?
		//
		case 7:
			if (pPlayer[0]->CollectedDiamonds >= 8)
			{
				Value1 = 8;
			}
		break;

		// die ersten drei augen plattgemacht?
		//
		case 9:		
		{
			GegnerClass* pTemp;

			pTemp = pGegner->pStart;			

			while (pTemp != NULL)
			{
				// noch mind. ein Auge am Leben=
				//
				if (pTemp->GegnerArt == AUGE &&
					pTemp->Value2 == 99)
					stillAlive = true;

				pTemp = pTemp->pNext;
			}

			if (stillAlive == false)
			{
				Value1 = 10;
			}
		}
		break;

		// Spreadshot das erste mal aufgepowert
		//
		case 10:
		{
			if (pPlayer[0]->CurrentWeaponLevel[0] >= 2 &&
				pPlayer[0]->BlinkCounter <= 3.0f)
			{
				Value1 = 11;
			} 
		}
		break;

		// Pfeil auf Spreadshot Stufe anzeigen
		case 11:
		{			
			pHUD->ShowArrow (256, 60);
		}
		break;

		// Pfeil auf Blitz Stufe anzeigen
		case 15:
		{			
			pHUD->ShowArrow (351, 60);
		}
		break;

		// ersten Powerblock angeschoßen?
		case 16:
		{			
			GegnerClass* pTemp;

			pTemp = pGegner->pStart;			

			while (pTemp != NULL)
			{
				if (pTemp->GegnerArt == POWERBLOCK)
				{
					GegnerPowerBlock *pPower;

					// angeschoßenen PowerBlock suchen
					//
					pPower = (GegnerPowerBlock*)(pTemp);

					if (pPower->ExtraType == 7 &&
						pPower->AnimPhase == 1)
					{
						Value1 = 17;
						pTemp = pGegner->pEnd;
					}
					else
						pTemp = pTemp->pNext;
				}
				else
						pTemp = pTemp->pNext;
			}
		}
		break;

		// alle PowerLines gesammelt?
		case 17:
		{		
			if (pPlayer[0]->PowerLines >= 5)
				Value1 = 18;
		}
		break;

		// Pfeil auf PowerLines
		case 18:
		{		
			pHUD->ShowArrow (391, 60);
		}
		break;

		// alle Granaten gesammelt?
		case 20:
		{		
			if (pPlayer[0]->Grenades >= 2)
			{
				pPlayer[0]->PowerLines = 0;
				Value1 = 21;
			}
		}
		break;

		// Pfeil auf Rad-Energie
		case 24:
		{		
			pHUD->ShowArrow (548, 70);
		}
		break;

		// Meldung nach der Schockwaver
		case 28:
		{		
			if (pPlayer[0]->SmartBombs == 0)
				Value1 = 29;
		}
		break;

		// Pfeil auf Shockwave
		case 29:
		{		
			pHUD->ShowArrow (434, 42);
		}
		break;

		default:
		break;
	}
}

// --------------------------------------------------------------------------------------
// Trigger explodiert
// --------------------------------------------------------------------------------------

void GegnerTutorialText::GegnerExplode(void)
{
}
