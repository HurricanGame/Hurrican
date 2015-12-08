//DKS - DX8Texture.cpp is a new file I have added.
//      It contains a new texture system class that allows sprites to share textures,
//      where before the game was loading duplicate copies in places. It also
//      abstracts away the details like the alpha textures needed by ETC1 compression.
//      It also unifies the interface that was once different between DX8 and SDL.

#if defined(PLATFORM_SDL)
#include "SDLPort/SDL_port.h"
#endif

#include "DX8Texture.h"

#if defined(PLATFORM_SDL)
#include "SDLPort/texture.h"
#endif

#include "Main.h"
#include "Gameplay.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#ifdef USE_UNRARLIB
#include "unrarlib.h"
#endif

const std::string TexturesystemClass::scalefactors_filename("scalefactors.txt");

// --------------------------------------------------------------------------------------
// TexturesystemClass functions
// --------------------------------------------------------------------------------------

void TexturesystemClass::Exit()
{
    // If there are any remaining textures in memory that have not already been
    // unloaded through DirectGraphicsSprite destructors, mop them up now:
    //   A good example would be textures that are member sprites of
    // global static objects, whose order of destruction can't be guaranteed,
    // such as PlayerClass.

    for (unsigned int i=0; i < _loaded_textures.size(); ++i) {
        // Force unloading of the texture if it's still loaded, by making its
        //  instances equal 1 and calling UnloadTexture():
        if (_loaded_textures[i].instances > 0) {
            _loaded_textures[i].instances = 1;
            UnloadTexture(i);
        }
    }
}


//DKS - This largely replaces and expands on a lot of the code
//      DirectGraphicsSprite::LoadImage used to handle.
int16_t TexturesystemClass::LoadTexture( const std::string &filename )
{
    if (filename.empty())
        return -1;

    int16_t idx = -1;

    // Check to see if the texture was previously loaded at some point
    std::map<std::string, int16_t>::iterator it = _texture_map.find(filename);
    if (it != _texture_map.end()) {
        // Texture has been loaded previously, so it at least has a slot in _loaded_textures,
        //  but if its instances == 0, it will need to be re-loaded from disk.
        idx = (*it).second;
#ifdef _DEBUG
        if (idx < 0 || (int)idx >= (int)_loaded_textures.size()) {
            Protokoll.WriteText( false, "-> Error: texture handle idx %d acquired from _texture_map is outside\n"
                    "\t_loaded_textures array bounds. Lower bound: %d  Upper bound: %d\n"
                    "\tfilename: %s\n", idx, 0, _loaded_textures.size()-1, filename.c_str());
            return -1;
        }
#endif
    } 
    
    if (idx == -1) {
        // There were no previous loadings of the texture, so create a new slot for it
        TextureHandle th;
        _loaded_textures.push_back(th);
        idx = _loaded_textures.size()-1;
        // Create entry in _texture_map mapping texture's filename to the new _loaded_textures index
        _texture_map[filename] = idx;
    }
        
    TextureHandle &th = _loaded_textures[idx];

    if (th.instances > 0) {
        // This texture is already loaded in VRAM
        ++th.instances;
#ifdef _DEBUG
        Protokoll.WriteText( false, "-> Prevented loading of duplicate texture: %s, total references: %d\n", 
                filename.c_str(), th.instances);
#endif
    } else {
        // This texture is not in VRAM, load it..
        if ( !LoadTextureFromFile(filename, th)) {
            Protokoll.WriteText( true, "-> Error loading texture from disk: %s\n", filename.c_str() );
            th.instances = 0;
        } else {
            th.instances = 1;
        }
    }

    // If we have loaded npot scale factors from an external file, see if we have some for this texture:
    if (!_scalefactors_map.empty()) {
        std::string filename_sans_ext(filename);
        ReplaceAll( filename_sans_ext, ".png", "" );
        std::map< std::string, std::pair<double,double> >::iterator it = _scalefactors_map.find(filename_sans_ext);
        if ( it != _scalefactors_map.end() ) {
            th.npot_scalex = (*it).second.first;
            th.npot_scaley = (*it).second.second;
#ifdef _DEBUG
            Protokoll.WriteText( false, "Using external npot scalefactors %f, %f for texture %s\n", th.npot_scalex, th.npot_scaley, filename.c_str() );
#endif
        }
    }

    return idx;
}

void TexturesystemClass::UnloadTexture(const int idx)
{
    if (idx >= 0 && idx < (int)_loaded_textures.size()) {
        TextureHandle &th = _loaded_textures[idx];
        if (th.instances > 0) {
            --th.instances;
            if (th.instances == 0) {
#if defined(PLATFORM_DIRECTX)
                DX8_UnloadTexture(th);
#else
                SDL_UnloadTexture(th);
#endif
#ifdef _DEBUG
		        Protokoll.WriteText( false, "-> Texture successfully released !\n" );
#endif
            }
        }
    }
}

void TexturesystemClass::ReadScaleFactorsFile( const std::string &fullpath )
{
    std::ifstream file(fullpath.c_str(), std::ios::in);
    if (!file.is_open())
        return;

    Protokoll.WriteText( false, "Reading texture NPOT scale factors from %s\n", fullpath.c_str() );

    std::string name;
    double xscale = 0, yscale = 0;
    while (file >> name >> xscale >> yscale) {
        if (!name.empty() && xscale != 0.0 && yscale != 0.0) {
            _scalefactors_map[name] = std::make_pair(xscale, yscale);
#ifdef _DEBUG
            Protokoll.WriteText( false, "Read name=%s xscale=%f yscale=%f\n", name.c_str(), xscale, yscale );
#endif
        }
    }

    file.close();
}

void TexturesystemClass::ReadScaleFactorsFiles()
{
    std::string path, fullpath;

    if (CommandLineParams.RunOwnLevelList) {
        path = std::string(g_storage_ext) + "/levels/" +
               std::string(CommandLineParams.OwnLevelList) + "/data/textures/";
    } else {
        path = std::string(g_storage_ext) + "/data/textures/";
    }

    // First, see if there is a file in data/textures/ where plain old .PNG files are and load its data:
    fullpath = path + scalefactors_filename;
    if (FileExists(fullpath.c_str()))
        ReadScaleFactorsFile(fullpath);

    // Then, handle any files in the compressed-textures subfolders, their data will also be loaded,
    // and any data they contain will override what's already loaded, on a file-by-file basis.
#if defined(USE_ETC1)
    fullpath = path + "etc1/" + scalefactors_filename; 
    if (FileExists(fullpath.c_str()))
        ReadScaleFactorsFile(fullpath);
#endif

#if defined(USE_PVRTC)
    fullpath = path + "pvr/" + scalefactors_filename; 
    if (FileExists(fullpath.c_str()))
        ReadScaleFactorsFile(fullpath);
#endif
}

bool TexturesystemClass::LoadTextureFromFile( const std::string &filename, TextureHandle &th )
{
    if (filename.empty()) {
        Protokoll.WriteText( false, "Error: empty filename passed to LoadTextureFromFile()\n" );
        return false;
    }

    std::string path = std::string(g_storage_ext);

    //DKS - All textures are now stored in their own data/textures/ subdir:
    // Are we using a custom level set?
    if (CommandLineParams.RunOwnLevelList) {
        path += "/levels/" + std::string(CommandLineParams.OwnLevelList);
    }

    path += "/data/textures";

    bool success = false;

#if defined(USE_UNRARLIB)
    // Are we using unrarlib to read all game data from a single RAR archive?
    void  *buf_data   = NULL;   // Memory  buffer file is read into, if using unrarlib
    unsigned long buf_size = 0;  // Size of memory buffer file is read into, if using unrarlib
    if ( FileExists(RARFILENAME) &&
            urarlib_get(&buf_data, &buf_size, filename.c_str(), RARFILENAME, convertText(RARFILEPASSWORD)) &&
            buf_data != NULL )
    {
        // Load the texture from the image that is now in buf_data[]
#if defined(PLATFORM_DIRECTX)
        success = DX8_LoadTexture( NULL, NULL, buf_data, buf_size, th );
#elif defined(PLATFORM_SDL)
        success = SDL_LoadTexture( NULL, NULL, buf_data, buf_size, th );
#endif
        if (buf_data)
            free(buf_data);

        if (success) {
            goto loaded;
        } else {
            Protokoll.WriteText( false, "Error loading texture %s from archive %s,\n", filename.c_str(), RARFILENAME );
            Protokoll.WriteText( false, "->Trying elsewhere..\n" );
        }
    }
#endif // USE_UNRARLIB

    // Load the texture from disk:
#if defined(PLATFORM_DIRECTX)
    success = DX8_LoadTexture( path, filename, NULL, 0, th );
#elif defined(PLATFORM_SDL)
    success = SDL_LoadTexture( path, filename, NULL, 0, th );
#endif
    if (success)
        goto loaded;

loaded:
    if (!success) {
        Protokoll.WriteText( true, "Error loading texture %s\n", filename.c_str() );
    } else {
        std::string tmpstr( std::string(TextArray[TEXT_LADE_BITMAP]) + ' ' + filename +
                ' ' + std::string(TextArray[TEXT_LADEN_ERFOLGREICH]) + '\n' );
        DisplayLoadInfo(tmpstr.c_str());
    }
        
    return success;
}


//DKS - This is an UNTESTED effort at incorporating DirectX support back into the new texture system.
//      It should be more flexible than the original game's DirectX texture support, in that
//      it allows for loading textures from disk that are arbitrarily resized.
#if defined(PLATFORM_DIRECTX)
bool TexturesystemClass::DX8_LoadTexture( const std::string &path, const std::string &filename, 
                                          void *buf, unsigned int buf_size, TextureHandle &th )
{
    HRESULT  hresult;
    bool     load_from_memory = buf_size > 0;
    std::string   fullpath = path + "/" + filename;


    if (load_from_memory && !buf) {
        Protokoll.WriteText( true, "Error: null ptr passed to DX8_LoadTexture() reading file from memory\n");
        return false;
    } else if (filename.empty()) {
        Protokoll.WriteText( true, "Error: empty filename passed to DX8_LoadTexture()\n");
        return false;
    }

    if (load_from_memory) {
        // Load texture from memory buffer
        hresult = D3DXCreateTextureFromFileInMemoryEx(
                      lpD3DDevice,
                      (LPVOID)buf,
                      buf_size,
                      NULL, NULL,				  // x und y Grösse des Sprites (aus Datei übernehmen)
                      1,                          // Nur eine Version der Textur
                      0,                          // Immer 0 setzen
                      D3DFMT_UNKNOWN,			  // Format aus der Datei lesen
                      D3DPOOL_MANAGED,            // DX bestimmt wo die Textur gespeichert wird
                      D3DX_FILTER_NONE,			  // Keine Filter verwenden
                      D3DX_FILTER_NONE,
                      0xFFFF00FF,                 // Colorkeyfarbe (Lila)
                      NULL,						  // Keine Image Info
                      NULL,						  // Keine Palette angeben
                      &th.tex);
    } else {
        if (!FileExists(fullpath.c_str()))
            return false;

        hresult = D3DXCreateTextureFromFileEx(
                      lpD3DDevice,
                      fullpath.c_str(),
                      NULL, NULL,				  // x und y Grösse des Sprites (aus Datei übernehmen)
                      1,                          // Nur eine Version der Textur
                      0,                          // Immer 0 setzen
                      D3DFMT_UNKNOWN,			  // Format aus der Datei lesen
                      D3DPOOL_MANAGED,            // DX bestimmt wo die Textur gespeichert wird
                      D3DX_FILTER_NONE,			  // Keine Filter verwenden
                      D3DX_FILTER_NONE,
                      0xFFFF00FF,                 // Colorkeyfarbe (Lila)
                      NULL,						  // Keine Image Info
                      NULL,						  // Keine Palette angeben
                      &th.tex);
    }

    if (hresult != D3D_OK) {
        if (load_from_memory)
            Protokoll.WriteText( true, "Error in DirectX loading texture\n" );
        else
            Protokoll.WriteText( true, "Error in DirectX loading texture: %s\n", fullpath.c_str() );

        return false;
    } else {
        // Get the dimensions of texture in VRAM:
        D3DSURFACE_DESC tex_info;
        tex_info.Width = tex_info.Height = 0;
        th.tex->GetLevelDesc(0,&tex_info);

        // Get the dimensions of the file directly:
        D3DXIMAGE_INFO img_info;
        img_info.Width = img_info.Height = 0;

        if (load_from_memory)
            hresult = D3DXGetimgInfoFromFileInMemory( buf, buf_size, &img_info );
        else
            hresult = D3DXGetimgInfoFromFile( fullpath.c_str(), &img_info );

        if (hresult != D3D_OK ||
                tex_info.Width == 0 || tex_info.Height == 0 ||
                img_info.Width == 0 || img_info.Height == 0) {
            Protokoll.WriteText( true, "Error in DirectX reading image dimensions\n" );
        } else {
            th.npot_scalex = (double)img_info.Width  / (double)tex_info.Width;
            th.npot_scaley = (double)img_info.Height / (double)tex_info.Height;
        }
    }
    
    th.instances = 1;
    return true;
}

void TexturesystemClass::DX8_UnloadTexture( TextureHandle &th )
{
    SafeRelease(th.tex);
    th.instances = 0;
}
#endif //PLATFORM_DIRECTX
