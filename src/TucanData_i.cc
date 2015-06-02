//////////////////////////////////////////////////////////
// TucanData_i (c++ impl)
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
//////////////////////////////////////////////////////////
// Filename:		TucanData_i.cc
// Description:		TucanData for CORBA implementation
// Author:			Rob Flynn <rob@difinium.com>
// Created at:		Thu Jan 25 10:52:33 PST 2001
//////////////////////////////////////////////////////////

#include <TucanConfig.hh>
#include <TucanData_i.hh>
#include <TucanUtilities.hh>
#include <fstream.h>
#include <string>

TucanData_i::TucanData_i(CORBA::ORB_var orb)
{
	myOrb = orb;

	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);

	myRawData = NULL;
	dataSize = 0;
}


char* TucanData_i::getText()
{
	return CORBA::string_dup(myText.c_str());
}

void TucanData_i::setText(const char* text)
{
	myText = text;
}

char* TucanData_i::getXML()
{
	return CORBA::string_dup(myXML.c_str());
}

void TucanData_i::setXML(const char* xml)
{
	myXML = xml;
}

char* TucanData_i::getMime()
{
	return CORBA::string_dup(myMime.c_str());
}

void TucanData_i::setMime(const char* mime)
{
	myMime = mime;
}

TucanRawData* TucanData_i::getData()
{
	unsigned long long i;
	TucanRawData *data = new TucanRawData();

	data->length(dataSize);

	for (i = 0; i < dataSize; i++)
		(*data)[i] = myRawData[i];
	
	return data;
}

void TucanData_i::setData(const TucanRawData& data)
{
	unsigned long long i;

	if (myRawData)
		free(myRawData);

	dataSize = data.length();

	myRawData = (unsigned char *)malloc(sizeof(unsigned char) * dataSize);

	for (i = 0; i < dataSize; i++)
		myRawData[i] = data[i];
}

char* TucanData_i::getPath()
{
	return CORBA::string_dup(myPath.c_str());
}

void TucanData_i::setPath(const char* path)
{
	myPath = path;
}

char* TucanData_i::getSize()
{
	return CORBA::string_dup(mySize.c_str());
}

void TucanData_i::setSize(const char* size)
{
	mySize = size;
}

char* TucanData_i::getContentType()
{
	return CORBA::string_dup(myContentType.c_str());
}

void TucanData_i::setContentType(const char* type)
{
	myContentType = type;
}

long TucanData_i::getPartCount() 
{
	return myParts.size();
}


void TucanData_i::addPart(const char *type, const TucanStringList &header, const TucanRawData& data)
{
	TucanData::TucanPart part;
	string mime("");

	if (myParts.size() == 0)
	{
		// Looks like we have no parts.  Lets back up our old
		// one and set the message as multipart

		part.type = CORBA::string_dup(myContentType.c_str());

		part.data.length(myText.length());
		for (unsigned int i = 0; i < myText.length(); i++)
			part.data[i] = myText[i];
			
		myParts.push_back(part);
			
		setContentType("multipart/mixed");
		setText("This is a multipart message in MIME format.");
	}


	part.type = CORBA::string_dup(type);
	
	part.header.length(header.length());
	
	part.data.length(data.length());

	for (unsigned long i = 0; i < data.length(); i++)
		part.data[i] = data[i];
	
	part.encoding = CORBA::string_dup("");

	for (unsigned long i = 0; i < header.length(); i++)
	{
		part.header[i] = CORBA::string_dup(header[i]);

		if (strlen(part.header[i]) == 0)
			continue;

		string headeritem(header[i]);

		vector<string>pieces = strsplit(headeritem, ':', 1);

		if (!strcasecmp(pieces[0].c_str(), "content-transfer-encoding"))
			part.encoding = CORBA::string_dup(pieces[1].c_str());

		pieces.clear();
	}


	myParts.push_back(part);
}


TucanStringList* TucanData_i::listParts()
{
	TucanStringList* list = new TucanStringList();

	list->length(getPartCount());

	for (unsigned long i = 0; i < myParts.size(); i++)
		(*list)[i] = CORBA::string_dup(myParts[i].type);

	return list;
}

TucanData::TucanPart* TucanData_i::getPart(long num) 
{
	TucanData::TucanPart *part = new TucanData::TucanPart;

	if (num > getPartCount())
		throw("Error: Part number out of range");
	
	part->type = myParts[num].type;
	part->data = myParts[num].data;

	part->header.length(myParts[num].header.length());

	unsigned int x;
	for (x = 0; x < myParts[num].header.length(); x++)
		part->header[x] = CORBA::string_dup(myParts[num].header[x]);

	return part;
}


void TucanData_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);

	if (myRawData)
		free(myRawData);
}


// Factory


TucanDataFactory_i::TucanDataFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;

	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanData_ptr TucanDataFactory_i::create()
{
	TucanData_i* myDataObject = new TucanData_i(myOrb);
	PortableServer::ObjectId_var myId = myPoa->activate_object(myDataObject);
	CORBA::Object_var obj = myPoa->servant_to_reference(myDataObject);
	myDataObject->_remove_ref();

	return TucanData::_narrow(obj);
}
