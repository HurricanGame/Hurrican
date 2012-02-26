#ifndef _GEGNER_DRONE_H_
#define _GEGNER_DRONE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerDrone : public GegnerClass
{	
	private :
		int		ShotCount;								// Wieviele Sch�sse abgeben ?
		float	ShotDelay;								// Zeitabstand zwischen Sch�ssen
		float	ActionDelay;							// Zeitabstand bis zur n�chsten Salve

	public :

	GegnerDrone (int  Wert1, int Wert2,					// Konstruktor
					   bool Light);
	void GegnerExplode (void);							// Gegner explodiert
	void DoKI		   (void);							// Gegner individuell mit seiner 
														// eigenen kleinen KI bewegen
};

#endif
