//////////////////////////////////////////////////////////
// TucanVoiceMessage_i (c++ impl)  
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
// Filename:		TucanVoiceMessage_i.cc
// Description:		TucanVoiceMessage for CORBA implementation 
// Author:			Rob Flynn <rob@difinium.com>
// Created at:		Wed Feb 21 14:52:30 PST 2001
//////////////////////////////////////////////////////////

#include <TucanVoiceMessage_i.hh>
#include <TucanConfig.hh>
#include <TucanUtilities.hh>

TucanVoiceMessage_i::TucanVoiceMessage_i(CORBA::ORB_var orb)
{
	myOrb = orb;

	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);

	myRecipients.clear();

	myMessageNumber = 0;
	myFlags =0 ;
}

TucanRawData* TucanVoiceMessage_i::getData()
{
	TucanRawData *data = myData->getData();

	return data;
}

TucanData_ptr TucanVoiceMessage_i::getDataObject()
{
	return TucanData::_duplicate(myData);
}

void TucanVoiceMessage_i::setDataObject(TucanData_ptr data)
{
	myData = TucanData::_duplicate(data);
}

TucanAddress_ptr TucanVoiceMessage_i::getFrom()
{
	return TucanAddress::_duplicate(myFrom);
}

void TucanVoiceMessage_i::setFrom(TucanAddress_ptr fromname)
{
	myFrom = TucanAddress::_duplicate(fromname);
}

TucanAddress_ptr TucanVoiceMessage_i::getNewAddressObject()
{
	TucanConfig* config = new TucanConfig();
	string filename = config->get("voice", "address");
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

TucanData_ptr TucanVoiceMessage_i::getNewDataObject()
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

TucanAddressSeq* TucanVoiceMessage_i::getRecipients(const char* type)
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

void TucanVoiceMessage_i::setRecipients(const char* type, const TucanAddressSeq& addresses)
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

void TucanVoiceMessage_i::setRecipient(const char* type, TucanAddress_ptr address)
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

TucanAddressSeq* TucanVoiceMessage_i::getAllRecipients()
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

void TucanVoiceMessage_i::addRecipient(const char* type, TucanAddress_ptr address)
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

void TucanVoiceMessage_i::addRecipients(const char* type, const TucanAddressSeq& addresses)
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



TucanAddress_ptr TucanVoiceMessage_i::getReplyTo()
{
	return TucanAddress::_duplicate(myReplyTo);
}

void TucanVoiceMessage_i::setReplyTo(TucanAddress_ptr address)
{
	myReplyTo = TucanAddress::_duplicate(address);
}

char* TucanVoiceMessage_i::getSubject()
{
	return CORBA::string_dup(mySubject.c_str());
}

void TucanVoiceMessage_i::setSubject(const char* subject)
{
	mySubject = subject;
}

char* TucanVoiceMessage_i::getSentDate(const char* format)
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

void TucanVoiceMessage_i::setSentDate(const char* date)
{
	mySentDate = date;
}

char* TucanVoiceMessage_i::getReceivedDate(const char* format)
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

void TucanVoiceMessage_i::setReceivedDate(const char* date)
{
	myRecvDate = date;
}

CORBA::Long TucanVoiceMessage_i::getMessageNumber()
{
	return myMessageNumber;
}

void TucanVoiceMessage_i::setMessageNumber(CORBA::Long msgnum)
{
	myMessageNumber = msgnum;
}

CORBA::Boolean TucanVoiceMessage_i::isFlagSet(CORBA::Long flag)
{
	return ((myFlags & flag)?1:0);
}

void TucanVoiceMessage_i::setFlags(CORBA::Long flags)
{
	myFlags = flags;
}

void TucanVoiceMessage_i::setFlagOn(CORBA::Long flag)
{
	myFlags |= flag;
}

void TucanVoiceMessage_i::setFlagOff(CORBA::Long flag)
{
	if (isFlagSet(flag))
		myFlags ^= flag;
}

TucanMessage_ptr TucanVoiceMessage_i::reply(CORBA::Boolean replytoall)
{
	throw TucanException("Error: Method reply() has not been implemented.");
}

void TucanVoiceMessage_i::destroy()
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

// Factory

TucanVoiceMessageFactory_i::TucanVoiceMessageFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanMessage_ptr TucanVoiceMessageFactory_i::create()
{
	TucanVoiceMessage_i* myMessage = new TucanVoiceMessage_i(myOrb);
	PortableServer::ObjectId_var myId = myPoa->activate_object(myMessage);
	CORBA::Object_var obj = myPoa->servant_to_reference(myMessage);
	myMessage->_remove_ref();

	return TucanVoiceMessage::_narrow(obj);
}
