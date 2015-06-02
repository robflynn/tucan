////////////////////////////////////////////////////////////
// TucanSession_i (c++ header)  
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
// Filename:        TucanSession_i.hh 
// Description:     TucanSession for CORBA implementation 
// Author:          Jared Peterson <jared@difinium.com>
// Created at:      Tue Dec 12 13:44:29 PST 2000 
////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <strstream.h>
#include <string>
#include <Tucan.hh>
#include <TucanConfig.hh>
#include <TucanDb.hh>

class TucanSession_i: public POA_TucanSession,
                public PortableServer::RefCountServantBase
{
	public:
  		TucanSession_i(CORBA::ORB_var orb);
  		virtual ~TucanSession_i() {};
		
  		TucanAuth_ptr getAuth();
  		void authorize(TucanAuth_ptr auth);
  		TucanStore_ptr getDefaultStore();
  		TucanStore_ptr getStoreByProtocol(const char* proto);
  		TucanTransport_ptr getDefaultTransport();
  		TucanTransport_ptr getTransportByProtocol(const char* proto);
		CORBA::Boolean isAdmin();
		void setNewPinNumber(CORBA::Long newpin);
  		void destroy();

	private:
		TucanAuth_var myAuth;
		CORBA::Boolean myAuthorized;
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
		CORBA::Boolean myAdmin;
};

class TucanSessionFactory_i: public POA_TucanSessionFactory,
                public PortableServer::RefCountServantBase
{
    public:
        TucanSessionFactory_i(CORBA::ORB_var orb);
        virtual ~TucanSessionFactory_i() {};

        TucanSession_ptr create();

    private:
        CORBA::ORB_var myOrb;
        PortableServer::POA_var myPoa;
};
