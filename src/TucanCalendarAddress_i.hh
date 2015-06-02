////////////////////////////////////////////////////////////
// TucanCalendarAddress (c++ header)
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
// Filename:        TucanCalendarAddress_i.hh 
// Description:     The Calendar address interface 
// Author:          Jared Peterson <jared@difinium.com>
// Created at:      Thu Feb  8 12:30:28 PST 2001 
////////////////////////////////////////////////////////////

#include <iostream.h>
#include <string>
#include <Tucan.hh>

class TucanCalendarAddress_i: public POA_TucanCalendarAddress,
                public PortableServer::RefCountServantBase 
{
	public:
  		TucanCalendarAddress_i(CORBA::ORB_var orb);
  		virtual ~TucanCalendarAddress_i() {};

  		char* getAddress();
  		void setAddress(const char* address);
  		CORBA::Boolean equals(TucanAddress_ptr address);
  		void destroy();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
		string myAddress;
};

class TucanCalendarAddressFactory_i: public POA_TucanCalendarAddressFactory,
                public PortableServer::RefCountServantBase 
{
	public:
		TucanCalendarAddressFactory_i(CORBA::ORB_var orb);
		virtual ~TucanCalendarAddressFactory_i() {};

		TucanAddress_ptr create();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};
