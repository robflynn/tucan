////////////////////////////////////////////////////////////
// TucanSession_i (c++ impl)  
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
// Filename:		TucanSession_i.cc 
// Description:	 TucanSession for CORBA implementation 
// Author:		  Jared Peterson <jared@difinium.com>
// Created at:	  Tue Dec 12 13:44:29 PST 2000 
////////////////////////////////////////////////////////////

#include <TucanSession_i.hh>
#include <TucanUtilities.hh>
#include <stdlib.h>

//Standard constructor method
TucanSession_i::TucanSession_i(CORBA::ORB_var orb)
{
	//IMPORTANT that the orb var and poa get set
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
	
	//A session is not authorized by default
	myAuthorized = 0;

	// They're not admin by default :-P
	myAdmin = 0;
}

//getAuth() return a TucanAuth object
TucanAuth_ptr TucanSession_i::getAuth()
{
	string str;
	TucanAuth_ptr auth;
	string filename;
	
	//Get the ior filename from our config file
	try
	{
		TucanConfig* config = new TucanConfig();
		filename = config->get("auth", "");
		delete config;
	}
	catch(...)
	{
		throw TucanException("Error: Error getting
				ior filename from TucanConfig.xml.");
	}

	//Read in the ior information from the file
	try
	{
		ifstream infile(filename.c_str());
		getline(infile, str);
		infile.close();
	}
	catch(...)
	{
		string excp_str = "Error: Error reading ior file: "
			+ filename + " in getAuth().";
		throw TucanException(excp_str.c_str());
	}

	//Get the object factory and create the auth
	//object and narrow it
	try
	{
		CORBA::Object_var object = 
			myOrb->string_to_object(str.c_str());
		TucanAuthFactory_var fact = 
			TucanAuthFactory::_narrow(object);
		auth = fact->create();
	}
	catch(...)
	{
		throw TucanException("Error: Error creating
				TucanAuth object in getAuth().");
	}

	//Return the auth object
	return auth;
}

//authorize() takes in a TucanAuth object and
//try to authorize the user based on it's attributes
void TucanSession_i::authorize(TucanAuth_ptr auth)
{

	string table_name("user_info");
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	delete config;

	//Get all of the attributes from the object
	CORBA::String_var username = auth->getUserName();
	CORBA::String_var password = auth->getPassword();
	CORBA::Long pinnumber = auth->getPinNumber();
	myAuth = TucanAuth::_duplicate(auth);

	if (strlen(username) < 1)
	{
			CORBA::String_var un = myAuth->getUserName();
			CORBA::String_var pw = myAuth->getPassword();
			CORBA::Long pn = myAuth->getPinNumber();

			throw TucanAuthException(un, pw, pn, 
				"Error : Invalid Authorization");
	}
   
	//Are we authorizing with password?
	if(strlen(password) > 0)
	{
		FILE* exec_res;
		char res[32];
		char* res_ptr;
		string value1;
		string value2;

		//Build our string to try to authorize user
		string command("authorize ");
		command += username;
		command += " ";
		command += password;
		command += " ";
		command += "/usr/bin/id -u";

		//Execute the string and get the result
		//if the user exist and is authorized
		//then the return will be the uid

		exec_res = popen(command.c_str(), "r");
		res_ptr = fgets(res, 32, exec_res);
		pclose(exec_res);

		value1 = res;
		
		if(res_ptr != NULL)
			value2 = res_ptr;
		else
			value2 = "";

		//Determine if we were authorized
		if(!(value1.compare(value2)))
		{
			myAuthorized = 1;
			value1 = strchomp(value1);
			auth->setUid(value1.c_str());
		}
		else
		{
			CORBA::String_var un = myAuth->getUserName();
			CORBA::String_var pw = myAuth->getPassword();
			CORBA::Long pn = myAuth->getPinNumber();

			throw TucanAuthException(un, pw, pn, 
				"Error : Invalid Authorization");
		}
	}
	//Authorizing with pin
	else if(pinnumber != 0)
	{
		const int MaxPinLength = 10;
		char pin[MaxPinLength];


		//We need to query the database to check pinnumber
		connection* db = new connection(dbname);
		cursor* c = db->getCursor();
		memset(pin, 0, MaxPinLength*sizeof(char));
		sprintf(pin, "%ld", pinnumber);
		string exec = "SELECT uid FROM " + table_name + 
			" WHERE login_name='";
		exec += username;
		exec += "' AND pin=";
		exec += pin;
		c->execute(exec);
		vector<string> res;
		TucanConfig* config = new TucanConfig();
		string dbname = config->get("dbname", "");
		delete config;

		//Is the username and pin correct?
		try
		{
			res = c->fetchone();
		}
		catch(exception e)
		{
			throw TucanAuthException(myAuth->getUserName(),
					myAuth->getPassword(),
					myAuth->getPinNumber(),
					"Error : Invalid Authorization");
		}

		myAuthorized = 1;
		auth->setUid(res[0].c_str());
		c->close();
		db->close();
		res.clear();
		delete c;
		delete db;
	}

	try
	{
		// Now, determine if they're an admin or not
		connection* db = new connection(dbname);
		cursor* c = db->getCursor();
		string exec = "SELECT uid FROM " + table_name + " WHERE login_name='";
		exec += username;
		exec += "' AND admin=TRUE";
		c->execute(exec);

		vector<string> res;
		
		// IF we have anything returned, then they're admin. Otherwise, they're not./		i
	if (c->rowcount())
			myAdmin = 1;
	else
			myAdmin = 0;

		c->close();
		db->close();
		res.clear();

		delete c;
		delete db;
	}
	catch (...)
	{
		myAdmin = 0;
	}
}

//getDefaultStore() returns an email store
TucanStore_ptr TucanSession_i::getDefaultStore()
{
	string filename;
	string str;
	TucanStore_ptr store;

	//Are we authorized??
	if(!(myAuthorized))
	{
		throw TucanAuthException(myAuth->getUserName(),
				myAuth->getPassword(),
				myAuth->getPinNumber(),
				"Error : Invalid Authorization");
	}

	//Attempt to get the ior filename
	try
	{
		TucanConfig* config = new TucanConfig();
		filename = config->get("email", "store");
		delete config;
	}
	catch(...)
	{
		throw TucanException("Error: Error getting
				ior filename from TucanConfig in 
				getDefaultStore().");
	}

	//Read in the ior string from the file
	try
	{
		ifstream infile(filename.c_str());
		getline(infile, str);
		infile.close();
	}
	catch(...)
	{
		string excp_str = "Error: Error reading ior file: "
			+ filename + " in getDefaultStore().";
		throw TucanException(excp_str.c_str());
	}

	//Convert the string to a factory object and
	//create() a new store and return it
	try
	{
		CORBA::Object_var object = 
			myOrb->string_to_object(str.c_str());
		TucanStoreFactory_var fact = 
			TucanStoreFactory::_narrow(object);
		store = fact->create();
		store->setAuth(myAuth);
	}
	catch(...)
	{
		throw TucanException("Error: Error creating
				TucanStore object in getDefaultStore().");
	}
	
	return store;
}

//getStoreByProtocol() returns a TucanStore based upon the
//protocol string that is passed to it
TucanStore_ptr TucanSession_i::getStoreByProtocol(const char* proto)
{
	string filename;
	string str;
	TucanStore_ptr store;

	//Are we authorized??
	if(!(myAuthorized))
	{
		throw TucanAuthException(myAuth->getUserName(),
				myAuth->getPassword(),
				myAuth->getPinNumber(),
				"Error : Invalid Authorization");
	}
	//Get the ior filename from TucanConfig
	try
	{
		TucanConfig* config = new TucanConfig();
		filename = config->get((char*)proto, "store");
		delete config;
	}
	catch(...)
	{
		throw TucanException("Error: Error getting
				ior filename from TucanConfig in 
				getStoreByProtocol().");
	}

	//Read in the ior string from the ior file
	try
	{
		ifstream infile(filename.c_str());
		getline(infile, str);
		infile.close();
	}
	catch(...)
	{
		string excp_str = "Error: Error reading ior file: "
			+ filename + " in getStoreByProtocol().";
		throw TucanException(excp_str.c_str());
	}

	//Convert the ior string to a factory object
	//and create a new store to return
	try
	{
		CORBA::Object_var object = 
			myOrb->string_to_object(str.c_str());
		TucanStoreFactory_var fact = 
			TucanStoreFactory::_narrow(object);
		store = fact->create();
		store->setAuth(myAuth);
	}
	catch(...)
	{
		throw TucanException("Error: Error creating
				TucanStore object in getStoreByProtocol().");
	}

	return store;
}

//getDefaultTransport() returns an email transport by default
TucanTransport_ptr TucanSession_i::getDefaultTransport()
{
	string filename;
	string str;
	TucanTransport_ptr transport;

	//Are we authorized?
	if(!(myAuthorized))
	{
		throw TucanAuthException(myAuth->getUserName(),
				myAuth->getPassword(),
				myAuth->getPinNumber(),
				"Error : Invalid Authorization");
	}

	//Get the ior filename from TucanConfig
	try
	{
		TucanConfig* config = new TucanConfig();
		filename = config->get("email", "transport");
		delete config;
	}
	catch(...)
	{
		throw TucanException("Error: Error getting
				ior filename from TucanConfig in 
				getDefaultTransport().");
	}

	//Read the ior string from the file
	try
	{
		ifstream infile(filename.c_str());
		getline(infile, str);
		infile.close();
	}
	catch(...)
	{
		string excp_str = "Error: Error reading ior file: "
			+ filename + " in getDefaultTransport().";
		throw TucanException(excp_str.c_str());
	}

	//Convert the string to a factory object
	//and use it to create a new transport object
	try
	{
		CORBA::Object_var object = 
			myOrb->string_to_object(str.c_str());
		TucanTransportFactory_var fact = 
			TucanTransportFactory::_narrow(object);
		transport = fact->create();
	}
	catch(...)
	{
		throw TucanException("Error: Error creating
				TucanStore object in getDefaultTranport().");
	}

	return transport;
}

//getTransportByProtocol returns a transport object based
//on the protocol that is passed to it
TucanTransport_ptr TucanSession_i::getTransportByProtocol(const char* proto)
{
	string filename;
	string str;
	TucanTransport_ptr transport;

	//Are we authorized??
	if(!(myAuthorized))
	{
		throw TucanAuthException(myAuth->getUserName(),
				myAuth->getPassword(),
				myAuth->getPinNumber(),
				"Error : Invalid Authorization");
	}

	//Get the ior filename from TucanConfig
	try
	{
		TucanConfig* config = new TucanConfig();
		filename = config->get((char*)proto, "transport");
		delete config;
	}
	catch(...)
	{
		throw TucanException("Error: Error getting
				ior filename from TucanConfig in 
				getTransportByProtocol().");
	}

	//Read in the ior string from the file
	try
	{
		ifstream infile(filename.c_str());
		getline(infile, str);
		infile.close();
	}
	catch(...)
	{
		string excp_str = "Error: Error reading ior file: "
			+ filename + " in getTransportByProtocol().";
		throw TucanException(excp_str.c_str());
	}

	//Convert the string to a factory object
	//and use that to create a new transport object
	try
	{
		CORBA::Object_var object = 
			myOrb->string_to_object(str.c_str());
		TucanTransportFactory_var fact = 
			TucanTransportFactory::_narrow(object);
		transport = fact->create();
	}
	catch(...)
	{
		throw TucanException("Error: Error creating
				TucanStore object in getTranportByProtocol().");
	}

	return transport;
}

CORBA::Boolean TucanSession_i::isAdmin()
{
	//Are we authorized??
	if(!(myAuthorized))
	{
		throw TucanAuthException(myAuth->getUserName(),
				myAuth->getPassword(),
				myAuth->getPinNumber(),
				"Error : Invalid Authorization");
	}
	
	return myAdmin;
}

void TucanSession_i::setNewPinNumber(CORBA::Long newpin)
{
	//Are we authorized??
	if(!(myAuthorized))
	{
		throw TucanAuthException(myAuth->getUserName(),
				myAuth->getPassword(),
				myAuth->getPinNumber(),
				"Error : Invalid Authorization");
	}

	string table_name("user_info");
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	delete config;

	// Change the pin bumber 
	myAuth->setPinNumber(newpin);

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();

	try {
		CORBA::String_var uname = myAuth->getUserName();
		char tmp[32];

		snprintf(tmp, 32, "%ld", newpin);

		string query = "update " + table_name + " set pin=";
		query += tmp;
		query += " where login_name='";
		query += uname;
		query += "'";

		c->execute(query.c_str());
	}
	catch (...)
	{
		c->close();
		db->close();
		
		delete c;
		delete db;

		throw TucanException("Error: Error setting new pin number.");
	}

	c->close();
	db->close();
	
	delete c;
	delete db;
}

void TucanSession_i::destroy()
{
	PortableServer::ObjectId_var id = 
		myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

TucanSessionFactory_i::TucanSessionFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = 
		myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanSession_ptr TucanSessionFactory_i::create()
{
	TucanSession_i* mySession = 
		new TucanSession_i(myOrb);
	PortableServer::ObjectId_var mySessionid = 
		myPoa->activate_object(mySession);
	CORBA::Object_var obj = 
		myPoa->servant_to_reference(mySession);
	//TucanSession_var session = mySession->_this();
	mySession->_remove_ref();

	//return session._retn();
	return TucanSession::_narrow(obj);
}
