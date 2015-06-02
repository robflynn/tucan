//////////////////////////////////////////////////////////
// TucanEmailStore_i (c++ impl)  
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
// Filename:        TucanEmailStore_i.cc
// Description:     TucanEmailStore for CORBA implementation 
// Author:          Rob Flynn <rob@difinium.com> 
// Created at:      Fri Jan 19 10:50:33 PST 2001
//////////////////////////////////////////////////////////

#include <TucanEmailStore_i.hh>
#include <TucanUtilities.hh>
#include <TucanConfig.hh>
#include <errno.h>

TucanEmailStore_i::TucanEmailStore_i(CORBA::ORB_var orb)
{
	myName = "None";
	myOrb = orb;

	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

char* TucanEmailStore_i::getName()
{
	return CORBA::string_dup(myName.c_str());
}

void TucanEmailStore_i::setName(const char* name)
{
	myName = name;
}

TucanFolder_ptr TucanEmailStore_i::getDefaultFolder()
{
	TucanFolder_ptr folder;

	folder = getNewFolderObject();
	folder->setName("inbox");

	return folder;
}

TucanFolder_ptr TucanEmailStore_i::getFolder(const char* name)
{
	TucanFolder_ptr folder;

	folder = getNewFolderObject();

	if (strlen(name) == 0)
		folder->setName("inbox");
	else
		folder->setName(name);

	return folder;
}

TucanFolder_ptr TucanEmailStore_i::getNewFolderObject()
{
	TucanConfig* config = new TucanConfig();
	string filename = config->get("email", "folder");
	delete config;
	string str;
	TucanFolder_ptr folder;

	ifstream infile(filename.c_str());
	getline(infile, str);
	infile.close();

	CORBA::Object_var object = myOrb->string_to_object(str.c_str());
	TucanFolderFactory_var fact = TucanFolderFactory::_narrow(object);
	folder = fact->create();

	folder->setAuth(TucanAuth::_duplicate(myAuthObject));
	
	return folder;
}


TucanStringList* TucanEmailStore_i::listFolders()
{
	DIR *dirfp;
	struct dirent *dent;
	TucanStringList* list = new TucanStringList();
	string tmp;
	int i = 0;
	CORBA::String_var user = myAuthObject->getUserName();

	tmp = gethomedir(user);
	tmp += "/tucan_email/";
	
	dirfp = opendir(tmp.c_str());

	if (errno == ENOENT)
		throw TucanException("Error: Folder tucan_email does not exist.");

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

TucanAuth_ptr TucanEmailStore_i::getAuth()
{
	return TucanAuth::_duplicate(myAuthObject);
}

void TucanEmailStore_i::setAuth(TucanAuth_ptr myAuth)
{
	myAuthObject = TucanAuth::_duplicate(myAuth);
}

void TucanEmailStore_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

// Factory

TucanEmailStoreFactory_i::TucanEmailStoreFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanStore_ptr TucanEmailStoreFactory_i::create()
{
	TucanEmailStore_i* myEmailStore = new TucanEmailStore_i(myOrb);
	PortableServer::ObjectId_var myEmailStoreId = myPoa->activate_object(myEmailStore);
	CORBA::Object_var obj = myPoa->servant_to_reference(myEmailStore);
	myEmailStore->_remove_ref();

	return TucanEmailStore::_narrow(obj);
}
