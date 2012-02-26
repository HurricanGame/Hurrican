// Datei : Tileengine.h

// -------------------------------------------------------------------------------------- 
//
// 2D Tile-Engine für Hurrican
// bestehend aus einem Vordergrund-Layer in verschiedenen Helligkeitsstufen
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include <d3dx8.h>
#include "DX8Graphics.h"
#include "DX8Sprite.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

//--- Alles Felder, die den Spieler NICHT blockieren

#define	EMPTY				0						// Leeres Feld
#define DAMGE				1						// Schaden
#define	ANIMATED			2						// Animiert
#define	OVERLAY				3						// Verdecken
#define	ANIMATED_OVERLAY	4						// Animiert UND Verdecken (Wasser zb)

//--- Alles Felder, die den Spieler in irgend einer Weise blockieren

#define PLATFORM			5						// Plattform, blockiert nur von oben
#define	WALL				6						// Massive Wand
#define ANIMATED_WALL		7						// Massive Wand, aber animiert

//--- Animationsgeschwindigkeit der animierten Level-Tiles

#define TILEANIM_SPEED      2.0f

//--- Werte zur Levelgrösse

#define	TILESIZE_X			20						// Grösse eines 
#define	TILESIZE_Y			20						// einzelnen Tiles
#define	TILEMUL_X			0.05f						// einzelnen Tiles
#define	TILEMUL_Y			0.05f						// einzelnen Tiles
#define TILESETSIZE_X		256.0f					// Grösse eines
#define TILESETSIZE_Y		256.0f					// Tilesets

#define SCREENSIZE_X		32						// Leveltiles pro Screen 
#define SCREENSIZE_Y		24

#define MAX_LEVELSIZE_X		1200					// Gesamtgrösse des Level
#define MAX_LEVELSIZE_Y		1200

#define MAX_TILESETS		256						// Maximalzahl der Tilesets
#define INCLUDE_ZEROTILE	1						// Tile 0,0 im Tileset mit verwenden ?

//----- Grösse des nicht scrollbaren bereichs setzen

#define	SCROLL_BORDER_LEFT		240
#define	SCROLL_BORDER_TOP		80
#define	SCROLL_BORDER_RIGHT		400
#define	SCROLL_BORDER_BOTTOM	280

// --------------------------------------------------------------------------------------
// Strukturen
// --------------------------------------------------------------------------------------

struct LevelTileStruct
{
	unsigned char	TileSetBack;					// Back  aus welchem Tileset ?
	unsigned char	TileSetFront;					// Front aus welchem Tileset ?
	unsigned char   BackArt;						// Tile im Hintergrund
	unsigned char	FrontArt;						// Tile im Vordergrund
	unsigned char	Red, Green, Blue, Alpha;		// Farbwert des Tiles
	unsigned int	Block;							// Blockierungsart (siehe #defines für Flags)
};

// --------------------------------------------------------------------------------------
// Struktur für das eines Objektes
// --------------------------------------------------------------------------------------

struct LevelObjectStruct
{
	unsigned int	ObjectID;					// Welche Objekt ID ?
	int				XPos;						// x-Position
	int				YPos;						// y-Position
	bool			ChangeLight;				// Umgebungslicht annehmen ?
};

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// TileEngine Klasse
// --------------------------------------------------------------------------------------

class TileEngineClass
{
	private:
		float			TileAnimCount;							// Animations-Zähler und
		float			CloudMovement;
		int				TileAnimPhase;							// Phase der Tile Animation
		float			ScrollSpeedX, ScrollSpeedY;				// Scrollspeed
		VERTEX2D		TilesToRender[33*25*6];					// Alle zu rendernden Leveltiles
		VERTEX2D		v1, v2, v3, v4;							// Vertices zum Sprite rendern
		char			Beschreibung[100];						// Beschreibung des Levels

	public:
		bool			bScrollBackground;						// Letzten Layer scrollen ?
		unsigned char	LoadedTilesets;							// Anzahl geladener Sets
		LevelTileStruct					Tiles[MAX_LEVELSIZE_X]  // Array mit Leveldaten
											 [MAX_LEVELSIZE_Y];
		DirectGraphicsSprite			TileGfx[MAX_TILESETS];	// Tilegrafiken
		DirectGraphicsSprite			Background;				// Hintergrund
		DirectGraphicsSprite			ParallaxLayer[5];		// Anzahl der Layer
		DirectGraphicsSprite			CloudLayer;				// Wolkenlayer
		long double						XOffset, YOffset;		// Scrolloffset des Levels		
		unsigned int					LEVELSIZE_X;			// Grösse des Levels
		unsigned int					LEVELSIZE_Y;			// in Tiles
		double							LEVELPIXELSIZE_X;		// Levelgrösse in Pixeln
		double							LEVELPIXELSIZE_Y;		// (für XOffset und YOffset)

		TileEngineClass(void);									// Konstruktor
	   ~TileEngineClass(void);									// Destruktor
	    bool LoadLevel(char Filename[100]);						// Level laden
	    void InitNewLevel(int xSize, int ySize);				// Neues Level initialisieren
		void SetScrollSpeed(float xSpeed, float ySpeed);		// neue Scrollspeed setzen
		void DrawBackground  (void);							// Hintergrund Layer zeichnen
		void DrawBackLevel   (void);							// Level hintergrund anzeigen
		void DrawFrontLevel  (void);							// Level vordergrund anzeigen
		void DrawOverlayLevel(void);							// Tiles, die verdecken
		void UpdateLevel(void);									// Level evtl scrollen usw
		int	 BlockRechts(float &x, float &y, RECT rect);		// Funktionen zum Überprüfen
		int  BlockLinksPlayer(float &x, float &y, RECT rect);	// Extra-Wurscht
		int	 BlockLinks (float &x, float &y, RECT rect);		// welche Blockart sich l r o u
		int	 BlockOben  (float &x, float &y, RECT rect);		// vom übergebenen Rect mit
		int	 BlockUnten (float &x, float &y, RECT rect);		// x/y befindet
		int	 BlockUntenNormal (float &x, float &y, RECT rect);	// Block unten ohne "Begradigung"
};