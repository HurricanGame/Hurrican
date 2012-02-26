// Datei : CCracktro.cpp

// -------------------------------------------------------------------------------------- 
//
// Klasse für das Cracktro am Anfang
//
// (c) 2004 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include <stdio.h>
#include "CCracktro.h"
#include "DX8Input.h"
#include "DX8Sound.h"

char CrackText	[] = {	"                                              "
						"yeehaaw! poke is back with another fine release of a "
						"retro-remake! this time, its -hurrican-, a remake of the great "
						"classic turrican made by manfred trenz, chris huelsbeck, andreas escher,"
						" jeroen tel, peter thierolf, ramiro vaca, markus siebold, julian eggebrecht,"
						" celal kandemiroglu and many other people. it took us a loooong time to crack "
						"this damn game, but at least it's here. hope you like it... "						
						"for updates and more visit www.hurrican-game.de or www.poke53280.de "
						"and now have fun with this game!"
						"                                               "
						""};

char StaticText	[] = {	"                                   "
						"poke53280 proudly presents -hurrican- cracked on 05-31-2007 by eiswuxe and turri. "
						"greetings fly to all turrican fans worldwide. "
						"visit www.hurrican-game.de for more infos, updates, downloads. check out the "
						"create section to learn how to make your own hurrican levels and add-ons! "
						"we hope you like this game!"
						"                                     "};

char BlinkText	[][30] = {"poke53280",
						  "brings you",
						  "-hurrican-",
						  "cracked on",
						  "05-31-2007",
						  "always remember",
						  "shoot...",
						  "or die!"
						  };

D3DCOLOR ScrollCol[] = {0xFF000088, 0xFF000088,
						0xFF0000AA, 0xFF0000AA,
						0xFF0000FF, 0xFF0000FF,
						0xFF0000FF, 0xFF0000FF,
						0xFF0088FF, 0xFF0088FF, 
						0xFF0088FF, 0xFF0088FF, 
						0xFFAAFFEE, 0xFFAAFFEE, 
						0xFFAAFFEE, 0xFFAAFFEE, 
						0xFFEEEE77, 
						0xFFFFFFFF, 0xFFFFFFFF,
						0xFFEEEE77, 
						0xFFAAFFEE, 0xFFAAFFEE, 
						0xFFAAFFEE, 0xFFAAFFEE,
						0xFF0088FF, 0xFF0088FF, 
						0xFF0088FF, 0xFF0088FF,
						0xFF0000FF, 0xFF0000FF,
						0xFF0000FF, 0xFF0000FF,
						0xFF000088, 0xFF000088,
						0xFF000088, 0xFF000088};

D3DCOLOR BlinkCol[] = {	0xFF664400, 
						0xFF664400, 
						0xFF880000, 
						0xFF880000, 
						0xFFCC44CC,
						0xFFCC44CC,
						0xFFDD8855,
						0xFFDD8855, 
						0xFFEEEE77,
						0xFFEEEE77,
						0xFFFFFFFF,
						0xFFFFFFFF,
						0xFFEEEE77,
						0xFFEEEE77,
						0xFFDD8855,
						0xFFDD8855,
						0xFFCC44CC,
						0xFFCC44CC,
						0xFF880000,
						0xFF880000,
						0xFF000000,
						0xFF000000,
						0xFF333333,
						0xFF333333,
						0xFF777777,
						0xFF777777,
						0xFFBBBBBB,
						0xFFBBBBBB,
						0xFFAAFF66,
						0xFFAAFF66,
						0xFFEEEE77,
						0xFFEEEE77,
						0xFFFFFFFF,
						0xFFFFFFFF,
						0xFFEEEE77,
						0xFFEEEE77,
						0xFFAAFF66,
						0xFFAAFF66,
						0xFFBBBBBB,
						0xFFBBBBBB,
						0xFF777777,
						0xFF777777,
						0xFF333333,
						0xFF333333,
						0xFF000000,
						0xFF000000};


// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

CCracktro::CCracktro()
{	
	b_running = true;
	pFont = new(DirectGraphicsFont);
	pFont->LoadFont("demofont.bmp", 288, 256, 18, 16, 16, 8);	

	Bars[0].LoadImage("copper1.bmp", 32, 36, 32, 36, 1, 1);
	Bars[1].LoadImage("copper2.bmp", 32, 18, 32, 18, 1, 1);
	Bars[2].LoadImage("copper3.bmp", 32, 100, 32, 100, 1, 1);

	Zahlen.LoadImage("demozahlen.bmp", 160, 40, 32, 40, 5, 1);

	Logo[0].LoadImage("demologo.bmp", 341, 80, 341, 80, 1, 1);

	Star.LoadImage("Star.bmp", 5, 5, 5, 5, 1, 1);

	ScrollOffset  = 0.0f;
	ScrollOffset2 = 0.0f;
	ScrollPos  = 0;
	ScrollPos2 = 0;
	SinPos = 0.0f;

	for (int i = 0; i < 150; i++)
	{
		Stars[i].Count   = (float)(rand()%640);
		Stars[i].Abstand = (float)(rand()%140 + 340);
		Stars[i].Ebene   = rand()%200 + 55;
	}

	pSoundManager->LoadSong("Cracktro.it", MUSIC_CRACKTRO);		
	pSoundManager->PlaySong(MUSIC_CRACKTRO, false);
	pSoundManager->SetAbsoluteSongVolume(MUSIC_CRACKTRO, 255);	

	DirectGraphics.SetAdditiveMode();
	DirectGraphics.SetColorKeyMode();

	State = 0;

} // Konstruktor

// --------------------------------------------------------------------------------------
// Destruktor
// --------------------------------------------------------------------------------------

CCracktro::~CCracktro()
{	
	delete (pFont);
} // Destruktor

// --------------------------------------------------------------------------------------
// Hauptteil
// --------------------------------------------------------------------------------------

void CCracktro::Main(void)
{
	lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,	D3DCOLOR_XRGB(0,0,0), 1.0f, 0);

	int i;

	// --------------------------------------------------------------------------------------
	// Sterne
	// --------------------------------------------------------------------------------------

	for (int i = 0; i < 200; i++)
	{
		Stars[i].Count -= Stars[i].Ebene * 0.1f SYNC;

		if (Stars[i].Count < 0.0f)
			Stars[i].Count += 640;

		RenderRect(Stars[i].Count, Stars[i].Abstand, 4, 2, D3DCOLOR_RGBA(255, 255, 255, Stars[i].Ebene));
	}

	// --------------------------------------------------------------------------------------
	// Sinus Scroller
	// --------------------------------------------------------------------------------------
	float xchar = 0;
	float s;	
	static float colpos = 0.0f;
	static float LogoPos = 0.0f;

	s = SinPos;
	s =(float)(sin(s)) * 80.0f;
	s = (float)(abs((int)s));

	colpos += 6.0f SYNC;
	
	for(i = 0; i < 43; i++)
	{				
		pFont->DrawDemoChar(xchar - ScrollOffset, (float)(450 - s), CrackText[i + ScrollPos], ScrollCol[(int)(colpos + i) % (sizeof(ScrollCol) / sizeof(D3DCOLOR))]);

		if (CrackText[i + ScrollPos] != 32)
			xchar += pFont->mCharLength[CrackText[i + ScrollPos] - 33] + 2;
		else
			xchar += pFont->mXCharSize;
	}	

	ScrollOffset += 12.0f SYNC;

	int l;
	if (CrackText[ScrollPos] == 32)
		l = pFont->mXCharSize;
	else
		l = pFont->mCharLength[CrackText[ScrollPos] - 33] + 2;

	if (ScrollOffset > l)
	{
		ScrollOffset -= l;
		ScrollPos++;

		if ((unsigned int)ScrollPos > strlen(CrackText) - 50)
			ScrollPos = 0;
	}

	SinPos += 0.3f SYNC;
	if (SinPos > 2 * PI)
		SinPos -= 2 * PI;

	LogoPos += 0.2f SYNC;
	if (LogoPos > 2 * PI)
		LogoPos -= 2 * PI;

	// --------------------------------------------------------------------------------------
	// Logo (hinter Copper Bars)
	// --------------------------------------------------------------------------------------

	static float numsin = 0.0f;

	if (SinPos > 0.7f && SinPos < PI + 1.0f)
	{
		DirectGraphics.SetColorKeyMode();
		Logo[0].RenderSprite((640 - 341) / 2 + (float)(sin(LogoPos) * 100.0f), 50, 0, 0xFFFFFFFF);
	
		numsin += 0.4f SYNC;

		if (numsin > 2 * PI)
			numsin -= 2 * PI;

		float off = 0;
		float off2 = 0;

		// Zahlen
		for (int i = 0; i < 5; i++)
		{
			off  = (float)(sin(numsin + i / 3.0f) * 10.0f);
			off2 = (float)(sin(numsin + i / 3.0f + PI / 2) * 20.0f);

			Zahlen.RenderSprite(75 + i * 40 - off + (640 - 341) / 2 + (float)(sin(LogoPos) * 100.0f), 
								110 - off2, i, 0xFFFFFFFF);
		}
	}

	// --------------------------------------------------------------------------------------
	// Copper Balken
	// --------------------------------------------------------------------------------------

	for(i = 0; i < 7; i++)
		Bars[1].RenderSpriteScaled(0, 95 - (float)(sin(SinPos + i / 5.0f) * 90.0f), 640, 6 + i * 3, 0, 0xFFFFFFFF);

	Bars[2].RenderSpriteScaled(0, 255, 640, 100, 0, 0xFFFFFFFF);

	// --------------------------------------------------------------------------------------
	// Scroller 2
	// --------------------------------------------------------------------------------------
	xchar = 0;
	
	for(i = 0; i < 50; i++)
	{				
		pFont->DrawDemoChar(xchar - ScrollOffset2, 295, StaticText[i + ScrollPos2], 0xFF000000);

		if (StaticText[i + ScrollPos2] != 32)
			xchar += pFont->mCharLength[StaticText[i + ScrollPos2] - 33] + 2;
		else
			xchar += pFont->mXCharSize;
	}	

	if (StaticText[ScrollPos2] == 32)
		l = pFont->mXCharSize;
	else
		l = pFont->mCharLength[StaticText[ScrollPos2] - 33] + 2;

	ScrollOffset2 += 10.0f SYNC;

	if (ScrollOffset2 > l)
	{
		ScrollOffset2 -= l;
		ScrollPos2++;

		if ((unsigned int)ScrollPos2 > strlen(StaticText) - 50)
			ScrollPos2 = 0;
	}

	// --------------------------------------------------------------------------------------
	// Logo (vor Copper Bars)
	// --------------------------------------------------------------------------------------

	if (!(SinPos > 0.7f && SinPos < PI + 1.0f))
	{
		DirectGraphics.SetColorKeyMode();
		Logo[0].RenderSprite((640 - 341) / 2 + (float)(sin(LogoPos) * 100.0f), 50, 0, 0xFFFFFFFF);

		numsin += 0.4f SYNC;

		if (numsin > 2 * PI)
			numsin -= 2 * PI;

		float off = 0;
		float off2 = 0;

		// Zahlen
		for (int i = 0; i < 5; i++)
		{
			off  = (float)(sin(numsin + i / 3.0f) * 10.0f);
			off2 = (float)(sin(numsin + i / 3.0f + PI / 2) * 20.0f);

			Zahlen.RenderSprite(75 + i * 40 - off + (640 - 341) / 2 + (float)(sin(LogoPos) * 100.0f), 
								110 - off2, i, 0xFFFFFFFF);
		}
	}
	
	// --------------------------------------------------------------------------------------
	// Static Blink Text
	// --------------------------------------------------------------------------------------

	static float blinkpos = 0.0f;
	static int   fontoff = 0;

	blinkpos += 5.0f SYNC;

	fontoff = (int)(blinkpos / 100.0f);

	if (fontoff >= (sizeof(BlinkText) / sizeof(BlinkText[0])))
	{
		blinkpos = 0.0f;
		fontoff = 0;
	}

	float    yo = 220 - (float)(cos(LogoPos) * 10.0f);
	D3DCOLOR col = BlinkCol[(int)(blinkpos) % (sizeof(BlinkCol) / sizeof(D3DCOLOR))];

	RenderRect(0, yo, 640, 2, col);
	RenderRect(0, yo + 20.0f, 640, 2, col);

	pFont->DrawDemoText((float)(-sin(LogoPos) * 140.0f) +
						(float)(626 - pFont->DemoStringLength(BlinkText[fontoff])) / 2.0f, yo + 2.0f, 
						BlinkText[fontoff],
						col);	

	// --------------------------------------------------------------------------------------
	// Kringel
	// --------------------------------------------------------------------------------------

	for (int i = 0; i < 16; i++)
	{
		RenderRect((float)(-sin(LogoPos) * 140.0f) +
				   320 +	 (float)(sin(SinPos + i * (2 * PI / 16)) * 140 ), 
				   yo + 10 + (float)(cos(SinPos + i * (2 * PI / 16)) * 20), 4, 2, 0xFFFFFFFF);
	}



	// beenden
	//
	if (DirectInput.AnyKeyDown() ||
		DirectInput.AnyButtonDown())
	{
		pSoundManager->StopSong(MUSIC_CRACKTRO, false);
		State = 1;		
	}	
}

// --------------------------------------------------------------------------------------
// Ladebalken
// --------------------------------------------------------------------------------------

void CCracktro::Load(void)
{
	static float count = 10.0f;

	// farbige Balken
	for (int i = 0; i < 320; i++)
		RenderRect(0,   (float)(i * 2), 
				   640, (float)(i * 2), ScrollCol[rand()%(int(sizeof(ScrollCol) / sizeof(D3DCOLOR)))]);

	count -= 1.0f SYNC;

	if (count <= 0.0f)
		b_running = false;
}

// --------------------------------------------------------------------------------------
// Laufen lassen
// --------------------------------------------------------------------------------------

void CCracktro::Run (void)
{	
	switch (State)
	{
	case 0 : Main();
		break;

	case 1 : Load();
		break;

	default : Main();
		break;
	}
} // RunCracktro