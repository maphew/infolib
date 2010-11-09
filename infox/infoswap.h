/* @(#)infoswap.h	1.2 12/21/93 */
/*
 *  Notice:  This software is being released into the public
 *           domain.  It may be freely used whole or in part or
 *           distributed as a whole as long as this notice remains
 *           attached to the source code.  The author and ESRI
 *           do not assume any liability or responsibility in the
 *           use or distribution of this software.  Direct any
 *           questions or comments via email to the author at
 *           tstellhorn@esri.com.
 *
 *                              Todd Stellhorn
 *                              ESRI
 */

/***************************************************************************
  Name: infoswap.h
  
  Description: infolib byte swaping definitions.
  
  History: 11/92  Todd Stellhorn  - original coding
  
  **************************************************************************/

#ifndef _INFOSWAP_H_
#define _INFOSWAP_H_

/***************************************************************************/

/* if swapping in on, define swapping function */
#ifdef INFO_SWAP
#     define INFO_BYTESWAP( buf, size, num ) InfoByteSwap( buf, size, num );
#else
#     define INFO_BYTESWAP( buf, size, num )
#endif

/***************************************************************************/

#endif  /* _INFOSWAP_H_ */
