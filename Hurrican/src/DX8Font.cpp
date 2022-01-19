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
#include <string>
#include "DX8Texture.hpp"
#include "Globals.hpp"
#include "Logdatei.hpp"
#include "Timer.hpp"
#if defined(PLATFORM_SDL)
#include "SDLPort/texture.hpp"
#endif  // PLATFORM_SDL
#include "DX8Font.hpp"
#include "DX8Graphics.hpp"
#include "Gameplay.hpp"

#ifdef USE_UNRARLIB
#include "unrarlib.h"
#endif

#include <string>

// DKS - Added these includes temporarily so LoadFont() could output character widths to
//      text files. The text was imported here to initialize the charwidths arrays below.
//      I've left them here in case fonts ever need to be altered and new width lists be made.
//#include <iostream>
//#include <fstream>
//#include <sstream>

// --------------------------------------------------------------------------------------
// Konstruktor (leer)
// --------------------------------------------------------------------------------------
uint8_t demofont_charwidths[256] = {
    10, 10, 12, 14, 14, 12, 2,  8,  6,  10, 10, 4,  6,  2,  12, 14, 8,  14, 14, 14, 14, 14, 16, 14, 14, 1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  16, 16, 16, 16, 16, 16, 14, 14, 6,  14, 14, 14, 14, 14, 14, 16, 14, 16, 14, 14, 14, 14, 14,
    14, 14, 16, 1,  1,  1,  1,  1,  1,  16, 16, 16, 16, 16, 16, 14, 14, 6,  14, 14, 14, 14, 14, 14, 16, 14, 16, 14, 14,
    14, 14, 14, 14, 14, 16, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1};
uint8_t menufont_charwidths[256] = {
    4,  8,  28, 19, 26, 23, 3,  6,  7,  28, 16, 4, 8,  3,  19, 18, 11, 17, 18, 19, 17, 18, 17, 18, 17, 3,  4,  15, 16,
    15, 15, 19, 21, 18, 18, 20, 15, 14, 19, 18, 3, 15, 19, 15, 25, 20, 20, 18, 22, 18, 18, 18, 18, 19, 28, 20, 18, 18,
    1,  1,  1,  1,  16, 1,  1,  1,  1,  1,  1,  1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1,  1,  1,  1,  1,  1,  21, 21, 1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  20, 1,  1,  1,  1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1};
uint8_t smallfont_charwidths[256] = {
    4, 5, 8, 7, 10, 10, 3, 5,  5,  7, 7, 4,  5,  3, 5, 7,  5, 7,  7, 7, 7, 7, 7,  7, 7, 3, 4, 7, 7, 7, 6, 9,
    8, 8, 8, 9, 8,  8,  9, 8,  5,  7, 8, 8,  10, 9, 9, 8,  9, 8,  7, 9, 8, 9, 10, 9, 9, 8, 5, 5, 5, 7, 8, 3,
    8, 7, 7, 7, 7,  7,  7, 7,  3,  6, 7, 4,  9,  7, 7, 7,  7, 6,  7, 5, 7, 7, 9,  7, 7, 7, 5, 3, 5, 6, 8, 8,
    7, 7, 8, 8, 8,  8,  7, 7,  7,  7, 5, 5,  4,  8, 7, 8,  9, 10, 7, 7, 7, 7, 7,  9, 8, 7, 8, 9, 7, 8, 8, 4,
    7, 7, 7, 9, 1,  5,  6, 10, 1,  7, 7, 3,  1,  1, 1, 1,  1, 1,  1, 8, 7, 8, 1,  1, 1, 1, 1, 1, 1, 8, 1, 1,
    1, 1, 1, 8, 8,  8,  1, 1,  10, 7, 1, 10, 1,  1, 1, 10, 8, 8,  8, 1, 1, 9, 1,  1, 1, 1, 1, 8, 1, 1, 7, 1,
    1, 1, 1, 8, 1,  1,  1, 1,  1,  1, 1, 1,  1,  1, 1, 1,  1, 1,  1, 1, 1, 7, 1,  1, 1, 1, 1, 7, 1, 1, 1, 1,
    1, 1, 1, 1, 1,  1,  1, 1,  1,  1, 1, 1,  1,  1, 1, 1,  1, 1,  1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1};

DirectGraphicsFont::DirectGraphicsFont() {
    // DKS - Added support for font scaling
    mScaleFactor = 1;

    mTexture = nullptr;

    // DKS - Character widths are now hard-coded arrays we merely point to:
    //// alle mit 0 initialiseren, falls ein fehlerhaftest Zeichen verwendet wird
    // for (int i = 0; i < 256; i++)
    //    mCharLength[i] = 1;
    mCharLength = nullptr;
}

// --------------------------------------------------------------------------------------
// Desktruktor gibt die Textur des Fonts wieder frei
// --------------------------------------------------------------------------------------

DirectGraphicsFont::~DirectGraphicsFont() {
    // Font Textur freigeben
    delete (mTexture);
    mTexture = nullptr;
}

// --------------------------------------------------------------------------------------
// Laden des Bildes "Filename" mit
// x-Grösse eines Zeichens und
// y-Grösse eines Zeichens und
// Anzahl der Zeichen pro Zeile
// --------------------------------------------------------------------------------------

bool DirectGraphicsFont::LoadFont(const char *Filename,
                                  int xts,
                                  int yts,
                                  int xCharsize,
                                  int yCharsize,
                                  int xChars,
                                  int yChars,
                                  uint8_t *charwidths)

{
    mTexture = new (DirectGraphicsSprite);

    // DKS - Why is last parameter 0? That would cause problems in memory allocation:
    // if (!mTexture->LoadImage(Filename, xts, yts, xCharsize, yCharsize, xChars, 0))
    if (!mTexture->LoadImage(Filename, xts, yts, xCharsize, yCharsize, xChars, yChars)) {
        Protokoll << "Error loading font: " << Filename << std::endl;
        GameRunning = false;
        return false;
    } else {
        Protokoll << "Loading font: " << Filename << std::endl;
    }

    // Grösse setzen
    //
    mXCharSize = xCharsize;
    mYCharSize = yCharsize;
    mXChars = xChars;

    // DKS - We no longer read the font directly, as character widths are now defined in hard-coded arrays,
    //      so most of the rest of the function is now disabled and mCharLength just points to one
    //      of the three new hard-coded charwidths arrays defined at the top of this file.
    mCharLength = charwidths;

#if 0
    // Länge der einzelnen Zeichen aus der Grafik bestimmen
    //

    // Geladene Font Textur locken
    //
    image_t image;
    std::string fullpath;
    char *pData;
    unsigned long Size;

    if (CommandLineParams.RunOwnLevelList == true)
    {
        //sprintf_s(Temp, "%s/levels/%s/%s", g_storage_ext, CommandLineParams.OwnLevelList, Filename);
        fullpath = g_storage_ext + "/levels/" + std::string(CommandLineParams.OwnLevelList) + std::string(Filename);
        if (FileExists(fullpath.c_str()))
        {
            if (!loadImageSDL(image, fullpath, nullptr, 0)) {
                delete [] image.data;
                Protokoll << "Error in LoadFont(): loadImageSDL() returned error loading " << fullpath << std::endl;
                GameRunning = false;
                return false;
            }
        }
    }

    if (image.data == nullptr)
    {
        //DKS - All textures are now stored in their own data/textures/ subdir:
        //sprintf_s(Temp, "%s/data/textures/%s", g_storage_ext, Filename);
        fullpath = g_storage_ext + "/data/textures/" + std::string(Filename);
        if (FileExists(fullpath.c_str()))
        {
            if (!loadImageSDL(image, fullpath, nullptr, 0)) {
                delete [] image.data;
                Protokoll << "Error in LoadFont(): loadImageSDL() returned error loading " << fullpath << std::endl;
                GameRunning = false;
                return false;
            }
        }
    }

#if defined(USE_UNRARLIB)
    if (image.data == nullptr)
    {
        if (urarlib_get(&pData, &Size, Filename, RARFILENAME, convertText(RARFILEPASSWORD)) != false)
        {
            if (!loadImageSDL(image, nullptr, pData, Size))
                delete [] image.data;
                free(pData);
                Protokoll << "Error in LoadFont(): loadImageSDL() returned error loading " << Filename << " from buffer" << std::endl;
                GameRunning = false;
                return false;
            }
            free(pData);
        }
    }
#endif  // USE_UNRARLIB

    if (image.data  == nullptr) {
        Protokoll << "Error in LoadFont(): image.data is NULL" << std::endl;
        GameRunning = false;
        return false;
    }

    // Colorkey feststellen
    //std::uint32_t key = ((std::uint32_t*)d3dlr.pBits)[0];
#if 1
    /* menufont.png: pixel at (0,0) (upper left corner) is part of a char/glyph,
       so pick key from lower left corner. Maybe in DirectX image is flipped
       vertically? */

    //DKS - All textures are now using a proper alpha channel, and shouldn't have any
    //      color information where their alpha is 0, so the key can now just be 0:
    //std::uint32_t key = (((std::uint32_t*)image.data )[image.w * (image.h - 1)]);
    std::uint32_t key = 0;
#else
    std::uint32_t key = ((std::uint32_t*)image.data )[0];
#endif

    /* // PICKLE Not OpenGLES compat, left for info
        int textureWidth, textureHeight;
        //DKS - Note: I did not bother updating this for the new TexturesystemClass
        glBindTexture( GL_TEXTURE_2D, mTexture->itsTexture );
        glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &textureWidth );
        glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &textureHeight );

        GLubyte *buffer = (GLubyte *)malloc(textureWidth*textureHeight*4);
        glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
        std::uint32_t key = ((std::uint32_t*)buffer)[0];

        glBindTexture( GL_TEXTURE_2D, 0 );
    */
    //key = 0;

    // Einzelne Zeichen durchgehen
    for (int i=0; i < xChars; i++)
        for (int j=0; j < yChars; j++) {
            int last  = 0;
            bool found = false;

            for (int k = 0; k<xCharsize; k++) {
                found = false;

                for (int l = 0; l<yCharsize; l++) {
                    if (((std::uint32_t*)image.data)[(j * yCharsize + l) * image.w + (i*xCharsize + k)] != key)
                        found = true;
                    /*
                    				if (((std::uint32_t*)buffer)[(j * yCharsize + l) * textureWidth + (i*xCharsize + k)] != key)
                    					found = true;
                    */
                }

                if (found == true)
                    last = k;
            }

            mCharLength[j * xChars + i] = last+1;
        }

    //DKS - I used this code to output the character widths to a text file, and they now reside in
    //      hard-coded arrays. This was so we can use compressed font images and not have to always
    //      have a .PNG image present.
    //std::string outfile_str = std::string(Filename) + ".txt";
    //std::ofstream outfile(outfile_str.c_str());
    //for (int i=0; i < 256; ++i) {
    //    std::ostringstream ostr;
    //    ostr << (int)mCharLength[i];
    //    outfile << ostr.str() << ", ";
    //}
    //outfile.close();


    delete [] image.data;
#endif  // 0

    return true;
}

#if 0
// --------------------------------------------------------------------------------------
// Zahlenwert an xPos,yPos mit Farbe Color ausgeben
// --------------------------------------------------------------------------------------

// DKS - Note: This function does not appear to actually be used anywhere:
bool DirectGraphicsFont::DrawValue(float x, float y, float Value, D3DCOLOR Color) {
    std::string Buf = std::to_string(Value);
    DrawText(x, y, Buf.c_str(), Color);

    return true;
}
#endif

// --------------------------------------------------------------------------------------
// Text an xPos,yPos mit Farbe Color ausgeben
// --------------------------------------------------------------------------------------

bool DirectGraphicsFont::DrawText(float x, float y, const char Text[], D3DCOLOR Color) {
    unsigned int const len = strlen(Text);

    // DKS - Added:
    if (!Text || len == 0)
        return true;

    float const oldx = x;
    for (unsigned int i = 0; i < len; i++) {
        unsigned char const ch = Text[i];  // Aktuell zu bearbeitendes Zeichen holen
        unsigned char const z = ch - 33;   // "!" als erstes Zeichen setzen, das heisst,
        // Fontgrafik muss mit "!" beginnen
        // Position des aktuellen Zeichens in der Grafik berechnen
        RECT_struct rect;
        rect.left = (z % mXChars) * mXCharSize;
        rect.top = (z / mXChars) * mYCharSize;
        rect.right = rect.left + mXCharSize;
        rect.bottom = rect.top + mYCharSize;

        mTexture->SetRect(rect.left, rect.top, rect.right, rect.bottom);

        if (ch != 32 && ch != '\n') {
            // DKS - Added support for font scaling
            if (mScaleFactor == 1) {
                mTexture->RenderSprite(x, y, Color);
            } else {
                mTexture->RenderSpriteScaled(x, y, mXCharSize * mScaleFactor - 1, mYCharSize * mScaleFactor - 1, Color);
            }
        }

        if (ch == 32)
            // DKS - Added support for font scaling
            if (mScaleFactor == 1) {
                x += static_cast<float>(mXCharSize - 3);  // Bei Space frei lassen
            } else {
                x += static_cast<float>((mXCharSize - 3) * mScaleFactor);  // Bei Space frei lassen
            }
        else if (ch == '\n')  // Zeilenumbruch
        {
            x = oldx;
            // DKS - Added support for font scaling
            if (mScaleFactor == 1) {
                y += static_cast<float>(mYCharSize + 6);
            } else {
                y += static_cast<float>(mYCharSize * mScaleFactor + 6);
            }
        } else {
            // DKS - Added support for font scaling
            if (mScaleFactor == 1) {
                x += static_cast<float>(mCharLength[z] - 1);  // Ansonsten Breite des Zeichens weiter
            } else {
                x += static_cast<float>((mCharLength[z] - 1) * mScaleFactor);  // Ansonsten Breite des Zeichens weiter
            }
        }
    }

    return true;
}

void DirectGraphicsFont::DrawTextRightAlign(float x, float y, const char Text[], D3DCOLOR Color, int Spacing) {
    DrawText(x - StringLength(Text, Spacing), y, Text, Color, Spacing);
}

void DirectGraphicsFont::DrawTextCenterAlign(float x, float y, const char Text[], D3DCOLOR Color, int Spacing) {
    DrawText(x - StringLength(Text, Spacing) / 2, y, Text, Color, Spacing);
}

// --------------------------------------------------------------------------------------
// Zeichen an xPos,yPos mit Farbe Color ausgeben (Demo Font, mit mehr Platz zwischen Zeichen)
// --------------------------------------------------------------------------------------

bool DirectGraphicsFont::DrawDemoChar(float x, float y, const char Text, D3DCOLOR Color) {
    if (Text == 32 || Text == '\n')
        return false;

    unsigned char const z = Text - 33;  // Aktuell zu bearbeitendes Zeichen holen
                                        // "!" als erstes Zeichen setzen, das heisst,
    // Fontgrafik muss mit "!" beginnen
    // Position des aktuellen Zeichens in der Grafik berechnen
    RECT_struct rect;
    rect.left = (z % mXChars) * mXCharSize;
    rect.top = (z / mXChars) * mYCharSize;
    rect.right = rect.left + mXCharSize;
    rect.bottom = rect.top + mYCharSize;

    mTexture->SetRect(rect.left, rect.top, rect.right, rect.bottom);
    mTexture->RenderSprite(x, y, Color);

    return true;
}

// --------------------------------------------------------------------------------------
// Text an xPos,yPos mit Farbe Color ausgeben (Demo Font, mit mehr Platz zwischen Zeichen)
// --------------------------------------------------------------------------------------

bool DirectGraphicsFont::DrawDemoText(float x, float y, const char Text[], D3DCOLOR Color) {
    float const oldx = x;
    unsigned int const len = strlen(Text);

    for (unsigned int i = 0; i < len; i++) {
        unsigned char const ch = Text[i];  // Aktuell zu bearbeitendes Zeichen holen
        unsigned char const z = ch - 33;   // "!" als erstes Zeichen setzen, das heisst,
        // Fontgrafik muss mit "!" beginnen
        // Position des aktuellen Zeichens in der Grafik berechnen
        RECT_struct rect;
        rect.left = (z % mXChars) * mXCharSize;
        rect.top = (z / mXChars) * mYCharSize;
        rect.right = rect.left + mXCharSize;
        rect.bottom = rect.top + mYCharSize;

        mTexture->SetRect(rect.left, rect.top, rect.right, rect.bottom);

        if (ch != 32 && ch != '\n')
            mTexture->RenderSprite(x, y, Color);

        if (ch == 32)
            x += mXCharSize;       // Bei Space frei lassen
        else if (ch == '\n')  // Zeilenumbruch
        {
            x = oldx;
            y += mYCharSize + 6;
        } else
            x += mCharLength[z] + 1;  // Ansonsten Breite des Zeichens weiter
    }

    return true;
}

// --------------------------------------------------------------------------------------
// Text an xPos,yPos mit Farbe Color ausgeben, aber "Spacing" Pixel Platz dazwischen lassen
// --------------------------------------------------------------------------------------

bool DirectGraphicsFont::DrawText(float x, float y, const char Text[], D3DCOLOR Color, int Spacing) {
    float const oldx = x;
    unsigned int const len = strlen(Text);

    for (unsigned int i = 0; i < len; i++) {
        unsigned char const ch = Text[i];  // Aktuell zu bearbeitendes Zeichen holen
        unsigned char const z = ch - 33;   // "!" als erstes Zeichen setzen, das heisst,
        // Fontgrafik muss mit "!" beginnen
        // Position des aktuellen Zeichens in der Grafik berechnen
        RECT_struct rect;
        rect.left = (z % mXChars) * mXCharSize;
        rect.top = (z / mXChars) * mYCharSize;
        rect.right = rect.left + mXCharSize;
        rect.bottom = rect.top + mYCharSize;

        mTexture->SetRect(rect.left, rect.top, rect.right, rect.bottom);

        if (ch != 32 && ch != '\n') {
            // DKS - Added support for font scaling
            if (mScaleFactor == 1) {
                mTexture->RenderSprite(x, y, Color);
            } else {
                mTexture->RenderSpriteScaled(x, y, mXCharSize * mScaleFactor - 1, mYCharSize * mScaleFactor - 1, Color);
            }
        }

        if (ch == 32)
            // DKS - Added support for font scaling
            if (mScaleFactor == 1) {
                x += static_cast<float>(mXCharSize - 3 - Spacing * 2);  // Bei Space frei lassen
            } else {
                x += static_cast<float>(((mXCharSize - 3) - Spacing * 2) * mScaleFactor);  // Bei Space frei lassen
            }
        else if (ch == '\n')  // Zeilenumbruch
        {
            x = oldx;
            // DKS - Added support for font scaling
            if (mScaleFactor == 1) {
                y += static_cast<float>(mYCharSize + 6 + Spacing);
            } else {
                y += static_cast<float>((mYCharSize + 6 + Spacing) * mScaleFactor);
            }
        } else
            // DKS - Added support for font scaling
            if (mScaleFactor == 1) {
            x += static_cast<float>(mCharLength[z] - 1 + Spacing);  // Ansonsten Breite des Zeichens weiter
        } else {
            x += static_cast<float>(((mCharLength[z] - 1) + Spacing) * mScaleFactor);  // Ansonsten Breite des Zeichens weiter
        }
    }

    return true;
}

// --------------------------------------------------------------------------------------
// Länge eines Strings in Pixeln zurückliefern
// --------------------------------------------------------------------------------------

int DirectGraphicsFont::DemoStringLength(const char Text[]) {
    unsigned int const len = strlen(Text);

    if (len <= 1)
        return 0;

    int l = 0;
    for (unsigned int i = 0; i < len - 1; i++) {
        unsigned char const ch = Text[i];  // Aktuell zu bearbeitendes Zeichen holen
        unsigned char const z = ch - 33;   // "!" als erstes Zeichen setzen, das heisst,
        // Fontgrafik muss mit "!" beginnen
        if (ch == 32)
            l += mXCharSize - 3;  // Bei Space frei lassen
        else
            l += mCharLength[z];  // Ansonsten Breite des Zeichens weiter
    }

    return l;
}

// --------------------------------------------------------------------------------------
// Länge eines Strings in Pixeln zurückliefern
// --------------------------------------------------------------------------------------

int DirectGraphicsFont::StringLength(const char Text[]) {
    unsigned int const len = strlen(Text);

    if (len < 1)
        return 0;

    int l = 0;
    for (unsigned int i = 0; i < len; i++) {
        unsigned char const ch = Text[i];  // Aktuell zu bearbeitendes Zeichen holen
        unsigned char const z = ch - 33;   // "!" als erstes Zeichen setzen, das heisst,
        // Fontgrafik muss mit "!" beginnen
        // DKS - Added support for font scaling
        if (mScaleFactor == 1) {
            if (ch == 32)
                l += mXCharSize - 3;  // Bei Space frei lassen
            else
                l += mCharLength[z] - 1;  // Ansonsten Breite des Zeichens weiter
        } else {
            if (ch == 32)
                l += (mXCharSize - 3) * mScaleFactor;  // Bei Space frei lassen
            else
                l += (mCharLength[z] - 1) * mScaleFactor;  // Ansonsten Breite des Zeichens weiter
        }
    }

    return l;
}

// --------------------------------------------------------------------------------------
// Länge eines Strings in Pixeln mit "Spacing" Pixeln freiraum zwischen den Zeichen zurückliefern
// --------------------------------------------------------------------------------------

int DirectGraphicsFont::StringLength(const char Text[], int Spacing) {
    unsigned int const len = strlen(Text);

    if (len < 1)
        return 0;

    int l = 0;
    for (unsigned int i = 0; i < len; i++) {
        unsigned char const ch = Text[i];  // Aktuell zu bearbeitendes Zeichen holen
        unsigned char const z = ch - 33;   // "!" als erstes Zeichen setzen, das heisst,
        // Fontgrafik muss mit "!" beginnen
        // DKS - Added support for font scaling
        if (mScaleFactor == 1) {
            if (ch == 32)
                l += mXCharSize - 3 - Spacing * 2;  // Bei Space frei lassen
            else
                l += mCharLength[z] - 1 + Spacing;  // Ansonsten Breite des Zeichens weiter
        } else {
            if (ch == 32)
                l += ((mXCharSize - 3) - Spacing * 2) * mScaleFactor;  // Bei Space frei lassen
            else
                l += ((mCharLength[z] - 1) + Spacing) * mScaleFactor;  // Ansonsten Breite des Zeichens weiter
        }
    }

    return l;
}

// --------------------------------------------------------------------------------------
// FPS Werte auf dem Screen anzeigen
// --------------------------------------------------------------------------------------

void DirectGraphicsFont::ShowFPS() {
    static int updateFPS;  // Trigger für die FPS, da sonst Anzeige zu schnell
    static double FPS;
    std::string Buffer;

    updateFPS++;
    if (updateFPS > FPS / 2) {
        updateFPS = 0;
        FPS = Timer.getFrameRate();
    }

    // Aktuelle FPS
    Buffer = std::to_string(FPS);
    DrawText(0, 0, "Current FPS :", 0xFFFFFFFF);
    DrawText(100, 0, Buffer.c_str(), 0xFFFFFFFF);

    // FPS Grenze
    Buffer = std::to_string(Timer.GetMaxFPS());
    DrawText(200, 0, "Max FPS :", 0xFFFFFFFF);
    DrawText(300, 0, Buffer.c_str(), 0xFFFFFFFF);

    // Durchschnittliche FPS
    Buffer = std::to_string(Timer.getAverageFPS());
    DrawText(0, 15, "Average FPS :", 0xFFFFFFFF);
    DrawText(100, 15, Buffer.c_str(), 0xFFFFFFFF);

    // Maximale FPS
    Buffer = std::to_string(Timer.getMaxFrameRate());
    DrawText(0, 30, "Highest FPS :", 0xFFFFFFFF);
    DrawText(100, 30, Buffer.c_str(), 0xFFFFFFFF);

    // Minimale FPS
    Buffer = std::to_string(Timer.getMinFrameRate());
    DrawText(0, 45, "Lowest FPS :", 0xFFFFFFFF);
    DrawText(100, 45, Buffer.c_str(), 0xFFFFFFFF);
}

// DKS - New functions added to facilitate resized fonts:
int DirectGraphicsFont::GetXCharSize() const {
    return mXCharSize * mScaleFactor;
}

int DirectGraphicsFont::GetYCharSize() const {
    return mYCharSize * mScaleFactor;
}

// DKS - Added support for font scaling
int DirectGraphicsFont::GetScaleFactor() const {
    return mScaleFactor;
}

// DKS - Added support for font scaling
void DirectGraphicsFont::SetScaleFactor(int scale_factor) {
    mScaleFactor = scale_factor;
}
