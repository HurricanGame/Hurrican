// Datei : DX8Input.cpp

// --------------------------------------------------------------------------------------
//
// DirectInput Klasse
// zum Erfassen von Eingaben durch Tastaur, Maus und Joystick/Joypad
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "DX8Input.hpp"
#include <algorithm>
#include "Gameplay.hpp"
#include "Globals.hpp"
#include "Logdatei.hpp"
#include "Main.hpp"

// --------------------------------------------------------------------------------------
// Variablen
// --------------------------------------------------------------------------------------

#if defined(PLATFORM_DIRECTX)
char TastaturPuffer[MAX_KEYS];  // Tastaturpuffer für Keyboardabfrage
#elif defined(PLATFORM_SDL)
const Uint8 *TastaturPuffer;
#endif

// --------------------------------------------------------------------------------------
// Joystick mit ForceFeedback Enumeration Callback-Funktion
// --------------------------------------------------------------------------------------

#if defined(PLATFORM_DIRECTX)
bool CALLBACK EnumForceFeedbackDevices(LPDIDEVICEINSTANCE lpddi, void *pv) {
    GUID *pguidDevice = NULL;

    if (pv) {
        pguidDevice = (GUID *)pv;
        *pguidDevice = lpddi->guidInstance;
    }

    DirectInput.Joysticks[DirectInput.JoysticksFound].CanForceFeedback = true;
    DirectInput.Joysticks[DirectInput.JoysticksFound].guidJoystickDevice = DirectInput.guidJoystickDevice;
    strcpy_s(DirectInput.Joysticks[DirectInput.JoysticksFound].JoystickName, strlen((char *)lpddi->tszProductName) + 1,
             (char *)lpddi->tszProductName);

    // Counter erhöhen
    DirectInput.JoysticksFound++;

    // Genug Joysticks? Dann halte die EnumDevices() Funktion an
    if (DirectInput.JoysticksFound < MAX_JOYSTICKS)
        return DIENUM_CONTINUE;
    else
        return DIENUM_STOP;
}
#elif defined(PLATFORM_SDL)
bool EnumForceFeedbackDevices(int lpddi, void *pv) {
    return true;
}
#endif

    // --------------------------------------------------------------------------------------
    // Joystick Enumeration Callback-Funktion
    // --------------------------------------------------------------------------------------

#if defined(PLATFORM_DIRECTX)
bool CALLBACK EnumJoystickDevices(LPDIDEVICEINSTANCE lpddi, void *pv) {
    GUID *pguidDevice = NULL;

    if (pv) {
        pguidDevice = (GUID *)pv;
        *pguidDevice = lpddi->guidInstance;
    }

    DirectInput.Joysticks[DirectInput.JoysticksFound].CanForceFeedback = false;
    strcpy_s(DirectInput.Joysticks[DirectInput.JoysticksFound].JoystickName, strlen((char *)lpddi->tszProductName) + 1,
             (char *)lpddi->tszProductName);

    // Counter erhöhen
    DirectInput.JoysticksFound++;

    // Genug Joysticks? Dann halte die EnumDevices() Funktion an
    if (DirectInput.JoysticksFound < MAX_JOYSTICKS)
        return DIENUM_CONTINUE;
    else
        return DIENUM_STOP;
}
#elif defined(PLATFORM_SDL)
bool EnumJoystickDevices(int lpddi, void *pv) {
    return true;
}
#endif

// --------------------------------------------------------------------------------------
// Klassenfunktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// DirectInput Klasse
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

DirectInputClass::DirectInputClass(void) {
#if defined(PLATFORM_DIRECTX)
    lpDI = NULL;
    lpDIKeyboard = NULL;
    lpDIMaus = NULL;
    NumberOfKeys = MAX_KEYS;
#endif

    // Zu Beginn alle Eingabegeräte zurücksetzen
    MausX = 0;
    MausY = 0;
    for (int i = 0; i < MAX_MOUSEBUTTONS; i++)
        MausButtons[i] = false;

    JoysticksFound = 0;
    UseForceFeedback = false;
#if defined(GCW)
    InternalJoystickIndex = 0;  // DKS: Set to the device's internal joystick controls, default is 0.
#endif                          // GCW
}

// --------------------------------------------------------------------------------------
// Destruktor
// --------------------------------------------------------------------------------------

DirectInputClass::~DirectInputClass(void) {}

    // --------------------------------------------------------------------------------------
    // DirectInput initialisieren
    // Keyboard und Maus initialisieren und Joystick, falls vorhanden
    // --------------------------------------------------------------------------------------

#if defined(PLATFORM_DIRECTX)
bool DirectInputClass::Init(HWND hwnd, HINSTANCE hinst) {
    HRESULT hresult;

    Protokoll << "\n--> DirectInput8 init <--\n";
    Protokoll << "-------------------------\n" << std::endl;

    hresult = DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID *)&lpDI, NULL);

    // DirectInput Haupt-Device erstellen
    if (hresult != DI_OK) {
        Protokoll << "\n-> DirectInput8Create error!" << std::endl;
        GameRunning = false;
        return false;
    }
    Protokoll << "DirectInput8Create successful!" << std::endl;

    //----- Keyboard

    // Keyboard Device erstellen
    hresult = lpDI->CreateDevice(GUID_SysKeyboard, &lpDIKeyboard, NULL);
    if (hresult != DI_OK) {
        Protokoll << "\n-> Keyboard : CreateDevice error!" << std::endl;
        GameRunning = false;
        return false;
    }
    Protokoll << "Keyboard : CreateDevice successful!" << std::endl;

    // Datenformat für Keyboard festlegen
    hresult = lpDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
    if (hresult != DI_OK) {
        Protokoll << "\n-> Keyboard : SetDataFormat error!" << std::endl;
        GameRunning = false;
        return false;
    }
    Protokoll << "Keyboard : SetDataFormat successful!" << std::endl;

    // Keyboard Cooperativelevel setzen
    if (CommandLineParams.RunWindowMode == true)
        hresult = lpDIKeyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
    else
        hresult = lpDIKeyboard->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

    if (hresult != DI_OK) {
        Protokoll << "\n-> Keyboard : SetCooperativeLevel error!" << std::endl;
        GameRunning = false;
        return false;
    }
    Protokoll << "Keyboard : SetCooperativeLevel successful!" << std::endl;

    // Keyboard akquirieren
    if (lpDIKeyboard) {
        hresult = lpDIKeyboard->Acquire();
        if (hresult != DI_OK) {
            Protokoll << "\n-> Keyboard : Acquire error!" << std::endl;
            GameRunning = false;
            return false;
        }
        Protokoll << "Keyboard : Acquire successful!" << std::endl;
    }

    // angeschlossenen Joystick mit ForceFeedback enumerieren
    hresult = lpDI->EnumDevices(DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)EnumForceFeedbackDevices,
                                &guidJoystickDevice, 0 /*DIEDFL_ATTACHEDONLY*/);
    if (hresult != DI_OK) {
        hresult = lpDI->EnumDevices(DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)EnumJoystickDevices,
                                    &guidJoystickDevice, 0 /*DIEDFL_ATTACHEDONLY*/);
    } else
        Protokoll << "Joystick : EnumDevices successful!" << std::endl;

    // Gefundene Joysticks initialisieren
    if (JoysticksFound == 0)
        Protokoll << "No joystick found, skipping init!" << std::endl;
    else
        for (int i = 0; i < JoysticksFound; i++) {
            char Buf[255];

            if (Joysticks[i].Init(hwnd, lpDI) == false) {
                strcpy_s(Buf, strlen("Error initialising Joystick ") + 1, "Error initialising Joystick ");
                strcat_s(Buf, strlen(Joysticks[i].JoystickName) + 1, Joysticks[i].JoystickName);
                strcat_s(Buf, 5, " !");
                Protokoll << Buf << std::endl;
            } else {
                JoystickFound = true;

                strcpy_s(Buf, strlen("Joystick found : ") + 1, "Joystick found : ");
                strcat_s(Buf, strlen(Joysticks[i].JoystickName) + 1, Joysticks[i].JoystickName);
                Protokoll << Buf << std::endl;

                if (Joysticks[i].CanForceFeedback) {
                    Protokoll << "Initializing ForceFeedback Effects" << std::endl;

                    // Vibrations-Effekte erstellen
                    // Kurzes, schwaches Vibrieren (ShorVib)
                    DICONSTANTFORCE diConstantForce;
                    DIRAMPFORCE diRampForce;
                    DIEFFECT diEffect;
                    std::uint32_t dwAxes[2] = {DIJOFS_X, DIJOFS_Y};
                    std::int32_t lDirection[2] = {180 * DI_DEGREES, 0};

                    diConstantForce.lMagnitude = DI_FFNOMINALMAX;

                    diEffect.dwSize = sizeof(DIEFFECT);
                    diEffect.dwFlags = DIEFF_OBJECTOFFSETS | DIEFF_CARTESIAN;
                    diEffect.dwDuration = DI_SECONDS / 4;
                    diEffect.dwSamplePeriod = 0;
                    diEffect.dwGain = DI_FFNOMINALMAX;
                    diEffect.dwTriggerButton = DIEB_NOTRIGGER;
                    diEffect.dwTriggerRepeatInterval = 0;
                    diEffect.cAxes = 2;
                    diEffect.rgdwAxes = dwAxes;
                    diEffect.rglDirection = lDirection;
                    diEffect.lpEnvelope = NULL;
                    diEffect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
                    diEffect.lpvTypeSpecificParams = &diConstantForce;
                    diEffect.dwStartDelay = 0;

                    hresult = Joysticks[i].lpDIJoystick->CreateEffect(GUID_ConstantForce, &diEffect,
                                                                      &Joysticks[i].pFFE_SmallVib, NULL);
                    if (hresult != DI_OK) {
                        Protokoll << "Error initializing Effect! Not using ForceFeedback" << std::endl;
                        Joysticks[i].CanForceFeedback = false;
                    }

                    // Kurzes, starkes Vibrieren (BigVib)
                    diEffect.dwFlags = DIEFF_OBJECTOFFSETS | DIEFF_POLAR;
                    diEffect.dwGain = DI_FFNOMINALMAX / 2;

                    hresult = Joysticks[i].lpDIJoystick->CreateEffect(GUID_ConstantForce, &diEffect,
                                                                      &Joysticks[i].pFFE_BigVib, NULL);
                    if (hresult != DI_OK) {
                        Protokoll << "Error initializing Effect! Not using ForceFeedback" << std::endl;
                        Joysticks[i].CanForceFeedback = false;
                    }

                    // Kurzes, abklingendes Vibrieren (MaxVib)
                    diEffect.dwGain = DI_FFNOMINALMAX;
                    diEffect.dwDuration = DI_SECONDS;
                    diRampForce.lStart = -10000;
                    diRampForce.lEnd = 0;
                    diEffect.cbTypeSpecificParams = sizeof(DIRAMPFORCE);
                    diEffect.lpvTypeSpecificParams = &diRampForce;

                    hresult = Joysticks[i].lpDIJoystick->CreateEffect(GUID_RampForce, &diEffect,
                                                                      &Joysticks[i].pFFE_MaxVib, NULL);
                    if (hresult != DI_OK) {
                        Protokoll << "Error initializing Effect! Not using ForceFeedback" << std::endl;
                        Joysticks[i].CanForceFeedback = false;
                    }

                    // Blitz Effekt
                    diEffect.dwTriggerRepeatInterval = INFINITE;
                    diEffect.dwGain = DI_FFNOMINALMAX;
                    diEffect.dwDuration = DI_SECONDS | INFINITE;
                    diConstantForce.lMagnitude = DI_FFNOMINALMAX;
                    diEffect.dwGain = DI_FFNOMINALMAX / 2;
                    diEffect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
                    diEffect.lpvTypeSpecificParams = &diConstantForce;

                    hresult = Joysticks[i].lpDIJoystick->CreateEffect(GUID_ConstantForce, &diEffect,
                                                                      &Joysticks[i].pFFE_Blitz, NULL);
                    if (hresult != DI_OK) {
                        Protokoll << "Error initializing Effect! Not using ForceFeedback" << std::endl;
                        Joysticks[i].CanForceFeedback = false;
                    }
                }
            }
        }

    Protokoll << "\n-> DirectInput8 init successful!\n" << std::endl;
    return true;
}
#elif defined(PLATFORM_SDL)
bool DirectInputClass::Init(HWND hwnd, HINSTANCE hinst) {
#if SDL_VERSION_ATLEAST(2, 0, 0)
    TastaturPuffer = SDL_GetKeyboardState(&NumberOfKeys);
#else
    TastaturPuffer = SDL_GetKeyState(&NumberOfKeys);
#endif
    Protokoll << "DirectInput8 polling for " << NumberOfKeys << " keys!" << std::endl;

#if defined(ANDROID)
    JoysticksFound = 0;
    JoystickFound = false;
#else
    JoysticksFound = SDL_NumJoysticks();

    for (int i = 0; i < JoysticksFound; i++) {
        if (Joysticks[i].Init(i) == false) {
            Protokoll << "Error opening joystick" << std::endl;
        } else {
            JoystickFound = true;

#if defined(GCW)
            if (strcmp(Joysticks[i].JoystickName, "linkdev device (Analog 2-axis 8-button 2-hat)") == 0) {
                // Replace GCW control's name with something that is clearer
                strcpy_s(Joysticks[i].JoystickName, "GCW Zero");
                InternalJoystickIndex = i;
                Protokoll << "Found GCW Zero's built-in controls." << std::endl;
            }
#endif  // GCW
        }
    }
#endif  /* ANDROID */

    return true;
}
#endif

// --------------------------------------------------------------------------------------
// DirectInput beenden
// --------------------------------------------------------------------------------------

void DirectInputClass::Exit(void) {
#if defined(PLATFORM_DIRECTX)
    if (lpDIKeyboard != NULL)  // Keyboard freigeben
    {
        lpDIKeyboard->Unacquire();
        lpDIKeyboard->Release();
        lpDIKeyboard = NULL;
    }

    /*if(lpDIMaus != NULL)				// Maus freigeben
    {
        lpDIMaus->Unacquire();
        lpDIMaus->Release();
        lpDIMaus = NULL;
    }*/

    for (int i = 0; i < JoysticksFound; i++) {
        if (Joysticks[i].lpDIJoystick != NULL)  // Joystick freigeben
        {
            Joysticks[i].lpDIJoystick->Unacquire();
            Joysticks[i].lpDIJoystick->Release();
            Joysticks[i].lpDIJoystick = NULL;
        }
    }

    SafeRelease(lpDI);  // DirectInput Hauptobjekt freigeben
    Protokoll << "-> DirectInput8 shutdown successfully completed !" << std::endl;
#elif defined(PLATFORM_SDL)
    for (int i = 0; i < JoysticksFound; i++) {
#if SDL_VERSION_ATLEAST(2, 0, 0)
        if (Joysticks[i].lpDIJoystick != NULL)
#else
        if (SDL_JoystickOpened(i))
#endif
        {
            SDL_JoystickClose(Joysticks[i].lpDIJoystick);
            Joysticks[i].lpDIJoystick = NULL;
        }
    }

    Protokoll << "-> SDL/OpenGL input shutdown successfully completed !" << std::endl;
#endif
}

// --------------------------------------------------------------------------------------
// Tasten abfragen und im Puffer speichern
// --------------------------------------------------------------------------------------

bool DirectInputClass::UpdateTastatur(void) {
#if defined(PLATFORM_DIRECTX)
    HRESULT hresult;

    hresult = lpDIKeyboard->GetDeviceState(sizeof(TastaturPuffer), (LPVOID)&TastaturPuffer);

    if (hresult == DIERR_INPUTLOST)  // Keyboard verloren gegangen ?
    {
        if (lpDIKeyboard)  // Dann nochmal versuchen
        {
            lpDIKeyboard->Acquire();
            hresult = lpDIKeyboard->GetDeviceState(sizeof(TastaturPuffer), (LPVOID)&TastaturPuffer);
        }
    }
#elif defined(PLATFORM_SDL)
    SDL_PumpEvents();
#if defined(ANDROID)
    UpdateTouchscreen();
#endif
#endif
    return true;
}

// --------------------------------------------------------------------------------------
// Maus abfragen
// true  = gepuffert
// false = ungepuffert
// --------------------------------------------------------------------------------------

bool DirectInputClass::UpdateMaus(bool gepuffert) {
#if defined(PLATFORM_DIRECTX)
    return true;

    HRESULT hresult;
    DIMOUSESTATE ms;
    bool fertig = false;

    // Gepufferte Abfrage : Langsam, bearbeitet aber ALLE Maus-Aktionen
    if (gepuffert == true) {
        while (!fertig) {
            DIDEVICEOBJECTDATA od;         // Speichert Mauszustand
            std::uint32_t dwElemente = 1;  // Elemente die abgefragt werden sollen

            hresult = lpDIMaus->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwElemente, 0);
            if (hresult == DIERR_INPUTLOST) {
                hresult = lpDIMaus->Acquire();
                if (hresult != DI_OK) {
                    Protokoll << "\n-> Maus : Re-Acquire Fehler !" << std::endl;
                    return false;
                } else {
                    hresult = lpDIMaus->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwElemente, 0);
                    if (hresult == DIERR_INPUTLOST) {
                        Protokoll << "\n-> Maus : GetData Fehler !" << std::endl;
                        return false;
                    }
                }
            } else if (hresult != DI_OK)  // anderer Fehler ?
            {
                fertig = true;
                return false;
            }

            if (dwElemente == 0)  // Keine Elemente wurden verändert
                fertig = true;

            switch (od.dwOfs)  // Feld 'dwOfs' enthält Maus-Aktion:
            {
                case DIMOFS_X:  // Horizontale Bewegung
                    MausX += od.dwData;
                    fertig = true;
                    break;

                case DIMOFS_Y:  // Vertikale Bewegung
                    MausY += od.dwData;
                    fertig = true;
                    break;

                case DIMOFS_BUTTON0:       // Knopf 0 gedrückt
                    if (od.dwData & 0x80)  // Knopf gedrück
                    {
                        MausButtons[0] = true;
                        fertig = TRUE;
                    } else  // Knopf losgelassen
                    {
                        MausButtons[0] = false;
                        fertig = TRUE;
                    }
                    break;

                case DIMOFS_BUTTON1:       // Knopf 1 gedrückt
                    if (od.dwData & 0x80)  // Knopf gedrück
                    {
                        MausButtons[1] = true;
                        fertig = TRUE;
                    } else  // Knopf losgelassen
                    {
                        MausButtons[1] = false;
                        fertig = TRUE;
                    }
                    break;

                case DIMOFS_BUTTON2:       // Knopf 2 gedrückt
                    if (od.dwData & 0x80)  // Knopf gedrück
                    {
                        MausButtons[2] = true;
                        fertig = TRUE;
                    } else  // Knopf losgelassen
                    {
                        MausButtons[2] = false;
                        fertig = TRUE;
                    }
                    break;
            };
        }
    }

    // Ungepuffert: Schnell, verpasst aber ab und zu Maus-Aktionen:
    else {
        hresult = lpDIMaus->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&ms);
        if (hresult == DIERR_INPUTLOST) {
            hresult = lpDIMaus->Acquire();
            if (hresult != DI_OK) {
                Protokoll << "\n-> Maus : Re-Acquire Fehler !" << std::endl;
                return false;
            } else {
                hresult = lpDIMaus->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&ms);
                if (hresult == DIERR_INPUTLOST) {
                    Protokoll << "\n-> Maus : GetDeviceState Fehler !" << std::endl;
                    return false;
                }
            }
        } else  // anderer Fehler
            if (hresult != DI_OK) {
            return false;
        }

        // Mauskoordinaten angleichen
        MausX += ms.lX;
        MausY += ms.lY;

        // Buttons prüfen
        if (ms.rgbButtons[0] & 0x80)
            MausButtons[0] = true;
        else
            MausButtons[0] = false;
        if (ms.rgbButtons[1] & 0x80)
            MausButtons[1] = true;
        else
            MausButtons[1] = false;
        if (ms.rgbButtons[2] & 0x80)
            MausButtons[2] = true;
        else
            MausButtons[2] = false;
    }

    // Die Maus nicht verschwinden lassen
    if (MausX < 0)
        MausX = 0;
    if (MausY < 0)
        MausY = 0;
    if (MausX > RENDERWIDTH)
        MausX = RENDERWIDTH;
    if (MausY > RENDERHEIGHT)
        MausY = RENDERHEIGHT;

    return true;
#elif defined(PLATFORM_SDL)
    // Position
    uint8_t buttons = SDL_GetMouseState(&MausX, &MausY);

    // Buttons
    if ((buttons & SDL_BUTTON(1)) == SDL_BUTTON(1))
        MausButtons[0] = true;
    else
        MausButtons[1] = false;
    if ((buttons & SDL_BUTTON(2)) == SDL_BUTTON(2))
        MausButtons[1] = true;
    else
        MausButtons[2] = false;
    if ((buttons & SDL_BUTTON(3)) == SDL_BUTTON(3))
        MausButtons[2] = true;
    else
        MausButtons[2] = false;

    // Clamps
    if (MausX < 0)
        MausX = 0;
    if (MausY < 0)
        MausY = 0;
    if (MausX > RENDERWIDTH)
        MausX = RENDERWIDTH;
    if (MausY > RENDERHEIGHT)
        MausY = RENDERHEIGHT;

    return true;
#endif
}

// --------------------------------------------------------------------------------------
// Keyboard wieder akquirieren (wenn Fenster gewechselt zB)
// --------------------------------------------------------------------------------------

void DirectInputClass::AcquireKeyboard(void) {
#if defined(PLATFORM_DIRECTX)
    if (lpDIKeyboard)
        lpDIKeyboard->Acquire();
#endif
}

// --------------------------------------------------------------------------------------
// Alle Joysticks updaten
// --------------------------------------------------------------------------------------

void DirectInputClass::UpdateJoysticks(void) {
    for (int i = 0; i < JoysticksFound; i++)
        Joysticks[i].Update();
}

// --------------------------------------------------------------------------------------
// Checken ob keine Taste mehr gedrückt ist
// --------------------------------------------------------------------------------------

bool DirectInputClass::AreAllKeysReleased() {
    for (int i = 0; i < std::min(NumberOfKeys, MAX_KEYS); i++)
        if (TastaturPuffer[i] != 0)
            return false;

    return true;
}

// --------------------------------------------------------------------------------------
// Checken ob irgendeine Taste gedrückt ist
// --------------------------------------------------------------------------------------

bool DirectInputClass::AnyKeyDown(void) {
    for (int i = 0; i < std::min(NumberOfKeys, MAX_KEYS); i++)
        if (KeyDown(i))
            return true;

    return false;
}

// --------------------------------------------------------------------------------------
// Checken ob irgendein Button gedrückt ist
// --------------------------------------------------------------------------------------

bool DirectInputClass::AnyButtonDown(void) {
    if (JoystickFound == false)
        return false;

    for (int i = 0; i < JoysticksFound; i++)
        for (int j = 0; j < Joysticks[i].NumButtons; j++)
            if (Joysticks[i].JoystickButtons[j])
                return true;

    return false;
}

#if defined(ANDROID)
void DirectInputClass::InitTouchBoxes(int w, int h) {
    int b;
    int index;
    int box_w, box_h;
    int y_offset;

    Width = w;
    Height = h;

    TouchDeviceCount = SDL_GetNumTouchDevices();
    Protokoll << "Finger devices detected: " << TouchDeviceCount << std::endl;

    y_offset = h - (h / 4);
    box_w = (RENDERWIDTH / 9) * ((float)w / (float)RENDERWIDTH);
    box_h = (RENDERHEIGHT / 9) * ((float)h / (float)RENDERHEIGHT);
    TouchBoxes.resize(BOX_RECT_TOTAL);
    TouchBoxMaps.resize(BOX_RECT_TOTAL);
    for (b = 0; b < BOX_RECT_TOTAL; b++) {
        TouchBoxes.at(b).w = box_w;
        TouchBoxes.at(b).h = box_h;
    }

    Protokoll << "box size: " << box_w << " x " << box_h << std::endl;

    // DPAD
    TouchdpadRadius = box_w * 1.5f;
    TouchdpadX = TouchdpadRadius;
    TouchdpadY = y_offset + (box_h / 2);
    // ABXY
    TouchBoxes.at(0).x = w - box_w;
    TouchBoxes.at(0).y = y_offset;
    TouchBoxes.at(1).x = w - (box_w * 2);
    TouchBoxes.at(1).y = y_offset - box_h;
    TouchBoxes.at(2).x = w - (box_w * 2);
    TouchBoxes.at(2).y = y_offset + box_h;
    TouchBoxes.at(3).x = w - (box_w * 3);
    TouchBoxes.at(3).y = y_offset;
    // Side buttons
    TouchBoxes.at(4).x = 0;
    TouchBoxes.at(4).y = h / 4;
    TouchBoxes.at(5).x = 0;
    TouchBoxes.at(5).y = (h / 4) + (box_h) + (box_h / 4);
    TouchBoxes.at(6).x = 0;
    TouchBoxes.at(6).y = (h / 4) + (box_h * 2) + (box_h / 2);
    TouchBoxes.at(7).x = w - box_w;
    TouchBoxes.at(7).y = h / 4;
    TouchBoxes.at(8).x = w - box_w;
    TouchBoxes.at(8).y = (h / 4) + (box_h) + (box_h / 4);
    TouchBoxes.at(9).x = w - box_w;
    TouchBoxes.at(9).y = (h / 4) + (box_h * 2) + (box_h / 2);
    // Shoulders
    TouchBoxes.at(10).x = 0;
    TouchBoxes.at(10).y = 0;
    TouchBoxes.at(11).x = w - box_w;
    TouchBoxes.at(11).y = 0;

    TouchBoxMaps.at(0) = SDLK_a;
    TouchBoxMaps.at(1) = SDLK_b;
    TouchBoxMaps.at(2) = SDLK_c;
    TouchBoxMaps.at(3) = SDLK_d;
    TouchBoxMaps.at(4) = SDLK_e;
    TouchBoxMaps.at(5) = SDLK_f;
    TouchBoxMaps.at(6) = SDLK_g;
    TouchBoxMaps.at(7) = SDLK_h;
    TouchBoxMaps.at(8) = SDLK_i;
    TouchBoxMaps.at(9) = SDLK_j;
    TouchBoxMaps.at(10) = SDLK_ESCAPE;
    TouchBoxMaps.at(11) = SDLK_RETURN;
}

void DirectInputClass::UpdateTouchscreen(void) {
    uint32_t buttons;
    uint8_t box;
    SDL_Finger *finger;
    uint32_t index;
    uint32_t finger_count;
    SDL_Rect touch;
    SDL_Rect dpad;
    SDL_TouchID fingerdevice;

    touch.w = touch.h = 2;

    for (box = 0; box < TouchBoxMaps.size(); box++) {
        TastaturPuffer[SDL_GetScancodeFromKey(TouchBoxMaps.at(box))] = 0;
    }
    TastaturPuffer[SDL_GetScancodeFromKey(SDLK_UP)] = 0;
    TastaturPuffer[SDL_GetScancodeFromKey(SDLK_DOWN)] = 0;
    TastaturPuffer[SDL_GetScancodeFromKey(SDLK_LEFT)] = 0;
    TastaturPuffer[SDL_GetScancodeFromKey(SDLK_RIGHT)] = 0;

    if (TouchDeviceCount > 0) {
        fingerdevice = SDL_GetTouchDevice(0);

        if (fingerdevice != 0) {
            finger_count = SDL_GetNumTouchFingers(fingerdevice);

            for (index = 0; index < finger_count; index++) {
                finger = SDL_GetTouchFinger(fingerdevice, index);
                if (finger != NULL) {
                    touch.x = finger->x * Width;
                    touch.y = finger->y * Height;

                    // Protokoll.WriteText( false, "Finger at %fx%f at %f\n",  finger->x, finger->y,  finger->pressure
                    // );

                    for (box = 0; box < TouchBoxes.size(); box++) {
                        if (CheckRectCollision(&touch, &TouchBoxes.at(box)) == true) {
                            TastaturPuffer[SDL_GetScancodeFromKey(TouchBoxMaps.at(box))] = 1;
                            break;
                            // Protokoll.WriteText( false, "Set %s %s!\n", SDL_GetKeyName(TouchBoxMaps.at(box)),
                            // SDL_GetScancodeName( SDL_GetScancodeFromKey(TouchBoxMaps.at(box)))  );
                        }
                    }

#define OFFSET 15
                    int rel_x = TouchdpadX - touch.x;
                    int rel_y = TouchdpadY - touch.y;
                    if (pow(TouchdpadRadius, 2) >= (pow(abs(rel_x), 2) + pow(abs(rel_y), 2))) {
                        int angle = atan2(-rel_y, -rel_x) / M_PI * 180 + 180;

                        // Protokoll.WriteText( false, "%d x %d angle %d !\n", rel_x, rel_y, angle );
                        if (angle > (45 - OFFSET) && angle < (135 + OFFSET))
                            TastaturPuffer[SDL_GetScancodeFromKey(SDLK_UP)] = 1;

                        if (angle > (135 - OFFSET) && angle < (225 + OFFSET))
                            TastaturPuffer[SDL_GetScancodeFromKey(SDLK_RIGHT)] = 1;

                        if (angle > (225 - OFFSET) && angle < (285 + OFFSET))
                            TastaturPuffer[SDL_GetScancodeFromKey(SDLK_DOWN)] = 1;

                        if ((angle > (285 - OFFSET) && angle <= 0) || (angle >= 0 && (angle < 45 + OFFSET)))
                            TastaturPuffer[SDL_GetScancodeFromKey(SDLK_LEFT)] = 1;
                    }

                } else {
                    Protokoll << "ERROR finger was NULL\n" << std::endl;
                }
            }
        } else {
            Protokoll << "ERROR device is " << std::dec << fingerdevice << std::endl;
        }
    }
}

bool DirectInputClass::CheckRectCollision(SDL_Rect *boxA, SDL_Rect *boxB) {
    // The sides of the SDL_Rects
    int16_t leftA, leftB;
    int16_t rightA, rightB;
    int16_t topA, topB;
    int16_t bottomA, bottomB;

    // Calculate the sides of rec mCollisionbox
    leftA = boxB->x;
    rightA = boxB->x + boxB->w;
    topA = boxB->y;
    bottomA = boxB->y + boxB->h;

    // Calculate the sides of rec box
    leftB = boxA->x;
    rightB = boxA->x + boxA->w;
    topB = boxA->y;
    bottomB = boxA->y + boxA->h;

    // If any of the sides from mCollisionbox are outside of box
    if (bottomA <= topB)
        return false;
    if (topA >= bottomB)
        return false;
    if (rightA <= leftB)
        return false;
    if (leftA >= rightB)
        return false;
    // If none of the sides from mCollisionbox are outside box
    return true;  // Collision has occured
}
#endif

// DKS - GCW Zero handheld-specific button strings
#if defined(GCW)
#define GCW_MAX_BUTTONS 8
static char InternalButtonNumToStringMap[8][10] = {
    "B",  // Button 0
    "A", "Y", "X", "Select", "Start", "L",
    "R"  // Button 7
};
#endif  // GCW

// DKS - Added helper function to facilitate customized naming of joystick buttons and
//      when not customized, to report joy buttons as ranging 1..99 instead of 0..98
char *DirectInputClass::MapButtonToString(int joy_idx, int button) {
    static char buf[60];

    if (button < 0) {
        return TextArray[TEXT_NICHT_DEFINIERT];
    } else {
#ifdef GCW
        // Special case for GCW Zero's internal controls:
        if (joy_idx == GetInternalJoystickIndex() && button < GCW_MAX_BUTTONS) {
            return InternalButtonNumToStringMap[button];
        } else
#endif  // GCW
        // Non-platform-specific generic code for all joysticks:
        {
            // Report button numbers as ranging from 1..99 instead of 0..98
            sprintf_s(buf, "%d", button + 1);
            return buf;
        }
    }
}

// DKS - Convenience function for when in the actual game
#if defined(GCW)
bool DirectInputClass::InternalJoystickMainMenuButtonDown(void) {
    int joy_idx = GetInternalJoystickIndex();
    int button_idx = GetInternalJoystickMainMenuButton();
    if (joy_idx >= 0 && joy_idx < JoysticksFound && Joysticks[joy_idx].Active &&
        button_idx < Joysticks[joy_idx].NumButtons)
        return Joysticks[joy_idx].JoystickButtons[button_idx];
    else
        return false;
}
#endif  // GCW
