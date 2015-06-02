////////////////////////////////////////////////////////////
// TucanAddressBookMessage (c++ header)
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
// Filename:        TucanAddressBookMessage_i.hh 
// Description:     The Address Book Message interface 
// Author:          Mathew Davis <mattd@difinium.com>
// Created at:      Tue Feb 13 16:11:28 PST 2001 
////////////////////////////////////////////////////////////

#include <iostream.h>
#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <fstream.h>
#include <string>
#include <vector>
#include <Tucan.hh>
#include <TucanConfig.hh>
#include <TucanUtilities.hh>
#include <map>


class TucanAddressBookMessage_i: public POA_TucanAddressBookMessage,
                public PortableServer::RefCountServantBase 
{
	public:
  		TucanAddressBookMessage_i(CORBA::ORB_var orb);
  		virtual ~TucanAddressBookMessage_i() {};

		char* getAttribute(const char* key);
		void setAttribute(const char* key, const char* value);
		TucanStringList* listAttributes();
		TucanAttributeSeq* getAttributes();
		char* getGroup();
		void setGroup(const char* group);
		char* getTime();
		void setTime(const char* myTime);
		TucanRawData* getData();
		TucanData_ptr getDataObject();
		void setDataObject(TucanData_ptr data);
		TucanData_ptr getNewDataObject();
		TucanAddress_ptr getFrom();
		void setFrom(TucanAddress_ptr fromname);
		TucanAddress_ptr getNewAddressObject();
		TucanAddressSeq* getRecipients(const char* type);
		void setRecipients(const char* type, 
				const TucanAddressSeq& addresses);
		void setRecipient(const char* type, 
				TucanAddress_ptr address);
		TucanAddressSeq* getAllRecipients();
		void addRecipient(const char* type, 
				TucanAddress_ptr address);
		void addRecipients(const char* type, 
				const TucanAddressSeq& addresses);
		TucanAddress_ptr getReplyTo();
		void setReplyTo(TucanAddress_ptr address);
		char* getSubject();
		void setSubject(const char* subject);
		char* getSentDate(const char* format);
		void setSentDate(const char* date);
		char* getReceivedDate(const char* format);
		void setReceivedDate(const char* date);
		CORBA::Long getMessageNumber();
		void setMessageNumber(CORBA::Long msgnum);
		CORBA::Boolean isFlagSet(CORBA::Long flag);
		void setFlags(CORBA::Long flags);
		void setFlagOn(CORBA::Long flag);
		void setFlagOff(CORBA::Long flag);
		TucanMessage_ptr reply(CORBA::Boolean replytoall);
		void destroy();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
		CORBA::Long myMsgNum;
		string mySentDate;
		string myReceivedDate;
		TucanData_var myData;
		string myTime;
		myHash myAttributes;
		TucanAddress_var myFrom;
		TucanAddress_var myReplyTo;
		CORBA::Long myFlags;

		string myGroup;
		string mySubject;

		map <string, TucanAddressSeq*> myRecipients;
};

class TucanAddressBookMessageFactory_i: public POA_TucanAddressBookMessageFactory,
                public PortableServer::RefCountServantBase 
{
	public:
		TucanAddressBookMessageFactory_i(CORBA::ORB_var orb);
		virtual ~TucanAddressBookMessageFactory_i() {};
		TucanMessage_ptr create();
	
	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};
