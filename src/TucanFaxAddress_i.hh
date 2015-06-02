////////////////////////////////////////////////////////////
// TucanFaxAddress (c++ header)
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
// Filename:        TucanFaxAddress_i.hh 
// Description:     The Fax address interface 
// Author:          Rob Flynn <rob@difinium.com>
// Created at:      Thu Feb 16 15:12:13 PST 2001 
////////////////////////////////////////////////////////////

#include <iostream.h>
#include <string>
#include <Tucan.hh>

class TucanFaxAddress_i: public POA_TucanFaxAddress,
                public PortableServer::RefCountServantBase 
{
	public:
  		TucanFaxAddress_i(CORBA::ORB_var orb);
  		virtual ~TucanFaxAddress_i() {};

  		char* getAddress();
  		void setAddress(const char* address);
  		CORBA::Boolean equals(TucanAddress_ptr address);
  		void destroy();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
		string myAddress;
};

class TucanFaxAddressFactory_i: public POA_TucanFaxAddressFactory,
                public PortableServer::RefCountServantBase 
{
	public:
		TucanFaxAddressFactory_i(CORBA::ORB_var orb);
		virtual ~TucanFaxAddressFactory_i() {};

		TucanAddress_ptr create();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};
