////////////////////////////////////////////////////////////
// TucanConfig  
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
// PURPOSE.  See the GNU General Public License for more 
// details.
//
// You should have received a copy of the GNU General Public 
// License along with this program; if not, write to the Free 
// Software Foundation, Inc., 59 Temple Place, Suite 330, 
// Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////
// Filename:        TucanConfig.hh 
// Description:     Interface for getting xml config data 
// Author:          Jared Peterson <jared@difinium.com>
// Created at:      Tue Dec 12 13:44:29 PST 2000 
////////////////////////////////////////////////////////////

#ifndef __TUCAN_CONFIG_H_
#define __TUCAN_CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include <string>
#include <sax/HandlerBase.hpp>
#include <parsers/SAXParser.hpp>
#include <sax/AttributeList.hpp>
#include <util/PlatformUtils.hpp>

class TucanGetConfig : public HandlerBase
{
	public:
		TucanGetConfig(char* search, char* attr);
		~TucanGetConfig();

		void startElement(const XMLCh* const name, AttributeList& attributes);
		void endElement(const XMLCh* const name);
		void characters(const XMLCh* const chars, const unsigned int length);
		
		char* getResult();

	private:
		int getIorContent;
		int inRightOne;
		int inRightOne2;
		int one;
		char* mySearch;
		char* myAttr;
		char* result;
		char* ior; 
};

class TucanConfig
{
	public:
		TucanConfig() {};
		~TucanConfig() {};

		char* get(char* search, char* attr);

	private:
		string result;
};

#endif
