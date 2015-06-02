////////////////////////////////////////////////////////////
// TucanCalendarMessage (c++ header)
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
// Filename:        TucanCalendarMessage_i.hh 
// Description:     The Calendar Message interface 
// Author:          Jared Peterson <jared@difinium.com>
// Created at:      Thu Feb  1 14:01:19 PST 2001 
////////////////////////////////////////////////////////////

#include <iostream.h>
#include <fstream.h>
#include <string>
#include <Tucan.hh>
#include <TucanConfig.hh>
#include <map>

class TucanCalendarMessage_i: public POA_TucanCalendarMessage,
                public PortableServer::RefCountServantBase 
{
	public:
  		TucanCalendarMessage_i(CORBA::ORB_var orb);
  		virtual ~TucanCalendarMessage_i() {};

		char* getTime();
		void setTime(const char* myTime);
		char* getNotifyDate();
		void setNotifyDate(const char* myDate);
		char* getNotifyTime();
		void setNotifyTime(const char* time);
		void setNotified(CORBA::Boolean notified);
		CORBA::Boolean isNotified();
		char* getNotification();
		void setNotification(const char* myNotification);
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
		CORBA::Long myFlags;
		TucanData_var myData;
		string myTime;
		string myNotification;
		string myNotifyDate;
		string myNotifyTime;
		CORBA::Boolean myNotified;
		TucanAddress_var myFrom;
		TucanAddress_var myReplyTo;
		string mySubject;

		map<string, TucanAddressSeq *> myRecipients;
};

class TucanCalendarMessageFactory_i: public POA_TucanCalendarMessageFactory,
                public PortableServer::RefCountServantBase 
{
	public:
		TucanCalendarMessageFactory_i(CORBA::ORB_var orb);
		virtual ~TucanCalendarMessageFactory_i() {};

		TucanMessage_ptr create();
	
	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
};
