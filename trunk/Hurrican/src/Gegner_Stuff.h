#ifndef _GEGNER_STUFF_H_
#define _GEGNER_STUFF_H_

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

//----- Definitionen der Blickrichtungen

#define RECHTS  1
#define LINKS  -1

//----- Gegner/Objekt-Defines

#define EXTRAS					0						// Alle Extras in einer Grafik
#define	ONEUP					1						// Extra-Leben
#define	DIAMANT					2						// Diamant für Punkte und Extralaben
#define	POWERBLOCK				3						// Block mit Extras
#define	SPITTER					4						// Kleiner "Spitter" Gegner
#define	WALKER					5						// Typischer Walker
#define	GUNTOWER				6						// Geschützturm
#define	SPIDERBOMB				7						// Spinnenbombe
#define	PIRANHA					8						// Piranha
#define	STAHLMUECKE				9						// Stahlmücke
#define	DECKENTURM				10						// Geschützturm an der Decke
#define	KUGELKLEIN				11						// Kleine   Stachel-Kugel
#define	KUGELMEDIUM				12						// Mittlere Stachel-Kugel
#define	KUGELGROSS				13						// Große    Stachel-Kugel
#define	KUGELRIESIG				14						// Riesige  Stachel-Kugel
#define	KRABBLEROBEN			15						// Decken Krabbler
#define	KRABBLERLINKS			16						// Wand Krabbler
#define	KRABBLERRECHTS			17						// Wand Krabbler
#define	AUGE					18						// Fliegendes Extra Auge
#define QUALLE					19						// Die kleine Qualle
#define FALLINGROCK				20						// Fallender   Felsblock
#define BROCKELROCK				21						// Bröckelnder Felsblock
#define ROBORAUPE				22						// Robo Raupe
#define STALAGTIT				23						// Stalagtit
#define RAKETENWERFER			24						// Raketenwerfer der auf und zuklappt
#define SWIMWALKER				25						// Schwimmender Walker
#define FLUGSACK				26						// Der fliegende Fettsack =)
#define REITFLUGSACK			27						// Der Fettsack, auf dem der Hurri später reitet
#define EISSTACHEL				28						// Sich drehender Eisstachel
#define SKIWALKER				29						// Walker auf Ski
#define FLUGKANONE				30						// Fliegende Kanone
#define SNOWBOMBE				31						// SchneeBomben Emitter
#define ROBOMAN1				32						// Robo fliegend mit Raketen
#define BOULDER					33						// Runterfallender blauer Stein
#define STELZSACK				34						// Runterfallender StelzSack
#define FALLBOMBE				35						// Fallende Bombe im Fahrstuhl
#define POKENUKE				36						// Riesen Poke Nuke
#define LAVAKRABBE				37						// Anfliegende Lava Krabbe
#define LAVABALLSPAWNER			38						// Lava Ball Spawner, der Lava Bälle erzeugt
#define LAVABALL				39						// Lava Ball
#define FETTESPINNE				40						// Die fiese fette Spinne
#define EIERMANN				41						// Der Spinnen-Eiermann, der Eier legt
#define WANDKANNONE				42						// Kannone an der Wand (Flugsack Level)
#define STARSMALL				43						// Kleiner Stern (Flugsack Level)
#define STARBIG					44						// Großer  Stern (Flugsack Level)
#define MADE					45						// Made, die der Alienboss spuckt
#define DRONE					46						// Drone
#define NEUFISCH				47						// Großer Fisch
#define PFLANZE					48						// Feuerball spuckende Pflanze
#define SPITTERBOMBE			49						// Bombe, aus der der Spitter rauskommt
#define NEST					50						// Wespennest
#define SCHIENENVIECH			51						// Die Schienendrone
#define WASSERMINE				52						// Wassermine
#define FLEDERMAUS				53						// Fledermaus
#define CLIMBSPIDER				54						// Kletterspinne
#define FIRESPIDER				55						// Feuerspinne
#define BALLERDRONE				56						// Die fliegende fette Drone
#define SCHWABBEL				57						// Der Madenkotzende Schwabbel
#define BIGROCKET				58						// Die fette Rakete im Flugsack Level
#define EISZAPFEN				59						// Runterfallender Eiszapfen
#define WANDKRABBE				60						// An der Wand krabbelnde Krabbe
#define JAEGER					61						// Abfangjäger im Flugsacklevel
#define SIDEROCKET				62						// Seitliche Rakete an der Wand
#define FIESEDRONE				63						// Die kleine fiese Drone im Space Level (und anderswo)
#define SCHLEIMBOLLER			64						// Der grüne Schleimbollen
#define SCHLEIMALIEN			65						// Der Schleimbollen im Alien Level
#define SCHLEIMMAUL				66						// Der Schleimbollen mit Maul dran
#define FIESERWALKER			67						// Der fiese Walker mit dem Laser
#define FIESERWALKER2			68						// Der fiese Walker mit dem Flammenwerfer
#define MITTELSPINNE			69						// Mittelgroße Spinne beim Wuxe Spinnenangriff
#define SMALLWESPE				70						// Kleine Wespe
#define DECKENKRABBE			71						// Krabbe an der Decke
#define STAMPFSTEIN				72						// Der stampfende Stein
#define LAVAMANN				73						// Der Lavamann
#define LAFASS					74						// Das Fass
#define GESCHUETZ				75						// Deckengeschuetz
#define FETTERAKETE				76						// Fette Spinnenrakete
#define SCHABE					77						// Wandschabe
#define MINIDRAGON				78						// Mini Drache
#define SCHNEEKOPPE				79						// Schuss des Schneekönigs
#define ZITRONE					80						// Die Zieh Drone
#define ROTZPOTT				81						// Rotzi
#define STACHELBEERE			82						// Die aufklappende Stachelbeere
#define MINIROCKET				83						// Kleine Rakete der Stachelbeere
#define MUTANT					84						// Der rumspringende Mutant aus der Röhre

#define PUNISHER				99						// Der Punisher, der kommt, wenn der Spieler keine Zeit mehr hat

// Viertel Bosse
#define RIESENPIRANHA			100						// Der riesige grüne Piranha
#define RIESENQUALLE			101						// Die riesige fette Qualle
#define RIESENRAUPE				102						// Die riesige Robo Raupe
#define RIESENWASP				103						// Die riesige Robo Wespe

// Ziwschen und Endbosse
#define	STAHLFAUST				120						// Riesige  Stahlfaust
#define	PHARAOKOPF				121						// Riesiger Pharao Kopf
#define RIESENSPINNE			122						// Die fiese ecklige Riesenspinne
#define EVILHURRI				123						// Böser Hurrican
#define UFO						124						// Das Ufo
#define FAHRSTUHLBOSS			125						// Boss im Fahrstuhl
#define FLUGBOSS				126						// Boss im Flugsack Level
#define BRATKLOPS				127						// Boss im Alien Level
#define GEGNERSTURM				128						// Ansturm von vielen fiesen Gegnern
#define METALHEAD				129						// Roboter Metall Kopf
#define EISFAUST				130						// Stahlfaust Boss im Eis
#define WUXESPINNEN				131						// Spinnenansturm vom Wuxe
#define GOLEM					132						// Golem Boss im Lava Level
#define SPINNENMASCHINE			133						// Spinnenmaschine in der Spiderfactory
#define DRACHE					134						// Der Drache im Turmlevel
#define ROLLMOPS				135						// Der Rollmops im Eislevel
#define SCHNEEKOENIG			136						// Der Rollmops im Eislevel
#define BIGFISH					137						// Der riesen Piranha
#define SKELETOR				138						// Skeletor
#define THEWALL					139						// Die Endbosswand

// Trigger und andere Objekte
#define PARTIKELSPAWN			140						// Partikel-Erzeuger (Regen, Schnee etc)
#define	FAHRSTUHL				141						// Fahrstuhl
#define FASS					142						// Fass explodiert bei Schuss in Splitter
#define FEUERSPUCKER			143						// Erzeugt die Feuerfalle
#define FADEMUSIC				144						// Music Fade Trigger
#define ENDLEVEL				145						// Level Ende Position
#define PLATTFORM				146						// Fliegende Plattform
#define PRESSE					147						// Stahlpresse
#define SHRINE					148						// Schrein mit Manfred drin =)
#define BRUECKE					149						// Hängebrücke
#define FLOATING				150						// kleine schwebe Plattform
#define FLOATING2				151						// mittlere schwebe Plattform
#define FLOATING3				152						// große schwebe Plattform

#define SURFBRETT				153						// Surfbrett Gegner =)
#define SHOOTBUTTON				154						// Schwebe Plattform Button (wird im Editor nicht gesetzt, nur vom Game)
#define SHOOTPLATTFORM			155						// Schwebe Plattform (durch Button aufzulösen)
#define GLUBSCHI				156						// Glubschauge, das dem Hurri hinterherkuckt
#define GLUBSCHI2				157						// Glubschauge von der Decke
#define COLUMN					158						// Die Säule, die umfällt, und das ganze Level schräg stellt
#define LIFT					159						// Lift mit Countdown, der explodiert
#define COLUMN2					160						// Die Säule, die umfällt, und das ganze Level wieder aufstellt
#define SCHLEUSEH				161						// Horizontale Schleuse
#define SCHLEUSEV				162						// Vertikale Schleuse
#define SWITCH					163						// Schalter (zum Türen öffnen)
#define WARNING					164						// Warnungs Schild
#define SOUNDTRIGGER			165						// Sound Trigger
#define LUEFTER_GROSS			166						// Großer Luefter
#define LUEFTER_KLEIN			167						// Kleiner Leufter 1
#define LUEFTER_KLEIN2			168						// Kleiner Leufter 2
#define TUTORIALTEXT			169						// Textbox für Tutorial
#define LIGHTFLARE				170						// Wie der Name schon sagt ;)
#define SECRET					171						// Secret Area im Level
#define MUSHROOM				172						// Pilzkopf-Trampolin =)
#define PRESSWURST				173						// Die Spinnen Presswurst
#define LAFASSSPAWNER			174						// Der Trigger für das Fass, das an der Decke entlang fährt
#define SPIKELIFT				175						// Lift mit Stacheln
#define TUBE					176						// Glasröhre

// Objekte, die nicht im Editor auftauchen
#define KETTENGLIED				190						// Kettenglied z.B. vom Rollmops
#define SKULL					191						// Schädel vom Skeletor

//----- Handlungen der Gegner

#define GEGNER_STEHEN					0				
#define GEGNER_LAUFEN					1
#define GEGNER_LAUFEN2					2
#define GEGNER_LAUFEN3					3
#define GEGNER_LAUFEN4					4
#define GEGNER_SCHIESSEN				5
#define GEGNER_SPRINGEN					6
#define GEGNER_FALLEN					7
#define GEGNER_WATSCHELN				8				// Für Walker
#define GEGNER_DREHEN					9				// Für Geschützturm
#define GEGNER_DREHEN2					10				// Für Stelzsack
#define GEGNER_VERFOLGEN				11				// Für Stahlmücke
#define GEGNER_UNVERWUNDBAR		 		12				// Für Deckenturm
#define GEGNER_OEFFNEN			 		13				// Für Deckenturm nochmal
#define GEGNER_SCHLIESSEN			 	14				// Für Deckenturm (einmal noch :P )
#define GEGNER_NOTVISIBLE				99

// Zwischenbosse

#define GEGNER_INIT			 			30				// Warten bis der Screen zentriert wurde
#define GEGNER_EINFLIEGEN		 		31				// Gegner erscheint am Screen
#define GEGNER_CRUSHEN			 		32				// Stahlfaust/Pharao zerquetscht den Hurri
#define GEGNER_CRUSHEN2			 		33				// Stahlfaust/Pharao zerquetscht den Hurri
#define GEGNER_CRUSHENERHOLEN	 		34				// Stahlfaust fliegt nach dem Crushen hoch
#define GEGNER_SPECIAL			 		35				// Special Move  (Pharao Steine rieseln lassen etc)
#define GEGNER_SPECIAL2			 		36				// Special Move2 (Bratklops Fettboller usw)
#define GEGNER_SPECIAL3			 		37				// Special Move3 (Bratklops Laser usw)
#define GEGNER_ABSENKEN					38				// Spinne senkt sich ab
#define GEGNER_ABSENKENZWEI				39				// Spinne senkt sich ab und schiesst fetten laser
#define GEGNER_AUFRICHTEN				40				// Spinne richtet sich wieder auf
#define GEGNER_AUFRICHTENZWEI			41				// Spinne richtet sich wieder auf und schiesst fetten laser =)
#define GEGNER_AUSSPUCKEN				42				// Spinne spuckt kleine Spinnen Bomben aus
#define GEGNER_AUSSPUCKENZWEI			43
#define GEGNER_BOMBARDIEREN				44				// Spinne wirft Granaten

#define GEGNER_INIT2					45				
#define GEGNER_INIT3					46				
#define GEGNER_INIT4					47

#define GEGNER_EINFAHREN				48
#define GEGNER_AUSFAHREN				49
#define GEGNER_EINFLIEGEN2		 		50
#define GEGNER_WARTEN					51

#define GEGNER_LAUFEN_LINKS				52
#define GEGNER_LAUFEN_RECHTS			53

#define GEGNER_LAUFEN_LINKS2			54
#define GEGNER_LAUFEN_RECHTS2			55

#define GEGNER_AUSWAHL					56

#define GEGNER_EXPLODIEREN		 		80				// Endboss fliegt in die Luft

//----- Sonstiges

#define MAX_GEGNER				2048					// Maximale Zahl Gegner pro Level
#define MAX_GEGNERGFX			200						// Maximale Grafiken der Gegner

//----- Externals

extern GegnerListClass		*pGegner;
extern DirectGraphicsSprite	*pGegnerGrafix[MAX_GEGNERGFX];		// Grafiken der Gegner
extern RECT					 GegnerRect	  [MAX_GEGNERGFX];		// Rechtecke für Kollision	
extern float				 g_Fahrstuhl_yPos;					// yPosition des Fahrstuhls
extern float				 g_Fahrstuhl_Offset;				// Scrollposition relativ zum Farhstuhl
extern float				 g_Fahrstuhl_Speed;					// Fahrstuhl Geschwindigkeit


#endif
