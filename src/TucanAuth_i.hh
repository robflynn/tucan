//////////////////////////////////////////////////////////
// TucanAuth_i (c++ header)  
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
// Filename:        TucanAuth_i.hh 
// Description:     TucanAuth for CORBA implementation 
// Author:          Jared Peterson <jared@difinium.com>
// Created at:      Tue Dec 12 13:44:29 PST 2000 
//////////////////////////////////////////////////////////

#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <string>
#include <Tucan.hh>

class TucanAuth_i: public POA_TucanAuth,
	public PortableServer::RefCountServantBase
{
	public:
		TucanAuth_i(CORBA::ORB_var orb);
		virtual ~TucanAuth_i(){};

		char* getUid();
		void setUid(const char* uid);
		char* getUserName();
		void setUserName(const char* name);
		char* getPassword();
		void setPassword(const char* password);
		CORBA::Long getPinNumber();
		void setPinNumber(CORBA::Long pin);
		void destroy();

	private:
		string myUserName;
		string myPassword;
		string myUid;
		CORBA::Long myPinNumber;
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};

class TucanAuthFactory_i: public POA_TucanAuthFactory,
	public PortableServer::RefCountServantBase
{
	public:
        	TucanAuthFactory_i(CORBA::ORB_var orb);
		virtual ~TucanAuthFactory_i() {};

		TucanAuth_ptr create();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};
