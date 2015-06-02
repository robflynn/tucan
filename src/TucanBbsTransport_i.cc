////////////////////////////////////////////////////////////
// TucanBbsTransport (c++ header)
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
// Filename:        TucanBbsTransport_i.hh 
// Description:     The Bbs transport interface 
// Author:          Rob Flynn <rob@difinium.com>
// Created at:      Fri Apr  6 11:29:20 PDT 2001
////////////////////////////////////////////////////////////

#include <TucanBbsTransport_i.hh>

TucanBbsTransport_i::TucanBbsTransport_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj =
		myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

void TucanBbsTransport_i::send(TucanMessage_ptr msg)
{
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("bbsdbtable", "");
	delete config;
	short bid, type = 0;
	long pid;
	char tmp[64];
	CORBA::String_var fullAddress, temp, body, tempSubject;
	string subject("");

	TucanBbsMessage_var the_msg = TucanBbsMessage::_narrow(msg);
	string query("");

	if (!CORBA::is_nil(the_msg))
	{
		TucanAddress_var addr;
		TucanData_var dob;

		addr = the_msg->getFrom();
		if (CORBA::is_nil(addr))
			throw TucanException("Error: No source address specified");
		
		fullAddress = addr->getAddress();

		dob = the_msg->getDataObject();
		temp = dob->getContentType();

		// We're not sending text.  Lets get out of here.
		if (strncasecmp("text/", temp, 5))
			return;

		body = dob->getText();

		tempSubject = the_msg->getSubject();

		if ((strlen(tempSubject) >= 1) && (strcasecmp("", tempSubject)))
		{
			subject = tempSubject;
		}
		else
		{
			subject = "No Subject";
		}
		
		bid = the_msg->getBid();
		pid = the_msg->getPid();
		type = the_msg->getAccessType();

	}
	else
	{
		//FIXME: Figure out best way to send other types of messages through BBS
		return;
	}
	
	string strbody(body);

	subject = escape(subject);
	fullAddress = escape(fullAddress);
	strbody = escape(strbody);

	// Parse out the username and email address from the string 'username <email_address>'
	string tempAddress(fullAddress);
	vector<string> info = strsplit(tempAddress, ' ');
	string username = info[0];
	string email = info[1];
	info.clear();
	email = email.substr(1, (email.length() - 2));
	
	snprintf(tmp, 8192, "INSERT INTO %s VALUES(%d, %ld, %d,'", tablename.c_str(), bid, pid, type);

	query = tmp;
	query += username;
	query += "','";
	query += email;
	query += "','";
	query += subject;
	query += "','";
	query += strbody;
	query += "',CURRENT_DATE, CURRENT_TIME)";

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();

	c->execute(query.c_str());

	c->close();
	db->close();

	delete c;
	delete db;
}

void TucanBbsTransport_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

TucanBbsTransportFactory_i::TucanBbsTransportFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj =
		myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanTransport_ptr TucanBbsTransportFactory_i::create()
{
	TucanBbsTransport_i* myObject = new TucanBbsTransport_i(myOrb);
	PortableServer::ObjectId_var myId = myPoa->activate_object(myObject);
	CORBA::Object_var obj = myPoa->servant_to_reference(myObject);
	myObject->_remove_ref();

	return TucanBbsTransport::_narrow(obj);
}
