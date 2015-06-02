//////////////////////////////////////////////////////////
// TucanSMTPTransport_i (c++ impl)  
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
// Filename:            TucanSMTPTransport_i.cc
// Description:	        TucanEmailMessage for CORBA implementation 
// Author:              Rob Flynn <rob@difinium.com>
// Created at:          Fri Jun  8 16:18:00 PDT 2001
//////////////////////////////////////////////////////////

#include <TucanSMTPTransport_i.hh>
#include <TucanUtilities.hh>
#include <iostream.h>
#include <string>
#include <stdio.h>
#include <time.h>

// Network stuff
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <TucanIMAP.hh>
#include <sys/time.h>


TucanSMTPTransport_i::TucanSMTPTransport_i(CORBA::ORB_var orb)
{
	myPort = 25;
	myServer = "";
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

void TucanSMTPTransport_i::setPort(CORBA::Short port)
{
	myPort = port;
}

void TucanSMTPTransport_i::setServer(const char *server)
{
	myServer = CORBA::string_dup(server);
}

/* The send method is almost a direct copy from the TucanEmailTransport object.
 * The only thing that will need to be modified is the way the final message 
 * is sent. Instead of spawning /sbin/sendmail, we'll just connect to an SMTP
 * port, fire off the email, and then sign off. */

void TucanSMTPTransport_i::send(TucanMessage_ptr msg)
{
	unsigned int i;
	unsigned int pc;
	int n;

	char *rectypes[3] = {"To","Cc","Bcc"};

	if ((myPort == 0) || (!myServer.compare("")))
	{
		throw TucanException("Error: Server and/or port are not set for TucanSMTPTransport. ");
	}

	string line("");

	TucanAddress_var fromAddr = msg->getFrom();
	CORBA::String_var from = fromAddr->getAddress();
	CORBA::String_var tmp;
	CORBA::String_var address;

	int fd;
	struct hostent *hent;
	struct sockaddr_in serv;
	char myBuf[8192];
	
	// Set up the structs
	fd = socket(AF_INET, SOCK_STREAM, 0);
	
	hent = gethostbyname(myServer.c_str());

	if (!hent)
	{
		// If we're here, then there was an error in resolving the host name
		string tmperr("Error: There was an error resolving the hostname '");
		tmperr += myServer;
		tmperr += "'";

		throw TucanException(tmperr.c_str());
	}

	serv.sin_family = AF_INET;
	serv.sin_port = htons(myPort);
	serv.sin_addr = *((struct in_addr *)hent->h_addr);
	bzero(&(serv.sin_zero), 8);

	// Connect
	if (connect(fd, (struct sockaddr*)&serv, sizeof(struct sockaddr)) < 0)
	{
		// If we're here, then there was an error in resolving the host name
		string tmperr("Error: A connection to '");
		tmperr += myServer;
		tmperr += "' could not be established.";

		throw TucanException(tmperr.c_str());
	}

	n = recv(fd, myBuf, 8192, 0);
	
	string smtpinit;
	smtpinit = "MAIL FROM: ";
    smtpinit += from;
	smtpinit +="\n";

	line += "From: ";
	line += from;
	line += "\n";

	n = ::send(fd, smtpinit.c_str(), smtpinit.length(), 0);
	n = recv(fd, myBuf, 8192, 0);
	
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

		smtpinit = "";

		
		if (addresses->length() > 0)
		{
			line += rectypes[j];
			line += ": ";
		
			for (i = 0; i < addresses->length(); i++)
			{
				address = addresses[i]->getAddress();

				// Prepare it for sending
				smtpinit = "RCPT ";
				smtpinit += rectypes[j];
				smtpinit += ": ";
				smtpinit += address;
				smtpinit += "\n";

				// Make sure we're legit here
				if (i > 0)
					line += ",";

				line += address;

				n = ::send(fd, smtpinit.c_str(), smtpinit.length(), 0);
				n = recv(fd, myBuf, 8192, 0);
			}
			line += "\n";
		}
	}

	// Geme t the subject
	tmp = msg->getSubject();

	line += "Subject: ";
	line += tmp;
	line +="\n";	

	CORBA::String_var tdate;
	tdate = msg->getSentDate("");

	line += "Date: ";
	if (strlen(tdate) == 0)
	{
		time_t tmptime = time((time_t *)NULL);
		line += asctime(localtime(&tmptime));
	}
	else
	{
		line += tdate;	
		line += "\n";
	}


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
			else if (strncasecmp(tmp, "text/", 5))
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



	// Now that we have the whole body of the message constructed, 
	// let's connect and send.

	smtpinit = "DATA\n";

	n = ::send(fd, smtpinit.c_str(), smtpinit.length(), 0);
	n = recv(fd, myBuf, 8192, 0);

	vector<string> emailtext = strsplit(line, '\n');
	for (unsigned int pp = 0; pp < emailtext.size(); pp++)
	{
		string yo = emailtext[pp];
		yo = strchomp(yo);

		yo += "\r\n";
		n = ::send(fd, yo.c_str(), yo.length(), 0);
	}

	smtpinit = "\r\n.\r\n";
	n = ::send(fd, smtpinit.c_str(), smtpinit.length(), 0);
	n = recv(fd, myBuf, 8192, 0);

	n = write(fd, "QUIT\n", 5);
	n = recv(fd, myBuf, 8192, 0);

	close(fd);
}

void TucanSMTPTransport_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

// Factory

TucanSMTPTransportFactory_i::TucanSMTPTransportFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanTransport_ptr TucanSMTPTransportFactory_i::create()
{
	TucanSMTPTransport_i* myTransport = new TucanSMTPTransport_i(myOrb);
	PortableServer::ObjectId_var myId = myPoa->activate_object(myTransport);
	CORBA::Object_var obj = myPoa->servant_to_reference(myTransport);
	myTransport->_remove_ref();

	return TucanSMTPTransport::_narrow(obj);
}
