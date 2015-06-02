//////////////////////////////////////////////////////////
// TucanEmailAddress_i (c++ impl)	
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
// Filename:		TucanEmailAddress_i.cc
// Description:		TucanEmailAddress for CORBA implementation 
// Author:			Rob Flynn <rob@difinium.com>
// Created at:		Fri Jan 23 23:35:33 PST 2001
//////////////////////////////////////////////////////////

#include <TucanEmailAddress_i.hh>

TucanEmailAddress_i::TucanEmailAddress_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

char* TucanEmailAddress_i::getAddress()
{
	return CORBA::string_dup(myAddress.c_str());
}

void TucanEmailAddress_i::setAddress(const char* address)
{
	myAddress = address;
}

CORBA::Boolean TucanEmailAddress_i::equals(TucanAddress_ptr address)
{
	CORBA::String_var other_address = address->getAddress();
	if (!strcmp(other_address, myAddress.c_str()))
		return 1;
	else
		return 0;
}

void TucanEmailAddress_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

// Factory

TucanEmailAddressFactory_i::TucanEmailAddressFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanAddress_ptr TucanEmailAddressFactory_i::create()
{
	TucanEmailAddress_i* myObject = new TucanEmailAddress_i(myOrb);
	PortableServer::ObjectId_var myId = myPoa->activate_object(myObject);
	CORBA::Object_var obj = myPoa->servant_to_reference(myObject);
	myObject->_remove_ref();

	return TucanEmailAddress::_narrow(obj);
}
