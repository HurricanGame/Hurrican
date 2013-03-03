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
#include "Gameplay.h"
#include "unrarlib.h"

// --------------------------------------------------------------------------------------
// Konstruktor (leer)
// --------------------------------------------------------------------------------------

DirectGraphicsFont::DirectGraphicsFont(void)
{
	mTexture = NULL;

	// alle mit 0 initialiseren, falls ein fehlerhaftest Zeichen verwendet wird
	for (int i = 0; i < 256; i++)
		mCharLength[i] = 0;
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

bool DirectGraphicsFont::LoadFont(const char *Filename, int xts, int yts,
								  int xCharsize, int yCharsize, int xChars,int yChars)
{
	mTexture = new (DirectGraphicsSprite);
	if (!mTexture->LoadImage(Filename, xts, yts, xCharsize, yCharsize, xChars, 0))
		return false;

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
#if defined(PLATFORM_DIRECTX)
	D3DSURFACE_DESC d3dsd;
	D3DLOCKED_RECT  d3dlr;

	HRESULT hresult;

	mTexture->itsTexture->GetLevelDesc(0, &d3dsd);
	hresult = mTexture->itsTexture->LockRect    (0, &d3dlr, 0, 0 );

	// Fehler beim Locken ?
	if (hresult != D3D_OK)
		Protokoll.WriteText("error locking font texture!", true);

	// Colorkey feststellen
	DWORD key = ((DWORD*)d3dlr.pBits)[0];
#elif defined(PLATFORM_SDL)
	image_t image;
	char Temp[256];
	char *pData;
	unsigned long Size;

	if (CommandLineParams.RunOwnLevelList == true)
	{
	    sprintf_s(Temp, "levels/%s/%s", CommandLineParams.OwnLevelList, Filename);
		if (FileExists(Temp))
		{
			loadImage(image, Temp);
		}
	}

	if (image.data == NULL)
	{
	    sprintf_s(Temp, "data/%s", Filename);
		if (FileExists(Temp))
		{
			loadImage(image, Temp);
		}
	}

	if (image.data == NULL)
	{
		if (urarlib_get(&pData, &Size, Filename, RARFILENAME, convertText(RARFILEPASSWORD)) != false)
	    {
			loadImage(image, pData, Size);
			free(pData);
		}
	}

	if (image.data  == NULL)
	{
		return true;
	}

#if 1
    /* menufont.png: pixel at (0,0) (upper left corner) is part of a char/glyph,
       so pick key from lower left corner. Maybe in DirectX image is flipped
       vertically? */

    DWORD key = (((DWORD*)image.data )[image.w * (image.h - 1)]);
#else
    DWORD key = ((DWORD*)image.data )[0];
#endif

/* // PICKLE Not OpenGLES compat, left for info
    int textureWidth, textureHeight;
    glBindTexture( GL_TEXTURE_2D, mTexture->itsTexture );
    glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &textureWidth );
    glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &textureHeight );

    GLubyte *buffer = (GLubyte *)malloc(textureWidth*textureHeight*4);
    glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
    DWORD key = ((DWORD*)buffer)[0];

    glBindTexture( GL_TEXTURE_2D, 0 );
*/
#endif
	//key = 0;

	// Einzelne Zeichen durchgehen
	for (int i=0; i < xChars; i++)
	 for (int j=0; j < yChars; j++)
	 {
		int		last  = 0;
		bool	found = false;

		for (int k = 0; k<xCharsize; k++)
		{
			found = false;

			for (int l = 0; l<yCharsize; l++)
			{
#if defined(PLATFORM_DIRECTX)
				if (((DWORD*)d3dlr.pBits)[(j * yCharsize + l) * d3dsd.Width + (i*xCharsize + k)] != key)
					found = true;
#elif defined(PLATFORM_SDL)
				if (((DWORD*)image.data)[(j * yCharsize + l) * image.w + (i*xCharsize + k)] != key)
					found = true;
/*
				if (((DWORD*)buffer)[(j * yCharsize + l) * textureWidth + (i*xCharsize + k)] != key)
					found = true;
*/
#endif
			}

			if (found == true)
				last = k;
		}

		mCharLength[j * xChars + i] = last+1;
	 }

#if defined(PLATFORM_DIRECTX)
	// Unlocken
	mTexture->itsTexture->UnlockRect(0);
#elif defined(PLATFORM_SDL)
    delete [] image.data;
#endif

	return false;
}

// --------------------------------------------------------------------------------------
// Zahlenwert an xPos,yPos mit Farbe Color ausgeben
// --------------------------------------------------------------------------------------

bool DirectGraphicsFont::DrawValue(float x, float y, float Value, D3DCOLOR Color)
{
	char Buf[20];

	sprintf_s(Buf, "%f", Value);
	DrawText(x, y, Buf, Color);

	return true;
}

// --------------------------------------------------------------------------------------
// Text an xPos,yPos mit Farbe Color ausgeben
// --------------------------------------------------------------------------------------

bool DirectGraphicsFont::DrawText(float x, float y, const char Text[], D3DCOLOR Color)
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

void DirectGraphicsFont::DrawTextRightAlign(float x, float y, const char Text[], D3DCOLOR Color, int Spacing)
{
	DrawText(x - StringLength(Text, Spacing), y, Text, Color, Spacing);
}

void DirectGraphicsFont::DrawTextCenterAlign(float x, float y, const char Text[], D3DCOLOR Color, int Spacing)
{
	DrawText(x - StringLength(Text, Spacing) / 2 , y, Text, Color, Spacing);
}

// --------------------------------------------------------------------------------------
// Zeichen an xPos,yPos mit Farbe Color ausgeben (Demo Font, mit mehr Platz zwischen Zeichen)
// --------------------------------------------------------------------------------------

bool DirectGraphicsFont::DrawDemoChar(float x, float y, const char Text, D3DCOLOR Color)
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

bool DirectGraphicsFont::DrawDemoText(float x, float y, const char Text[], D3DCOLOR Color)
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

bool DirectGraphicsFont::DrawText(float x, float y, const char Text[], D3DCOLOR Color, int Spacing)
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

int DirectGraphicsFont::DemoStringLength(const char Text[])
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

int DirectGraphicsFont::StringLength(const char Text[])
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

int DirectGraphicsFont::StringLength(const char Text[], int Spacing)
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


// --------------------------------------------------------------------------------------
// FPS Werte auf dem Screen anzeigen
// --------------------------------------------------------------------------------------

void DirectGraphicsFont::ShowFPS (void)
{
	static int    updateFPS;					// Trigger für die FPS, da sonst Anzeige zu schnell
	static double FPS;
	char	Buffer[20];
	double	Value;

	updateFPS++;
	if(updateFPS>FPS/2)
	{
		updateFPS = 0;
		FPS = pTimer->getFrameRate();
	}

	// Aktuelle FPS
	_itoa_s((int)(FPS), Buffer, 10);
	DrawText(0,   0, "Aktuelle FPS :", 0xFFFFFFFF);
	DrawText(150, 0, Buffer, 0xFFFFFFFF);

	// FPS Grenze
	_itoa_s((int)(pTimer->maxFPS), Buffer, 10);
	DrawText(200,   0, "FPS Grenze :", 0xFFFFFFFF);
	DrawText(300, 0, Buffer, 0xFFFFFFFF);

	// Durchschnittliche FPS
	Value = pTimer->getAverageFPS();
	_itoa_s((int)(Value), Buffer, 10);
	DrawText(0,   15, "Durchschnitt FPS :", 0xFFFFFFFF);
	DrawText(150, 15, Buffer, 0xFFFFFFFF);

	// Maximale FPS
	Value = pTimer->getMaxFrameRate();
	_itoa_s((int)(Value), Buffer, 10);
	DrawText(0,   30, "Maximale FPS :", 0xFFFFFFFF);
	DrawText(150, 30, Buffer, 0xFFFFFFFF);

	// Minimale FPS
	Value = pTimer->getMinFrameRate();
	_itoa_s((int)(Value), Buffer, 10);
	DrawText(0,   45, "Minimale FPS :", 0xFFFFFFFF);
	DrawText(150, 45, Buffer, 0xFFFFFFFF);
}
