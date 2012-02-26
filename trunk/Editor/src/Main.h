// Datei : Main.h

// -------------------------------------------------------------------------------------- 
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __Main_h__
#define __Main_h__

// --------------------------------------------------------------------------------------
// Defines 
// --------------------------------------------------------------------------------------

#define WIN_32_LEAN_AND_MEAN						// MFC abschalten
#define WINDOWCLASSNAME	"myWindowsClass"			// Name der Windows-Klasse
#define WINDOWWIDTH		1024						// Fensterbreite
#define WINDOWHEIGHT	768							// Fensterhöhe
#define VIEW_XOFFSET    10							// X-Pos des Levels im Fenster
#define VIEW_YOFFSET    210							// Y-Pos des Levels im Fenster
#define TILE_XOFFSET	760							// X-Pos des Tilesets zur Auswahl
#define TILE_YOFFSET	90							// Y-Pos des Tilesets zur Auswahl
#define MINIMAP_XOFFSET	689							// X-Pos der MiniMap
#define MINIMAP_YOFFSET	480							// Y-Pos der MiniMap
#define MAX_UNDO_STEPS	256

// --------------------------------------------------------------------------------------
// Strukturen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Level-Datei Header
// --------------------------------------------------------------------------------------

struct FileHeader
{
	char			Kennung[46];					// Level-Kennung
	char			Beschreibung[100];				// Level-Beschreibung
	char			BackgroundFile[24];				// Dateiname des Hintergrundes
	char			ParallaxAFile [24];				// Dateiname des 1. Parallax Layers
	char			ParallaxBFile [24];				// Dateiname des 2. Parallax Layers
	char			CloudFile     [24];				// Dateiname des WolkenLayers
	unsigned int	Timelimit;						// Zeitlimit des Levels
	unsigned char	UsedTilesets;					// Anzahl der Tilesets
	char			SetNames[64][16];				// Namen der benutzten Sets
	unsigned int	SizeX, SizeY;					// Größe des Levels
	unsigned int	NumObjects;						// Anzahl der Objekte
	bool			ScrollBackground;				// Hintergrund scrollen ?
};

// --------------------------------------------------------------------------------------
// Anhang am File nach den Level Daten
// damit nicht alle levels nochmal konvertiert werden, hänge ich einfach alle dinge, die
// noch ins level format reinmüssen, dahinter =)
// --------------------------------------------------------------------------------------

struct FileAppendix
{
	char			Songs[2][30];					// Namen der benutzten Songs (Stage und Boss)
	int				UsedPowerblock;					// ID der benutzten Powerblock Art
	char			Col1[8], Col2[8];				// Farben für Liquid
	bool			Taschenlampe;					// Taschenlampeneffekt
};

// --------------------------------------------------------------------------------------
// Struktur für das Speichern und Laden eines Objektes
// --------------------------------------------------------------------------------------

struct ObjectForFileAccess
{
	unsigned int	ObjectID;					// Welche Objekt ID ?
	int				XPos;						// x-Position
	int				YPos;						// y-Position
	bool			ChangeLight;				// Umgebungslicht annehmen ?
	unsigned char	Skill;						// 0 = Easy, 1 = Medium, 2 = Hard
	int				Value1;						// Werte für diverse Trigger
	int				Value2;						// Werte für diverse Trigger
};

// --------------------------------------------------------------------------------------
// Objekt Klasse
// --------------------------------------------------------------------------------------

class Object
{
	public:
		unsigned int	ObjectID;					// Welche Objekt ID ?
		int				XPos;						// x-Position
		int				YPos;						// y-Position
		bool			ChangeLight;				// Umgebungslicht annehmen ?
		unsigned char	Skill;						// 0 = Easy, 1 = Medium, 2 = Hard
		int				Value1;						// Werte für diverse Trigger
		int				Value2;						// Werte für diverse Trigger

		Object			*pPrev;						// Vorheriges Listen-Objekt
		Object			*pNext;						// Nächstes Listen-Objekt
};

// --------------------------------------------------------------------------------------
// Objekt-Listen Klasse
// --------------------------------------------------------------------------------------

class ObjectList
{
   public:
        Object *pStart;								// Zeiger auf das erste  Element
		Object *pEnd;								// Zeiger auf das letzte Element
 
        ObjectList			(void);					// Konstruktor
	   ~ObjectList			(void);					// Desktruktor
        void    Push		(int x, int y,	    	// Neues Objekt zur Liste hinzufügen
						     unsigned int ObjID);
		void	Push		(ObjectForFileAccess Temp); // Für das Objekt Laden
		void    DelSel		(Object *pTemp);		// Ausgewähltes Objekt entfernen
		void    ShowAll		(void);					// Alle Objekts anzeigen
		void    ClearAll	(void);					// Alle Objekts löschen
		void    GetSelected (void);					// Das mit der Maus selektierte Objekt suchen
		int		GetNumType	(int Type);				// Anzahl Objekte eines bestimmten Typs holen
};

// --------------------------------------------------------------------------------------
// Prototypen von Funktionen
// --------------------------------------------------------------------------------------

bool GameInit			(HINSTANCE hinstance);		// Spiel initialisieren
void CreateEnvironment	(HINSTANCE hinstance);		// Editor Umgebung "bauen"
bool GameExit			(void);						// Spiel de-initialisieren
bool Heartbeat			(void);						// Haupt Game Loop
void DrawLevel			(void);						// Level malen
void DrawTileset		(void);						// Tile malen
void DrawMiniMap		(void);						// MiniMap malen
void ShowValues			(void);						// Werte des Levels anzeigen
void DoMouse			(void);						// Maus abhandeln
void DoKeys				(void);						// Tastatur checken
bool LoadClipboard		(void);
bool SaveClipboard		(void);
bool SaveLevel			(bool SaveAs);				// Level speichern
bool LoadLevel			(void);						// Level laden
void TestLevel			(void);						// Level testen
void CopyFromBlock		(void);
void ShowObjectInfo	    (void);
void SaveUndoStep		(bool reset);
void RestoreUndoStep	(bool back);
int	 GetDecValue(char *pair, int len);			// Dezimalwert einer Hexzahl mit Länge len

#endif