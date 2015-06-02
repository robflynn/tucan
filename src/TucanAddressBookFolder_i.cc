////////////////////////////////////////////////////////////
// TucanAddressBookFolder (c++ impl)
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
// Filename:        TucanAddressBookFolder_i.cc 
// Description:     The Address Book folder interface 
// Author:          Mathew Davis <mattd@difinium.com>
// Created at:      Tue Feb 13 14:23:40 PST 2001 
////////////////////////////////////////////////////////////

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>
#include <time.h>
#include <string>
#include <vector>
#include <TucanUtilities.hh>
#include <TucanConfig.hh>
#include <TucanDb.hh>
#include <TucanAddressBookFolder_i.hh>

TucanAddressBookFolder_i::TucanAddressBookFolder_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

CORBA::Boolean TucanAddressBookFolder_i::exists()
{
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("addrdbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	CORBA::String_var temp = myAuth->getUid();
	string uid(temp);
	
	string exec = "SELECT * FROM " + tablename 
		+ " WHERE \"group\"='" + myName + "'"
		+ " AND uid=" + uid;

	c->execute(exec);

	int rows = c->rowcount();
	
	c->close();
	db->close();
	delete db;
	delete c;

	if(rows > 0)
		return 1;
	else
		return 0;
}

CORBA::Boolean TucanAddressBookFolder_i::create()
{
	throw TucanException("Error: Method create() has not been implemented.");
}

CORBA::Boolean TucanAddressBookFolder_i::deleteFolder()
{
	if(!(exists()))
	{
		string exec_string = "Error: Folder " + myName 
			+ " does not exist.";
		throw TucanException(exec_string.c_str());
	}

	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("addrdbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	CORBA::String_var temp = myAuth->getUid();
	string uid(temp);

	string exec = "DELETE FROM " + tablename + 
		" WHERE uid=" + uid
		+ " AND \"group\"='" + myName + "'";
	c->execute(exec);
		
	c->close();
	db->close();
	delete db;
	delete c;
	
	return 1;
}

TucanMessageSeq* TucanAddressBookFolder_i::expunge()
{
	throw TucanException("Error: Method expunge() has not been implemented.");
}

TucanFolder_ptr TucanAddressBookFolder_i::getFolder(const char* name)
{
	TucanFolder_ptr folder = getNewFolderObject();
	folder->setName(name);
	return folder;
}

TucanFolder_ptr TucanAddressBookFolder_i::getNewFolderObject()
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

void TucanAddressBookFolder_i::setParent(TucanFolder_ptr folder)
{
	throw TucanException("Error: Method setParent() has not been implemented.");
}

TucanFolder_ptr TucanAddressBookFolder_i::getParent()
{
	throw TucanException("Error: Method getParent() has not been implemented.");
}

TucanStringList* TucanAddressBookFolder_i::listFolders()
{
	throw TucanException("Error: Method listFolders() has not been implemented.");
}

TucanFolderSeq* TucanAddressBookFolder_i::list()
{
	TucanStringList_var folder_list = listFolders();
	TucanFolderSeq* master_list = new TucanFolderSeq;
	master_list->length(folder_list->length());

	for(CORBA::ULong i = 0; i < folder_list->length(); i++)
	{
		TucanFolder_var folder = getNewFolderObject();
		folder->setName(folder_list[i]);
		(*master_list)[i] = TucanFolder::_duplicate(folder);
	}

	return master_list;
}

CORBA::Boolean TucanAddressBookFolder_i::renameTo(const char* name)
{
	throw TucanException("Error: Method renameTo() has not been implemented.");
}

char* TucanAddressBookFolder_i::getName()
{
	return CORBA::string_dup(myName.c_str());
}

void TucanAddressBookFolder_i::setName(const char* name)
{
	myName = name;
}

char* TucanAddressBookFolder_i::getFullName()
{
	return CORBA::string_dup(myFullName.c_str());
}

void TucanAddressBookFolder_i::setFullName(const char* name)
{
	myFullName = name;
}

TucanMessage_ptr TucanAddressBookFolder_i::getMessage(CORBA::Long msgnum)
{
	if(!(exists()))
	{
		string excp_string = "Error: Folder " + myName 
			+ " does not exist.";
		throw TucanException(excp_string.c_str());
	}

	CORBA::Long max = getMessageCount();

	if((msgnum > max) || (msgnum <= 0))
	{
		string excp_string = "Error: Requested message out of range.";
		throw TucanException(excp_string.c_str());
	}
	
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("addrdbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	CORBA::String_var uid_temp = myAuth->getUid();
	string uid(uid_temp);
	char temp[32];
	sprintf(temp, "%ld", (msgnum));
	string msg_num = temp;

	string exec = "SELECT * FROM " + tablename
		+ " WHERE uid=" + uid + " AND card=" 
		+ msg_num + " AND \"group\"='" + myName + "'";

	c->execute(exec);

	vector< vector<string> > result = c->fetchall();
	c->close();
	db->close();
	delete c;
	delete db;

	TucanMessage_var msg = getNewMessageObject();
	TucanAddressBookMessage_ptr the_msg = TucanAddressBookMessage::_narrow(msg);
	the_msg->setMessageNumber(msgnum);
	the_msg->setGroup(myName.c_str());
	the_msg->setDataObject(the_msg->getNewDataObject());
	
	//May need to set more attributes of the_msg
	
	for(int i = 0; i < (signed)result.size(); i++)
	{
		// New line added to set the attributes in hash
		the_msg->setAttribute(result[i][3].c_str(), result[i][4].c_str());
	}

	result.clear();

	return the_msg;
}

TucanMessageSeq* TucanAddressBookFolder_i::getMessages()
{
	if(!(exists()))
	{
		string excp_string = "Error: Folder " + myName 
			+ " does not exist.";
		throw TucanException(excp_string.c_str());
	}

	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("addrdbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	CORBA::String_var uid_temp = myAuth->getUid();
	string uid(uid_temp);

	string exec = "SELECT * FROM " + tablename
		+ " WHERE uid=" + uid +
		" AND \"group\"='" + myName + "'"
		+ " AND key='name' ORDER BY value asc";

	c->execute(exec);

	vector< vector<string> > result = c->fetchall();
	c->close();
	db->close();
	delete c;
	delete db;
	
	long count, i;
	TucanMessageSeq* list = new TucanMessageSeq;
	TucanMessage_var message;

	count = getMessageCount();

	list->length(count);

	for (i = 0; i < count; i++)
	{
		CORBA::Long mnum = atoi(result[i][2].c_str());
		message = getMessage(mnum);
		(*list)[i] = TucanMessage::_duplicate(message);
	}

	return list;
}

TucanMessageSeq* TucanAddressBookFolder_i::getNewMessages()
{
	throw TucanException("Error: Method getNewMessages() has not been implemented.");
}

TucanMessageSeq* TucanAddressBookFolder_i::getMessagesInRange
	(CORBA::Long start, CORBA::Long stop)
{
	throw TucanException("Error: Method getMessagesInRange() has not been implemented.");
}

TucanMessage_ptr TucanAddressBookFolder_i::getNewMessageObject()
{
	TucanConfig* config = new TucanConfig();
	string filename = config->get("addressbook", "message");
	delete config;
	string str;
	TucanMessage_ptr message;

	ifstream infile(filename.c_str());
	getline(infile, str);
	infile.close();

	CORBA::Object_var object = myOrb->string_to_object(str.c_str());
	TucanMessageFactory_var fact = TucanMessageFactory::_narrow(object);
	message = fact->create();

	return message;
}

CORBA::Long TucanAddressBookFolder_i::getMessageCount()
{
	if(!(exists()))
	{
		string excp_string = "Error: Folder " + myName 
			+ " does not exist.";
		throw TucanException(excp_string.c_str());
	}
	
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("addrdbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	CORBA::String_var uid_temp = myAuth->getUid();
	string uid(uid_temp);

	string exec = "SELECT DISTINCT CARD FROM " + tablename
		+ " WHERE \"group\"='" + myName + "'"
		+ " AND uid=" + uid;
	c->execute(exec);
	
	CORBA::Long num = c->rowcount();
	c->close();
	db->close();
	delete c;
	delete db;

	return num;
}

CORBA::Long TucanAddressBookFolder_i::getNewMessageCount()
{
	throw TucanException("Error: Method getNewMessageCount() has not been implemented.");
}

CORBA::Boolean TucanAddressBookFolder_i::hasNewMessages()
{
	throw TucanException("Error: Method hasNewMessages() has not been implemented.");
}

void TucanAddressBookFolder_i::moveMessage(CORBA::Long msgnum, 
		TucanFolder_ptr folder)
{
	throw TucanException("Error: Method moveMessage() has not been implemented.");
}

void TucanAddressBookFolder_i::moveMessages(const TucanLongList& msgnums, 
		TucanFolder_ptr folder)
{
	throw TucanException("Error: Method moveMessages() has not been implemented.");
}

void TucanAddressBookFolder_i::copyMessage(CORBA::Long msgnum, 
		TucanFolder_ptr folder)
{
	throw TucanException("Error: Method copyMessage() has not been implemented.");
}

void TucanAddressBookFolder_i::copyMessages(const TucanLongList& msgnums, 
		TucanFolder_ptr folder)
{
	throw TucanException("Error: Method copyMessages() has not been implemented.");
}

void TucanAddressBookFolder_i::appendMessage(TucanMessage_ptr msg)
{
	throw TucanException("Error: Method appendMessage() has not been implemented.");
}

void TucanAddressBookFolder_i::appendMessages(const TucanMessageSeq& msgs)
{
	throw TucanException("Error: Method appendMessages() has not been implemented.");
}

void TucanAddressBookFolder_i::addMessage(TucanMessage address)
{
	throw TucanException("Error: Method addMessage() has not been implemented.");
}

void TucanAddressBookFolder_i::addMessages(TucanMessageSeq addresses)
{
	throw TucanException("Error: Method addMessages() has not been implemented.");
}

void TucanAddressBookFolder_i::deleteMessage(CORBA::Long msgnum)
{	
	if(!(exists()))
	{
		string excp_string = "Error: Folder " + myName 
			+ " does not exist.";
		throw TucanException(excp_string.c_str());
	}

	CORBA::Long max = getMessageCount();

	if((msgnum > max) || (msgnum <= 0))
	{
		string excp_string = "Error: Requested message out of range.";
		throw TucanException(excp_string.c_str());
	}
	
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("addrdbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	CORBA::String_var uid_temp = myAuth->getUid();
	string uid(uid_temp);
	char temp[32];
	sprintf(temp, "%ld", (msgnum));
	string msg_num = temp;

	string exec = "DELETE FROM " + tablename
		+ " WHERE uid=" + uid + " AND card=" 
		+ msg_num + " AND \"group\"='" + myName + "'";
	c->execute(exec);

	string update = "UPDATE " + tablename
		+ " SET card = card - 1 WHERE uid=" + uid +
		" AND \"group\"='" + myName + "' AND card > "
		+ msg_num;
	c->execute(update);

	c->close();
	db->close();
	delete c;
	delete db;
}


TucanAuth_ptr TucanAddressBookFolder_i::getAuth()
{
	return TucanAuth::_duplicate(myAuth);
}

void TucanAddressBookFolder_i::setAuth(TucanAuth_ptr auth)
{
	myAuth = TucanAuth::_duplicate(auth);
}

void TucanAddressBookFolder_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);

	myPoa->deactivate_object(id);
}

TucanAddressBookFolderFactory_i::TucanAddressBookFolderFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj =  myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanFolder_ptr TucanAddressBookFolderFactory_i::create()
{
	TucanAddressBookFolder_i* myAddressBookFolder = 
		new TucanAddressBookFolder_i(myOrb);
	PortableServer::ObjectId_var myAddressBookFolderId =
		myPoa->activate_object(myAddressBookFolder);
	CORBA::Object_var obj = 
		myPoa->servant_to_reference(myAddressBookFolder);
	myAddressBookFolder->_remove_ref();
	
	return TucanAddressBookFolder::_narrow(obj);
}
