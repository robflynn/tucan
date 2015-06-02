////////////////////////////////////////////////////////////
// TucanCalendarFolder (c++ impl)
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
// Filename:        TucanCalendarFolder_i.cc 
// Description:     The Calendar folder interface 
// Author:          Jared Peterson <jared@difinium.com>
// Created at:      Fri Jan 26 10:51:53 PST 2001 
////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>
#include <time.h>
#include <string>
#include <vector>
#include <TucanUtilities.hh>
#include <TucanConfig.hh>
#include <TucanDb.hh>
#include <TucanCalendarFolder_i.hh>

TucanCalendarFolder_i::TucanCalendarFolder_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj =
		myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

CORBA::Boolean TucanCalendarFolder_i::exists()
{
	string date1;
	string date2;
	string date3;
	int month;
	string days;
	int query = 0;
	int rows;

	vector<string> string_list = strsplit(myName, '/');

	if(string_list.size() == 1)
	{
		date1 = "1/1/" + string_list[0];
		date2 = "12/31/" + string_list[0];
	}
	else if(string_list.size() == 2)
	{
		month = atoi(string_list[0].c_str());

		if(month == 2)
		{
			int year = atoi(string_list[1].c_str());
			int result = year % 4;
			
			if(result == 0)
			{
				result = year % 100;

				if(result == 0)
				{
					result = year % 400;

					if(result == 0)
					{
						days = "29";
					}
					else
					{
						days = "29";
					}
				}
				else
				{
					days = "29";
				}
			}
			else
			{
				days = "28";
			}
		}
		else if(month == 4 || month == 6 || month == 9 || month == 11)
			days = "30";
		else
			days ="31";
	
		date1 = string_list[0] + "/1/" 
			+ string_list[1];
		date2 = string_list[0] + "/" 
			+ days + "/" + string_list[1];
	}
	else if(string_list.size() == 3)
	{
		date3 = myName;
		query = 1;
	}

	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("caldbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	CORBA::String_var uid_tmp = myAuth->getUid();
	string uid(uid_tmp);
	
	if(query)
	{
		//Search for just a date
		string exec = "SELECT * FROM " + tablename 
			+ " WHERE uid=" + uid + " AND date='" 
			+ date3 + "'";
		c->execute(exec);
	}
	else
	{
		//Search for a range
		string exec = "SELECT * FROM " + tablename 
			+ " WHERE uid=" + uid + " AND date >= '" 
			+ date1 +"' AND date <= '" + date2 +"'";
		c->execute(exec);
	}

	rows = c->rowcount();
	
	c->close();
	db->close();
	delete db;
	delete c;

	if(rows > 0)
		return 1;
	else
		return 0;
}

CORBA::Boolean TucanCalendarFolder_i::create()
{
	throw TucanException("Error: Method create() has not been implemented.");
}

CORBA::Boolean TucanCalendarFolder_i::deleteFolder()
{
	if(!(exists()))
	{
		string exec_string = "Error: Folder " + myName 
			+ " does not exist.";
		throw TucanException(exec_string.c_str());
	}

	string date1;
	string date2;
	string date3;
	int month;
	string days;
	int query = 0;

	vector<string> string_list = strsplit(myName, '/');

	if(string_list.size() == 1)
	{
		date1 = "1/1/" + string_list[0];
		date2 = "12/31/" + string_list[0];
	}
	else if(string_list.size() == 2)
	{
		month = atoi(string_list[0].c_str());

		if(month == 2)
			days = "29";
		else if(month == 4 || month == 6 || month == 9 || month == 11)
			days = "30";
		else
			days ="31";
	
		date1 = string_list[0] + "/1/" 
			+ string_list[1];
		date2 = string_list[0] + "/" 
			+ days + "/" + string_list[1];
	}
	else if(string_list.size() == 3)
	{
		date3 = myName;
		query = 1;
	}

	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("caldbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	CORBA::String_var uid_tmp = myAuth->getUid();
	string uid(uid_tmp);

	if(query)
	{
		string exec = "DELETE FROM " + tablename 
			+ " WHERE uid=" + uid + " AND date='" 
			+ date3 + "'";
		c->execute(exec);
	}
	else
	{
		string exec = "DELETE FROM " + tablename 
			+ " WHERE uid=" + uid + " AND date >= '" 
			+ date1 + "' AND date <= '" + date2 + "'";
		c->execute(exec);
	}
	
	c->close();
	db->close();
	delete db;
	delete c;
	string_list.clear();
	
	return 1;
}

TucanMessageSeq* TucanCalendarFolder_i::expunge()
{
	throw TucanException("Error: Method expunge() has not been implemented.");
}

TucanFolder_ptr TucanCalendarFolder_i::getFolder(const char* name)
{
	if(!(exists()))
	{
		string excp_string = "Error: Folder " + myName 
			+ " does not exist.";
		throw TucanException(excp_string.c_str());
	}

	vector<string> string_list1 = strsplit(myName, '/');
	vector<string> string_list2 = strsplit(name, '/');

	if(string_list1.size() == 1)
	{
		if(string_list2.size() != 2)
		{
			string_list1.clear();
			string_list2.clear();

			string excp_string = "Error: Folder " + myName 
				+ " does not contain the folder " + name;
			throw TucanException(excp_string.c_str());
		}
	}
	else if(string_list1.size() == 2)
	{
		if(string_list2.size() != 3)
		{
			string_list1.clear();
			string_list2.clear();

			string excp_string = "Error: Folder " + myName
				+ " does not contain the folder " + name;
			throw TucanException(excp_string.c_str());
		}
	}
	else if(string_list1.size() == 3)
	{
		string_list1.clear();
		string_list2.clear();

		string excp_string = "Error: Folder " + myName
			+ " does not contain the folder " + name;
		throw TucanException(excp_string.c_str());
	}

	TucanFolder_ptr folder = getNewFolderObject();
	folder->setName(name);
	folder->setAuth(myAuth);
	
	string_list1.clear();
	string_list2.clear();
			
	return folder;
}

TucanFolder_ptr TucanCalendarFolder_i::getNewFolderObject()
{
	TucanConfig* config = new TucanConfig();
	string filename = config->get("calendar", "folder");
	delete config;
	string str;
	TucanFolder_ptr folder;

	ifstream infile(filename.c_str());
	getline(infile, str);
	infile.close();

	CORBA::Object_var object =
		myOrb->string_to_object(str.c_str());
	TucanFolderFactory_var fact =
		TucanFolderFactory::_narrow(object);
	folder = fact->create();

	return folder;
}

void TucanCalendarFolder_i::setParent(TucanFolder_ptr folder)
{
	throw TucanException("Error: Method setParent() has not been implemented.");
}

TucanFolder_ptr TucanCalendarFolder_i::getParent()
{
	throw TucanException("Error: Method getParent() has not been implemented.");
}

TucanStringList* TucanCalendarFolder_i::listFolders()
{
	if(!(exists()))
	{
		string excp_string = "Error: Folder " + myName 
			+ " does not exist.";
		throw TucanException(excp_string.c_str());
	}
	
	string date1;
	string date2;
	int month;
	string days;
	
	vector<string> string_list = strsplit(myName, '/');

	if(string_list.size() == 1)
	{
		date1 = "1/1/" + string_list[0];
		date2 = "12/31/" + string_list[0];
	}
	else if(string_list.size() == 2)
	{
		month = atoi(string_list[0].c_str());

		if(month == 2)
			days = "29";
		else if(month == 4 || month == 6 || month == 9 || month == 11)
			days = "30";
		else
			days = "31";

		date1 = string_list[0] + "/1/" 
			+ string_list[1];
		date2 = string_list[0] + "/" + days 
			+ "/" + string_list[1];
	}
	else
	{
		string excp_string = "Error: Folder " + myName
			+ " does not contain folders.";
		throw TucanException(excp_string.c_str());
	}
	
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("caldbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	CORBA::String_var uid_temp = myAuth->getUid();
	string uid(uid_temp);

	string exec = "SELECT * FROM " + tablename
		+ " WHERE uid=" + uid + " AND date >='" + date1
		+ "' AND date <= '" + date2 + "'";
	c->execute(exec);
	vector< vector<string> > result = c->fetchall();
	c->close();
	db->close();
	delete c;
	delete db;

	TucanStringList* master_list = new TucanStringList;
	master_list->length(result.size() * 3);
	int insert = 0;
	int counter = 0;

	for(int i = 0; i < (signed)result.size(); i++)
	{
		vector<string> date_list = strsplit(result[i][1], '-');

		if(string_list.size() == 1)
		{
			string temp_string = date_list[0] + "/" 
				+ date_list[2];

			for(CORBA::ULong j = 0; j < master_list->length(); j++)
			{
				string temp((*master_list)[j]);

				if(!(temp.compare(temp_string)))
				{
					insert = 0;
					break;
				}
				else
					insert = 1;
			}

			if(insert)
			{
				(*master_list)[counter] = 
					CORBA::string_dup(temp_string.c_str());
				counter++;
			}

			insert = 0;
		}
		else if(string_list.size() == 2)
		{
			string temp_string = date_list[0] + "/" 
				+ date_list[1] + "/" + date_list[2];
			
			for(CORBA::ULong j = 0; j < master_list->length(); j++)
			{
				string temp((*master_list)[j]);

				if(!(temp.compare(temp_string)))
				{
					insert = 0;
					break;
				}
				else
					insert = 1;
			}

			if(insert)
			{
				(*master_list)[counter] = 
					CORBA::string_dup(temp_string.c_str());
				counter++;
			}

			insert = 0;
		}
	}

	master_list->length(counter);
	return master_list;
}

TucanFolderSeq* TucanCalendarFolder_i::list()
{
	TucanStringList* folder_list = new TucanStringList;
	folder_list = listFolders();
	TucanFolderSeq* master_list = new TucanFolderSeq;
	master_list->length(folder_list->length());

	for(CORBA::ULong i = 0; i < folder_list->length(); i++)
	{
		TucanFolder_ptr folder = getNewFolderObject();
		folder->setName((*folder_list)[i]);
		folder->setAuth(TucanAuth::_duplicate(myAuth));
		(*master_list)[i] = folder;
	}
		
	return master_list;
}

CORBA::Boolean TucanCalendarFolder_i::renameTo(const char* name)
{
	throw TucanException("Error: Method renameTo() has not been implemented.");
}

char* TucanCalendarFolder_i::getName()
{
	return CORBA::string_dup(myName.c_str());
}

void TucanCalendarFolder_i::setName(const char* name)
{
	myName = name;
}

char* TucanCalendarFolder_i::getFullName()
{
	return CORBA::string_dup(myFullName.c_str());
}

void TucanCalendarFolder_i::setFullName(const char* name)
{
	myFullName = name;
}

TucanMessage_ptr TucanCalendarFolder_i::getMessage(CORBA::Long msgnum)
{
	if(!(exists()))
	{
		string excp_string = "Error: Folder " + myName 
			+ " does not exist.";
		throw TucanException(excp_string.c_str());
	}

	CORBA::Long max = getMessageCount();

	if(msgnum > max || msgnum == 0)
	{
		string excp_string = "Error: Requested message out of range.";
		throw TucanException(excp_string.c_str());
	}

	string date1;
	string date2;
	string date3;
	int month;
	string days;
	int query = 0;
	vector<string> temp_list = strsplit(myName, '/');

	if(temp_list.size() == 1)
	{
		date1 = "1/1/" + temp_list[0];
		date2 = "12/31/" + temp_list[0];
	}
	else if(temp_list.size() == 2)
	{
		month = atoi(temp_list[0].c_str());

		if(month == 2)
			days = "29";
		else if(month == 4 || month == 6 || month == 9 || month == 11)
			days = "30";
		else
			days = "31";

		date1 = temp_list[0] + "/1/" 
			+ temp_list[1];
		date2 = temp_list[0] + "/"
			+ days + "/" + temp_list[1];
	}
	else
	{
		date3 = myName;
		query = 1;
	}
	
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("caldbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	CORBA::String_var uid_tmp = myAuth->getUid();
	string uid(uid_tmp);

	if(query)
	{
		string exec = "SELECT * FROM " + tablename
			+ " WHERE uid=" + uid + " AND date='" 
			+ date3 + "' ORDER BY date,time";
		c->execute(exec);
	}
	else
	{
		string exec = "SELECT * FROM " + tablename
			+ " WHERE uid='" + uid + "' AND date >= '"
			+ date1 + "' AND date <= '" + date2 
			+ "' ORDER BY date,time";
		c->execute(exec);
	}

	vector< vector<string> > result = c->fetchall();
	c->close();
	db->close();
	delete c;
	delete db;

	vector<string> my_msg = result[msgnum - 1];
	TucanMessage_var msg = getNewMessageObject();
	TucanCalendarMessage_ptr the_msg = TucanCalendarMessage::_narrow(msg);

	string temp_date_str("");

	temp_date_str = my_msg[1];
	temp_date_str += " ";
	temp_date_str += my_msg[2];

	the_msg->setMessageNumber(msgnum);
	the_msg->setSentDate(temp_date_str.c_str());
	the_msg->setReceivedDate(temp_date_str.c_str());
	
	the_msg->setTime(my_msg[2].c_str());

	if(!(my_msg[4].compare("1")))
		the_msg->setNotification("email");
	else if(!(my_msg[4].compare("2")))
		the_msg->setNotification("phone");
	else if(!(my_msg[4].compare("3")))
		the_msg->setNotification("fax");
	else if(!(my_msg[4].compare("4")))
		the_msg->setNotification("all");
	else
		the_msg->setNotification("none");

	the_msg->setNotifyDate(my_msg[5].c_str());
	the_msg->setNotifyTime(my_msg[6].c_str());

	if(!(my_msg[7].compare("f")))
		the_msg->setNotified(0);
	else
		the_msg->setNotified(1);

	TucanData_var data = the_msg->getNewDataObject();
	data->setContentType(my_msg[8].c_str());

	if (!(strncasecmp(my_msg[8].c_str(), "audio/", 6)))
	{
		TucanRawData audioData;
		CORBA::String_var tmp = myAuth->getUserName();
		
		string audioFile(tmp);

		audioFile = gethomedir(audioFile);
		audioFile += "/cal/" + my_msg[3];

		int myFile;
		unsigned long num, i, j;
		char buffer[1024];

		// Open the audio file
		myFile = open(audioFile.c_str(), O_RDONLY);
		
		i = 0;

		do
		{
			// Put some data in the buffer
			num = read(myFile, buffer, 1024);
			audioData.length(audioData.length() + num);

			// Copy it to the raw data
			for (j = 0; j < num; j++)
				audioData[i++] = buffer[j];
			
		} while (num > 0);

		// Close the audio file
		close(myFile);

		data->setData(audioData);
	}
	else
	{
		data->setText(my_msg[3].c_str());
	}

	the_msg->setDataObject(data);

	if(!(my_msg[9].compare("1")))
		the_msg->setFlagOn(MESSAGE_EXPUNGED);

	result.clear();
	my_msg.clear();
	temp_list.clear();
	
	return the_msg;
}

TucanMessageSeq* TucanCalendarFolder_i::getMessages()
{
	long count, i;
	TucanMessageSeq* list = new TucanMessageSeq;
	TucanMessage_var message;

	count = getMessageCount();

	list->length(count);
	
	for (i = 0; i < count; i++)
	{
		message = getMessage(i + 1);
		(*list)[i] = TucanMessage::_duplicate(message);
	}

	return list;
}

TucanMessageSeq* TucanCalendarFolder_i::getNewMessages()
{
	throw TucanException("Error: Method getNewMessages() has not been implemented.");
}

TucanMessageSeq* TucanCalendarFolder_i::getMessagesInRange
	(CORBA::Long start, CORBA::Long stop)
{
	throw TucanException("Error: Method getMessagesInRange() has not been implemented.");
}

TucanMessage_ptr TucanCalendarFolder_i::getNewMessageObject()
{
	TucanConfig* config = new TucanConfig();
	string filename = config->get("calendar", "message");
	delete config;
	string str;
	TucanMessage_ptr message;

	ifstream infile(filename.c_str());
	getline(infile, str);
	infile.close();

	CORBA::Object_var object =
		myOrb->string_to_object(str.c_str());
	TucanMessageFactory_var fact =
		TucanMessageFactory::_narrow(object);
	message = fact->create();

	return message;
}

CORBA::Long TucanCalendarFolder_i::getMessageCount()
{
	if(!(exists()))
	{
		string excp_string = "Error: Folder " + myName 
			+ " does not exist.";
		throw TucanException(excp_string.c_str());
	}

	string date1;
	string date2;
	string date3;
	int month;
	string days;
	int query = 0;
	vector<string> temp_list = strsplit(myName, '/');

	if(temp_list.size() == 1)
	{
		date1 = "1/1/" + temp_list[0];
		date2 = "12/31/" + temp_list[0];
	}
	else if(temp_list.size() == 2)
	{
		month = atoi(temp_list[0].c_str());

		if(month == 2)
			days = "29";
		else if(month == 4 || month == 6 || month == 9 || month == 11)
			days = "30";
		else
			days = "31";

		date1 = temp_list[0] + "/1/" 
			+ temp_list[1];
		date2 = temp_list[0] + "/"
			+ days + "/" + temp_list[1];
	}
	else
	{
		date3 = myName;
		query = 1;
	}
	
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("caldbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	CORBA::String_var uid_tmp = myAuth->getUid();
	string uid(uid_tmp);

	if(query)
	{
		string exec = "SELECT * FROM " + tablename
			+ " WHERE uid=" + uid + " AND date='" 
			+ date3 + "'";
		c->execute(exec);
	}
	else
	{
		string exec = "SELECT * FROM " + tablename
			+ " WHERE uid='" + uid + "' AND date >= '"
			+ date1 + "' AND date <= '" + date2 + "'";
		c->execute(exec);
	}

	CORBA::Long num = c->rowcount();
	c->close();
	db->close();
	delete c;
	delete db;
	temp_list.clear();


	return num;
}

CORBA::Long TucanCalendarFolder_i::getNewMessageCount()
{
	if(!(exists()))
	{
		string excp_string = "Error: Folder " + myName 
			+ " does not exist.";
		throw TucanException(excp_string.c_str());
	}

	//We need to get today's date
	time_t curr_time;
	time_t *curr_ptr;
	struct tm *time_struct;
	char *date;
	string today;

	curr_ptr = (time_t*)malloc(32);
	curr_time = time(curr_ptr);
	time_struct = localtime(curr_ptr);
	
	date = (char*)malloc(32);
	strftime(date, 32, "%m/%d/%Y", time_struct);

	today = date;
	free(date);
	free(curr_ptr);

	//Seperate the date into seperate parts
	vector<string> temp_list = strsplit(today, '/');
	string cur_month = temp_list[0];
	string cur_day = temp_list[1];
	string cur_year = temp_list[2];
	
	vector<string> temp_list2 = strsplit(myName, '/');
	int query = 0;

	//Decide whether we should query the db or not?
	if(temp_list2.size() == 1)
	{
		if(!(cur_year.compare(temp_list2[0])))
			query = 1;
	}
	else if(temp_list2.size() == 2)
	{
		if(!(cur_month.compare(temp_list2[0])) 
				&& !(cur_year.compare(temp_list2[1])))
		{
			query = 1;
		}
	}
	else
	{
		if(!(cur_month.compare(temp_list2[0])) 
				&& !(cur_day.compare(temp_list2[1]))
				&& !(cur_year.compare(temp_list2[2])))
		{
			query = 1;
		}
	}

	CORBA::Long count = 0;

	//Query if we need to
	if(query)
	{
		TucanConfig* config = new TucanConfig();
		string dbname = config->get("dbname", "");
		string tablename = config->get("caldbtable", "");
		delete config;

		connection* db = new connection(dbname);
		cursor* c = db->getCursor();
		CORBA::String_var uid_tmp = myAuth->getUid();
		string uid(uid_tmp);

		string exec = "SELECT * FROM " + tablename
			+ " WHERE uid=" + uid + " and date='"
			+ today + "'";
		c->execute(exec);
	
		count = c->rowcount();

		c->close();
		db->close();
		delete c;
		delete db;
	}

	temp_list.clear();
	temp_list2.clear();

	return count;
}

CORBA::Boolean TucanCalendarFolder_i::hasNewMessages()
{
	CORBA::Long count = getNewMessageCount();

	if(count > 0)
		return 1;
	else
		return 0;
}

void TucanCalendarFolder_i::moveMessage(CORBA::Long msgnum, 
		TucanFolder_ptr folder)
{
	throw TucanException("Error: Method moveMessage() has not been implemented.");
}

void TucanCalendarFolder_i::moveMessages(const TucanLongList& msgnums, 
		TucanFolder_ptr folder)
{
	throw TucanException("Error: Method moveMessages() has not been implemented.");
}

void TucanCalendarFolder_i::copyMessage(CORBA::Long msgnum, 
		TucanFolder_ptr folder)
{
	throw TucanException("Error: Method copyMessage() has not been implemented.");
}

void TucanCalendarFolder_i::copyMessages(const TucanLongList& msgnums, 
		TucanFolder_ptr folder)
{
	throw TucanException("Error: Method copyMessages() has not been implemented.");
}

void TucanCalendarFolder_i::appendMessage(TucanMessage_ptr msg)
{
	throw TucanException("Error: Method appendMessage() has not been implemented.");
}

void TucanCalendarFolder_i::appendMessages(const TucanMessageSeq& msgs)
{
	throw TucanException("Error: Method appendMessages() has not been implemented.");
}

void TucanCalendarFolder_i::deleteMessage(CORBA::Long msgnum)
{
	if(!(exists()))
	{
		string excp_string = "Error: Folder " + myName 
			+ " does not exist.";
		throw TucanException(excp_string.c_str());
	}

	CORBA::Long max = getMessageCount();
	
	if(msgnum > max || msgnum == 0)
	{
		string excp_string = "Error: Requested message out of range.";
		throw TucanException(excp_string.c_str());
	}

	string date1;
	string date2;
	string date3;
	int month;
	string days;
	int query = 0;
	vector<string> temp_list = strsplit(myName, '/');

	if(temp_list.size() == 1)
	{
		date1 = "1/1/" + temp_list[0];
		date2 = "12/31/" + temp_list[0];
	}
	else if(temp_list.size() == 2)
	{
		month = atoi(temp_list[0].c_str());

		if(month == 2)
			days = "29";
		else if(month == 4 || month == 6 || month == 9 || month == 11)
			days = "30";
		else
			days = "31";

		date1 = temp_list[0] + "/1/" 
			+ temp_list[1];
		date2 = temp_list[0] + "/"
			+ days + "/" + temp_list[1];
	}
	else
	{
		date3 = myName;
		query = 1;
	}
	
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("caldbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	CORBA::String_var uid_tmp = myAuth->getUid();
	string uid(uid_tmp);

	// FIXME: This is a hack. I dont want to depend on Postgres' OID.  I shall come back
	// to this and find a way around it.
	
	if(query)
	{
		string exec = "SELECT oid FROM " + tablename
			+ " WHERE uid=" + uid + " AND date='" 
			+ date3 + "' ORDER BY date,time";
		c->execute(exec);
	}
	else
	{
		string exec = "SELECT oid FROM " + tablename
			+ " WHERE uid='" + uid + "' AND date >= '"
			+ date1 + "' AND date <= '" + date2 
			+ "' ORDER BY date,time";
		c->execute(exec);
	}

	vector< vector<string> > result = c->fetchall();


	string num = result[msgnum - 1][0];

	string exec = "DELETE FROM " + tablename + " WHERE oid=" + num;
	c->execute(exec);

	c->close();
	db->close();
	delete c;
	delete db;

	result.clear();
	temp_list.clear();


}

TucanAuth_ptr TucanCalendarFolder_i::getAuth()
{
	return TucanAuth::_duplicate(myAuth);
}

void TucanCalendarFolder_i::setAuth(TucanAuth_ptr auth)
{
	myAuth = TucanAuth::_duplicate(auth);
}

void TucanCalendarFolder_i::destroy()
{
	PortableServer::ObjectId_var id =
		myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

TucanCalendarFolderFactory_i::TucanCalendarFolderFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = 
		myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanFolder_ptr TucanCalendarFolderFactory_i::create()
{
	TucanCalendarFolder_i* myCalendarFolder = 
		new TucanCalendarFolder_i(myOrb);
	PortableServer::ObjectId_var myCalendarFolderId =
		myPoa->activate_object(myCalendarFolder);
	CORBA::Object_var obj = 
		myPoa->servant_to_reference(myCalendarFolder);
	myCalendarFolder->_remove_ref();
	
	return TucanCalendarFolder::_narrow(obj);
}
