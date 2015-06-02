//////////////////////////////////////////////////////////
// TucanData_i (c++ header)
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
// Filename:		TucanData_i.hh
// Description:		TucanData for CORBA implementation
// Author:			Rob Flynn <rob@difinium.com>
// Created at:		Thu Jan 25 10:44:04 PST 2001
//////////////////////////////////////////////////////////

#include <Tucan.hh>
#include <string>
#include <vector>
#include <map>

class TucanData_i: public POA_TucanData,
	public PortableServer::RefCountServantBase 
{
	public:
		TucanData_i(CORBA::ORB_var orb);
		virtual ~TucanData_i() {};

		char* getText();
		void setText(const char* text);
		char* getXML();
		void setXML(const char* xml);
		char* getMime();
		void setMime(const char* mime);
		TucanRawData* getData();
		void setData(const TucanRawData& data);
		char* getPath();
		void setPath(const char* path);
		char* getSize();
		void setSize(const char* size);
		char* getContentType();
		void setContentType(const char* type);
		long getPartCount();
		void addPart(const char *type, const TucanStringList &header, const TucanRawData& data);
		TucanData::TucanPart* getPart(long num);
		TucanStringList* listParts();
		void destroy();

	private:
		vector<struct TucanData::TucanPart> myParts;
		
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
		string myText;
		string myXML;
		string myMime;
		string myPath;
		string mySize;
		string myContentType;
	
		unsigned long long dataSize;
		unsigned char *myRawData;
};

class TucanDataFactory_i: public POA_TucanDataFactory,
	public PortableServer::RefCountServantBase 
{
	public:
		TucanDataFactory_i(CORBA::ORB_var orb);
		virtual ~TucanDataFactory_i() {};

		TucanData_ptr create();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};
