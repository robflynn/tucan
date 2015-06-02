////////////////////////////////////////////////////////////
// TucanAddressBookTransport (c++ impl)
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
// Filename:        TucanAddressBookTransport_i.cc 
// Description:     The Address Book trasnport interface 
// Author:          Mathew Davis <mattd@difinium.com>
// Created at:      Tue Feb 13 17:03:13 PST 2001 
////////////////////////////////////////////////////////////

#include <TucanAddressBookTransport_i.hh>

TucanAddressBookTransport_i::TucanAddressBookTransport_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

void TucanAddressBookTransport_i::send(TucanMessage_ptr msg)
{
	struct passwd *passwd_struct;
	TucanData_var data;
	string mydata("");
	string mytype("");
	string mygroup("");
	TucanAddressSeq_var recp;
	int type = 1;
	TucanAddressBookMessage_var the_msg;
	int intMax;
	CORBA::String_var temp;

	the_msg = TucanAddressBookMessage::_narrow(msg);

	if(CORBA::is_nil(the_msg))
		type = 0;

	if(type)
	{
		temp = the_msg->getGroup();
		mygroup = temp;

		try
		{
			data = the_msg->getDataObject();
		}
		catch(...)
		{
			throw TucanException("Error: Data object can 
					not be retrieved.");
		}
		
		temp = data->getText();
		mydata = temp;

		temp = data->getContentType();
		mytype = temp;

		if(!(mytype.compare("")))
			mytype = "text/plain";

		mytype = "'" + mytype + "'";
	
		recp = the_msg->getRecipients("TO");
	}
	else
	{
		try
		{
			data = msg->getDataObject();
		}
		catch(...)
		{
			throw TucanException("Error: Data object 
					can not be retrieved.");
		}

		temp = data->getText();
		mydata = temp;

		temp = data->getContentType();
		mytype = temp;

		if(!(mytype.compare("")))
			mytype = "text/plain";
			
		mytype = "'" + mytype + "'";
		
		recp = msg->getRecipients("TO");
	}
	
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("addrdbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	cursor* d = db->getCursor();
	char *tmp;
	tmp = (char*)malloc(32);
	char *tmp2;
	tmp2 = (char*)malloc(32);
	
	vector<string> raw_data_list = strsplit(mydata, '\n');
	
	for(CORBA::ULong i = 0; i < recp->length(); i++)
	{
		temp = recp[i]->getAddress();
		string address(temp);
		
		vector<string> user_list = strsplit(address, '@');
		string username = user_list[0];
		passwd_struct = getpwnam(username.c_str());
		int int_uid = 0;
	       
		if (!(passwd_struct))
		{
			string error = "Error: User " + username + " is unknown.";
			cout << error << endl;
		}
		
		else
		{
			int_uid = passwd_struct->pw_uid;
			sprintf(tmp, "%d", (int_uid));
			string uid = tmp;

			string cardNum = "SELECT MAX(card) FROM " +
				tablename + " WHERE uid=" + uid +
				" AND \"group\"='" + mygroup + "'";

			d->execute(cardNum);
			
			vector<string> result = d->fetchone();
			string stringMax = result[0];
			intMax = atoi(stringMax.c_str());
			intMax++;
			sprintf(tmp2, "%d", (intMax));
			string card_no = tmp2;

			for(CORBA::ULong j = 0; j < raw_data_list.size(); j++)
			{
				int subscript = raw_data_list[j].find(": ");
				string key_data =
					raw_data_list[j].substr(0, subscript);
				string value_data =
					raw_data_list[j].substr(subscript + 2,
					raw_data_list[j].size() - (subscript + 2));
		
				mygroup = escape(mygroup);
				key_data = escape(key_data);
				value_data = escape(value_data);
				
				string exec = "INSERT INTO " + tablename + 
					" VALUES(" + uid + ", '" + mygroup + "', " +
					card_no + ", '" + key_data + "', '" +
					value_data + "')";

				c->execute(exec);
			}

			result.clear();
		}

		user_list.clear();
	}
	
	free(tmp);
	free(tmp2);
	raw_data_list.clear();
	c->close();
	d->close();
	db->close();

	delete c;
	delete d;
	delete db;
}

void TucanAddressBookTransport_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

TucanAddressBookTransportFactory_i::TucanAddressBookTransportFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanTransport_ptr TucanAddressBookTransportFactory_i::create()
{
	TucanAddressBookTransport_i* myObject = new TucanAddressBookTransport_i(myOrb);
	PortableServer::ObjectId_var myId = myPoa->activate_object(myObject);
	CORBA::Object_var obj = myPoa->servant_to_reference(myObject);
	myObject->_remove_ref();

	return TucanAddressBookTransport::_narrow(obj);
}
