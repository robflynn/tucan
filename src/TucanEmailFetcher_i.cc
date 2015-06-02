//////////////////////////////////////////////////////////
// TucanEmailFetcher_i (c++ impl)  
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
// Filename:		TucanEmailFetcher_i.cc
// Description:		TucanEmailFetcher for CORBA implementation 
// Author:		Rob Flynn <rob@difinium.com>
// Created at:		Mon Feb 05 11:27:30 PST 2001
//////////////////////////////////////////////////////////

#include <TucanEmailFetcher_i.hh>
#include <TucanUtilities.hh>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include "TucanParse.hh"
#include "TucanPop3.hh"
#include "TucanIMAP.hh"
#include "TucanMBOX.hh"

#define HISTORY_FILE ".uidl_history"

TucanEmailFetcher_i::TucanEmailFetcher_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
	myServer = "localhost";
	myProto = "POP3";
	myMailer = NULL;
}

void TucanEmailFetcher_i::open()
{
	if (!myProto.compare("POP3"))
		myMailer = new TucanPop3(myServer);
	else if (!myProto.compare("IMAP"))
		myMailer = new TucanIMAP(myServer);
	else if (!myProto.compare("MBOX"))
		myMailer = new TucanMBOX();
	else
		throw("Error: Protocol '"+myProto+"' is unkown.");

	myMailer->open(myName, myPassword);
}

void TucanEmailFetcher_i::close()
{
	myMailer->close();
}

TucanAuth_ptr TucanEmailFetcher_i::getAuth()
{
	return TucanAuth::_duplicate(myAuthObject);
}

void TucanEmailFetcher_i::setAuth(TucanAuth_ptr myAuth)
{
	myAuthObject = TucanAuth::_duplicate(myAuth);
}

void TucanEmailFetcher_i::setServer(const char* server)
{
	myServer = server;
}

char* TucanEmailFetcher_i::getServer()
{
	return CORBA::string_dup(myServer.c_str());
}

void TucanEmailFetcher_i::setPort(CORBA::Short port)
{
	myPort = port;
}

CORBA::Short TucanEmailFetcher_i::getPort()
{
	return myPort;
}

void TucanEmailFetcher_i::setProtocol(const char* proto)
{
	myProto = proto;
}

char* TucanEmailFetcher_i::getProtocol()
{
	return CORBA::string_dup(myProto.c_str());
}


void TucanEmailFetcher_i::setName(const char* name)
{
	myName = name;
}

char* TucanEmailFetcher_i::getName()
{
	return CORBA::string_dup(myName.c_str());
}

void TucanEmailFetcher_i::setPassword(const char* password)
{
	myPassword = password;
}

char* TucanEmailFetcher_i::getPassword()
{
	return CORBA::string_dup(myPassword.c_str());
}

CORBA::Short TucanEmailFetcher_i::getMessageCount()
{
	return myMailer->getMessageCount();
}

CORBA::Short TucanEmailFetcher_i::getMessageSize(CORBA::Short num)
{
	int i;

	i = getMessageCount();

	if (num > i)
		throw TucanException("Error: Requested message out of range.");

	
	return myMailer->getMessageSize(num);
}
		
char* TucanEmailFetcher_i::getMessageId(CORBA::Short num)
{
	int i;
	string value;

	i = getMessageCount();

	if (num > i)
		throw TucanException("Error: Requested message out of range.");

	value = myMailer->getMessageId(num);
	
	return CORBA::string_dup(value.c_str());
}

// This message will return 1 or 0 depending on whether or not the message
// is currently cached (has been downloaded).

int find_message_by_uidl(string dir, string id)
{
	string tmp;
	
	tmp = dir;
	tmp += "/";
	tmp += HISTORY_FILE;

	ifstream history(tmp.c_str());

	// Something bad happened, hehe. The file probably doesnt exist.
	if (history.bad())
	{
		history.close();
		return 0;
	}

	while (!history.eof())
	{
		// Read a line
		getline(history, tmp);

		// Cut off the line breaks and such
		tmp = strchomp(tmp);

		// And see if they match. If so, return 1.
		if (!tmp.compare(id))
			return 1;
	}

	history.close();

	return 0;
}

// This method will cache a UIDL of a message
void cache_uidl(string dir, string id)
{
	string tmp;
	
	tmp = dir;
	tmp += "/";
	tmp += HISTORY_FILE;

	// Open the file for appending
	ofstream history(tmp.c_str(), ios::app);

	if (history.bad())
	{
		// The file doesnt exist.  Let's try to create it
		history.close();
		ofstream history(tmp.c_str(), ios::out);

		if (history.bad())
		{
			// If we're still here, something very wrong happend.
			// FIXME: Should we abort here??? Or raise an exception? Or what?
			return;
		}
	}

	// Write the ID
	history << id << endl;

	// And exit
	history.close();
}

// This method will tell you if a message has already been downloaded

CORBA::Short TucanEmailFetcher_i::isRead(CORBA::Short num)
{
	CORBA::String_var temp = myAuthObject->getUserName();
	string msgId = myMailer->getMessageId(num);
	string tmp(temp);
	tmp += "/tucan_email";

	return find_message_by_uidl(tmp, msgId);
}

CORBA::Short TucanEmailFetcher_i::transfer(CORBA::Short num)
{
	string tmp("");
	ofstream message;
	string msgId;
	CORBA::String_var temp = myAuthObject->getUserName();

	tmp = gethomedir(temp);
	tmp += "/tucan_email";

	msgId = myMailer->getMessageId(num);

	// Now, try to find out if the message has been downloaded before.
	// If it has, don't download it again.
	
	if (find_message_by_uidl(tmp, msgId))
		return 2;
	
	// If we've not downloaded it yet, cache that fool :-)
	cache_uidl(tmp, msgId);

	// Looks like everything turned out ok.
	tmp += "/inbox/";
	tmp += msgId;

	// Get our message
	string myMsg = myMailer->getMessage(num);

	// And write it
	message.open(tmp.c_str(), ios::out);
	message << myMsg << endl;
	message.close();

	return 1;
}

void TucanEmailFetcher_i::transferAll()
{
	unsigned int i;
	unsigned int j;
	
	i = getMessageCount();

	for (j = 0; j < i; j++)
		transfer(j+1);
}

void TucanEmailFetcher_i::deleteMessage(CORBA::Short num)
{
	int i;
	string value;

	i = getMessageCount();

	if (num > i)
		throw TucanException("Error: Requested message out of range.");

	myMailer->deleteMessage(num);
	
}

void TucanEmailFetcher_i::deleteAllMessages()
{
	myMailer->deleteAllMessages();
}


void TucanEmailFetcher_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);

	if (myMailer)
		delete myMailer;

}


// Factory


TucanEmailFetcherFactory_i::TucanEmailFetcherFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanEmailFetcher_ptr TucanEmailFetcherFactory_i::create()
{
	TucanEmailFetcher_i* myObject = new TucanEmailFetcher_i(myOrb);
	PortableServer::ObjectId_var myId = myPoa->activate_object(myObject);
	CORBA::Object_var obj = myPoa->servant_to_reference(myObject);
	myObject->_remove_ref();

	return TucanEmailFetcher::_narrow(obj);
}
