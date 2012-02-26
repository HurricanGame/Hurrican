#ifndef _BOSS_RIESENWESPE_H_
#define _BOSS_RIESENWESPE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerRiesenWasp : public GegnerClass
{
	private:
		float ShotDelay;

	public:

	GegnerRiesenWasp(int  Wert1, int Wert2,				// Konstruktor
					   bool Light);
	void GegnerExplode (void);							// Gegner explodiert
	void DoKI		   (void);							// Gegner individuell mit seiner 
														// eigenen kleinen KI bewegen
};

#endif
