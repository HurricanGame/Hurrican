// Datei : DX8Graphics.cpp

// --------------------------------------------------------------------------------------
//
// Direct Graphics Klasse
// zum initialisieren von DirectX8
// beinhaltet zudem verschiedene Grafik-Funktionen zum Speichern von Screenshots usw
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "DX8Graphics.hpp"
#include <time.h>
#include <algorithm>
#include <stdexcept>
#include <string>
#include "DX8Font.hpp"
#include "DX8Texture.hpp"
#include "Gameplay.hpp"
#include "Globals.hpp"
#include "Logdatei.hpp"
#include "Main.hpp"
#if defined(USE_EGL_SDL) || defined(USE_EGL_RAW) || defined(USE_EGL_RPI)
#include "eglport.h"
#endif

// --------------------------------------------------------------------------------------
// sonstige Variablen
// --------------------------------------------------------------------------------------

glm::mat4x4 matProj;   // Projektionsmatrix
glm::mat4x4 matWorld;  // Weltmatrix
glm::mat4x4 matProjWindow;
glm::mat4x4 matProjRender;
float DegreetoRad[360];  // Tabelle mit Rotationswerten

// --------------------------------------------------------------------------------------
// Klassenfunktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// DirectGraphics Klasse
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

DirectGraphicsClass::DirectGraphicsClass() {
    SupportedETC1 = false;
    SupportedPVRTC = false;
    use_shader = shader_t::COLOR;

#if defined(USE_GL2) || defined(USE_GL3)
    ProgramCurrent = PROGRAM_NONE;
#endif
}

// --------------------------------------------------------------------------------------
// Desktruktor
// --------------------------------------------------------------------------------------

DirectGraphicsClass::~DirectGraphicsClass() {}

// --------------------------------------------------------------------------------------
// D3D Initialisieren
// --------------------------------------------------------------------------------------
bool DirectGraphicsClass::Init(std::uint32_t dwBreite, std::uint32_t dwHoehe, std::uint32_t dwZ_Bits, bool VSync) {
    bool isFullscreen = CommandLineParams.RunWindowMode != ScreenMode::WINDOW;
    uint16_t ScreenWidth = SCREENWIDTH;
    uint16_t ScreenHeight = SCREENHEIGHT;

    if (CommandLineParams.LowRes) {
        ScreenWidth = LOWRES_SCREENWIDTH;
        ScreenHeight = LOWRES_SCREENHEIGHT;
    }

    uint16_t ScreenDepth = CommandLineParams.ScreenDepth;
#if SDL_VERSION_ATLEAST(2, 0, 0)
    uint32_t flags = SDL_WINDOW_OPENGL;
#else /* SDL 1.2 */
#if defined(USE_EGL_SDL) || defined(USE_EGL_RAW) || defined(USE_EGL_RPI)
    uint32_t flags = SDL_SWSURFACE;
#else
    uint32_t flags = SDL_SWSURFACE | SDL_OPENGL;
#endif
#endif

#if defined(RPI)
    ScreenWidth = 0;
    ScreenHeight = 0;
#endif

    Protokoll << "\n--> SDL/OpenGL init <--\n";
    Protokoll << "---------------------\n" << std::endl;

    // Initialize defaults, Video and Audio subsystems
    Protokoll << "Initializing SDL." << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) == -1) {
        Protokoll << "Failed to initialize SDL: " << SDL_GetError() << "." << std::endl;
        return false;
    }
    Protokoll << "SDL initialized." << std::endl;

#if !defined(USE_EGL_SDL) && !defined(USE_EGL_RAW) && !defined(USE_EGL_RPI)
    if (CommandLineParams.ScreenDepth > 16) {     // DKS - Screen depth is now default 16 under GLES, 32 others
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);  //      (Can now be changed via command line switch)
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);

        if (CommandLineParams.ScreenDepth >= 32) {
            SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
        } else {
            SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
            SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 24);
        }
    } else {
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 16);
    }
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);          // DKS - No need for a depth buffer in this game
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);  // DKS - Changed this to 0 (Game would not load w/ GL1.2 laptop)
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
#if !SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, VSync);
#endif

#if SDL_VERSION_ATLEAST(2, 0, 0)
#if defined(USE_GLES1)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#elif defined(USE_GLES2)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(USE_GLES3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif /* defined(USE_GLES1) */
#endif /* SDL_VERSION_ATLEAST(2,0,0) */
#endif // !EGL

    // Setup SDL Screen
    if (isFullscreen) {
#if SDL_VERSION_ATLEAST(2, 0, 0)
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
#else /* SDL 1.2 */
        flags |= SDL_FULLSCREEN;
#endif
    }

#if SDL_VERSION_ATLEAST(2, 0, 0)
    // Create a window. Window mode MUST include SDL_WINDOW_OPENGL for use with OpenGL.
    Window =
        SDL_CreateWindow("Hurrican", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ScreenWidth, ScreenHeight, flags);
    if (Window == nullptr) {
        Protokoll << "Failed to create " << ScreenWidth << "x" << ScreenHeight
                  << " window: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create an OpenGL context associated with the window.
    GLcontext = SDL_GL_CreateContext(Window);
    if (GLcontext == nullptr) {
        Protokoll << "Failed to create GL context: " << SDL_GetError() << std::endl;
        return false;
    }
#else /* SDL 1.2 */
    // SDL_WM_SetCaption("Hurrican", "Hurrican");

    Screen = SDL_SetVideoMode(ScreenWidth, ScreenHeight, ScreenDepth, flags);
    if (Screen == nullptr) {
        Protokoll << "Failed to set " << ScreenWidth << "x" << ScreenHeight << "x" << ScreenDepth
                  << " video mode: " << SDL_GetError() << std::endl;
        return false;
    }
#endif

    SDL_ShowCursor(SDL_DISABLE);

#if defined(USE_EGL_SDL) || defined(USE_EGL_RAW) || defined(USE_EGL_RPI)
    int actual_w, actual_h;
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_GetWindowSize(Window, &actual_w, &actual_h);
#else
    actual_w = Screen->w;
    actual_h = Screen->h;
#endif

    // DKS - EGL_Open now takes additional parameters to specify desired screen
    //      depth and vsync. If eglport.cfg file is present, it will override
    //      whatever is passed here, however:
    if (EGL_Open(actual_w, actual_h, CommandLineParams.ScreenDepth == 16 ? 16 : 32, CommandLineParams.VSync) != 0) {
        return false;
    }
#endif

    // DKS - BEGIN VSYNC-RELATED CODE:
    // If not using EGL, i.e. using SDL's GL handling, some more handling of
    //  Vsync is necessary now that context has been created:
#if !defined(USE_EGL_SDL) && !defined(USE_EGL_RAW) && !defined(USE_EGL_RPI)
#if SDL_VERSION_ATLEAST(2, 0, 0)
    {
        int retval = -1;
        if (VSync) {
            // Beginning with SDL 2.0, we set vsync directly with function.
            // First, try setting it to -1, which requests 'late swap tearing', which
            //  will not wait for vsync if previous frame was missed:

            Protokoll << "-> Requesting SDL2 GL to enable VSync with 'late swap tearing' (optimal)" << std::endl;
            retval = SDL_GL_SetSwapInterval(-1);
            if (retval < 0) {
                Protokoll << "-> 'Late swap tearing' VSync not supported:\n" << SDL_GetError() << std::endl;
                Protokoll << "-> Requesting SDL2 GL to enable standard VSync" << std::endl;
                retval = SDL_GL_SetSwapInterval(1);
                if (retval < 0) {
                    Protokoll << "-> *** SDL2 GL failed to enable VSync:\n" << SDL_GetError() << std::endl;
                    VSyncEnabled = false;
                } else {
                    Protokoll << "-> VSync enabled successfully" << std::endl;
                    VSyncEnabled = true;
                }
            } else {
                Protokoll << "-> VSync with late-swap-tearing enabled successfully" << std::endl;
                VSyncEnabled = true;
            }
        } else {
            Protokoll << "-> Requesting SDL2 GL to disable VSync" << std::endl;
            retval = SDL_GL_SetSwapInterval(0);
            if (retval < 0) {
                Protokoll << "-> *** SDL2 GL failed to disable VSync:\n" << SDL_GetError() << std::endl;
                VSyncEnabled = true;
            } else {
                VSyncEnabled = false;
            }
        }
    }
#else   // SDL_VERSION < 2.0:
    {
        // Try to determine if VSync got set, because if not, we'll have to call
        //  glFlush() every frame to ensure image gets displayed on all platforms
        int status;
        int retval = SDL_GL_GetAttribute(SDL_GL_SWAP_CONTROL, &status);
        if (retval >= 0) {
            if (status == 1) {
                if (VSync) {
                    Protokoll << "-> SDL1 GL reports VSync was enabled successfully" << std::endl;
                } else {
                    // We requested no VSync, but got it anyways:
                    Protokoll << "-> *** SDL1 GL reports VSync could not be disabled. (try fullscreen?)" << std::endl;
                }

                VSyncEnabled = true;
            } else {
                if (VSync) {
                    // We requested VSync, but didn't get it:
                    Protokoll << "-> *** SDL1 GL reports VSync is unavailable. (try fullscreen?)" << std::endl;
                } else {
                    Protokoll << "-> SDL1 GL reports VSync was disabled successfully" << std::endl;
                }
                VSyncEnabled = false;
            }
        } else {
            // Call to SDL_GL_GetAttribute failed for some reason, we'll assume VSync
            //  is not set and call glFlush() each frame
            Protokoll << "-> *** Unable to determine SDL GL VSync status:\n" << SDL_GetError() << std::endl;
            Protokoll << "-> *** (Will assume VSync is unavailable). )" << std::endl;
            VSyncEnabled = false;
        }
    }
#endif  // SDL GL VERSION CHECKS
#else   // Using EGL:
    // TODO: If using EGL, determine if we actually are using VSync in
    //  SDLPort/eglport.cpp and provide interface to set UsingVSync
    //  boolean here. For now, assume we got it if we requested it,
    //  ignoring the fact that if EGL_Open() found eglport.cfg file, its
    //  settings will override whatever Hurrican requested.
    //   Note that we don't actually do anything with the VSyncEnabled
    //  boolean here one way or the other when EGL is being used, we just
    //  let SDLPort/eglport.cpp take care of its own business.
    VSyncEnabled = VSync;
#endif  // END VSYNC-RELATED CODE

    if (!SetDeviceInfo())
        return false;

    Protokoll << "\n-> OpenGL init successful!\n" << std::endl;

    // DegreetoRad-Tabelle füllen
    for (int i = 0; i < 360; i++)
        DegreetoRad[i] = PI * static_cast<float>(i) / 180.0f;

    SetColorKeyMode();

    return true;
}

// --------------------------------------------------------------------------------------
// Direct3D beenden
// --------------------------------------------------------------------------------------

bool DirectGraphicsClass::Exit() {
#if defined(USE_GL2) || defined(USE_GL3)
    Shaders[PROGRAM_COLOR].Close();
    Shaders[PROGRAM_TEXTURE].Close();
    Shaders[PROGRAM_RENDER].Close();
#if (defined(USE_GL2) || defined(USE_GL3)) && defined(USE_FBO)
    RenderBuffer.Close();
#endif /* USE_FBO */
#endif /* USE_GL2 || USE_GL3 */
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_GL_DeleteContext(GLcontext);
    SDL_DestroyWindow(Window);
#endif
#if defined(USE_EGL_SDL) || defined(USE_EGL_RAW) || defined(USE_EGL_RPI)
    EGL_Close();
#endif
    SDL_Quit();
    Protokoll << "-> SDL/OpenGL shutdown successfully completed !" << std::endl;
    return true;
}

// --------------------------------------------------------------------------------------
// Infos für Device Objekt setzen
// Für Init und nach Task Wechsel
// --------------------------------------------------------------------------------------

bool DirectGraphicsClass::SetDeviceInfo() {
#if defined(USE_GL2) || defined(USE_GL3)
    std::string vert;
    std::string frag;
#if defined(USE_GL3)
    const char* glsl_version = "320";
#else /* USE_GL2 */
    const char* glsl_version = "100";
#endif
#endif

    SetupFramebuffers();

    /* OpenGL Information */
    Protokoll << "GL_VENDOR: " << glGetString(GL_VENDOR) << std::endl;
    Protokoll << "GL_RENDERER: " << glGetString(GL_RENDERER) << std::endl;
    Protokoll << "GL_VERSION: " << glGetString(GL_VERSION) << std::endl;
#if defined(USE_GL2) || defined(USE_GL3)
    Protokoll << "GL_SHADING_LANGUAGE_VERSION: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#endif
    glextensions = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));
    Protokoll << "GL_EXTENSIONS: " << glextensions << std::endl;

#if defined(USE_ETC1)
    SupportedETC1 = ExtensionSupported("GL_OES_compressed_ETC1_RGB8_texture");
#endif
#if defined(USE_PVRTC)
    SupportedPVRTC = ExtensionSupported("GL_IMG_texture_compression_pvrtc");
#endif

    /* Init OpenGL */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); /* Set the background black */
    glClearDepth(1.0f);                   /* Depth buffer setup */

    glDisable(GL_DEPTH_TEST); /* No Depth Testing */
    glEnable(GL_BLEND);

#if defined(USE_GL1)
    glDisable(GL_LIGHTING);                            /* No light */
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); /* Really Nice Perspective Calculations */
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#endif

#if defined(USE_GL2) || defined(USE_GL3)
    // Compile the shader code and link into a program
    vert = g_storage_ext + "/data/shaders/" + glsl_version + "/shader_color.vert";
    frag = g_storage_ext + "/data/shaders/" + glsl_version + "/shader_color.frag";

    if (!Shaders[PROGRAM_COLOR].Load(vert, frag)) {
        return false;
    }

    vert = g_storage_ext + "/data/shaders/" + glsl_version + "/shader_texture.vert";
#if defined(USE_ETC1)
    if (SupportedETC1 == true) {
        sprintf_s(frag, "%s/data/shaders/%s/shader_etc1_texture.frag", g_storage_ext, glsl_version);
    } else {
#endif
    frag = g_storage_ext + "/data/shaders/" + glsl_version + "/shader_texture.frag";
#if defined(USE_ETC1)
    }
#endif

    if (!Shaders[PROGRAM_TEXTURE].Load(vert, frag)) {
        return false;
    }

    vert = g_storage_ext + "/data/shaders/" + glsl_version + "/shader_render.vert";
    frag = g_storage_ext + "/data/shaders/" + glsl_version + "/shader_render.frag";

    if (!Shaders[PROGRAM_RENDER].Load(vert, frag)) {
        return false;
    }

    // Get names for attributes and uniforms
    Shaders[PROGRAM_COLOR].NamePos = Shaders[PROGRAM_COLOR].GetAttribute("a_Position");
    Shaders[PROGRAM_COLOR].NameClr = Shaders[PROGRAM_COLOR].GetAttribute("a_Color");
    Shaders[PROGRAM_COLOR].NameMvp = Shaders[PROGRAM_COLOR].GetUniform("u_MVPMatrix");

    Shaders[PROGRAM_TEXTURE].NamePos = Shaders[PROGRAM_TEXTURE].GetAttribute("a_Position");
    Shaders[PROGRAM_TEXTURE].NameClr = Shaders[PROGRAM_TEXTURE].GetAttribute("a_Color");
    Shaders[PROGRAM_TEXTURE].NameTex = Shaders[PROGRAM_TEXTURE].GetAttribute("a_Texcoord0");
    Shaders[PROGRAM_TEXTURE].NameMvp = Shaders[PROGRAM_TEXTURE].GetUniform("u_MVPMatrix");

#if defined(USE_ETC1)
    if (SupportedETC1 == true) {
        Shaders[PROGRAM_TEXTURE].texUnit0 = Shaders[PROGRAM_TEXTURE].GetUniform("u_Texture0");
        Shaders[PROGRAM_TEXTURE].texUnit1 = Shaders[PROGRAM_TEXTURE].GetUniform("u_Texture1");
    }
#endif
    Shaders[PROGRAM_RENDER].NamePos = Shaders[PROGRAM_RENDER].GetAttribute("a_Position");
    Shaders[PROGRAM_RENDER].NameClr = Shaders[PROGRAM_RENDER].GetAttribute("a_Color");
    Shaders[PROGRAM_RENDER].NameTex = Shaders[PROGRAM_RENDER].GetAttribute("a_Texcoord0");
    Shaders[PROGRAM_RENDER].NameMvp = Shaders[PROGRAM_RENDER].GetUniform("u_MVPMatrix");
    NameWW                          = Shaders[PROGRAM_RENDER].GetUniform("u_WindowWidth");
    NameWH                          = Shaders[PROGRAM_RENDER].GetUniform("u_WindowHeight");
    NameTime                        = Shaders[PROGRAM_RENDER].GetUniform("u_Time");
#endif /* USE_GL2 || USE_GL3 */

    /* Matrices setup */
    g_matView = glm::mat4x4(1.0f);
    g_matModelView = glm::mat4x4(1.0f);

    matProjWindow = glm::ortho(0.0f, static_cast<float>(WindowView.w), static_cast<float>(WindowView.h), 0.0f, 0.0f, 1.0f);
    matProjRender = glm::ortho(0.0f, static_cast<float>(RenderView.w), static_cast<float>(RenderView.h), 0.0f, 0.0f, 1.0f);

    matProj = matProjWindow;

#if defined(USE_GL1)
    /* change to the projection matrix and set our viewing volume. */
    load_matrix(GL_PROJECTION, glm::value_ptr(matProj));
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif /* USE_GL1 */

#if (defined(USE_GL2) || defined(USE_GL3)) && defined(USE_FBO)
    SelectBuffer(true);
#endif

    return true;
}

// --------------------------------------------------------------------------------------
// Screenshot machen
// bei Filename000.bmp anfangen und dann weiter bis 999, falls diese schon existieren
// --------------------------------------------------------------------------------------

bool DirectGraphicsClass::TakeScreenshot(const char Filename[100], int screenx, int screeny) {
    return true;
}

// --------------------------------------------------------------------------------------
// Renderstates für Sprites mit ColorKey setzen
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::SetColorKeyMode() {
    if (BlendMode == BlendModeEnum::COLORKEY)
        return;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    BlendMode = BlendModeEnum::COLORKEY;
}

// --------------------------------------------------------------------------------------
// Renderstates für Sprites setzen, die komplett weiss gerendert werden
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::SetWhiteMode() {
    if (BlendMode == BlendModeEnum::WHITE)
        return;

    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA);

    BlendMode = BlendModeEnum::WHITE;
}

// --------------------------------------------------------------------------------------
// Renderstates für Sprites mit Additivem Alphablending setzen
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::SetAdditiveMode() {
    if (BlendMode == BlendModeEnum::ADDITIV)
        return;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    BlendMode = BlendModeEnum::ADDITIV;
}

// --------------------------------------------------------------------------------------
// Renderstates für linearen Texturfilter ein/ausschalten
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::SetFilterMode(bool filteron) {
    FilterMode = filteron;
}

// --------------------------------------------------------------------------------------
// Rendert in den Buffer, der am Ende eines jeden Frames komplett
// in den Backbuffer gerendert wird
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::RendertoBuffer(GLenum PrimitiveType,
                                         std::uint32_t PrimitiveCount,
                                         void *pVertexStreamZeroData) {
    int stride = sizeof(VERTEX2D);
    size_t clr_offset = offsetof(VERTEX2D, color);
    size_t tex_offset = offsetof(VERTEX2D, tu);

#if defined(USE_GL2) || defined(USE_GL3)
    uint8_t program_next;
    bool is_texture;

    // Determine the shader program to use
    switch (use_shader) {
    case shader_t::TEXTURE:
        program_next = PROGRAM_TEXTURE;
        is_texture = true;
        break;
    case shader_t::RENDER:
        program_next = PROGRAM_RENDER;
        is_texture = true;
        break;
    default:
        program_next = PROGRAM_COLOR;
        is_texture = false;
    }

    // Check if the program is already in use
    if (ProgramCurrent != program_next) {
        Shaders[program_next].Use();
        if (program_next==PROGRAM_RENDER) {
            glUniform1i(NameWW, RenderRect.w);
            glUniform1i(NameWH, RenderRect.h);
            glUniform1i(NameTime, 50*SDL_GetTicks()/1000);
        }
        ProgramCurrent = program_next;
    }
#endif

    if (PrimitiveType == GL_LINES) {
        PrimitiveCount *= 2;
    } else if (PrimitiveType == GL_LINE_STRIP) {
        PrimitiveCount += 2;
    } else if (PrimitiveType == GL_TRIANGLES) {
        PrimitiveCount *= 3;
    } else if (PrimitiveType == GL_TRIANGLE_STRIP) {
        PrimitiveCount += 2;
    } else {
        Protokoll << "Add type to count indinces" << std::endl;
        return;
    }

#if defined(USE_GL1)
    // Enable the client states for transfer
    if (use_shader == shader_t::TEXTURE) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, stride, reinterpret_cast<uint8_t *>(pVertexStreamZeroData) + tex_offset);
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, stride, pVertexStreamZeroData);

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, stride, reinterpret_cast<uint8_t *>(pVertexStreamZeroData) + clr_offset);
#elif defined(USE_GL2) || defined(USE_GL3)
        // Enable attributes and uniforms for transfer
        if (is_texture) {
#if defined(USE_ETC1)
            if (SupportedETC1 == true) {
                glUniform1i(Shaders[ProgramCurrent].texUnit0, 0);
                glUniform1i(Shaders[ProgramCurrent].texUnit1, 1);
            }
#endif
            glEnableVertexAttribArray(Shaders[ProgramCurrent].NameTex);
            glVertexAttribPointer(Shaders[ProgramCurrent].NameTex, 2, GL_FLOAT, GL_FALSE, stride,
                                  reinterpret_cast<uint8_t *>(pVertexStreamZeroData) + tex_offset);
        }

        glEnableVertexAttribArray(Shaders[ProgramCurrent].NamePos);
        glVertexAttribPointer(Shaders[ProgramCurrent].NamePos, 2, GL_FLOAT, GL_FALSE, stride, pVertexStreamZeroData);

        glEnableVertexAttribArray(Shaders[ProgramCurrent].NameClr);
        glVertexAttribPointer(Shaders[ProgramCurrent].NameClr, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride,
                              reinterpret_cast<uint8_t *>(pVertexStreamZeroData) + clr_offset);

        glm::mat4x4 matMVP = matProj * g_matModelView;
        glUniformMatrix4fv(Shaders[ProgramCurrent].NameMvp, 1, GL_FALSE, glm::value_ptr(matMVP));
#endif

    glDrawArrays(PrimitiveType, 0, PrimitiveCount);

#if defined(USE_GL1)
    // Disable the client states
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    if (use_shader == shader_t::TEXTURE) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
#elif defined(USE_GL2) || defined(USE_GL3)
        // Disable attributes and uniforms
        glDisableVertexAttribArray(Shaders[ProgramCurrent].NamePos);
        glDisableVertexAttribArray(Shaders[ProgramCurrent].NameClr);

        if (is_texture) {
            glDisableVertexAttribArray(Shaders[ProgramCurrent].NameTex);
        }
#endif
}

// --------------------------------------------------------------------------------------
// Render den Buffer auf den Backbuffer
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::DisplayBuffer() {
    // Backbuffer mit Frontbuffer tauschen
    ShowBackBuffer();
}

#if defined(PLATFORM_SDL)
bool DirectGraphicsClass::ExtensionSupported(const char *ext) {
    if (strstr(glextensions, ext) != nullptr) {
        Protokoll << ext << " is supported" << std::endl;
        return true;
    }

    Protokoll << ext << " is not supported" << std::endl;
    return false;
}
#endif

// DKS - Supports new TexturesystemClass and is now used for both GL and DirectX
#if 0
#if defined(PLATFORM_SDL)
void DirectGraphicsClass::SetTexture( int32_t index )
{
    if (index >= 0)
    {
        use_shader = shader_t::TEXTURE;
        glBindTexture( GL_TEXTURE_2D, textures.at(index) );
#if defined(USE_GL1)
        glEnable( GL_TEXTURE_2D );
#endif
#if defined(USE_ETC1)
        if (SupportedETC1 == true)
        {
            glActiveTexture( GL_TEXTURE1 );
            glBindTexture( GL_TEXTURE_2D, alphatexs.at(index) );
            glActiveTexture( GL_TEXTURE0 );
        }
#endif
    }
    else
    {
        use_shader = shader_t::COLOR;
        glBindTexture( GL_TEXTURE_2D, 0 );
#if defined(USE_GL1)
        glDisable( GL_TEXTURE_2D );
#endif

#if defined(USE_ETC1)
        if (SupportedETC1 == true)
        {
            glActiveTexture( GL_TEXTURE1 );
            glBindTexture( GL_TEXTURE_2D, 0 );
            glActiveTexture( GL_TEXTURE0 );
        }
#endif
    }
}
#endif
#endif  // 0
void DirectGraphicsClass::SetTexture(int idx) {
    if (idx >= 0) {
        use_shader = shader_t::TEXTURE;
        TextureHandle &th = Textures[idx];
        glBindTexture(GL_TEXTURE_2D, th.tex);
#if defined(USE_ETC1)
        if (SupportedETC1 == true) {
            // Bind the alpha-channel texture when using ETC1-compressed textures:
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, th.alphatex);
            glActiveTexture(GL_TEXTURE0);
        }
#endif

#if defined(USE_GL1)
        glEnable(GL_TEXTURE_2D);
#endif
    } else {
        use_shader = shader_t::COLOR;
        // DKS - There is no need to call glBindTexture():
        // glBindTexture( GL_TEXTURE_2D, 0 );
#if defined(USE_GL1)
        glDisable(GL_TEXTURE_2D);
#endif

        // DKS - There is no need to call glBindTexture():
#if 0
#if defined(USE_ETC1)
        if (SupportedETC1 == true)
        {
            glActiveTexture( GL_TEXTURE1 );
            glBindTexture( GL_TEXTURE_2D, 0 );
            glActiveTexture( GL_TEXTURE0 );
        }
#endif
#endif  // 0
    }
}

// --------------------------------------------------------------------------------------
// Present aufrufen
// --------------------------------------------------------------------------------------

void DirectGraphicsClass::ShowBackBuffer() {
#if (defined(USE_GL2) || defined(USE_GL3)) && defined(USE_FBO)
    if (RenderBuffer.IsEnabled() == true) {
        VERTEX2D vertices[4];

        // Protokoll << std::dec << RenderRect.w << "x" << RenderRect.h << " at " << RenderRect.x << "x" << RenderRect.y
        // << std::endl;

        vertices[0].x = RenderRect.x;
        vertices[0].y = RenderRect.y + RenderRect.h; /* lower left */
        vertices[1].x = RenderRect.x;
        vertices[1].y = RenderRect.y; /* upper left */
        vertices[2].x = RenderRect.x + RenderRect.w;
        vertices[2].y = RenderRect.y + RenderRect.h; /* lower right */
        vertices[3].x = RenderRect.x + RenderRect.w;
        vertices[3].y = RenderRect.y; /* upper right */
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = 0xFFFFFFFF;

        vertices[0].tu = 0;
        vertices[0].tv = 0; /* lower left */
        vertices[1].tu = 0;
        vertices[1].tv = 1; /* upper left */
        vertices[2].tu = 1;
        vertices[2].tv = 0; /* lower right */
        vertices[3].tu = 1;
        vertices[3].tv = 1; /* upper right */

        SelectBuffer(false);

        use_shader = CommandLineParams.Scanlines ? shader_t::RENDER : shader_t::TEXTURE;
        RenderBuffer.BindTexture(true);

        RendertoBuffer(GL_TRIANGLE_STRIP, 2, &vertices[0]);

        use_shader = shader_t::COLOR;
        RenderBuffer.BindTexture(false);

#if defined(ANDROID)
        DrawTouchOverlay();
#endif
    }
#endif

#if defined(USE_EGL_SDL) || defined(USE_EGL_RAW) || defined(USE_EGL_RPI)
    EGL_SwapBuffers();
#else
// DKS - TODO: Test with SDL2.0 and ensure it wouldn't be better to
//      include this in the new VSyncEnabled test below, calling
//      glFlush() instead of SDL_GL_SwapWindow() when vsync is either
//      disabled or unavailable.
#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_GL_SwapWindow(Window);
#else  // SDL1.2:
        glFlush();
        SDL_GL_SwapBuffers();
#endif
#endif

#if defined(DEBUG)
    int error = glGetError();

    if (error != 0) {
        Protokoll << "GL Error " << std::hex << error << " file " << __FILE__ << ": line " << std::dec << __LINE__
                  << std::endl;
    }
#endif

#if (defined(USE_GL2) || defined(USE_GL3)) && defined(USE_FBO)
    SelectBuffer(true);
#endif
}

#if defined(PLATFORM_SDL)
void DirectGraphicsClass::SetupFramebuffers() {
/* Read the current window size */
#if SDL_VERSION_ATLEAST(2, 0, 0)
    {
        int tmp_w, tmp_h;
        SDL_GetWindowSize(Window, &tmp_w, &tmp_h);
        WindowView.w = tmp_w;
        WindowView.h = tmp_h;
    }
#else
    WindowView.w = Screen->w;
    WindowView.h = Screen->h;
#endif
    Protokoll << "Window resolution: " << WindowView.w << "x" << WindowView.h << std::endl;

    RenderView.x = 0;
    RenderView.y = 0;
    RenderView.w = RENDERWIDTH;
    RenderView.h = RENDERHEIGHT;
#if (defined(USE_GL2) || defined(USE_GL3)) && defined(USE_FBO)
    /* Create an FBO for rendering */
    RenderBuffer.Open(RenderView.w, RenderView.h);

    if (RenderBuffer.IsEnabled() == true) {
        /* Set the render viewport */
        SelectBuffer(true);
        glViewport(RenderView.x, RenderView.y, RenderView.w, RenderView.h);
        SelectBuffer(false);
        Protokoll << "Render viewport resolution: " << RenderView.w << "x" << RenderView.h << " at " << RenderView.x
                  << "x" << RenderView.y << std::endl;

        if (CommandLineParams.RunWindowMode == ScreenMode::FULLSCREEN_STRETCHED) {
            /* Fill the whole screen area */
            RenderRect.w = WindowView.w;
            RenderRect.h = WindowView.h;
            RenderRect.x = 0;
            RenderRect.y = 0;
        } else {
            /* Select the best 4:3 resolution */
            if (WindowView.w < WindowView.h) {
                RenderRect.w = WindowView.w;
                RenderRect.h = (WindowView.w / 4) * 3;
            } else {
                RenderRect.w = (WindowView.h / 3) * 4;
                RenderRect.h = WindowView.h;
            }

            RenderRect.x = std::max(0, WindowView.w - RenderRect.w) / 2;
            RenderRect.y = std::max(0, WindowView.h - RenderRect.h) / 2;
        }

        Protokoll << "Render area: " << RenderRect.w << "x" << RenderRect.h << " at " << RenderRect.x << "x"
                  << RenderRect.y << std::endl;
    } else
#endif
    {
        /* No scaling just center the rendering in the window */
        WindowView.x = std::max(0, (WindowView.w - RenderView.w) / 2);
        WindowView.y = std::max(0, (WindowView.h - RenderView.h) / 2);
        WindowView.w = RenderView.w;
        WindowView.h = RenderView.h;

#if defined(PANDORA)
        WindowView.x = 80;
        WindowView.y = 0;
#endif
    }

    if (CommandLineParams.LowRes) {
        glViewport(0, 0, LOWRES_SCREENWIDTH, LOWRES_SCREENHEIGHT);
    } else {
        glViewport(WindowView.x, WindowView.y, WindowView.w, WindowView.h); /* Setup our viewport. */
    }
    Protokoll << "Window viewport: " << WindowView.w << "x" << WindowView.h << " at " << WindowView.x << "x"
              << WindowView.y << std::endl;
}

void DirectGraphicsClass::ClearBackBuffer() {
#if (defined(USE_GL2) || defined(USE_GL3)) && defined(USE_FBO)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
    glClear(GL_COLOR_BUFFER_BIT);

#if (defined(USE_GL2) || defined(USE_GL3)) && defined(USE_FBO)
    glBindFramebuffer(GL_FRAMEBUFFER, RenderBuffer.GetFramebuffer());
    glClear(GL_COLOR_BUFFER_BIT);
#endif
}

#if (defined(USE_GL2) || defined(USE_GL3)) && defined(USE_FBO)
void DirectGraphicsClass::SelectBuffer(bool active) {
    if (RenderBuffer.IsEnabled() == true) {
        if (active == true) {
            glBindFramebuffer(GL_FRAMEBUFFER, RenderBuffer.GetFramebuffer());
            glViewport(RenderView.x, RenderView.y, RenderView.w, RenderView.h);
            matProj = matProjRender;
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(WindowView.x, WindowView.y, WindowView.w, WindowView.h);
            g_matModelView = glm::mat4x4(1.0f);
            matProj = matProjWindow;
        }
    }
}

#if defined(ANDROID)
void DirectGraphicsClass::DrawTouchOverlay() {
    int i;
    int x, y, w, h;
    VERTEX2D vertices[4];

    for (i = 0; i < DirectInput.TouchBoxes.size(); i++) {
        SDL_Rect *box = &DirectInput.TouchBoxes.at(i);

        vertices[0].x = box->x;
        vertices[0].y = box->y + box->h; /* lower left */
        vertices[1].x = box->x;
        vertices[1].y = box->y; /* upper left */
        vertices[2].x = box->x + box->w;
        vertices[2].y = box->y + box->h; /* lower right */
        vertices[3].x = box->x + box->w;
        vertices[3].y = box->y; /* upper right */
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = 0x4000FF00;

#if 0
        vertices[0].tu = 0;
        vertices[0].tv = 0; /* lower left */
        vertices[1].tu = 0;
        vertices[1].tv = 1; /* upper left */
        vertices[2].tu = 1;
        vertices[2].tv = 0; /* lower right */
        vertices[3].tu = 1;
        vertices[3].tv = 1; /* upper right */
#endif
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        RendertoBuffer(GL_TRIANGLE_STRIP, 2, &vertices[0]);
    }

    DrawCircle(DirectInput.TouchdpadX, DirectInput.TouchdpadY, DirectInput.TouchdpadRadius);
}

void DirectGraphicsClass::DrawCircle(uint16_t x, uint16_t y, uint16_t radius) {
    constexpr int SECTORS = 40;
    VERTEX2D vtx[SECTORS + 2];

    float radians = 0;
    for (int i = 0; i < SECTORS + 1; i++) {
        vtx[i].color = 0x4000FF00;

        vtx[i].x = x + (radius * cosf(cml::rad(radians)));
        vtx[i].y = y + (radius * sinf(cml::rad(radians)));
        radians += (360 / SECTORS);
    }
    vtx[SECTORS + 1].x = vtx[0].x;
    vtx[SECTORS + 1].y = vtx[0].y;
    vtx[SECTORS + 1].color = 0x8000FF00;

    glLineWidth(3);
    RendertoBuffer(GL_LINE_STRIP, SECTORS, &vtx[0]);
}
#endif /* ANDROID */
#endif /* (USE_GL2 || USE_GL3) && USE_FBO */

#endif /* PLATFORM_SDL */
