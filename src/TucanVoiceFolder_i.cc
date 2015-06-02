//////////////////////////////////////////////////////////
// TucanVoiceFolder_i (c++ impl)  
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
// Filename:        TucanVoiceFolder_i.cc
// Description:     TucanVoiceFolder for CORBA implementation 
// Author:          Rob Flynn <rob@difinium.com> 
// Created at:      Tue Feb 16 17:02:37 PST 2000 
//////////////////////////////////////////////////////////

#include <TucanVoiceFolder_i.hh>
#include <TucanConfig.hh>
#include <TucanUtilities.hh>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <algorithm>

TucanVoiceFolder_i::TucanVoiceFolder_i(CORBA::ORB_var orb)
{
	myName = "None";
	myFullName = "None";
	myOrb = orb;
	myParent = NULL;

	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

string TucanVoiceFolder_i::findFileName(CORBA::ULong msgnum)
{
	DIR *dirfp;
	struct dirent *dent;
	CORBA::ULong count;

	if (msgnum < 1)
		throw TucanException("Error: Message index out of range.");

	// Now we need to read the directory
	dirfp = opendir(myFullName.c_str());

	count = 1;

	while ((dent = readdir(dirfp)))
	{
		if ((strcmp(".", dent->d_name)) && (strcmp("..", dent->d_name)))
		{
			if (count == msgnum)
				break;

			count++;
		}
	}

	closedir(dirfp);

	if (count < msgnum)
		throw TucanException("Error: Message index out of range.");
	else
	{
		string tmp(dent->d_name);
		return tmp;
	}
}

CORBA::Boolean TucanVoiceFolder_i::exists()
{
	return (access(myFullName.c_str(), 0) == 0);
}

CORBA::Boolean TucanVoiceFolder_i::create()
{
	// Attempt to create the directory
	int i = mkdir(myFullName.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);

	// Check the result
	if (i)
	{
		string tmp = "Erorr: Cannot create folder " + myName + " in create()";
		throw TucanException(tmp.c_str());
	}

	return 1;
}

CORBA::Boolean TucanVoiceFolder_i::deleteFolder()
{
	DIR *dirfp;
	struct dirent *dent;
	string tmp("");
	string filename;

	// Remember, we cant delete our default folder.  That would be bad.
	if (!strcmp("inbox", myName.c_str()))
		throw TucanException("Error: Default folder cannot be deleted.");

	CORBA::String_var user = myAuth->getUserName();
	
	tmp = gethomedir(user);
	tmp += "/vm/";
	tmp += myName;
	
	if (!exists())
	{
		tmp = "Error: Folder " + myName + " does not exist.";
		throw TucanException(tmp.c_str());
	}

	// Now we need to read the directory
	dirfp = opendir(tmp.c_str());

	while ((dent = readdir(dirfp)))
	{
		if ((strcmp(".", dent->d_name)) && (strcmp("..", dent->d_name)))
		{
			filename = tmp + "/" + dent->d_name;
			unlink(filename.c_str());
		}
	}

	closedir(dirfp);
	rmdir(tmp.c_str());

	return 1;
}

TucanMessageSeq* TucanVoiceFolder_i::expunge()
{
	throw TucanException("Error: Method expunge() has not been implemented.");
}

TucanFolder_ptr TucanVoiceFolder_i::getFolder(const char* name)
{
	TucanFolder_ptr folder;
	string tmp;

	tmp = myFullName;
	tmp += "/";
	tmp += name;

	folder = getNewFolderObject();
	folder->setName(name);

	return folder;
}

TucanFolder_ptr TucanVoiceFolder_i::getNewFolderObject()
{
	TucanConfig* config = new TucanConfig();
	string filename = config->get("voice", "folder");
	delete config;
	string str;
	TucanFolder_var folder;

	ifstream infile(filename.c_str());
	getline(infile, str);
	infile.close();

	CORBA::Object_var object = myOrb->string_to_object(str.c_str());
	TucanFolderFactory_var fact = TucanFolderFactory::_narrow(object);
	folder = fact->create();

	folder->setAuth(myAuth);

	CORBA::Object_var obj = myPoa->servant_to_reference(this);
	TucanFolder_var parent = TucanFolder::_narrow(obj);

	folder->setParent(parent);

	return folder._retn();
}

void TucanVoiceFolder_i::setParent(TucanFolder_ptr folder)
{
	myParent = TucanFolder::_duplicate(folder);
}

TucanFolder_ptr TucanVoiceFolder_i::getParent()
{
	return TucanFolder::_duplicate(myParent);
}

TucanStringList* TucanVoiceFolder_i::listFolders()
{
	throw TucanException("Error: Method listFolders() has not been implemented.");
}

TucanFolderSeq* TucanVoiceFolder_i::list()
{
	throw TucanException("Error: Method list() has not been implemented.");
}

CORBA::Boolean TucanVoiceFolder_i::renameTo(const char* name)
{
	int err;
	string olddir;
	string newdir;
	DIR *dirfp;

	if (!strcmp("inbox", myName.c_str()))
		throw TucanException("The default folder can not be deleted");

	CORBA::String_var user = myAuth->getUserName();

	olddir = gethomedir(user);
	olddir += "/vm/";

	newdir = olddir + name;
	olddir += myName;

	dirfp = opendir(olddir.c_str());
	err = errno;
	closedir(dirfp);

	if (err != 0)
		throw TucanException("Error renaming directory");

	dirfp = opendir(newdir.c_str());
	err = errno;
	closedir(dirfp);

	if (err == 0)
		throw TucanException("Error renaming directory");

	rename(olddir.c_str(), newdir.c_str());

	return 1;
}

char* TucanVoiceFolder_i::getName()
{
	return CORBA::string_dup(myName.c_str());
}

void TucanVoiceFolder_i::setName(const char* name)
{
	CORBA::String_var tmp;
	string newpath("");
	
	if (!CORBA::is_nil(myParent))
	{
		tmp = myParent->getFullName();
		newpath = tmp;
	}
		
	else
	{
		CORBA::String_var user = myAuth->getUserName();
		newpath = gethomedir(user);
		newpath += "/vm";
	}
		
	myName = name;
	newpath += "/";
	newpath += myName;

	setFullName(newpath.c_str());
}

char* TucanVoiceFolder_i::getFullName()
{
	return CORBA::string_dup(myFullName.c_str());
}

void TucanVoiceFolder_i::setFullName(const char* name)
{
	myFullName = name;
}

TucanMessage_ptr TucanVoiceFolder_i::getMessage(CORBA::Long msgnum)
{
	TucanMessage_ptr message;
	TucanRawData data;
	int fd;
	unsigned long num, i, j;
	DIR *dirfp;
	struct dirent *dent;
	long count = 0;
	string tmp("");
	char buffer[1024];

	count = getMessageCount();

	if (msgnum > count)
		throw TucanException("Error: Requested message out of range");

	CORBA::String_var user = myAuth->getUserName();
	
	tmp = gethomedir(user);
	tmp += "/vm/";
	tmp += myName;

	if (!exists())
	{
		tmp = "Error: Folder " + myName + " does not exist.";
		throw TucanException(tmp.c_str());
	}

	dirfp = opendir(tmp.c_str());

	count = 1;

	while ((dent = readdir(dirfp)))
	{
		if ((strcmp(".", dent->d_name)) && (strcmp("..", dent->d_name)))
		{
			if (count == msgnum)
				break;

			count++;
		}
	}

	closedir(dirfp);

	tmp += "/";
	tmp += dent->d_name;

	// Find just the date
	for (i = 3, j = 0; dent->d_name[i] != '.'; i++, j++)
		buffer[j] = dent->d_name[i];

	buffer[j] = 0;

	// Convert it to long
	long timer = atol(buffer);

	// And then into a lovely english string :)
	string sentdate = ctime(&timer);

	// Chomp off the trailing \n
	sentdate = strchomp(sentdate);

	// Open the new file
	fd = open(tmp.c_str(), O_RDONLY);

	// And read the data
	i = 0;
	do 
	{
		num = read(fd, buffer, 1024);

		data.length(data.length() + num);	
		
		for (j = 0; j < num; j++)
			data[i++] = buffer[j];
		
	} while (num > 0);

	// Close'er up
	close(fd);

		
	// Get the new message object
	message = getNewMessageObject();

	message->setMessageNumber(msgnum);
	message->setSubject("Voice Message");

	TucanAddress_var address;
	
	address = message->getNewAddressObject();
	address->setAddress("Voice Mail System");
	message->setFrom(address);

	address = message->getNewAddressObject();
	address->setAddress(user);
	message->setRecipient("to", address);
	message->setSentDate(sentdate.c_str());
	message->setReceivedDate(sentdate.c_str());

	TucanData_var dob = message->getNewDataObject();
	dob->setContentType("audio/x-wav");
	dob->setData(data);
	message->setDataObject(dob);
	

	return message;	
}

TucanMessageHeaderSeq* TucanVoiceFolder_i::getMessageHeaders()
{
	TucanMessageHeader item;
	TucanMessageHeaderSeq *list = new TucanMessageHeaderSeq;
	unsigned int i = 0;
	unsigned int j;
	unsigned int count;
	struct stat *stats = (struct stat *)malloc(sizeof(struct stat));
	char buffer[512];

	CORBA::String_var tmp = myAuth->getUserName();
	
	count = getMessageCount();
	
	string dirstub = gethomedir(tmp);

	dirstub += "/vm/";
	dirstub += myName;
	dirstub += "/";

	for (i = 0; i < count; i++)
	{
		string filename = dirstub + findFileName(i + 1);

		item.fromaddr = CORBA::string_dup("Voice Mail System");
		item.subject = CORBA::string_dup("Voice Message");

		// Find just the date
		for (i = 3, j = 0; filename.c_str()[i] != '.'; i++, j++)
			buffer[j] = filename.c_str()[i];

		buffer[j] = 0;

		// Convert it to long
		item.date = atol(buffer);
		
		item.attach = 0;


		vector<string> temp = strsplit(filename, ',');
	
		if (temp.size() > 1)
		{
			try 
			{
				item.flags = atol(temp[temp.size()-1].c_str());
			} catch (...)
			{
				item.flags = 0;
			}
		}
		else
		{
			item.flags = 0;
		}

		temp.clear();

		stat(filename.c_str(), stats);
		item.size = stats->st_size;

		list->length(list->length() + 1);
		(*list)[i] = item;
	}

	free(stats);

	return list;
}

TucanMessageSeq* TucanVoiceFolder_i::getMessages()
{
	long count;
	long i;
	
	count = getMessageCount();
	TucanMessageSeq* list = new TucanMessageSeq;
	TucanMessage_var message;
	
	list->length(count);
	for (i = 0; i < count; i++)
	{
		message = getMessage(i+1);
		(*list)[i] = TucanMessage::_duplicate(message);
	}

	return list;
}

TucanMessageSeq* TucanVoiceFolder_i::getNewMessages()
{
	throw TucanException("Error: Method getNewMessages() has not been implemented.");
}

TucanMessageSeq* TucanVoiceFolder_i::getMessagesInRange(CORBA::Long start, CORBA::Long stop)
{
	long count;
	long i;
	
	count = getMessageCount();
	TucanMessageSeq* list = new TucanMessageSeq;
	TucanMessage_var message;
	
	list->length(count);
	for (i = start; i <= stop; i++)
	{
		message = getMessage(i);
		(*list)[i] = TucanMessage::_duplicate(message);
	}

	return list;
}

TucanMessage_ptr TucanVoiceFolder_i::getNewMessageObject()
{
	TucanConfig* config = new TucanConfig();
	string filename = config->get("voice", "message");
	delete config;
	string str;
	TucanMessage_var message;

	ifstream infile(filename.c_str());
	getline(infile, str);
	infile.close();

	CORBA::Object_var object = myOrb->string_to_object(str.c_str());
	TucanMessageFactory_var fact = TucanMessageFactory::_narrow(object);
	message = fact->create();

	return message._retn();
}

CORBA::Long TucanVoiceFolder_i::getMessageCount()
{
	DIR *dirfp;
	struct dirent *dent;
	long count = 0;
	string tmp("");
	CORBA::String_var user = myAuth->getUserName();

	
	tmp = gethomedir(user);
	tmp += "/vm/";
	tmp += myName;

	if (!exists())
	{
		tmp = (string)"Error: Folder " + myName + (string)" does not exist.";
		throw TucanException(tmp.c_str());
	}

	dirfp = opendir(tmp.c_str());
	if (!dirfp)
	{
		tmp = (string)"Error: Can't open Folder " + myName + 
			(string)": " + (string)strerror(errno);
		throw TucanException(tmp.c_str());
	}

	while ((dent = readdir(dirfp)))
	{
		if ((strcmp(".", dent->d_name)) && (strcmp("..", dent->d_name)))
			count++;
	}

	closedir(dirfp);

	return count;
}

CORBA::Long TucanVoiceFolder_i::getNewMessageCount()
{
	CORBA::Long count = 0;
	CORBA::Long flags = 0;

	// Loop once for each message
	for (int i = 0; i < getMessageCount(); i++)
	{
		// Get our flags
		flags = getMessageFlags(i+1);

		// If it's not read, lets increase our counter
		if (!(flags & MESSAGE_READ))
			count++;
	}

	// And return the value
	return count;
}

CORBA::Boolean TucanVoiceFolder_i::hasNewMessages()
{
	if (getNewMessageCount() > 0)
		return 1;
	else
		return 0;
}

void TucanVoiceFolder_i::moveMessage(CORBA::Long msgnum, TucanFolder_ptr folder)
{
	copyMessage(msgnum, folder);
	deleteMessage(msgnum);
}

void TucanVoiceFolder_i::moveMessages(const TucanLongList& msgnums, TucanFolder_ptr folder)
{
	vector<CORBA::ULong> nums;

	// First get a copy of all of the numbers
	for (CORBA::ULong i = 0; i < msgnums.length(); i++)
		nums.push_back(msgnums[i]);

	// And now sort them
	sort(nums.begin(), nums.end());

	// Copy the messages
	copyMessages(msgnums, folder);

	// Now, we should delete each message, in reverse order so that we don't
	// have to keep track of weird numbering
	for (unsigned int i = nums.size(); i > 0; i--)
		deleteMessage(nums[i-1]);
}

void TucanVoiceFolder_i::copyMessage(CORBA::Long msgnum, TucanFolder_ptr folder)
{
	CORBA::String_var temp;
	char buffer[1024];
	int fd, nfd, num;

	// Verify the existence of our two folders
	if (!exists())
	{
		string tmp = "Error: Folder " + myName + " does not exist.";
		throw TucanException(tmp.c_str());
	}

	if (!folder->exists())
	{
		temp = folder->getName();
		string tmp = "Error: Folder ";
		tmp += temp;
	        tmp + " does not exist.";
		throw TucanException(tmp.c_str());
	}

	// And then get the message file name
	string msgfilename(findFileName(msgnum));

	// Build our two path strings
	string curfolder = myFullName;
	curfolder += "/";
	curfolder += msgfilename;

	temp = folder->getFullName();
	string newfolder(temp);
	newfolder += "/";
	newfolder += msgfilename;

	// Open the original file
	fd = open(curfolder.c_str(), O_RDONLY);
				
	// Open the new file for writing, creating it if necessary
	nfd = open(newfolder.c_str(), O_WRONLY | O_CREAT, 
			S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

	// And read the data
	do 
	{
		num = read(fd, buffer, 1024);
		write(nfd, buffer, num);

	} while (num > 0);

	// Close'er up
	close(fd);
	close(nfd);
}

void TucanVoiceFolder_i::copyMessages(const TucanLongList& msgnums, TucanFolder_ptr folder)
{
	for (unsigned int i = 0; i < msgnums.length(); i++)
		copyMessage(msgnums[i], folder);
}

void TucanVoiceFolder_i::appendMessage(TucanMessage_ptr msg)
{
	TucanRawData *data;
	string filename;
	int fd;
	char buf[64];
	CORBA::String_var type;
	TucanData_var dob;
	
	snprintf(buf, 64, "msg%ld.%ld.wav", time((time_t)NULL), ((long)random() % 10000));

	dob = msg->getDataObject();
	type = dob->getContentType();

	if (strncasecmp(type, "audio/", 6))
		return;
	
	// Get our message's raw data
	data = msg->getData();

	filename = myFullName;
	filename += "/";
	filename += buf;		

	// Open the file for writing, creating it if necessary
	fd = open(filename.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

	char *tmp;

	tmp = (char *)malloc(data->length() * sizeof(char));

	for (unsigned long i = 0; i < data->length(); i++)
		tmp[i] = (*data)[i];
	
	write(fd, tmp, data->length());

	// And close up shop :)
	close(fd);
}

void TucanVoiceFolder_i::appendMessages(const TucanMessageSeq& msgs)
{
	for (unsigned int i = 0; i < msgs.length(); i++)
		appendMessage(msgs[i]);
}

void TucanVoiceFolder_i::deleteMessage(CORBA::Long msgnum)
{
	string tmp("");
	CORBA::String_var user = myAuth->getUserName();

	// Get our directory
	tmp = gethomedir(user);
	tmp += "/vm/";
	tmp += myName;

	if (!exists())
	{
		tmp = "Error: Folder " + myName + " does not exist.";
		throw TucanException(tmp.c_str());
	}

	if (msgnum > getMessageCount())
		throw TucanException("Error: Requested message out of range");
	
	string msgfilename = findFileName(msgnum);
	
	string filename = tmp + "/";
	filename += msgfilename;

	unlink(filename.c_str());
}

CORBA::Long TucanVoiceFolder_i::getMessageFlags(CORBA::Long msgnum)
{
	string tmp("");
	CORBA::String_var user = myAuth->getUserName();

	tmp = myFullName;

	if (!exists())
	{
		tmp = "Error: Folder " + myName + " does not exist.";
		throw TucanException(tmp.c_str());
	}

	if (msgnum > getMessageCount())
		throw TucanException("Error: Requested message out of range");
	
	string msgfilename = findFileName(msgnum);

	string filename = myFullName + "/";
	filename += msgfilename;

	// Now that we have the filename, we should rip off the ending stuff
	vector<string> pieces = strsplit(filename, ',');

	CORBA::Long flags = 0;
	
	if (pieces.size() > 1)
	{
		try 
		{
			flags = atol(pieces[pieces.size()-1].c_str());
		} catch (...)
		{
			// Just incase something funny happens
		}
	}

	pieces.clear();

	return flags;
}

void TucanVoiceFolder_i::setMessageFlags(CORBA::Long msgnum, CORBA::Long flags)
{
	string tmp("");
	CORBA::String_var user = myAuth->getUserName();

	tmp = myFullName;

	if (!exists())
	{
		tmp = "Error: Folder " + myName + " does not exist.";
		throw TucanException(tmp.c_str());
	}

	if (msgnum > getMessageCount())
		throw TucanException("Error: Requested message out of range");
	
	string msgfilename = findFileName(msgnum);

	string filename = myFullName + "/";
	filename += msgfilename;

	// Now that we have the filename, we should rip off the ending stuff
	vector<string> pieces = strsplit(filename, ',');

	if (pieces.size() > 1)
	{
		try 
		{
			char buf[16];
			snprintf(buf, 16, "%ld", flags);
			pieces[pieces.size() - 1] = buf;
		} catch (...)
		{
			// Just incase something funny happens
		}
	}
	else
	{
		char buf[16];
		snprintf(buf, 16, "%ld", flags);
		pieces.push_back(buf);
	}

	string newfilename = strjoin(pieces, ',');

	rename(filename.c_str(), newfilename.c_str());

	pieces.clear();
}

void TucanVoiceFolder_i::setMessageFlag(CORBA::Long msgnum, CORBA::Long flag)
{
	// There's no need to duplicate lots of code.  Mmmm, code reuse.

	CORBA::Long flags = getMessageFlags(msgnum);
	flags |= flag;
	setMessageFlags(msgnum, flags);
}

void TucanVoiceFolder_i::unsetMessageFlag(CORBA::Long msgnum, CORBA::Long flag)
{
	// There's no need to duplicate lots of code.  Mmmm, code reuse.

	CORBA::Long flags = getMessageFlags(msgnum);
	
	if (flags & flag)
	{
		flags ^= flag;
		setMessageFlags(msgnum, flags);
	}
}

TucanAuth_ptr TucanVoiceFolder_i::getAuth()
{
	return TucanAuth::_duplicate(myAuth);
}

void TucanVoiceFolder_i::setAuth(TucanAuth_ptr auth)
{
	myAuth = TucanAuth::_duplicate(auth);
}

void TucanVoiceFolder_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

// Factory

TucanVoiceFolderFactory_i::TucanVoiceFolderFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanFolder_ptr TucanVoiceFolderFactory_i::create()
{
	TucanVoiceFolder_i* myFolder = new TucanVoiceFolder_i(myOrb);
	PortableServer::ObjectId_var myId = myPoa->activate_object(myFolder);
	CORBA::Object_var obj = myPoa->servant_to_reference(myFolder);
	myFolder->_remove_ref();

	return TucanVoiceFolder::_narrow(obj);
	
}

