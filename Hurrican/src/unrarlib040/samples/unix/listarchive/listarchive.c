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
 * Program    : listarchive.c
 * Function   : Example for the usage of the UniquE RAR File Library
 *              Print a list of all files of an archive
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

void print_list(ArchiveList_struct *list);  /* functions used within example*/


int main ()
{
  int             ch;                       /* for "please press ENTER"     */
  int             filecounter = 0;          /* used for urarlib_list()      */
  ArchiveList_struct *List = NULL;          /* structure with file infos    */


  printf("\n Demonstration of the UniquE RAR File Library\n");
  printf(  " ~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~\n");

  /*** list files from RAR archive                                          */
  printf("\n  o list all files from 'best.rar...");
  filecounter = urarlib_list("../../rars/best.rar",
                             (ArchiveList_struct*)&List);

  printf("%d files(s) found.\n\n", filecounter);
  print_list(List);

  urarlib_freelist(List);                   /* free memory                  */


  printf("\n (C) 2000-2002 by Christian Scheurer aka. UniquE of Vantage"\
         "\n http://www.unrarlib.org and http://www.vantage.ch"\
         "\n Linux port by Johannes Winkelmann (jw@tks6.net)\n\n");


  printf("\n End of listing. Please press [ENTER]...");
  ch = getchar();
  printf("\n\n");

  return 0;
}



void print_list(ArchiveList_struct *list)
/* display structure of information about a RAR archive                     */
{
  printf("                   Name     Size  Packed   OS  FileTime    ");
  printf("CRC-32 Attr Ver Meth\n");
  printf("     ------------------------------------------------------");
  printf("--------------------\n");

  while(list != NULL){
    if(list->item.NameSize < 23)
    {
      printf("%23s", list->item.Name);
    } else
    {
      printf("%23s", "[name too long]");
    }
    printf("%9ld",  list->item.UnpSize);
    printf("%8ld",  list->item.PackSize);
    printf("%5d",  list->item.HostOS);
    printf("%10lx", list->item.FileTime);
    printf("%10lx", list->item.FileCRC);
    printf("%5ld",  list->item.FileAttr);
    printf("%4d",  list->item.UnpVer);
    printf("%5d",  list->item.Method);
    printf("\n");

    list = (ArchiveList_struct*)list->next;
  }

}
