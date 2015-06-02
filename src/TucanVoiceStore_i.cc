//////////////////////////////////////////////////////////
// TucanVoiceStore_i (c++ impl)  
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
// Filename:        TucanVoiceStore_i.cc
// Description:     TucanVoiceStore for CORBA implementation 
// Author:          Rob Flynn <rob@difinium.com> 
// Created at:      Fri Feb 21 16:20:53 PST 2001
//////////////////////////////////////////////////////////

#include <TucanVoiceStore_i.hh>
#include <TucanUtilities.hh>
#include <TucanConfig.hh>
#include <errno.h>

TucanVoiceStore_i::TucanVoiceStore_i(CORBA::ORB_var orb)
{
	myName = "None";
	myOrb = orb;

	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

char* TucanVoiceStore_i::getName()
{
	return CORBA::string_dup(myName.c_str());
}

void TucanVoiceStore_i::setName(const char* name)
{
	myName = name;
}

TucanFolder_ptr TucanVoiceStore_i::getDefaultFolder()
{
	TucanFolder_ptr folder;
	
	folder = getNewFolderObject();
	folder->setName("inbox");
	
	return folder;
}

TucanFolder_ptr TucanVoiceStore_i::getFolder(const char* name)
{
	TucanFolder_ptr folder;

	folder = getNewFolderObject();
	folder->setName(name);

	return folder;
}

TucanFolder_ptr TucanVoiceStore_i::getNewFolderObject()
{
	TucanConfig* config = new TucanConfig();
	string filename = config->get("voice", "folder");
	delete config;
	string str;
	TucanFolder_ptr folder;

	ifstream infile(filename.c_str());
	getline(infile, str);
	infile.close();

	CORBA::Object_var object = myOrb->string_to_object(str.c_str());
	TucanFolderFactory_var fact = TucanFolderFactory::_narrow(object);
	folder = fact->create();

	folder->setAuth(myAuthObject);
	
	return folder;
}

TucanStringList* TucanVoiceStore_i::listFolders()
{
	DIR *dirfp;
	struct dirent *dent;
	TucanStringList* list = new TucanStringList();
	string tmp;
	int i = 0;
	CORBA::String_var user = myAuthObject->getUserName();

	tmp = gethomedir(user);
	tmp += "/vm/";
	
	dirfp = opendir(tmp.c_str());

	if (errno == ENOENT)
		throw TucanException("Error: Folder vm does not exist.");

	while ((dent = readdir(dirfp)))
	{
		if ((strcmp(".", dent->d_name)) && (strcmp("..", dent->d_name)))
		{
			DIR *tempdir;
			string tmp2(tmp);
			tmp2 += dent->d_name;
			tempdir = opendir(tmp2.c_str());
			if (errno != ENOTDIR)
			{
				list->length(i+1);
				(*list)[i] = strdup(dent->d_name);
				i++;
			}
			closedir(tempdir);
		}
	}
	closedir(dirfp);

	return list;
}

TucanAuth_ptr TucanVoiceStore_i::getAuth()
{
	return TucanAuth::_duplicate(myAuthObject);
}

void TucanVoiceStore_i::setAuth(TucanAuth_ptr myAuth)
{
	myAuthObject = TucanAuth::_duplicate(myAuth);
}

void TucanVoiceStore_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

// Factory

TucanVoiceStoreFactory_i::TucanVoiceStoreFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanStore_ptr TucanVoiceStoreFactory_i::create()
{
	TucanVoiceStore_i* myVoiceStore = new TucanVoiceStore_i(myOrb);
	PortableServer::ObjectId_var myVoiceStoreId = myPoa->activate_object(myVoiceStore);
	CORBA::Object_var obj = myPoa->servant_to_reference(myVoiceStore);
	myVoiceStore->_remove_ref();

	return TucanVoiceStore::_narrow(obj);
}
