//////////////////////////////////////////////////////////
// TucanEmailStore_i (c++ header)	
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
// PURPOSE.	See the GNU General Public License for more 
// details.
//
// You should have received a copy of the GNU General Public 
// License along with this program; if not, write to the Free 
// Software Foundation, Inc., 59 Temple Place, Suite 330, 
// Boston, MA	02111-1307	USA
// 
//////////////////////////////////////////////////////////
// Filename:		TucanEmailStore_i.hh 
// Description:		TucanEmailStore for CORBA implementation 
// Author:			Rob Flynn <rob@difinium.com> 
// Created at:		Tue Dec 12 13:44:29 PST 2000 
//////////////////////////////////////////////////////////

#include <iostream.h>
#include <fstream.h>
#include <Tucan.hh>
#include <sys/types.h>
#include <dirent.h>
#include <string>

class TucanEmailStore_i: public POA_TucanEmailStore,
	public PortableServer::RefCountServantBase 
{
	public:
		TucanEmailStore_i(CORBA::ORB_var orb);
		virtual ~TucanEmailStore_i() {};

		char* getName();
		void setName(const char* name);
		TucanFolder_ptr getDefaultFolder();
		TucanFolder_ptr getFolder(const char* name);
		TucanFolder_ptr getNewFolderObject();
		TucanStringList* listFolders();
		TucanAuth_ptr getAuth();
		void setAuth(TucanAuth_ptr myAuth);
		void destroy();

	private:
		string myName;
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
		TucanAuth_var myAuthObject;
};

class TucanEmailStoreFactory_i: public POA_TucanEmailStoreFactory,
	public PortableServer::RefCountServantBase 
{
	public:
		TucanEmailStoreFactory_i(CORBA::ORB_var orb);
		virtual ~TucanEmailStoreFactory_i() {};

		TucanStore_ptr create();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};
