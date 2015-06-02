////////////////////////////////////////////////////////////
// TucanMain
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
// Filename:        TucanMain.cc 
// Description:     Main server that backs all of the servants 
// Author:          Jared Peterson <jared@difinium.com>
// Created at:      Tue Dec 12 13:44:29 PST 2000 
////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <time.h>
#include <string>
#include <Tucan.hh>
#include <TucanConfig.hh>

#include <TucanSession_i.hh>
#include <TucanAuth_i.hh>
#include <TucanData_i.hh>
#include <TucanUserPrefs_i.hh>

#include <TucanEmailStore_i.hh>
#include <TucanEmailFolder_i.hh>
#include <TucanEmailMessage_i.hh>
#include <TucanEmailAddress_i.hh>
#include <TucanEmailTransport_i.hh>
#include <TucanSMTPTransport_i.hh>
#include <TucanEmailFetcher_i.hh>

#include <TucanVoiceStore_i.hh>
#include <TucanVoiceFolder_i.hh>
#include <TucanVoiceMessage_i.hh>
#include <TucanVoiceAddress_i.hh>
#include <TucanVoiceTransport_i.hh>

#include <TucanFaxStore_i.hh>
#include <TucanFaxFolder_i.hh>
#include <TucanFaxMessage_i.hh>
#include <TucanFaxAddress_i.hh>
#include <TucanFaxTransport_i.hh>

#include <TucanCalendarStore_i.hh>
#include <TucanCalendarFolder_i.hh>
#include <TucanCalendarMessage_i.hh>
#include <TucanCalendarAddress_i.hh>
#include <TucanCalendarTransport_i.hh>

#include <TucanAddressBookStore_i.hh>
#include <TucanAddressBookFolder_i.hh>
#include <TucanAddressBookMessage_i.hh>
#include <TucanAddressBookAddress_i.hh>
#include <TucanAddressBookTransport_i.hh>

#include <TucanBbsStore_i.hh>
#include <TucanBbsFolder_i.hh>
#include <TucanBbsMessage_i.hh>
#include <TucanBbsAddress_i.hh>
#include <TucanBbsTransport_i.hh>

#include <util/PlatformUtils.hpp>

#if HAVE_SIGNAL_H
#include <signal.h>

void sighandler(int sig)
{
	cout << "TUCAN has experienced a segmentation fault.  Please visit" << endl;
	cout << "http://tucan.sourceforge.net/ and place a bug report.  Please" << endl;
	cout << "state what you were doing at the time of the crash. " << endl;
	cout << endl;
	cout << " - The TUCAN Devel Team" << endl;
	cout << endl;

	exit(0);
}
#endif



int main(int argc, char** argv)
{
	CORBA::Object_var ref;
	CORBA::String_var str;
	TucanConfig* config = new TucanConfig();
	string filename;
	ofstream ior;

	srandom(time((time_t *)NULL));

#if HAVE_SIGNAL_H
	signal(SIGSEGV, sighandler);
#endif
	

	XMLPlatformUtils::Initialize();

	string size = config->get("maxmessage", "");
	int tmp_size = atoi(size.c_str());
	size_t mySize = tmp_size;
	omniORB::MaxMessageSize(mySize);
	
	CORBA::ORB_var orb = 
		CORBA::ORB_init(argc, argv, "omniORB3");
    CORBA::Object_var obj = 
		orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = 
		PortableServer::POA::_narrow(obj);
    
	TucanSessionFactory_i* mySessionFactory = 
		new TucanSessionFactory_i(orb);
    PortableServer::ObjectId_var mySessionFactoryid = 
		poa->activate_object(mySessionFactory);
    ref = mySessionFactory->_this();
    str = orb->object_to_string(ref);

	filename = config->get("session", "");	
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();
    
	TucanAuthFactory_i* myAuthFactory = 
		new TucanAuthFactory_i(orb);
    PortableServer::ObjectId_var myAuthFactoryid = 
		poa->activate_object(myAuthFactory);
    ref = myAuthFactory->_this();
    str = orb->object_to_string(ref);

	filename = config->get("auth", "");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();
    
	TucanDataFactory_i* myDataFactory = 
		new TucanDataFactory_i(orb);
    PortableServer::ObjectId_var myDataFactoryid = 
		poa->activate_object(myDataFactory);
    ref = myDataFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("data", "");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanEmailStoreFactory_i* myEmailStoreFactory = 
		new TucanEmailStoreFactory_i(orb);
    PortableServer::ObjectId_var myEmailStoreFactoryid = 
		poa->activate_object(myEmailStoreFactory);
    ref = myEmailStoreFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("email", "store");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();
	
	TucanEmailFolderFactory_i* myEmailFolderFactory = 
		new TucanEmailFolderFactory_i(orb);
    PortableServer::ObjectId_var myEmailFolderFactoryid = 
		poa->activate_object(myEmailFolderFactory);
    ref = myEmailFolderFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("email", "folder");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanEmailMessageFactory_i* myEmailMessageFactory = 
		new TucanEmailMessageFactory_i(orb);
    PortableServer::ObjectId_var myEmailMessageFactoryid = 
		poa->activate_object(myEmailMessageFactory);
    ref = myEmailMessageFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("email", "message");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanEmailAddressFactory_i* myEmailAddressFactory = 
		new TucanEmailAddressFactory_i(orb);
    PortableServer::ObjectId_var myEmailAddressFactoryid = 
		poa->activate_object(myEmailAddressFactory);
    ref = myEmailAddressFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("email", "address");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanEmailTransportFactory_i* myEmailTransportFactory = 
		new TucanEmailTransportFactory_i(orb);
    PortableServer::ObjectId_var myEmailTransportFactoryid = 
		poa->activate_object(myEmailTransportFactory);
    ref = myEmailTransportFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("email", "transport");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanEmailFetcherFactory_i* myMailFactory = 
		new TucanEmailFetcherFactory_i(orb);
    PortableServer::ObjectId_var myMailFactoryid = 
		poa->activate_object(myMailFactory);
    ref = myMailFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("email", "fetcher");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanSMTPTransportFactory_i* mySMTPTransport = 
		new TucanSMTPTransportFactory_i(orb);
    PortableServer::ObjectId_var mySMTPTransportid = 
		poa->activate_object(mySMTPTransport);
    ref = mySMTPTransport->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("smtp", "transport");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanCalendarStoreFactory_i* myCalendarStoreFactory = 
		new TucanCalendarStoreFactory_i(orb);
    PortableServer::ObjectId_var myCalendarStoreFactoryid = 
		poa->activate_object(myCalendarStoreFactory);
    ref = myCalendarStoreFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("calendar", "store");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanCalendarFolderFactory_i* myCalendarFolderFactory = 
		new TucanCalendarFolderFactory_i(orb);
    PortableServer::ObjectId_var myCalendarFolderFactoryid = 
		poa->activate_object(myCalendarFolderFactory);
    ref = myCalendarFolderFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("calendar", "folder");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanCalendarMessageFactory_i* myCalendarMessageFactory = 
		new TucanCalendarMessageFactory_i(orb);
    PortableServer::ObjectId_var myCalendarMessageFactoryid = 
		poa->activate_object(myCalendarMessageFactory);
    ref = myCalendarMessageFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("calendar", "message");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanCalendarAddressFactory_i* myCalendarAddressFactory = 
		new TucanCalendarAddressFactory_i(orb);
    PortableServer::ObjectId_var myCalendarAddressFactoryid = 
		poa->activate_object(myCalendarAddressFactory);
    ref = myCalendarAddressFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("calendar", "address");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanCalendarTransportFactory_i* myCalendarTransportFactory = 
		new TucanCalendarTransportFactory_i(orb);
    PortableServer::ObjectId_var myCalendarTransportFactoryid = 
		poa->activate_object(myCalendarTransportFactory);
    ref = myCalendarTransportFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("calendar", "transport");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanVoiceStoreFactory_i* myVoiceStoreFactory = 
		new TucanVoiceStoreFactory_i(orb);
    PortableServer::ObjectId_var myVoiceStoreFactoryid = 
		poa->activate_object(myVoiceStoreFactory);
    ref = myVoiceStoreFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("voice", "store");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanVoiceFolderFactory_i* myVoiceFolderFactory = 
		new TucanVoiceFolderFactory_i(orb);
    PortableServer::ObjectId_var myVoiceFolderFactoryid = 
		poa->activate_object(myVoiceFolderFactory);
    ref = myVoiceFolderFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("voice", "folder");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanVoiceMessageFactory_i* myVoiceMessageFactory = 
		new TucanVoiceMessageFactory_i(orb);
    PortableServer::ObjectId_var myVoiceMessageFactoryid = 
		poa->activate_object(myVoiceMessageFactory);
    ref = myVoiceMessageFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("voice", "message");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanVoiceAddressFactory_i* myVoiceAddressFactory = 
		new TucanVoiceAddressFactory_i(orb);
    PortableServer::ObjectId_var myVoiceAddressFactoryid = 
		poa->activate_object(myVoiceAddressFactory);
    ref = myVoiceAddressFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("voice", "address");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanVoiceTransportFactory_i* myVoiceTransportFactory = 
		new TucanVoiceTransportFactory_i(orb);
    PortableServer::ObjectId_var myVoiceTransportFactoryid = 
		poa->activate_object(myVoiceTransportFactory);
    ref = myVoiceTransportFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("voice", "transport");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanFaxStoreFactory_i* myFaxStoreFactory = 
		new TucanFaxStoreFactory_i(orb);
    PortableServer::ObjectId_var myFaxStoreFactoryid = 
		poa->activate_object(myFaxStoreFactory);
    ref = myFaxStoreFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("fax", "store");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanFaxFolderFactory_i* myFaxFolderFactory = 
		new TucanFaxFolderFactory_i(orb);
    PortableServer::ObjectId_var myFaxFolderFactoryid = 
		poa->activate_object(myFaxFolderFactory);
    ref = myFaxFolderFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("fax", "folder");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanFaxMessageFactory_i* myFaxMessageFactory = 
		new TucanFaxMessageFactory_i(orb);
    PortableServer::ObjectId_var myFaxMessageFactoryid = 
		poa->activate_object(myFaxMessageFactory);
    ref = myFaxMessageFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("fax", "message");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanFaxAddressFactory_i* myFaxAddressFactory = 
		new TucanFaxAddressFactory_i(orb);
    PortableServer::ObjectId_var myFaxAddressFactoryid = 
		poa->activate_object(myFaxAddressFactory);
    ref = myFaxAddressFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("fax", "address");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanFaxTransportFactory_i* myFaxTransportFactory = 
		new TucanFaxTransportFactory_i(orb);
    PortableServer::ObjectId_var myFaxTransportFactoryid = 
		poa->activate_object(myFaxTransportFactory);
    ref = myFaxTransportFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("fax", "transport");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanAddressBookStoreFactory_i* myAddressBookStoreFactory = 
		new TucanAddressBookStoreFactory_i(orb);
    PortableServer::ObjectId_var myAddressBookStoreFactoryid = 
		poa->activate_object(myAddressBookStoreFactory);
    ref = myAddressBookStoreFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("addressbook", "store");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanAddressBookFolderFactory_i* myAddressBookFolderFactory = 
		new TucanAddressBookFolderFactory_i(orb);
    PortableServer::ObjectId_var myAddressBookFolderFactoryid = 
		poa->activate_object(myAddressBookFolderFactory);
    ref = myAddressBookFolderFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("addressbook", "folder");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanAddressBookMessageFactory_i* myAddressBookMessageFactory = 
		new TucanAddressBookMessageFactory_i(orb);
    PortableServer::ObjectId_var myAddressBookMessageFactoryid = 
		poa->activate_object(myAddressBookMessageFactory);
    ref = myAddressBookMessageFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("addressbook", "message");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanAddressBookAddressFactory_i* myAddressBookAddressFactory = 
		new TucanAddressBookAddressFactory_i(orb);
    PortableServer::ObjectId_var myAddressBookAddressFactoryid = 
		poa->activate_object(myAddressBookAddressFactory);
    ref = myAddressBookAddressFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("addressbook", "address");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanAddressBookTransportFactory_i* myAddressBookTransportFactory = 
		new TucanAddressBookTransportFactory_i(orb);
    PortableServer::ObjectId_var myAddressBookTransportFactoryid = 
		poa->activate_object(myAddressBookTransportFactory);
    ref = myAddressBookTransportFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("addressbook", "transport");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanUserPrefsFactory_i* myUserPrefsFactory = 
		new TucanUserPrefsFactory_i(orb);
    PortableServer::ObjectId_var myUserPrefsFactoryid = 
		poa->activate_object(myUserPrefsFactory);
    ref = myUserPrefsFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("prefs", "");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanBbsStoreFactory_i* myBbsStoreFactory = 
		new TucanBbsStoreFactory_i(orb);
    PortableServer::ObjectId_var myBbsStoreFactoryid = 
		poa->activate_object(myBbsStoreFactory);
    ref = myBbsStoreFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("bbs", "store");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanBbsFolderFactory_i* myBbsFolderFactory = 
		new TucanBbsFolderFactory_i(orb);
    PortableServer::ObjectId_var myBbsFolderFactoryid = 
		poa->activate_object(myBbsFolderFactory);
    ref = myBbsFolderFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("bbs", "folder");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanBbsMessageFactory_i* myBbsMessageFactory = 
		new TucanBbsMessageFactory_i(orb);
    PortableServer::ObjectId_var myBbsMessageFactoryid = 
		poa->activate_object(myBbsMessageFactory);
    ref = myBbsMessageFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("bbs", "message");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanBbsAddressFactory_i* myBbsAddressFactory = 
		new TucanBbsAddressFactory_i(orb);
    PortableServer::ObjectId_var myBbsAddressFactoryid = 
		poa->activate_object(myBbsAddressFactory);
    ref = myBbsAddressFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("bbs", "address");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();

	TucanBbsTransportFactory_i* myBbsTransportFactory = 
		new TucanBbsTransportFactory_i(orb);
    PortableServer::ObjectId_var myBbsTransportFactoryid = 
		poa->activate_object(myBbsTransportFactory);
    ref = myBbsTransportFactory->_this();
    str = orb->object_to_string(ref);
	
	filename = config->get("bbs", "transport");
	ior.open(filename.c_str(), ios::out);
   	ior << str;
	ior.close();


	XMLPlatformUtils::Initialize();

	delete config;
	PortableServer::POAManager_var mgr = poa->the_POAManager();
    mgr->activate();
    orb->run();
    orb->destroy();
	
	return 0;
}
