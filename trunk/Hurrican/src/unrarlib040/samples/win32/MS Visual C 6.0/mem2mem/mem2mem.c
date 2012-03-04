/* ***************************************************************************
 **
 **  This file is part of the UniquE RAR File Library.
 **
 **  Copyright (C) 2000-2002 by Christian Scheurer (www.ChristianScheurer.ch)
 **  UNIX port copyright (c) 2000-2002 by Johannes Winkelmann (jw@tks6.net)
 ** 
 **  The contents of this file are subject to the UniquE RAR File Library
 **  License (the "unrarlib-license.txt"). You may not use this file except 
 **  in compliance with the License. You may obtain a copy of the License
 **  at http://www.unrarlib.org/license.html.
 **  Software distributed under the License is distributed on an "AS IS"
 **  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied warranty.
 ** 
 **  Alternatively, the contents of this file may be used under the terms
 **  of the GNU General Public License Version 2 or later (the "GPL"), in
 **  which case the provisions of the GPL are applicable instead of those
 **  above. If you wish to allow use of your version of this file only
 **  under the terms of the GPL and not to allow others to use your version
 **  of this file under the terms of the UniquE RAR File Library License,
 **  indicate your decision by deleting the provisions above and replace
 **  them with the notice and other provisions required by the GPL. If you
 **  do not delete the provisions above, a recipient may use your version
 **  of this file under the terms of the GPL or the UniquE RAR File Library
 **  License.
 ** 
 ************************************************************************** */
 
/* ***************************************************************************
 * Program    : mem2mem.c
 * Function   : Example for the usage of the UniquE RAR File Library
 *              Decompress an archive from memory to memory. This is useful
 *              if you want to include compressed resources into your program
 *              or if you want to decompress a rar archive without write 
 *              access to any disk device.
 *
 *              *****************************************************
 *              *ooooooooooooooooooooooooooooooooooooooooooooooooooo*
 *              *o NOTE: read unrarlib.c for more details and use  o*
 *              *o unrarlib.h to configure the lib.                o*
 *              *ooooooooooooooooooooooooooooooooooooooooooooooooooo*
 *              *****************************************************
 *
 * Author     : Christian Scheurer aka. UniquE (cs@unrarlib.org)
 *              http://www.unrarlib.org
 *              http://www.christianscheurer.ch
 * Version    : 
 * 0.1    10.05.2002    CS     initial version, based on "example.c" of v0.3.4
 *
 ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unrarlib.h>                       /* include the unrarlib         */

#include <windows.h>
#include "resource.h"                       /* include the resources if you */
                                            /* want to link RAR files into  */
                                            /* the .EXE using MS VisualC    */

void openRARResource(MemoryFile *memfile, int resource_id);

int main ()
{
  char            *data_ptr;                /* pointers for the decompressed*/
                                            /* data                         */
  unsigned long   data_size;                /* size of the decompressed     */
                                            /* data                         */
  int             ch;                       /* for "please press ENTER"     */
  ArchiveList_struct *List = NULL;          /* structure with file infos    */
  MemoryFile      rarfile;                  /* file in memory, used for mem-*/
                                            /* to-mem decompression         */

  
  printf("\n Demonstration of the UniquE RAR File Library\n");
  printf(  " ~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~\n");

  /*** "open" memory file                                                   */
  openRARResource(&rarfile, IDR_RARFILE_BEST_RAR);

  /*** extract files for RAR archive                                        */
  printf("  o extracting 'best.rar:\\cryptdir\\File_id.diz'...");
  if(urarlib_get(&data_ptr, &data_size, "cryptdir\\File_id.diz", 
                 &rarfile, "password")) 
  {
    printf("OK.\n");
    
    printf("\n *** Dump of the decompressed file ***\n");
    realloc(data_ptr, data_size + 1);
    data_ptr[data_size] = 0x00;
    printf("%s", data_ptr);
    printf("*** End of dump ***\n");

    if(data_ptr != NULL) free(data_ptr);    /* free memory                  */
  } else 
  { 
    printf("Error!\n"); 
  }


  printf("\n (C) 2000-2002 by Christian Scheurer aka. UniquE of Vantage"\
         "\n http://www.unrarlib.org and http://www.vantage.ch"\
         "\n Linux port by Johannes Winkelmann (jw@tks6.net)\n\n");


  printf("\n End of decompression. Please press [ENTER]...");
  ch = getchar();
  printf("\n");

  return 0;
}


void openRARResource(MemoryFile *memfile, int resource_id)
/* get pointer to RAR file in memory which is linked as resource file       */
/* This works fine for MS VisualC 6.0, you may have to change this method   */
/* for other compilers and operating systems. Please give me your versions  */
/* for gcc, BC and other compilers, so I can distribute them with the next  */
/* release.                                                                 */
{
  HRSRC           hsResourceFH;             /* needed to load resources     */
  HGLOBAL         hgResourceLH;
  HINSTANCE	      hInstance  = GetModuleHandleA(NULL); 

  memfile->data   = 0;                      /* init with NULL values        */
  memfile->size   = 0;
  memfile->offset = 0;

  hsResourceFH = FindResource(hInstance, MAKEINTRESOURCE(resource_id), "RARFILE");
  if(hsResourceFH)
  {
    hgResourceLH = LoadResource(hInstance, hsResourceFH);
    if(hgResourceLH)
    {
       memfile->size = SizeofResource(hInstance, hsResourceFH);
       memfile->data = (void*)LockResource(hgResourceLH);
    }
  } 

}

