#ifndef _TRIGGER_COLUMN2_H_
#define _TRIGGER_COLUMN2_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerColumn2 : public GegnerClass
{
	private:
		float	FallSpeed;
		float	FallValue;

	public:

	GegnerColumn2  (int Wert1,	int Wert2,		// Konstruktor
						bool Light);	
	void GegnerExplode (void);							// Gegner explodiert
	void DoKI		   (void);							// Gegner individuell mit seiner 
														// eigenen kleinen KI bewegen
	void DoDraw		   (void);
};

#endif
