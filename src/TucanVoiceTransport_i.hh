//////////////////////////////////////////////////////////
// TucanVoiceTransport_i (c++ header)
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
// Filename:	TucanVoiceTransport_i.hh
// Description:	TucanVoiceMessage for CORBA implementation 
// Author:	Rob Flynn <rob@difinium.com>
// Created at:	Wed Feb 21 17:25:50 PST 2001
//////////////////////////////////////////////////////////

#include <Tucan.hh>

class TucanVoiceTransport_i: public POA_TucanVoiceTransport,
	public PortableServer::RefCountServantBase 
{

	public:
		TucanVoiceTransport_i(CORBA::ORB_var orb);
		virtual ~TucanVoiceTransport_i() {};

		void send(TucanMessage_ptr msg);
		void destroy();
	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};


class TucanVoiceTransportFactory_i: public POA_TucanVoiceTransportFactory,
	public PortableServer::RefCountServantBase 
{

	public:
		TucanVoiceTransportFactory_i(CORBA::ORB_var orb);
		virtual ~TucanVoiceTransportFactory_i() {};

		TucanTransport_ptr create();
	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};
