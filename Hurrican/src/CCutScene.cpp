// Datei : CCutScene.cpp

// --------------------------------------------------------------------------------------
//
// Klasse für die CutScenes zwischen zwei Stages
//
// (c) 2004 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// DKS - This code was never used in the original game, so I've commented it out to make
//      that clear:
#if 0

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "CCutScene.h"
#include "DX8Input.hpp"



// --------------------------------------------------------------------------------------
// Sonstige Funktionen für die CutScenes
// --------------------------------------------------------------------------------------

void PlayCutScene (int nr)
{
    CCutScene *pScene;				// Die CutScene, die wir sehen wollen

    // je nach Nummer eine andere CutScene Instanz erzeugen
    //
    switch (nr)
    {
    case 1 :
        pScene = new CCutScene1();
        break;

    } // switch

    // Warten, bis alle Tasten, die die CutScene abbrechen könnten, losgelassen wurden
    //
    do
    {
        DirectInput.UpdateTastatur ();

    }
    while (KeyDown(DIK_ESCAPE));

    // CutScene abspielen
    //
    do
    {
        // CutScene rendern
        //
        pScene->RunCutScene();
        DirectGraphics.ShowBackBuffer();

        // Keyboard abfragen
        //
        DirectInput.UpdateTastatur ();

        // Abbruch ?
        //
        if (KeyDown(DIK_ESCAPE))
            pScene->m_iFading = 1;

        Timer.update();
        SpeedFaktor = Timer.SpeedFaktor;
        Timer.wait();

    }
    while (pScene->m_bIsDone == false);

    // und CutScene wieder löschen
    //
    delete (pScene);
}

// --------------------------------------------------------------------------------------
// CutSCene-Klasse
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

CCutScene::CCutScene()
{
    m_bIsDone = false;
    m_fFadeAlpha = 255.0f;
    m_iFading    = - 1;
    m_fYScroll   = - 16.0f;

} // Konstruktor

// --------------------------------------------------------------------------------------
// Destruktor
// --------------------------------------------------------------------------------------

CCutScene::~CCutScene()
{
} // Destruktor



// --------------------------------------------------------------------------------------
// Die einzelnen CutScenes sind alles von der Hauptklasse "CCutScene" abgeleitete
// Unterklassen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// CutScene1
// --------------------------------------------------------------------------------------

CCutScene1::CCutScene1 ()
{
    BackGround = new DirectGraphicsSprite ();
    //DKS - Corrected Y frame-count from 0 to 1 here:
    BackGround->LoadImage ("static_jungle.png", 640, 480, 640, 480, 1, 1);

} // CCutScene1 Konstruktor

// --------------------------------------------------------------------------------------

void CCutScene1::RunCutScene ()
{
    //static float yscroll = 0.0f; // PICKLE not used

    BackGround->RenderSprite (0, 0, 0xFFFFFFFF);

    for (int i = 0; i < 15; i++)
    {
        //int   a;
        //a = 255;

        // CutScene Text anzeigen, sofern ausgefüllt
//		if (strcmp (TextArray [TEXT_CUTSCENE1_1 + i], "-") != 0)
//			pDefaultFont->DrawText (20, 480 - m_fYScroll + i * 12, TextArray [TEXT_CUTSCENE1_1 + i], D3DCOLOR_RGBA (255, 255, 255, a));
    }

    // Schwarzes Rechteck zum Faden anzeigen?
    //
    if (m_iFading != 0)
    {
        m_fFadeAlpha += 20.0f * m_iFading SYNC;

        int a = int (m_fFadeAlpha);

        // Stop Fading
        if (m_iFading == -1 && a < 0)
            m_iFading = 0;


        // Stop Scene
        else if (m_iFading ==  1 && a > 255)
            m_bIsDone = true;

        else
            RenderRect (0, 0, 640, 480, D3DCOLOR_RGBA (0, 0, 0, a));
    }

    m_fYScroll += 1.0f SYNC;

} // RunCutScene

// --------------------------------------------------------------------------------------

CCutScene1::~CCutScene1 ()
{
    delete (BackGround);

} // CCutScene1 Destruktor

#endif  // 0
