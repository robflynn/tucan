////////////////////////////////////////////////////////////
// TucanBbsMessage (c++ impl)
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
// Filename:        TucanBbsMessage_i.cc 
// Description:     The Bbs Message interface 
// Author:          Rob Flynn <rob@difinium.com>
// Created at:      Thu Apr  5 11:23:50 PDT 2001
////////////////////////////////////////////////////////////

#include <TucanBbsMessage_i.hh>
#include <TucanUtilities.hh>
#include <TucanDb.hh>

TucanBbsMessage_i::TucanBbsMessage_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
	
	myFrom = NULL;
	myData = NULL;
	myReplyTo = NULL;

	mySubject = "";
	mySentDate = "";
	myRecvDate = "";

	myPid = 0;
	myBid = 0;
	myAccessType = 0;

	myRecipients.clear();

	myFlags = 0;
}

CORBA::Short TucanBbsMessage_i::getBid()
{
	return myBid;
}

void TucanBbsMessage_i::setBid(CORBA::Short bid)
{
	myBid = bid;
}

CORBA::Long TucanBbsMessage_i::getPid()
{
	return myPid;
}

void TucanBbsMessage_i::setPid(CORBA::Long pid)
{
	myPid = pid;
}

char* TucanBbsMessage_i::getAuthor()
{
	return CORBA::string_dup(myAuthor.c_str());
}

void TucanBbsMessage_i::setAuthor(const char* author)
{
	myAuthor = author;
}

CORBA::Short TucanBbsMessage_i::getAccessType()
{
	return myAccessType;
}

void TucanBbsMessage_i::setAccessType(CORBA::Short accesstype)
{
	myAccessType = accesstype;
}

TucanRawData* TucanBbsMessage_i::getData()
{
	TucanRawData *data = myData->getData();

	return data;
}

TucanData_ptr TucanBbsMessage_i::getDataObject()
{
	return TucanData::_duplicate(myData);
}

void TucanBbsMessage_i::setDataObject(TucanData_ptr data)
{
	myData = TucanData::_duplicate(data);
}

TucanData_ptr TucanBbsMessage_i::getNewDataObject()
{
	TucanConfig* config = new TucanConfig();
	string filename = config->get("data","");
	delete config;
	string str;
	TucanData_ptr data;

	ifstream infile(filename.c_str());
	getline(infile, str);
	infile.close();

	CORBA::Object_var object = myOrb->string_to_object(str.c_str());
	TucanDataFactory_var fact = TucanDataFactory::_narrow(object);
	data = fact->create();

	return data;
}

TucanAddress_ptr TucanBbsMessage_i::getFrom()
{
	return TucanAddress::_duplicate(myFrom);
}

void TucanBbsMessage_i::setFrom(TucanAddress_ptr fromname)
{
	myFrom = TucanAddress::_duplicate(fromname);
}

TucanAddress_ptr TucanBbsMessage_i::getNewAddressObject()
{
	TucanConfig* config = new TucanConfig();
	string filename = config->get("bbs", "address");
	delete config;
	string str;
	TucanAddress_ptr address;

	ifstream infile(filename.c_str());
	getline(infile, str);
	infile.close();

	CORBA::Object_var object = myOrb->string_to_object(str.c_str());
	TucanAddressFactory_var fact = TucanAddressFactory::_narrow(object);
	address = fact->create();

	return address;
}

TucanAddressSeq* TucanBbsMessage_i::getRecipients(const char* type)
{
	TucanAddressSeq *list;
	TucanAddressSeq *newlist = new TucanAddressSeq;
	unsigned long len;
	unsigned long i;
	string rtype = uppercase(type);
	
	list = myRecipients[rtype];

	if (list)
	{
		len = list->length();
		newlist->length(len);

		for (i = 0; i < len; i++)
		{
			(*newlist)[i] = (*list)[i];
		}
	}

        return newlist;
}

void TucanBbsMessage_i::setRecipients(const char* type, const TucanAddressSeq& addresses)
{
	TucanAddressSeq *list;
	string rtype = uppercase(type);

	if (myRecipients.find(rtype) != myRecipients.end())
	{

		list = myRecipients[rtype];

		if (list)
		{
		
			// Free up the memory
			for(CORBA::ULong i = 0; i < list->length(); i++)
				(*list)[i]->destroy();

			delete list;

			myRecipients.erase(rtype);
		}
	}

	list = new TucanAddressSeq;
	
	// And set the new ones
	list->length(addresses.length());	

	for(CORBA::ULong i = 0; i < addresses.length(); i++)
		(*list)[i] = TucanAddress::_duplicate(addresses[i]);

	myRecipients.insert(pair<string, TucanAddressSeq *>(rtype, list));
}

void TucanBbsMessage_i::setRecipient(const char* type, TucanAddress_ptr address)
{
	TucanAddressSeq *list;
	string rtype = uppercase(type);

	if (myRecipients.find(rtype) != myRecipients.end())
	{
		list = myRecipients[rtype];

		if (list)
		{
		
			// Free up the memory
			for(CORBA::ULong i = 0; i < list->length(); i++)
				(*list)[i]->destroy();

			delete list;
			myRecipients.erase(rtype);
		}
		
	}

	list = new TucanAddressSeq;

	list->length(1);

	(*list)[0] = TucanAddress::_duplicate(address);

	myRecipients.insert(pair<string, TucanAddressSeq *>(rtype, list));
}

TucanAddressSeq* TucanBbsMessage_i::getAllRecipients()
{
	CORBA::ULong k = 0;

	TucanAddressSeq *newlist = new TucanAddressSeq;

	map<string, TucanAddressSeq*>::iterator kit = myRecipients.begin();

	for (unsigned long i = 0; i < myRecipients.size(); i++)
	{
		string key = (*kit).first;

		if (myRecipients[key])
		{
			CORBA::ULong len = myRecipients[key]->length();

			newlist->length(newlist->length() + len);
			for (CORBA::ULong j = 0; j < len; j++)
			{
				(*newlist)[k] = (*myRecipients[key])[j];
				k++;
			}
		}

		kit++;
	}


        return newlist;
}

void TucanBbsMessage_i::addRecipient(const char* type, TucanAddress_ptr address)
{
	int len;
	TucanAddressSeq *list;
	string rtype = uppercase(type);

	if (myRecipients.find(rtype) != myRecipients.end())
	{
		list = myRecipients[rtype];
		len = list->length();
		list->length(len+1);
		(*list)[len] = TucanAddress::_duplicate(address);
	}
	else
	{
		// If there are no recipients, we should just set it this way
		setRecipient(type, address);
	}
}

void TucanBbsMessage_i::addRecipients(const char* type, const TucanAddressSeq& addresses)
{
	int len;
	TucanAddressSeq *list;
	string rtype = uppercase(type);

	if (myRecipients.find(rtype) != myRecipients.end())
	{
		list = myRecipients[rtype];

		len = list->length();
		list->length(len + addresses.length());
		
		// And set the new ones
		for(CORBA::ULong i = 0; i < addresses.length(); i++)
			(*list)[len+i] = TucanAddress::_duplicate(addresses[i]);

	}
	else
	{
		// If there are no recipients, we should just set it this way
		setRecipients(type, addresses);
	}
}

TucanAddress_ptr TucanBbsMessage_i::getReplyTo()
{
	return TucanAddress::_duplicate(myReplyTo);
}

void TucanBbsMessage_i::setReplyTo(TucanAddress_ptr address)
{
	myReplyTo = TucanAddress::_duplicate(address);
}

char* TucanBbsMessage_i::getSubject()
{
	return CORBA::string_dup(mySubject.c_str());
}

void TucanBbsMessage_i::setSubject(const char* subject)
{
	mySubject = subject;
}

void TucanBbsMessage_i::updateSubject(const char* subject)
{
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string bbstable = config->get("bbsdbtable", "");
	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	string query;
	
	char buf[16];

	sprintf(buf, "%ld", myMessageNumber);

	query = "UPDATE " + bbstable + " SET subject = '" + subject;
	query += "' WHERE oid = ";
	query += buf;
	c->execute(query);

	db->close();
	c->close();

	delete db;
	delete c;
}

void TucanBbsMessage_i::updateBody(const char* body)
{
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string bbstable = config->get("bbsdbtable", "");
	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	string query;
	
	char buf[16];

	sprintf(buf, "%ld", myMessageNumber);

	query = "UPDATE " + bbstable + " SET body = '" + body;
	query += "' WHERE oid = ";
	query += buf;
	c->execute(query);

	db->close();
	c->close();

	delete db;
	delete c;
}

char* TucanBbsMessage_i::getSentDate(const char* format)
{
	struct tm *time;
	string tmp("");
	char *newdate = (char *)malloc(64);

	if (strlen(format) == 0)
		return CORBA::string_dup(mySentDate.c_str());

	time = parsedate(mySentDate.c_str());
	strftime(newdate, 64, format, time);

	tmp += newdate;
	
	free(newdate);
	free(time);
	
	return CORBA::string_dup(tmp.c_str());
}

void TucanBbsMessage_i::setSentDate(const char* date)
{
	mySentDate = date;
}

char* TucanBbsMessage_i::getReceivedDate(const char* format)
{
	struct tm *time;
	string tmp("");
	char *newdate = (char *)malloc(64);

	if (strlen(format) == 0)
		return CORBA::string_dup(myRecvDate.c_str());

	time = parsedate(myRecvDate.c_str());
	strftime(newdate, 64, format, time);

	tmp += newdate;
	
	free(newdate);
	free(time);
	
	return CORBA::string_dup(tmp.c_str());
}

void TucanBbsMessage_i::setReceivedDate(const char* date)
{
	myRecvDate = date;
}

CORBA::Long TucanBbsMessage_i::getMessageNumber()
{
	return myMessageNumber;
}

void TucanBbsMessage_i::setMessageNumber(CORBA::Long msgnum)
{
	myMessageNumber = msgnum;
}

CORBA::Boolean TucanBbsMessage_i::isFlagSet(CORBA::Long flag)
{
	return ((myFlags & flag)?1:0);
}


void TucanBbsMessage_i::setFlags(CORBA::Long flags)
{
	myFlags = flags;
}

void TucanBbsMessage_i::setFlagOn(CORBA::Long flag)
{
	myFlags |= flag;
}

void TucanBbsMessage_i::setFlagOff(CORBA::Long flag)
{
	if (isFlagSet(flag))
		myFlags ^= flag;
}

TucanMessage_ptr TucanBbsMessage_i::reply(CORBA::Boolean replytoall)
{
	throw TucanException("Error: Method reply() has not been implemented.");
}

void TucanBbsMessage_i::destroy()
{
	if (!CORBA::is_nil(myFrom))
		myFrom->destroy();

	if (!CORBA::is_nil(myReplyTo))
		myReplyTo->destroy();

	if (!CORBA::is_nil(myData))
		myData->destroy();

	map<string, TucanAddressSeq*>::iterator kit = myRecipients.begin();

	for (unsigned long i = 0; i < myRecipients.size(); i++)
	{
		string key = (*kit).first;

		if (myRecipients[key])
		{
			CORBA::ULong len = myRecipients[key]->length();

			for (CORBA::ULong j = 0; j < len; j++)
				(*myRecipients[key])[j]->destroy();

			delete myRecipients[key];
		}

		kit++;
	}

	myRecipients.clear();

	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

TucanBbsMessageFactory_i::TucanBbsMessageFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj =
		myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}


TucanMessage_ptr TucanBbsMessageFactory_i::create()
{
	TucanBbsMessage_i* myBbsMessage = 
		new TucanBbsMessage_i(myOrb);
	PortableServer::ObjectId_var myBbsMessageId =
		myPoa->activate_object(myBbsMessage);
	CORBA::Object_var obj =
		myPoa->servant_to_reference(myBbsMessage);
	myBbsMessage->_remove_ref();

	return TucanBbsMessage::_narrow(obj);
}
