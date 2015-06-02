//////////////////////////////////////////////////////////
// TucanEmailTransport_i (c++ header)
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
// Filename:	TucanEmailTransport_i.hh
// Description:	TucanEmailMessage for CORBA implementation 
// Author:	Rob Flynn <rob@difinium.com>
// Created at:	Fri Jan 25 14:13:25 PST 2001
//////////////////////////////////////////////////////////

#include <Tucan.hh>

class TucanEmailTransport_i: public POA_TucanEmailTransport,
	public PortableServer::RefCountServantBase 
{

	public:
		TucanEmailTransport_i(CORBA::ORB_var orb);
		virtual ~TucanEmailTransport_i() {};

		void send(TucanMessage_ptr msg);
		void destroy();
	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};


class TucanEmailTransportFactory_i: public POA_TucanEmailTransportFactory,
	public PortableServer::RefCountServantBase 
{

	public:
		TucanEmailTransportFactory_i(CORBA::ORB_var orb);
		virtual ~TucanEmailTransportFactory_i() {};

		TucanTransport_ptr create();
	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};
