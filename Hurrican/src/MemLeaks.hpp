/*// Datei : MemLeaks.hpp

// -------------------------------------------------------------------------------------- 
//
// Überlädt den "new" und den "delete" Operator
// New speichert zudem jeden Aufruf mit Angabe der Datei und der Zeilen Nummer im Code in
// einer Liste.
// Delete löscht diesen Eintrag wieder.
// Wird also ein New ohne dazugehöriges Delete aufgerufen, wird diese Zeile nicht vermerkt
// und bei Programmende im Debugfenster ausgegeben.
// Auf diese Weise werden Memory Leaks mit Dateinamen und Zeilennummern gefunden.
//
// (c) 2004 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _MEMLEAKS_HPP_
#define _MEMLEAKS_HPP_

#include <windows.h>
#include <stdlib.h>


// Struktur, die Infos zu einem New-Aufruf entählt
//
typedef struct 
{
	std::uint32_t	address;
	std::uint32_t	size;
    char	file[64];
    std::uint32_t	line;
} ALLOC_INFO;

// Funktionen zum hinzufügen und entfernen von allokiertem Speicher
//
typedef list<ALLOC_INFO*> AllocList;

AllocList *allocList;

void AddTrack(std::uint32_t addr,  std::uint32_t asize,  const char *fname, std::uint32_t lnum)
{
	ALLOC_INFO *info;

    if(!allocList) 
	{
		allocList = new(AllocList);
    }

    info = new(ALLOC_INFO);
    info->address = addr;
    strncpy(info->file, fname, 63);
    info->line = lnum;
    info->size = asize;
    allocList->insert(allocList->begin(), info);
};

void RemoveTrack(std::uint32_t addr)
{
	AllocList::iterator i;

    if(!allocList)
		return;

    for(i = allocList->begin(); i != allocList->end(); i++)
    {
	    if((*i)->address == addr)
	    {
			allocList->remove((*i));
			break;
		}
	}
};

void DumpUnfreed()
{
	AllocList::iterator i;
    std::uint32_t totalSize = 0;
    char buf[1024];

    if(!allocList)
		return;

    for(i = allocList->begin(); i != allocList->end(); i++) 
	{
		sprintf(buf, "%-50s:\t\tLINE %d,\t\tADDRESS %d\t%d unfreed\n", 
				(*i)->file, (*i)->line, (*i)->address, (*i)->size);
		OutputDebugString(buf);
		totalSize += (*i)->size;
	}

	sprintf(buf, "-----------------------------------------------------------\n");
	OutputDebugString(buf);
	sprintf(buf, "Total Unfreed: %d bytes\n", totalSize);
	OutputDebugString(buf);
};

// Operatoren überladen
//

#ifdef _DEBUG

	// new überladen
	//
	inline void * __cdecl operator new(unsigned int size, const char *file, int line)
	{
		void *ptr = (void *)malloc(size);
		AddTrack((std::uint32_t)ptr, size, file, line);
		return(ptr);
    };

	// delete überladen
	//
    inline void __cdecl operator delete(void *p)
	{
		RemoveTrack((std::uint32_t)p);
		free(p);
	};
#endif


// Für den Debug Modus wird ein neuer new Befehl generiert, der zusätzlich noch Dateiname und Zeilennummer
// der Codezeile ausgibt, die die new funktion aufruft.
#ifdef _DEBUG
#define DEBUG_NEW new(__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif

// normalen new aufruf durch DEBUG_NEW ersetzen
#define new DEBUG_NEW

#endif
*/
