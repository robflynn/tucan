//////////////////////////////////////////////////////////
// TucanFaxTransport_i (c++ impl)  
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
// Filename:            TucanFaxTransport_i.cc
// Description:         TucanFaxMessage for CORBA implementation 
// Author:              Rob Flynn <rob@difinium.com>
// Created at:          Web Feb 21 17:26:22 PST 2001
//////////////////////////////////////////////////////////

#include <TucanFaxTransport_i.hh>
#include <TucanUtilities.hh>
#include <iostream.h>
#include <string>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

TucanFaxTransport_i::TucanFaxTransport_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

void TucanFaxTransport_i::send(TucanMessage_ptr msg)
{
	TucanAddressSeq_var addresses;
	TucanRawData *data;
	TucanData_var dob;
	string filename;
	int fd;
	char buf[64];
	CORBA::String_var tmp;
	string extension;
	char *rtypes[4] = {"to", "cc", "bcc", "fax"};

	// Get our message's raw data
	dob = msg->getDataObject();
	data = dob->getData();

	if (data->length() == 0)
	{
		string text = dob->getText();
		
		if (text.length() > 0)
		{
			char *rectypes[2] = {"To", "Cc"};

			string etmp("");
			TucanAddress_var addr = msg->getFrom();
			tmp = addr->getAddress();

			etmp += "From: ";
			etmp += tmp;
			etmp += "\n";

			tmp = msg->getSubject();
			if (strlen(tmp) > 0)
			{
				etmp += "Subject: ";
				etmp += tmp;
				etmp += "\n";
			}

			tmp = msg->getSentDate("");
			etmp += "Date: ";
			etmp += tmp;
			etmp += "\n";

			// Get the recipients
			for (unsigned int i = 0; i < sizeof(rectypes)/(sizeof(char *)); i++)
			{
				addresses = msg->getRecipients(rectypes[i]);
				unsigned int len = addresses->length();

				// If there are any for this type, append the header
				if (len)
				{
					etmp += rectypes[i];
					etmp += ": ";
				}
			
				// And fetch each one
				for (unsigned int j = 0; j < len; j++)
				{
					tmp = addresses[j]->getAddress();
					
					etmp += tmp;

					// Leaving off the comma on the last one
					if (j < len-1)
						etmp += ", ";	
				}

				if (len)
					etmp += "\n";
			}

			etmp += "\n";
			etmp += text;

			data->length(etmp.length());
			for (unsigned long i = 0; i < etmp.length(); i++)
				(*data)[i] = etmp[i];	
		}
	}

	CORBA::String_var ctype = dob->getContentType();

	if (!strcasecmp(ctype, "application/postscript"))
		extension = "ps";
	else if (!strcasecmp(ctype, "image/tiff"))
		extension = "tiff";
	else if (!strcasecmp(ctype, "text/plain"))
		extension = "txt";
	else
		extension = "fax";

	snprintf(buf, 64, "fax%ld.%s", time((time_t)NULL), extension.c_str());

	for (unsigned int i = 0; i < sizeof(rtypes)/sizeof(char *); i++)
	{
		unsigned int len;
	
		addresses = msg->getRecipients(rtypes[i]);

		len = addresses->length();
		if (strcasecmp(rtypes[i], "fax"))
		{
			for (unsigned long j = 0; j < len; j++)
			{
				tmp = addresses[j]->getAddress();

				if (!gethomedir(tmp))
					continue;

				filename = gethomedir(tmp);
				filename += "/fax/inbox/";
				filename += buf;

				// Open the file for writing, creating it if necessary
				fd = open(filename.c_str(), O_WRONLY | O_CREAT, 
						S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

				char *tmpbuf;

				tmpbuf = (char *)malloc(data->length() * sizeof(char));

				for (unsigned long k = 0; k < data->length(); k++)
					tmpbuf[k] = (*data)[k];
				
				write(fd, tmpbuf, data->length());

				free(tmpbuf);

				// And close up shop :)
				close(fd);
			}
		}
		else
		{
			/* Fax it */
			filename = "/tmp/tmp_";
			filename += buf;

			// Open the file for writing, creating it if necessary
			fd = open(filename.c_str(), O_WRONLY | O_CREAT, 
					S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

			char *tmpbuf;

			tmpbuf = (char *)malloc(data->length() * sizeof(char));

			for (unsigned long k = 0; k < data->length(); k++)
				tmpbuf[k] = (*data)[k];
			
			write(fd, tmpbuf, data->length());

			free(tmpbuf);

			// And close up shop :)
			close(fd);

			for (unsigned long j = 0; j < len; j++)
			{
				tmp = addresses[j]->getAddress();

				// Build the command
				string cmd = "sendfax -d";
				cmd += tmp;
				cmd += " ";
				cmd += filename;

				// Execute it	
				system(cmd.c_str());
			}
		}
	}
}

void TucanFaxTransport_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

// Factory

TucanFaxTransportFactory_i::TucanFaxTransportFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanTransport_ptr TucanFaxTransportFactory_i::create()
{
	TucanFaxTransport_i* myTransport = new TucanFaxTransport_i(myOrb);
	PortableServer::ObjectId_var myId = myPoa->activate_object(myTransport);
	CORBA::Object_var obj = myPoa->servant_to_reference(myTransport);
	myTransport->_remove_ref();

	return TucanFaxTransport::_narrow(obj);
}
