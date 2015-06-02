////////////////////////////////////////////////////////////
// TucanAddressBookStore (c++ impl)
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
// Filename:        TucanAddressBookStore_i.cc 
// Description:     The Address Book store interface 
// Author:          Mathew Davis <mattd@difinium.com>
// Created at:      Tue Feb 13 11:13:17 PST 2001 
////////////////////////////////////////////////////////////

#include <TucanAddressBookStore_i.hh>
#include <TucanConfig.hh>
#include <TucanDb.hh>
#include <TucanUtilities.hh>
#include <stdio.h>
#include <fstream.h>
#include <time.h>
#include <string>
#include <vector>

TucanAddressBookStore_i::TucanAddressBookStore_i(CORBA::ORB_var orb)
{
	myName = "None";
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);		
}

char* TucanAddressBookStore_i::getName()
{
	return CORBA::string_dup(myName.c_str());
}

void TucanAddressBookStore_i::setName(const char* name)
{
	myName = name;
}

TucanFolder_ptr TucanAddressBookStore_i::getDefaultFolder()
{
	TucanFolder_ptr folder = getNewFolderObject();
	folder->setName("Default");
	return folder;
}

TucanFolder_ptr TucanAddressBookStore_i::getFolder(const char* name)
{
	TucanFolder_ptr folder = getNewFolderObject();
	folder->setName(name);
	return folder;
}

TucanFolder_ptr TucanAddressBookStore_i::getNewFolderObject()
{
	TucanConfig* config = new TucanConfig();
	string filename = config->get("addressbook", "folder");
	delete config;
	string str;
	TucanFolder_ptr folder;

	ifstream infile(filename.c_str());
	getline(infile, str);
	infile.close();

	CORBA::Object_var object = myOrb->string_to_object(str.c_str());
	TucanFolderFactory_var fact = TucanFolderFactory::_narrow(object);

	folder = fact->create();
	folder->setAuth(myAuth);

	return folder;
}

TucanStringList* TucanAddressBookStore_i::listFolders()
{
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("addrdbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	CORBA::String_var temp = myAuth->getUid();
	string uid(temp);

	string exec = "SELECT DISTINCT \"group\" FROM " +
		tablename + " WHERE uid=" + uid;
	c->execute(exec);
	vector< vector<string> > result = c->fetchall();
	c->close();
	db->close();
	delete db;
	delete c;

	TucanStringList* master_list = new TucanStringList;
	master_list->length(result.size());

	for(int i = 0; i < (signed)result.size(); i++)
		(*master_list)[i] = CORBA::string_dup(result[i][0].c_str());

	result.clear();

	return master_list;		
}

TucanAuth_ptr TucanAddressBookStore_i::getAuth()
{
	return TucanAuth::_duplicate(myAuth);
}

void TucanAddressBookStore_i::setAuth(TucanAuth_ptr auth)
{
	myAuth = TucanAuth::_duplicate(auth);
}

void TucanAddressBookStore_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

TucanAddressBookStoreFactory_i::TucanAddressBookStoreFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanStore_ptr TucanAddressBookStoreFactory_i::create()
{
	TucanAddressBookStore_i* myAddressBookStore = 
		new TucanAddressBookStore_i(myOrb);
	PortableServer::ObjectId_var myAddressBookStoreID = 
		myPoa->activate_object(myAddressBookStore);
	CORBA::Object_var obj =
		myPoa->servant_to_reference(myAddressBookStore);
	myAddressBookStore->_remove_ref();

	return TucanAddressBookStore::_narrow(obj);
}
