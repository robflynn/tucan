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

#include "TucanUserPrefs_i.hh"

TucanUserPrefs_i::TucanUserPrefs_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);

	myAuth = NULL;
}

TucanAuth_ptr TucanUserPrefs_i::getAuth()
{
	return TucanAuth::_duplicate(myAuth);
}

void TucanUserPrefs_i::setAuth(TucanAuth_ptr auth)
{
	myAuth = TucanAuth::_duplicate(auth);
}

TucanAttributeSeq* TucanUserPrefs_i::getAllAttributes()
{
	TucanAttributeSeq* attributes = new TucanAttributeSeq;
	TucanAttribute attribute;
	string query;

	TucanConfig *config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("prefsdbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();

	CORBA::String_var temp = myAuth->getUserName();
	string username(temp);

	// Let's build our request query
	query = "SELECT key,value FROM " + tablename + " WHERE name='" + username + "'";

	// Execute it
	c->execute(query);

	// And then grab the results
	if (c->rowcount())
	{
		vector< vector<string> > results = c->fetchall();

		for (unsigned int i = 0; i < results.size(); i++)
		{
			string key = results[i][0];
			string value = results[i][1];

			attribute.name = CORBA::string_dup(key.c_str());
			attribute.value = CORBA::string_dup(value.c_str());

			attributes->length(i + 1);
			(*attributes)[i] = attribute;
		}

		results.clear();
	}

	c->close();
	db->close();

	delete c;
	delete db;

	return attributes;
}

char* TucanUserPrefs_i::getAttribute(const char* key)
{
	string query;

	TucanConfig *config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("prefsdbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();

	CORBA::String_var temp = myAuth->getUserName();
	string username(temp);

	// Let's build our request query
	query = "SELECT value FROM " + tablename + " WHERE name='" + username + "' AND key='";
	query += key;
	query += "'";

	// Execute it
	c->execute(query);

	// And then grab the results
	if (!c->rowcount())
	{
		string exp;

		exp = "Error: Key '";
		exp += key;
		exp += "' does not exist in user preferences";

		// Close up our database stuff.
		c->close();
		db->close();

		delete c;
		delete db;

		throw TucanException(exp.c_str());
	}

	vector<string> results = c->fetchone();

	string value = results[0];

	results.clear();
	c->close();
	db->close();

	delete c;
	delete db;

	return CORBA::string_dup(value.c_str());
}

void TucanUserPrefs_i::setAttribute(const char* key, const char* value)
{
	string query;
	
	TucanConfig *config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("prefsdbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();

	deleteAttribute(key);

	CORBA::String_var temp = myAuth->getUserName();
	string username(temp);

	username = escape(username);
	string key_str(key);
	string val_str(value);

	key_str = escape(key_str);
	val_str = escape(val_str);
	
	// Now that the item is deleted, we should insert a new one.
	query = "INSERT INTO " + tablename + " VALUES ('" + username + "', '";
	query += key_str;
	query += "','";
	query += val_str;
	query += "')";

	c->execute(query);

	c->close();
	db->close();

	delete c;
	delete db;
}

void TucanUserPrefs_i::deleteAttribute(const char* key)
{
	string query;
	
	TucanConfig *config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("prefsdbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();

	// Now that we have a database and a table, let's make an attempt at
	// deleting the current entry.
	
	CORBA::String_var temp = myAuth->getUserName();
	string username(temp);

	query = "DELETE FROM " + tablename + " WHERE name='" + username + "' and key='";
	query += key;
	query += "'";

	c->execute(query);

	c->close();
	db->close();

	delete c;
	delete db;
}

void TucanUserPrefs_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

TucanUserPrefsFactory_i::TucanUserPrefsFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanUserPrefs_ptr TucanUserPrefsFactory_i::create()
{
	TucanUserPrefs_i* myUserPrefs = new TucanUserPrefs_i(myOrb);
	PortableServer::ObjectId_var myUserPrefsMessageId = 
		myPoa->activate_object(myUserPrefs);
	CORBA::Object_var obj = myPoa->servant_to_reference(myUserPrefs);
	myUserPrefs->_remove_ref();

	return TucanUserPrefs::_narrow(obj);
}
