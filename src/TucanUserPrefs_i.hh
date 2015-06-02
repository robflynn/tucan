////////////////////////////////////////////////////////////
// TucanUserPrefs (c++ header)
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
// Filename:        TucanUserPrefs_i.hh 
// Description:     The User Preferences interface
// Author:          Rob Flynn <rob@difinium.com>
// Created at:      Tue Mar 27 14:17:47 PST 2001 
////////////////////////////////////////////////////////////

#include <iostream.h>
#include <pwd.h>
#include <string>
#include <vector>
#include <Tucan.hh>
#include <TucanConfig.hh>
#include <TucanUtilities.hh>
#include <TucanDb.hh>


class TucanUserPrefs_i: public POA_TucanUserPrefs,
	public PortableServer::RefCountServantBase 
{

	public:
		TucanUserPrefs_i(CORBA::ORB_var orb);
		virtual ~TucanUserPrefs_i() {};

		TucanAttributeSeq* getAllAttributes();
		char* getAttribute(const char* key);
		void setAttribute(const char* key, const char* value);
		void deleteAttribute(const char* key);

		void setAuth(TucanAuth_ptr myAuth);
		TucanAuth_ptr getAuth();
		
		void destroy();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
		TucanAuth_var myAuth;
		
};

class TucanUserPrefsFactory_i: public POA_TucanUserPrefsFactory,
	public PortableServer::RefCountServantBase 
{
	public:
		TucanUserPrefsFactory_i(CORBA::ORB_var orb);
		virtual ~TucanUserPrefsFactory_i() {};

		TucanUserPrefs_ptr create();
	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};


