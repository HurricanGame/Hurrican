// DKS - DX8Texture.cpp is a new file I have added.
//      It contains a new texture system class that allows sprites to share textures,
//      where before the game was loading duplicate copies in places. It also
//      abstracts away the details like the alpha textures needed by ETC1 compression.
//      It also unifies the interface that was once different between DX8 and SDL.

#if defined(PLATFORM_SDL)
#include "SDLPort/SDL_port.hpp"
#endif

#include "DX8Texture.hpp"

#if defined(PLATFORM_SDL)
#include "SDLPort/texture.hpp"
#endif

#include "Gameplay.hpp"
#include "Main.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
namespace fs = std::filesystem;

#ifdef USE_UNRARLIB
#include "unrarlib.h"
#endif

const std::string TexturesystemClass::scalefactors_filename("scalefactors.txt");

// --------------------------------------------------------------------------------------
// TexturesystemClass functions
// --------------------------------------------------------------------------------------

void TexturesystemClass::Exit() {
    // If there are any remaining textures in memory that have not already been
    // unloaded through DirectGraphicsSprite destructors, mop them up now:
    //   A good example would be textures that are member sprites of
    // global static objects, whose order of destruction can't be guaranteed,
    // such as PlayerClass.

    for (unsigned int i = 0; i < _loaded_textures.size(); ++i) {
        // Force unloading of the texture if it's still loaded, by making its
        //  instances equal 1 and calling UnloadTexture():
        if (_loaded_textures[i].instances > 0) {
            _loaded_textures[i].instances = 1;
            UnloadTexture(i);
        }
    }
}

// DKS - This largely replaces and expands on a lot of the code
//      DirectGraphicsSprite::LoadImage used to handle.
int16_t TexturesystemClass::LoadTexture(const std::string &filename) {
    if (filename.empty())
        return -1;

    int16_t idx = -1;

    // Check to see if the texture was previously loaded at some point
    std::map<std::string, int16_t>::iterator it = _texture_map.find(filename);
    if (it != _texture_map.end()) {
        // Texture has been loaded previously, so it at least has a slot in _loaded_textures,
        //  but if its instances == 0, it will need to be re-loaded from disk.
        idx = (*it).second;
#ifndef NDEBUG
        if (idx < 0 || static_cast<int>(idx) >= static_cast<int>(_loaded_textures.size())) {
            Protokoll << "-> Error: texture handle idx " << idx
                      << " acquired from _texture_map is outside\n"
                         "\t_loaded_textures array bounds. Lower bound: 0  Upper bound: "
                      << _loaded_textures.size() - 1
                      << "\n"
                         "\tfilename: "
                      << filename << std::endl;
            return -1;
        }
#endif
    }

    if (idx == -1) {
        // There were no previous loadings of the texture, so create a new slot for it
        TextureHandle th;
        _loaded_textures.push_back(th);
        idx = _loaded_textures.size() - 1;
        // Create entry in _texture_map mapping texture's filename to the new _loaded_textures index
        _texture_map[filename] = idx;
    }

    TextureHandle &th = _loaded_textures[idx];

    if (th.instances > 0) {
        // This texture is already loaded in VRAM
        ++th.instances;
#ifndef NDEBUG
        Protokoll << "-> Prevented loading of duplicate texture: " << filename << ", total references: " << th.instances
                  << std::endl;
#endif
    } else {
        // This texture is not in VRAM, load it..
        if (!LoadTextureFromFile(filename, th)) {
            Protokoll << "-> Error loading texture from disk: " << filename << std::endl;
            GameRunning = false;
            th.instances = 0;
        } else {
            th.instances = 1;
        }
    }

    // If we have loaded npot scale factors from an external file, see if we have some for this texture:
    if (!_scalefactors_map.empty()) {
        std::string filename_sans_ext(filename);
        ReplaceAll(filename_sans_ext, ".png", "");
        std::map<std::string, std::pair<double, double> >::iterator it2 = _scalefactors_map.find(filename_sans_ext);
        if (it2 != _scalefactors_map.end()) {
            th.npot_scalex = (*it2).second.first;
            th.npot_scaley = (*it2).second.second;
#ifndef NDEBUG
            Protokoll << "Using external npot scalefactors " << th.npot_scalex << " " << th.npot_scaley
                      << " for texture " << filename << std::endl;
#endif
        }
    }

    return idx;
}

void TexturesystemClass::UnloadTexture(const int idx) {
    if (idx >= 0 && idx < static_cast<int>(_loaded_textures.size())) {
        TextureHandle &th = _loaded_textures[idx];
        if (th.instances > 0) {
            --th.instances;
            if (th.instances == 0) {
                SDL_UnloadTexture(th);
#ifndef NDEBUG
                Protokoll << "-> Texture successfully released !" << std::endl;
#endif
            }
        }
    }
}

void TexturesystemClass::ReadScaleFactorsFile(const std::string &fullpath) {
    std::ifstream file(fullpath.c_str(), std::ios::in);
    if (!file.is_open())
        return;

    Protokoll << "Reading texture NPOT scale factors from " << fullpath << std::endl;

    std::string name;
    double xscale = 0, yscale = 0;
    while (file >> name >> xscale >> yscale) {
        if (!name.empty() && xscale != 0.0 && yscale != 0.0) {
            _scalefactors_map[name] = std::make_pair(xscale, yscale);
#ifndef NDEBUG
            Protokoll << "Read name= " << name << " xscale=" << xscale << " yscale=" << yscale << std::endl;
#endif
        }
    }

    file.close();
}

void TexturesystemClass::ReadScaleFactorsFiles() {
    std::string path, fullpath;

    if (CommandLineParams.RunOwnLevelList) {
        path = g_storage_ext + "/data/levels/" + std::string(CommandLineParams.OwnLevelList);
    } else {
        path = g_storage_ext + "/data/textures/";
    }

    // First, see if there is a file in data/textures/ where plain old .PNG files are and load its data:
    fullpath = path + scalefactors_filename;
    if (fs::exists(fullpath) && fs::is_regular_file(fullpath))
        ReadScaleFactorsFile(fullpath);

    // Then, handle any files in the compressed-textures subfolders, their data will also be loaded,
    // and any data they contain will override what's already loaded, on a file-by-file basis.
#if defined(USE_ETC1)
    fullpath = path + "etc1/" + scalefactors_filename;
    if (fs::exists(fullpath) && fs::is_regular_file(fullpath))
        ReadScaleFactorsFile(fullpath);
#endif

#if defined(USE_PVRTC)
    fullpath = path + "pvr/" + scalefactors_filename;
    if (fs::exists(fullpath) && fs::is_regular_file(fullpath))
        ReadScaleFactorsFile(fullpath);
#endif
}

bool TexturesystemClass::LoadTextureFromFile(const std::string &filename, TextureHandle &th) {
    if (filename.empty()) {
        Protokoll << "Error: empty filename passed to LoadTextureFromFile()" << std::endl;
        return false;
    }

    bool success = false;

    std::string fullpath, path;

    // Zuerst checken, ob sich der Song in einem MOD-Ordner befindet
    if (CommandLineParams.RunOwnLevelList) {
        path = g_storage_ext + "/data/levels/" + std::string(CommandLineParams.OwnLevelList);
        fullpath = path + "/" + filename;
        if (fs::exists(fullpath) && fs::is_regular_file(fullpath))
            goto loadfile;
    }

    // Dann checken, ob sich das File im Standard Ordner befindet
    path = g_storage_ext + "/data/textures";
    fullpath = path + "/" + filename;
    if (fs::exists(fullpath) && fs::is_regular_file(fullpath))
        goto loadfile;

#if defined(USE_UNRARLIB)
    // Are we using unrarlib to read all game data from a single RAR archive?
    void *buf_data = nullptr;       // Memory  buffer file is read into, if using unrarlib
    unsigned long buf_size = 0;  // Size of memory buffer file is read into, if using unrarlib
    if (fs::exists(RARFILENAME) && fs::is_regular_file(RARFILENAME) &&
        urarlib_get(&buf_data, &buf_size, filename.c_str(), RARFILENAME, convertText(RARFILEPASSWORD)) &&
        buf_data != nullptr) {
        // Load the texture from the image that is now in buf_data[]
        success = SDL_LoadTexture(nullptr, nullptr, buf_data, buf_size, th);
        if (buf_data)
            free(buf_data);

        if (success) {
            goto loaded;
        } else {
            Protokoll << "Error loading texture " << filename << " from archive " << RARFILENAME << std::endl;
            Protokoll << "->Trying elsewhere.." << std::endl;
        }
    }
#endif  // USE_UNRARLIB
loadfile:
    // Load the texture from disk:
    success = SDL_LoadTexture(path, filename, nullptr, 0, th);
    if (success)
        goto loaded;

loaded:
    if (!success) {
        Protokoll << "Error loading texture " << filename << std::endl;
        GameRunning = false;
    } else {
        std::string tmpstr(std::string(TextArray[TEXT_LADE_BITMAP]) + ' ' + filename + ' ' +
                           std::string(TextArray[TEXT_LADEN_ERFOLGREICH]) + '\n');
        DisplayLoadInfo(tmpstr.c_str());
    }

    return success;
}
