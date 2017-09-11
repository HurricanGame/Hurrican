#ifndef _BOSS_FAHRSTUHLBOSS_H_
#define _BOSS_FAHRSTUHLBOSS_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerFahrstuhlBoss : public GegnerClass
{
public:

    DirectGraphicsSprite	Head;					// Grafiken für den "Kopf" des Gegners
    DirectGraphicsSprite	Canon;				// Grafiken für die zwei Kanonen
    float					ShotDelay;				// Schussverzögerung
    float					TempY;					// yPosition relativ zum Fahrstuhl
    float					x1, y1, x2, y2, x3;		// Koordinaten für Kopf und Kanonen
    float					dx1, dx2;				// Speed für Kopf und Kanonen
    float					CanonAnim;				// AnimationsPhase der Kanone

    bool					Activated;				// Boss kommt schon rein
    int						Shots;					// Wieviele Schüsse abgeben

    GegnerFahrstuhlBoss (int Wert1,	int Wert2,			// Konstruktor
                         bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif
