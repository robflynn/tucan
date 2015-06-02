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

#include <TucanConfig.hh>

TucanGetConfig::TucanGetConfig(char* search, char* attr)
{
	getIorContent = 0;
	inRightOne = 0;
	inRightOne2 = 0;
	result = NULL;
	mySearch = strdup(search);
	myAttr = strdup(attr);

	if(!(strcmp(myAttr, "")))
		one = 1;
	else
		one = 0;
}

TucanGetConfig::~TucanGetConfig()
{
	if (mySearch)
		free(mySearch);

	if (myAttr)
		free(myAttr);

	if (result)
		free(result);
}

void TucanGetConfig::startElement(const XMLCh* const name, AttributeList& attributes)
{
	char* n = XMLString::transcode(name);
	
	if(one && !(strcmp(n, mySearch)))
	{
		inRightOne = 1;
		inRightOne2 = 1;
	}

	if(!(strcmp(n, "provider")))
	{
		const int i = 0;
		char* value = XMLString::transcode(attributes.getValue(i));
		
		if(!(strcmp(mySearch, value)))
			inRightOne = 1;

		delete[] value;
	}
	else if(!(strcmp(n, myAttr)))
	{
		if(inRightOne)
			inRightOne2 = 1;
	}
	else if(!(strcmp(n, "ior")))
	{
		if(inRightOne && inRightOne2)
		{
			getIorContent = 1;
		}
	}
	else if(!(strcmp(n, "value")))
	{
		if(inRightOne && inRightOne2)
		{
			getIorContent = 1;
		}
	}
	delete[] n;
}		

void TucanGetConfig::endElement(const XMLCh* const name)
{
	char* n = XMLString::transcode(name);
	
	if(one && !(strcmp(n, mySearch)))
	{
		inRightOne = 0;
		inRightOne2 = 0;
	}

	if(!(strcmp(n, "provider")))
	{
		if(inRightOne)
			inRightOne = 0;
	}
	else if(!(strcmp(n, myAttr)))
	{
		if(inRightOne)
			inRightOne2 = 0;
	}
	else if(!(strcmp(n, "ior")))
	{
		if(getIorContent)
		{
			getIorContent = 0;
			ior = "";
		}
	}
	else if(!(strcmp(n, "value")))
	{
		if(getIorContent)
		{
			getIorContent = 0;
			ior = "";
		}
	}
	delete[] n;
}

void TucanGetConfig::characters(const XMLCh* const chars, const unsigned int length)
{
	char* c = XMLString::transcode(chars);
	
	if(getIorContent)
	{
		result = strdup(c);
	}

	delete[] c;
}

char* TucanGetConfig::getResult() 
{
	if (!result)
		return "";

       	return result; 
}

char* TucanConfig::get(char* search, char* attr)
{
	const char* xmlFile = PREFIX_PATH"/share/config/TucanConfig.xml";

	SAXParser parser;
	TucanGetConfig dh(search, attr);
	parser.setDocumentHandler(&dh);
	HandlerBase errHandler;
	parser.setErrorHandler(&errHandler);
	parser.parse(xmlFile);

	result = dh.getResult();

	return (char*)result.c_str();
}
