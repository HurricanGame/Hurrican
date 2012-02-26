// Datei : Partikelsystem.h

// -------------------------------------------------------------------------------------- 
//
// Partikelsystem für Hurrican
// für Funken, Rauchwolken, Patronenhülsen usw.
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __Partikelsystem_h__
#define __Partikelsystem_h__

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include <d3dx8.h>
#include "DX8Sprite.h"
#include "Player.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define MODE_NORMAL  0
#define MODE_ROTATED 1

//----- Partikel, die normal oder nur mit Alphablending gerendert werden

#define	BULLET					0						// Patronenhülse
#define	EXPLOSION_MEDIUM		2						// Mittelgrosse Explosion
#define	EXPLOSION_MEDIUM2		3						// Mittelgrosse Explosion Nr 2
#define	EXPLOSION_MEDIUM3		4						// Mittelgrosse Explosion Nr 3
#define	EXPLOSION_GREEN 		5						// Grüne mittelgroße Explosion
#define	EXPLOSION_BIG			6						// Grosse Explosion
#define	EXPLOSION_GIANT			10						// Riesen Explosion
#define	EXPLOSION_ALIEN			11						// Riesen Explosion
#define	BLUE_EXPLOSION			12						// Blaue kleine Explosion
#define	SPLITTER				13						// Kleiner animierter Splitter
#define	PIRANHATEILE			14						// Teile eines kaputten Piranhas
#define	PIRANHATEILE2			15						// Teile eines kaputten Riesen Piranhas
#define	PIRANHABLUT				16						// Blut  eines kaputten Piranhas
#define ROCKSPLITTER			17						// Brocken eines kaputten Felsblocks
#define ROCKSPLITTERSMALL		18						// kleine Brocken eines kaputten Felsblocks ;)
#define ROCKSPLITTERSMALLBLUE	19						// kleine Brocken eines kaputten Stalagtits
#define SPIDERSPLITTER			20						// kleine Brocken einer kaputten Spinne :P
#define SPIDERSPLITTER2			21						// kleine Brocken einer kaputten Spinne :P
#define SPIDERGRENADE			22						// Spinnen Granate (kein SCHUSS, da er Spieler nicht treffen kann)
#define EVILSMOKE				23						// Schatten des EvilHurri links
#define EVILSMOKE2				24						// Schatten des EvilHurri rechts
#define STELZE					25						// Bein eines StelzSacks
#define STELZHEAD				26						// Kopf eines StelzSacks
#define SMOKE2					27						// Rauch2
#define	EXPLOSION_GIGA			28						// Riesen Explosion

#define SMOKE3					29						// wie Rauch2, nur steigt er nach oben
#define SMOKE3_RO				30						// Rauch3 nach rechts oben
#define SMOKE3_R				31						// Rauch3 nach rechts
#define SMOKE3_RU				32						// Rauch3 nach rechts unten
#define SMOKE3_U				33						// Rauch3 nach unten
#define SMOKE3_LU				34						// Rauch3 nach links unten
#define SMOKE3_L				35						// Rauch3 nach links
#define SMOKE3_LO				36						// Rauch3 nach links oben

#define SMOKEBIG				37						// Riesen Rauch
#define SMOKEBIG2				38						// Riesen Rauch
#define SMOKEBIG_OUTTRO			99						// Riesen Rauch

#define	MADEBLUT				39						// Blut der Made
#define SPAWNDROP				40						// Wassertropfen der vom Partikelspawner augespuckt wird
#define BLATT					41						// Umherwirbelndes Blatt aus der Pflanze
#define BLATT2					42						// Umherwirbelndes Blatt aus dem Partikelspawner
#define NESTLUFT				43						// Umherwirbelndes Wespennest nach Abschuss =)
#define FLOATSMOKE				44						// Rauch der kleinen Floating Plattform
#define THUNDERBOLT				45						// Blitz + Donner (Screen flackert kurz auf)
#define REGENTROPFEN			46						// Regentropfen
#define FOG						47						// Nebel Partikel
#define	HALSWIRBEL				48						// Halswirbel des MetalHead Bosses
#define	KAPUTTETURBINE			49						// Kaputte Turbine des MetalHead Bosses
#define SCHNEEFLOCKE			50						// Eine Schneeflocke
#define HURRITEILE				51						// Teile vom Hurri, wenn er explodiert ist =)
#define BOULDER_SMALL			52						// Kleiner blauer Boulder

#define WASSER_SPRITZER2		53						// Wasserspritzer, wenn der Spieler ins Wasser hopst
#define WASSER_SPRITZER			54						// Wasserspritzer, wenn der Spieler aus dem Wasser hopst
#define LAVA_SPRITZER			55						// Lavaspritzer beim reinhopsen
#define LAVA_SPRITZER2			56						// Lavaspritzer beim raushopsen

#define LAVAKRABBE_KOPF			57						// Kopf der Lavakrabbe
#define LAVAKRABBE_BEIN			58						// Bein der Lavakrabbe

#define SPIDERPARTS				59						// Spinnenstücke auf dem Fliessband
#define KETTENTEILE				60						// Kettenteile (Arm links)
#define KETTENTEILE2			61						// Kettenteile (Arm rechts)
#define KETTENTEILE3			62						// Kettenteile (Arm Mitte)
#define KETTENTEILE4			63						// Kettenteile (Kettenglied)

#define SMOKE					64						// Rauch
#define DUST					65						// Staub
#define SCHROTT1				66						// Schrotteil1
#define SCHROTT2				67						// Schrotteil1
#define	BULLET_SKELETOR			68						// Patronenhülse vom letzten Endboss
#define	GLASSPLITTER			69						// Glassplitter (von der Tube)

#define SCHNEEFLOCKE_END		70						// Eine Schneeflocke fürs Outtro

// Teile vom explodierten Hurrican :)

#define HURRITEILE_ARM1			80
#define HURRITEILE_ARM2			81
#define HURRITEILE_BEIN1		82
#define HURRITEILE_BEIN2		83
#define HURRITEILE_KOPF			84
#define HURRITEILE_WAFFE		85
#define HURRITEILE_TORSO		86

//----- Partikel, die mit additivem Alphablending gerendert werden

#define ADDITIV_GRENZE			100
#define FUNKE					100						// Roter Funke
#define FUNKE2					101						// Grüner Funke
#define LASERFUNKE				103
#define LASERFUNKE2				104
#define WASSERTROPFEN			106						// Spritzer beim ins Wasser eintreten
#define WASSERTROPFEN2			107						// Spritzer beim in die Säure eintreten
#define BUBBLE					108						// Luftbläschen unter Wasser
#define LASERFLAME				111						// Leuchteffekt für den Krabblerlaser
#define LASERFLAMEPHARAO		112						// Leuchteffekt für die Pharaokopf Augen
#define PHARAOSMOKE				113						// Leuchteffekt für den Pharaokopf Laser
#define SHIELD					114						// Schutzschild-Stern
#define ROCKETSMOKE				115						// Rauch einer Rakete
#define ROCKETSMOKEBLUE			116						// Blauer Rauch
#define ROCKETSMOKEGREEN		117						// Grüner Rauch
#define FLUGSACKSMOKE			118						// Rauch des Flug Sacks links
#define FLUGSACKSMOKE2			119						// Rauch des Flug Sacks rechts
#define EVILFUNKE				120						// Funke des Evil Blitzes
#define EVILROUNDSMOKE			121						// Rauch des Rundumschusses des Evil Hurri
#define BEAMSMOKE				122						// Rauch des Blitzbeams
#define BEAMSMOKE2				123						// Rauch beim Aufladen des Blitzbeams
#define BEAMSMOKE3				124						// Rauch beim Explodieren des Blitzbeams
#define BEAMSMOKE4				125						// Rauch beim Explodieren des Blitzbeams
#define BEAMSMOKE5				126						// Kringel beim Explodieren des Blitzbeams
#define SNOWFLUSH				127						// Schneegestöber
#define WATERFLUSH				128						// Wasserfall Rauch
#define WATERFLUSH2				129						// Wasserfall Rauch
#define WATERFLUSH_HIGH			130						// Wasserfall Rauch
#define WATERFLUSH_HIGH2		131						// Wasserspritzer Rauch
#define UFOLASERFLARE			132						// Flare des Ufo Lasers
#define ROBOMANSMOKE			133						// Rauch des Flug Sacks rechts
#define STELZFLARE				134						// Flare des Stelzsack Lasers
#define LONGFUNKE				135						// Langer Funke, der eine Spur zieht (Linie)
#define WATERFUNKE				136						// Wasserspritzer, der eine Spur zieht
#define KRINGEL					137						// Die Kringelpartikel, die eine neue Waffenstufe angeben
#define KRINGELR				138						// rot
#define KRINGELB				139						// blau
#define KRINGELG				140						// grün
#define KRINGELHB				141						// hellblau
#define TURBINESMOKE			142						// Partikel, die in die Turbine des Metalhead Bosses gesaugt werden
#define MINIFLARE				143						// Flare vom Lava Ball, der runtersegelt
#define GRENADEFLARE			144						// Aufleuchten bei Granaten-Treffer
#define EXPLOSIONFLARE			145						// Aufleuchten bei Granaten-Treffer
#define EXPLOSIONFLARE2			146						// Aufleuchten bei Lilaschuss-Treffer
#define SCHLEIM					147						// Schleimtropfen
#define SCHLEIM2				148						// Schleim aus SchleimAlien mit Maul
#define SHOCKEXPLOSION			149						// SchockWelle bei Spieler Explosion
#define SHOTFLARE				150						// Leuchten bei Schuss-Explosion gelb
#define SHOTFLARE2				151						// Leuchten bei Schuss-Explosion grün
#define EXTRACOLLECTED			152						// Leuchten beim Einsammeln eines Extras
#define LASERFLARE				153						// Flare beim Auftreffen des Lasers
#define EXPLOSION_MEDIUM2_ADD	154						// Die coolste Explosion
#define EXPLOSION_MEDIUM3_ADD	155						// Die coolste Explosion
#define EXPLOSION_TRACE			156						// Eine Bahn von Explosionen ziehen
#define EXPLOSION_REGULAR		157						// Kleinere Explosion in Gelb, die immer gleich abläuft
#define DIAMANTCOLLECTED		158						// Leuchten bei Diamant eingesammelt
#define LILA					159						// Lila leuchten
#define EXPLOSION_KRINGEL		160						// Kringel/Druckwelle bei Explosion
#define DRACHE_SMOKE			161						// Antrieb des Drachen
#define FIREBALL_SMOKE			162						// Rauch des Feuerballs des Minidrachens
#define EXPLOSION_TRACE_END		163						// In der Größe veränderbar
#define LAVADUST				164						// Lavafunken
#define KRINGELSECRET			165						// Die Kringelpartikel beim Secret
#define TEXTSECRET				166						// Text "Secret"

// Partikel ohne Grafik
#define LILA2					167						// Lila leuchten fällt runter
#define LILA3					168						// Lila leuchten fliegt links

#define MAX_PARTIKELGFX			169						// Maximale Anzahl verschiedener Grafiken

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Partikel Klasse (Werte für EIN Partikel)
// --------------------------------------------------------------------------------------

class PartikelClass
{
	private:
		int					red,green,blue,alpha;		// Farbwerte des Partikels
		D3DCOLOR			Color;						// Farbe des Partikels
		float				xSpeed,ySpeed;				// Geschwindigkeit des Partikels
		float				xAcc, yAcc;					// Beschleunigung des Partikels
		int					AnimPhase, AnimEnde;		// Aktuelle Phase und Endphase		
		float				AnimSpeed, AnimCount;		// Anim-Geschwindigkeit und Counter		
		bool				Rotate;						// evtl rotieren?
		float				Rot;						// Rotation
		float				RotDir;						// Richtung
		bool				BounceWalls;				// an Wänden abprallen ?
		bool				OwnDraw;					// Extrawurscht beim rendern?
		bool				RemoveWhenOffScreen;		// verschiwnden lassen, wenn ausserhalb des Screeens?

	public:
		float				xPos,yPos;					// Position des Partikels
		float				xPosOld, yPosOld;			// Alte Position

		int					PartikelArt;				// Art des Partikels (siehe Defines)
		float				Lebensdauer;				// Wie lange existiert das Partikel ?

		PartikelClass(void);							// Konstruktor
	   ~PartikelClass(void);							// Destruktor
		bool CreatePartikel(float x, float y, int Art,  // Bestimmten Partikel erzeugen
							PlayerClass* pParent = NULL);
		void Run	(void);								// Partikel animieren und bewegen
		bool Render	(void);								// Partikel anzeigen
		PartikelClass		*pNext;						// Zeiger auf den nächsten   Partikel
		PartikelClass		*pPrev;						// Zeiger auf den vorherigen Partikel
		PlayerClass			*m_pParent;
};

// --------------------------------------------------------------------------------------
// Partikelsystem Klasse, Linked List, die alle Partikel beinhaltet
// --------------------------------------------------------------------------------------

class PartikelsystemClass
{
	private:
		int						NumPartikel;			// aktuelle Zahl der Partikel		
		int						MAX_PARTIKEL;			// was wohl

	public:
		float					xtarget, ytarget;		// Zielpunkt, auf den sich bestimmte Partikel richten
		float					ThunderAlpha;			// Alpha für Blitz
		unsigned char			ThunderColor[3];		// Farbe des Blitzes (r, g, b)

		PartikelClass			*pStart;				// Erstes  Element der Liste
		PartikelClass			*pEnd;					// Letztes Element der Liste

		PartikelsystemClass(void);						// Konstruktor
	   ~PartikelsystemClass(void);						// Destruktor

	    bool PushPartikel(float x, float y, int Art,
						 PlayerClass* pParent = NULL);	// Partikel "Art" hinzufügen
		void DelSel		(PartikelClass *pTemp);			// Ausgewähltes Objekt entfernen
		void ClearAll	(void);							// Alle Objekte löschen
	    int  GetNumPartikel(void);						// Zahl der Partikel zurückliefern
		void DoPartikel(void);							// Alle Partikel der Liste animieren/anzeigen
		void DoPartikelSpecial(bool ShowThem);			// Alle Partikel der Liste animieren/anzeigen
		void DrawOnly(void);							// Alle Partikel der Liste nur anzeigen
		void DoThunder (void);
		void ClearPowerUpEffects(void);
		void SetParticleCount(void);					// Je nach Detailstufe Maximum setzen
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern PartikelsystemClass	*pPartikelSystem;
extern DirectGraphicsSprite	*pPartikelGrafix[MAX_PARTIKELGFX];	// Grafiken der Partikel
extern int					CurrentPartikelTexture;

#endif
