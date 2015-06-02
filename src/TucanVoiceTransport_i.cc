//////////////////////////////////////////////////////////
// TucanVoiceTransport_i (c++ impl)  
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
// Filename:            TucanVoiceTransport_i.cc
// Description:         TucanVoiceMessage for CORBA implementation 
// Author:              Rob Flynn <rob@difinium.com>
// Created at:          Web Feb 21 17:26:22 PST 2001
//////////////////////////////////////////////////////////

#include <TucanVoiceTransport_i.hh>
#include <TucanUtilities.hh>
#include <iostream.h>
#include <string>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

TucanVoiceTransport_i::TucanVoiceTransport_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

void TucanVoiceTransport_i::send(TucanMessage_ptr msg)
{
	TucanAddressSeq_var addresses;
	TucanRawData *data;
	string filename;
	int fd;
	char buf[64];
	CORBA::String_var tmp;
	
	snprintf(buf, 64, "msg%ld.wav", time((time_t)NULL));

	addresses = msg->getAllRecipients();

	// Get our message's raw data
	data = msg->getData();
	
	for (unsigned long i = 0; i < addresses->length(); i++)
	{
		tmp = addresses[i]->getAddress();

		if (!gethomedir(tmp))
			continue;

		filename = gethomedir(tmp);
		filename += "/vm/inbox/";
		filename += buf;

		// Open the file for writing, creating it if necessary
		fd = open(filename.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

		char *tmp;;

		tmp = (char *)malloc(data->length() * sizeof(char));

		for (unsigned long i = 0; i < data->length(); i++)
			tmp[i] = (*data)[i];
		
		write(fd, tmp, data->length());

		// And close up shop :)
		close(fd);
	}
}

void TucanVoiceTransport_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

// Factory

TucanVoiceTransportFactory_i::TucanVoiceTransportFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanTransport_ptr TucanVoiceTransportFactory_i::create()
{
	TucanVoiceTransport_i* myTransport = new TucanVoiceTransport_i(myOrb);
	PortableServer::ObjectId_var myId = myPoa->activate_object(myTransport);
	CORBA::Object_var obj = myPoa->servant_to_reference(myTransport);
	myTransport->_remove_ref();

	return TucanVoiceTransport::_narrow(obj);
}
