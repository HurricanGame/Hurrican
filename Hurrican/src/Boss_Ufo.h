#ifndef _BOSS_UFO_H_
#define _BOSS_UFO_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerUfo : public GegnerClass
{
	public:

		float			ShotDelay;						// Schussverz�gerung
		float			ActionDelay;					// Wie lange wird die aktuelle Aktion durchgef�hrt
		float			SmokeDelay;

	GegnerUfo		   (int Wert1,	int Wert2,			// Konstruktor
							  bool		  Light);	
	void GegnerExplode (void);							// Gegner explodiert
	void DoKI		   (void);							// Gegner individuell mit seiner 
	void DoDraw		   (void);
														// eigenen kleinen KI bewegen
};

#endif
