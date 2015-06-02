//////////////////////////////////////////////////////////
// TucanAuth_i (c++ impl)  
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
// Filename:        TucanAuth_i.cc 
// Description:     TucanAuth for CORBA implementation 
// Author:          Jared Peterson <jared@difinium.com>
// Created at:      Tue Dec 12 13:44:29 PST 2000 
//////////////////////////////////////////////////////////

#include <TucanAuth_i.hh>

//Standard constructor
TucanAuth_i::TucanAuth_i(CORBA::ORB_var orb)
{
	//IMPORTANT that we get the orb and initialize
	//the POA
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);

	//Inititalize our attributes
	myUserName = "None";
	myPassword = "";
	myUid = "None";
	myPinNumber = 0;
}

char* TucanAuth_i::getUid()
{
	return CORBA::string_dup(myUid.c_str());
}

void TucanAuth_i::setUid(const char* uid)
{
	myUid = uid;
}

char* TucanAuth_i::getUserName()
{
	return CORBA::string_dup(myUserName.c_str());
}

void TucanAuth_i::setUserName(const char* username)
{
	myUserName = username;
}

char* TucanAuth_i::getPassword()
{
	return CORBA::string_dup(myPassword.c_str());
}

void TucanAuth_i::setPassword(const char* password)
{
	myPassword = password;
}

CORBA::Long TucanAuth_i::getPinNumber()
{
	return myPinNumber;
}

void TucanAuth_i::setPinNumber(CORBA::Long pin)
{
	myPinNumber = pin;
}

void TucanAuth_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

TucanAuthFactory_i::TucanAuthFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanAuth_ptr TucanAuthFactory_i::create()
{
	TucanAuth_i* myAuth = new TucanAuth_i(myOrb);
	PortableServer::ObjectId_var myAuthid = 
		myPoa->activate_object(myAuth);
	CORBA::Object_var obj = 
		myPoa->servant_to_reference(myAuth);
	//TucanAuth_var auth = myAuth->_this();
	myAuth->_remove_ref();

	//return auth._retn();
	return TucanAuth::_narrow(obj);
}
