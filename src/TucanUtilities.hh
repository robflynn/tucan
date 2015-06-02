//////////////////////////////////////////////////////////
// TucanUtilities.hh
//
// Copyright (C) 2000-2001, Difinium
// 
// This program is free software; you can redistribute it 
// and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; 
// either version 2 of the License, or (at your option) 
// any later version.
//
// This program is distributed in the hope that it will be 
// useful, but WITHOUT ANY WARRANTY; without even the implied 
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
// PURPOSE.     See the GNU General Public License for more 
// details.
//
// You should have received a copy of the GNU General Public 
// License along with this program; if not, write to the Free 
// Software Foundation, Inc., 59 Temple Place, Suite 330, 
// Boston, MA   02111-1307      USA
// 
//////////////////////////////////////////////////////////
// Filename:            TucanUtilities.hh
// Description:         Various utilities for TUCAN 
// Author:              Rob Flynn <rob@difinium.com>
// Created at:          Fri Jan 23 23:35:33 PST 2001
//////////////////////////////////////////////////////////

#ifndef __TUCAN_UTILITIES_H_
#define __TUCAN_UTILITIES_H_

#include <string>
#include <vector>
#include <time.h>
#include <map>

typedef map<string, string> myHash;

extern vector<string> strsplit(string, char );
extern vector<string> strsplit(string, char, int );
extern string strreplace(string, char, char);
extern string strjoin(vector<string>, char );
extern string strchomp(string);
extern string uppercase(const char *);
extern string uppercase(string);
extern string strwstrip(string);
extern string escape(string);
extern const char* escape(char*);
extern int ishex(char);
extern int unhex(string);
extern string qpDecode(string);
extern long uueDecode(char **, string);
extern string encodeBase64(char *, unsigned long);
extern long decodeBase64(char **, string);
extern struct tm* parsedate(const char *);
extern char * gethomedir(string);
extern char * gethomedir(char *);
#endif
