//////////////////////////////////////////////////////////
// TucanEmailFetcher_i (header file)
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
// Filename:            TucanEmailFetcher_i.hh
// Description:         TucanEmailFetcher for CORBA implementation 
// Author:              Rob Flynn <rob@difinium.com>
// Created at:          Mon Feb 05 11:27:30 PST 2001
//////////////////////////////////////////////////////////

#include <Tucan.hh>
#include <string>
#include <TucanEmail.hh>

class TucanEmailFetcher_i: public POA_TucanEmailFetcher,
		public PortableServer::RefCountServantBase 
{

	public:
		TucanEmailFetcher_i(CORBA::ORB_var orb);
		virtual ~TucanEmailFetcher_i() {};

		TucanAuth_ptr getAuth();
		void setAuth(TucanAuth_ptr myAuth);
		void setServer(const char* server);
		char* getServer();
		void setPort(CORBA::Short port);
		CORBA::Short getPort();
		void setProtocol(const char* proto);
		char* getProtocol();
		void setName(const char* name);
		char* getName();
		void setPassword(const char* password);
		char* getPassword();
		CORBA::Short getMessageCount();
		CORBA::Short getMessageSize(CORBA::Short num);
		char* getMessageId(CORBA::Short num);
		CORBA::Short isRead(CORBA::Short num);
		CORBA::Short transfer(CORBA::Short num);
		void transferAll();
		void deleteMessage(CORBA::Short num);
		void deleteAllMessages();
		void open();
		void close();
		void destroy();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
		TucanAuth_var myAuthObject;
		string myName;
		string myPassword;
		string myServer;
		unsigned int myPort;
		TucanEmail *myMailer;
		string myProto;
		
};

class TucanEmailFetcherFactory_i: public POA_TucanEmailFetcherFactory,
		public PortableServer::RefCountServantBase 
{

	public:
		TucanEmailFetcherFactory_i(CORBA::ORB_var orb);
		virtual ~TucanEmailFetcherFactory_i() {};  

		TucanEmailFetcher_ptr create();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
		
};

