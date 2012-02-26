// Datei : Main.cpp

// -------------------------------------------------------------------------------------- 
//
// Hurrican Level-Editor
// 
// benutzt die DirectX8.1 API für Grafik und Input
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
//
// Hinweise zum Editor
//
// --------------------------------------------------------------------------------------

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

#define	LEVELMODE					0				// Level   editieren
#define OBJECTMODE					1				// Objekte editieren
#define SMALLLIGHTMODE				2				// Kleines Licht zum Level
#define MEDIUMLIGHTMODE				3				// Medium  Licht zum Level
#define LARGELIGHTMODE				4				// Großes  Licht zum Level
#define SMALLSHADOWMODE				5				// Kleines Schatten vom Level abziehen
#define MEDIUMSHADOWMODE			6				// Medium  Schatten vom Level abziehen
#define LARGESHADOWMODE				7				// Großen  Schatten vom Level abziehen

#define EDIT_ID_LevelName			100
#define EDIT_ID_Beschreibung		101
#define EDIT_ID_SizeX				102
#define EDIT_ID_SizeY				103
#define EDIT_ID_Value1				104
#define EDIT_ID_Value2				105
#define EDIT_ID_Timelimit			106
#define EDIT_ID_Color1				107
#define EDIT_ID_Color2				108

#define BOX_ID_SelectTileset		201
#define BOX_ID_SelectBackground		202
#define BOX_ID_SelectParallaxA		203
#define BOX_ID_SelectParallaxB		204
#define BOX_ID_SelectClouds			205
#define BOX_ID_SelectStageMusic		206
#define BOX_ID_SelectBossMusic		207
#define BOX_ID_SelectPowerblock		208


#define BUTTON_ID_NextTileSet		210
#define BUTTON_ID_PreviousTileSet	211
#define BUTTON_ID_EditMode			212

#define BUTTON_ID_SmallLight		213
#define BUTTON_ID_MediumLight		214
#define BUTTON_ID_LargeLight		215

#define BUTTON_ID_SmallShadow		223
#define BUTTON_ID_MediumShadow		224
#define BUTTON_ID_LargeShadow		225

#define BUTTON_ID_ScrollBackground	300
#define BUTTON_ID_ShowBackground	301
#define BUTTON_ID_ShowLevel			302
#define BUTTON_ID_ShowOverlay		303
#define BUTTON_ID_ShowLight			304
#define BUTTON_ID_ShowBlock			305
#define BUTTON_ID_ShowObjects		306
#define BUTTON_ID_ShowGrid			307
#define BUTTON_ID_KeepTileOffset	308
#define BUTTON_ID_Taschenlampe		309
#define BUTTON_ID_ChangeLight		310

#define BUTTON_ID_CopyBack			311
#define BUTTON_ID_CopyFront			312
#define BUTTON_ID_CopyLight			313
#define BUTTON_ID_CopyBlock			314

#define BUTTON_ID_LightRed			315
#define BUTTON_ID_LightGreen		316
#define BUTTON_ID_LightBlue			317

#define BUTTON_ID_HScrollBar		401
#define BUTTON_ID_VScrollBar		402

#define BUTTON_ID_RedValue			500
#define BUTTON_ID_GreenValue		501
#define BUTTON_ID_BlueValue			502
#define BUTTON_ID_AlphaValue		503
#define BUTTON_ID_AllValues			504

#define BUTTON_ID_Bruecke			505
#define BUTTON_ID_TestLevel			506
#define BUTTON_ID_CopyFromBlock		507

#define BOX_ID_ObjectType				600
#define BOX_ID_BlockWert_Wand			601
#define BOX_ID_BlockWert_GegnerWand		602
#define BOX_ID_BlockWert_Plattform		603
#define BOX_ID_BlockWert_Light			604
#define BOX_ID_BlockWert_Verdecken		605
#define BOX_ID_BlockWert_AnimiertB		606
#define BOX_ID_BlockWert_AnimiertF		607
#define BOX_ID_BlockWert_Wasser			608
#define BOX_ID_BlockWert_Schaden		609
#define BOX_ID_BlockWert_FliessBandL	610
#define BOX_ID_BlockWert_FliessBandR	611
#define BOX_ID_BlockWert_WendePunkt		612
#define BOX_ID_BlockWert_Destructible	613
#define BOX_ID_BlockWert_MOVELINKS			614
#define BOX_ID_BlockWert_Overlay_Light	615
#define BOX_ID_BlockWert_Lava			616
#define BOX_ID_BlockWert_Eis			617
#define BOX_ID_BLOCKWERT_MOVEVERTICAL			618
#define BOX_ID_BlockWert_WasserFall		619
#define BOX_ID_BlockWert_TexRechts		620

#define GROUPBOX_ID_Farbe			700
#define GROUPBOX_ID_Tileset			701
#define GROUPBOX_ID_Block			702
#define GROUPBOX_ID_Objekt			703
#define GROUPBOX_ID_EditMode		704
#define GROUPBOX_ID_LevelInfo		705
#define GROUPBOX_ID_CopyInfo		706
#define GROUPBOX_ID_LightMode		707
#define GROUPBOX_ID_Backgrounds		708
#define GROUPBOX_ID_MiniMap			709
#define GROUPBOX_ID_Misc			710

#define RADIOBUTTON_ID_Easy			800
#define RADIOBUTTON_ID_Medium		801
#define RADIOBUTTON_ID_Hard			802

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include <windows.h>									// Alle Windows Header includen
#include <stdlib.h>
#include <direct.h>
#include "stdio.h"
#include "Main.h"
#include "Globals.h"
#include "Logdatei.h"
#include "DX8Graphics.h"
#include "DX8Sprite.h"
#include "DX8Input.h"
#include "DX8Sound.h"
#include "DX8Font.h"
#include "Tileengine.h"
#include "MainMenu.h"
#include <Winuser.h>

// --------------------------------------------------------------------------------------
// externe Variablen
// --------------------------------------------------------------------------------------

extern char						 TastaturPuffer[256];				// Tastaturpuffer des Keyboards

// --------------------------------------------------------------------------------------
// globale Variablen
// --------------------------------------------------------------------------------------

// Arrays für die Licht/Schatten Effekte

signed int	LightMap[6][9][9] =	   {{{ 0,  0,  0,  0,  0,  0,  0,  0,  0},		// Small Light
									{  0,  0,  0,  2,  2,  2,  0,  0,  0},
									{  0,  0,  2,  8, 12,  8,  2,  0,  0},
									{  0,  2,  8, 16, 20, 16,  8,  2,  0},
									{  0,  2, 12, 20, 24, 20, 12,  2,  0},
									{  0,  2,  8, 16, 20, 16,  8,  2,  0},
									{  0,  0,  2,  8, 12,  8,  2,  0,  0},
									{  0,  0,  0,  2,  2,  2,  0,  0,  0},
									{  0,  0,  0,  0,  0,  0,  0,  0,  0}},
								
									{{ 0,  0,  0,  0,  0,  0,  0,  0,  0},		// Medium Light
									{  0,  0,  2,  3,  6,  3,  2,  0,  0},
									{  0,  2,  6, 12, 16, 12,  6,  2,  0},
									{  0,  3, 12, 20, 26, 20, 12,  3,  0},
									{  0,  6, 16, 26, 32, 26, 16,  6,  0},
									{  0,  3, 12, 20, 26, 20, 12,  3,  0},
									{  0,  2,  6, 12, 16, 12,  6,  2,  0},
									{  0,  0,  2,  3,  6,  3,  2,  0,  0},
									{  0,  0,  0,  0,  0,  0,  0,  0,  0}},
								 
									{{ 0,  0,  0,  0,  0,  0,  0,  0,  0},		// Large Light
									{  0,  0,  4,  6, 12,  6,  4,  0,  0},
									{  0,  4, 12, 24, 32, 24, 32,  4,  0},
									{  0,  6, 24, 40, 52, 40, 32,  6,  0},
									{  0, 12, 32, 52, 64, 52, 32, 12,  0},
									{  0,  6, 24, 40, 52, 40, 32,  6,  0},
									{  0,  4, 12, 24, 32, 24, 32,  4,  0},
									{  0,  0,  4,  6, 12,  6,  4,  0,  0},
									{  0,  0,  0,  0,  0,  0,  0,  0,  0}},

									{{ 0,-  0,-  0,-  0,-  0,-  0,-  0,-  0,-  0}, 		// Small Shadow
									{  0,-  0,-  0,-  2,-  2,-  2,-  0,-  0,-  0}, 
									{  0,-  0,-  2,-  8,- 12,-  8,-  2,-  0,-  0}, 
									{  0,-  2,-  8,- 16,- 20,- 16,-  8,-  2,-  0}, 
									{  0,-  2,- 12,- 20,- 24,- 20,- 12,-  2,-  0}, 
									{  0,-  2,-  8,- 16,- 20,- 16,-  8,-  2,-  0}, 
									{  0,-  0,-  2,-  8,- 12,-  8,-  2,-  0,-  0}, 
									{  0,-  0,-  0,-  2,-  2,-  2,-  0,-  0,-  0}, 
									{  0,-  0,-  0,-  0,-  0,-  0,-  0,-  0,-  0}}, 
								
									{{ 0,-  0,-  0,-  0,-  0,-  0,-  0,-  0,-  0}, 		// Medium Shadow
									{  0,-  0,-  2,-  3,-  6,-  3,-  2,-  0,-  0}, 
									{  0,-  2,-  6,- 12,- 16,- 12,-  6,-  2,-  0}, 
									{  0,-  3,- 12,- 20,- 26,- 20,- 12,-  3,-  0}, 
									{  0,-  6,- 16,- 26,- 32,- 26,- 16,-  6,-  0}, 
									{  0,-  3,- 12,- 20,- 26,- 20,- 12,-  3,-  0}, 
									{  0,-  2,-  6,- 12,- 16,- 12,-  6,-  2,-  0}, 
									{  0,-  0,-  2,-  3,-  6,-  3,-  2,-  0,-  0}, 
									{  0,-  0,-  0,-  0,-  0,-  0,-  0,-  0,-  0}}, 
								 
									{{ 0,-  0,-  0,-  0,-  0,-  0,-  0,-  0,-  0}, 		// Large Shadow
									{  0,-  0,-  4,-  6,- 12,-  6,-  4,-  0,-  0}, 
									{  0,-  4,- 12,- 24,- 32,- 24,- 32,-  4,-  0}, 
									{  0,-  6,- 24,- 40,- 52,- 40,- 32,-  6,-  0}, 
									{  0,- 12,- 32,- 52,- 64,- 52,- 32,- 12,-  0}, 
									{  0,-  6,- 24,- 40,- 52,- 40,- 32,-  6,-  0}, 
									{  0,-  4,- 12,- 24,- 32,- 24,- 32,-  4,-  0}, 
									{  0,-  0,-  4,-  6,- 12,-  6,-  4,-  0,-  0},
									{  0,-  0,-  0,-  0,-  0,-  0,-  0,-  0,-  0}}
};

//----- Handles der Buttons und Editor-Elemente

HWND					EditorRahmen			= NULL;

HWND					GroupBoxLevelInfo		= NULL;
HWND					GroupBoxFarbe			= NULL;
HWND					GroupBoxTileset			= NULL;
HWND					GroupBoxBlock			= NULL;
HWND					GroupBoxObjekt			= NULL;
HWND					GroupBoxEditMode		= NULL;
HWND					GroupBoxCopyInfo		= NULL;
HWND					GroupBoxLightMode		= NULL;
HWND					GroupBoxBackgrounds		= NULL;
HWND					GroupBoxMiniMap			= NULL;
HWND					GroupBoxMisc			= NULL;

HWND					SmallLight				= NULL;
HWND					MediumLight				= NULL;
HWND					LargeLight				= NULL;

HWND					SmallShadow				= NULL;
HWND					MediumShadow			= NULL;
HWND					LargeShadow				= NULL;

HWND					LevelNamenFeld			= NULL;
HWND					BeschreibungsFeld		= NULL;
HWND					EditSizeX				= NULL;
HWND					EditSizeY				= NULL;
HWND					Value1Feld				= NULL;
HWND					Value2Feld				= NULL;
HWND					TimelimitFeld			= NULL;
HWND					Color1Feld				= NULL;
HWND					Color2Feld				= NULL;

HWND					ButtonBruecke			= NULL;
HWND					ButtonTestLevel			= NULL;
HWND					ButtonCopyFromBlock		= NULL;

HWND					ButtonEditMode			= NULL;
HWND					ComboBoxSelectTileset	= NULL;
HWND					ComboBoxSelectBackground= NULL;
HWND					ComboBoxSelectParallaxA = NULL;
HWND					ComboBoxSelectParallaxB = NULL;
HWND					ComboBoxSelectClouds	= NULL;
HWND					ComboBoxSelectStageMusic= NULL;
HWND					ComboBoxSelectBossMusic	= NULL;
HWND					ComboBoxSelectPowerblock= NULL;
HWND					NextTileSet				= NULL;
HWND					PreviousTileSet			= NULL;

HWND					BoxTaschenlampe			= NULL;
HWND					BoxScrollBackground		= NULL;
HWND					BoxKeepTileOffset		= NULL;
HWND					BoxShowBackground		= NULL;
HWND					BoxShowLevel			= NULL;
HWND					BoxShowOverlay			= NULL;
HWND					BoxShowLight			= NULL;
HWND					BoxShowBlock			= NULL;
HWND					BoxShowObjects			= NULL;
HWND					BoxShowGrid				= NULL;
HWND					BoxChangeLight			= NULL;

HWND					BoxLightRed 			= NULL;
HWND					BoxLightGreen 			= NULL;
HWND					BoxLightBlue 			= NULL;

HWND					BoxCopyBack				= NULL;
HWND					BoxCopyFront			= NULL;
HWND					BoxCopyLight			= NULL;
HWND					BoxCopyBlock			= NULL;

HWND					HScrollBar				= NULL;
HWND					VScrollBar				= NULL;

HWND					RedValue				= NULL;
HWND					GreenValue				= NULL;
HWND					BlueValue				= NULL;
HWND					AlphaValue				= NULL;
HWND					AllValues				= NULL;

HWND					BoxBlockWertWand		= NULL;
HWND					BoxBlockWertGegnerWand	= NULL;
HWND					BoxBlockWertPlattform	= NULL;
HWND					BoxBlockWertLight		= NULL;
HWND					BoxBlockWertVerdecken	= NULL;
HWND					BoxBlockWertAnimiertB	= NULL;
HWND					BoxBlockWertAnimiertF	= NULL;
HWND					BoxBlockWertWasser		= NULL;
HWND					BoxBlockWertSchaden		= NULL;
HWND					BoxBlockWertFliessBandL	= NULL;
HWND					BoxBlockWertFliessBandR	= NULL;
HWND					BoxBlockWertWendePunkt	= NULL;
HWND					BoxBlockWertDestructible= NULL;
HWND					BoxBlockWertSaeure      = NULL;
HWND					BoxBlockWertOverlay_Light = NULL;
HWND					BoxBlockWertLava		= NULL;
HWND					BoxBlockWertEis			= NULL;
HWND					BoxBlockWertMagen		= NULL;
HWND					BoxBlockWertWasserFall  = NULL;
HWND					BoxBlockWertTexRechts  = NULL;
HWND					ComboBoxObjectType		= NULL;

HWND					RadioButtonEasy			= NULL;
HWND					RadioButtonMedium		= NULL;
HWND					RadioButtonHard			= NULL;

HDC						hdc;

HWND					MainHWND				= NULL;		// Fenster Handle

HPEN					Pens[10];						// Pens für Linien
														// der Block-Arten

D3DFORMAT				D3DFormat;						// Format der Primary Surface
LPDIRECT3DDEVICE8		lpD3DDevice	 = NULL;			// Direct3D Device-Objekt
LPDIRECT3DSURFACE8		lpBackbuffer = NULL;			// Der Backbuffer
D3DCAPS8				d3dcaps;						// Möglichkeiten der Hardware

bool					EditorPaused	= false;		// Editor eingefroren (wenn man zb das Fenster verlässt)

bool					MouseInTile	    = false;		// Mauscursor im Tile ?
bool					MouseInLevel    = false;		// Mauscursor im Level ?
bool					EditorRunning   = true;			// Editor läuft :-)
bool					RedrawLevel     = true;			// Level neu zeichnen ?
bool					RedrawTileset   = true;			// Tileset neu zeichnen ?
bool					bShowLevel      = true;			
bool					bShowBackground = false;		// Was wird
bool					bShowOverlay    = true;			// zu Beginn
bool					bShowLight	    = true;			// zu Beginn
bool					bShowBlock	    = false;		// alles angezeigt ?
bool					bShowObjects    = true;			// Objekte zB
bool					bShowGrid		= false;		// oder das Grid
bool					bScrollBackground = false;		// Letzten Layer scrollen ?
bool					bTaschenlampe	= false;		// Taschenlampeneffekt
bool					bKeepTileOffset = false;		// Offset innerhalb des Rechtecks merken
int						KeepX = 0;						// Dazugehörige x und y Offsets
int						KeepY = 0;

bool					bCopyBack		= true;			// Back ins Clipboard ?
bool					bCopyFront		= false;		// Front ins Clipboard ?
bool					bCopyLight		= true;			// Light ins Clipboard ?
bool					bCopyBlock		= true;			// Block ins Clipboard ?

bool					bLightRed		= true;			// Rot Grün Blau beim
bool					bLightGreen		= true;			// Licht/Schatten hinzufügen
bool					bLightBlue		= true;			// berücksichtigen ?

HBRUSH					BlockBrush[10];
RECT					ActRect;						// Aktueller Ausschnitt

unsigned char			TilesetNummer[MAX_TILESETS];	// Nummern der Tileset in der Liste
unsigned char			LoadedTilesets  = 0;			// Wieviele Sets im Verzeichnis gefunden ?
unsigned char			ActualTileset   = 0;			// Gerade gewähltes Tileset
unsigned char			TilesetsUsed    = 0;
unsigned char			LightUsed		= 0;			// Welche Lichteinstellung ?

int						XOffset			= 0;			// Offset im Level
int						YOffset			= 0;
int						EditMode		= LEVELMODE;	// Edit-Modus
int						LevelSizeX		= 64;			// X-Grösse des Levels
int						LevelSizeY		= 48;			// Y-Grösse des Levels

DirectGraphicsClass		DirectGraphics;					// Grafik-Objekt
DirectInputClass		DirectInput;					// Input-Objekt
DirectSoundClass		DirectSound;					// Sound_Objekt
Logdatei				Protokoll("Logdatei.txt");		// Protokoll Datei
TileEngineClass			*pTileEngine;					// TileEngine
int						FensterMausX, FensterMausY;		// Maus-Pos im Fenster

Object					*pSelectedObject = new Object;	// aktuell ausgewähltes Objekt
unsigned int			NumObjects = 0;					// Anzahl aller Objekte

int						ClipX,    ClipY;				// Grösse des Clipboard-Ausschnitts
int						ClipOffX, ClipOffY;				// Linke obere Ecke des Clipboard-Ausschnitts
char					LevelName[100];					// Name des Levels
char					Beschreibung[100];				// Beschreibung des Levels
unsigned int			Timelimit;						// Zeitlimit des Levels
LevelTileStruct			ClipBoard[SCREENSIZE_X][SCREENSIZE_Y];	// Clipboard

DirectGraphicsSprite	BlockArten;						// Grafiken für Blockarten
DirectGraphicsSprite	ObjectGFX[201];					// Grafiken der Objekte
DirectGraphicsSprite	LiquidGfx[2];
char					Buffer[100];					// Für die häufige _itoa Umwandlung
char					HurriPath[255];					// Pfad, in dem Hurri liegt

FileHeader				BackgroundNames;				// Zum sichern der Hintergrundgrafiken
FileAppendix			Appendix;						// Level Anhang

int						SCREENBPP;

int DiamondsPerLevel = 0;
int LivesPerLevel = 0;

int CurrentUndoStep = 0;
bool UndoTriggered = false;

LevelTileStruct			UndoBuffer[MAX_UNDO_STEPS][SCREENSIZE_X][SCREENSIZE_X];
int						UndoOffsetX[MAX_UNDO_STEPS];
int						UndoOffsetY[MAX_UNDO_STEPS];

D3DCOLOR				Col1, Col2;

DirectGraphicsFont		*pFont        = new(DirectGraphicsFont);

// --------------------------------------------------------------------------------------
// Klasse für die Objekte
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

ObjectList::ObjectList (void)
{
    pStart = NULL;									// Zu Begin kein Objekt in der Liste
	pEnd   = NULL;						
}

// --------------------------------------------------------------------------------------
// Anzahl Objekte vom Typ "Type" zurückgeben
// --------------------------------------------------------------------------------------

int	ObjectList::GetNumType(int Type)
{
	int count = 0;
	Object		*pTemp = pStart;			// Zeiger auf das erste Objekt
	D3DCOLOR	Color;

    while (pTemp != NULL)					// Ende der Liste erreicht ?
    {
		if (pTemp->ObjectID == Type)
			count++;

        pTemp = pTemp->pNext;				// Zeiger auf das nächste Objekt richten    
    }

	return count;
}

// --------------------------------------------------------------------------------------
// Objekt in die Liste einfügen
// --------------------------------------------------------------------------------------

void ObjectList::Push  (int x, int y, unsigned int ObjID)
{
    Object *pNew = new Object;				// neues Objekt

	pNew->ChangeLight = false;
	pNew->ObjectID	  = ObjID;
	pNew->XPos		  = x;
	pNew->YPos		  = y;
	pNew->Skill		  = 0;					// Sicherheitshalber auf Easy setzen
	pNew->Value1	  = 0;
	pNew->Value2	  = 0;
	
	if(pStart==NULL)						// Liste leer ?
	{
		pStart = pNew;						// Ja, dann neues Objekt gleich erstes
		pEnd   = pNew;						// und letztes Objekt

		pStart->pNext=NULL;					// Next/Previous gibts nich, da wir
		pStart->pPrev=NULL;					// nur 1 Objekt haben
	} 
	else									// Liste ist NICHT leer
	{
		pEnd->pNext = pNew;					// Letztes Objekt zeigt auf den neuen
		pNew->pPrev = pEnd;					// Letztes Objekt ist nicht mehr das letzte
		
		pNew->pNext = NULL;					// Nach dem neuen Objekt kommt keiner mehr
		pEnd		= pNew;					// da es jetzt das letzte in der Liste ist
	}

	NumObjects++;							// Objekt Anzahl erhöhen
}

// --------------------------------------------------------------------------------------
// Objekt in die Liste einfügen und Werte aus geladenem Objekt übernehmen
// --------------------------------------------------------------------------------------

void ObjectList::Push(ObjectForFileAccess Temp)
{
    Object *pNew = new Object;				// neues Objekt

	pNew->ChangeLight	= Temp.ChangeLight;
	pNew->ObjectID		= Temp.ObjectID;
	pNew->XPos			= Temp.XPos;
	pNew->YPos			= Temp.YPos;
	pNew->Skill			= Temp.Skill;
	pNew->Value1	    = Temp.Value1;
	pNew->Value2	    = Temp.Value2;
	
	if(pStart==NULL)						// Liste leer ?
	{
		pStart = pNew;						// Ja, dann neues Objekt gleich erstes
		pEnd   = pNew;						// und letztes Objekt

		pStart->pNext=NULL;					// Next/Previous gibts nich, da wir
		pStart->pPrev=NULL;					// nur 1 Objekt haben
	} 
	else									// Liste ist NICHT leer
	{
		pEnd->pNext = pNew;					// Letztes Objekt zeigt auf den neuen
		pNew->pPrev = pEnd;					// Letztes Objekt ist nicht mehr das letzte
		
		pNew->pNext = NULL;					// Nach dem neuen Objekt kommt keiner mehr
		pEnd		= pNew;					// da es jetzt das letzte in der Liste ist
	}

	NumObjects++;							// Objekt Anzahl erhöhen
}

// --------------------------------------------------------------------------------------
// Objekt aus der Liste entfernen
// --------------------------------------------------------------------------------------

void ObjectList::DelSel  (Object *pTemp)
{
	Object  *pN;
    Object  *pP;

	if(pTemp!=NULL)						// zu löschendes Objekt existiert 
	{
		pN = pTemp->pNext;			
		pP = pTemp->pPrev;
  
		if(pP == NULL)					// Wird das erste Objekt gelöscht ?
			pStart = pN;				// Dann wird dessen Nächstes zum Ersten
		else
			pP->pNext = pN;	   	        // ansonsten normal eins aufrücken

		if(pN == NULL)					// Wird das letzte Objekt gelöscht ?
			pEnd = pP;					// Dann wir das letzte Objekt zum ersten
		else
		pN->pPrev = pP;
  
		delete (pTemp);					// Speicher freigeben
		pTemp = NULL;

		NumObjects--;					// Gegneranzahl verringern
	}
}

// --------------------------------------------------------------------------------------
// alle Objekte (sofern sichtbar) anzeigen
// --------------------------------------------------------------------------------------

void ObjectList::ShowAll (void)
{
    Object		*pTemp = pStart;			// Zeiger auf das erste Objekt
	D3DCOLOR	Color;

	DiamondsPerLevel = 0;
	LivesPerLevel = 0;

    while (pTemp != NULL)					// Ende der Liste erreicht ?
    {
		if (pTemp->ObjectID == 1)
			LivesPerLevel++;

		if (pTemp->ObjectID == 2)
			DiamondsPerLevel++;

		if(pTemp->XPos < XOffset*20+640		// Objekt überhaupt auf dem Screen ?
		&& pTemp->YPos < YOffset*20+480
		&& pTemp->XPos > XOffset*20
		&& pTemp->YPos > YOffset*20)

		if (pTemp->ObjectID != 3 &&
			pTemp->ObjectID != 53)
		ObjectGFX[pTemp->ObjectID].SetRect(0, 0, 
										   ObjectGFX[pTemp->ObjectID].itsXFrameSize,
										   ObjectGFX[pTemp->ObjectID].itsYFrameSize);
		else
		if (pTemp->ObjectID == 3)
			ObjectGFX[pTemp->ObjectID].SetRect(ObjectGFX[pTemp->ObjectID].itsXFrameSize * (Appendix.UsedPowerblock * 2 + 1),
											   0, 
											   ObjectGFX[pTemp->ObjectID].itsXFrameSize * (Appendix.UsedPowerblock * 2 + 2), 
											   ObjectGFX[pTemp->ObjectID].itsXFrameSize);
		else
		ObjectGFX[pTemp->ObjectID].SetRect(ObjectGFX[pTemp->ObjectID].itsXFrameSize * 5,
										   ObjectGFX[pTemp->ObjectID].itsYFrameSize * 1,
										   ObjectGFX[pTemp->ObjectID].itsXFrameSize * 6, 
										   ObjectGFX[pTemp->ObjectID].itsYFrameSize * 2);

		// Objekt anzeigen (momentan ausgewähltes Objekt ist ROT)
		if(pTemp != pSelectedObject)
		{
			DirectGraphics.SetColorKeyMode();

			// Normal anzeigen ?
			if (pTemp->ChangeLight == false)	 
				Color = 0xFFFFFFFF;

			// Mit Farbe des Hinteren Level-Tiles anzeigen ?
			else
			{
				int x,y;

				// Level Koordinaten der Objekt Mitte errechnen
				x = (int)((pTemp->XPos + (ObjectGFX[pTemp->ObjectID].itsXFrameSize) / 2) /20);
				y = (int)((pTemp->YPos + (ObjectGFX[pTemp->ObjectID].itsYFrameSize) / 2) /20);

				Color = D3DCOLOR_RGBA(pTileEngine->Tiles[x][y].Red, 
									  pTileEngine->Tiles[x][y].Green, 
									  pTileEngine->Tiles[x][y].Blue, 255);
			}

			// Objekt anzeigen
			// kein Auge und keine Feuerfalle
			if ((pTemp->ObjectID != 143) &&
				(pTemp->ObjectID != 18 ||
				 pTemp->Value2 != 1))
				ObjectGFX[pTemp->ObjectID].RenderSprite((float)(pTemp->XPos-XOffset*20),     
														(float)(pTemp->YPos-YOffset*20),
														Color);
			// Auge kriegt ne Extra wurst (weil es auch ein Extra sein kann)
			else
			if (pTemp->ObjectID == 18)
			{
				ObjectGFX[200].RenderSprite((float)(pTemp->XPos-XOffset*20),     
											(float)(pTemp->YPos-YOffset*20),
											pTemp->Value1, Color);
			}			
			else
			if (pTemp->ObjectID == 143)
			{
				ObjectGFX[pTemp->ObjectID].RenderSprite((float)(pTemp->XPos-XOffset*20),     
											(float)(pTemp->YPos-YOffset*20),
											pTemp->Value1, Color);
			}			

			// Bei Secret Triggern noch den Bereich rendern
			if (pTemp->ObjectID == 171)
			{
				RenderRect((float)(pTemp->XPos-XOffset*20) + 20 - pTemp->Value1 / 2.0f,
						   (float)(pTemp->YPos-YOffset*20) + 20 - pTemp->Value2 / 2.0f,
						   pTemp->Value1, pTemp->Value2, 0x40FF0000);
			}
		}

		// Value1 / Value2 anzeigen
		if (pTemp->Value1 != 0 ||
			pTemp->Value2 != 0)
		{
			char buf[128];
			sprintf(buf, "%d/%d", pTemp->Value1, pTemp->Value2);
			pFont->DrawText((float)(pTemp->XPos-XOffset*20),     
							(float)(pTemp->YPos-YOffset*20), buf, 0xFFFFFFFF);			
		}

	    
        pTemp = pTemp->pNext;				// Zeiger auf das nächste Objekt richten    
    }

	// Gewähltes Objekt anzeigen
	if(pSelectedObject != NULL)
	{
		Color = 0xFFFFFFFF;
		DirectGraphics.SetAdditiveMode();

		if ((pSelectedObject->ObjectID != 143) &&
			(pSelectedObject->ObjectID != 18 ||
			 pSelectedObject->Value2 != 1))
		{
			ObjectGFX[pSelectedObject->ObjectID].RenderSprite((float)(pSelectedObject->XPos-XOffset*20),     
															  (float)(pSelectedObject->YPos-YOffset*20),     
															  Color);
			ObjectGFX[pSelectedObject->ObjectID].RenderSprite((float)(pSelectedObject->XPos-XOffset*20),     
															  (float)(pSelectedObject->YPos-YOffset*20),     
															  Color);
		}
		else
		if (pSelectedObject->ObjectID == 18)
		{
			ObjectGFX[200].RenderSprite((float)(pSelectedObject->XPos-XOffset*20),     
										(float)(pSelectedObject->YPos-YOffset*20),
										pSelectedObject->Value1, Color);

			ObjectGFX[200].RenderSprite((float)(pSelectedObject->XPos-XOffset*20),     
										(float)(pSelectedObject->YPos-YOffset*20),
										pSelectedObject->Value1, Color);
		}

		else
		if (pSelectedObject->ObjectID == 143)
		{
			ObjectGFX[pSelectedObject->ObjectID].RenderSprite((float)(pSelectedObject->XPos-XOffset*20),     
										(float)(pSelectedObject->YPos-YOffset*20),
										pSelectedObject->Value1, Color);

			ObjectGFX[pSelectedObject->ObjectID].RenderSprite((float)(pSelectedObject->XPos-XOffset*20),     
										(float)(pSelectedObject->YPos-YOffset*20),
										pSelectedObject->Value1, Color);
		}

		// Bei Secret Triggern noch den Bereich rendern
		if (pSelectedObject->ObjectID == 171)
		{
			RenderRect((float)(pSelectedObject->XPos-XOffset*20) + 20 - pSelectedObject->Value1 / 2.0f,
					   (float)(pSelectedObject->YPos-YOffset*20) + 20 - pSelectedObject->Value2 / 2.0f,
					   pSelectedObject->Value1, pSelectedObject->Value2, 0x40FF0000);
		}
	}
}

// --------------------------------------------------------------------------------------
// Alle Objekte der Liste löschen
// --------------------------------------------------------------------------------------

void ObjectList::ClearAll (void)
{
    Object *pTemp    = pStart;				// Zeiger auf das erste Objekt
	Object *pNaechst;						// Zeiger auf das nächste Objekt (falls
											// das eine gelöscht wird)
    while (pTemp != NULL)					// Ende der Liste erreicht ?
    {
		pNaechst = pTemp->pNext;			// Zeiger auf das nächste Element
		DelSel(pTemp);						// Das aktuelle löschen
        pTemp = pNaechst;				    // und das nächste bearbeiten
    }

	pStart = NULL;
	pEnd   = NULL;
}

// --------------------------------------------------------------------------------------
// Testen, welches Objekt mit der Maus angeklickt wurde
// --------------------------------------------------------------------------------------

void ObjectList::GetSelected (void)
{
	POINT  ptCursor;
    Object *pTemp    = pStart;				// Zeiger auf das erste Objekt

	pSelectedObject = NULL;

    while (pTemp != NULL)					// Ende der Liste erreicht ?
    {	
		GetCursorPos(&ptCursor);
		ScreenToClient(MainHWND, &ptCursor);

		// Gegner ausgewählt ?
		int xMaus = ptCursor.x - VIEW_XOFFSET;
		int yMaus = ptCursor.y - VIEW_YOFFSET;

		if((xMaus>=pTemp->XPos-XOffset*20)			
		 &&(xMaus< pTemp->XPos-XOffset*20+ObjectGFX[pTemp->ObjectID].itsXFrameSize)
		 &&(yMaus>=pTemp->YPos-YOffset*20)			
		 &&(yMaus< pTemp->YPos-YOffset*20+ObjectGFX[pTemp->ObjectID].itsYFrameSize))
		{
			pSelectedObject = pTemp;
			pTemp = NULL;
		} 
		else
		pTemp = pTemp->pNext;
	}
}

//----- doppelt verkettete Liste für die Objekte

ObjectList *pObjectList	= new ObjectList;		

// --------------------------------------------------------------------------------------
// Callback Funktion
// --------------------------------------------------------------------------------------

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;

	switch(message)
	{

//----- Fenster verlassen und wieder fokussiert ?

		case WM_ACTIVATE:
		{
			int Active = LOWORD(wparam);           // activation flag 

			if (Active == WA_INACTIVE)
				EditorPaused = true;
			else
			{
				EditorPaused = false;				
				DirectInput.AcquireKeyboard();
				DirectInput.AcquireMaus();	

				RedrawWindow(MainHWND, NULL, NULL, NULL);			// Fenster anzeigen (sicher ist sicher)
				UpdateWindow(MainHWND);								// Fenster-infos updaten
			}			

			RedrawLevel = true;
			RedrawTileset = true;
		} break;

//----- Fenster schliessen

		case WM_DESTROY:								// Fenster wird geschlossen
		{
			EditorRunning = false;						// Spiel beenden
			PostQuitMessage(0);							// Quit-Message posten
			return(0);									// Success zurückliefern
		} break;

		case WM_KEYDOWN:
		{
		} break;

//----- Maus-Funktionen

		case WM_MOUSEMOVE:
		{
		} break;

//----- Paint-Funktion

		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd,&ps);					// simply validate the window
														// you would do all your painting here
			EndPaint(hwnd,&ps);
			return(0);
		} break;

//----- Buttons abfragen

		case WM_COMMAND:
		{
			switch(LOWORD(wparam))
			{

//----- Menupunkt Datei
				case MENU_DATEI_ID_SPEICHERN:				// Level speichern
				{
					SaveLevel(false);
					RedrawLevel   = true;
					RedrawTileset = true;
				} break;

				case MENU_DATEI_ID_SPEICHERN_UNTER:		// Level speichern unter
				{
					SaveLevel(true);
					RedrawLevel   = true;
					RedrawTileset = true;
				} break;

				case MENU_DATEI_ID_LADEN:					// Level laden
				{
					LoadLevel();
					RedrawLevel   = true;
					RedrawTileset = true;
				} break;

				case MENU_DATEI_ID_BEENDEN:				// Beenden Button im MainMenu
				{
					if(MessageBox (NULL, "Nicht gespeicherte Daten gehen verloren !\nWirklich beenden ?", 
						"Editor Beenden", MB_YESNO | MB_ICONINFORMATION) == IDYES)
						EditorRunning = false;
				} break;

//----- Menupunkt Hilfe
				case MENU_HILFE_ID_ABOUT:				// About Button im Hilfe Menu
				{
					MessageBox (NULL, "by Eiswuxe\n(c) 2002-2007 Poke53280\n\nhttp://www.poke53280.de", 
								"Hurrican Level Editor V1.0", MB_OK | MB_ICONINFORMATION);
				} break;

				case MENU_HILFE_ID_TASTENBELEGUNG:		// Tastenbelegung im Hilfe Menu
				{
					// Hilfe html-Site anzeigen
					ShellExecute(MainHWND, NULL,  "Help\\Anleitung.html",NULL,NULL, SW_SHOWNORMAL);
				} break;


//----- Sonstige Buttons 

				case BUTTON_ID_SmallLight:				// Kleines Licht wählen
				{
					LightUsed = 0;
					EditMode = SMALLLIGHTMODE;
					ClipX = -1;
					SendMessage(ButtonEditMode,WM_SETTEXT,0,(long)"Small Light");
				} break;

				case BUTTON_ID_MediumLight:				// Mittleres Licht wählen
				{
					LightUsed = 1;
					EditMode = MEDIUMLIGHTMODE;
					ClipX = -1;
					SendMessage(ButtonEditMode,WM_SETTEXT,0,(long)"Med. Light");
				} break;

				case BUTTON_ID_LargeLight:				// Großes Licht wählen
				{
					LightUsed = 2;
					EditMode = LARGELIGHTMODE;
					ClipX = -1;
					SendMessage(ButtonEditMode,WM_SETTEXT,0,(long)"Large Light");
				} break;


				case BUTTON_ID_SmallShadow:				// Kleiner Schatten
				{
					LightUsed = 3;
					EditMode = SMALLSHADOWMODE;
					ClipX = -1;
					SendMessage(ButtonEditMode,WM_SETTEXT,0,(long)"Small Shadow");
				} break;

				case BUTTON_ID_MediumShadow:			// Mittlerer Schatten
				{
					LightUsed = 4;
					EditMode = MEDIUMSHADOWMODE;
					ClipX = -1;
					SendMessage(ButtonEditMode,WM_SETTEXT,0,(long)"Med. Shadow");
				} break;

				case BUTTON_ID_LargeShadow:			// Mittlerer Schatten
				{
					LightUsed = 5;
					EditMode = LARGESHADOWMODE;
					ClipX = -1;
					SendMessage(ButtonEditMode,WM_SETTEXT,0,(long)"Large Shadow");
				} break;

				case BUTTON_ID_Bruecke:				// Brücke bauen
				{
					// Brückenteile anfügen
					for (int i = 0; i < ClipX; i++)
					{
						pObjectList->Push (ActRect.left*20+XOffset*20+i*20, 
										   ActRect.top*20+YOffset*20 - 3, 149);

						pObjectList->Push (ActRect.left*20+XOffset*20+i*20+10, 
										   ActRect.top*20+YOffset*20 - 3, 149);

						pTileEngine->Tiles[ActRect.left+XOffset+i][ActRect.top+YOffset].Block = BLOCKWERT_PLATTFORM;
					}

					RedrawLevel   = true;
				} break;

				case BUTTON_ID_TestLevel:				// Level testen
				{	
					TestLevel();
				} break;

				case BUTTON_ID_CopyFromBlock:			// Vom "Verdecken" nach "Overlay" setzen
				{	
					CopyFromBlock();
				} break;

				case BUTTON_ID_EditMode:				// Edit-Mode wechseln
				{
					if (EditMode == LEVELMODE)
					{
						EditMode = OBJECTMODE;
						SendMessage(ButtonEditMode,WM_SETTEXT,0,(long)"Object-Mode");
					}
					else
					{
						EditMode = LEVELMODE;
						SendMessage(ButtonEditMode,WM_SETTEXT,0,(long)"Level-Mode");
					}
				} break;

				case RADIOBUTTON_ID_Easy:				// Gegner Easy
				if (pSelectedObject != NULL)
				{
						pSelectedObject->Skill = 0;
				} break;

				case RADIOBUTTON_ID_Medium:				// Gegner Medium
				if (pSelectedObject != NULL)
				{
						pSelectedObject->Skill = 1;
				} break;

				case RADIOBUTTON_ID_Hard:				// Gegner Hard
				if (pSelectedObject != NULL)
				{
						pSelectedObject->Skill = 2;
				} break;

				case BUTTON_ID_ChangeLight:					// Licht anpassen
				if (pSelectedObject != NULL)
				{
					if (HIWORD(wparam) == BN_CLICKED && pSelectedObject->ChangeLight == true)
					{
						SendMessage(BoxChangeLight, BM_SETCHECK, BST_UNCHECKED, 0);
						pSelectedObject->ChangeLight = false;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxChangeLight, BM_SETCHECK, BST_CHECKED, 0);
						pSelectedObject->ChangeLight = true;
						RedrawLevel = true;
					}
				} break;

				case BUTTON_ID_LightRed:				// Red bei Licht/Schatten ein/aus
				{
					if (HIWORD(wparam) == BN_CLICKED && bLightRed == false)
					{
						SendMessage(BoxLightRed, BM_SETCHECK, BST_CHECKED, 0);
						bLightRed = true;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxLightRed, BM_SETCHECK, BST_UNCHECKED, 0);
						bLightRed = false;
						RedrawLevel = true;
					}
				} break;

				case BUTTON_ID_LightGreen:				// Green bei Licht/Schatten ein/aus
				{
					if (HIWORD(wparam) == BN_CLICKED && bLightGreen == false)
					{
						SendMessage(BoxLightGreen, BM_SETCHECK, BST_CHECKED, 0);
						bLightGreen = true;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxLightGreen, BM_SETCHECK, BST_UNCHECKED, 0);
						bLightGreen = false;
						RedrawLevel = true;
					}
				} break;

				case BUTTON_ID_LightBlue:				// Blue bei Licht/Schatten ein/aus
				{
					if (HIWORD(wparam) == BN_CLICKED && bLightBlue == false)
					{
						SendMessage(BoxLightBlue, BM_SETCHECK, BST_CHECKED, 0);
						bLightBlue = true;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxLightBlue, BM_SETCHECK, BST_UNCHECKED, 0);
						bLightBlue = false;
						RedrawLevel = true;
					}
				} break;

				case BUTTON_ID_CopyBack:			// Clipboard Option Back
				{
					if (HIWORD(wparam) == BN_CLICKED && bCopyBack == false)
					{
						SendMessage(BoxCopyBack, BM_SETCHECK, BST_CHECKED, 0);
						bCopyBack = true;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxCopyBack, BM_SETCHECK, BST_UNCHECKED, 0);
						bCopyBack = false;
						RedrawLevel = true;
					}
				} break;

				case BUTTON_ID_CopyFront:			// Clipboard Option Front
				{
					if (HIWORD(wparam) == BN_CLICKED && bCopyFront == false)
					{
						SendMessage(BoxCopyFront, BM_SETCHECK, BST_CHECKED, 0);
						bCopyFront = true;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxCopyFront, BM_SETCHECK, BST_UNCHECKED, 0);
						bCopyFront = false;
						RedrawLevel = true;
					}
				} break;

				case BUTTON_ID_CopyLight:			// Clipboard Option Light
				{
					if (HIWORD(wparam) == BN_CLICKED && bCopyLight == false)
					{
						SendMessage(BoxCopyLight, BM_SETCHECK, BST_CHECKED, 0);
						bCopyLight = true;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxCopyLight, BM_SETCHECK, BST_UNCHECKED, 0);
						bCopyLight = false;
						RedrawLevel = true;
					}
				} break;

				case BUTTON_ID_CopyBlock:			// Clipboard Option Block
				{
					if (HIWORD(wparam) == BN_CLICKED && bCopyBlock == false)
					{
						SendMessage(BoxCopyBlock, BM_SETCHECK, BST_CHECKED, 0);
						bCopyBlock = true;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxCopyBlock, BM_SETCHECK, BST_UNCHECKED, 0);
						bCopyBlock = false;
						RedrawLevel = true;
					}
				} break;

				case BUTTON_ID_ScrollBackground:			// Letzten Layer scrollen
				{
					if (HIWORD(wparam) == BN_CLICKED && bScrollBackground == false)
					{
						SendMessage(BoxScrollBackground, BM_SETCHECK, BST_CHECKED, 0);
						bScrollBackground = true;
						pTileEngine->bScrollBackground = true;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxScrollBackground, BM_SETCHECK, BST_UNCHECKED, 0);
						bScrollBackground = false;
						pTileEngine->bScrollBackground = false;
						RedrawLevel = true;
					}
				} break;

				case BUTTON_ID_Taschenlampe:			// Letzten Layer scrollen
				{
					if (HIWORD(wparam) == BN_CLICKED && bTaschenlampe == false)
					{
						SendMessage(BoxTaschenlampe, BM_SETCHECK, BST_CHECKED, 0);
						bTaschenlampe = true;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxTaschenlampe, BM_SETCHECK, BST_UNCHECKED, 0);
						bTaschenlampe = false;						
						RedrawLevel = true;
					}
				} break;

				case BUTTON_ID_KeepTileOffset:			// Offset im Auswahlrechteck merken
				{
					if (HIWORD(wparam) == BN_CLICKED && bKeepTileOffset == false)
					{
						SendMessage(BoxKeepTileOffset, BM_SETCHECK, BST_CHECKED, 0);
						bKeepTileOffset = true;
					}
					else
					{
						SendMessage(BoxKeepTileOffset, BM_SETCHECK, BST_UNCHECKED, 0);
						bKeepTileOffset = false;
					}
				} break;

				case BUTTON_ID_ShowBackground:			// Background anzeigen
				{
					if (HIWORD(wparam) == BN_CLICKED && bShowBackground == false)
					{
						SendMessage(BoxShowBackground, BM_SETCHECK, BST_CHECKED, 0);
						bShowBackground = true;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxShowBackground, BM_SETCHECK, BST_UNCHECKED, 0);
						bShowBackground = false;
						RedrawLevel = true;
					}
				} break;

				case BUTTON_ID_ShowLevel:				// Level anzeigen
				{
					if (HIWORD(wparam) == BN_CLICKED && bShowLevel == false)
					{
						SendMessage(BoxShowLevel, BM_SETCHECK, BST_CHECKED, 0);
						bShowLevel = true;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxShowLevel, BM_SETCHECK, BST_UNCHECKED, 0);
						bShowLevel = false;
						RedrawLevel = true;
					}
				} break;

				case BUTTON_ID_ShowOverlay:				// Overlay anzeigen
				{
					if (HIWORD(wparam) == BN_CLICKED && bShowOverlay == false)
					{
						SendMessage(BoxShowOverlay, BM_SETCHECK, BST_CHECKED, 0);
						bShowOverlay = true;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxShowOverlay, BM_SETCHECK, BST_UNCHECKED, 0);
						bShowOverlay = false;
						RedrawLevel = true;
					}
				} break;

				case BUTTON_ID_ShowLight:				// Overlay anzeigen
				{
					if (HIWORD(wparam) == BN_CLICKED && bShowLight == false)
					{
						SendMessage(BoxShowLight, BM_SETCHECK, BST_CHECKED, 0);
						bShowLight = true;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxShowLight, BM_SETCHECK, BST_UNCHECKED, 0);
						bShowLight = false;
						RedrawLevel = true;
					}
				} break;

				case BUTTON_ID_ShowBlock:				// Block anzeigen
				{
					if (HIWORD(wparam) == BN_CLICKED && bShowBlock == false)
					{
						SendMessage(BoxShowBlock, BM_SETCHECK, BST_CHECKED, 0);
						bShowBlock = true;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxShowBlock, BM_SETCHECK, BST_UNCHECKED, 0);
						bShowBlock = false;
						RedrawLevel = true;
					}
				} break;

				case BUTTON_ID_ShowObjects:				// Gegner anzeigen
				{
					if (HIWORD(wparam) == BN_CLICKED && bShowObjects == false)
					{
						SendMessage(BoxShowObjects, BM_SETCHECK, BST_CHECKED, 0);
						bShowObjects = true;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxShowObjects, BM_SETCHECK, BST_UNCHECKED, 0);
						bShowObjects = false;
						RedrawLevel = true;
					}
				} break;

				case BUTTON_ID_ShowGrid:				// Grid anzeigen
				{
					if (HIWORD(wparam) == BN_CLICKED && bShowGrid == false)
					{
						SendMessage(BoxShowGrid, BM_SETCHECK, BST_CHECKED, 0);
						bShowGrid	 = true;
						RedrawLevel = true;
					}
					else
					{
						SendMessage(BoxShowGrid, BM_SETCHECK, BST_UNCHECKED, 0);
						bShowGrid	 = false;
						RedrawLevel = true;
					}
				} break;

				case BOX_ID_BlockWert_Wand:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_WAND)
						SendMessage(BoxBlockWertWand, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertWand, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_WAND;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_GegnerWand:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_GEGNERWAND)
						SendMessage(BoxBlockWertGegnerWand, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertGegnerWand, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_GEGNERWAND;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_Plattform:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_PLATTFORM)
						SendMessage(BoxBlockWertPlattform, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertPlattform, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_PLATTFORM;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_Light:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_LIGHT)
						SendMessage(BoxBlockWertLight, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertLight, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_LIGHT;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_Verdecken:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_VERDECKEN)
						SendMessage(BoxBlockWertVerdecken, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertVerdecken, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_VERDECKEN;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_AnimiertB:			// Blockwert Back Set ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_ANIMIERT_BACK)
						SendMessage(BoxBlockWertAnimiertB, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertAnimiertB, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_ANIMIERT_BACK;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_AnimiertF:			// Blockwert Front Set ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_ANIMIERT_FRONT)
						SendMessage(BoxBlockWertAnimiertF, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertAnimiertF, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_ANIMIERT_FRONT;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_Wasser:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_WASSER)
						SendMessage(BoxBlockWertWasser, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertWasser, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_WASSER;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_Schaden:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_SCHADEN)
						SendMessage(BoxBlockWertSchaden, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertSchaden, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_SCHADEN;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_FliessBandL:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_FLIESSBANDL)
						SendMessage(BoxBlockWertFliessBandL, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertFliessBandL, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_FLIESSBANDL;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_FliessBandR:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_FLIESSBANDR)
						SendMessage(BoxBlockWertFliessBandR, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertFliessBandR, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_FLIESSBANDR;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_WendePunkt:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_WENDEPUNKT)
						SendMessage(BoxBlockWertWendePunkt, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertWendePunkt, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_WENDEPUNKT;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_Destructible:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_DESTRUCTIBLE)
						SendMessage(BoxBlockWertDestructible, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertDestructible, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_DESTRUCTIBLE;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_MOVELINKS:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_MOVELINKS)
						SendMessage(BoxBlockWertSaeure, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertSaeure, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_MOVELINKS;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_Overlay_Light:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_OVERLAY_LIGHT)
						SendMessage(BoxBlockWertOverlay_Light, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertOverlay_Light, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_OVERLAY_LIGHT;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_Lava:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_SUMPF)
						SendMessage(BoxBlockWertLava, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertLava, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_SUMPF;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_Eis:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_EIS)
						SendMessage(BoxBlockWertEis, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertEis, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_EIS;
					RedrawLevel = true;

				} break;

				case BOX_ID_BLOCKWERT_MOVEVERTICAL:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_MOVEVERTICAL)
						SendMessage(BoxBlockWertMagen, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertMagen, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_MOVEVERTICAL;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_WasserFall:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_WASSERFALL)
						SendMessage(BoxBlockWertWasserFall, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertWasserFall, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_WASSERFALL;
					RedrawLevel = true;

				} break;

				case BOX_ID_BlockWert_TexRechts:				// Blockwert ändern
				{
					if (HIWORD(wparam) == BN_CLICKED && 
						pTileEngine->Tiles[ActRect.left+XOffset]
										  [ActRect.top +YOffset].Block & BLOCKWERT_MOVERECHTS)
						SendMessage(BoxBlockWertTexRechts, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(BoxBlockWertTexRechts, BM_SETCHECK, BST_CHECKED, 0);

					// Block-Wert umkehren
					for (int i=0; i<ClipX; i++)
					 for (int j=0; j<ClipY; j++)
						pTileEngine->Tiles[ActRect.left+XOffset+i]
										  [ActRect.top +YOffset+j].Block ^= BLOCKWERT_MOVERECHTS;
					RedrawLevel = true;

				} break;

				case BUTTON_ID_NextTileSet:					// Nächstes Tileset (falls vorhanden)
				{
					if (ActualTileset < MAX_TILESETS-1) 
						ActualTileset++;
					RedrawTileset = true;
					RedrawLevel   = true;
					SendMessage(ComboBoxSelectTileset, CB_SETCURSEL, TilesetNummer[ActualTileset],0);
				} break;

				case BUTTON_ID_PreviousTileSet:				// Vorheriges Tileset (falls vorhanden)
				{
					if (ActualTileset > 0)
						ActualTileset--;
					RedrawTileset = true;
					RedrawLevel   = true;
					SendMessage(ComboBoxSelectTileset, CB_SETCURSEL, TilesetNummer[ActualTileset],0);
				} break;
			}

			//----- Combo Boxes und ListBox

		switch(HIWORD(wparam))
		{
			case CBN_SELCHANGE:									// Selektion in einer ComboBox
			{	
				if(LOWORD(wparam) == BOX_ID_SelectTileset)		// Tileset ausgesucht
					SetFocus(MainHWND);							// Focus auf Hauptfenster setzen

				if(LOWORD(wparam) == BOX_ID_SelectBackground)	// Hintergrund ausgesucht
				{
					int  nr;
					char Buffer[24];

					nr = SendMessage(ComboBoxSelectBackground, CB_GETCURSEL, 0, 0);
					SendMessage(ComboBoxSelectBackground, CB_GETLBTEXT, nr, (LPARAM) Buffer);

					pTileEngine->Background.~DirectGraphicsSprite();
					pTileEngine->Background.LoadFromGfx(Buffer, 640, 480, 640, 480, 0, 0);

					strcpy(BackgroundNames.BackgroundFile, Buffer);

					RedrawLevel = true;
					SetFocus(MainHWND);				// Focus auf Hauptfenster setzen
				}

				if(LOWORD(wparam) == BOX_ID_SelectParallaxA)	// Layer 1 ausgesucht
				{
					int  nr;
					char Buffer[20];

					nr = SendMessage(ComboBoxSelectParallaxA, CB_GETCURSEL, 0, 0);
					SendMessage(ComboBoxSelectParallaxA, CB_GETLBTEXT, nr, (LPARAM) Buffer);

					pTileEngine->ParallaxLayer[0].~DirectGraphicsSprite();
					pTileEngine->ParallaxLayer[0].LoadFromGfx(Buffer, 640, 480, 640, 480, 0, 0);

					strcpy(BackgroundNames.ParallaxAFile, Buffer);

					RedrawLevel = true;
					SetFocus(MainHWND);				// Focus auf Hauptfenster setzen
				}

				if(LOWORD(wparam) == BOX_ID_SelectParallaxB)	// Layer 2 ausgesucht
				{
					int  nr;
					char Buffer[20];

					nr = SendMessage(ComboBoxSelectParallaxB, CB_GETCURSEL, 0, 0);
					SendMessage(ComboBoxSelectParallaxB, CB_GETLBTEXT, nr, (LPARAM) Buffer);

					pTileEngine->ParallaxLayer[1].~DirectGraphicsSprite();
					pTileEngine->ParallaxLayer[1].LoadFromGfx(Buffer, 640, 480, 640, 480, 0, 0);

					strcpy(BackgroundNames.ParallaxBFile, Buffer);

					RedrawLevel = true;
					SetFocus(MainHWND);				// Focus auf Hauptfenster setzen
				}

				if(LOWORD(wparam) == BOX_ID_SelectClouds)		// Walkenlayer ausgesucht
				{
					int  nr;
					char Buffer[20];

					nr = SendMessage(ComboBoxSelectClouds, CB_GETCURSEL, 0, 0);
					SendMessage(ComboBoxSelectClouds, CB_GETLBTEXT, nr, (LPARAM) Buffer);

					pTileEngine->CloudLayer.~DirectGraphicsSprite();
					pTileEngine->CloudLayer.LoadFromGfx(Buffer, 640, 240, 640, 240, 0, 0);

					strcpy(BackgroundNames.CloudFile, Buffer);

					RedrawLevel = true;
					SetFocus(MainHWND);				// Focus auf Hauptfenster setzen
				}

				if(LOWORD(wparam) == BOX_ID_SelectStageMusic)		// Stage Mucke ausgesucht
				{
					int  nr;
					char Buffer[30];

					nr = SendMessage(ComboBoxSelectStageMusic, CB_GETCURSEL, 0, 0);
					SendMessage(ComboBoxSelectStageMusic, CB_GETLBTEXT, nr, (LPARAM) Buffer);

					strcpy(Appendix.Songs[0], Buffer);
					
					SetFocus(MainHWND);				// Focus auf Hauptfenster setzen
				}

				if(LOWORD(wparam) == BOX_ID_SelectBossMusic)		// Boss Mucke ausgesucht
				{
					int  nr;
					char Buffer[30];

					nr = SendMessage(ComboBoxSelectBossMusic, CB_GETCURSEL, 0, 0);
					SendMessage(ComboBoxSelectBossMusic, CB_GETLBTEXT, nr, (LPARAM) Buffer);

					strcpy(Appendix.Songs[1], Buffer);
					
					SetFocus(MainHWND);				// Focus auf Hauptfenster setzen
				}

				if(LOWORD(wparam) == BOX_ID_SelectPowerblock)		// Boss Mucke ausgesucht
				{
					int  nr;
					nr = SendMessage(ComboBoxSelectPowerblock, CB_GETCURSEL, 0, 0);

					Appendix.UsedPowerblock = nr;
					RedrawLevel = true;
					
					SetFocus(MainHWND);				// Focus auf Hauptfenster setzen
				}

			} break;

			default: break;
			}
		}

//----- Scrollbars

		case WM_VSCROLL:
		{

// Im Level nach Oben/Unten scrollen

			if ((HWND) lparam == VScrollBar)
			{
				RedrawLevel = true;

				switch((int) LOWORD(wparam))
				{
					case SB_THUMBTRACK:						// Mit der Maus ziehen
					{
						YOffset = int(HIWORD(wparam));
					} break;

					case SB_LINEUP:							// Ein Tile hochscrollen
					{
						if (YOffset > 0)
						{
							pTileEngine->YOffset = pTileEngine->YOffset - 20.0f;
							YOffset--;
						}
					} break;

					case SB_LINEDOWN:						// Ein Tile runterscrollen
					{
						if (YOffset < LevelSizeY-24)
						{
							pTileEngine->YOffset = pTileEngine->YOffset + 20.0f;
							YOffset++;
						}
					} break;
				}
				SendMessage(VScrollBar, SBM_SETPOS, (int)((YOffset*(LevelSizeY-24))/24), true);
			}

		} break;

		case WM_HSCROLL:
		{

	// Im Level nach Rechts/Links scrollen

			if ((HWND) lparam == HScrollBar)				
			{
				RedrawLevel = true;

				switch((int) LOWORD(wparam))
				{
					case SB_THUMBTRACK:						// Mit der Maus ziehen
					{
						XOffset = int(HIWORD(wparam));
					} break;

					case SB_LINELEFT:						// Ein Tile links scrollen
					{
						if (XOffset > 0)
						{
							pTileEngine->XOffset = pTileEngine->XOffset - 20.0f;
							XOffset--;
						}
					} break;

					case SB_LINERIGHT:						// Ein Tile rechts scrollen
					{
						if (XOffset < LevelSizeX-32)
						{
							pTileEngine->XOffset = pTileEngine->XOffset + 20.0f;
							XOffset++;
						}
					} break;
				}
				SendMessage(HScrollBar, SBM_SETPOS, (int)((XOffset*(LevelSizeX-32))/32), true);
			}

	// Rotwert ändern
			if ((HWND) lparam == RedValue)				
			{
				RedrawLevel = true;

				switch((int) LOWORD(wparam))
				{
					case SB_THUMBTRACK:
					{
						for (int i=0; i<ClipX; i++)
						 for (int j=0; j<ClipY; j++)
						  pTileEngine->Tiles[ActRect.left+i+XOffset]
										    [ActRect.top+j+YOffset].Red = (char)HIWORD(wparam);						
					} break;

					case SB_LINELEFT:
					{
						for (int i=0; i<ClipX; i++)
						 for (int j=0; j<ClipY; j++)
						  pTileEngine->Tiles[ActRect.left+i+XOffset]
											[ActRect.top+j+YOffset].Red--;
					} break;

					case SB_LINERIGHT:
					{
						for (int i=0; i<ClipX; i++)
						 for (int j=0; j<ClipY; j++)
						  pTileEngine->Tiles[ActRect.left+i+XOffset]
											[ActRect.top+j+YOffset].Red++;
					} break;
				}
			}

// Grünwert ändern
			if ((HWND) lparam == GreenValue)				
			{
				RedrawLevel = true;

				switch((int) LOWORD(wparam))
				{
					case SB_THUMBTRACK:
					{
						for (int i=0; i<ClipX; i++)
						 for (int j=0; j<ClipY; j++)
						  pTileEngine->Tiles[ActRect.left+i+XOffset]
										    [ActRect.top+j+YOffset].Green = (char)HIWORD(wparam);						
					} break;

					case SB_LINELEFT:
					{
						for (int i=0; i<ClipX; i++)
						 for (int j=0; j<ClipY; j++)
						  pTileEngine->Tiles[ActRect.left+i+XOffset]
											[ActRect.top+j+YOffset].Green--;
					} break;

					case SB_LINERIGHT:
					{
						for (int i=0; i<ClipX; i++)
						 for (int j=0; j<ClipY; j++)
						  pTileEngine->Tiles[ActRect.left+i+XOffset]
											[ActRect.top+j+YOffset].Green++;
					} break;
				}
			}

// Blauwert ändern
			if ((HWND) lparam == BlueValue)				
			{
				RedrawLevel = true;

				switch((int) LOWORD(wparam))
				{
					case SB_THUMBTRACK:
					{
						for (int i=0; i<ClipX; i++)
						 for (int j=0; j<ClipY; j++)
						  pTileEngine->Tiles[ActRect.left+i+XOffset]
										    [ActRect.top+j+YOffset].Blue = (char)HIWORD(wparam);						
					} break;

					case SB_LINELEFT:
					{
						for (int i=0; i<ClipX; i++)
						 for (int j=0; j<ClipY; j++)
						  pTileEngine->Tiles[ActRect.left+i+XOffset]
											[ActRect.top+j+YOffset].Blue--;
					} break;

					case SB_LINERIGHT:
					{
						for (int i=0; i<ClipX; i++)
						 for (int j=0; j<ClipY; j++)
						  pTileEngine->Tiles[ActRect.left+i+XOffset]
											[ActRect.top+j+YOffset].Blue++;
					} break;
				}
			}

// Alphawert ändern
			if ((HWND) lparam == AlphaValue)				
			{
				RedrawLevel = true;

				switch((int) LOWORD(wparam))
				{
					case SB_THUMBTRACK:
					{
						for (int i=0; i<ClipX; i++)
						 for (int j=0; j<ClipY; j++)
						  pTileEngine->Tiles[ActRect.left+i+XOffset]
										    [ActRect.top+j+YOffset].Alpha = (char)HIWORD(wparam);						
					} break;

					case SB_LINELEFT:
					{
						for (int i=0; i<ClipX; i++)
						 for (int j=0; j<ClipY; j++)
						  pTileEngine->Tiles[ActRect.left+i+XOffset]
											[ActRect.top+j+YOffset].Alpha--;
					} break;

					case SB_LINERIGHT:
					{
						for (int i=0; i<ClipX; i++)
						 for (int j=0; j<ClipY; j++)
						  pTileEngine->Tiles[ActRect.left+i+XOffset]
											[ActRect.top+j+YOffset].Alpha++;
					} break;
				}
			}

// Helligkeit (R G B gleichzeitig) ändern
			if ((HWND) lparam == AllValues)				
			{
				RedrawLevel = true;

				switch((int) LOWORD(wparam))
				{
					case SB_THUMBTRACK:
					{
						for (int i=0; i<ClipX; i++)
						 for (int j=0; j<ClipY; j++)
						 {
							pTileEngine->Tiles[ActRect.left+i+XOffset]
										    [ActRect.top+j+YOffset].Red   = (char)HIWORD(wparam);
							pTileEngine->Tiles[ActRect.left+i+XOffset]
										    [ActRect.top+j+YOffset].Green = (char)HIWORD(wparam);
							pTileEngine->Tiles[ActRect.left+i+XOffset]
										    [ActRect.top+j+YOffset].Blue  = (char)HIWORD(wparam);
						 }
						 SendMessage(AllValues,  SBM_SETPOS, 
						 pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Red,   true);
					} break;

					case SB_LINELEFT:
					{
						for (int i=0; i<ClipX; i++)
						 for (int j=0; j<ClipY; j++)
						 {
							 pTileEngine->Tiles[ActRect.left+i+XOffset]
											[ActRect.top+j+YOffset].Red--;
							 pTileEngine->Tiles[ActRect.left+i+XOffset]
											[ActRect.top+j+YOffset].Green--;
							 pTileEngine->Tiles[ActRect.left+i+XOffset]
											[ActRect.top+j+YOffset].Blue--;						  
						 }
						 SendMessage(AllValues,  SBM_SETPOS, 
						 pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Red,   true);
					} break;

					case SB_LINERIGHT:
					{
						for (int i=0; i<ClipX; i++)
						 for (int j=0; j<ClipY; j++)
						  {
							 pTileEngine->Tiles[ActRect.left+i+XOffset]
											[ActRect.top+j+YOffset].Red++;
							 pTileEngine->Tiles[ActRect.left+i+XOffset]
											[ActRect.top+j+YOffset].Green++;
							 pTileEngine->Tiles[ActRect.left+i+XOffset]
											[ActRect.top+j+YOffset].Blue++;						  
						 }
						 SendMessage(AllValues,  SBM_SETPOS, 
						 pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Red,   true);
					} break;
				}
			}

		} break;


		default : break;
	}

	// unbearbeitete Nachrichten zurückliefern
	return(DefWindowProc(hwnd, message, wparam, lparam));
}

// --------------------------------------------------------------------------------------
// Win-Main Funktion
// --------------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstace, 
				   LPSTR lpcmdline,		int nshowcmd)
{
	WNDCLASSEX			winclass;							// eigene Windows-Klasse
	HMENU				MainMenu;
	MSG					message;							// Message	
	HWND				DesktopHWND;
	RECT				rect;

	//
	//
	// HurriPath auslesen
	//
	//

	FILE *Datei = NULL;

	Datei = fopen ("hurripath.txt", "rt");

	if (Datei == NULL)
	{
		MessageBox(NULL, "Fehler beim Laden des Hurrican\nPfades aus 'hurripath.txt'!", "Fehler !", MB_OK);
		strcpy (HurriPath, "");
	}
	else
	{
		strcpy (HurriPath, "");
		int count = 0;
		char c = getc (Datei);

		// Zeichen für Zeichen auslesen, bis ein c==10 gelesen wird. Dies markiert das Zeilenende
		//
		do
		{
			HurriPath[count]= c;
			c = getc (Datei);
			count++;
		} while (c != 10 && c != EOF);

		HurriPath [count]= '\0';

		fclose (Datei);
	}

	// Versuchen, die Exe zu öffnen
	//
	char temp[255];

	strcpy(temp, HurriPath);
	strcat(temp, "Hurrican.exe");
	Datei = NULL;
	Datei = fopen (temp, "rt");

	// Exe nicht vorhanden? Dann User angeben lassen, wo sie sich befindet
	//
	if (Datei == NULL)
	{
		char dirsave[255];

		_getcwd (dirsave, sizeof (dirsave));

		MessageBox(NULL, "Hurrican.exe nicht gefunden!\nBitte Pfad angeben.", "Fehler !", MB_OK);

		strcpy (HurriPath, "");

		static OPENFILENAME fn;
		static char Filter[] = "Hurrican.exe (*.exe)\0*.EXE\0";
		fn.lStructSize       = sizeof(OPENFILENAME);
		fn.hwndOwner         = GetActiveWindow();
		fn.lpstrFilter       = Filter;
		fn.lpstrCustomFilter = NULL;
		fn.nFilterIndex      = 0;
		fn.lpstrFile         = HurriPath;
		fn.nMaxFile          = sizeof(HurriPath);
		fn.lpstrFileTitle    = NULL;
		fn.lpstrInitialDir   = "";
		fn.lpstrTitle        = NULL;
		fn.Flags             = OFN_READONLY | OFN_FILEMUSTEXIST;
		fn.lpstrDefExt       = NULL;
		
		// File dialog öffnen und Pfad der Hurrican.exe auslesen
		//
		GetOpenFileName(&fn);

		strcpy  (temp, "");
		strncat (temp, HurriPath, strlen (HurriPath) - 12);
		strcpy  (HurriPath, temp);

		// Wieder in den aktuellen Ordner wechseln
		//
		_chdir (dirsave);

		// HurriPath sichern
		//
		Datei = fopen ("hurripath.txt", "wt");
		fprintf (Datei, "%s", HurriPath);
		fclose (Datei);		
	}

	// Desktop Size holen, um Fenster in der Mitte zu zentrieren
	// 
	DesktopHWND = GetDesktopWindow();
	GetWindowRect(DesktopHWND, &rect);

	// Anfang der Logdatei mit Datum und Uhrzeit
	char StringBuffer[100];
	Protokoll.WriteText(">------------------------------<\n", false);
	Protokoll.WriteText("|  Hurrican Level-Editor V1.0  |\n", false);
	Protokoll.WriteText("| (c) 2002 Jörg M. Winterstein |\n", false);
	Protokoll.WriteText(">------------------------------<\n", false);
	Protokoll.WriteText("Logdatei vom : ", false);
	strcpy(StringBuffer, __DATE__);	Protokoll.WriteText(StringBuffer, false); Protokoll.WriteText(" - ", false);
	strcpy(StringBuffer, __TIME__);	Protokoll.WriteText(StringBuffer, false); 

	Protokoll.WriteText("\n\n>-----------------------------------<\n", false);
	Protokoll.WriteText(    "| Fenster Initialisierung gestartet |\n", false);
	Protokoll.WriteText(    ">-----------------------------------<\n\n", false);


	// Werte für die Windows-Klasse festlegen
	winclass.cbSize			= sizeof (WNDCLASSEX);					// Grösse der Klasse
	winclass.style			= CS_HREDRAW | CS_VREDRAW;				// Fenster-Einstellungen
	winclass.lpfnWndProc	= WindowProc;							// Callback Funktion
	winclass.cbClsExtra		= 0;									// extra Klassen-Info Space
	winclass.cbWndExtra		= 0;									// extra Fenster-Info Space
	winclass.hInstance		= hinstance;							// Fenster-Handle
	winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);		// Fenster-Icon
	winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);			// Mauscursor setzen
	winclass.hbrBackground  = GetSysColorBrush (COLOR_BTNFACE);;	// Hintergrundfarbe setzen
	winclass.lpszMenuName	= "MainMenu";							// Menu-Name
	winclass.lpszClassName	= WINDOWCLASSNAME;						// Name der neuen Klasse
	winclass.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);		// Task-Leiste Icon laden

	// Checken, ob 16 oder 32 Bit gesetzt ist
	//
	hdc = GetDC(MainHWND);
	if (GetDeviceCaps(hdc, BITSPIXEL) == 16)
		SCREENBPP = 16;
	else
	if (GetDeviceCaps(hdc, BITSPIXEL) == 32)
		SCREENBPP = 32;
	else
	{
		MessageBox(NULL, "Fehler bei Farbtiefe\n"
						 "Weder 16 noch 32 bit ?", "Fehler !", MB_OK);
		return 0;
	}

	// checken ob 1024x768 oder höher gesetzt ist

	if (GetDeviceCaps(hdc, HORZRES) < 1024 ||
		GetDeviceCaps(hdc, VERTRES) < 768)
		MessageBox(NULL, "Auflösung von mindestens 1024x768 erforderlich, um anständig zu laufen ;)", "Fehler !", MB_OK);

	// Fensterklasse bei Windows registrieren
	if (!RegisterClassEx(&winclass))
	{
		Protokoll.WriteText("RegisterClassEx Fehler !\n", true);
		return(0);
	}

	Protokoll.WriteText("RegisterClassEx	: erfolgreich !\n", false);

	// Neues Fenster mit der registrierten Klasse erstellen
	if (!(MainHWND = CreateWindowEx(NULL,							// Eigenschaften
								WINDOWCLASSNAME,					// Name der neuen Klasse
								"Hurrican Level-Editor",			// Titel des Fenster
								WS_OVERLAPPED	|						// Fenster Style
								WS_CAPTION		|
								WS_SYSMENU		|
								WS_BORDER		| 
								WS_MINIMIZEBOX	| 
								WS_VISIBLE,							
								(rect.right  - 1024) / 2,			// x-Position
								(rect.bottom -  768) / 2,			// y-Position
								WINDOWWIDTH,						// Fensterbreite
								WINDOWHEIGHT,						// Fensterhöhe
								NULL,								// Handle des Parentfensters
								NULL,								// Vorerst kein Menus
								hinstance,							// Instance von Main
								NULL)))								// extra creation parms
		{
			Protokoll.WriteText("CreateWindowEx Fehler !\n", true);
			return(0);
		}

	Protokoll.WriteText("CreateWindowEx	: erfolgreich !\n", false);
	Protokoll.WriteText("WindowSizeX	: ", false); Protokoll.WriteValue(WINDOWWIDTH);
	Protokoll.WriteText("WindowSizeY	: ", false); Protokoll.WriteValue(WINDOWHEIGHT);

	MainMenu = LoadMenu(hinstance, "MainMenu");

	Protokoll.WriteText("\n-> Fenster Initialisierung erfolgreich beendet !\n", false);

	ShowWindow(MainHWND, nshowcmd);						// Fenster anzeigen (sicher ist sicher)
	UpdateWindow(MainHWND);								// Fenster-infos updaten

//----- Spiel-Initialisierung


	if(!GameInit(hinstance))
	{
		Protokoll.WriteText("\n-> GameInit Fehler !\n\n", true);
		EditorRunning = false;
	}
	else
		Protokoll.WriteText("\n-> GameInit erfolgreich beendet !\n\n", false);

//----- Editor-Umgebung aufbauen

	CreateEnvironment(hinstance);	

//----- Main-Loop 

	while(EditorRunning == true)
	{
		while (PeekMessage (&message, NULL, 0, 0, PM_REMOVE))	// Nachricht vom Stapel holen
		{														// und löschen
			TranslateMessage(&message);							// Nachricht überetzen
			DispatchMessage(&message);							// Nachricht an WinProc weiterleiten
		}

		if (EditorPaused == false)
		{
			hdc = GetDC(MainHWND);									// Handle holen
			Heartbeat();											// Main Loop
			ReleaseDC(MainHWND,hdc);								// Handle freigeben

			// Eingabegeräte updaten
			DirectInput.TastaturUpdate();
			DirectInput.MausUpdate(false);
			
			// Fenster-infos updaten
			UpdateWindow(MainHWND);
		}
}

//----- Spiel verlassen

	if(!GameExit())
		Protokoll.WriteText("-> GameExit Fehler !\n", true);

	Protokoll.WriteText("\n-> GameExit erfolgreich beendet !\n", false);
	Protokoll.WriteText("\nhttp://www.poke53280.de\n", false);
	Protokoll.WriteText("Bugreports, Fragen etc : Eiswuxe@Poke53280.de\n", false);
	Protokoll.WriteText("\n-> Logdatei Ende", false);

	return(message.wParam);										// Rückkehr zu Windows
}

// -------------------------------------------------------------------------------------- 
// Heartbeat, der Mainloop. der bei jedem Frame durchlaufen wird
// -------------------------------------------------------------------------------------- 

bool Heartbeat(void)
{
	DoMouse();										// Maus bewegt ?
	DoKeys();										// Tasten gedrückt ?

	MouseInTile	 = false;		// Mauscursor im Tile ?
	MouseInLevel = false;		// Mauscursor im Level ?

	POINT ptCursor;

	GetCursorPos(&ptCursor);
	ScreenToClient(MainHWND, &ptCursor);

	FensterMausX = ptCursor.x;		// Maus Koordinaten setzen
	FensterMausY = ptCursor.y;

	// Level-Scrollbars anpassen
	SetScrollRange(HScrollBar, SB_CTL, 0, (pTileEngine->LEVELSIZE_X-32), false);
	SetScrollRange(VScrollBar, SB_CTL, 0, (pTileEngine->LEVELSIZE_Y-24), false);
	SendMessage(HScrollBar, SBM_SETPOS, (int)(XOffset*(pTileEngine->LEVELSIZE_X-32)/32), true);
	SendMessage(VScrollBar, SBM_SETPOS, (int)(YOffset*(pTileEngine->LEVELSIZE_Y-24)/24), true);

	// Aktuelle Licht-Einstellungen setzen
	SendMessage(RedValue,   SBM_SETPOS, pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Red,   true);
	SendMessage(GreenValue, SBM_SETPOS, pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Green, true);
	SendMessage(BlueValue,  SBM_SETPOS, pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Blue,  true);
	SendMessage(AlphaValue, SBM_SETPOS, pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Alpha, true);

	// Levelbeschreibung aus Textfeld auslesen
	int ilen = 0;
	char buf[8];

	SendMessage(BeschreibungsFeld, WM_GETTEXT, ilen+1, (long)Beschreibung);

	// Farben für Liquid auslesen
	ilen = SendMessage(Color1Feld, WM_GETTEXTLENGTH, 0, 0);
	SendMessage(Color1Feld, WM_GETTEXT, ilen+1, (long)Appendix.Col1);	 

	Col1 = D3DCOLOR_RGBA(GetDecValue(&Appendix.Col1[0], 2),
						 GetDecValue(&Appendix.Col1[2], 2),
						 GetDecValue(&Appendix.Col1[4], 2),
						 GetDecValue(&Appendix.Col1[6], 2));
				
	ilen = SendMessage(Color2Feld, WM_GETTEXTLENGTH, 0, 0);
	SendMessage(Color2Feld, WM_GETTEXT, ilen+1, (long)Appendix.Col2);

	Col2 = D3DCOLOR_RGBA(GetDecValue(&Appendix.Col2[0], 2),
						 GetDecValue(&Appendix.Col2[2], 2),
						 GetDecValue(&Appendix.Col2[4], 2),
						 GetDecValue(&Appendix.Col2[6], 2));

	// Levelgrösse aus den Textfeldern lesen
	ilen = SendMessage(EditSizeX, WM_GETTEXTLENGTH, 0, 0);
	SendMessage(EditSizeX, WM_GETTEXT, ilen+1, (long)buf);
	LevelSizeX = atoi(buf);
	pTileEngine->LEVELSIZE_X = LevelSizeX;

	ilen = SendMessage(EditSizeY, WM_GETTEXTLENGTH, 0, 0);
	SendMessage(EditSizeY, WM_GETTEXT, ilen+1, (long)buf);
	LevelSizeY = atoi(buf);
	pTileEngine->LEVELSIZE_Y = LevelSizeY;

	// Value 1 aus dem Textfeld lesen
	ilen = SendMessage(Value1Feld, WM_GETTEXTLENGTH, 0, 0);
	SendMessage(Value1Feld, WM_GETTEXT, ilen+1, (long)buf);
	if (pSelectedObject != NULL)
		pSelectedObject->Value1 = atoi(buf);

	// Value 2 aus dem Textfeld lesen
	ilen = SendMessage(Value2Feld, WM_GETTEXTLENGTH, 0, 0);
	SendMessage(Value2Feld, WM_GETTEXT, ilen+1, (long)buf);
	if (pSelectedObject != NULL)
		pSelectedObject->Value2 = atoi(buf);

	// Timelimit aus Textfeld auslesen
	ilen = SendMessage(TimelimitFeld, WM_GETTEXTLENGTH, 0, 0);
	SendMessage(TimelimitFeld, WM_GETTEXT, ilen+1, (long)buf);
	Timelimit = atoi(buf);

	if (FensterMausX > TILE_XOFFSET     &&
		FensterMausY > TILE_YOFFSET	    &&
		FensterMausX < TILE_XOFFSET+240 &&
		FensterMausY < TILE_YOFFSET+240)
			MouseInTile = true;

	if (FensterMausX > VIEW_XOFFSET     &&
		FensterMausY > VIEW_YOFFSET	    &&
		FensterMausX < VIEW_XOFFSET+640 &&
		FensterMausY < VIEW_YOFFSET+480)
			MouseInLevel = true;

//----- Direct3D Sachen

//----- Level anzeigen

	//if(RedrawLevel == true) 
	{		
		DrawLevel();

		if(RedrawLevel == true) 
		{
			ShowValues();
			DrawMiniMap();
			RedrawLevel = false;		
		}
	}

//-----Tileset anzeigen

	if(RedrawTileset == true)
	{
		DrawTileset();
		RedrawTileset = false;
	}

	// Aktuelles Objekt durch Klick in die Liste in ein anderes umgewandelt ?
	if(pSelectedObject != NULL &&
	   pSelectedObject->ObjectID != (unsigned int)(SendMessage(ComboBoxObjectType, CB_GETCURSEL, 0, 0)))
	{
		pSelectedObject->ObjectID = SendMessage(ComboBoxObjectType, CB_GETCURSEL, 0, 0);
		RedrawLevel = true;
	}

	// Aktuell gewähltes Tileset in der ComboBox auslesen
	if(TilesetNummer[ActualTileset] != (int)(SendMessage(ComboBoxSelectTileset, CB_GETCURSEL, 0, 0)))
	{
		TilesetNummer[ActualTileset] = (int)(SendMessage(ComboBoxSelectTileset, CB_GETCURSEL, 0, 0));
		RedrawTileset = true;
	}

	// Bei Objekt Bewegung mit Shift langsam machen
	if (EditMode == OBJECTMODE && (KeyDown(DIK_LSHIFT) || KeyDown(DIK_RSHIFT)))
		Sleep(200);

	Sleep(10);
	return true;
}

// -------------------------------------------------------------------------------------- 
// Level zeichnen
// -------------------------------------------------------------------------------------- 

void DrawLevel(void)
{
	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);			// Alpha-Blending
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// für Colorkey
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// einschalten

	// Backbuffer löschen
	lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	// Mit dem Darstellen beginnen
	lpD3DDevice->BeginScene();						
	
	// Hintergrund ?
	if (bShowBackground)
		pTileEngine->DrawBackground();

	// Level ?
	if(bShowLevel)									// Level anzeigen ?
	{
		//pTileEngine->DrawBackLevel();				

		int			xLevel,		yLevel;				// Position im Level-Array
		int			xTileOffs,  yTileOffs;			// Tile-Verschiebung am Screen (0-19)
		char		red,green,blue;					// Komponenten des Tiles
		D3DCOLOR	Color;							// Farbe des Tiles

		// Position im Level Array errechnen
		xLevel = (int)(XOffset*20 / TILESIZE_X);
		yLevel = (int)(YOffset*20 / TILESIZE_Y);

		// Offsets der Tiles berechnen (0-19)
		xTileOffs = (int)(XOffset*20) % TILESIZE_X;
		yTileOffs = (int)(YOffset*20) % TILESIZE_Y;

		lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);			// Alpha-Blending
		lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// für Colorkey
		lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// einschalten

		// Alle möglichen Tilesets durchmachen
		for(int k=0; k<MAX_TILESETS; k++)
		 for(int j=0; j<SCREENSIZE_Y + 1; j++)
  		  for(int i=0; i<SCREENSIZE_X + 1; i++)
		  {
			if(pTileEngine->Tiles[xLevel+i][yLevel+j].TileSetBack == k &&
			   pTileEngine->Tiles[xLevel+i][yLevel+j].BackArt > 0)
				{
					red   = pTileEngine->Tiles[xLevel+i][yLevel+j].Red;	// Farbe zusammensetzen
					green = pTileEngine->Tiles[xLevel+i][yLevel+j].Green;
					blue  = pTileEngine->Tiles[xLevel+i][yLevel+j].Blue;

					if (bShowLight)
						Color = D3DCOLOR_RGBA(red, green, blue, 255);
					else
						Color = 0xFFFFFFFF;

					int Type = pTileEngine->Tiles[xLevel+i][yLevel+j].BackArt - INCLUDE_ZEROTILE;
					
					pTileEngine->TileGfx[TilesetNummer[k]].RenderSprite((float)(i*20-xTileOffs), 
																		(float)(j*20-yTileOffs), Type, Color);
				}
		  }		
	}
	
	if(bShowOverlay)
	{
		//pTileEngine->DrawFrontLevel();				

		int			xLevel,		yLevel;				// Position im Level-Array
		int			xTileOffs,  yTileOffs;			// Tile-Verschiebung am Screen (0-19)
		char		red,green,blue,alpha;			// Komponenten des Tiles
		D3DCOLOR	Color;							// Farbe des Tiles

		// Position im Level Array errechnen
		xLevel = (int)(XOffset*20 / TILESIZE_X);
		yLevel = (int)(YOffset*20 / TILESIZE_Y);

		// Offsets der Tiles berechnen (0-19)
		xTileOffs = (int)(XOffset*20) % TILESIZE_X;
		yTileOffs = (int)(YOffset*20) % TILESIZE_Y;

		lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);			// Alpha-Blending
		lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// für Colorkey
		lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// einschalten

		// Alle möglichen Tilesets durchmachen
		for(int k=0; k<MAX_TILESETS; k++)
		 for(int j=0; j<SCREENSIZE_Y + 1; j++)
  		  for(int i=0; i<SCREENSIZE_X + 1; i++)
		{
			// normales tile (nicht verdecken)
			if(pTileEngine->Tiles[xLevel+i][yLevel+j].TileSetFront == k &&
			   pTileEngine->Tiles[xLevel+i][yLevel+j].FrontArt > 0 &&
			   !(pTileEngine->Tiles[xLevel+i][yLevel+j].Block & BLOCKWERT_VERDECKEN))
			{
				if (pTileEngine->Tiles[xLevel+i][yLevel+j].Block & BLOCKWERT_OVERLAY_LIGHT &&
					bShowLight)
				{
					red   = pTileEngine->Tiles[xLevel+i][yLevel+j].Red;	// Farbe zusammensetzen
					green = pTileEngine->Tiles[xLevel+i][yLevel+j].Green;
					blue  = pTileEngine->Tiles[xLevel+i][yLevel+j].Blue;
					alpha = pTileEngine->Tiles[xLevel+i][yLevel+j].Alpha;

					Color = D3DCOLOR_RGBA(red, green, blue, alpha);
				}
				else
					Color = D3DCOLOR_RGBA(255, 255, 255, pTileEngine->Tiles[xLevel+i][yLevel+j].Alpha);

				int Type = pTileEngine->Tiles[xLevel+i][yLevel+j].FrontArt - INCLUDE_ZEROTILE;
				pTileEngine->TileGfx[TilesetNummer[k]].RenderSprite((float)(i*20-xTileOffs), 
																	(float)(j*20-yTileOffs), Type, Color);
			}			

			// Wasser
			if(k == 0 &&
			   (pTileEngine->Tiles[xLevel+i][yLevel+j].Block & BLOCKWERT_WASSER ||
			    pTileEngine->Tiles[xLevel+i][yLevel+j].Block & BLOCKWERT_SUMPF))
			{
				int anim = i % 6 + (j % 6) * 6;

				DirectGraphics.SetColorKeyMode();
				LiquidGfx[0].RenderSprite((float)(i*20-xTileOffs), (float)(j*20-yTileOffs), anim,Col1);

				DirectGraphics.SetAdditiveMode();
				LiquidGfx[1].RenderSprite((float)(i*20-xTileOffs), (float)(j*20-yTileOffs), anim,Col2);

				DirectGraphics.SetColorKeyMode();
			}

			// normales tile (verdecken)
			if(pTileEngine->Tiles[xLevel+i][yLevel+j].TileSetFront == k &&
			   pTileEngine->Tiles[xLevel+i][yLevel+j].FrontArt > 0 &&
			   pTileEngine->Tiles[xLevel+i][yLevel+j].Block & BLOCKWERT_VERDECKEN)
			{
				if (pTileEngine->Tiles[xLevel+i][yLevel+j].Block & BLOCKWERT_OVERLAY_LIGHT &&
					bShowLight)
				{
					red   = pTileEngine->Tiles[xLevel+i][yLevel+j].Red;	// Farbe zusammensetzen
					green = pTileEngine->Tiles[xLevel+i][yLevel+j].Green;
					blue  = pTileEngine->Tiles[xLevel+i][yLevel+j].Blue;
					alpha = pTileEngine->Tiles[xLevel+i][yLevel+j].Alpha;
					Color = D3DCOLOR_RGBA(red, green, blue, alpha);
				}
				else
					Color = D3DCOLOR_RGBA(255, 255, 255, pTileEngine->Tiles[xLevel+i][yLevel+j].Alpha);

				int Type = pTileEngine->Tiles[xLevel+i][yLevel+j].FrontArt - INCLUDE_ZEROTILE;
				pTileEngine->TileGfx[TilesetNummer[k]].RenderSprite((float)(i*20-xTileOffs), 
																	(float)(j*20-yTileOffs), Type, Color);
			}			
		}
	}

	pTileEngine->UpdateLevel();

	  // Clipboard an mouse pos anzeigen
	if (EditMode == LEVELMODE &&
		ClipX > 0 &&
		ClipY > 0)
	{			
		int xm = (FensterMausX-VIEW_XOFFSET)/TILESIZE_X*TILESIZE_X;
		int ym = (FensterMausY-VIEW_YOFFSET)/TILESIZE_Y*TILESIZE_Y;

		if (bShowLevel && bCopyBack)
		{
			int			xTileOffs,  yTileOffs;			// Tile-Verschiebung am Screen (0-19)
			char		red,green,blue;					// Komponenten des Tiles
			D3DCOLOR	Color;							// Farbe des Tiles

			// Offsets der Tiles berechnen (0-19)
			xTileOffs = (int)(XOffset*20) % TILESIZE_X;
			yTileOffs = (int)(YOffset*20) % TILESIZE_Y;			

			RenderRect(xm-1, ym-1, ClipX * TILESIZE_X, 1, 0xFFFFFFFF);
			RenderRect(xm-1, ym-1, 1, ClipY * TILESIZE_Y, 0xFFFFFFFF);
			RenderRect(xm + ClipX * TILESIZE_X, ym-1, 0, ClipY * TILESIZE_Y + 1, 0xFFFFFFFF);
			RenderRect(xm - 1, ym + ClipY * TILESIZE_Y, ClipX * TILESIZE_X + 1, 0, 0xFFFFFFFF);

			for(int k=0; k<MAX_TILESETS; k++)
			 for(int j=0; j<ClipY; j++)
  			  for(int i=0; i<ClipX; i++)
			  {
				if(ClipBoard[i][j].TileSetBack == k &&
				   ClipBoard[i][j].BackArt > 0)
					{
						red   = ClipBoard[i][j].Red;	// Farbe zusammensetzen
						green = ClipBoard[i][j].Green;
						blue  = ClipBoard[i][j].Blue;

						if (bShowLight)
							Color = D3DCOLOR_RGBA(red, green, blue, 255);
						else
							Color = 0xFFFFFFFF;

						int Type = ClipBoard[i][j].BackArt;

						if (Type < 255) 
						pTileEngine->TileGfx[TilesetNummer[k]].RenderSprite(xm + (float)(i*20-xTileOffs), 
																			ym + (float)(j*20-yTileOffs), Type, Color);
					}
			  }			
		}

		if(bShowOverlay && bCopyFront)
		{
			int			xTileOffs,  yTileOffs;			// Tile-Verschiebung am Screen (0-19)
			char		red,green,blue,alpha;			// Komponenten des Tiles
			D3DCOLOR	Color;							// Farbe des Tiles

			// Offsets der Tiles berechnen (0-19)
			xTileOffs = (int)(XOffset*20) % TILESIZE_X;
			yTileOffs = (int)(YOffset*20) % TILESIZE_Y;

			lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);			// Alpha-Blending
			lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// für Colorkey
			lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// einschalten

			// Alle möglichen Tilesets durchmachen
			for(int k=0; k<MAX_TILESETS; k++)
			 for(int j=0; j<ClipY; j++)
  			  for(int i=0; i<ClipX; i++)
			{
				// normales tile (nicht verdecken)
				if(ClipBoard[i][j].TileSetFront == k &&
				   ClipBoard[i][j].FrontArt > 0 &&
				   !(ClipBoard[i][j].Block & BLOCKWERT_VERDECKEN))
				{
					if (ClipBoard[i][j].Block & BLOCKWERT_OVERLAY_LIGHT &&
						bShowLight)
					{
						red   = ClipBoard[i][j].Red;	// Farbe zusammensetzen
						green = ClipBoard[i][j].Green;
						blue  = ClipBoard[i][j].Blue;
						alpha = ClipBoard[i][j].Alpha;

						Color = D3DCOLOR_RGBA(red, green, blue, alpha);
					}
					else
						Color = D3DCOLOR_RGBA(255, 255, 255, ClipBoard[i][j].Alpha);

					int Type = ClipBoard[i][j].FrontArt;

					if (Type < 255) 
					pTileEngine->TileGfx[TilesetNummer[k]].RenderSprite(xm + (float)(i*20-xTileOffs), 
																		ym + (float)(j*20-yTileOffs), Type, Color);
				}			

				// Wasser
				if(k == 0 &&
				   (ClipBoard[i][j].Block & BLOCKWERT_WASSER ||
					ClipBoard[i][j].Block & BLOCKWERT_SUMPF))
				{
					int anim = i % 6 + (j % 6) * 6;

					DirectGraphics.SetColorKeyMode();
					LiquidGfx[0].RenderSprite(xm + (float)(i*20-xTileOffs), ym + (float)(j*20-yTileOffs), anim,Col1);

					DirectGraphics.SetAdditiveMode();
					LiquidGfx[1].RenderSprite(xm + (float)(i*20-xTileOffs), ym + (float)(j*20-yTileOffs), anim,Col2);

					DirectGraphics.SetColorKeyMode();
				}

				// normales tile (verdecken)
				if(ClipBoard[i][j].TileSetFront == k &&
				   ClipBoard[i][j].FrontArt > 0 &&
				   ClipBoard[i][j].Block & BLOCKWERT_VERDECKEN)
				{
					if (ClipBoard[i][j].Block & BLOCKWERT_OVERLAY_LIGHT &&
						bShowLight)
					{
						red   = ClipBoard[i][j].Red;	// Farbe zusammensetzen
						green = ClipBoard[i][j].Green;
						blue  = ClipBoard[i][j].Blue;
						alpha = ClipBoard[i][j].Alpha;
						Color = D3DCOLOR_RGBA(red, green, blue, alpha);
					}
					else
						Color = D3DCOLOR_RGBA(255, 255, 255, ClipBoard[i][j].Alpha);

					int Type = ClipBoard[i][j].FrontArt;

					if (Type < 255) 
						pTileEngine->TileGfx[TilesetNummer[k]].RenderSprite(xm + (float)(i*20-xTileOffs), 
																			ym + (float)(j*20-yTileOffs), Type, Color);
				}			
			}
		}
	}

	// Objekte ?
	if(bShowObjects)
		pObjectList->ShowAll();

	// Blockierungsarten anzeigen
	if(bShowBlock == true)
	for(int i=0; i<SCREENSIZE_X; i++)
	 for(int j=0; j<SCREENSIZE_Y; j++)
	 {
		int Art = pTileEngine->Tiles[XOffset+i][YOffset+j].Block;

		if(Art & BLOCKWERT_WAND)		   BlockArten.RenderSprite((float)i*20, (float)j*20, 1, 0xFFFFFFFF);
		if(Art & BLOCKWERT_GEGNERWAND)	   BlockArten.RenderSprite((float)i*20, (float)j*20, 2, 0xFFFFFFFF);
		if(Art & BLOCKWERT_OVERLAY_LIGHT)  BlockArten.RenderSprite((float)i*20, (float)j*20,15, 0xFFFFFFFF);
		if(Art & BLOCKWERT_PLATTFORM)	   BlockArten.RenderSprite((float)i*20, (float)j*20, 3, 0xFFFFFFFF);
		if(Art & BLOCKWERT_LIGHT)		   BlockArten.RenderSprite((float)i*20, (float)j*20, 4, 0xFFFFFFFF);
		if(Art & BLOCKWERT_ANIMIERT_BACK)  BlockArten.RenderSprite((float)i*20, (float)j*20, 6, 0xFFFFFFFF);
		if(Art & BLOCKWERT_ANIMIERT_FRONT) BlockArten.RenderSprite((float)i*20, (float)j*20, 7, 0xFFFFFFFF);
		if(Art & BLOCKWERT_WASSER)		   BlockArten.RenderSprite((float)i*20, (float)j*20, 8, 0xFFFFFFFF);
		if(Art & BLOCKWERT_MOVELINKS)		   BlockArten.RenderSprite((float)i*20, (float)j*20,14, 0xFFFFFFFF);
		if(Art & BLOCKWERT_SUMPF)		   BlockArten.RenderSprite((float)i*20, (float)j*20,16, 0xFFFFFFFF);
		if(Art & BLOCKWERT_EIS)			   BlockArten.RenderSprite((float)i*20, (float)j*20,17, 0xFFFFFFFF);
		if(Art & BLOCKWERT_MOVEVERTICAL)		   BlockArten.RenderSprite((float)i*20, (float)j*20,18, 0xFFFFFFFF);
		if(Art & BLOCKWERT_WASSERFALL)	   BlockArten.RenderSprite((float)i*20, (float)j*20,19, 0xFFFFFFFF);
		if(Art & BLOCKWERT_SCHADEN)		   BlockArten.RenderSprite((float)i*20, (float)j*20, 9, 0xFFFFFFFF);
		if(Art & BLOCKWERT_FLIESSBANDL)	   BlockArten.RenderSprite((float)i*20, (float)j*20,10, 0xFFFFFFFF);
		if(Art & BLOCKWERT_FLIESSBANDR)	   BlockArten.RenderSprite((float)i*20, (float)j*20,11, 0xFFFFFFFF);
		if(Art & BLOCKWERT_WENDEPUNKT)	   BlockArten.RenderSprite((float)i*20, (float)j*20,12, 0xFFFFFFFF);
		if(Art & BLOCKWERT_DESTRUCTIBLE)   BlockArten.RenderSprite((float)i*20, (float)j*20,13, 0xFFFFFFFF);
		if(Art & BLOCKWERT_VERDECKEN)	   BlockArten.RenderSprite((float)i*20, (float)j*20, 5, 0xFFFFFFFF);
		if(Art & BLOCKWERT_MOVERECHTS)	   BlockArten.RenderSprite((float)i*20, (float)j*20,20, 0xFFFFFFFF);
	 }

	// Darstellung beenden
	lpD3DDevice->EndScene();						

	RECT rectsource, rectdest;
	rectsource.left   = 0;		rectdest.left   = VIEW_XOFFSET;
	rectsource.right  = 640;	rectdest.right  = VIEW_XOFFSET+640;
	rectsource.top    = 0;		rectdest.top    = VIEW_YOFFSET;	
	rectsource.bottom = 480;	rectdest.bottom = VIEW_YOFFSET+480;
			
	lpD3DDevice->Present(&rectsource, &rectdest, 0, NULL);		// Frontbuffer anzeigen

	// Grid im Level anzeigen
	if(bShowGrid == true)
		for(int i=1; i<SCREENSIZE_X; i++)
		 for(int j=1; j<SCREENSIZE_Y; j++)
			 SetPixel(hdc, VIEW_XOFFSET+i*20, VIEW_YOFFSET+j*20, RGB(255, 255, 255));
//			 DirectGraphics.PutPixel(VIEW_XOFFSET+i*20, VIEW_YOFFSET+j*20, 
//									 255, 255, 255, lpBackbuffer);

	// Aktuelles Rechteck zeichnen
	SelectObject(hdc, Pens[0]);
	int x,y;

	//if (DirectInput.MausButtons[1] == true)
	if (ActRect.right > -1)
	{

		x = ActRect.right *20;
		y = ActRect.bottom*20;

		if (x > 639) x = 639;
		if (y > 479) y = 479;

		if (ActRect.left < 0) ActRect.left = 0;
		if (ActRect.top  < 0) ActRect.top  = 0;

		MoveToEx(hdc, VIEW_XOFFSET+ActRect.left*20,  VIEW_YOFFSET+ActRect.top*20, NULL);
		LineTo  (hdc, VIEW_XOFFSET+x,				 VIEW_YOFFSET+ActRect.top*20);
		LineTo  (hdc, VIEW_XOFFSET+x,				 VIEW_YOFFSET+y);
		LineTo  (hdc, VIEW_XOFFSET+ActRect.left*20,  VIEW_YOFFSET+y);
		LineTo  (hdc, VIEW_XOFFSET+ActRect.left*20,  VIEW_YOFFSET+ActRect.top*20);
	}
			 
//	SelectObject(hdc, Pens[1]);
//	SetBkColor(hdc, TRANSPARENT);


//	SelectObject(hdc, BlockBrush[1]);

}

// -------------------------------------------------------------------------------------- 
// Tile zeichnen
// -------------------------------------------------------------------------------------- 

void DrawTileset(void)
{
	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);			// Alpha-Blending
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// für Colorkey
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// einschalten

	// Mit dem Darstellen beginnen
	lpD3DDevice->BeginScene();						

	lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 0, 255), 1.0f, 0);

	if(TilesetNummer[ActualTileset]>0)
	{
		pTileEngine->TileGfx[TilesetNummer[ActualTileset]].SetRect(0, 0, 256, 256);
		pTileEngine->TileGfx[TilesetNummer[ActualTileset]].RenderSprite(0, 0, 0xFFFFFFFF);
	}

	// Darstellung beenden
	lpD3DDevice->EndScene();						

	RECT rectsource, rectdest;
	rectsource.left   = 0;		rectdest.left   = TILE_XOFFSET;
	rectsource.right  = 240;	rectdest.right  = TILE_XOFFSET+240;
	rectsource.top    = 0;		rectdest.top    = TILE_YOFFSET;	
	rectsource.bottom = 240;	rectdest.bottom = TILE_YOFFSET+240;
	lpD3DDevice->Present(&rectsource, &rectdest, 0, NULL);		// Frontbuffer anzeigen

	// Grid am Tile anzeigen
	for(int i=1; i<12; i++)
	  for(int j=1; j<12; j++)
		  SetPixel(hdc, TILE_XOFFSET+i*20-1, TILE_YOFFSET+j*20-1, RGB(255, 255, 255));
}

// -------------------------------------------------------------------------------------- 
// MiniMap anzeigen
// -------------------------------------------------------------------------------------- 

void DrawMiniMap(void)
{
	// MiniMap leeren
	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);			// Alpha-Blending
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// für Colorkey
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// einschalten

	// Mit dem Darstellen beginnen
	lpD3DDevice->BeginScene();						

	lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 32, 0), 1.0f, 0);

	// Darstellung beenden
	lpD3DDevice->EndScene();						

	RECT rectsource, rectdest;
	rectsource.left   = 0;		rectdest.left   = MINIMAP_XOFFSET;
	rectsource.right  = 129;	rectdest.right  = MINIMAP_XOFFSET+129;
	rectsource.top    = 0;		rectdest.top    = MINIMAP_YOFFSET;	
	rectsource.bottom = 97;		rectdest.bottom = MINIMAP_YOFFSET+97;
	lpD3DDevice->Present(&rectsource, &rectdest, 0, NULL);		// Frontbuffer anzeigen

	// MiniMap füllen
	for (int i=0; i<128; i++)
	 for (int j=0; j<96; j++)
		 if (XOffset+i-48 >= 0 &&
			 YOffset+j-36 >= 0)
	 {				  
		 if (pTileEngine->Tiles[XOffset+i-48][YOffset+j-36].Block & BLOCKWERT_WAND)
			SetPixel(hdc, MINIMAP_XOFFSET+i, MINIMAP_YOFFSET+j, RGB(255, 255, 255));
		 else
  		 if (pTileEngine->Tiles[XOffset+i-48][YOffset+j-36].Block & BLOCKWERT_PLATTFORM)
			SetPixel(hdc, MINIMAP_XOFFSET+i, MINIMAP_YOFFSET+j, RGB(128, 128, 128));

		 if (pTileEngine->Tiles[XOffset+i-48][YOffset+j-36].Block & BLOCKWERT_DESTRUCTIBLE)
			SetPixel(hdc, MINIMAP_XOFFSET+i, MINIMAP_YOFFSET+j, RGB(255, 255, 0));
	  }

	// Aktuelle Level Position markieren
	SelectObject(hdc, Pens[3]);
	SetBkColor(hdc, NULL);

	RECT rectselect;

	rectselect.left   = 48;
	rectselect.top    = 36;
	rectselect.right  = 48+32;
	rectselect.bottom = 36+24;

	MoveToEx(hdc, MINIMAP_XOFFSET+rectselect.left,  MINIMAP_YOFFSET+rectselect.top, NULL);
	LineTo  (hdc, MINIMAP_XOFFSET+rectselect.right, MINIMAP_YOFFSET+rectselect.top);
	LineTo  (hdc, MINIMAP_XOFFSET+rectselect.right, MINIMAP_YOFFSET+rectselect.bottom);
	LineTo  (hdc, MINIMAP_XOFFSET+rectselect.left,  MINIMAP_YOFFSET+rectselect.bottom);
	LineTo  (hdc, MINIMAP_XOFFSET+rectselect.left,  MINIMAP_YOFFSET+rectselect.top);
}

// -------------------------------------------------------------------------------------- 
// Infos des ausgewählten Objekts anzeigen
// ------------------------------------------------------------------------------------

void ShowObjectInfo(void)
{
	// Objekt ausgewählt ? Dann Eigenschaften übernehmen
	if(pSelectedObject != NULL)
	{
		if (pSelectedObject->Skill == 0)
			SendMessage(RadioButtonEasy, BM_SETCHECK, BST_CHECKED, 0);

		if (pSelectedObject->Skill == 1)
			SendMessage(RadioButtonMedium, BM_SETCHECK, BST_CHECKED, 0);

		 if (pSelectedObject->Skill == 2)
			SendMessage(RadioButtonHard, BM_SETCHECK, BST_CHECKED, 0);

		SendMessage(ComboBoxObjectType, CB_SETCURSEL, pSelectedObject->ObjectID, 0);
		if(pSelectedObject->ChangeLight == true) 
			SendMessage(BoxChangeLight, BM_SETCHECK, BST_CHECKED, 0);	else
			SendMessage(BoxChangeLight, BM_SETCHECK, BST_UNCHECKED, 0);

		_itoa(pSelectedObject->Value1, Buffer, 10);
		SendMessage(Value1Feld, WM_SETTEXT, 0, long(Buffer));

		_itoa(pSelectedObject->Value2, Buffer, 10);
		SendMessage(Value2Feld, WM_SETTEXT, 0, long(Buffer));

		// Objekt an Maus Position setzen, bis rechte Maustaste losgelassen wird
		/*while (DirectInput.MausButtons[1] == true)
		{
			POINT  ptCursor;

			GetCursorPos(&ptCursor);
			ScreenToClient(MainHWND, &ptCursor);

			pSelectedObject->XPos = int(ptCursor.x - VIEW_XOFFSET + pTileEngine->XOffset);
			pSelectedObject->YPos = int(ptCursor.y - VIEW_YOFFSET + pTileEngine->YOffset);

			DirectInput.MausUpdate(false);
		}*/
	}
	else

	// kein Objekt ? Dann auch keine Eigenschaften anzeigen
	SendMessage(BoxChangeLight,    BM_SETCHECK, BST_UNCHECKED, 0);

	RedrawLevel = true;
}

// -------------------------------------------------------------------------------------- 
// Maus bearbeiten, d.h. ggf Rects an der Position zeichnen oder Tiles auswählen/setzen
// -------------------------------------------------------------------------------------- 

void DoMouse(void)
{
	// Rechteck um aktuelles Tile zeichnen
	SetTextColor(hdc, RGB(rand()%50+200, rand()%50+200, rand()%50+200));
	SetBkColor(hdc, NULL);

	// Licht setzen ?
	if (MouseInLevel &&	
	    EditMode >= SMALLLIGHTMODE && 
		EditMode <=LARGESHADOWMODE && 
		DirectInput.MausButtons[0] == true)
	{
		SetFocus(0);

		if (UndoTriggered == false)
			SaveUndoStep(true);

		UndoTriggered = true;		

		int xm = (FensterMausX-VIEW_XOFFSET)/20+XOffset-4;
		int ym = (FensterMausY-VIEW_YOFFSET)/20+YOffset-4;

		for(int i=0; i<9; i++)
		 for(int j=0; j<9; j++)
		 {
			 int r,g,b;

			 // Position im Level errechnen
			 // Farbewerte auslesen
			 r = pTileEngine->Tiles[xm+i][ym+j].Red;
			 g = pTileEngine->Tiles[xm+i][ym+j].Green;
			 b = pTileEngine->Tiles[xm+i][ym+j].Blue;

			 int Wert = LightMap[LightUsed][i][j];

			 // Werte manipulieren
			 if(bLightRed)   { r += Wert; if (r > 255) r = 255; if (r < 0) r = 0; }
			 if(bLightGreen) { g += Wert; if (g > 255) g = 255; if (g < 0) g = 0; }
			 if(bLightBlue)  { b += Wert; if (b > 255) b = 255; if (b < 0) b = 0; }

			 // Neue Farbwerte setzen
			 pTileEngine->Tiles[xm+i][ym+j].Red   = (unsigned char) r;
			 pTileEngine->Tiles[xm+i][ym+j].Green = (unsigned char) g;
			 pTileEngine->Tiles[xm+i][ym+j].Blue  = (unsigned char) b;
		 }

		 RedrawLevel = true;
		 while (DirectInput.MausButtons[0] == true)
			 DirectInput.MausUpdate(false);
	}

	if(EditMode == LEVELMODE && MouseInLevel && DirectInput.MausButtons[1] == true)
	{
		ClipX = 0;
		ClipY = 0;
		SetFocus(0);
		POINT ptCursor;

		GetCursorPos(&ptCursor);
		ScreenToClient(MainHWND, &ptCursor);

		ActRect.left = (ptCursor.x-VIEW_XOFFSET)/20;
		ActRect.top  = (ptCursor.y-VIEW_YOFFSET)/20;

		// Richtige Blockierung des aktuellen Tiles setzen
		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_WAND)
			SendMessage(BoxBlockWertWand, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertWand, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_GEGNERWAND)
			SendMessage(BoxBlockWertGegnerWand, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertGegnerWand, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_PLATTFORM)
			SendMessage(BoxBlockWertPlattform, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertPlattform, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_LIGHT)
			SendMessage(BoxBlockWertLight, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertLight, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_VERDECKEN)
			SendMessage(BoxBlockWertVerdecken, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertVerdecken, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_ANIMIERT_BACK)
			SendMessage(BoxBlockWertAnimiertB, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertAnimiertB, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_ANIMIERT_FRONT)
			SendMessage(BoxBlockWertAnimiertF, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertAnimiertF, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_WASSER)
			SendMessage(BoxBlockWertWasser, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertWasser, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_SCHADEN)
			SendMessage(BoxBlockWertSchaden, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertSchaden, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_FLIESSBANDL)
			SendMessage(BoxBlockWertFliessBandL, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertFliessBandL, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_FLIESSBANDR)
			SendMessage(BoxBlockWertFliessBandR, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertFliessBandR, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_SCHADEN)
			SendMessage(BoxBlockWertSchaden, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertSchaden, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_WENDEPUNKT)
			SendMessage(BoxBlockWertWendePunkt, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertWendePunkt, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_DESTRUCTIBLE)
			SendMessage(BoxBlockWertDestructible, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertDestructible, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_MOVELINKS)
			SendMessage(BoxBlockWertSaeure, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertSaeure, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_OVERLAY_LIGHT)
			SendMessage(BoxBlockWertOverlay_Light, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertOverlay_Light, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_SUMPF)
			SendMessage(BoxBlockWertLava, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertLava, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_EIS)
			SendMessage(BoxBlockWertEis, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertEis, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_MOVEVERTICAL)
			SendMessage(BoxBlockWertMagen, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertMagen, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_WASSERFALL)
			SendMessage(BoxBlockWertWasserFall, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertWasserFall, BM_SETCHECK, BST_UNCHECKED, 0);

		if (pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Block & BLOCKWERT_MOVERECHTS)
			SendMessage(BoxBlockWertTexRechts, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxBlockWertTexRechts, BM_SETCHECK, BST_UNCHECKED, 0);

		RedrawLevel = true;
	}

	// Tilestück aussuchen im Tileset Fenster rechts oben
	// solange warten, bis Maus losgelassen wurde und dann den markierten
	// Bereich ins Clipboard kopieren

	if(EditMode == LEVELMODE && MouseInTile && DirectInput.MausButtons[0] == true)
	{
		SetFocus(0);
		RECT rectselect;
//		int  swap;
			
		ClipOffX = 0;
		ClipOffY = 0;

		rectselect.left = (FensterMausX-TILE_XOFFSET)/20*20;
		rectselect.top  = (FensterMausY-TILE_YOFFSET)/20*20;

		while(DirectInput.MausButtons[0] == true)
		{
			POINT ptCursor;

			SelectObject(hdc, Pens[0]);

			GetCursorPos(&ptCursor);
			ScreenToClient(MainHWND, &ptCursor);

			rectselect.right  = (ptCursor.x-TILE_XOFFSET+20)/20*20;
			rectselect.bottom = (ptCursor.y-TILE_YOFFSET+20)/20*20;

			if (rectselect.right  < 0)		rectselect.right  = 0;
			if (rectselect.bottom < 0)		rectselect.bottom = 0;
			if (rectselect.right  > 240)	rectselect.right  = 240;
			if (rectselect.bottom > 240)	rectselect.bottom = 240;

			/*if (rectselect.right < rectselect.left)			// Ecken vertauschen
			{
				swap = rectselect.left;
				rectselect.left  = rectselect.right;
				rectselect.right = swap;
			}

			if (rectselect.bottom < rectselect.top)	
			{
				swap = rectselect.top;
				rectselect.top    = rectselect.bottom;
				rectselect.bottom = swap;
			}*/
				
			DirectInput.MausUpdate(false);

			DrawTileset();
			MoveToEx(hdc, TILE_XOFFSET+rectselect.left,  TILE_YOFFSET+rectselect.top, NULL);
			LineTo  (hdc, TILE_XOFFSET+rectselect.right, TILE_YOFFSET+rectselect.top);
			LineTo  (hdc, TILE_XOFFSET+rectselect.right, TILE_YOFFSET+rectselect.bottom);
			LineTo  (hdc, TILE_XOFFSET+rectselect.left,  TILE_YOFFSET+rectselect.bottom);
			LineTo  (hdc, TILE_XOFFSET+rectselect.left,  TILE_YOFFSET+rectselect.top);
		}

		// Markierten Bereich nochmal umrahmen
		SelectObject(hdc, Pens[0]);

		if (rectselect.right  < 0)		rectselect.right  = 0;
		if (rectselect.bottom < 0)		rectselect.bottom = 0;
		if (rectselect.right  > 240)	rectselect.right  = 240;
		if (rectselect.bottom > 240)	rectselect.bottom = 240;

		MoveToEx(hdc, TILE_XOFFSET+rectselect.left,  TILE_YOFFSET+rectselect.top, NULL);
		LineTo  (hdc, TILE_XOFFSET+rectselect.right, TILE_YOFFSET+rectselect.top);
		LineTo  (hdc, TILE_XOFFSET+rectselect.right, TILE_YOFFSET+rectselect.bottom);
		LineTo  (hdc, TILE_XOFFSET+rectselect.left,  TILE_YOFFSET+rectselect.bottom);
		LineTo  (hdc, TILE_XOFFSET+rectselect.left,  TILE_YOFFSET+rectselect.top);

		// Kanten des gewählten Bereichs
		// auf Tile-Ebene umrechnen
		rectselect.right  /= 20;
		rectselect.left   /= 20;
		rectselect.top    /= 20;
		rectselect.bottom /= 20;

		ClipX = rectselect.right  - rectselect.left;	// Grösse des Clip-Bereichs 
		ClipY = rectselect.bottom - rectselect.top;		// errechnen

		for (int i=0; i< ClipX; i++)					// Clipboard füllen
		 for (int j=0; j< ClipY; j++)
		 {
			 ClipBoard[i][j].BackArt	  = (unsigned char)((j+rectselect.top)*12+i+rectselect.left);
			 ClipBoard[i][j].FrontArt	  = (unsigned char)((j+rectselect.top)*12+i+rectselect.left);
			 ClipBoard[i][j].Red		  = 255;
			 ClipBoard[i][j].Green		  = 255;
			 ClipBoard[i][j].Blue		  = 255;
			 ClipBoard[i][j].Alpha		  = 255;
			 ClipBoard[i][j].Block		  = 0;
			 ClipBoard[i][j].TileSetBack  = ActualTileset;
			 ClipBoard[i][j].TileSetFront = ActualTileset;
		 }


 		ActRect.left   = (FensterMausX-VIEW_XOFFSET)/20;
		ActRect.top    = (FensterMausY-VIEW_YOFFSET)/20;
		ActRect.right  = ActRect.left + ClipX;
		ActRect.bottom = ActRect.top  + ClipY;

		// Offsets für KeepTileOffset auf "ungesetzt" setzen, damit beim ersten Setzen des Tiles die
		// Werte übernommen werden
		KeepX = -1;
		KeepY = -1;		

		// und ins Clipboard kopieren
		SaveClipboard();
	}

	//
	// Clipboard an der aktuellen Stelle im Level zeichnen
	//

	if (EditMode == LEVELMODE && MouseInLevel && DirectInput.MausButtons[0] == true && ClipX > 0)
	{
		SetFocus(0);
		int xm, ym;
		POINT ptCursor;

		if (UndoTriggered == false)
			SaveUndoStep(true);

		UndoTriggered = true;		

		LoadClipboard();

		GetCursorPos(&ptCursor);
		ScreenToClient(MainHWND, &ptCursor);

		xm = (ptCursor.x-VIEW_XOFFSET)/20;
		ym = (ptCursor.y-VIEW_YOFFSET)/20;

		int xs = 0, ys = 0;

		// tileoffset merken
		if (bKeepTileOffset == true && ClipX > 0 && ClipY > 0)
		{
			xs = (xm+XOffset) % ClipX;
			ys = (ym+YOffset) % ClipY;
		}

		// Tile vom Clipboard ins Level kopieren
		int xoff, xoff2;
		int yoff, yoff2;

		// Offsetwerte für KeepTileOffset beim ersten Klick ins Level neu setzen
		if (KeepX == -1 &&
			ClipX > 0   &&
			ClipY > 0)
		{
			KeepX = (xm+XOffset) % ClipX;
			KeepY = (ym+YOffset) % ClipY;
		}

		if (bKeepTileOffset == false)
		{
			KeepX = KeepY = 0;
		}

		for (int i=0; i<ClipX; i++)
		 for (int j=0; j<ClipY; j++)
			{				
				// Offset im Level
				xoff = xm+XOffset+i;
				yoff = ym+YOffset+j;

				// Offset innerhalb des ClipBoards
				xoff2 = xs + i - KeepX;
				yoff2 = ys + j - KeepY;

				// Grenzen des Clipboard checken
				if (xoff2 >= ClipX) xoff2 -= ClipX;
				if (yoff2 >= ClipY) yoff2 -= ClipY;

				if (xoff2 < 0) xoff2 += ClipX;
				if (yoff2 < 0) yoff2 += ClipY;

				if(bCopyBack && bShowLevel)	
				{	
					pTileEngine->Tiles[xoff][yoff].BackArt 		= ClipBoard[xoff2][yoff2].BackArt + INCLUDE_ZEROTILE;
					pTileEngine->Tiles[xoff][yoff].TileSetBack	= ClipBoard[xoff2][yoff2].TileSetBack;
				}

				if(bCopyFront && bShowOverlay)	
				{
					pTileEngine->Tiles[xoff][yoff].FrontArt 	= ClipBoard[xoff2][yoff2].FrontArt + INCLUDE_ZEROTILE;
					pTileEngine->Tiles[xoff][yoff].TileSetFront	= ClipBoard[xoff2][yoff2].TileSetFront;
				}

				if(bCopyLight)
				{
					pTileEngine->Tiles[xoff][yoff].Red		= ClipBoard[xoff2][yoff2].Red;
					pTileEngine->Tiles[xoff][yoff].Green	= ClipBoard[xoff2][yoff2].Green;
					pTileEngine->Tiles[xoff][yoff].Blue		= ClipBoard[xoff2][yoff2].Blue;
					pTileEngine->Tiles[xoff][yoff].Alpha	= ClipBoard[xoff2][yoff2].Alpha;
				}

				if(bCopyBlock)
					pTileEngine->Tiles[xoff][yoff].Block	= ClipBoard[xoff2][yoff2].Block;				
			}
		 
		 ActRect.left   = (FensterMausX-VIEW_XOFFSET)/20;
		 ActRect.top    = (FensterMausY-VIEW_YOFFSET)/20;
		 ActRect.right  = ActRect.left + ClipX;
		 ActRect.bottom = ActRect.top  + ClipY;
		 RedrawLevel = true;
	}

	if (UndoTriggered && DirectInput.MausButtons[0] == false)
	{
		UndoTriggered = false;		
	}

	//
	// Ausschnitt aus dem Level ins Clipboard kopieren
	//

	if(EditMode == LEVELMODE && MouseInLevel && DirectInput.MausButtons[1] == true)
	{
		SetFocus(0);
		RECT rectselect;

		rectselect.left = (FensterMausX-VIEW_XOFFSET)/20*20;
		rectselect.top  = (FensterMausY-VIEW_YOFFSET)/20*20;

		ClipOffX = rectselect.left + XOffset * 20;
		ClipOffY = rectselect.top  + YOffset * 20;

		while(DirectInput.MausButtons[1] == true)
		{
			POINT ptCursor;

			GetCursorPos(&ptCursor);
			ScreenToClient(MainHWND, &ptCursor);

			rectselect.right  = (ptCursor.x-VIEW_XOFFSET+20)/20*20;
			rectselect.bottom = (ptCursor.y-VIEW_YOFFSET+20)/20*20;

			if (rectselect.right  < 0)
				rectselect.right  = 0;
			if (rectselect.bottom < 0)
				rectselect.bottom = 0;
			if (rectselect.right  > 640)
				rectselect.right  = 640;
			if (rectselect.bottom > 480)
				rectselect.bottom = 480;

			/*if (rectselect.right < rectselect.left)			// Ecken vertauschen
			{
				swap = rectselect.left;
				rectselect.left  = rectselect.right;
				rectselect.right = swap;
			}

			if (rectselect.bottom < rectselect.top)	
			{
				swap = rectselect.top;
				rectselect.top    = rectselect.bottom;
				rectselect.bottom = swap;
			}*/
			ActRect.left   = rectselect.left  /20;
			ActRect.right  = rectselect.right /20;
			ActRect.top    = rectselect.top   /20;
			ActRect.bottom = rectselect.bottom/20;
				
			DirectInput.MausUpdate(false);
			RedrawLevel = true;
			DrawLevel();
		}

		// Kanten des gewählten Bereichs
		// auf Level-Ebene umrechnen
		rectselect.right  /= 20;
		rectselect.left   /= 20;
		rectselect.top    /= 20;
		rectselect.bottom /= 20;

		ClipX = rectselect.right  - rectselect.left;	// Grösse des Clip-Bereichs 
		ClipY = rectselect.bottom - rectselect.top;		// errechnen		

		for (int i=0; i< ClipX; i++)					// Clipboard füllen
		 for (int j=0; j< ClipY; j++)
		 {
			 ClipBoard[i][j].BackArt = 
			 pTileEngine->Tiles[i+XOffset+rectselect.left]
							   [j+YOffset+rectselect.top].BackArt-INCLUDE_ZEROTILE;

			 ClipBoard[i][j].FrontArt = 
			 pTileEngine->Tiles[i+XOffset+rectselect.left]
							   [j+YOffset+rectselect.top].FrontArt-INCLUDE_ZEROTILE;

			 ClipBoard[i][j].Red = 
			 pTileEngine->Tiles[i+XOffset+rectselect.left]
							   [j+YOffset+rectselect.top].Red;

			 ClipBoard[i][j].Green = 
			 pTileEngine->Tiles[i+XOffset+rectselect.left]
							   [j+YOffset+rectselect.top].Green;

			 ClipBoard[i][j].Blue = 
			 pTileEngine->Tiles[i+XOffset+rectselect.left]
							   [j+YOffset+rectselect.top].Blue;

			 ClipBoard[i][j].Alpha = 
			 pTileEngine->Tiles[i+XOffset+rectselect.left]
							   [j+YOffset+rectselect.top].Alpha;

			 ClipBoard[i][j].Block = 
			 pTileEngine->Tiles[i+XOffset+rectselect.left]
							   [j+YOffset+rectselect.top].Block;

			 ClipBoard[i][j].TileSetBack =
			 pTileEngine->Tiles[i+XOffset+rectselect.left]
							   [j+YOffset+rectselect.top].TileSetBack;

			 ClipBoard[i][j].TileSetFront =
			 pTileEngine->Tiles[i+XOffset+rectselect.left]
							   [j+YOffset+rectselect.top].TileSetFront;
		 }	

		// Offsets für KeepTileOffset errechnen, damit die folgenden Tiles passend zur eben gewählten Auswahl
		// gesetzt werden
		 if (ClipX > 0 && ClipY > 0)
		 {
			KeepX = rectselect.left % ClipX;
			KeepY = rectselect.top  % ClipY;

			KeepX = -1;
			KeepY = -1;
		 }

		 // und ins Clipboard kopieren
		SaveClipboard();
	}

	

	if (EditMode == OBJECTMODE && MouseInLevel == true)
	{
		SetFocus(0);

		// Maus Position merken
		POINT ptCursor;

		GetCursorPos(&ptCursor);
		ScreenToClient(MainHWND, &ptCursor);		

		//
		// Neues Objekt ins Level setzen
		//
		if (KeyDown(DIK_INSERT))
		{
			while (KeyDown(DIK_INSERT))
				DirectInput.TastaturUpdate();					

			int type;

			type = SendMessage(ComboBoxObjectType, CB_GETCURSEL, 0, 0);

			pObjectList->Push((int)(ptCursor.x) - VIEW_XOFFSET + XOffset*20,
							  (int)(ptCursor.y) - VIEW_YOFFSET + YOffset*20,
							  type);

			// Objekt dupliziert ? Dann Eigenschaften übernehmen
			if(pSelectedObject != NULL)
			{
				pObjectList->pEnd->ObjectID		= pSelectedObject->ObjectID;
				pObjectList->pEnd->Skill		= pSelectedObject->Skill;
				pObjectList->pEnd->ChangeLight	= pSelectedObject->ChangeLight;
			}

			pSelectedObject = pObjectList->pEnd;

			// Warten, bis Maus wieder losgelassen wurde
			while (DirectInput.MausButtons[0] == true)
				DirectInput.MausUpdate(false);

			RedrawLevel = true;
		}
		else
		if (DirectInput.MausButtons[0] == true &&
			pSelectedObject != NULL)
		{
			pSelectedObject->XPos = (float)(ptCursor.x) - VIEW_XOFFSET + XOffset*20;
			pSelectedObject->YPos = (float)(ptCursor.y) - VIEW_YOFFSET + YOffset*20;

			RedrawLevel = true;
		}
	}

	//
	// Objekt auswählen (rechte Maustaste)
	//

	if (EditMode == OBJECTMODE && MouseInLevel == true && DirectInput.MausButtons[1] == true)
	{
		SetFocus(0);
		// Radiobuttons leeren 
		SendMessage(RadioButtonEasy,   BM_SETCHECK, BST_UNCHECKED, 0);
		SendMessage(RadioButtonMedium, BM_SETCHECK, BST_UNCHECKED, 0);
		SendMessage(RadioButtonHard,   BM_SETCHECK, BST_UNCHECKED, 0);

		pObjectList->GetSelected();
		ShowObjectInfo();		
	}
	
}

// -------------------------------------------------------------------------------------- 
// Tasten abfragen (Level scrollen, Hotkeys usw)
// -------------------------------------------------------------------------------------- 

void DoKeys(void)
{

	// Clipboard löschen
	if (KeyDown(DIK_ESCAPE))
	{
		ClipX = 0;
		ClipY = 0;
		ActRect.right = -1;
	}

	// undo
	if(KeyDown(DIK_U))
	{
		if (KeyDown(DIK_LSHIFT) || KeyDown(DIK_RSHIFT))
			RestoreUndoStep(false);
		else
			RestoreUndoStep(true);
	}

//----- F2 - speichern / F3 - laden

	if(KeyDown(DIK_F2))
	{
		while (KeyDown(DIK_F2))
			DirectInput.TastaturUpdate();
		SaveLevel(false);
		RedrawLevel = true;
	}

	if(KeyDown(DIK_F3))
	{
		while (KeyDown(DIK_F3))
			DirectInput.TastaturUpdate();
		LoadLevel();
		RedrawLevel = true;
	}

//----- Taste "M" Edit-Mode wechseln

	if(KeyDown(DIK_M))
	{
		while(KeyDown(DIK_M)) DirectInput.TastaturUpdate();
		if (EditMode == LEVELMODE)
		{ EditMode = OBJECTMODE; SendMessage(ButtonEditMode,WM_SETTEXT,0,(long)"Object-Mode");	}
		else
		{EditMode = LEVELMODE; 	SendMessage(ButtonEditMode,WM_SETTEXT,0,(long)"Level-Mode");	}

		RedrawLevel = true;
	}

//----- Tasten Shift + 1-6 View-Optionen ein/aus schalten

	if (GetFocus() == NULL)
	{

		// Taste 1 = Grid
		if(KeyDown(DIK_1))
		{
			while(KeyDown(DIK_1)) DirectInput.TastaturUpdate();
			if  (bShowGrid == true)
			{ SendMessage(BoxShowGrid, BM_SETCHECK, BST_UNCHECKED, 0); bShowGrid = false;	} else 
			{ SendMessage(BoxShowGrid, BM_SETCHECK, BST_CHECKED,   0); bShowGrid = true;	}
			RedrawLevel = true;
		}

		// Taste 2 = Background
		if(KeyDown(DIK_2))
		{
			while(KeyDown(DIK_2)) DirectInput.TastaturUpdate();
			if  (bShowBackground == true)
			{ SendMessage(BoxShowBackground, BM_SETCHECK, BST_UNCHECKED, 0); bShowBackground = false;	} else 
			{ SendMessage(BoxShowBackground, BM_SETCHECK, BST_CHECKED,   0); bShowBackground = true;	}
			RedrawLevel = true;
		}
	
		// Taste 3 = Level
		if(KeyDown(DIK_3))
		{
			while(KeyDown(DIK_3)) DirectInput.TastaturUpdate();
			if  (bShowLevel == true)
			{ SendMessage(BoxShowLevel, BM_SETCHECK, BST_UNCHECKED, 0); bShowLevel = false;	} else 
			{ SendMessage(BoxShowLevel, BM_SETCHECK, BST_CHECKED,   0); bShowLevel = true;	}
			RedrawLevel = true;
		}

		// Taste 4 = Overlay
		if(KeyDown(DIK_4))
		{
			while(KeyDown(DIK_4)) DirectInput.TastaturUpdate();
			if  (bShowOverlay == true)
			{ SendMessage(BoxShowOverlay, BM_SETCHECK, BST_UNCHECKED, 0); bShowOverlay = false;	} else 
			{ SendMessage(BoxShowOverlay, BM_SETCHECK, BST_CHECKED,   0); bShowOverlay = true;	}
			RedrawLevel = true;
		}

		// Taste 5 = Block
		if(KeyDown(DIK_5))
		{
			while(KeyDown(DIK_5)) DirectInput.TastaturUpdate();
			if  (bShowBlock == true)
			{ SendMessage(BoxShowBlock, BM_SETCHECK, BST_UNCHECKED, 0); bShowBlock = false;	} else 
			{ SendMessage(BoxShowBlock, BM_SETCHECK, BST_CHECKED,   0); bShowBlock = true;	}
			RedrawLevel = true;
		}

		// Taste 6 = Objekte
		if(KeyDown(DIK_6))
		{
			while(KeyDown(DIK_6)) DirectInput.TastaturUpdate();
			if  (bShowObjects == true)
			{ SendMessage(BoxShowObjects, BM_SETCHECK, BST_UNCHECKED, 0); bShowObjects = false;	} else 
			{ SendMessage(BoxShowObjects, BM_SETCHECK, BST_CHECKED,   0); bShowObjects = true;	}
			RedrawLevel = true;
		}

		// Taste 7 = Background kopieren
		if(KeyDown(DIK_7))
		{
			while(KeyDown(DIK_7)) DirectInput.TastaturUpdate();
			if  (bCopyBack == true)
			{ SendMessage(BoxCopyBack, BM_SETCHECK, BST_UNCHECKED, 0); bCopyBack = false;	} else 
			{ SendMessage(BoxCopyBack, BM_SETCHECK, BST_CHECKED,   0); bCopyBack = true;	}
			RedrawLevel = true;
		}

		// Taste 8 = Overlay kopieren
		if(KeyDown(DIK_8))
		{
			while(KeyDown(DIK_8)) DirectInput.TastaturUpdate();
			if  (bCopyFront == true)
			{ SendMessage(BoxCopyFront, BM_SETCHECK, BST_UNCHECKED, 0); bCopyFront = false;	} else 
			{ SendMessage(BoxCopyFront, BM_SETCHECK, BST_CHECKED,   0); bCopyFront = true;	}
			RedrawLevel = true;
		}

		// Taste 9 = Light kopieren
		if(KeyDown(DIK_9))
		{
			while(KeyDown(DIK_9)) DirectInput.TastaturUpdate();
			if  (bCopyLight == true)
			{ SendMessage(BoxCopyLight, BM_SETCHECK, BST_UNCHECKED, 0); bCopyLight = false;	} else 
			{ SendMessage(BoxCopyLight, BM_SETCHECK, BST_CHECKED,   0); bCopyLight = true;	}
			RedrawLevel = true;
		}

		// Taste 0 = Block kopieren
		if(KeyDown(DIK_0))
		{
			while(KeyDown(DIK_0)) DirectInput.TastaturUpdate();
			if  (bCopyBlock == true)
			{ SendMessage(BoxCopyBlock, BM_SETCHECK, BST_UNCHECKED, 0); bCopyBlock = false;	} else 
			{ SendMessage(BoxCopyBlock, BM_SETCHECK, BST_CHECKED,   0); bCopyBlock = true;	}
			RedrawLevel = true;
		}
	}

//----- Screenshot machen ?

	if(KeyDown(DIK_F12)) DirectGraphics.TakeScreenshot("HurricanEditor");

//----- Special

	
	// F5 zum nächsten Objekt jumpen
	if (KeyDown(DIK_F5) &&
		pSelectedObject != NULL)
	{
		Object *pSave, *pTemp;

		pSave = pSelectedObject;
		pTemp = pSave->pNext;

		while (pTemp != pSave)
		{
			if (pTemp == NULL)
			{
				pTemp = pObjectList->pStart;
				if (pTemp == NULL)
					break;
			}

			// Objekt gefunden? Dann auswählen und zentrieren
			if (pTemp->ObjectID == pSave->ObjectID)
			{
				pSelectedObject = pTemp;
				RedrawLevel = true;

				XOffset = (pSelectedObject->XPos - 320.0f) * TILEMUL_X;
				YOffset = (pSelectedObject->YPos - 200.0f )* TILEMUL_X;

				if (XOffset < 0.0f)
					XOffset = 0.0;

				if (YOffset < 0.0f)
					YOffset = 0.0;

				break;
			}	

			pTemp = pTemp->pNext;
		}

		while (KeyDown(DIK_F5))
			DirectInput.TastaturUpdate();

		ShowObjectInfo();
	}


//----- Level verschieben

	// Level hoch scrollen
	if (KeyDown(DIK_NUMPAD2))
	{				
		for(int i=0; i<LevelSizeX; i += 1)
		 for(int j=LevelSizeY; j>1; j -= 1)
			pTileEngine->Tiles[i][j] = pTileEngine->Tiles[i][j-1];
		
		Object		*pTemp = pObjectList->pStart;			// Zeiger auf das erste Objekt
		while (pTemp != NULL)					// Ende der Liste erreicht ?
		{				
			pTemp->YPos += 20;
			pTemp = pTemp->pNext;				// Zeiger auf das nächste Objekt richten    
		}

		while (KeyDown(DIK_NUMPAD2))
			DirectInput.TastaturUpdate();

		RedrawLevel = true;
	}

	// Level runter scrollen
	if (KeyDown(DIK_NUMPAD8))
	{				
		for(int i=0; i<LevelSizeX; i += 1)
		 for(int j=0; j< LevelSizeY - 1; j += 1)
			pTileEngine->Tiles[i][j] = pTileEngine->Tiles[i][j+1];
		
		Object		*pTemp = pObjectList->pStart;			// Zeiger auf das erste Objekt
		while (pTemp != NULL)					// Ende der Liste erreicht ?
		{				
			pTemp->YPos -= 20;
			pTemp = pTemp->pNext;				// Zeiger auf das nächste Objekt richten    
		}

		while (KeyDown(DIK_NUMPAD8))
			DirectInput.TastaturUpdate();

		RedrawLevel = true;
	}

	// Level links scrollen
	if (KeyDown(DIK_NUMPAD6))
	{				
		for(int i=LevelSizeX; i>1; i -= 1)
		  for(int j=0; j<LevelSizeY; j += 1)		 
			pTileEngine->Tiles[i][j] = pTileEngine->Tiles[i-1][j];
		
		Object		*pTemp = pObjectList->pStart;			// Zeiger auf das erste Objekt
		while (pTemp != NULL)					// Ende der Liste erreicht ?
		{				
			pTemp->XPos += 20;
			pTemp = pTemp->pNext;				// Zeiger auf das nächste Objekt richten    
		}

		while (KeyDown(DIK_NUMPAD6))
			DirectInput.TastaturUpdate();

		RedrawLevel = true;
	}

	// Level rechts scrollen
	if (KeyDown(DIK_NUMPAD4))
	{				
		for(int i=0; i< LevelSizeX - 1; i += 1)
		  for(int j=0; j<LevelSizeY; j += 1)		 
			pTileEngine->Tiles[i][j] = pTileEngine->Tiles[i+1][j];
		
		Object		*pTemp = pObjectList->pStart;			// Zeiger auf das erste Objekt
		while (pTemp != NULL)					// Ende der Liste erreicht ?
		{				
			pTemp->XPos -= 20;
			pTemp = pTemp->pNext;				// Zeiger auf das nächste Objekt richten    
		}

		while (KeyDown(DIK_NUMPAD4))
			DirectInput.TastaturUpdate();

		RedrawLevel = true;
	}

//----- Level scrollen, egal in welchem Edit Modus ? (danach neu zeichnen)

	if (pSelectedObject == NULL)
	{
		// Rechts
		if (KeyDown(DIK_RIGHT) && XOffset < LevelSizeX-SCREENSIZE_X)
		{	
			SetFocus(MainHWND);
			XOffset += 1;
			RedrawLevel = true;
			DrawMiniMap();
		}

		// Links
		if (KeyDown(DIK_LEFT) && XOffset > 0)
		{	
			SetFocus(MainHWND);
			XOffset -= 1;
			RedrawLevel = true;
			DrawMiniMap();
		}

		// Unten
		if (KeyDown(DIK_DOWN) && YOffset < LevelSizeY-SCREENSIZE_Y)
		{	
			SetFocus(MainHWND);
			YOffset += 1;
			RedrawLevel = true;
			DrawMiniMap();
		}

		// Oben
		if (KeyDown(DIK_UP) && YOffset > 0)
		{	
			SetFocus(MainHWND);
			YOffset -= 1;
			RedrawLevel = true;
			DrawMiniMap();
		}
	}

//----- Akuell gewählten Level-Ausschitt löschen ?

	if(EditMode == LEVELMODE && KeyDown(DIK_DELETE))
	{
		for(int i=0; i<ClipX; i++)
		 for(int j=0; j<ClipY; j++)
		 {
			 if(bCopyBack)  pTileEngine->Tiles[ActRect.left+XOffset+i][ActRect.top+YOffset+j].BackArt = 0;
			 if(bCopyFront)	pTileEngine->Tiles[ActRect.left+XOffset+i][ActRect.top+YOffset+j].FrontArt = 0;
			 if(bCopyLight) pTileEngine->Tiles[ActRect.left+XOffset+i][ActRect.top+YOffset+j].Alpha = 255;
			 if(bCopyLight) pTileEngine->Tiles[ActRect.left+XOffset+i][ActRect.top+YOffset+j].Red = 255;
			 if(bCopyLight) pTileEngine->Tiles[ActRect.left+XOffset+i][ActRect.top+YOffset+j].Green = 255;
			 if(bCopyLight) pTileEngine->Tiles[ActRect.left+XOffset+i][ActRect.top+YOffset+j].Blue = 255;
			 if(bCopyBack)  pTileEngine->Tiles[ActRect.left+XOffset+i][ActRect.top+YOffset+j].TileSetBack = 0;
			 if(bCopyFront) pTileEngine->Tiles[ActRect.left+XOffset+i][ActRect.top+YOffset+j].TileSetFront = 0;
			 if(bCopyBlock) pTileEngine->Tiles[ActRect.left+XOffset+i][ActRect.top+YOffset+j].Block = 0;
		 }
		RedrawLevel = true;
	}


//----- Aktuell gewähltes Objekt bewegen ? (mit SHIFT == langsam bewegen)

	if(EditMode == OBJECTMODE && pSelectedObject != NULL)
	{
		// Links
		if (KeyDown(DIK_LEFT))
		{
			SetFocus(MainHWND);
			pSelectedObject->XPos--;
			RedrawLevel = true;
		}

		// Rechts
		if (KeyDown(DIK_RIGHT))
		{
			SetFocus(MainHWND);
			pSelectedObject->XPos++;
			RedrawLevel = true;
		}

		// Oben
		if (KeyDown(DIK_UP))
		{
			SetFocus(MainHWND);
			pSelectedObject->YPos--;
			RedrawLevel = true;
		}

		// Unten
		if (KeyDown(DIK_DOWN))
		{
			SetFocus(MainHWND);
			pSelectedObject->YPos++;
			RedrawLevel = true;
		}

//----- Objekt löschen ?

		if (KeyDown(DIK_DELETE))
		{
			pObjectList->DelSel(pSelectedObject);
			pSelectedObject = NULL;
			RedrawLevel = true;
		}
	}
}

// -------------------------------------------------------------------------------------- 
// Werte des Level wie zB Position anzeigen
// -------------------------------------------------------------------------------------- 

void ShowValues(void)
{
	// alte Werte löschen
	SetBkColor  (hdc, RGB(255, 255, 255));
	SelectObject(hdc, Pens[2]);
	SelectObject(hdc, BlockBrush[0]);

	Rectangle(hdc, 208, 45, 250, 61);			// X-Offset im  Level
	Rectangle(hdc, 208, 65, 250, 81);			// Y-Offset im  Level
	Rectangle(hdc, 695, 360, 730, 376);			// Rot   Wert
	Rectangle(hdc, 695, 380, 730, 396);			// Grün  Wert
	Rectangle(hdc, 695, 400, 730, 416);			// Blau  Wert
	Rectangle(hdc, 695, 420, 730, 436);			// Alpha Wert
	Rectangle(hdc, 880, 66, 915, 82);			// Tileset Nummer

	Rectangle(hdc, 960, 25, 1000, 41);			// Back  TilesetNr
	Rectangle(hdc, 960, 40, 1000, 56);			// Front TilesetNr

	// Objekt-Werte löschen

	Rectangle(hdc, 895, 615, 950, 631);			// Anzahl
	Rectangle(hdc, 700, 651, 760, 667);			// X-Pos
	Rectangle(hdc, 700, 665, 760, 680);			// Y-Pos

	SetTextColor(hdc, RGB(0, 0, 0));
    SetBkColor(hdc, RGB(0,0,0));
    SetBkMode(hdc, TRANSPARENT);

	// Farbwert des aktuellen Tiles
	TextOut(hdc, 680, 440, "Alle", strlen("Alle"));

	TextOut(hdc, 220, 20, "Liquid RGBA", strlen("Liquid RGBA"));

	_itoa((int)(pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Red), Buffer, 10);
	TextOut(hdc, 680, 360, "R", strlen("R"));
	TextOut(hdc, 700, 360, Buffer, strlen(Buffer));

	_itoa(pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Green, Buffer, 10);
	TextOut(hdc, 680, 380, "G", strlen("G"));
	TextOut(hdc, 700, 380, Buffer, strlen(Buffer));

	_itoa(pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Blue, Buffer, 10);
	TextOut(hdc, 680, 400, "B", strlen("B"));
	TextOut(hdc, 700, 400, Buffer, strlen(Buffer));

	_itoa(pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].Alpha, Buffer, 10);
	TextOut(hdc, 680, 420, "A", strlen("A"));
	TextOut(hdc, 700, 420, Buffer, strlen(Buffer));

	// aktuelle Tileset NR anzeigen
	_itoa(ActualTileset, Buffer, 10);
	TextOut(hdc, 845, 66, "NR :", strlen("NR :"));
	TextOut(hdc, 890, 66, Buffer, strlen(Buffer));

	// Levelgrösse anzeigen
	TextOut(hdc,10, 45, "X-Size :", strlen("X-Size :"));
	TextOut(hdc,10, 65, "Y-Size :", strlen("Y-Size :"));

	// LevelPosition anzeigen
	_itoa(XOffset, Buffer, 10);
	TextOut(hdc,130, 45, "X-Offset :", strlen("X-Offset :"));
	TextOut(hdc,210, 45, Buffer, strlen(Buffer));
	
	_itoa(YOffset, Buffer, 10);
	TextOut(hdc,130, 65, "Y-Offset :", strlen("Y-Offset :"));
	TextOut(hdc,210, 65 , Buffer, strlen(Buffer));

	// Werte für Front und Back Tileset anzeigen
	_itoa(pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].TileSetBack, Buffer, 10);
	TextOut(hdc, 920, 25, "Back",  strlen("Back"));
	TextOut(hdc, 965, 25, Buffer, strlen(Buffer));

	_itoa(pTileEngine->Tiles[ActRect.left+XOffset][ActRect.top+YOffset].TileSetFront, Buffer, 10);
	TextOut(hdc, 920, 40, "Front", strlen("Front"));
	TextOut(hdc, 965, 40, Buffer, strlen(Buffer));

	//
	// Objekt-Werte anzeigen
	//

	TextOut(hdc, 840, 615, "Anzahl :", strlen("Anzahl :"));	// Objekte Anzahl
	int num = NumObjects;

	if (pSelectedObject != NULL)
		num = pObjectList->GetNumType(pSelectedObject->ObjectID);

	_itoa(num, Buffer, 10);
	TextOut(hdc, 900, 615, Buffer, strlen(Buffer));

	TextOut(hdc, 680, 651, "X :", strlen("X :"));
	TextOut(hdc, 680, 665, "Y :", strlen("Y :"));

	TextOut(hdc, 680, 685, "Value1 :", strlen("Value1 :"));
	TextOut(hdc, 680, 699, "Value2 :", strlen("Value2 :"));

	if(pSelectedObject != NULL)
	{
		_itoa((int)(pSelectedObject->XPos), Buffer, 10);	// X-Pos aktuelles Objekt
		TextOut(hdc, 705, 651, Buffer, strlen(Buffer));

		_itoa((int)(pSelectedObject->YPos), Buffer, 10);	// Y-Pos aktuelles Objekt
		TextOut(hdc, 705, 665, Buffer, strlen(Buffer));
	}

	TextOut(hdc, 675, 210, "Music", strlen("Music"));
	TextOut(hdc, 660, 290, "Powerblock", strlen("Powerblock"));
}

// -------------------------------------------------------------------------------------- 
// Clipboard speichern
// -------------------------------------------------------------------------------------- 

bool SaveClipboard(void)
{	
	unsigned char			NumUsedSets = 0;					// Anzahl benutzter Tilesets	
	FILE					*Datei;								// Level-Datei	
	
	// Datei zum speichern (überschreiben !) öffnen
	Datei = fopen("clipboard", "wb");

	// Größe speichern
	fwrite(&ClipX, sizeof(ClipX), 1, Datei);
	fwrite(&ClipY, sizeof(ClipY), 1, Datei);

	// LevelDaten sichern		 
	 for(int i=0; i<ClipX; i++)
	  for(int j=0; j<ClipY; j++)
		 fwrite(&ClipBoard[i][j], sizeof(ClipBoard[i][j]), 1, Datei);

	// Datei schliessen
	fclose(Datei);	

	return true;
}

// -------------------------------------------------------------------------------------- 
// Clipboard laden
// -------------------------------------------------------------------------------------- 

bool LoadClipboard(void)
{
	FILE					*Datei = NULL;					// Level-Datei

	// Prüfen, ob die Datei existiert
	Datei = fopen("clipboard", "rb");

	if(!Datei)
	{
		MessageBox(NULL, "Clipboard nicht gefunden !\n", "Fehler !", MB_OK);
		return false;
	}

	// Größe laden
	fread(&ClipX, sizeof(ClipX), 1, Datei);
	fread(&ClipY, sizeof(ClipY), 1, Datei);

	// LevelDaten laden
	 for(int i=0; i<ClipX; i++)
	  for(int j=0; j<ClipY; j++)
		 fread(&ClipBoard[i][j], sizeof(ClipBoard[i][j]), 1, Datei);

	// Datei schliessen
	fclose(Datei);

	return true;
}


// -------------------------------------------------------------------------------------- 
// Level speichern
// -------------------------------------------------------------------------------------- 

bool SaveLevel(bool SaveAs)
{
	char					StringBuffer[16];
	unsigned char			NumUsedSets = 0;					// Anzahl benutzter Tilesets
	FileHeader				DateiHeader;						// Header der Level-Datei
	FILE					*Datei;								// Level-Datei
	ObjectForFileAccess		SaveObject;

	// Speichern unter oder noch kein Filename angegeben ?
	// Dann File Dialog öffnen
	//
	if (SaveAs == true ||
		strcmp (LevelName, "") == 0)
	{

		strcpy (LevelName, "");

		// File Dialog
		//
		static OPENFILENAME fn;
		static char Filter[] = "Hurrican Level Files (*.map)\0*.MAP;\0";
		fn.lStructSize       = sizeof(OPENFILENAME);
		fn.hwndOwner         = GetActiveWindow();
		fn.lpstrFilter       = Filter;
		fn.lpstrCustomFilter = NULL;
		fn.nFilterIndex      = 0;
		fn.lpstrFile         = LevelName;
		fn.nMaxFile          = sizeof(LevelName);
		fn.lpstrFileTitle    = NULL;
		fn.lpstrInitialDir   = "maps\\";
		fn.lpstrTitle        = NULL;
		fn.Flags             = OFN_OVERWRITEPROMPT;
		fn.lpstrDefExt       = NULL;
		
		// get output filename
		//
		GetSaveFileName(&fn);
	}

	else
		if (MessageBox (NULL, "Die bestehende Datei wird überschrieben!\nFortfahren ?", "Achtung !", MB_YESNO) != IDYES)
			return false;

	// Kein File angegeben ?
	//
	if (strcmp (LevelName, "") == 0)
		return false;

	// .map noch anhängen ?
	//
	if (strstr (LevelName, ".map") == NULL)
		strcat (LevelName, ".map");

	// Rausfinden, wieviele Tilesets benutzt werden
	for(int i=0;i < MAX_TILESETS; i++)
		if(TilesetNummer[i] > 0) 
			NumUsedSets++;

	// Datei zum speichern (überschreiben !) öffnen
	Datei = fopen(LevelName, "wb");

	// DateiHeader füllen
	strcpy(DateiHeader.Kennung, "Hurrican Level File V1.0 (c) 2002 Poke53280 - ");
	strcpy(DateiHeader.Beschreibung, Beschreibung);
	DateiHeader.SizeX			 = LevelSizeX;
	DateiHeader.SizeY			 = LevelSizeY;
	DateiHeader.UsedTilesets	 = NumUsedSets;
	DateiHeader.NumObjects		 = NumObjects;
	DateiHeader.ScrollBackground = bScrollBackground;	
	DateiHeader.Timelimit		 = Timelimit;

	strcpy(DateiHeader.BackgroundFile, BackgroundNames.BackgroundFile);
	strcpy(DateiHeader.ParallaxAFile,  BackgroundNames.ParallaxAFile);
	strcpy(DateiHeader.ParallaxBFile,  BackgroundNames.ParallaxBFile);
	strcpy(DateiHeader.CloudFile,	   BackgroundNames.CloudFile);

	for(i=0; i<NumUsedSets; i++)
	{
		SendMessage(ComboBoxSelectTileset, CB_GETLBTEXT, TilesetNummer[i], (LPARAM) StringBuffer);
		strcpy(DateiHeader.SetNames[i], StringBuffer);
	}

	// Header speichern
	fwrite(&DateiHeader, sizeof(DateiHeader), 1, Datei);

	// LevelDaten sichern		 
	 for(i=0; i<LevelSizeX; i++)
	  for(int j=0; j<LevelSizeY; j++)
		 fwrite(&pTileEngine->Tiles[i][j], sizeof(pTileEngine->Tiles[i][j]), 1, Datei);

	// Objekt Daten sichern
    Object		*pTemp = pObjectList->pStart;	// Zeiger auf das erste Objekt

    while (pTemp != NULL)						// Ende der Liste erreicht ?
    {
		// Werte zum Speichern "umlagern"
		SaveObject.ChangeLight	= pTemp->ChangeLight;
		SaveObject.ObjectID		= pTemp->ObjectID;
		SaveObject.XPos			= pTemp->XPos;
		SaveObject.YPos			= pTemp->YPos;
		SaveObject.Skill		= pTemp->Skill;
		SaveObject.Value1		= pTemp->Value1;
		SaveObject.Value2		= pTemp->Value2;

		// Werte speichern
		fwrite(&SaveObject, sizeof(SaveObject), 1, Datei);
		
		// Zeiger auf das nächste Objekt richten    
        pTemp = pTemp->pNext;				
    }

	// Appendix speichern
	Appendix.Taschenlampe = bTaschenlampe;
	fwrite(&Appendix, sizeof(Appendix), 1, Datei);

	// Datei schliessen
	fclose(Datei);

	return true;
}

// -------------------------------------------------------------------------------------- 
// Level laden
// -------------------------------------------------------------------------------------- 

bool LoadLevel(void)
{
	unsigned char			NumUsedSets   = 0;				// Anzahl benutzter Tilesets
	FileHeader				DateiHeader;					// Header der Level-Datei
	FILE					*Datei = NULL;					// Level-Datei
	ObjectForFileAccess		LoadObject;	

	pSelectedObject = NULL;

	strcpy (LevelName, "");

	// File Dialog 
	//
	static OPENFILENAME fn;
	static char Filter[] = "Hurrican Map Files (*.map)\0*.MAP\0";
	fn.lStructSize       = sizeof(OPENFILENAME);
	fn.hwndOwner         = GetActiveWindow();
	fn.lpstrFilter       = Filter;
	fn.lpstrCustomFilter = NULL;
	fn.nFilterIndex      = 0;
	fn.lpstrFile         = LevelName;
	fn.nMaxFile          = sizeof(LevelName);
	fn.lpstrFileTitle    = NULL;
	fn.lpstrInitialDir   = "maps\\";
	fn.lpstrTitle        = NULL;
	fn.Flags             = OFN_READONLY | OFN_FILEMUSTEXIST;
	fn.lpstrDefExt       = NULL;
	
	// File dialog öffnen
	//
	GetOpenFileName(&fn);

	// Kein File angegeben ?
	//
	if (strcmp (LevelName, "") == 0)
		return false;

	// Zuerst die Objekt-Liste löschen, damit neue Objekte geladen werden können
	pObjectList->ClearAll();			

	// geladene Tilesets löschen
	for (int i = 0; i < MAX_TILESETS; i++)
		TilesetNummer[i] = 0;

	ActualTileset = 0;
	SendMessage(ComboBoxSelectTileset, CB_SETCURSEL, 0, 0);
	RedrawTileset = true;

	// Prüfen, ob die Datei existiert
	Datei = fopen(LevelName, "rb");

	if(!Datei)
	{
		MessageBox(NULL, "Datei nicht gefunden !\n", "Fehler !", MB_OK);
		return false;
	}

	DateiHeader.NumObjects = 0;

	// DateiHeader auslesen
	fread(&DateiHeader, sizeof(DateiHeader), 1, Datei);

	// und Werte übertragen
	LevelSizeX    = DateiHeader.SizeX;
	LevelSizeY    = DateiHeader.SizeY;
	NumUsedSets   = DateiHeader.UsedTilesets;
	Timelimit	  = DateiHeader.Timelimit;
	strcpy(Beschreibung, DateiHeader.Beschreibung);
	bScrollBackground = DateiHeader.ScrollBackground;

	char Buffer[100];
	_itoa(Timelimit, Buffer, 10);
	SendMessage(TimelimitFeld, WM_SETTEXT, 0, (long)Buffer);
	SendMessage(BeschreibungsFeld, WM_SETTEXT, 0, (long)Beschreibung);
	
	if (bScrollBackground == true)
	{
		SendMessage(BoxScrollBackground, BM_SETCHECK, BST_CHECKED, 0);
		pTileEngine->bScrollBackground = true;
	}
	else
	{
		SendMessage(BoxScrollBackground, BM_SETCHECK, BST_UNCHECKED, 0);
		pTileEngine->bScrollBackground = false;
	}	

	strcpy(BackgroundNames.BackgroundFile, DateiHeader.BackgroundFile);
	strcpy(BackgroundNames.ParallaxAFile,  DateiHeader.ParallaxAFile);
	strcpy(BackgroundNames.ParallaxBFile,  DateiHeader.ParallaxBFile);
	strcpy(BackgroundNames.CloudFile,	   DateiHeader.CloudFile);

	// Daten des Hintergrundes laden
	pTileEngine->Background.LoadFromGfx			(BackgroundNames.BackgroundFile, 640, 480, 640, 480, 1, 1);
	pTileEngine->ParallaxLayer[0].LoadFromGfx	(BackgroundNames.ParallaxAFile,  640, 320, 640, 320, 1, 1);
	pTileEngine->ParallaxLayer[1].LoadFromGfx	(BackgroundNames.ParallaxBFile,  640, 160, 640, 160, 1, 1);
	pTileEngine->CloudLayer.LoadFromGfx			(BackgroundNames.CloudFile,	 	 640, 240, 640, 240, 1, 1);

	// Combo Boxen entsprechend den Namen der Backgrounds setzen
	int nr = SendMessage(ComboBoxSelectBackground, CB_FINDSTRING, -1, (LPARAM) (LPCSTR) BackgroundNames.BackgroundFile);
	if (nr == CB_ERR)
		nr = 0;
	SendMessage(ComboBoxSelectBackground, CB_SETCURSEL, nr, 0);

	nr = SendMessage(ComboBoxSelectParallaxA, CB_FINDSTRING, -1, (LPARAM) (LPCSTR) BackgroundNames.ParallaxAFile);
	if (nr == CB_ERR)
		nr = 0;
	SendMessage(ComboBoxSelectParallaxA, CB_SETCURSEL, nr, 0);

	nr = SendMessage(ComboBoxSelectParallaxB, CB_FINDSTRING, -1, (LPARAM) (LPCSTR) BackgroundNames.ParallaxBFile);
	if (nr == CB_ERR)
		nr = 0;
	SendMessage(ComboBoxSelectParallaxB, CB_SETCURSEL, nr, 0);

	nr = SendMessage(ComboBoxSelectClouds, CB_FINDSTRING, -1, (LPARAM) (LPCSTR) BackgroundNames.CloudFile);
	if (nr == CB_ERR)
		nr = 0;
	SendMessage(ComboBoxSelectClouds, CB_SETCURSEL, nr, 0);

	_itoa(LevelSizeX, Buffer, 10);	SendMessage(EditSizeX, WM_SETTEXT, 0, (long)Buffer);
	_itoa(LevelSizeY, Buffer, 10);	SendMessage(EditSizeY, WM_SETTEXT, 0, (long)Buffer);

	// Tilesets richtig im Editor anordnen wie im Level
	for(i=0; i< NumUsedSets; i++)
	{
		char StringBuffer[16];

		Protokoll.WriteText(DateiHeader.SetNames[i], false);
		Protokoll.WriteText("\n", false);
		strcpy(StringBuffer, DateiHeader.SetNames[i]);
		TilesetNummer[i] = (unsigned char)(SendMessage(ComboBoxSelectTileset, CB_FINDSTRING, 0, 
						   (LPARAM)(LPCTSTR)StringBuffer));

		// Gerade geladenes Set in der Box auswählen, falls diese grade angezeigt wird 
		// (damit es nicht zurückspringt)
		if(i == ActualTileset)
			SendMessage(ComboBoxSelectTileset, CB_SETCURSEL, TilesetNummer[i], 0);
	}

	// LevelDaten laden
	pTileEngine->InitNewLevel(LevelSizeX, LevelSizeY);

	for(i=0; i<LevelSizeX; i++)
	 for(int j=0; j<LevelSizeY; j++) 
	 {
		 fread(&pTileEngine->Tiles[i][j], sizeof(pTileEngine->Tiles[i][j]), 1, Datei);
	 }

	// Objekt Daten laden und gleich Liste mit Objekten erstellen
	if (DateiHeader.NumObjects > 0)
	for(unsigned int k=0; k < DateiHeader.NumObjects; k++)
	{
		fread(&LoadObject, sizeof(LoadObject), 1, Datei);				// Objekt laden
		pObjectList->Push(LoadObject);								// und in die Liste setzen
		// Bei Spieler Position gleich hinscrollen
		if (LoadObject.ObjectID == 0)
		{
			XOffset = int ((LoadObject.XPos - 300) / 20.0f);
			YOffset = int ((LoadObject.YPos - 200) / 20.0f);

			if (XOffset < 0) XOffset = 0;
			if (YOffset < 0) YOffset = 0;
		}
	}

	// Appendix laden
	if (!feof(Datei))
	{
		fread(&Appendix, sizeof(Appendix), 1, Datei);

		// Combo Boxen entsprechend der Namen der Songs setzen
		nr = SendMessage(ComboBoxSelectStageMusic, CB_FINDSTRING, -1, (LPARAM) (LPCSTR) Appendix.Songs[0]);
		if (nr == CB_ERR)
			nr = 0;
		SendMessage(ComboBoxSelectStageMusic, CB_SETCURSEL, nr, 0);

		nr = SendMessage(ComboBoxSelectBossMusic, CB_FINDSTRING, -1, (LPARAM) (LPCSTR) Appendix.Songs[1]);
		if (nr == CB_ERR)
			nr = 0;
		SendMessage(ComboBoxSelectBossMusic, CB_SETCURSEL, nr, 0);

		// Combo Boxen entsprechend des gewählten Powerblock Typs setzen
		SendMessage(ComboBoxSelectPowerblock, CB_SETCURSEL, Appendix.UsedPowerblock, 0);

		// Liquid Color
		char buf[9];

		strcpy(buf, Appendix.Col1);
		buf[8] = '\0';
		SendMessage(Color1Feld, WM_SETTEXT, 0, (long)buf);

		strcpy(buf, Appendix.Col2);
		buf[8] = '\0';
		SendMessage(Color2Feld, WM_SETTEXT, 0, (long)buf);

		bTaschenlampe = Appendix.Taschenlampe;
		if (bTaschenlampe == true)
			SendMessage(BoxTaschenlampe, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(BoxTaschenlampe, BM_SETCHECK, BST_UNCHECKED, 0);
	}

	// Datei schliessen
	fclose(Datei);

	if(pSelectedObject != NULL)
		SendMessage(ComboBoxObjectType, CB_SETCURSEL, pSelectedObject->ObjectID, 0);

	SaveUndoStep(true);

	// Levelname setzen
	//
	SetWindowText (LevelNamenFeld, LevelName);

	return true;
}

// -------------------------------------------------------------------------------------- 
// Level testen
// -------------------------------------------------------------------------------------- 

void TestLevel(void)
{
	// Zuerst das Level speichern unter dem Namen "temp.map"

	char					StringBuffer[16];
	char					LevelFile[100];
	unsigned char			NumUsedSets = 0;					// Anzahl benutzter Tilesets
	FileHeader				DateiHeader;						// Header der Level-Datei
	FILE					*Datei;								// Level-Datei
	ObjectForFileAccess		SaveObject;

	strcpy (LevelFile, HurriPath);
	strcat (LevelFile, "data/");
	strcat (LevelFile, "temp.map");

	// Rausfinden, wieviele Tilesets benutzt werden
	for(int i=0;i < MAX_TILESETS; i++)
		if(TilesetNummer[i] > 0) 
			NumUsedSets++;

	// Datei zum speichern (überschreiben !) öffnen
	Datei = fopen(LevelFile, "wb");

	// DateiHeader füllen
	strcpy(DateiHeader.Kennung, "Hurrican Level File V1.0 (c) 2002 Poke53280 - ");
	strcpy(DateiHeader.Beschreibung, Beschreibung);
	DateiHeader.SizeX			 = LevelSizeX;
	DateiHeader.SizeY			 = LevelSizeY;
	DateiHeader.UsedTilesets	 = NumUsedSets;
	DateiHeader.NumObjects		 = NumObjects;
	DateiHeader.ScrollBackground = bScrollBackground;
	DateiHeader.Timelimit		 = Timelimit;

	strcpy(DateiHeader.BackgroundFile, BackgroundNames.BackgroundFile);
	strcpy(DateiHeader.ParallaxAFile,  BackgroundNames.ParallaxAFile);
	strcpy(DateiHeader.ParallaxBFile,  BackgroundNames.ParallaxBFile);
	strcpy(DateiHeader.CloudFile,	   BackgroundNames.CloudFile);

	for(i=0; i<NumUsedSets; i++)
	{
		SendMessage(ComboBoxSelectTileset, CB_GETLBTEXT, TilesetNummer[i], (LPARAM) StringBuffer);
		strcpy(DateiHeader.SetNames[i], StringBuffer);
	}

	// Header speichern
	fwrite(&DateiHeader, sizeof(DateiHeader), 1, Datei);

	// LevelDaten sichern		 
	 for(i=0; i<LevelSizeX; i++)
	  for(int j=0; j<LevelSizeY; j++)
		 fwrite(&pTileEngine->Tiles[i][j], sizeof(pTileEngine->Tiles[i][j]), 1, Datei);

	// Objekt Daten sichern
    Object		*pTemp = pObjectList->pStart;	// Zeiger auf das erste Objekt

    while (pTemp != NULL)						// Ende der Liste erreicht ?
    {
		// Werte zum Speichern "umlagern"
		SaveObject.ChangeLight	= pTemp->ChangeLight;
		SaveObject.ObjectID		= pTemp->ObjectID;
		SaveObject.XPos			= pTemp->XPos;
		SaveObject.YPos			= pTemp->YPos;
		SaveObject.Skill		= pTemp->Skill;
		SaveObject.Value1		= pTemp->Value1;
		SaveObject.Value2		= pTemp->Value2;

		// Werte speichern
		fwrite(&SaveObject, sizeof(SaveObject), 1, Datei);
		
		// Zeiger auf das nächste Objekt richten    
        pTemp = pTemp->pNext;				
    }

	// Appendix speichern
	Appendix.Taschenlampe = bTaschenlampe;
	fwrite(&Appendix, sizeof(Appendix), 1, Datei);

	// Datei schliessen
	fclose(Datei);

	// Hurrican ausführen mit Temp Level als Parameter
	//
	char comm [255];

	_chdir (HurriPath);
	strcpy (comm, HurriPath);
	strcat (comm, "Hurrican.exe windowmode level temp.map");
	int err = WinExec(comm, SW_SHOW);
	if (err <= 31)
		MessageBox (NULL, "Fehler !", "Fehler !", MB_OK);
	
	// Und Temp Level löschen
	//DeleteFile(LevelName);
}

// -------------------------------------------------------------------------------------- 
// Auswahl durchgehen und dort, wo "verdecken" gesetzt ist, aber das Overlay Tile noch
// leer ist, das Level Tile ins Overlaytile kopieren
// -------------------------------------------------------------------------------------- 

void CopyFromBlock(void)
{
	if (ClipX    <= 0 ||
		ClipY    <= 0 ||
		ClipOffX <= 0 ||
		ClipOffY <= 0)
		return;

	int xoff;
	int yoff;

	for (int i=0; i<ClipX; i++)
	 for (int j=0; j<ClipY; j++)
		{				
			// Offset im Level
			xoff = ClipOffX/20 + i;
			yoff = ClipOffY/20 + j;

			if((pTileEngine->Tiles[xoff][yoff].Block & BLOCKWERT_VERDECKEN) &&
			   (pTileEngine->Tiles[xoff][yoff].FrontArt == 0))
			{
				pTileEngine->Tiles[xoff][yoff].FrontArt		= pTileEngine->Tiles[xoff][yoff].BackArt;
				pTileEngine->Tiles[xoff][yoff].BackArt		= 0;
				pTileEngine->Tiles[xoff][yoff].TileSetFront = pTileEngine->Tiles[xoff][yoff].TileSetBack;
			}
	 }
	 
	 RedrawLevel = true;	
}

// -------------------------------------------------------------------------------------- 
// GameInit, initialisiert die DX Objekte und das Spiel
// -------------------------------------------------------------------------------------- 

bool GameInit(HINSTANCE hinstance)
{

	Protokoll.WriteText("\n>--------------------<\n", false);
	Protokoll.WriteText(  "| GameInit gestartet |\n", false);
	Protokoll.WriteText(  ">--------------------<\n", false);

	// DirectInput initialisieren
	if(!DirectInput.Init(MainHWND, hinstance))
	{
		Protokoll.WriteText("\n-> DirectInput8 Initialisierung Fehler ...!\n", true);
		return false;
	}

/*	// DirectSound initialisieren
	if(!DirectSound.Init(hwnd))
	{
		Protokoll.WriteText("\n-> DirectSound8 Initialisierung Fehler ...!\n", true);
		return false;
	}*/

	// Direct3D initialisieren
	if(!DirectGraphics.Init(MainHWND, SCREENWIDTH, SCREENHEIGHT, SCREENBPP, 0, false))
	{
		Protokoll.WriteText("\n-> Direct3D Initialisierung Fehler ...!\n", true);
		return false;
	}

	// Tileengine initialisieren
	pTileEngine = new(TileEngineClass);
	pTileEngine->SetScrollSpeed(0.0f, 0.0f);

	pTileEngine->Background.LoadFromGfx("static_jungle.png", 640, 480, 640, 480, 1, 1);
	pTileEngine->CloudLayer.LoadFromGfx("clouds_jungle.bmp",     640, 240, 640, 240, 1, 1);
	pTileEngine->ParallaxLayer[0].LoadFromGfx("back_jungle.png", 640, 480, 640, 480, 1, 1);
	pTileEngine->ParallaxLayer[1].LoadFromGfx("front_jungle.png", 640, 480, 640, 480, 1, 1);

	// Undo array initialisieren
	ZeroMemory(&UndoBuffer, sizeof(UndoBuffer));

	// Objekte Grafiken laden
	ObjectGFX[200].LoadFromGfx("extras.png",  312, 24, 24, 24, 13, 1);
	ObjectGFX[0].LoadImage ("gfx/spielerstart.bmp",   70,  80, 70, 80, 1,1);
	ObjectGFX[1].LoadFromGfx ("oneup.png",		 200,  160, 40, 40, 5,4);
	ObjectGFX[2].LoadFromGfx ("diamant.bmp",		 261,  29, 29, 29, 9,1);
	ObjectGFX[3].LoadFromGfx ("powerblock.png", 440, 40, 40, 40, 11, 1);

	// Gegner
	ObjectGFX[4].LoadFromGfx ("spitter.bmp",		110,  23, 22, 23, 5,1);
	ObjectGFX[5].LoadFromGfx ("enemy-walker.bmp",	 360, 180, 45, 45, 8,4);
	ObjectGFX[6].LoadFromGfx ("enemy-guntower.bmp",300, 120, 60, 40, 5,3);
	ObjectGFX[7].LoadFromGfx ("spiderbomb.bmp",    350,  34, 50, 34, 7,1);
	ObjectGFX[8].LoadFromGfx ("piranha.bmp",		 225, 232, 45, 29, 5,8);
	ObjectGFX[9].LoadFromGfx ("stahlmuecke.bmp",	 170,  70, 34, 35, 5,2);
	ObjectGFX[10].LoadFromGfx("deckenturm.bmp",	 190, 240, 38, 40, 5,6);
	ObjectGFX[11].LoadFromGfx("kugel4.bmp",		 150, 120, 30, 30, 5,4);
	ObjectGFX[12].LoadFromGfx("kugel3.bmp",		 225, 180, 45, 45, 5,4);
	ObjectGFX[13].LoadFromGfx("kugel2.bmp",		 300, 240, 60, 60, 5,4);
	ObjectGFX[14].LoadFromGfx("kugel1.bmp",		 450, 360, 90, 90, 5,4);
	ObjectGFX[15].LoadFromGfx("krabbleroben.bmp",	 250, 225, 50, 75, 5,3);
	ObjectGFX[16].LoadFromGfx("krabblerlinks.bmp", 225, 250, 75, 50, 3,5);
	ObjectGFX[17].LoadFromGfx("krabblerrechts.bmp",225, 250, 75, 50, 3,5);
	ObjectGFX[18].LoadFromGfx("auge.bmp",			 360, 26, 60, 26, 6, 1);
	ObjectGFX[19].LoadFromGfx("qualle.bmp",		 240, 180, 40, 60, 6,3);
	ObjectGFX[20].LoadFromGfx("fallingstone.bmp",	 80,   40, 80, 40, 1,1);
	ObjectGFX[21].LoadFromGfx("fallingstone2.bmp", 160,   40, 80, 40, 2,1);
	ObjectGFX[22].LoadFromGfx("roboraupe.bmp",	 360, 105, 90, 35, 4,3);
	ObjectGFX[23].LoadFromGfx("stalagtit.bmp",	 40,   80, 40, 80, 1,1);
	ObjectGFX[24].LoadFromGfx("raketenwerfer.bmp", 225,  50, 45, 25, 5,2);
	ObjectGFX[25].LoadFromGfx("swimwalker.bmp",    260, 135, 65, 45, 4,3);
	ObjectGFX[26].LoadFromGfx("flugsack.bmp",      320, 210, 80, 70, 4,3);
	ObjectGFX[27].LoadFromGfx("flugsack.bmp",		 320, 210, 80, 70, 4,3);
	ObjectGFX[28].LoadFromGfx("eisstachel.bmp",	 420, 390,105, 78, 4,5);
	ObjectGFX[29].LoadFromGfx("skiwalker.bmp",	 195, 152, 65, 38, 3,4);
	ObjectGFX[30].LoadFromGfx("flugkanone.bmp",	 260, 225, 65, 45, 4,5);
	ObjectGFX[31].LoadFromGfx("snowbomb.bmp",	     480, 320, 80, 80, 6,4);
	ObjectGFX[32].LoadFromGfx("roboman1.bmp",	     450, 300, 90,100, 5,3);
	ObjectGFX[33].LoadFromGfx("boulder.bmp",	     300, 165, 60, 55, 5,3);
	ObjectGFX[34].LoadFromGfx("stelzsack.bmp",	 500, 441,100, 147, 5,4);
	ObjectGFX[35].LoadFromGfx("fallbombe.bmp",	 138,  59, 23, 59, 6,1);
	ObjectGFX[36].LoadFromGfx("pokenuke.bmp",	 	  81, 217, 81,217, 1,1);
	ObjectGFX[37].LoadFromGfx("lavakrabbe.bmp",	 280, 136, 70, 34, 4,4);
	ObjectGFX[38].LoadFromGfx("lavaball.bmp",		 300, 240, 60, 60, 5,4);
	ObjectGFX[39].LoadFromGfx("lavaball.bmp",		 300, 240, 60, 60, 5,4);
	ObjectGFX[40].LoadFromGfx("fettespinne.png", 416, 414, 104, 69, 4,5);
	ObjectGFX[41].LoadFromGfx("eiermann.bmp",		 310, 132,62,66, 5,2);
	ObjectGFX[42].LoadFromGfx("wandcannon.bmp",	 141,  32, 47, 32, 3,1);
	ObjectGFX[43].LoadFromGfx("starsmall.bmp",	  80,  80, 40, 40, 2,2);
	ObjectGFX[44].LoadFromGfx("starbig.bmp",		 160, 160, 80, 80, 2,2);
	ObjectGFX[45].LoadFromGfx("made.bmp",			 200,  40, 20, 20, 10,2);
	ObjectGFX[46].LoadFromGfx("drone.bmp",		 280, 240, 70, 80, 4,3);
	ObjectGFX[47].LoadFromGfx("neufisch.bmp",		 360, 292, 90, 73, 4,4);
	ObjectGFX[48].LoadFromGfx("pflanze.bmp",		 440, 450,110, 90, 4,5);
	ObjectGFX[49].LoadFromGfx("spitterbombe.bmp",	 174,  50, 29, 50, 6,1);
	ObjectGFX[50].LoadFromGfx("nest.bmp",			  43,  60, 43, 60, 1,1);
	ObjectGFX[51].LoadFromGfx("schienenviech.bmp", 245,  40, 49, 40, 1,1);
	ObjectGFX[52].LoadFromGfx("wassermine.bmp",    300, 190, 60, 90, 5,2);
	ObjectGFX[53].LoadFromGfx("fledermaus.bmp",    500, 174, 50, 58, 10,3);
	ObjectGFX[54].LoadFromGfx("climbspider.bmp",   250, 105, 50, 35, 5,3);
	ObjectGFX[55].LoadFromGfx("feuerspinne.bmp",   480, 110, 80, 55, 6,2);
	ObjectGFX[56].LoadFromGfx("ballerdrone.bmp",   450, 195, 90, 65, 5,3);
	ObjectGFX[57].LoadFromGfx("schwabbel.bmp",     300, 240, 60, 60, 5,4);
	ObjectGFX[58].LoadFromGfx("bigrocket.bmp",     578, 600, 73, 300, 8,2);
	ObjectGFX[59].LoadFromGfx("eiszapfen.png", 30, 100, 30, 100, 1,1);
	ObjectGFX[60].LoadFromGfx("wandkrabbe.bmp",	   136, 192, 34, 64, 4,3);
	ObjectGFX[61].LoadFromGfx("jaeger.bmp",		   280, 212, 70, 53, 4,4);
	ObjectGFX[62].LoadFromGfx("siderocket.bmp",	    40,  18, 40, 18, 1,1);
	ObjectGFX[63].LoadFromGfx("fiesedrone.bmp",	   200, 150, 50, 50, 4,3);
	ObjectGFX[64].LoadFromGfx("schleimboller.png", 240, 240, 60, 60, 4,4);
	ObjectGFX[65].LoadFromGfx("schleimrot.png",    240, 240, 60, 60, 4,4);
	ObjectGFX[66].LoadFromGfx("schleimmaul.png",   272, 272, 68, 68, 4,4);
	ObjectGFX[67].LoadFromGfx("fieserwalker.bmp",   264, 240, 66, 60, 4,4);
	ObjectGFX[68].LoadFromGfx("fieserwalker2.bmp",   264, 240, 66, 60, 4,4);
	ObjectGFX[69].LoadFromGfx("mittelspinne.png",    321, 344, 107, 86, 4,3);
	ObjectGFX[70].LoadFromGfx("smallwespe.bmp",    200, 40, 50, 40, 4,1);
	ObjectGFX[71].LoadFromGfx("lavakrabbe.bmp",	 280, 136, 70, 34, 4,4);
	ObjectGFX[72].LoadFromGfx("stampfstein.png",	 80, 256, 80, 256, 1,1);
	ObjectGFX[73].LoadFromGfx("lavamann.bmp",	 280, 150, 40, 50, 7,3);
	ObjectGFX[74].LoadFromGfx("lafass.png",	 444, 495, 74, 165, 7,3);
	ObjectGFX[75].LoadFromGfx("geschuetz.bmp",	 300, 300, 60, 60, 5,5);
	ObjectGFX[76].LoadFromGfx("fetterakete.png", 250, 34, 25, 34, 10,1);
	ObjectGFX[77].LoadFromGfx("schabe.bmp",	 80, 60, 20, 20, 4,3);
	ObjectGFX[78].LoadFromGfx("minidragon.png",	 300, 141, 60, 47, 5,3);
	ObjectGFX[79].LoadFromGfx("Schneekoppe.bmp", 150, 120, 30, 30, 5,4);
	ObjectGFX[80].LoadFromGfx("zitrone.png",	 460, 243, 115, 81, 4,3);
	ObjectGFX[81].LoadFromGfx("rotzpott.png",	 55, 41, 55, 41, 1, 1);
	ObjectGFX[82].LoadFromGfx("stachelbeere.png", 360, 360, 60, 60, 6, 6);
	ObjectGFX[83].LoadFromGfx("minirocket.bmp", 12, 12, 12, 12, 1,1);
	ObjectGFX[84].LoadFromGfx("mutant.bmp", 500, 450, 100, 90, 5, 5);

	for (int i=85; i<99; i++)
		ObjectGFX[i].LoadImage("gfx/nix.bmp",		 32,  32,  32,  32, 6, 3);

	// Viertelgegner (100 - 120)
	ObjectGFX[100].LoadFromGfx("riesenpiranha.bmp",1024, 420, 256, 210, 4,2);
	ObjectGFX[101].LoadFromGfx("riesenqualle.bmp",  390, 480, 130, 240, 3,2);
	ObjectGFX[102].LoadFromGfx("riesenraupe.bmp",   540, 290, 180,  58, 3,5);
	ObjectGFX[103].LoadFromGfx("wasp.png",		  232, 300, 116, 100, 2,3);

	for (i=104; i<119; i++)
		ObjectGFX[i].LoadImage("gfx/nix.bmp",		 32,  32,  32,  32, 1,1);

	// Zwischen und Endgegner (120 - 140)
	ObjectGFX[120].LoadFromGfx("stahlfaust.bmp",			512, 264, 256, 264, 2,1);
	ObjectGFX[121].LoadFromGfx("pharaokopf.bmp",			448, 448, 224, 224, 2,2);
	ObjectGFX[122].LoadFromGfx("spiderboss_rumpf.png",		415, 160, 415, 160, 1,1);
	ObjectGFX[123].LoadFromGfx("ufo.bmp",					600, 658,200, 94, 3, 7);
	ObjectGFX[124].LoadFromGfx("ufo.bmp",					600, 658,200, 94, 3, 7);
	ObjectGFX[125].LoadFromGfx("fahrstuhlboss_rahmen.bmp",420, 280,420,280, 1, 1);
	ObjectGFX[126].LoadFromGfx("flugboss_main.png",		280, 285, 280, 285, 1,1);
	ObjectGFX[127].LoadFromGfx("bratklops0000.bmp",		232, 400,232,400, 1, 1);
	ObjectGFX[128].LoadImage("gfx/trschmidt.bmp",			 40,  40, 40, 40, 1, 1);
	ObjectGFX[129].LoadFromGfx("metalhead_birne.bmp",		204, 242, 204, 242, 1, 1);
	ObjectGFX[130].LoadFromGfx("stahlfaust_eis.bmp",		512, 264, 256, 264, 2,1);
	ObjectGFX[131].LoadImage("gfx/wuxe.bmp",			 40,  40, 40, 40, 1, 1);
	ObjectGFX[132].LoadFromGfx("golem_rumpf.png",			 185,  291, 185, 291, 1, 1);
	ObjectGFX[133].LoadFromGfx("spinnenmaschine_oben.png",	317,  413, 317, 413, 1, 1);
	ObjectGFX[134].LoadFromGfx("drache_body.png", 201, 147, 201, 147, 1, 1);
	ObjectGFX[135].LoadFromGfx("rollmops_rollen.png", 650, 260, 130, 130, 5, 2);
	ObjectGFX[136].LoadFromGfx("schneekoenig.png", 800, 600, 200, 200, 4, 3);
	ObjectGFX[137].LoadFromGfx("bigfish.bmp", 208, 157, 208, 157, 1, 1);
	ObjectGFX[138].LoadFromGfx("skeletor.png", 1000, 400, 125, 200, 8, 2);
	ObjectGFX[139].LoadFromGfx("endbossmaschine.png",	285,  480, 285, 480, 1, 1);

	// Trigger
	ObjectGFX[140].LoadImage("gfx/partikelspawner.bmp",40, 40,  40,  40, 1,1);
	ObjectGFX[141].LoadFromGfx("fahrstuhl.bmp",	  400, 154, 400, 154, 1,1);
	ObjectGFX[142].LoadFromGfx("fass.png",		   48,  56,  48,  56, 1,1);
	ObjectGFX[143].LoadFromGfx("flammenwerfer.bmp",120, 120, 60,  60, 2,2);
	ObjectGFX[144].LoadImage("gfx/fademusic.bmp",	   40,  40,  40,  40, 1,1);
	ObjectGFX[145].LoadImage("gfx/endlevel.bmp",	  120, 120, 120, 120, 1,1);
	ObjectGFX[146].LoadFromGfx("plattform.bmp",	  128,  64, 128,  32, 1,2);
	ObjectGFX[147].LoadFromGfx("presse.bmp",		  100, 256, 100, 256, 1,1);
	ObjectGFX[148].LoadFromGfx("trenzshrine.bmp",	  120, 180, 120, 180, 1,1);
	ObjectGFX[149].LoadFromGfx("bruecke.bmp",		  10,   10,  10,  10, 1,1);
	ObjectGFX[150].LoadFromGfx("floating.bmp",	  39,   14,  39,  14, 1,1);
	ObjectGFX[151].LoadFromGfx("floating2.bmp",	 110,   18, 110,  18, 1,1);
	ObjectGFX[152].LoadFromGfx("floating3.bmp",	 100,   54, 100,  27, 1,2);
	ObjectGFX[153].LoadImage("gfx/surfbrett.bmp",	  52,   35,  52,  35, 1,1);
	ObjectGFX[154].LoadFromGfx("shootplattformbutton.bmp",32,  9, 16,  9, 2,1);
	ObjectGFX[155].LoadFromGfx("shootplattform.bmp",100,  54, 100,  54, 1,1);
	ObjectGFX[156].LoadFromGfx("glubschi.bmp",	  510, 360,  51, 120, 10,3);
	ObjectGFX[157].LoadFromGfx("glubschi2.bmp",	  510, 360,  51, 120, 10,3);
	ObjectGFX[158].LoadFromGfx("column.bmp",		  40,  100,  40, 100, 1,1);
	ObjectGFX[159].LoadFromGfx("lift.bmp",		  80,  230,  80, 23, 1,10);	
	ObjectGFX[160].LoadFromGfx("column.bmp",		  40,  100,  40, 100, 1,1);
	ObjectGFX[161].LoadFromGfx("Schleuse.bmp",		 200,   40, 200, 40, 1,1);
	ObjectGFX[162].LoadFromGfx("Schleuse2.bmp",		  40,  200,  40,200, 1,1);
	ObjectGFX[163].LoadFromGfx("switch.bmp",		  48,   32,  24, 32, 2,1);
	ObjectGFX[164].LoadFromGfx("warning.png",		 180,   40, 180, 40, 1,1);
	ObjectGFX[165].LoadImage("gfx/soundtrigger.bmp",  40,   40,  40,  40, 1,1);
	ObjectGFX[166].LoadFromGfx("luefter_gross.png",	 224,  223, 224,223, 1,1);
	ObjectGFX[167].LoadFromGfx("luefter1.png",	 453,  343, 113,114, 4,3);
	ObjectGFX[168].LoadFromGfx("luefter2.png",	 453,  343, 113,114, 4,3);
	ObjectGFX[169].LoadImage("gfx/tutorialtext.bmp",40, 40,  40,  40, 1,1);
	ObjectGFX[170].LoadFromGfx("lavaflare.bmp",120, 120,  120,  120, 1,1);
	ObjectGFX[171].LoadImage("gfx/secret.bmp",40, 40, 40, 40, 1,1);
	ObjectGFX[172].LoadFromGfx("mushroom_head.png",135, 61, 135, 61, 1,1);
	ObjectGFX[173].LoadFromGfx("presswurst.png",185, 357, 185, 357, 1,1);
	ObjectGFX[174].LoadFromGfx("lafass_spawner.png",74, 165, 74, 165, 1,1);
	ObjectGFX[175].LoadFromGfx("spikelift.bmp",460, 80, 460, 80, 1,1);
	ObjectGFX[176].LoadFromGfx("tube.png",400, 220, 100, 220, 4,1);
	
	SetClippingArea(0, 0, 640, 480);
	pSelectedObject = NULL;

	DirectInput.MausX = 200;
	DirectInput.MausY = 200;

//	DirectSound.SetVolume(100, 100);
//	DirectSound.SetAllSongVolumes();

	LiquidGfx[0].LoadFromGfx("water.bmp", 128, 128, 20, 20, 6, 6);
	LiquidGfx[1].LoadFromGfx("water2.bmp", 128, 128, 20, 20, 6, 6);

	// Blockarten laden

	BlockArten.LoadImage("gfx/blockarten.png", 20, 420, 20, 20, 1, 20);

	BlockBrush[0] = CreateSolidBrush(RGB(255, 255, 255));
	BlockBrush[1] = CreateHatchBrush(HS_DIAGCROSS, RGB(255,   0, 0));
	BlockBrush[2] = CreateHatchBrush(HS_DIAGCROSS, RGB(  0, 255, 0));
	BlockBrush[3] = CreateHatchBrush(HS_DIAGCROSS, RGB(255, 255, 255));
	BlockBrush[4] = CreateHatchBrush(HS_DIAGCROSS, RGB(  0, 255, 0));
	BlockBrush[5] = CreateHatchBrush(HS_BDIAGONAL, RGB(255, 255, 255));
	BlockBrush[6] = CreateHatchBrush(HS_BDIAGONAL, RGB(  0, 255, 0));
	BlockBrush[7] = CreateHatchBrush(HS_BDIAGONAL, RGB(128, 128, 128));

	Pens[0]		  = CreatePen(PS_DOT,  0, RGB(255, 255, 255));
	Pens[1]		  = CreatePen(PS_NULL,  0, TRANSPARENT);
	Pens[2]		  = CreatePen(PS_SOLID,  0, RGB(0, 0, 0));
	Pens[3]		  = CreatePen(PS_SOLID,  0, RGB(0, 255, 0));

	pTileEngine->InitNewLevel(64, 48);
	pObjectList->ClearAll();

	strcpy(BackgroundNames.BackgroundFile,"static_aliencave.png");
	strcpy(BackgroundNames.ParallaxAFile, "back_aliencave.png");
	strcpy(BackgroundNames.ParallaxBFile, "front_aliencave.png");
	strcpy(BackgroundNames.CloudFile,     "Clouds1.bmp");

	pFont->LoadFont("gfx/smallfont.bmp", 320, 84, 10, 12, 32, 7);

	SetFocus(NULL);
	return true;
}

// -------------------------------------------------------------------------------------- 
// GameExit, de-initialisieren der DX Objekte, Sprites etc.
// -------------------------------------------------------------------------------------- 

bool GameExit(void)
{
	Protokoll.WriteText("\n>--------------------<\n", false);
	Protokoll.WriteText(  "| GameExit gestartet |\n", false);
	Protokoll.WriteText(  ">--------------------<\n\n", false);

	delete(pFont);
	pFont = NULL;

	for(int i=0; i<10; i++)
		DeleteObject(BlockBrush[i]);

	for(i=0; i<3; i++)
		DeleteObject(Pens[i]);

	// Tileengine beenden
	delete(pTileEngine);

	for(i=0; i<100; i++)
		ObjectGFX[i].~DirectGraphicsSprite();

	DirectGraphics.Exit();				// Direct3D    beenden
	DirectInput.Exit();					// DirectInput beenden
	//DirectSound.Exit();					// DirectSound beenden

	return true;
}

// -------------------------------------------------------------------------------------- 
// Editor Umgebung aus Buttons zusammenbauen
// -------------------------------------------------------------------------------------- 

void CreateEnvironment(HINSTANCE hinstance)
{
	// Rahmen des Editorfensters
	EditorRahmen = CreateWindow("STATIC",
			 					  "",
								  WS_CHILD | WS_BORDER | WS_VISIBLE | WS_THICKFRAME,
								  -8,    0,
								  1032,  768,
								  MainHWND, NULL, hinstance, NULL);

	// Rahmen der Hintergründe und Parallax Layer
	GroupBoxBackgrounds = CreateWindow("BUTTON", 
								"Background-Layer",
								 WS_CHILD | WS_VISIBLE | ES_LEFT | BS_GROUPBOX,
								 490, 3,
								 252, 85,
								 MainHWND, (HMENU)GROUPBOX_ID_Backgrounds, hinstance, NULL);

	// Rahmen des Objektes
	GroupBoxObjekt = CreateWindow("BUTTON", 
								"Object",
								 WS_CHILD | WS_VISIBLE | ES_LEFT | BS_GROUPBOX,
								 670, 600,
								 345, 120,
								 MainHWND, (HMENU)GROUPBOX_ID_Objekt, hinstance, NULL);

	// Rahmen des aktuellen Tiles (Licht einstellung usw)
	GroupBoxFarbe = CreateWindow("BUTTON",
			 					  "Color",
								  WS_CHILD | WS_VISIBLE | ES_LEFT | BS_GROUPBOX,
								  670, 340,
								  170, 121,
								  MainHWND, (HMENU)GROUPBOX_ID_Farbe, hinstance, NULL);

	// Rahmen der MiniMap
	GroupBoxMiniMap = CreateWindow("BUTTON",
			 					  "Mini-Map",
								  WS_CHILD | WS_VISIBLE | ES_LEFT | BS_GROUPBOX,
								  670, 462,
								  170, 130,
								  MainHWND, (HMENU)GROUPBOX_ID_MiniMap, hinstance, NULL);

	// Rahmen um die Block-Auswahl
	GroupBoxBlock = CreateWindow("BUTTON",
			 					  "Block-Info",
								  WS_CHILD | WS_VISIBLE | ES_LEFT | BS_GROUPBOX,
								  838, 340,
								  177, 270,
								  MainHWND, (HMENU)GROUPBOX_ID_Block, hinstance, NULL);

	// Rahmen des aktuellen Tilesets
	GroupBoxTileset = CreateWindow("BUTTON",
			 					   "Tileset",
								   WS_CHILD | WS_VISIBLE | ES_LEFT | BS_GROUPBOX,
								   740, 3,
								   275, 335,
								   MainHWND, (HMENU)GROUPBOX_ID_Tileset, hinstance, NULL);

	// Rahmen des Edit Modes
	GroupBoxEditMode = CreateWindow("BUTTON", 
								"Display-Mode (+Left Shift)",
								 WS_CHILD | WS_VISIBLE | ES_LEFT | BS_GROUPBOX,
								 5,	  90,
								 230, 116,
								 MainHWND, (HMENU)GROUPBOX_ID_EditMode, hinstance, NULL);

	// Rahmen der Level Info
	GroupBoxLevelInfo = CreateWindow("BUTTON", 
								"Level-Information",
								 WS_CHILD | WS_VISIBLE | ES_LEFT | BS_GROUPBOX,
								 5,	  3,
								 480, 85,
								 MainHWND, (HMENU)GROUPBOX_ID_LevelInfo, hinstance, NULL);

	// Rahmen der Copy Info (was wird ins Clipboard kopiert ... Back Front Light und/oder Block)
	GroupBoxCopyInfo = CreateWindow("BUTTON", 
								"Clipboard",
								 WS_CHILD | WS_VISIBLE | ES_LEFT | BS_GROUPBOX,
								 240,  90,
								 120, 116,
								 MainHWND, (HMENU)GROUPBOX_ID_CopyInfo, hinstance, NULL);

	// Rahmen der Shadow/Light-Buttons
	GroupBoxLightMode = CreateWindow("BUTTON", 
								"Light-Mode",
								 WS_CHILD | WS_VISIBLE | ES_LEFT | BS_GROUPBOX,
								 365,  90,
								 290, 116,
								 MainHWND, (HMENU)GROUPBOX_ID_LightMode, hinstance, NULL);

	// Rahmen für Misc
	GroupBoxLightMode = CreateWindow("BUTTON", 
								"Misc",
								 WS_CHILD | WS_VISIBLE | ES_LEFT | BS_GROUPBOX,
								 660,  90,
								 82, 116,
								 MainHWND, (HMENU)GROUPBOX_ID_LightMode, hinstance, NULL);

	// Button für Brücke
	ButtonBruecke = CreateWindow("BUTTON",
								  "Bridge",
								  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								  670, 110,
								  60, 22,
								  MainHWND, (HMENU)BUTTON_ID_Bruecke, hinstance, NULL);

	// Button für "Copy from Block"
	ButtonCopyFromBlock = CreateWindow("BUTTON",
								  "CfB",
								  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								  670, 142,
								  60, 22,
								  MainHWND, (HMENU)BUTTON_ID_CopyFromBlock, hinstance, NULL);

	// Button für Level testen
	ButtonTestLevel = CreateWindow("BUTTON",
								  "Test It",
								  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								  670, 174,
								  60, 22,
								  MainHWND, (HMENU)BUTTON_ID_TestLevel, hinstance, NULL);	

	// Level X-Grösse
	EditSizeX = CreateWindow("EDIT",
			 				 "64",
							  WS_CHILD | WS_BORDER | WS_VISIBLE |
							  ES_LEFT  | ES_NUMBER | ES_WANTRETURN,
							  78, 45,
							  42, 16,
							  MainHWND,(HMENU)EDIT_ID_SizeX,hinstance,NULL);


	// Level Y-Grösse
	EditSizeY = CreateWindow("EDIT",
			 				 "48",
							  WS_CHILD | WS_BORDER | WS_VISIBLE |
							  ES_LEFT  | ES_NUMBER | ES_WANTRETURN,
							  78, 65,
							  42, 16,
							  MainHWND,(HMENU)EDIT_ID_SizeY,hinstance,NULL);

	// Namenseingabefeld für das Level
	/*LevelNamenFeld = CreateWindow("EDIT",
			 					  "",
								  WS_CHILD | WS_BORDER | WS_VISIBLE |
								  ES_LEFT  | ES_WANTRETURN,
								  10,  20,
								  110, 20,
								  MainHWND,(HMENU)EDIT_ID_LevelName,hinstance,NULL);*/

	// Timelimit Feld
	TimelimitFeld = CreateWindow("EDIT",
			 					  "Timelimit",
								  WS_CHILD | WS_BORDER | WS_VISIBLE |
								  ES_LEFT  | ES_WANTRETURN,
  								  265,  64,
								  180,  20,
								  MainHWND,(HMENU)EDIT_ID_Timelimit,hinstance,NULL);


	// Beschreibungsfeld für das Level
	BeschreibungsFeld = CreateWindow("EDIT",
			 					  "Description",
								  WS_CHILD | WS_BORDER | WS_VISIBLE | 
								  ES_LEFT  | ES_WANTRETURN,
								  10, 20,
								  200, 20,
								  MainHWND,(HMENU)EDIT_ID_Beschreibung,hinstance,NULL);	

	// Farbe1 für Liquid
	Color1Feld = CreateWindow("EDIT",
			 					  "FFFFFFFF",
								  WS_CHILD | WS_BORDER | WS_VISIBLE | 
								  ES_LEFT  | ES_WANTRETURN,
								  310, 20,
								  80, 20,
								  MainHWND,(HMENU)EDIT_ID_Color1,hinstance,NULL);

	// Farbe2 für Liquid
	Color2Feld = CreateWindow("EDIT",
			 					  "FFFFFFFF",
								  WS_CHILD | WS_BORDER | WS_VISIBLE | 
								  ES_LEFT  | ES_WANTRETURN,
								  400, 20,
								  80, 20,
								  MainHWND,(HMENU)EDIT_ID_Color2,hinstance,NULL);

	// Value1 Feld für das Objekt
	Value1Feld = CreateWindow("EDIT",
			 				 "0",
							  WS_CHILD | WS_BORDER | WS_VISIBLE | 
							  ES_LEFT  | ES_NUMBER | ES_WANTRETURN,
							  735, 684,
							  60,  16,
							  MainHWND,(HMENU)EDIT_ID_Value1,hinstance, NULL);

	// Value2 Feld für das Objekt
	Value2Feld = CreateWindow("EDIT",
			 				 "0",
							  WS_CHILD | WS_BORDER | WS_VISIBLE | 
							  ES_LEFT  | ES_NUMBER | ES_WANTRETURN,
							  735, 699,
							  60,  16,
							  MainHWND,(HMENU)EDIT_ID_Value2,hinstance, NULL);

	// Button für Small Light
	SmallLight = CreateWindow("BUTTON",
								  "Small Light",
								  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								  375, 116,
								  100, 24,
								  MainHWND, (HMENU)BUTTON_ID_SmallLight, hinstance, NULL);

	// Button für Medium Light
	MediumLight = CreateWindow("BUTTON",
								  "Medium Light",
								  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								  375, 146,
								  100, 24,
								  MainHWND, (HMENU)BUTTON_ID_MediumLight, hinstance, NULL);

	// Button für Large Light
	LargeLight = CreateWindow("BUTTON",
								  "Large Light",
								  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								  375, 176,
								  100, 24,
								  MainHWND, (HMENU)BUTTON_ID_LargeLight, hinstance, NULL);

	// Button für Small Shadow
	SmallShadow = CreateWindow("BUTTON",
								  "Small Shadow",
								  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								  480, 116,
								  100, 24,
								  MainHWND, (HMENU)BUTTON_ID_SmallShadow, hinstance, NULL);

	// Button für Medium Shadow
	SmallShadow = CreateWindow("BUTTON",
								  "Med. Shadow",
								  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								  480, 146,
								  100, 24,
								  MainHWND, (HMENU)BUTTON_ID_MediumShadow, hinstance, NULL);

	// Button für Medium Shadow
	LargeShadow = CreateWindow("BUTTON",
								  "Large Shadow",
								  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								  480, 176,
								  100, 24,
								  MainHWND, (HMENU)BUTTON_ID_LargeShadow, hinstance, NULL);

	// Button für den Edit-Modus
	ButtonEditMode = CreateWindow("BUTTON",
								  "Level-Mode",
								  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								  10,  113,
								  100,  24,
								  MainHWND, (HMENU)BUTTON_ID_EditMode, hinstance, NULL);

	// Pulldown-Menu für auswählbare Tilesets
	ComboBoxSelectTileset = CreateWindow("COMBOBOX",
			 				   "Select Tileset",
							    WS_CHILD | WS_VISIBLE  | WS_VSCROLL | WS_BORDER | CBS_DROPDOWNLIST,
								740+10,	10+20,
								160,	280,
								MainHWND,(HMENU)BOX_ID_SelectTileset,hinstance,NULL);

	// Pulldown-Menu für auswählbaren Hintergrund
	ComboBoxSelectBackground = CreateWindow("COMBOBOX",
			 				   "Select Background",
							    WS_CHILD | WS_VISIBLE  | WS_VSCROLL | WS_BORDER | CBS_DROPDOWNLIST,
								495, 25,
								110, 280,
								MainHWND,(HMENU)BOX_ID_SelectBackground,hinstance,NULL);

	// Pulldown-Menu für auswählbaren Cloud-Layer
	ComboBoxSelectClouds = CreateWindow("COMBOBOX",
			 				   "Select Clouds",
							    WS_CHILD | WS_VISIBLE  | WS_VSCROLL | WS_BORDER | CBS_DROPDOWNLIST,
								495, 55,
								110, 280,
								MainHWND,(HMENU)BOX_ID_SelectClouds, hinstance,NULL);

	// Pulldown-Menu für auswählbare Stage-Music
	ComboBoxSelectStageMusic = CreateWindow("COMBOBOX",
			 				   "Stage",
							    WS_CHILD | WS_VISIBLE  | WS_VSCROLL | WS_BORDER | CBS_DROPDOWNLIST,
								655, 230,
								85, 160,
								MainHWND,(HMENU)BOX_ID_SelectStageMusic, hinstance,NULL);

	// Pulldown-Menu für auswählbare Boss-Music
	ComboBoxSelectBossMusic = CreateWindow("COMBOBOX",
			 				   "Boss",
							    WS_CHILD | WS_VISIBLE  | WS_VSCROLL | WS_BORDER | CBS_DROPDOWNLIST,
								655, 260,
								85, 160,
								MainHWND,(HMENU)BOX_ID_SelectBossMusic, hinstance,NULL);

	// Pulldown-Menu für auswählbaren Powerblock Typ
	ComboBoxSelectPowerblock = CreateWindow("COMBOBOX",
			 				   "Powerblock",
							    WS_CHILD | WS_VISIBLE  | WS_VSCROLL | WS_BORDER | CBS_DROPDOWNLIST,
								655, 310,
								85, 160,
								MainHWND,(HMENU)BOX_ID_SelectPowerblock, hinstance,NULL);

	// Pulldown-Menu für auswählbaren Parallax Layer 1
	ComboBoxSelectParallaxA = CreateWindow("COMBOBOX",
			 				   "Select ParallaxA",
							    WS_CHILD | WS_VISIBLE  | WS_VSCROLL | WS_BORDER | CBS_DROPDOWNLIST,
								610, 25,
								125, 280,
								MainHWND,(HMENU)BOX_ID_SelectParallaxA, hinstance,NULL);

	// Pulldown-Menu für auswählbaren Parallax Layer 2
	ComboBoxSelectParallaxB = CreateWindow("COMBOBOX",
			 				   "Select ParallaxB",
							    WS_CHILD | WS_VISIBLE  | WS_VSCROLL | WS_BORDER | CBS_DROPDOWNLIST,
								610, 55,
								125, 280,
								MainHWND,(HMENU)BOX_ID_SelectParallaxB, hinstance,NULL);

	// Button für "vorheriges Tileset"
	PreviousTileSet = CreateWindow("BUTTON",
			 				   "previous",
							    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								740+10,	10+50,
								80,		24,
								MainHWND,(HMENU)BUTTON_ID_PreviousTileSet,hinstance,NULL);

	// Button für "nächstes Tileset"
	NextTileSet = CreateWindow("BUTTON",
			 				   "next",
							    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								740+185, 10+50,
								80,		 24,
								MainHWND,(HMENU)BUTTON_ID_NextTileSet,hinstance,NULL);

	// Checkbox , ob "Rot" beim Licht/Schatten Effekt verwendet wird
	BoxLightRed = CreateWindow("BUTTON",
			 				   "Red",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								590, 117,
								60,  20,
								MainHWND,(HMENU)BUTTON_ID_LightRed,hinstance,NULL);

	// Checkbox , ob "Grün" beim Licht/Schatten Effekt verwendet wird
	BoxLightGreen = CreateWindow("BUTTON",
			 				   "Green",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								590, 147,
								60,  20,
								MainHWND,(HMENU)BUTTON_ID_LightGreen,hinstance,NULL);

	// Checkbox , ob "Blau" beim Licht/Schatten Effekt verwendet wird
	BoxLightBlue = CreateWindow("BUTTON",
			 				   "Blue",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								590, 177,
								60,  20,
								MainHWND,(HMENU)BUTTON_ID_LightBlue,hinstance,NULL);

	// Checkbox Button, ob das Grid angezeigt wird
	BoxShowGrid = CreateWindow("BUTTON",
			 				   "1 Grid",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								10,	 140,
								100,  20,
								MainHWND,(HMENU)BUTTON_ID_ShowGrid,hinstance,NULL);


	// Checkbox Button, ob der Background mitgescrollt wird
	BoxScrollBackground = CreateWindow("BUTTON",
			 				   "Scroll Back",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								265,  42,
								95,  20,
								MainHWND,(HMENU)BUTTON_ID_ScrollBackground,hinstance,NULL);	

	// Checkbox, ob die taschenlampe an oder aus sein soll
	BoxTaschenlampe = CreateWindow("BUTTON",
			 				   "Flashlight",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								365,  42,
								120,  20,
								MainHWND,(HMENU)BUTTON_ID_Taschenlampe,hinstance,NULL);	

	// Checkbox Button, ob der Background angezeigt wird
	BoxShowBackground = CreateWindow("BUTTON",
			 				   "2 Background",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								10,	 160,
								120,  20,
								MainHWND,(HMENU)BUTTON_ID_ShowBackground,hinstance,NULL);


	// Checkbox Button, ob das Level angezeigt wird
	BoxShowLevel = CreateWindow("BUTTON",
			 				   "3 Level",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								10,	180,
								80,  20,
								MainHWND,(HMENU)BUTTON_ID_ShowLevel,hinstance,NULL);

	// Checkbox Button, ob das Level-Overlay angezeigt wird
	BoxShowOverlay = CreateWindow("BUTTON",
			 				   "4 Overlay",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								140, 140,
								90,   20,
								MainHWND,(HMENU)BUTTON_ID_ShowOverlay,hinstance,NULL);

	// Checkbox Button, ob das Level-Overlay angezeigt wird
	BoxShowLight = CreateWindow("BUTTON",
			 				   "Light",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								140, 120,
								90,   20,
								MainHWND,(HMENU)BUTTON_ID_ShowLight,hinstance,NULL);

	// Checkbox Button, ob die Blockierungen angezeigt werden
	BoxShowBlock = CreateWindow("BUTTON",
			 				   "5 Block",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								140, 160,
								80,   20,
								MainHWND,(HMENU)BUTTON_ID_ShowBlock,hinstance,NULL);

	// Checkbox Button, ob die Blockierungen angezeigt werden
	BoxShowObjects = CreateWindow("BUTTON",
			 				   "6 Objekte",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								140, 180,
								80,   20,
								MainHWND,(HMENU)BUTTON_ID_ShowObjects,hinstance,NULL);

	// Checkbox Button, ob das TileOffset gemerkt werden soll
	BoxKeepTileOffset = CreateWindow("BUTTON",
			 				   "Save Offset",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								250,  110,
								106,  20,
								MainHWND,(HMENU)BUTTON_ID_KeepTileOffset,hinstance,NULL);

	// Checkbox Button, was ins Clipboard kopiert wird (Back)
	BoxCopyBack = CreateWindow("BUTTON",
			 				   "7 Background",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								250, 135,
								110,  20,
								MainHWND,(HMENU)BUTTON_ID_CopyBack,hinstance,NULL);

	// Checkbox Button, was ins Clipboard kopiert wird (Front)
	BoxCopyFront = CreateWindow("BUTTON",
			 				   "8 Overlay",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								250, 151,
								100,  20,
								MainHWND,(HMENU)BUTTON_ID_CopyFront,hinstance,NULL);

	// Checkbox Button, was ins Clipboard kopiert wird (Light)
	BoxCopyLight = CreateWindow("BUTTON",
			 				   "9 Color",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								250, 167,
								100,  20,
								MainHWND,(HMENU)BUTTON_ID_CopyLight,hinstance,NULL);

	// Checkbox Button, was ins Clipboard kopiert wird (Block)
	BoxCopyBlock = CreateWindow("BUTTON",
			 				   "0 Block-Art",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								250, 183,
								100,  20,
								MainHWND,(HMENU)BUTTON_ID_CopyBlock,hinstance,NULL);

	// Checkbox Button, ob sich ein Objekt im Licht des Levels verfärbt
	BoxChangeLight = CreateWindow("BUTTON",
			 				   "Adopt light",
							    WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								770, 655,
								130,  20,
								MainHWND,(HMENU)BUTTON_ID_ChangeLight,hinstance,NULL);

	// Rotwert der Farbe 
	RedValue = CreateWindow("SCROLLBAR", 
							  (LPSTR)NULL,
							  WS_CHILD | WS_VISIBLE | SBS_HORZ,
							  670+65,  340+20,
							  100,     17,
							  MainHWND,
							  (HMENU)BUTTON_ID_RedValue, hinstance,NULL);

	// Gruenwert der Farbe 
	GreenValue = CreateWindow("SCROLLBAR", 
							  (LPSTR)NULL,
							  WS_CHILD | WS_VISIBLE | SBS_HORZ,
							  670+65,  340+40,
							  100,	   17,
							  MainHWND,
							  (HMENU)BUTTON_ID_GreenValue, hinstance,NULL);

	// Blauwert der Farbe 
	BlueValue = CreateWindow("SCROLLBAR", 
							  (LPSTR)NULL,
							  WS_CHILD | WS_VISIBLE | SBS_HORZ,
							  670+65, 340+60,
							  100,	  17,
							  MainHWND,
							  (HMENU)BUTTON_ID_BlueValue, hinstance,NULL);

	// Alphawert der Farbe 
	AlphaValue = CreateWindow("SCROLLBAR", 
							  (LPSTR)NULL,
							  WS_CHILD | WS_VISIBLE | SBS_HORZ,
							  670+65, 340+80,
							  100,	  17,
							  MainHWND,
							  (HMENU)BUTTON_ID_AlphaValue, hinstance,NULL);

	// Alle Farbwerte Regler
	AllValues = CreateWindow("SCROLLBAR", 
							  (LPSTR)NULL,
							  WS_CHILD | WS_VISIBLE | SBS_HORZ,
							  670+65,  340+100,
							  100,	   17,
							  MainHWND,
							  (HMENU)BUTTON_ID_AllValues, hinstance,NULL);

	// Checkboxes mit den Block-Modi
	BoxBlockWertWand = CreateWindow("BUTTON", 
							  "wall",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  850,  355,
							  90,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_Wand, hinstance,NULL);

	BoxBlockWertGegnerWand = CreateWindow("BUTTON", 
							  "enemy wall",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  850,  370,
							  90,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_GegnerWand, hinstance,NULL);

	BoxBlockWertPlattform = CreateWindow("BUTTON", 
							  "platfrm",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  850,  385,
							  90,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_Plattform, hinstance,NULL);

	BoxBlockWertLight = CreateWindow("BUTTON", 
							  "use light",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  850,  400,
							  90,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_Light, hinstance,NULL);

	BoxBlockWertVerdecken = CreateWindow("BUTTON", 
							  "overlap",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  850,  415,
							  90,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_Verdecken, hinstance,NULL);

	BoxBlockWertAnimiertB = CreateWindow("BUTTON", 
							  "an.back",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  850,  430,
							  90,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_AnimiertB, hinstance,NULL);

	BoxBlockWertAnimiertF = CreateWindow("BUTTON", 
							  "an.front",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  850,  445,
							  90,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_AnimiertF, hinstance,NULL);

	BoxBlockWertWasser = CreateWindow("BUTTON", 
							  "water",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  850,  460,
							  90,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_Wasser, hinstance,NULL);

	BoxBlockWertSchaden = CreateWindow("BUTTON", 
							  "damage",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  850,  475,
							  90,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_Schaden, hinstance,NULL);

	BoxBlockWertFliessBandL = CreateWindow("BUTTON", 
							  "conv. l.",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  850,  490,
							  90,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_FliessBandL, hinstance,NULL);

	BoxBlockWertFliessBandR = CreateWindow("BUTTON", 
							  "conv. r.",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  850,  505,
							  90,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_FliessBandR, hinstance,NULL);

	BoxBlockWertWendePunkt = CreateWindow("BUTTON", 
							  "turn",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  850,  520,
							  90,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_WendePunkt, hinstance,NULL);

	BoxBlockWertDestructible = CreateWindow("BUTTON", 
							  "destroyable",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  850,  535,
							  90,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_Destructible, hinstance,NULL);

	BoxBlockWertSaeure = CreateWindow("BUTTON", 
							  "tex left",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  850,  550,
							  90,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_MOVELINKS, hinstance,NULL);

	BoxBlockWertOverlay_Light = CreateWindow("BUTTON", 
							  "overlight",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  850,  565,
							  90,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_Overlay_Light, hinstance,NULL);

	BoxBlockWertLava = CreateWindow("BUTTON", 
							  "swamp",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  850,  580,
							  90,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_Lava, hinstance,NULL);

	BoxBlockWertEis = CreateWindow("BUTTON", 
							  "ice",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  945,  355,
							  80,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_Eis, hinstance,NULL);

	BoxBlockWertMagen = CreateWindow("BUTTON", 
							  "tex down",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  945,  370,
							  80,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BLOCKWERT_MOVEVERTICAL, hinstance,NULL);

	BoxBlockWertWasserFall = CreateWindow("BUTTON", 
							  "waterfall",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  945,  385,
							  80,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_WasserFall, hinstance,NULL);

	BoxBlockWertTexRechts = CreateWindow("BUTTON", 
							  "tex right",
							  WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							  945,  400,
							  80,	20,
							  MainHWND,
							  (HMENU)BOX_ID_BlockWert_TexRechts, hinstance,NULL);


	// Auswahlliste mit den Objekt-Typen
	ComboBoxObjectType = CreateWindow("COMBOBOX", 
							  "Object",
							  WS_CHILD | WS_VISIBLE  | WS_VSCROLL | WS_BORDER | CBS_DROPDOWNLIST,
							  670+8,  620,
							  150,	  140,
							  MainHWND,
							  (HMENU)BOX_ID_ObjectType, hinstance,NULL);

	// Radio-Buttons zur Wahl des Skills der Gegner
	// Easy
	RadioButtonEasy = CreateWindow("BUTTON", 
									"Easy", 
									WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
									920, 640, 
									60,  24, 
									MainHWND, (HMENU)RADIOBUTTON_ID_Easy, 0, NULL);

	// Medium
	RadioButtonMedium = CreateWindow("BUTTON", 
									"Medium", 
									WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
									920, 661, 
									80,  24, 
									MainHWND, (HMENU)RADIOBUTTON_ID_Medium, 0, NULL);

	// Hard
	RadioButtonHard = CreateWindow("BUTTON", 
									"Hard", 
									WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
									920, 682, 
									50,  24, 
									MainHWND, (HMENU)RADIOBUTTON_ID_Hard, 0, NULL);

	// Scrollranges der Scrollbars setzen
	SetScrollRange(RedValue,   SB_CTL, 0, 255, false);
	SetScrollRange(GreenValue, SB_CTL, 0, 255, false);
	SetScrollRange(BlueValue,  SB_CTL, 0, 255, false);
	SetScrollRange(AlphaValue, SB_CTL, 0, 255, false);
	SetScrollRange(AllValues,  SB_CTL, 0, 255, false);

	// Objekt List Box mit Werten füllen
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"0  Player start");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"1  One Up");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"2  Diamond");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"3  PowerBlock");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"4  Spitter");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"5  Walker");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"6  GunTower");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"7  SpinnenBombe");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"8  Piranha");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"9  Stahlmücke");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"10 Deckenturm");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"11 Kugel klein");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"12 Kugel medium");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"13 Kugel gross");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"14 Kugel riesig");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"15 Krabbler oben");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"16 Krabbler links");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"17 Krabbler rechts");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"18 Auge Extracontainer");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"19 Qualle");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"20 Fallender Stein");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"21 Broeselnder Stein");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"22 Robo Raupe");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"23 Stalagtit");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"24 Raketenwerfer");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"25 Schwimmender Walker");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"26 Flugsack");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"27 ReitFlugSack");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"28 Eis-Stachel");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"29 Ski-Walker");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"30 FlugKanone");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"31 Schnee Bombe");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"32 Robo Flug Raketen");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"33 Blauer Boulder");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"34 Stelzsack");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"35 Fall Bombe");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"36 Poke Nuke");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"37 Lava Krabbe");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"38 Lava Ball Spawner");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"39 Lava Ball");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"40 Fette Spinne");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"41 Eiermann");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"42 Wandcannon");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"43 Star Small");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"44 Star Big");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"45 Made");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"46 Drone");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"47 Neuer Fisch");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"48 Mario Pflanze");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"49 Spitterbombe");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"50 Wespennest");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"51 Schienenviech");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"52 Wassermine");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"53 Fledermaus");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"54 Climb Spider");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"55 Fire  Spider");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"56 Ballerdrone");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"57 Schwabbel");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"58 Big Rocket");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"59 Eiszapfen");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"60 Wandkrabbe");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"61 Jaeger");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"62 Siderocket");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"63 Fiese kleine Drone");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"64 Schleimboller");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"65 Schleim Alien");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"66 Schleim Maul");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"67 Fieser Walker");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"68 Fieser Fire Walker");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"69 MittelSpinne");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"70 Kleine Wespe");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"71 Decken Krabbe");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"72 StampferStein");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"73 Lava Mann");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"74 La Fass (dont use)");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"75 Geschuetzturm");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"76 Fette Rakete");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"77 Schabe");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"78 Mini Dragon");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"79 leer lassen");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"80 Zitrone");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"81 Rotzpott");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"82 Stachelbeere");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"83 Minirocket");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"84 Mutant");

	// Lücken werden später noch mit anderen Gegnern gefüllt																					
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"85 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"86 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"87 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"88 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"89 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"90 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"91 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"92 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"93 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"94 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"95 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"96 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"97 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"98 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"99 Nix");

	// Viertel Gegner
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"100 Riesen Piranha");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"101 Riesen Qualle");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"102 Riesen Raupe");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"103 Riesen Wespe");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"104 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"105 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"106 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"107 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"108 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"109 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"110 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"111 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"112 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"113 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"114 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"115 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"116 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"117 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"118 Nix");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"119 Nix");

	// Zwischen und Endgegner
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"120 Stahlfaust Boss");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"121 Pharao Kopf Boss");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"122 Riesenspinne Boss");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"123 Böser Hurrican");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"124 Ufo");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"125 Fahrstuhlboss");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"126 FlugBoss");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"127 Bratklops");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"128 T.R. Schmidts Orgie");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"129 Metalhead");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"130 Eis Faust");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"131 Wuxe Spinnen");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"132 Golem");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"133 Spinnenmaschine");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"134 Drache");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"135 Rollmops");				
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"136 Schneekoenig");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"137 Bigfish");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"138 Skeletor");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"139 The Wall");

	// Trigger und sonstige Objekte
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"140 Partikel Spawner");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"141 Fahrstuhl");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"142 Explosives Fass");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"143 Feuerfalle");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"144 Fade Music Trigger");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"145 Endlevel Trigger");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"146 Plattform");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"147 Presse");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"148 Trenzshrine");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"149 Bruecke");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"150 Floating Platform");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"151 Floating Platform2");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"152 Turning Platform");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"153 Surfbrett");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"154 don't use!");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"155 Shoot-Plattform");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"156 Glubschi");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"157 Glubschi2");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"158 Column");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"159 Exploding Lift");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"160 Column2");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"161 Schleuse H");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"162 Schleuse V");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"163 Switch");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"164 Warning");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"165 Sound Trigger");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"166 Luefter Gr");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"167 Luefter Kl 1");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"168 Luefter Kl 2");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"169 Tutorial Text");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"170 LightFlare");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"171 Secret Room");
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"172 Mushroom Trampolin");	
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"173 Presswurst");	
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"174 La Fass Spawn");	
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"175 Spikelift");	
	SendMessage(ComboBoxObjectType, CB_ADDSTRING, 0, (LPARAM)"176 Tube");

	// Objekt Nr 0 am Anfang auswählen
	SendMessage(ComboBoxObjectType, CB_SETCURSEL, 0, 0);

	// Objekt Zustände auf "unchecked" setzen
	SendMessage(BoxChangeLight, BM_SETCHECK, BST_UNCHECKED, 0);

	// Alle Clipboard Optionen auf true setzen
	SendMessage(BoxCopyBack,  BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(BoxCopyFront, BM_SETCHECK, BST_UNCHECKED, 0);
	SendMessage(BoxCopyLight, BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(BoxCopyBlock, BM_SETCHECK, BST_CHECKED, 0);

	char temp[512];

	// Tileset Liste mit Dateinamen füllen	
	SendMessage(ComboBoxSelectTileset, CB_ADDSTRING, 0, (LPARAM)"none");

	strcpy(temp, HurriPath);	strcat(temp, "data/s_*.bmp");
	SendMessage(ComboBoxSelectTileset, CB_DIR, DDL_READWRITE, (LPARAM) temp);

	strcpy(temp, HurriPath);	strcat(temp, "data/s_*.png");
	SendMessage(ComboBoxSelectTileset, CB_DIR, DDL_READWRITE, (LPARAM) temp);

	// Background Pulldown-Menus mit Dateinamen füllen
	strcpy(temp, HurriPath);	strcat(temp, "data/static_*.bmp");
	SendMessage(ComboBoxSelectBackground, CB_DIR, DDL_READWRITE, (LPARAM) temp);

	strcpy(temp, HurriPath);	strcat(temp, "data/static_*.png");
	SendMessage(ComboBoxSelectBackground, CB_DIR, DDL_READWRITE, (LPARAM) temp);

	strcpy(temp, HurriPath);	strcat(temp, "data/back_*.bmp");
	SendMessage(ComboBoxSelectParallaxA,  CB_DIR, DDL_READWRITE, (LPARAM) temp);

	strcpy(temp, HurriPath);	strcat(temp, "data/back_*.png");
	SendMessage(ComboBoxSelectParallaxA,  CB_DIR, DDL_READWRITE, (LPARAM) temp);

	strcpy(temp, HurriPath);	strcat(temp, "data/front_*.bmp");
	SendMessage(ComboBoxSelectParallaxB,  CB_DIR, DDL_READWRITE, (LPARAM) temp);

	strcpy(temp, HurriPath);	strcat(temp, "data/front_*.png");
	SendMessage(ComboBoxSelectParallaxB,  CB_DIR, DDL_READWRITE, (LPARAM) temp);

	strcpy(temp, HurriPath);	strcat(temp, "data/clouds*.bmp");
	SendMessage(ComboBoxSelectClouds,	  CB_DIR, DDL_READWRITE, (LPARAM) temp);

	strcpy(temp, HurriPath);	strcat(temp, "data/clouds*.png");
	SendMessage(ComboBoxSelectClouds,	  CB_DIR, DDL_READWRITE, (LPARAM) temp);

	SendMessage(ComboBoxSelectBackground, CB_SETCURSEL, 0, 0);
	SendMessage(ComboBoxSelectParallaxA,  CB_SETCURSEL, 0, 0);
	SendMessage(ComboBoxSelectParallaxB,  CB_SETCURSEL, 0, 0);
	SendMessage(ComboBoxSelectClouds,	  CB_SETCURSEL, 0, 0);

	// Rausfinden, wieviele verschiedene Sets geladen wurden
	LoadedTilesets = (int)(SendMessage(ComboBoxSelectTileset, CB_GETCOUNT, 0, 0));
	pTileEngine->LoadedTilesets = LoadedTilesets-1;

	// Tileset auf "keines" am Anfang auswählen
	SendMessage(ComboBoxSelectTileset, CB_SETCURSEL, 0, 0);

	// Tileset Sprites mit den möglichen Tilesets füllen
	for(int i=1; i<LoadedTilesets; i++)
	{
		// Namen des i-ten Tilesets in der ComboBox erstellen
		SendMessage(ComboBoxSelectTileset, CB_GETLBTEXT, i, (LPARAM) Buffer);

		// Und Tileset laden
		pTileEngine->TileGfx[i].LoadFromGfx(Buffer, 256, 256, 20, 20, 12, 12);
	}

	// ShowLevel   auf "true" setzen
	SendMessage(BoxShowLevel,   BM_SETCHECK, BST_CHECKED, 0);

	// ShowOverlay auf "true" setzen
	SendMessage(BoxShowOverlay,   BM_SETCHECK, BST_CHECKED, 0);

	// ShowLight auf "true" setzen
	SendMessage(BoxShowLight,   BM_SETCHECK, BST_CHECKED, 0);

	// ShowEnemies auf "true" setzen
	SendMessage(BoxShowObjects, BM_SETCHECK, BST_CHECKED, 0);

	// Red Green und Blue im Licht/Schatten Fenster auf true setzen
	SendMessage(BoxLightRed,   BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(BoxLightGreen, BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(BoxLightBlue,  BM_SETCHECK, BST_CHECKED, 0);

	// Keep Offset Status auf true setzen
	SendMessage(BoxKeepTileOffset, BM_SETCHECK, BST_UNCHECKED, 0);

	// Combo Boxen f+r Musik
	strcpy(temp, HurriPath);	strcat(temp, "data/*.it");
	SendMessage(ComboBoxSelectStageMusic,	  CB_DIR, DDL_READWRITE, (LPARAM) temp);

	strcpy(temp, HurriPath);	strcat(temp, "data/*.s3m");
	SendMessage(ComboBoxSelectStageMusic,	  CB_DIR, DDL_READWRITE, (LPARAM) temp);

	strcpy(temp, HurriPath);	strcat(temp, "data/*.xm");
	SendMessage(ComboBoxSelectStageMusic,	  CB_DIR, DDL_READWRITE, (LPARAM) temp);

	strcpy(temp, HurriPath);	strcat(temp, "data/*.mod");
	SendMessage(ComboBoxSelectStageMusic,	  CB_DIR, DDL_READWRITE, (LPARAM) temp);

	strcpy(temp, HurriPath);	strcat(temp, "data/*.it");
	SendMessage(ComboBoxSelectBossMusic,	  CB_DIR, DDL_READWRITE, (LPARAM) temp);

	strcpy(temp, HurriPath);	strcat(temp, "data/*.s3m");
	SendMessage(ComboBoxSelectBossMusic,	  CB_DIR, DDL_READWRITE, (LPARAM) temp);

	strcpy(temp, HurriPath);	strcat(temp, "data/*.xm");
	SendMessage(ComboBoxSelectBossMusic,	  CB_DIR, DDL_READWRITE, (LPARAM) temp);

	strcpy(temp, HurriPath);	strcat(temp, "data/*.mod");
	SendMessage(ComboBoxSelectBossMusic,	  CB_DIR, DDL_READWRITE, (LPARAM) temp);

	SendMessage(ComboBoxSelectStageMusic, CB_SETCURSEL, 0, 0);
	SendMessage(ComboBoxSelectBossMusic,  CB_SETCURSEL, 1, 0);

	strcpy(Appendix.Songs[0], "Machine Monotony.it");
	strcpy(Appendix.Songs[1], "Machine Monotony.it");

	// Powerblock Types
	SendMessage(ComboBoxSelectPowerblock, CB_ADDSTRING, 0, (LPARAM)"0 Jungle");
	SendMessage(ComboBoxSelectPowerblock, CB_ADDSTRING, 0, (LPARAM)"1 Temple");
	SendMessage(ComboBoxSelectPowerblock, CB_ADDSTRING, 0, (LPARAM)"2 Cave");
	SendMessage(ComboBoxSelectPowerblock, CB_ADDSTRING, 0, (LPARAM)"3 Mechanic");
	SendMessage(ComboBoxSelectPowerblock, CB_ADDSTRING, 0, (LPARAM)"4 Ice");

	Appendix.UsedPowerblock = 0;
	SendMessage(ComboBoxSelectPowerblock, CB_SETCURSEL, 0, 0);
}

// --------------------------------------------------------------------------------------
// Dec Value einer Hex zahl ermitteln
// --------------------------------------------------------------------------------------

int GetDecValue(char *pair, int len)
{
	int r = 0;
	int f = 0;
	char c;

	for (int i = 0; i < len; i++)
	{
		c = pair[i];

		switch (c)
		{
			case '0' : f = 0; break;
			case '1' : f = 1; break;
			case '2' : f = 2; break;
			case '3' : f = 3; break;
			case '4' : f = 4; break;
			case '5' : f = 5; break;
			case '6' : f = 6; break;
			case '7' : f = 7; break;
			case '8' : f = 8; break;
			case '9' : f = 9; break;
			case 'A' : f = 10; break;
			case 'B' : f = 11; break;
			case 'C' : f = 12; break;
			case 'D' : f = 13; break;
			case 'E' : f = 14; break;
			case 'F' : f = 15; break;			
		}

		for (int j = 0; j < i; j++)
			f *= 16;

		r += f;
	}

	return r;
}

void SaveUndoStep(bool reset)
{	
	UndoOffsetX[CurrentUndoStep] = XOffset;
	UndoOffsetY[CurrentUndoStep] = YOffset;	

	for (int x = 0; x < SCREENSIZE_X; x++)
		for (int y = 0; y < SCREENSIZE_Y; y++)
			memcpy(&UndoBuffer[CurrentUndoStep][x][y], &pTileEngine->Tiles[x + XOffset][y + YOffset], sizeof(LevelTileStruct));	

	CurrentUndoStep++;

	if (CurrentUndoStep >= MAX_UNDO_STEPS)
		CurrentUndoStep -= MAX_UNDO_STEPS;
}

void RestoreUndoStep(bool back)
{
	if (back)
	{
		SaveUndoStep(false);
		CurrentUndoStep -= 2;
	}
	else
	
	CurrentUndoStep += 1;

	if (CurrentUndoStep < 0)
		CurrentUndoStep += MAX_UNDO_STEPS;

	if (CurrentUndoStep >= MAX_UNDO_STEPS)
		CurrentUndoStep -= MAX_UNDO_STEPS;


	for (int x = 0; x < SCREENSIZE_X; x++)
		for (int y = 0; y < SCREENSIZE_Y; y++)
			memcpy(&pTileEngine->Tiles[x + UndoOffsetX[CurrentUndoStep]][y + UndoOffsetY[CurrentUndoStep]], &UndoBuffer[CurrentUndoStep][x][y], sizeof(LevelTileStruct));	

	// warten bis user loslässt
	while (KeyDown(DIK_U))
		DirectInput.TastaturUpdate();

	RedrawLevel = true;	
}