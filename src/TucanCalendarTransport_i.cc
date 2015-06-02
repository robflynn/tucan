////////////////////////////////////////////////////////////
// TucanCalendarTransport (c++ impl)
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
// Filename:        TucanCalendarTransport_i.cc 
// Description:     The Calendar trasnport interface 
// Author:          Jared Peterson <jared@difinium.com>
// Created at:      Wed Feb  7 15:49:10 PST 2001 
////////////////////////////////////////////////////////////

#include <TucanCalendarTransport_i.hh>

TucanCalendarTransport_i::TucanCalendarTransport_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj =
		myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

void TucanCalendarTransport_i::send(TucanMessage_ptr msg)
{
	struct passwd *passwd_struct;
	TucanData_var data;
	TucanRawData *raw_data;
	string mydata("");
	string mynotifydate("");
	string mynotifytime("");
	string notification("");
	string mytype("");
	string filename("");
	CORBA::String_var temp;
	CORBA::Boolean notified;
	TucanAddressSeq_var recp;
	int type = 1;
	TucanCalendarMessage_var the_msg;
	char buf[64];
	CORBA::String_var tmp_add;
	int fd;

	the_msg = TucanCalendarMessage::_narrow(msg);
	
	if(CORBA::is_nil(the_msg))
		type = 0;

	if(type)
	{
		try
		{
			data = the_msg->getDataObject();
		}
		catch(...)
		{
			throw TucanException("Error: Data object can 
					not be retrieved.");
		}
	
		raw_data = the_msg->getData();
		
		temp = data->getText();	
		mydata = temp;

		temp = data->getContentType();
		mytype = temp;

		if(!(mytype.compare("")))
			throw TucanException("Error: Please set the 
					content-type for the message.");

		mytype = "'" + mytype + "'";

		// Get our recipient list
		
		recp = the_msg->getRecipients("TO");
		
		// Are we dealing with an audio calendar message?	
		
		if(!(mytype.compare("'audio/x-wav'")))
		{
			snprintf(buf, 64, "msg%ld.wav", time((time_t)NULL));
		
			//Set our calendar data to the filename
			
			mydata = buf;
			
			// Now we need to save the data to a file

			unsigned char *tmp_data;
			tmp_data = (unsigned char *)malloc(raw_data->length() * sizeof(unsigned char));

			for (unsigned long i = 0; i < raw_data->length(); i++)
				tmp_data[i] = (*raw_data)[i];	

			for(unsigned long i = 0; i < recp->length(); i++)
			{
				CORBA::String_var tmp_add;

				tmp_add = recp[i]->getAddress();
		
				if (!gethomedir(tmp_add))
					continue;

				filename = gethomedir(tmp_add);
				filename += "/cal/";
				filename += buf;
		
				fd = open(filename.c_str(), O_WRONLY | O_CREAT, 
						S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

				write(fd, tmp_data, raw_data->length());
				close(fd);

			}

			free(tmp_data);
		}
				
		try
		{
			CORBA::String_var tempdate;

			tempdate = the_msg->getNotifyDate();

			if ((strlen(tempdate) > 1) && (strcasecmp("NULL", tempdate)))
			{
				mynotifydate = tempdate;
				mynotifydate = "'" + mynotifydate + "'";
			}
			else
				mynotifydate = "NULL";

			tempdate = the_msg->getNotifyTime();

			if ((strlen(tempdate) > 1) && (strcasecmp("NULL", tempdate)))
			{
				mynotifytime = tempdate;
				mynotifytime = "'" + mynotifytime + "'";
			}
			else
				mynotifytime = "NULL";

			notified = the_msg->isNotified();

			temp = the_msg->getNotification();
			notification = temp;

			if (!mynotifydate.compare("''"))
				mynotifydate = "NULL";
			if (!mynotifytime.compare("''"))
				mynotifytime = "NULL";
		}
		catch(...)
		{
			throw TucanException("Error: Error retrieving 
					attribute.");
		}
	
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
		mynotifydate = "CURRENT_DATE";
		mynotifytime = "CURRENT_TIME";
		notified = 0;
		notification = "none";

		recp = msg->getRecipients("TO");
	}

	//determine notification method
	string notify_int;
	
	if(!(notification.compare("email")))
		notify_int = "1";
	else if(!(notification.compare("phone")))
		notify_int = "2";
	else if(!(notification.compare("fax")))
		notify_int = "3";
	else if(!(notification.compare("all")))
		notify_int = "4";
	else	
		notify_int = "0";
	
	//Have we been notified?
	string not_var;

	if(notified)
		not_var = "'t'";
	else
		not_var = "'f'";

	CORBA::String_var recvdate = the_msg->getReceivedDate("%m/%d/%y");
	CORBA::String_var recvtime = the_msg->getTime();

	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("caldbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	char *tmp;
	tmp = (char*)malloc(32);

	CORBA::ULong len = recp->length();

	for(CORBA::ULong i = 0; i < len; i++)
	{
		CORBA::String_var tempstr;

		tempstr = recp[i]->getAddress();
		string temprecp(tempstr);

		vector<string> user_list = strsplit(temprecp, '@');

		string username = user_list[0];
		passwd_struct = getpwnam(username.c_str());
		int int_uid = passwd_struct->pw_uid;
		sprintf(tmp, "%d", (int_uid));
		string uid(tmp);

		mydata = escape(mydata);

		string exec = "INSERT INTO " + tablename + 
			" VALUES(" + uid + ", '";
	       	exec +=	recvdate;
		exec += "', '";
		exec +=	recvtime;
	      	exec += "', '" +
			mydata + "', " + notify_int + ", " + mynotifydate + ", " +
			mynotifytime + ", " + not_var + ", " + mytype + ", 0)";

		c->execute(exec);

		user_list.clear();
	}
	
	free(tmp);
	c->close();
	db->close();
	delete c;
	delete db;
}

void TucanCalendarTransport_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

TucanCalendarTransportFactory_i::TucanCalendarTransportFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj =
		myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanTransport_ptr TucanCalendarTransportFactory_i::create()
{
	TucanCalendarTransport_i* myObject = new TucanCalendarTransport_i(myOrb);
	PortableServer::ObjectId_var myId = myPoa->activate_object(myObject);
	CORBA::Object_var obj = myPoa->servant_to_reference(myObject);
	myObject->_remove_ref();

	return TucanCalendarTransport::_narrow(obj);
}
