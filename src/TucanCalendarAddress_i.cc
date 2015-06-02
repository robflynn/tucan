////////////////////////////////////////////////////////////
// TucanCalendarAddress (c++ impl)
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
// Filename:        TucanCalendarAddress_i.cc 
// Description:     The Calendar address interface 
// Author:          Jared Peterson <jared@difinium.com>
// Created at:      Thu Feb  8 12:31:46 PST 2001 
////////////////////////////////////////////////////////////

#include <TucanCalendarAddress_i.hh>

TucanCalendarAddress_i::TucanCalendarAddress_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

char* TucanCalendarAddress_i::getAddress()
{
	return CORBA::string_dup(myAddress.c_str());
}

void TucanCalendarAddress_i::setAddress(const char* address)
{
	myAddress = address;
}

CORBA::Boolean TucanCalendarAddress_i::equals(TucanAddress_ptr address)
{
	CORBA::String_var other_address = address->getAddress();
	if(!(myAddress.compare(other_address)))
		return 1;
	else
		return 0;
}

void TucanCalendarAddress_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

TucanCalendarAddressFactory_i::TucanCalendarAddressFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanAddress_ptr TucanCalendarAddressFactory_i::create()
{
	TucanCalendarAddress_i* myObject = new TucanCalendarAddress_i(myOrb);
	PortableServer::ObjectId_var myId = myPoa->activate_object(myObject);
	CORBA::Object_var obj = myPoa->servant_to_reference(myObject);
	myObject->_remove_ref();

	return TucanCalendarAddress::_narrow(obj);
}
