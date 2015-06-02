////////////////////////////////////////////////////////////
// TucanCalendarTransport (c++ header)
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
// Filename:        TucanCalendarTransport_i.hh 
// Description:     The Calendar transport interface 
// Author:          Jared Peterson <jared@difinium.com>
// Created at:      Wed Feb  7 16:01:25 PST 2001 
////////////////////////////////////////////////////////////

#include <iostream.h>
#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <Tucan.hh>
#include <TucanDb.hh>
#include <TucanConfig.hh>
#include <TucanUtilities.hh>

class TucanCalendarTransport_i: public POA_TucanCalendarTransport,
                public PortableServer::RefCountServantBase 
{
	public:
  		TucanCalendarTransport_i(CORBA::ORB_var orb);
  		virtual ~TucanCalendarTransport_i() {};

		void send(TucanMessage_ptr msg);
		void destroy();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};

class TucanCalendarTransportFactory_i: public POA_TucanCalendarTransportFactory,
                public PortableServer::RefCountServantBase 
{
	public:
		TucanCalendarTransportFactory_i(CORBA::ORB_var orb);
		virtual ~TucanCalendarTransportFactory_i() {};

		TucanTransport_ptr create();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};
