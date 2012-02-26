// Datei : DX8Font.h

// -------------------------------------------------------------------------------------- 
//
// Font Klasse
// zum Anzeigen von Schriften
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include-Dateien
// --------------------------------------------------------------------------------------

#include <stdio.h>
#include "Globals.h"
#include "Logdatei.h"
#include "Timer.h"
#include "DX8Font.h"
#include "DX8Graphics.h"


// --------------------------------------------------------------------------------------
// Konstruktor (leer)
// --------------------------------------------------------------------------------------

DirectGraphicsFont::DirectGraphicsFont(void)
{
	mTexture = NULL;
}

// --------------------------------------------------------------------------------------
// Desktruktor gibt die Textur des Fonts wieder frei
// --------------------------------------------------------------------------------------

DirectGraphicsFont::~DirectGraphicsFont(void)
{
	// Font Textur freigeben
	delete(mTexture);
	mTexture = NULL;
}

// --------------------------------------------------------------------------------------
// Laden des Bildes "Filename" mit
// x-Grösse eines Zeichens und
// y-Grösse eines Zeichens und
// Anzahl der Zeichen pro Zeile
// --------------------------------------------------------------------------------------

bool DirectGraphicsFont::LoadFont(char Filename[256], int xts, int yts,
								  int xCharsize, int yCharsize, int xChars,int yChars)
{
	mTexture = new (DirectGraphicsSprite);
	mTexture->LoadImage(Filename, xts, yts, xCharsize, yCharsize, xChars, 0);
	
	// Grösse setzen
	//
	mXCharSize		= xCharsize;
	mYCharSize		= yCharsize;
	mXChars			= xChars;
	mXTextureSize	= xts;
	mYTextureSize	= yts;

	// Länge der einzelnen Zeichen aus der Grafik bestimmen
	//

	// Geladene Font Textur locken
	//
	D3DSURFACE_DESC d3dsd;
	D3DLOCKED_RECT  d3dlr;

	HRESULT hresult;
/*
	mTexture->itsTexture->GetLevelDesc(0, &d3dsd);
	hresult = mTexture->itsTexture->LockRect    (0, &d3dlr, 0, 0 );

	// Colorkey feststellen
	DWORD key = ((DWORD*)d3dlr.pBits)[0];
	key = 0;
*/
	
	// Einzelne Zeichen durchgehen
	for (int i=0; i < xChars; i++)
	 for (int j=0; j < yChars; j++)
	 {
/*		int		last  = 0;
		bool	found = false;

		for (int k = 0; k<xCharsize; k++)
		{
			found = false;

			for (int l = 0; l<yCharsize; l++) 
			{
				if (((DWORD*)d3dlr.pBits)[(j * yCharsize + l) * d3dsd.Width + (i*xCharsize + k)] != key)
					found = true;
			}

			if (found == true)
				last = k;
		}
*/

//		mCharLength[j * xChars + i] = last+1;
		mCharLength[j * xChars + i] = xCharsize-1;
	 }

	// Unlocken
	mTexture->itsTexture->UnlockRect(0);
	
	return true;
}

// --------------------------------------------------------------------------------------
// Zahlenwert an xPos,yPos mit Farbe Color ausgeben
// --------------------------------------------------------------------------------------

bool DirectGraphicsFont::DrawValue(float x, float y, float Value, D3DCOLOR Color)
{
	char Buf[20];

	sprintf(Buf, "%f", Value);
	DrawText(x, y, Buf, Color);

	return true;
}

// --------------------------------------------------------------------------------------
// Text an xPos,yPos mit Farbe Color ausgeben
// --------------------------------------------------------------------------------------

bool DirectGraphicsFont::DrawText(float x, float y, char Text[], D3DCOLOR Color)
{
	RECT rect;
	unsigned char z;
	float oldx = x;

	for(unsigned int i=0; i<strlen(Text); i++)
	{
		z = Text[i];						// Aktuell zu bearbeitendes Zeichen holen
		z -=33;								// "!" als erstes Zeichen setzen, das heisst,
											// Fontgrafik muss mit "!" beginnen
		// Position des aktuellen Zeichens in der Grafik berechnen
		rect.left   = (z%mXChars)*mXCharSize;
		rect.top    = (z/mXChars)*mYCharSize;
		rect.right  = rect.left + mXCharSize;
		rect.bottom = rect.top  + mYCharSize;

		mTexture->SetRect(rect.left, rect.top, rect.right, rect.bottom);

		if(Text[i] != 32 &&
		   Text[i] != '\n') 
		   mTexture->RenderSprite(x, y, Color);

		if(Text[i] == 32) 
			x += mXCharSize-3;				// Bei Space frei lassen
		else			  
		if(Text[i] == '\n')					// Zeilenumbruch
		{
			x = oldx;
			y += mYCharSize + 6;
		}
		else			  
			x += mCharLength[z]-1;			// Ansonsten Breite des Zeichens weiter
	}

	return true;
}

void DirectGraphicsFont::DrawTextRightAlign(float x, float y, char Text[], D3DCOLOR Color)
{
	DrawText(x - StringLength(Text, 0), y, Text, Color);
}

// --------------------------------------------------------------------------------------
// Zeichen an xPos,yPos mit Farbe Color ausgeben (Demo Font, mit mehr Platz zwischen Zeichen)
// --------------------------------------------------------------------------------------

bool DirectGraphicsFont::DrawDemoChar(float x, float y, char Text, D3DCOLOR Color)
{
	if(Text == 32 ||
	   Text == '\n') 
	   return false;

	RECT rect;
	unsigned char z;

	z = Text;							// Aktuell zu bearbeitendes Zeichen holen
	z -=33;								// "!" als erstes Zeichen setzen, das heisst,
											// Fontgrafik muss mit "!" beginnen
	// Position des aktuellen Zeichens in der Grafik berechnen
	rect.left   = (z%mXChars)*mXCharSize;
	rect.top    = (z/mXChars)*mYCharSize;
	rect.right  = rect.left + mXCharSize;
	rect.bottom = rect.top  + mYCharSize;

	mTexture->SetRect(rect.left, rect.top, rect.right, rect.bottom);	
	mTexture->RenderSprite(x, y, Color);

	return true;
}

// --------------------------------------------------------------------------------------
// Text an xPos,yPos mit Farbe Color ausgeben (Demo Font, mit mehr Platz zwischen Zeichen)
// --------------------------------------------------------------------------------------

bool DirectGraphicsFont::DrawDemoText(float x, float y, char Text[], D3DCOLOR Color)
{
	RECT rect;
	unsigned char z;
	float oldx = x;

	for(unsigned int i=0; i<strlen(Text); i++)
	{
		z = Text[i];						// Aktuell zu bearbeitendes Zeichen holen
		z -=33;								// "!" als erstes Zeichen setzen, das heisst,
											// Fontgrafik muss mit "!" beginnen
		// Position des aktuellen Zeichens in der Grafik berechnen
		rect.left   = (z%mXChars)*mXCharSize;
		rect.top    = (z/mXChars)*mYCharSize;
		rect.right  = rect.left + mXCharSize;
		rect.bottom = rect.top  + mYCharSize;

		mTexture->SetRect(rect.left, rect.top, rect.right, rect.bottom);

		if(Text[i] != 32 &&
		   Text[i] != '\n') 
		   mTexture->RenderSprite(x, y, Color);

		if(Text[i] == 32) 
			x += mXCharSize;				// Bei Space frei lassen
		else			  
		if(Text[i] == '\n')					// Zeilenumbruch
		{
			x = oldx;
			y += mYCharSize + 6;
		}
		else			  
			x += mCharLength[z]+1;			// Ansonsten Breite des Zeichens weiter
	}

	return true;
}

// --------------------------------------------------------------------------------------
// Text an xPos,yPos mit Farbe Color ausgeben, aber "Spacing" Pixel Platz dazwischen lassen
// --------------------------------------------------------------------------------------

bool DirectGraphicsFont::DrawText(float x, float y, char Text[], D3DCOLOR Color, int Spacing)
{
	RECT rect;
	unsigned char z;
	float oldx = x;

	for(unsigned int i=0; i<strlen(Text); i++)
	{
		z = Text[i];						// Aktuell zu bearbeitendes Zeichen holen
		z -=33;								// "!" als erstes Zeichen setzen, das heisst,
											// Fontgrafik muss mit "!" beginnen
		// Position des aktuellen Zeichens in der Grafik berechnen
		rect.left   = (z%mXChars)*mXCharSize;
		rect.top    = (z/mXChars)*mYCharSize;
		rect.right  = rect.left + mXCharSize;
		rect.bottom = rect.top  + mYCharSize;

		mTexture->SetRect(rect.left, rect.top, rect.right, rect.bottom);

		if(Text[i] != 32 &&
		   Text[i] != '\n') 
		   mTexture->RenderSprite(x, y, Color);

		if(Text[i] == 32) 
			x += mXCharSize-3 - Spacing*2;				// Bei Space frei lassen
		else			  
		if(Text[i] == '\n')								// Zeilenumbruch
		{
			x = oldx;
			y += mYCharSize + 6 + Spacing;
		}
		else			  
			x += mCharLength[z]-1 + Spacing;	// Ansonsten Breite des Zeichens weiter
	}

	return true;
}

// --------------------------------------------------------------------------------------
// Länge eines Strings in Pixeln zurückliefern
// --------------------------------------------------------------------------------------

int DirectGraphicsFont::DemoStringLength(char Text[])
{
	if (strlen(Text) <= 1)
		return 0;

	unsigned char z;
	int  l;

	l = 0;
	for(unsigned int i=0; i<strlen(Text)-1; i++)
	{
		z = Text[i];						// Aktuell zu bearbeitendes Zeichen holen
		z -=33;								// "!" als erstes Zeichen setzen, das heisst,
											// Fontgrafik muss mit "!" beginnen		
		if(Text[i] == 32) 
			l += mXCharSize-3;				// Bei Space frei lassen
		else			  
			l += mCharLength[z];			// Ansonsten Breite des Zeichens weiter
	}

	return l;
}


// --------------------------------------------------------------------------------------
// Länge eines Strings in Pixeln zurückliefern
// --------------------------------------------------------------------------------------

int DirectGraphicsFont::StringLength(char Text[])
{
	if (strlen(Text) <= 1)
		return 0;

	unsigned char z;
	int  l;

	l = 0;
	for(unsigned int i=0; i<strlen(Text)-1; i++)
	{
		z = Text[i];						// Aktuell zu bearbeitendes Zeichen holen
		z -=33;								// "!" als erstes Zeichen setzen, das heisst,
											// Fontgrafik muss mit "!" beginnen		
		if(Text[i] == 32) 
			l += mXCharSize-1;				// Bei Space frei lassen
		else			  
			l += mCharLength[z]-1;			// Ansonsten Breite des Zeichens weiter
	}

	return l;
}

// --------------------------------------------------------------------------------------
// Länge eines Strings in Pixeln mit "Spacing" Pixeln freiraum zwischen den Zeichen zurückliefern
// --------------------------------------------------------------------------------------

int DirectGraphicsFont::StringLength(char Text[], int Spacing)
{
	if (strlen(Text) <= 1)
		return 0;

	unsigned char z;
	int  l;

	l = 0;
	for(unsigned int i=0; i<strlen(Text)-1; i++)
	{
		z = Text[i]
			;						// Aktuell zu bearbeitendes Zeichen holen
		z -=33;								// "!" als erstes Zeichen setzen, das heisst,
											// Fontgrafik muss mit "!" beginnen		
		if(Text[i] == 32) 
			l += mXCharSize-3 - Spacing*2;	// Bei Space frei lassen
		else			  
			l += mCharLength[z]-1+ Spacing;	// Ansonsten Breite des Zeichens weiter
	}

	return l;
}
