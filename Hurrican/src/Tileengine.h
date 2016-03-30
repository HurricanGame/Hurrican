// Datei : Tileengine.h

// --------------------------------------------------------------------------------------
//
// 2D Tile-Engine für Hurrican
// bestehend aus einem Vordergrund-Layer in verschiedenen Helligkeitsstufen
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _TileEngine_h_
#define _TileEngine_h_

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#if defined(PLATFORM_DIRECTX)
#include <d3dx8.h>
#endif
#include "DX8Graphics.h"
#include "DX8Sprite.h"
//#include "lightmap.h"     //DKS - all light-map related code disabled
#include "CDragonHack.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

//----- Flags für den Blockwert

#define BLOCKWERT_WAND				1		// Solide Wand
#define BLOCKWERT_GEGNERWAND		2		// Wand nur für Gegner
#define BLOCKWERT_PLATTFORM			4		// Plattform
#define BLOCKWERT_LIGHT				8		// Licht bei Objekten verändern
#define BLOCKWERT_VERDECKEN			16		// Spieler und Objekte verdecken
#define BLOCKWERT_ANIMIERT_BACK		32		// Animiert Hintergrund
#define BLOCKWERT_ANIMIERT_FRONT	64		// Animiert Overlay
#define BLOCKWERT_WASSER			128		// Wasser (macht platsch :D )
#define BLOCKWERT_SCHADEN			256		// Schaden
#define BLOCKWERT_FLIESSBANDL		512		// Fliessband Links
#define BLOCKWERT_FLIESSBANDR		1024	// Fliessband Rechts
#define BLOCKWERT_WENDEPUNKT		2048	// Bewegte Plattformen umdrehen lassen
#define BLOCKWERT_DESTRUCTIBLE		4096	// Zerstörbare Wand
#define BLOCKWERT_MOVELINKS			8192	// Textur nach links bewegen
#define BLOCKWERT_OVERLAY_LIGHT	   16384	// Overlay nimmt Licht an
#define BLOCKWERT_SUMPF			   32768	// Einsinken
#define BLOCKWERT_EIS			   65536	// Eis, auf dem man ausrutscht
#define BLOCKWERT_MOVEVERTICAL	  131072	// Vertikale Texturbewegung
#define BLOCKWERT_WASSERFALL	  262144	// Wasserfall
#define BLOCKWERT_MOVERECHTS	  524288	// Textur nach rechts bewegen

#define BLOCKWERT_SCHRAEGE_L	 2097152	// Schräge Rechts
#define BLOCKWERT_SCHRAEGE_R	 4194304	// Schräge Rechts
#define BLOCKWERT_LIQUID		 8388608	// Flüssigkeit (Wasser, Säure, Lava, Magensäure)

//--- Animationsgeschwindigkeit der animierten Level-Tiles

#define TILEANIM_SPEED      0.8f

//--- Werte zur Levelgrösse

#define	TILESIZE_X			20						// Grösse eines
#define	TILESIZE_Y			20						// einzelnen Tiles
#define TILESETSIZE_X		256.0f					// Grösse eines
#define TILESETSIZE_Y		256.0f					// Tilesets

#define SCREENSIZE_X		32						// Leveltiles pro Screen
#define SCREENSIZE_Y		24

#define MAX_LEVELSIZE_X		1024					// Gesamtgrösse des Level
#define MAX_LEVELSIZE_Y		1600

#define MAX_TILESETS		64						// Maximalzahl der Tilesets
#define INCLUDE_ZEROTILE	1						// Tile 0,0 im Tileset mit verwenden ?

//----- Grösse des nicht scrollbaren Bereichs

#define	SCROLL_BORDER_EXTREME_LEFT		0
#define	SCROLL_BORDER_EXTREME_TOP		0
#define	SCROLL_BORDER_EXTREME_RIGHT		570
#define	SCROLL_BORDER_EXTREME_BOTTOM	400

#define	SCROLL_BORDER_HORIZ		0
#define	SCROLL_BORDER_TOP		50
#define	SCROLL_BORDER_BOTTOM	50


#define	LOOK_BORDER_TOP			60
#define	LOOK_BORDER_BOTTOM		400

//--- Zustände der Tileengine

#define	ZUSTAND_SCROLLBAR		0		// Spieler kann Level scrollen durch laufen
#define ZUSTAND_SCROLLTO		1		// Level Scrollt zu bestimmter Position
#define ZUSTAND_SCROLLTOLOCK	2		// Level Scrollt zu bestimmter Position und lockt dann
#define ZUSTAND_SCROLLTOPLAYER	4		// Level Scrollt zum Spieler
#define ZUSTAND_LOCKED			5		// Level sitzt an bestimmter Position fest

//--- LiightMaps
//DKS - all light-map related code disabled:
//#define MAX_LIGHTMAPS			5
//
//#define LIGHTMAP_BLITZ			0
//#define LIGHTMAP_EXPLOSION		1
//#define LIGHTMAP_GOLEMSHOT		2
//#define LIGHTMAP_LILA			3

// --------------------------------------------------------------------------------------
// Strukturen
// --------------------------------------------------------------------------------------

// Struktur für ein Level Tile wie es aus dem Level geladen wird
//
//DKS - Made binary sizes of member vars explicit here to make loading/saving binary files more robust:
struct LevelTileLoadStruct
{
    uint8_t  TileSetBack;              // Back  aus welchem Tileset ?
    uint8_t  TileSetFront;             // Front aus welchem Tileset ?
    uint8_t  BackArt;                  // Tile im Hintergrund
    uint8_t  FrontArt;                 // Tile im Vordergrund
    uint8_t  Red, Green, Blue, Alpha;  // Farbwert des Tiles
    uint32_t Block;                    // Blockierungsart (siehe #defines)
};

// Struktur für die Farbwerte von einem Level Tile
//
struct LevelTileSaveStruct
{
    D3DCOLOR		Color[4];							// Farbwert des Tiles (Alle vier Ecken)
};


// Struktur für ein Level Tile wie es im Level vorkommt (wie beim Laden, nur noch mit Extra Farben für alle Ecken)
//
struct LevelTileStruct
{
    unsigned char	TileSetBack;						// Back  aus welchem Tileset ?
    unsigned char	TileSetFront;						// Front aus welchem Tileset ?
    unsigned char   BackArt;							// Tile im Hintergrund
    unsigned char	FrontArt;							// Tile im Vordergrund
    unsigned char	Red, Green, Blue, Alpha;			// Farbwert des Tiles
    D3DCOLOR		Color[4];							// Farbwert des Tiles (Alle vier Ecken)
    uint32_t        Block;								// Blockierungsart (siehe #defines)
    bool			move_v1, move_v2, move_v3, move_v4; // Die Ecken eines Tiles bei der Wasseranim bewegen?
};

// --------------------------------------------------------------------------------------
// Struktur für ein aus dem Level zu ladendes Objekte
// --------------------------------------------------------------------------------------

//DKS - Made binary sizes of member vars and padding bytes explicit here to make
//      loading/saving binary files more robust:
struct LevelObjectStruct
{
    uint32_t ObjectID;               // Welche Objekt ID ?
    int32_t  XPos;                   // x-Position
    int32_t  YPos;                   // y-Position
    uint8_t  ChangeLight;            // Umgebungslicht annehmen ?
    uint8_t  Skill;                  // 0 = Easy, 1 = Medium, 2 = Hard
    uint8_t  PADDING_CHUNK_1[2];     // 2 PADDING BYTES
    int32_t  Value1;                 // Werte für diverse Trigger
    int32_t  Value2;                 // Werte für diverse Trigger
};

// --------------------------------------------------------------------------------------
// Level-Datei Header
// --------------------------------------------------------------------------------------

//DKS - Made binary sizes of member vars and padding bytes explicit here to make
//      loading/saving binary files more robust:
struct FileHeader
{
    char            Kennung[46];                    // Level-Kennung
    char            Beschreibung[100];              // Level-Beschreibung
    char            BackgroundFile[24];             // Dateiname des Hintergrundes
    char            ParallaxAFile [24];             // Dateiname des 1. Parallax Layers
    char            ParallaxBFile [24];             // Dateiname des 2. Parallax Layers
    char            CloudFile     [24];             // Dateiname des WolkenLayers
    uint8_t         PADDING_CHUNK_1[2];             // 2 PADDING  BYTES
    uint32_t        Timelimit;                      // aktuelles Zeitlimit des Levels
    uint8_t         UsedTilesets;                   // Anzahl der Tilesets
    char            SetNames[64][16];               // Namen der benutzten Sets
    uint8_t         PADDING_CHUNK_2[3];             // 3 PADDING BYTES
    uint32_t        SizeX, SizeY;                   // Größe des Levels
    uint32_t        NumObjects;                     // Anzahl der Objekte
    uint8_t         ScrollBackground;               // Hintergrundbild srollen oder statisch ?
    uint8_t         PADDING_CHUNK_3[3];             // 3 PADDING BYTES
};

// --------------------------------------------------------------------------------------
// Anhang am File nach den Level Daten
// damit nicht alle levels nochmal konvertiert werden, hänge ich einfach alle dinge, die
// noch ins level format reinmüssen, dahinter =)
// --------------------------------------------------------------------------------------

//DKS - Made binary sizes of member vars and padding bytes explicit here to make
//      loading/saving binary files more robust:
struct FileAppendix
{
    char    Songs[2][30];             // Namen der benutzten Songs (Stage und Boss)
    int32_t UsedPowerblock;           // ID der benutzten Powerblock Art
    char    Col1[8], Col2[8];         // Farben für Liquid
    uint8_t Taschenlampe;
    uint8_t PADDING_CHUNK_1[3];       // 3 padding bytes
};

// --------------------------------------------------------------------------------------
// Infos für die Schwabbelverscheibung eines einzelnen Vertex
// --------------------------------------------------------------------------------------

struct SchwabbelPos
{
    float lo, ro, lu, ru;
};


// --------------------------------------------------------------------------------------
// Unions
// --------------------------------------------------------------------------------------

// Union für r,g,b,a Zugriff in D3DCOLOR

union FarbUnion
{
    D3DCOLOR color;

    struct
    {
        unsigned char b;
        unsigned char g;
        unsigned char r;
        unsigned char a;
    } farbStruct;
};

struct Vector2D
{
    float x;
    float y;
};

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// WaterSinTableClass
// --------------------------------------------------------------------------------------
//DKS -   Animated water is now drawn using a tiny lookup table that provides the
//      vertex offsets used for both the up/down swaying motion of the water
//      textures as well as for the left/right swaying motion of any artwork
//      drawn behind the water. WaterSinTableClass encapsulates it.
//        There were  problems with the original lookup tables used for
//      these purposes, WaterList[] and SinList2[]:
//      Problem 1.) Tables were humongous (4096 elements each) despite repeating
//                  every 40 elements. This was cache-unfriendly.
//      Problem 2.) The computation of offsets into these tables was not
//                  allowing the swaying animation to syncronize smoothly as
//                  the screen scrolled, especially horizontally.
//      
//        Before, the two trig tables SinList2[] and WaterList[] had elements
//      which stored values of sin() for successive increments of pi/20, thus
//      repeating every 40 elements.
//        WaterList[] stored these values multiplied by 2.5, and was used to
//      animate the up/down swaying of the water textures.
//        SinList2[] stored these same values multiplied by 5.0, and was used
//      to animate the left/right swaying of any artwork laying behind water.
//
//        We now use a 17-element trig table that covers domain 0-pi/2, using
//      trig symmetry to handle anything outside that. Each element x represents
//      2.5f*sin(x * pi/32), SinTable[0] giving 2.5f*sin(0), and
//      SinTable[16] giving 2.5f*sin(pi/2). There is a second trig table that
//      stores the same values multiplied by 2.0 for the left/right swaying.
//
//        Before, each tile 'i,j' would access the sin tables like so:
//
//      /* BEGIN EXAMPLE OF ORIGINAL TRIG TABLE-RELATED GAME CODE FROM DrawWater() */
//      float			WaterList[4096];         // Ridiculously large sin lookup table
//                                               //  that repeats every 40 elements, i.e.
//      float			SinList2[4096];          // Ridiculously large sin lookup table
//                                               //  that repeats every 40 elements, i.e.
//      WaterList[0]  = 0  * pi/20.0 * 2.5f;   SinList2[0]  = WaterList[0]  * 2;
//      (..)
//      WaterList[39] = 39 * pi/20.0 * 2.5f;   SinList2[39] = WaterList[39] * 2;
//      WaterList[40] = 40 * pi/20.0 * 2.5f;   SinList2[40] = WaterList[40] * 2;
//      (..)
//      /* At this point we begin repeating over and over until end of array */
//
//      (...)   /* In tile rendering code .. */
//
//      /* BEGIN DrawWater() ORIGINAL CODE EXAMPLE USING WaterList[] */
//      /* Vertex offset computation for tile i,j (that would not alllow water
//         animation to smoothly move in sync when scrolling left/right):    */
//      (...)
//      int off = (int)(int(WaterPos) + xLevel % 32 + (yLevel * 10) % 40) % 1024;
//      if (..) v1.y += WaterList[off + j*10 + i * 2];      // Top left vertex
//      if (..) v2.y += WaterList[off + j*10 + i * 2];      // Top right vertex
//      if (..) v3.y += WaterList[off + j*10 + i * 2 + 10]; // Bottom left vertex (pi/2 past pair above)
//      if (..) v4.y += WaterList[off + j*10 + i * 2 + 10]; // Bottom right vertex (pi/2 past pair above)
//      (...)
//      /* END ORIGINAL WATER CODE EXAMPLE */
//
//      /* BEGIN ORIGINAL ARTWORK-BEHIND-WATER CODE EXAMPLE: */
//      /* How left/right swaying was calculated for a tile i,j using
//         SinList2[] in DrawBackLevel() and DrawFrontLevel():        */
//      (...)
//      int off = (int(SinPos2) + (yLevel * 2) % 40 + j*2) % 1024;
//      if (..) v1.x += SinList2[off];                      // Top left vertex
//      if (..) v2.x += SinList2[off];                      // Top right vertex
//      if (..) v3.x += SinList2[off + 2];                  // Bottom left vertex (pi/10 past pair above)
//      if (..) v4.x += SinList2[off + 2];                  // Bottom right vertex (pi/10 past pair above)
//      (...)
//      /* END ORIGINAL ARTWORK-BEHIND-WATER CODE EXAMPLE: */
//
//        The new sine table has a phase of 64 increments (64 * pi / 32), so
//      modulus of inputs is done easily with a single bitwise op. Similarly,
//      since SinTable[16] is pi/2, conversion of values from this 64-element
//      domain to the 17-element first-quadrant-table is also trivially done.
//       Since our new tiny, yet more accurate sin table has increments of pi/32,
//      not pi/10 or pi/20, it does not match precisely. However, it does have
//      higher resolution than the original table. Using a factor of 3 in the
//      offset calculations ends up being visually indistinguishable in
//      comparison (3pi/32 == 0.2945  vs. pi/10 == .3142).
//       Because the universal table offset was originally advanced 2.0 SYNC per
//      frame, SinTablePos is instead advanced (16.0/5.0) SYNC. Thus, the overall
//      speed of the animation remains the exact same, i.e.
//      (2 SYNC)*(pi/20) == ((16/5) SYNC)*(pi/32)
//      (NOTE: SYNC is a timer macro that expands to '* SpeedFaktor')

class WaterSinTableClass
{
    public:
        WaterSinTableClass()
        { 
            ResetPosition();
            for (int i=0; i<17; ++i) {
                SinTable[i] = 2.5f * sinf( i * M_PI / 32.0 );
                NonWaterSinTable[i] = 2.0f * SinTable[i];
            }
        }

        // Called in ctor and also when a level is loaded:
        void ResetPosition()
        {
            SinTablePos = 0;
            WaterSinTableIdx = 0;
            NonWaterSinTableIdx = 0;
        }

        // Compute individual table indexes based on SinTablePos and the
        //  current screen position within overall map. (Called from
        //  TileEngineClass::CalcRenderRange() where xLevel/yLevel are
        //  updated each frame before tiles are rendered)
        void UpdateTableIndexes(const int xlev, const int ylev)
        {
            // This is for the two water layers that sway up/down:
            WaterSinTableIdx = (int(SinTablePos) + xlev*3 + ylev*16) % 64;
            // This is for any artwork that sways left/right in the water, like plants or backgrounds
            NonWaterSinTableIdx = (int(SinTablePos) + ylev*3) % 64;
        }

        // Called once per frame in TileEngineClass::UpdateLevel()
        void AdvancePosition(const float speed_faktor)
        {
            // Advance primary offset by 3.20, which is the same as
            // advancing the original code's sin table offset by
            // 2, which was the original position advancement, i.e.
            // 2 * (pi/20) SYNC == (16/5) *(pi/32) SYNC
            SinTablePos += float(16.0/5.0) * speed_faktor;
            while (SinTablePos >= 64.0f) SinTablePos -= 64.0f;
        }

        // This is for the two layers of water textures that sway up/down.
        // (Called from TileEngineClass::DrawWaterVectors())
        // The top two vertices of a tile use sin_pair[0]
        // and the bottom two vertices use sin_pair[1].
        void GetWaterSin(const int i, const int j, float (&sin_pair)[2])
        {
            // Originally, every tile on a row would have specify a sin
            //  value pi/10 past the one to the left, but we'll instead
            //  make that 3(pi/32) past the one to the left. Close enough
            //  to be unnoticeable.
            int ext_idx = WaterSinTableIdx + i*3 + j*16;

            for (int ctr=0; ctr < 2; ++ctr) {
                int table_idx = ext_idx;
                bool negate_result = ConvertToTableIndex(table_idx);
                sin_pair[ctr] = negate_result ? -SinTable[table_idx] : SinTable[table_idx];
                ext_idx += 16;      // Second sine value is one quadrant past first one
            }
        }

        // This is for the two layers of water textures that sway up/down.
        // (Called from TileEngineClass::DrawTileGroupedForwardList())
        // The top two vertices of a tile use sin_pair[0]
        // and the bottom two vertices use sin_pair[1].
        void GetNonWaterSin(const int j, float (&sin_pair)[2])
        {
            int ext_idx = NonWaterSinTableIdx + j*3;

            for (int ctr=0; ctr < 2; ++ctr) {
                int table_idx = ext_idx;
                bool negate_result = ConvertToTableIndex(table_idx);
                sin_pair[ctr] = negate_result ? -NonWaterSinTable[table_idx] : NonWaterSinTable[table_idx];

                ext_idx += 3;  // Second sine value originally is pi/10 past first one (2*j),but
                               //  we'll end up moving just 3*pi/32 using new trig table. That's 
                               //  close enough to be unnoticable. (16.87 deg instead of 18 deg)
            }
        }

    private:

        // Convert an index x representing x/64 fraction of a circle,
        // 64 being 2*pi, to an internal table index 0-16, 16
        // representing pi/2, using symmetric property of sine function.
        // Boolean return value indicates if the value retrieved from the
        // table should be negated.
        bool ConvertToTableIndex(int &idx)
        {
            bool negate_result = false;
            idx %= 64;
            int increments_past_quad = idx % 16;
            int quad = idx / 16;

            switch (quad) {
                default:    // Default case shouldn't happen; fall through to case 0

                case 0:
                    idx = increments_past_quad;
                    break;
                case 1:
                    idx = 16 - increments_past_quad;
                    break;
                case 2:
                    idx = increments_past_quad;
                    negate_result = !negate_result;
                    break;
                case 3:
                    idx = 16 - increments_past_quad;
                    negate_result = !negate_result;
                    break;
            }

            return negate_result;
        }


        float SinTablePos;           // Value 0.0-63.999 that represents current base
                                     //  used to generate offsets of water tile vertices.
                                     //  Represents 0*(2*pi/64) through 64*(2*pi/64).
                                     //  Incremented through UpdatePosition() function.

        // The two index vars, one for each table, are updated in UpdateTableIndexes() function:
        int   WaterSinTableIdx;           // For the two layers of actual water that sway up/down
        int   NonWaterSinTableIdx;        // For any artwork on a water tile like plants that sway left/right

        float SinTable[17], NonWaterSinTable[17];
};


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
    VERTEX2D		TilesToRender[40*30*6];				// Alle zu rendernden Leveltiles
    VERTEX2D		v1, v2, v3, v4;							// Vertices zum Sprite rendern
    unsigned char	LoadedTilesets;							// Anzahl geladener Sets
    LevelTileStruct Tiles[MAX_LEVELSIZE_X]  // Array mit Leveldaten
                         [MAX_LEVELSIZE_Y];

    // DKS - Replaced both SinList2 and WaterList lookup tables with new class
    //       WaterSinTableClass. See its comments for more info.
    WaterSinTableClass WaterSinTable;

    //DKS - Lightmap code in original game was never used and all related code has now been disabled:
    //CLightMap		lightmaps[MAX_LIGHTMAPS];

public:
    FileAppendix	DateiAppendix;							// Anhang der Level-Datei
    bool			IsElevatorLevel;
    bool			MustCenterPlayer;
    int				ColR1, ColG1, ColB1;					// Farben in RGB
    int				ColR2, ColG2, ColB2;
    int				ColR3, ColG3, ColB3, ColA3;

    // DKS - Replaced both SinList2 and WaterList lookup tables with new class
    //       WaterSinTableClass.  See its comments for more info.
    //float			WaterPos;								// Position in der WaterListe für die Wasseroberfläche
    ////DKS - Disabled this, was only ever filled with zeroes and had no effect.
    ////float			SinList[4096];							// Sinus Liste zum Schwabbeln des Alienlevels

    //float			SinList2[4096];							// Sinus Liste zum Schwabbeln des Wasserhintergrunds
    //float			WaterList[4096];						// Sinus Liste zum Schwabbeln der Oberfläche

    RECT			TileRects[144];							// vorberechnete Tile Ausschnitte
    char			Beschreibung[100];						// Beschreibung des Levels
    int				Zustand;								// Aktueller Zustand
    float			ScrolltoX, ScrolltoY;					// Lock-Werte
    float			SpeedX, SpeedY;							// Speed for ScrollTo Funktion
    bool			bDrawShadow;							// Taschenlampen Shatten im Alien Level rendern?
    int				MaxBlocks;
    int				MaxOneUps;
    int				MaxSecrets;
    int				MaxDiamonds;

    //DKS - See above note regarding SinList: disabled SinPos (unused)
    //float			SinPos;									// Position in der SinusListe für das AlienLevel
    float			SinPos2;								// Position in der SinusListe für den Wasserhintergrund

    bool							bScrollBackground;		// Hintegrundbild scrollen ?

    //DKS - Lightmap code in original game was never used and all related code has now been disabled:
    //LevelTileSaveStruct				OriginalTiles[MAX_LEVELSIZE_X]  // Array mit Leveldaten merken (für Lightmaps)
    //[MAX_LEVELSIZE_Y];

    DirectGraphicsSprite			TileGfx[MAX_TILESETS];	// Tilegrafiken
    DirectGraphicsSprite			Background;				// Hintergrund
    DirectGraphicsSprite			ParallaxLayer[6];		// Anzahl der Layer
    DirectGraphicsSprite			CloudLayer;				// Wolkenlayer
    DirectGraphicsSprite			GameOver;				// GameOver Symbol
    DirectGraphicsSprite			Wasserfall[2];			// Wasserfall Grafiken
    DirectGraphicsSprite			LiquidGfx[2];				// Flüssigkeit
    float							WasserU[9];				// vorberechnete TexturKoordinaten für das Wasser TU
    float							WasserV[9];				// vorberechnete TexturKoordinaten für das Wasser TV
    DirectGraphicsSprite			Shadow;					// Schatten im Alien Level
    float							WasserfallOffset;		// Wasserfall Offset
    float							XOffset, YOffset;		// Scrolloffset des Levels
    float							NewXOffset, NewYOffset; // Neue Scrolloffsets, falls das Level von einem Gegner gelockt ist
    double							Timelimit;				// Zeitlimit
    double							TimelimitSave;			// usprüngliches Zeitlimit
    int								LEVELSIZE_X;			// Grösse des Levels
    int								LEVELSIZE_Y;			// in Tiles
    float							LEVELPIXELSIZE_X;		// Levelgrösse in Pixeln
    float							LEVELPIXELSIZE_Y;		// (für XOffset und YOffset)

    CDragonHack	    *pDragonHack;


    // Vorberechnung fürs Levelrendern
    int 							RenderPosX;				// Bereich des Levels, der gerendert wird
    int 							RenderPosY;
    int 							RenderPosXTo;
    int 							RenderPosYTo;
    int								xLevel;
    int								yLevel;
    int								xTileOffs;
    int								yTileOffs;
    float							xScreen;
    float							yScreen;

    TileEngineClass(void);									// Konstruktor
    ~TileEngineClass(void);									// Destruktor

    //DKS - Added initialization function that will load the sprites.
    //      This was necessary since making TileEngineClass a global
    //      static in Main.cpp instead of a dyanmically-allocated pointer.
    //      The class constructor therefore should never load sprites by
    //      itself, since graphics system should be initialized first.
    void LoadSprites();

    void ClearLevel(void);									// Level freigeben
    bool LoadLevel(char Filename[100]);						// Level laden
    void InitNewLevel(int xSize, int ySize);				// Neues Level initialisieren
    void SetScrollSpeed(float xSpeed, float ySpeed);		// neue Scrollspeed setzen
    void CalcRenderRange		(void);						// Bereiche berechnen, die gerendert werden sollen
    void DrawBackground			(void);							// Hintergrund Layer zeichnen
    void DrawBackLevel			(void);							// Level hintergrund anzeigen
    void DrawFrontLevel			(void);							// Level vordergrund anzeigen
    void DrawBackLevelOverlay	(void);							// Boden Tiles, die verdecken
    void DrawOverlayLevel		(void);							// Sonstige, die verdecken
    //DKS - this was an empty function in the original code, disabling it:
    //void DrawSpecialLayer		(void);							// Spezielle Sachen wie der Drache
    void DrawWater				(void);							// Wasser Planes rendern
    void CheckBounds			(void);
    void UpdateLevel			(void);							// Level evtl scrollen usw
    void ScrollLevel		(float x,  float y,  int neu,
                             float sx = 10.0f, float sy = 10.0f);// Screen scrollen
    void MakeBordersLookCool	(int x, int y);

    //DKS - These were all a mess and I rewrote them. See new comments in Tileengine.cpp
#if 0
    int	 BlockRechts		(float &x, float &y, float &xo, float &yo, RECT rect, bool resolve = false);
    int	 BlockLinks			(float &x, float &y, float &xo, float &yo, RECT rect, bool resolve = false);
    int	 BlockOben			(float &x, float &y, float &xo, float &yo, RECT rect, bool resolve = false);
    int	 BlockUnten			(float &x, float &y, float &xo, float &yo, RECT rect, bool resolve = false);
    int	 BlockUntenNormal	(float &x, float &y, float &xo, float &yo, RECT rect);
    //DKS - ResolveLinks() was never used anywhere, disabled and no replacement was made:
    void ResolveLinks		(float &x, float &y, float &xo, float &yo, RECT rect);
    bool BlockDestroyRechts		(float &x, float &y, float &xo, float &yo, RECT rect);
    bool BlockDestroyLinks		(float &x, float &y, float &xo, float &yo, RECT rect);
    bool BlockDestroyOben		(float &x, float &y, float &xo, float &yo, RECT rect);
    bool BlockDestroyUnten		(float &x, float &y, float &xo, float &yo, RECT rect);
#endif //0
    uint32_t BlockRechts        (float &x, float y, float &xo, float yo, RECT rect, bool resolve = false);
    uint32_t BlockLinks         (float &x, float y, float &xo, float yo, RECT rect, bool resolve = false);
    uint32_t BlockOben          (float x, float &y, float xo, float &yo, RECT rect, bool resolve = false);
    uint32_t BlockUnten         (float x, float &y, float xo, float &yo, RECT rect, bool resolve = false);
    uint32_t BlockUntenNormal   (float x, float y, float xo, float yo, RECT rect);
    bool BlockDestroyRechts     (float x, float y, float xo, float yo, RECT rect);
    bool BlockDestroyLinks      (float x, float y, float xo, float yo, RECT rect);
    bool BlockDestroyOben       (float x, float y, float xo, float yo, RECT rect);
    bool BlockDestroyUnten      (float x, float y, float xo, float yo, RECT rect);

    //DKS - Rewrote this function, see notes in Tileengine.cpp
    //int	 BlockSlopes		(float &x, float &y, float &xo, float &yo, RECT rect, float ySpeed, bool resolve = false);
    uint32_t BlockSlopes        (const float x, float &y, const RECT rect, const float ySpeed);

    //DKS - x,y parameters did not need to be references and are now value params:
    bool CheckDestroyableWalls(float x, float y,			// Schuss auf Zerstörbare
                               float xs, float ys,			// Wände testen
                               RECT rect);

    void ExplodeWall (int x, int y);						// Wand an x/y explodieren lassen
    void ExplodeWalls(int x, int y);						// Wand an x/y und alle angrenzenden Wände
    // explodieren lassen

    D3DCOLOR LightValue(float x, float y, RECT rect, bool forced);	// Helligkeit an Stelle x/y

    void ComputeCoolLight (void);							// Coole   Lightberechnung

    //DKS - This function was never actually used in the original game and is now disabled:
    //      (Only place was as a console command in Console.cpp)
    //void ComputeShitLight (void);							// Billige Lightberechnung

    void DrawShadow		  (void);							// Schatten im Alien Level zeichnen


    //DKS - The original code had a return as line 5 of DrawLightmap, making it a stub.
    //      When I tried removing it, I saw why: the lightmaps colors' don't match what
    //      you'd expect and they appear in places off-center from where they should be.
    //      I have now disabled the function and all four calls to it elsewhere in the code,
    //      and all related code.
    //void ClearLightMaps	  (void);							// alle LightMaps entfernen und Ursprungshelligkeit wieder herstellen
    //void DrawLightmap	  (int Map, float x, float y, int alpha);	// LightMap an x/y mit alpha addieren

    void WertAngleichen	  (float &nachx, float &nachy, float vonx, float vony);
    bool LineHitsLine(const Vector2D p,
                      const Vector2D u,
                      const Vector2D q,
                      const Vector2D v,
                      Vector2D &pHit);
    
    //DKS - Added bounds-checked accessor for Tiles[][] array for debugging purposes:
    LevelTileStruct& TileAt(const int i, const int j)
    {
#ifdef _DEBUG
        if (i >= MAX_LEVELSIZE_X || i < 0 || j >= MAX_LEVELSIZE_Y || j < 0) {
            Protokoll.WriteText( true, "-> Error: Out of bounds in TileEngineClass::TileAt():\n"
                    "\tparam i: %d\tLower bound: %d\tUpper bound: %d\n"
                    "\tparam j: %d\tLower bound: %d\tUpper bound: %d\n",
                    i, 0, MAX_LEVELSIZE_X-1, j, 0, MAX_LEVELSIZE_Y-1 );
            exit(1);    //WriteText above should do this for us (first param==true)
        }
#endif
        return Tiles[i][j];
    }   
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern	TileEngineClass		TileEngine;
extern	D3DCOLOR			Col1, Col2, Col3;
extern  bool				DrawDragon;
extern  float				ShadowAlpha;

#endif

