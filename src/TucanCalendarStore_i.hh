////////////////////////////////////////////////////////////
// TucanCalendarStore (c++ header)
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
// Filename:        TucanCalendarStore_i.hh 
// Description:     The Calendar store interface 
// Author:          Jared Peterson <jared@difinium.com>
// Created at:      Fri Jan 19 10:05:49 PST 2001 
////////////////////////////////////////////////////////////

#include <iostream.h>
#include <string>
#include <Tucan.hh>

class TucanCalendarStore_i: public POA_TucanCalendarStore,
                public PortableServer::RefCountServantBase 
{
	public:
  		TucanCalendarStore_i(CORBA::ORB_var orb);
  		virtual ~TucanCalendarStore_i() {};

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

class TucanCalendarStoreFactory_i: public POA_TucanCalendarStoreFactory,
                public PortableServer::RefCountServantBase 
{
	public:
  		TucanCalendarStoreFactory_i(CORBA::ORB_var orb);
  		virtual ~TucanCalendarStoreFactory_i() {};

  		TucanStore_ptr create();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};
