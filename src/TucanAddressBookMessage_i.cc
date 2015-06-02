////////////////////////////////////////////////////////////
// TucanAddressBookMessage (c++ impl)
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
// Filename:        TucanAddressBookMessage_i.cc 
// Description:     The Address Book Message interface 
// Author:          Mathew Davis <mattd@difinium.com>
// Created at:      Tue Feb 13 16:32:31 PST 2001 
////////////////////////////////////////////////////////////

#include <TucanAddressBookMessage_i.hh>
#include <TucanUtilities.hh>

TucanAddressBookMessage_i::TucanAddressBookMessage_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj =
		myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
	myMsgNum = 0;  //This needs to be changed//
	myGroup = "none";
	myFrom = NULL;
	myReplyTo = NULL;

	myRecipients.clear();

	myFlags = 0;
}

char* TucanAddressBookMessage_i::getAttribute(const char* key)
{
	return CORBA::string_dup(myAttributes[key].c_str());
}

void TucanAddressBookMessage_i::setAttribute(const char* key, const char* value)
{
	// Set the value in the attribute hash
	myAttributes.insert(pair<string, string>(key, value));
	
	// Update the text that is stored in the data object
	myHash::iterator step = myAttributes.begin();
	string newText("");

	// Build a new text string
	for (unsigned int i = 0; i < myAttributes.size(); i++)
	{
		string myKey = (*step).first;
		string myValue = myAttributes[myKey];

		newText += myKey;
		newText += ": ";
		newText += myValue;
		newText += "\n";

		step++;
	}

	myData->setText(newText.c_str());
}

TucanAttributeSeq* TucanAddressBookMessage_i::getAttributes()
{
	TucanAttributeSeq *result = new TucanAttributeSeq;
	TucanAttribute temp;
	myHash::iterator i;
	unsigned int j;
	CORBA::ULong counter = 0;

	for(j = 0, i = myAttributes.begin(); j < myAttributes.size(); i++, j++)
	{
		string key;
		string value;

		key = i->first;
		value = myAttributes[key];

		temp.name = CORBA::string_dup(key.c_str());
		temp.value = CORBA::string_dup(value.c_str());
		result->length(counter + 1);
		(*result)[counter] = temp;
		
		counter++;
	}

	return result;
}

TucanStringList* TucanAddressBookMessage_i::listAttributes()
{
	throw TucanException("Error: Method listAttributes() has not been implemented.");
}

char* TucanAddressBookMessage_i::getGroup()
{
	return CORBA::string_dup(myGroup.c_str());
}

void TucanAddressBookMessage_i::setGroup(const char* group)
{
	myGroup = group;
}

char* TucanAddressBookMessage_i::getTime()
{
	return CORBA::string_dup(myTime.c_str());
}

void TucanAddressBookMessage_i::setTime(const char* time)
{
	myTime = time; 
}

TucanRawData* TucanAddressBookMessage_i::getData()
{
	TucanRawData *data = myData->getData();

	return data;
}

TucanData_ptr TucanAddressBookMessage_i::getDataObject()
{
	return TucanData::_duplicate(myData);
}

void TucanAddressBookMessage_i::setDataObject(TucanData_ptr data)
{
	myData = TucanData::_duplicate(data);
}

TucanData_ptr TucanAddressBookMessage_i::getNewDataObject()
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

TucanAddress_ptr TucanAddressBookMessage_i::getFrom()
{
	return TucanAddress::_duplicate(myFrom);
}

void TucanAddressBookMessage_i::setFrom(TucanAddress_ptr fromname)
{
	myFrom = TucanAddress::_duplicate(fromname);	
}

TucanAddress_ptr TucanAddressBookMessage_i::getNewAddressObject()
{
	TucanConfig* config = new TucanConfig();
	string filename = config->get("addressbook", "address");
	delete config;
	string str;
	TucanAddress_ptr address;

	ifstream infile(filename.c_str());
	getline(infile, str);
	infile.close();

	CORBA::Object_var object =
		myOrb->string_to_object(str.c_str());
	TucanAddressFactory_var fact =
		TucanAddressFactory::_narrow(object);
	address = fact->create();

	return address;
}

TucanAddressSeq* TucanAddressBookMessage_i::getRecipients(const char* type)
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

void TucanAddressBookMessage_i::setRecipients(const char* type, const TucanAddressSeq& addresses)
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

void TucanAddressBookMessage_i::setRecipient(const char* type, TucanAddress_ptr address)
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

TucanAddressSeq* TucanAddressBookMessage_i::getAllRecipients()
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

void TucanAddressBookMessage_i::addRecipient(const char* type, TucanAddress_ptr address)
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

void TucanAddressBookMessage_i::addRecipients(const char* type, const TucanAddressSeq& addresses)
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

TucanAddress_ptr TucanAddressBookMessage_i::getReplyTo()
{
	return TucanAddress::_duplicate(myReplyTo);
}

void TucanAddressBookMessage_i::setReplyTo(TucanAddress_ptr address)
{
	myReplyTo = TucanAddress::_duplicate(address);
}

char* TucanAddressBookMessage_i::getSubject()
{
	return CORBA::string_dup(mySubject.c_str());
}

void TucanAddressBookMessage_i::setSubject(const char* subject)
{
	mySubject = subject;
}

char* TucanAddressBookMessage_i::getSentDate(const char* format)
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

void TucanAddressBookMessage_i::setSentDate(const char* date)
{
	mySentDate = date;
}

char* TucanAddressBookMessage_i::getReceivedDate(const char* format)
{
	struct tm *time;
	string tmp("");
	char *newdate = (char *)malloc(64);

	if (strlen(format) == 0)
		return CORBA::string_dup(myReceivedDate.c_str());

	time = parsedate(myReceivedDate.c_str());
	strftime(newdate, 64, format, time);

	tmp += newdate;
	
	free(newdate);
	free(time);
	
	return CORBA::string_dup(tmp.c_str());
}

void TucanAddressBookMessage_i::setReceivedDate(const char* date)
{
	myReceivedDate = date;
}


CORBA::Long TucanAddressBookMessage_i::getMessageNumber()
{
	return myMsgNum;
}

void TucanAddressBookMessage_i::setMessageNumber(CORBA::Long msgnum)
{
	myMsgNum = msgnum;
}

CORBA::Boolean TucanAddressBookMessage_i::isFlagSet(CORBA::Long flag)
{
	return ((myFlags & flag)?1:0);
}


void TucanAddressBookMessage_i::setFlags(CORBA::Long flags)
{
	myFlags = flags;
}

void TucanAddressBookMessage_i::setFlagOn(CORBA::Long flag)
{
	myFlags |= flag;
}

void TucanAddressBookMessage_i::setFlagOff(CORBA::Long flag)
{
	if (isFlagSet(flag))
		myFlags ^= flag;
}

TucanMessage_ptr TucanAddressBookMessage_i::reply(CORBA::Boolean replytoall)
{
	throw TucanException("Error: Method reply() has not been implemented.");
}

void TucanAddressBookMessage_i::destroy()
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

TucanAddressBookMessageFactory_i::TucanAddressBookMessageFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj =
		myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanMessage_ptr TucanAddressBookMessageFactory_i::create()
{
	TucanAddressBookMessage_i* myAddressBookMessage =
		new TucanAddressBookMessage_i(myOrb);
	PortableServer::ObjectId_var myAddressBookMessageId =
		myPoa->activate_object(myAddressBookMessage);
	CORBA::Object_var obj =
		myPoa->servant_to_reference(myAddressBookMessage);
	myAddressBookMessage->_remove_ref();

	return TucanAddressBookMessage::_narrow(obj);
}

