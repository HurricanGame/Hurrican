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
 * Program    : getfile.c
 * Function   : Example for the usage of the UniquE RAR File Library
 *              This is the most simple example which will extract one file
 *              out of a RAR archive.
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



void write_file(char *filename, char *data, unsigned long datasize);

int main ()
{
  char            *data_ptr;                /* pointers for the decompressed*/
                                            /* data                         */
  unsigned long   data_size;                /* size of the decompressed     */
                                            /* data                         */
  int             ch;                       /* for "please press ENTER"     */


  printf("\n Demonstration of the UniquE RAR File Library\n");
  printf(  " ~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~\n");


  /*** extract files for RAR archive                                        */
  printf("  o extracting 'best.rar:\\smbconf.txt'...");
  if(urarlib_get(&data_ptr, &data_size, "smbconf.txt",
                 "../../rars/best.rar", "password"))
  {
    printf("OK.\n");
    write_file("smbconf.txt", data_ptr, data_size);
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


void write_file(char *filename, char *data, unsigned long datasize)
/* write buffer to a file                                                   */
{
  FILE            *datafp;

  if(((datafp = fopen(filename, "wb")) == NULL) || (data == NULL))
  {
    printf("  o Error creating binary output file!\n\n");
  } else
  {
    printf("  o writing %u bytes to '%s'...",
         (unsigned int)datasize, filename);
    fwrite(data, 1, datasize, datafp);
    fclose(datafp);
    printf("OK.\n\n");
  }
}

