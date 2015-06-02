//////////////////////////////////////////////////////////
// TucanEmailTransport_i (c++ impl)  
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
// Filename:            TucanEmailTransport_i.cc
// Description:	        TucanEmailMessage for CORBA implementation 
// Author:              Rob Flynn <rob@difinium.com>
// Created at:          Fri Jan 25 14:13:25 PST 2001
//////////////////////////////////////////////////////////

#include <TucanEmailTransport_i.hh>
#include <TucanUtilities.hh>
#include <iostream.h>
#include <string>
#include <stdio.h>
#include <time.h>

TucanEmailTransport_i::TucanEmailTransport_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

void TucanEmailTransport_i::send(TucanMessage_ptr msg)
{
	unsigned int i;
	unsigned int pc;

	FILE *mail;
	char *rectypes[3] = {"To","Cc","Bcc"};

	string line("");

	TucanAddress_var fromAddr = msg->getFrom();
	CORBA::String_var from = fromAddr->getAddress();
	CORBA::String_var tmp;
	CORBA::String_var address;
	
	mail = popen("/usr/sbin/sendmail -t", "w");

	line = "From: ";
        line += from;
	line +="\n";

	TucanAddress_var replyAddr = msg->getReplyTo();

	if (!CORBA::is_nil(replyAddr))
	{
		CORBA::String_var replyto = replyAddr->getAddress();

		if (strlen(replyto) > 0)
		{
			line += "Reply-To: ";
			line += replyto;
			line += "\n";
		}
	}

	for (unsigned int j = 0; j < sizeof(rectypes)/sizeof(char *); j++)
	{
		TucanAddressSeq_var addresses;

		addresses = msg->getRecipients(rectypes[j]);

		if (addresses->length() > 0)
		{
			line += rectypes[j];
			line += ": ";

			for (i = 0; i < addresses->length(); i++)
			{
				if (i > 0)
					line += ",";

				address = addresses[i]->getAddress();
				line += address;
			}

			line += "\n";
		}

	}

	// Get the subject
	tmp = msg->getSubject();

	line += "Subject: ";
	line += tmp;
	line +="\n";	

	line += "X-Mailer: TUCAN/SAM\n";
	line += "X-Secret-To-Life: Software and meatloaf, mmmm.\n";

	TucanData_var dob = msg->getDataObject();

	pc = dob->getPartCount();

	if (pc > 0)
	{
		CORBA::String_var type = dob->getContentType();

		char buf[128];
		
		// FIXME: This needs to be better so that it KNOWS it's boundary is unique
		snprintf(buf, 128, "%ld", time((time_t)NULL));
		string boundary = "--tucan";
		boundary += buf;
	        boundary += "MPx--";
		
		
		// If it's not multipart, it should be!
		if (strncasecmp(type, "multipart/", 10))
			type = CORBA::string_dup("multipart/mixed");
			
		line += "Content-Type: ";
		line +=	type;

		line += ";\n\tboundary=\"" + boundary;
		line += "\"\n\n";

		line += "This is a multipart message in MIME format.\n\n";
			
		for (i = 0; i < pc; i++)
		{	
			TucanData::TucanPart_var part;
			string part_data("");

			line += "--" + boundary + "\n";
			line += "Content-Type: ";

			part = dob->getPart(i);

			tmp = part->type;

			if (strlen(tmp) > 1)
				line +=	tmp;
			else
				line += "text/plain";

			line += "\n";

			// We should write all of the header stuff now
			unsigned int y;
			
			for (y = 0; y < part->header.length(); y++)
			{
				line += part->header[y];
				line += "\n";
			}
		
			char *rd = (char *)malloc(sizeof(char *) * part->data.length());

			bzero(rd, sizeof(char *) * part->data.length());
			
			for (unsigned long dc = 0; dc < part->data.length(); dc++)
				rd[dc] = part->data[dc];

			if (strlen(part->encoding) > 0)
			{
				if (!strcasecmp(part->encoding, "base64"))
				{
					// Yeah, we should probably encode it.
					part_data += encodeBase64(rd, part->data.length());
				}
			}
			else if (!strcasecmp(part->encoding, "7bit"))
			{
				part_data += rd;
			}
			else if ((strncasecmp(tmp, "text/", 5) && (strcasecmp(tmp, "message/rfc822"))))
			{
				line += "Content-transfer-encoding: base64\n";
				part_data += encodeBase64(rd, part->data.length());
			}
			else
			{
				part_data += rd;
			}

			free(rd);
		
			// End the header
			line += "\n";	

			// And add the data
			line += part_data; 

			line += "\n";
		}

		line += "--" + boundary + "--\n";

	}
	else
	{
		// The content Type, if needed
		tmp = dob->getContentType();

		CORBA::String_var temptext = dob->getText();

		string datastr("");

		if (!strncasecmp(tmp, "text/", 5) && (strlen(temptext) > 1))
		{
			datastr = temptext;
		}
		else
		{
			TucanRawData* tempdata = msg->getData();

			char *rd = (char *)malloc(sizeof(char *) * tempdata->length());
			
			for (unsigned long dc = 0; dc < tempdata->length(); dc++)
				rd[dc] = (*tempdata)[dc];

			if (strncasecmp("text/", tmp, 5))
			{
				line += "Content-Transfer-Encoding: ";
				line += "base64";
				line += "\n";

				datastr =  encodeBase64(rd, tempdata->length());
			}
			else
			{
				datastr += rd;
			}

			free(rd);

		}

		if (strlen(tmp) > 1)
		{	
			line += "Content-Type: ";
			line +=	tmp;
			line += "\n";
		}

		line += "\n";
		line += datastr;
		line += "\n";
	}

	fwrite(line.c_str(), sizeof(char), line.size(), mail);

	fclose(mail);
}

void TucanEmailTransport_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

// Factory

TucanEmailTransportFactory_i::TucanEmailTransportFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanTransport_ptr TucanEmailTransportFactory_i::create()
{
	TucanEmailTransport_i* myTransport = new TucanEmailTransport_i(myOrb);
	PortableServer::ObjectId_var myId = myPoa->activate_object(myTransport);
	CORBA::Object_var obj = myPoa->servant_to_reference(myTransport);
	myTransport->_remove_ref();

	return TucanEmailTransport::_narrow(obj);
}
