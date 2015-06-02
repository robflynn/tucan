//////////////////////////////////////////////////////////
// TucanSMTPTransport_i (c++ header)
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
// Filename:	TucanSMTPTransport_i.hh
// Description:	TucanEmailMessage for CORBA implementation 
// Author:	Rob Flynn <rob@difinium.com>
// Created at:	Fri Jun  8 16:18:00 PDT 2001
//////////////////////////////////////////////////////////

#include <Tucan.hh>
#include <string>

class TucanSMTPTransport_i: public POA_TucanSMTPTransport,
	public PortableServer::RefCountServantBase 
{

	public:
		TucanSMTPTransport_i(CORBA::ORB_var orb);
		virtual ~TucanSMTPTransport_i() {};

		void send(TucanMessage_ptr msg);
		void setPort(CORBA::Short port);
		void setServer(const char *server);
		void destroy();
	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
		string myServer;
		int myPort;
};


class TucanSMTPTransportFactory_i: public POA_TucanSMTPTransportFactory,
	public PortableServer::RefCountServantBase 
{

	public:
		TucanSMTPTransportFactory_i(CORBA::ORB_var orb);
		virtual ~TucanSMTPTransportFactory_i() {};

		TucanTransport_ptr create();
	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};
