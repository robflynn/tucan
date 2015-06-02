//////////////////////////////////////////////////////////
// TucanCalendarStore (c++ impl)
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
// Filename:        TucanCalendarStore_i.cc 
// Description:     The Calendar store interface 
// Author:          Jared Peterson <jared@difinium.com>
// Created at:      Fri Jan 19 10:05:49 PST 2001 
////////////////////////////////////////////////////////////

#include <TucanCalendarStore_i.hh>
#include <TucanConfig.hh>
#include <TucanDb.hh>
#include <TucanUtilities.hh>
#include <stdio.h>
#include <fstream.h>
#include <time.h>
#include <string>
#include <vector>

TucanCalendarStore_i::TucanCalendarStore_i(CORBA::ORB_var orb)
{
	myName = "None";
	myOrb = orb;
	CORBA::Object_var obj = 
		myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

char* TucanCalendarStore_i::getName()
{
	return CORBA::string_dup(myName.c_str());
}

void TucanCalendarStore_i::setName(const char* name)
{
	myName = name;
}

//getDefaultFolder() will return the a folder of the current
//date ... it will only contain messages if there are messages
//for that day
TucanFolder_ptr TucanCalendarStore_i::getDefaultFolder()
{
	time_t curr_time;
	time_t *curr_ptr;
	struct tm *time_struct;
	char *date;

	curr_ptr = (time_t*)malloc(32);
	curr_time = time(curr_ptr);
	time_struct = localtime(curr_ptr);

	//Get our date
	date = (char*)malloc(32);
	strftime(date, 32, "%m/%d/%Y", time_struct);

	//Get the folder object and set the name
	TucanFolder_ptr folder = getNewFolderObject();
	folder->setName(date);
	folder->setAuth(TucanAuth::_duplicate(myAuth));

	//Free the memory
	free(date);
	free(curr_ptr);
	
	return folder;
}

TucanFolder_ptr TucanCalendarStore_i::getFolder(const char* name)
{
	CORBA::String_var temp;
	string str_name(name);
	int exist = 0;
	TucanStringList* master_list;
	
	master_list = listFolders();
	
	for(int i = 0; i < (signed)master_list->length(); i++)
	{
		temp = (*master_list)[i];

		if(!strcmp(str_name.c_str(), temp))
		{
			exist = 1;
			break;
		}
	}

	delete master_list;
	
	if(!(exist))
	{
		string exp_string("Error: Folder ");
		exp_string += name;
		exp_string += " does not exist.";
		throw TucanException(exp_string.c_str());
	}

	TucanFolder_ptr folder = getNewFolderObject();
	folder->setName(name);
	folder->setAuth(myAuth);
	
	return folder;
}

TucanFolder_ptr TucanCalendarStore_i::getNewFolderObject()
{
	string filename;
	string str;
	TucanFolder_ptr folder;
	
	TucanConfig* config = new TucanConfig();
	filename = config->get("calendar", "folder");
	delete config;

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

TucanStringList* TucanCalendarStore_i::listFolders()
{
	CORBA::String_var uid = myAuth->getUid();

	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	string exec = "SELECT * FROM calendar WHERE uid=";
	exec += uid;
	exec += " ORDER BY date, time";
	c->execute(exec);
	vector< vector<string> > result = c->fetchall();
	c->close();
	db->close();
	delete db;
	delete c;

	TucanStringList* master_list = new TucanStringList;
	//TucanStringList_var master_list = new TucanStringList;
	master_list->length(result.size() * 3);
	
	int insert = 0;
	int counter = 0;

	for(int i=0; i < (signed)result.size(); i++)
	{
		vector<string> string_list = strsplit(result[i][1], '-');
		string year_string(string_list[2]);
		
		for(CORBA::ULong j = 0; j < master_list->length(); j++)
		{
			string temp((*master_list)[j]);
			
			if(!(temp.compare(year_string)))
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
				CORBA::string_dup(year_string.c_str());
			counter++;
		}

		insert = 0;

		string month_year_string;
		month_year_string = string_list[0] + "/" + string_list[2];

		for(CORBA::ULong k = 0; k < master_list->length(); k++)
		{
			string temp((*master_list)[k]);

			if(!(temp.compare(month_year_string)))
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
				CORBA::string_dup(month_year_string.c_str());
			counter++;
		}

		string month_day_year_string;
		month_day_year_string = string_list[0] + "/" +
			string_list[1] + "/" + string_list[2];

		for(CORBA::ULong l = 0; l < master_list->length(); l++)
		{
			string temp((*master_list)[l]);

			if(!(temp.compare(month_day_year_string)))
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
				CORBA::string_dup(month_day_year_string.c_str());
			counter++;
		}
		
		string_list.clear();
	}

	result.clear();
	master_list->length(counter);
	return master_list;
}

TucanAuth_ptr TucanCalendarStore_i::getAuth()
{
	return TucanAuth::_duplicate(myAuth);
}

void TucanCalendarStore_i::setAuth(TucanAuth_ptr auth)
{
	myAuth = TucanAuth::_duplicate(auth);
}

void TucanCalendarStore_i::destroy()
{
	PortableServer::ObjectId_var id = 
		myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

TucanCalendarStoreFactory_i::TucanCalendarStoreFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = 
		myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanStore_ptr TucanCalendarStoreFactory_i::create()
{
	TucanCalendarStore_i* myCalendarStore = 
		new TucanCalendarStore_i(myOrb);
	PortableServer::ObjectId_var myCalendarStoreId = 
		myPoa->activate_object(myCalendarStore);
	CORBA::Object_var obj = 
		myPoa->servant_to_reference(myCalendarStore);
	myCalendarStore->_remove_ref();
	
	return TucanCalendarStore::_narrow(obj);
}
