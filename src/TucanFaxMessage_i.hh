//////////////////////////////////////////////////////////
// TucanFaxMessage_i (c++ header)  
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
// Filename:		TucanFaxMessage_i.hh
// Description:		TucanFaxMessage for CORBA implementation 
// Author:			Rob Flynn <rob@difinium.com>
// Created at:		Wed Feb 21 14:52:53 PST 2001
//////////////////////////////////////////////////////////

#include <iostream.h>
#include <fstream.h>
#include <Tucan.hh>
#include <string>
#include <map>

class TucanFaxMessage_i: public POA_TucanFaxMessage,
	public PortableServer::RefCountServantBase 
{
	public:
		TucanFaxMessage_i(CORBA::ORB_var orb);
		virtual ~TucanFaxMessage_i(){};

		char* getAttachment(const char* id);
		TucanRawData* getData();
		TucanData_ptr getDataObject();
		TucanData_ptr getNewDataObject();
		void setDataObject(TucanData_ptr data);
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
		string myClassName;
		string mySubject;
		string mySentDate;
		string myRecvDate;
		CORBA::Long myMessageNumber;
		TucanFolder_var myFolder;
		TucanAddress_var myFrom;
		TucanAddress_var myReplyTo;
		TucanData_var myData;
		CORBA::Long myFlags;

		map<string, TucanAddressSeq *> myRecipients;
};

class TucanFaxMessageFactory_i: public POA_TucanFaxMessageFactory,
	public PortableServer::RefCountServantBase 
{
	public:
		TucanFaxMessageFactory_i(CORBA::ORB_var orb);
		virtual ~TucanFaxMessageFactory_i(){};

		TucanMessage_ptr create();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
						
		
};
