////////////////////////////////////////////////////////////
// TucanBbsMessage (c++ header)
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
// Filename:        TucanBbsMessage_i.hh 
// Description:     The Bbs Message interface 
// Author:          Rob Flynn <rob@difinium.com>
// Created at:      Thu Apr  5 11:23:50 PDT 2001
////////////////////////////////////////////////////////////

#include <iostream.h>
#include <fstream.h>
#include <string>
#include <Tucan.hh>
#include <TucanConfig.hh>
#include <map>

class TucanBbsMessage_i: public POA_TucanBbsMessage, 
	public PortableServer::RefCountServantBase 
{

	public:
		TucanBbsMessage_i(CORBA::ORB_var orb);
		virtual ~TucanBbsMessage_i() {};

		CORBA::Short getBid();
		void setBid(CORBA::Short bid);
		CORBA::Long getPid();
		void setPid(CORBA::Long pid);
		char* getAuthor();
		void setAuthor(const char *author);
		CORBA::Short getAccessType();
		void setAccessType(CORBA::Short accesstype);
		TucanRawData* getData();
		TucanData_ptr getDataObject();
		void setDataObject(TucanData_ptr data);
		TucanData_ptr getNewDataObject();
		TucanAddress_ptr getFrom();
		void setFrom(TucanAddress_ptr fromname);
		TucanAddress_ptr getNewAddressObject();
		TucanAddressSeq* getRecipients(const char* type);
		void setRecipients(const char* type, const TucanAddressSeq& addresses);
		void setRecipient(const char* type, TucanAddress_ptr address);
		TucanAddressSeq* getAllRecipients();
		void addRecipient(const char* type, TucanAddress_ptr address);
		void addRecipients(const char* type, const TucanAddressSeq& addresses);
		TucanAddress_ptr getReplyTo();
		void setReplyTo(TucanAddress_ptr address);
		char* getSubject();
		void setSubject(const char* subject);
		void updateSubject(const char* subject);
		void updateBody(const char* body);
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
		string myAuthor;
		string mySubject;
		string mySentDate;
		string myRecvDate;
		CORBA::Long myMessageNumber;
		TucanAddress_var myFrom;
		TucanAddress_var myReplyTo;
		TucanData_var myData;

		CORBA::Long myPid;
		CORBA::Short myBid;
		CORBA::Short myAccessType;

		CORBA::Long myFlags;

		map <string, TucanAddressSeq*> myRecipients;
};

class TucanBbsMessageFactory_i: public POA_TucanBbsMessageFactory, 
	public PortableServer::RefCountServantBase 
{

	public:
		TucanBbsMessageFactory_i(CORBA::ORB_var orb);
		virtual ~TucanBbsMessageFactory_i() {};

		TucanMessage_ptr create();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
	
};

