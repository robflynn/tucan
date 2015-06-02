////////////////////////////////////////////////////////////
// TucanAddressBookStore (c++ header)
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
// Filename:        TucanAddressBookStore_i.hh 
// Description:     The Address Book store interface 
// Author:          Mathew Davis <mattd@difinium.com>
// Created at:      Mon Feb 12 17:32:22 PST 2001 
////////////////////////////////////////////////////////////

#include <iostream.h>
#include <string>
#include <Tucan.hh>

class TucanAddressBookStore_i: public POA_TucanAddressBookStore,
                public PortableServer::RefCountServantBase
{		
	public:
		TucanAddressBookStore_i(CORBA::ORB_var orb);
		virtual ~TucanAddressBookStore_i() {};
		char* getName();
		void setName(const char* name);
		TucanFolder_ptr getDefaultFolder();
		TucanFolder_ptr getFolder(const char* name);
		TucanFolder_ptr getNewFolderObject();
		TucanStringList* listFolders();
		TucanAuth_ptr getAuth();
		void setAuth(TucanAuth_ptr auth);
		void destroy();

	private:
		TucanAuth_var myAuth;
		string myName;
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};

class TucanAddressBookStoreFactory_i: public POA_TucanAddressBookStoreFactory,
                public PortableServer::RefCountServantBase
{		
	public:
		TucanAddressBookStoreFactory_i(CORBA::ORB_var orb);
		virtual ~TucanAddressBookStoreFactory_i() {};
		TucanStore_ptr create();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};
