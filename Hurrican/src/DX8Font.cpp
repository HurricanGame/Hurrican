// Datei : DX8Font.h

// --------------------------------------------------------------------------------------
//
// Font Klasse
// zum Anzeigen von Schriften
//
// (c) 2002 J�rg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include-Dateien
// --------------------------------------------------------------------------------------

#include <stdio.h>
#include <string>
#include "Globals.h"
#include "Logdatei.h"
#include "Timer.h"
#include "DX8Texture.h"
#if defined(PLATFORM_SDL)
#include "SDLPort/texture.h"
#endif //PLATFORM_SDL
#include "DX8Font.h"
#include "DX8Graphics.h"
#include "Gameplay.h"
#include "unrarlib.h"

#include <string>

//DKS - Added these includes temporarily so LoadFont() could output character widths to
//      text files. The text was imported here to initialize the charwidths arrays below.
//      I've left them here in case fonts ever need to be altered and new width lists be made.
//#include <iostream>
//#include <fstream>
//#include <sstream>

using namespace std;

// --------------------------------------------------------------------------------------
// Konstruktor (leer)
// --------------------------------------------------------------------------------------
uint8_t demofont_charwidths[256] = { 10, 10, 12, 14, 14, 12, 2, 8, 6, 10, 10, 4, 6, 2, 12, 14, 8, 14, 14, 14, 14, 14, 16, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 16, 16, 16, 16, 16, 16, 14, 14, 6, 14, 14, 14, 14, 14, 14, 16, 14, 16, 14, 14, 14, 14, 14, 14, 14, 16, 1, 1, 1, 1, 1, 1, 16, 16, 16, 16, 16, 16, 14, 14, 6, 14, 14, 14, 14, 14, 14, 16, 14, 16, 14, 14, 14, 14, 14, 14, 14, 16, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
uint8_t menufont_charwidths[256] = { 4, 8, 28, 19, 26, 23, 3, 6, 7, 28, 16, 4, 8, 3, 19, 18, 11, 17, 18, 19, 17, 18, 17, 18, 17, 3, 4, 15, 16, 15, 15, 19, 21, 18, 18, 20, 15, 14, 19, 18, 3, 15, 19, 15, 25, 20, 20, 18, 22, 18, 18, 18, 18, 19, 28, 20, 18, 18, 1, 1, 1, 1, 16, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 21, 21, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
uint8_t smallfont_charwidths[256] = { 4, 5, 8, 7, 10, 10, 3, 5, 5, 7, 7, 4, 5, 3, 5, 7, 5, 7, 7, 7, 7, 7, 7, 7, 7, 3, 4, 7, 7, 7, 6, 9, 8, 8, 8, 9, 8, 8, 9, 8, 5, 7, 8, 8, 10, 9, 9, 8, 9, 8, 7, 9, 8, 9, 10, 9, 9, 8, 5, 5, 5, 7, 8, 3, 8, 7, 7, 7, 7, 7, 7, 7, 3, 6, 7, 4, 9, 7, 7, 7, 7, 6, 7, 5, 7, 7, 9, 7, 7, 7, 5, 3, 5, 6, 8, 8, 7, 7, 8, 8, 8, 8, 7, 7, 7, 7, 5, 5, 4, 8, 7, 8, 9, 10, 7, 7, 7, 7, 7, 9, 8, 7, 8, 9, 7, 8, 8, 4, 7, 7, 7, 9, 1, 5, 6, 10, 1, 7, 7, 3, 1, 1, 1, 1, 1, 1, 1, 8, 7, 8, 1, 1, 1, 1, 1, 1, 1, 8, 1, 1, 1, 1, 1, 8, 8, 8, 1, 1, 10, 7, 1, 10, 1, 1, 1, 10, 8, 8, 8, 1, 1, 9, 1, 1, 1, 1, 1, 8, 1, 1, 7, 1, 1, 1, 1, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7, 1, 1, 1, 1, 1, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

DirectGraphicsFont::DirectGraphicsFont(void)
{
    //DKS - Added support for font scaling
    mScaleFactor = 1;

    mTexture = NULL;

    //DKS - Character widths are now hard-coded arrays we merely point to:
    //// alle mit 0 initialiseren, falls ein fehlerhaftest Zeichen verwendet wird
    //for (int i = 0; i < 256; i++)
    //    mCharLength[i] = 1;
    mCharLength = NULL;
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
// x-Gr�sse eines Zeichens und
// y-Gr�sse eines Zeichens und
// Anzahl der Zeichen pro Zeile
// --------------------------------------------------------------------------------------

bool DirectGraphicsFont::LoadFont(const char *Filename, int xts, int yts,
                                  int xCharsize, int yCharsize, int xChars, int yChars,
                                  uint8_t *charwidths)
                                  
{
    mTexture = new (DirectGraphicsSprite);

    //DKS - Why is last parameter 0? That would cause problems in memory allocation:
    //if (!mTexture->LoadImage(Filename, xts, yts, xCharsize, yCharsize, xChars, 0))
    if (!mTexture->LoadImage(Filename, xts, yts, xCharsize, yCharsize, xChars, yChars)) {
        Protokoll.WriteText(true, "Error loading font: %s\n", Filename);
        return false;
    } else {
        Protokoll.WriteText(false, "Loading font: %s\n", Filename);
    }

    // Gr�sse setzen
    //
    mXCharSize		= xCharsize;
    mYCharSize		= yCharsize;
    mXChars			= xChars;

    //DKS - We no longer read the font directly, as character widths are now defined in hard-coded arrays,
    //      so most of the rest of the function is now disabled and mCharLength just points to one
    //      of the three new hard-coded charwidths arrays defined at the top of this file.
    mCharLength = charwidths;

#if 0
    // L�nge der einzelnen Zeichen aus der Grafik bestimmen
    //

    // Geladene Font Textur locken
    //
#if defined(PLATFORM_DIRECTX)
    D3DSURFACE_DESC d3dsd;
    D3DLOCKED_RECT  d3dlr;
    HRESULT hresult;

    //DKS - Adapted to new TexturesystemClass
    //mTexture->itsTexture->GetLevelDesc(0, &d3dsd);
    //hresult = mTexture->itsTexture->LockRect    (0, &d3dlr, 0, 0 );
    TextureHandle &th = Textures[mTexture->itsTexIdx];
    th.tex->GetLevelDesc(0, &d3dsd);

    // Fehler beim Locken ?
    if (hresult != D3D_OK)
        Protokoll.WriteText(true, "error locking font texture!");

#elif defined(PLATFORM_SDL)
    image_t image;
    string fullpath;
    char *pData;
    unsigned long Size;

    if (CommandLineParams.RunOwnLevelList == true)
    {
        //sprintf_s(Temp, "%s/levels/%s/%s", g_storage_ext, CommandLineParams.OwnLevelList, Filename);
        fullpath = string(g_storage_ext) + "/levels/" + string(CommandLineParams.OwnLevelList) + string(Filename);
        if (FileExists(fullpath.c_str()))
        {
            if (!loadImageSDL(image, fullpath, NULL, 0)) {
                delete [] image.data;
                Protokoll.WriteText(true, "Error in LoadFont(): loadImageSDL() returned error loading %s\n", fullpath.c_str() );
                return false;
            }
        }
    }

    if (image.data == NULL)
    {
        //DKS - All textures are now stored in their own data/textures/ subdir:
        //sprintf_s(Temp, "%s/data/textures/%s", g_storage_ext, Filename);
        fullpath = string(g_storage_ext) + "/data/textures/" + string(Filename);
        if (FileExists(fullpath.c_str()))
        {
            if (!loadImageSDL(image, fullpath, NULL, 0)) {
                delete [] image.data;
                Protokoll.WriteText(true, "Error in LoadFont(): loadImageSDL() returned error loading %s\n", fullpath.c_str() );
                return false;
            }
        }
    }

#if defined(USE_UNRARLIB)
    if (image.data == NULL)
    {
        if (urarlib_get(&pData, &Size, Filename, RARFILENAME, convertText(RARFILEPASSWORD)) != false)
        {
            if (!loadImageSDL(image, NULL, pData, Size))
                delete [] image.data;
                free(pData);
                Protokoll.WriteText(true, "Error in LoadFont(): loadImageSDL() returned error loading %s from buffer\n", Filename );
                return false;
            }
            free(pData);
        }
    }
#endif // USE_UNRARLIB

    if (image.data  == NULL) {
        Protokoll.WriteText(true, "Error in LoadFont(): image.data is NULL\n" );
        return false;
    }

    // Colorkey feststellen
    //DWORD key = ((DWORD*)d3dlr.pBits)[0];
#if 1
    /* menufont.png: pixel at (0,0) (upper left corner) is part of a char/glyph,
       so pick key from lower left corner. Maybe in DirectX image is flipped
       vertically? */

    //DKS - All textures are now using a proper alpha channel, and shouldn't have any
    //      color information where their alpha is 0, so the key can now just be 0:
    //DWORD key = (((DWORD*)image.data )[image.w * (image.h - 1)]);
    DWORD key = 0;
#else
    DWORD key = ((DWORD*)image.data )[0];
#endif

    /* // PICKLE Not OpenGLES compat, left for info
        int textureWidth, textureHeight;
        //DKS - Note: I did not bother updating this for the new TexturesystemClass
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

    //DKS - I used this code to output the character widths to a text file, and they now reside in
    //      hard-coded arrays. This was so we can use compressed font images and not have to always
    //      have a .PNG image present.
    //string outfile_str = string(Filename) + ".txt";
    //ofstream outfile(outfile_str.c_str());
    //for (int i=0; i < 256; ++i) {
    //    ostringstream ostr;
    //    ostr << (int)mCharLength[i];
    //    outfile << ostr.str() << ", ";
    //}
    //outfile.close();

#if defined(PLATFORM_DIRECTX)
    // Unlocken
    th.tex->UnlockRect(0);  //DKS - Adapted to new TexturesystemClass
#elif defined(PLATFORM_SDL)
    delete [] image.data;
#endif
#endif //0

    return true;
}

// --------------------------------------------------------------------------------------
// Zahlenwert an xPos,yPos mit Farbe Color ausgeben
// --------------------------------------------------------------------------------------

//DKS - Note: This function does not appear to actually be used anywhere:
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

    //DKS - Added:
    if (!Text || strlen(Text) == 0)
        return true;

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

        if (Text[i] != 32 && Text[i] != '\n') {
            //DKS - Added support for font scaling
            if (mScaleFactor == 1) {
                mTexture->RenderSprite(x, y, Color);
            } else {
                mTexture->RenderSpriteScaled(x, y, mXCharSize * mScaleFactor - 1, mYCharSize * mScaleFactor - 1, Color);
            }
        }

        if(Text[i] == 32)
            //DKS - Added support for font scaling
            if (mScaleFactor == 1) {
                x += float(mXCharSize-3);				// Bei Space frei lassen
            } else {
                x += float((mXCharSize - 3) * mScaleFactor);				// Bei Space frei lassen
            }
        else if(Text[i] == '\n')					// Zeilenumbruch
        {
            x = oldx;
            //DKS - Added support for font scaling
            if (mScaleFactor == 1) {
                y += float(mYCharSize + 6);
            } else {
                y += float(mYCharSize * mScaleFactor + 6);
            }
        }
        else {
            //DKS - Added support for font scaling
            if (mScaleFactor == 1) {
                x += float(mCharLength[z]-1);			// Ansonsten Breite des Zeichens weiter
            } else {
                x += float((mCharLength[z]-1) * mScaleFactor);			// Ansonsten Breite des Zeichens weiter
            }
        }
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
        else if(Text[i] == '\n')					// Zeilenumbruch
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

        if (Text[i] != 32 && Text[i] != '\n') {
            //DKS - Added support for font scaling
            if (mScaleFactor == 1) {
                mTexture->RenderSprite(x, y, Color);
            } else {
                mTexture->RenderSpriteScaled(x, y, mXCharSize * mScaleFactor - 1, mYCharSize * mScaleFactor - 1, Color);
            }
        }

        if(Text[i] == 32)
            //DKS - Added support for font scaling
            if (mScaleFactor == 1) {
                x += float(mXCharSize-3 - Spacing*2);				// Bei Space frei lassen
            } else {
                x += float(((mXCharSize - 3) - Spacing * 2) * mScaleFactor );				// Bei Space frei lassen
            }
        else if(Text[i] == '\n')								// Zeilenumbruch
        {
            x = oldx;
            //DKS - Added support for font scaling
            if (mScaleFactor == 1) {
                y += float(mYCharSize + 6 + Spacing);
            } else {
                y += float((mYCharSize + 6 + Spacing) * mScaleFactor);
            }
        }
        else
            //DKS - Added support for font scaling
            if (mScaleFactor == 1) {
                x += float(mCharLength[z] - 1 + Spacing);	// Ansonsten Breite des Zeichens weiter
            } else {
                x += float(((mCharLength[z] - 1) + Spacing) * mScaleFactor);	// Ansonsten Breite des Zeichens weiter
            }
    }

    return true;
}

// --------------------------------------------------------------------------------------
// L�nge eines Strings in Pixeln zur�ckliefern
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
// L�nge eines Strings in Pixeln zur�ckliefern
// --------------------------------------------------------------------------------------

int DirectGraphicsFont::StringLength(const char Text[])
{
    if (strlen(Text) < 1)
        return 0;

    unsigned char z;
    int  l;

    l = 0;
    unsigned int length = strlen(Text);
    for(unsigned int i=0; i<length; i++)
    {
        z = Text[i];						// Aktuell zu bearbeitendes Zeichen holen
        z -=33;								// "!" als erstes Zeichen setzen, das heisst,
        // Fontgrafik muss mit "!" beginnen
        //DKS - Added support for font scaling
        if (mScaleFactor == 1) {
            if(Text[i] == 32)
                l += mXCharSize-3;				// Bei Space frei lassen
            else
                l += mCharLength[z]-1;			// Ansonsten Breite des Zeichens weiter
        } else {
            if(Text[i] == 32)
                l += (mXCharSize-3) * mScaleFactor;				// Bei Space frei lassen
            else
                l += (mCharLength[z]-1) * mScaleFactor;			// Ansonsten Breite des Zeichens weiter
        }
    }

    return l;
}

// --------------------------------------------------------------------------------------
// L�nge eines Strings in Pixeln mit "Spacing" Pixeln freiraum zwischen den Zeichen zur�ckliefern
// --------------------------------------------------------------------------------------

int DirectGraphicsFont::StringLength(const char Text[], int Spacing)
{
    if (strlen(Text) < 1)
        return 0;

    unsigned char z;
    int  l;

    l = 0;
    unsigned int length = strlen(Text);
    for(unsigned int i=0; i<length; i++)
    {
        z = Text[i];						// Aktuell zu bearbeitendes Zeichen holen
        z -=33;								// "!" als erstes Zeichen setzen, das heisst,
        // Fontgrafik muss mit "!" beginnen
        //DKS - Added support for font scaling
        if (mScaleFactor == 1) {
            if(Text[i] == 32)
                l += mXCharSize-3 - Spacing*2;	// Bei Space frei lassen
            else
                l += mCharLength[z]-1+ Spacing;	// Ansonsten Breite des Zeichens weiter
        } else {
            if(Text[i] == 32)
                l += ((mXCharSize-3) - Spacing*2) * mScaleFactor;	// Bei Space frei lassen
            else
                l += ((mCharLength[z]-1) + Spacing) * mScaleFactor;	// Ansonsten Breite des Zeichens weiter
        }
    }

    return l;
}


// --------------------------------------------------------------------------------------
// FPS Werte auf dem Screen anzeigen
// --------------------------------------------------------------------------------------

void DirectGraphicsFont::ShowFPS (void)
{
    static int    updateFPS;					// Trigger f�r die FPS, da sonst Anzeige zu schnell
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

//DKS - New functions added to facilitate resized fonts:
int DirectGraphicsFont::GetYCharSize(void)
{
    return mYCharSize * mScaleFactor;
}

//DKS - Added support for font scaling
int DirectGraphicsFont::GetScaleFactor(void)
{
    return mScaleFactor;
}

//DKS - Added support for font scaling
void DirectGraphicsFont::SetScaleFactor(int scale_factor)
{
    mScaleFactor = scale_factor;
}
