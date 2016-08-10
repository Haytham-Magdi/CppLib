/* This is file bios.h */
/*
** Copyright (C) 1994 DJ Delorie, 24 Kirsten Ave, Rochester NH 03867-2954
**
** This file is distributed under the terms listed in the document
** "copying.dj", available from DJ Delorie at the address above.
** A copy of "copying.dj" should accompany this file; if not, a copy
** should be available from where this file was obtained.  This file
** may not be distributed without a verbatim copy of "copying.dj".
**
** This file is distributed WITHOUT ANY WARRANTY; without even the implied
** warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef _BIOS_H_
#define _BIOS_H_

#ifdef __cplusplus
extern "C" {
#endif

int bioscom(int cmd, char data, int port);
int biosdisk(int cmd, int drive, int head, int track, int sector,
	     int nsects, void *buffer);
int biosequip(void);
int bioskey(int cmd);
int biosmemory(void);
int biosprint(int cmd, int byte, int port);
long biostime(int cmd, long newtime);

#ifdef __cplusplus
}
#endif

#endif
