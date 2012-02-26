#ifndef _TRIGGER_SCHLEUSEV_H_
#define _TRIGGER_SCHLEUSEV_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerSchleuseV : public GegnerClass
{
	public:

	GegnerSchleuseV  (int Wert1,	int Wert2,		// Konstruktor
						bool Light);	
	void GegnerExplode (void);							// Gegner explodiert
	void DoKI		   (void);							// Gegner individuell mit seiner 
														// eigenen kleinen KI bewegen
};

#endif