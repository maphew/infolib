/* @(#)infoswap.c	1.2 12/21/93 */
/*
 * Notice:  This software is being released into the public
 *          domain.  It may be freely used whole or in part or 
 *          distributed as a whole as long as this notice remains
 *          attached to the source code.  The author and ESRI 
 *          do not assume any liability or responsibility in the 
 *          use or distribution of this software.  Direct any 
 *          questions or comments via email to the author at 
 *          tstellhorn@esri.com.
 *           
 *                             Todd Stellhorn
 *                             ESRI
 */

/***************************************************************************
  Name: infoswap.c
  
  Description: Contains infolib byte swaping functions.  The following
  functions are contained in this module:
  
  InfoBtyeSwap
  
  History: 11/92  Todd Stellhorn  - original coding
  
  10/94 Randy Deardorff - my own peculiar includes.
  
  01/95 Randy Deardorff - added stdio.h.  This is not needed unless
  you are also including rbdpaths.h.
  
  ***************************************************************************/

#include <stdio.h>
#include <rbdpaths.h>
#include INFODEFS_H
#include INFOSWAP_H

/***************************************************************************
  Name: InfoByteSwap
  
  Purpose: This function swaps the bytes in input buffer (buf) based
  on element size and the number of elements.
  
  Parameters:
  buf ( void * ) Pointer to input/output buffer
  size ( short int ) Input size of buffer elements
  num ( short int ) Input number of buffer elements
  
  Outputs: Function - ( void ) Function status is not returned.  Values
  in buffer buf are updated.
  
  History: 11/92  Todd Stellhorn  - original coding
  
  ***************************************************************************/

void InfoByteSwap( buf, size, num )
     
     void *buf;
     short int size;
     short int num;
     
{
  short int      i, j;
  unsigned char  *p, tmp;
  
  p = ( unsigned char * ) buf;
  
  for (i = 0; i < num; i++)
    {
      for (j = 0; j < ( size >> 1 ); j++)
        {
          tmp = p[j];
          p[j] = p[size - j - 1];
          p[size - j - 1] = tmp;
        }
      p += size;
    }
}
