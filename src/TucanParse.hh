//////////////////////////////////////////////////////////
// TucanParse.hh
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
// Filename:    TucanParse.hh 
// Description: E-mail parser written for TUCAN
// Author:      Rob Flynn <rob@difinium.com>
// Created at:  Fri Jan 23 23:35:33 PST 2001
//////////////////////////////////////////////////////////


#ifndef __TUCAN_PARSE_H_
#define __TUCAN_PARSE_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <hash_map>
#include <ctype.h>
#include <TucanUtilities.hh>

class TucanParse
{
	public:
		TucanParse() {};
		~TucanParse() {};
		
		string getHeader(string message);
		string getBody(string message);
		myHash parseHeader(string header);
		myHash parseItem(string name, string line);
		vector<string> parseBody(string body, string boundary);
};

#endif
