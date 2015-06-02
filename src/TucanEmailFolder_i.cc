//////////////////////////////////////////////////////////
// TucanEmailFolder_i (c++ impl)  
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
// Filename:        TucanEmailFolder_i.cc
// Description:     TucanEmailFolder for CORBA implementation 
// Author:          Rob Flynn <rob@difinium.com> 
// Created at:      Tue Jan 23 13:44:29 PST 2000 
//////////////////////////////////////////////////////////

#include <TucanEmailFolder_i.hh>
#include <TucanUtilities.hh>
#include <TucanConfig.hh>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <TucanParse.hh>
#include <algorithm>
#include <stdlib.h>
#include <sys/stat.h>

TucanEmailFolder_i::TucanEmailFolder_i(CORBA::ORB_var orb)
{
	myName = "None";
	myFullName = "None";
	myOrb = orb;

	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

string TucanEmailFolder_i::findFileName(CORBA::ULong msgnum)
{
	DIR *dirfp;
	struct dirent *dent;
	CORBA::ULong count;
	vector<string> names; 

	if (msgnum < 1 || msgnum > (CORBA::ULong)getMessageCount())
		throw TucanException("Error: Message index out of range.");

	// Now we need to read the directory
	dirfp = opendir(myFullName.c_str());

	count = 1;

	while ((dent = readdir(dirfp)))
	{
		if ((strcmp(".", dent->d_name)) && (strcmp("..", dent->d_name)))
		{
			string tmp(dent->d_name);
			names.push_back(tmp);
		}
	}

	closedir(dirfp);

	sort(names.begin(), names.end());

	return names[msgnum-1];
}

CORBA::Boolean TucanEmailFolder_i::exists()
{
	return (access(myFullName.c_str(), 0) == 0);
}

CORBA::Boolean TucanEmailFolder_i::create()
{
	int i = mkdir(myFullName.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);

	if (i)
	{
		string tmp = "Error: Cannot create folder " + myName + " in create()";
		throw TucanException(tmp.c_str());
	}
	
	return 1;
}

CORBA::Boolean TucanEmailFolder_i::deleteFolder()
{
	DIR *dirfp;
	struct dirent *dent;
	string tmp("");
	string filename;

	// Remember, we cant delete our default folder.  That would be bad.
	if (!strcmp("inbox", getName()))
		throw TucanException("Error: Default folder cannot be deleted.");

	CORBA::String_var user = myAuth->getUserName();
	
	// Now we need to read the directory
	dirfp = opendir(myFullName.c_str());

	if (errno != 0)
	{
		tmp = "Error: Folder " + myName + " does not exist.";
		throw TucanException(tmp.c_str());
	}

	while ((dent = readdir(dirfp)))
	{
		if ((strcmp(".", dent->d_name)) && (strcmp("..", dent->d_name)))
		{
			filename = myFullName + "/" + dent->d_name;
			unlink(filename.c_str());
		}
	}

	closedir(dirfp);

	rmdir(myFullName.c_str());

	return 1;
}

TucanMessageSeq* TucanEmailFolder_i::expunge()
{
	throw TucanException("Error: Method expunge() has not been implemented.");
}

TucanFolder_ptr TucanEmailFolder_i::getFolder(const char* name)
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

TucanFolder_ptr TucanEmailFolder_i::getNewFolderObject()
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
	folder->setAuth(TucanAuth::_duplicate(myAuth));

	CORBA::Object_var obj = myPoa->servant_to_reference(this);
	TucanFolder_var parent = TucanFolder::_narrow(obj);

	folder->setParent(parent);

	return folder;
}

void TucanEmailFolder_i::setParent(TucanFolder_ptr folder)
{
	myParent = TucanFolder::_duplicate(folder);
}

TucanFolder_ptr TucanEmailFolder_i::getParent()
{
	return TucanFolder::_duplicate(myParent);
}

TucanStringList* TucanEmailFolder_i::listFolders()
{
	throw TucanException("Error: Method listFolders() has not been implemented.");
}

TucanFolderSeq* TucanEmailFolder_i::list()
{
	throw TucanException("Error: Method list() has not been implemented.");
}

CORBA::Boolean TucanEmailFolder_i::renameTo(const char* name)
{
	int err;
	string olddir;
	string newdir;
	DIR *dirfp;

	if (!strcmp("inbox", getName()))
		throw TucanException("The default folder can not be deleted");

	CORBA::String_var user = myAuth->getUserName();

	olddir = gethomedir(user);
	olddir += "/tucan_email/";

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

char* TucanEmailFolder_i::getName()
{
	return CORBA::string_dup(myName.c_str());
}

void TucanEmailFolder_i::setName(const char* name)
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
		newpath += "/tucan_email";
	}
		
	myName = name;
	newpath += "/";
	newpath += myName;

	setFullName(newpath.c_str());
}

char* TucanEmailFolder_i::getFullName()
{
	return CORBA::string_dup(myFullName.c_str());
}

void TucanEmailFolder_i::setFullName(const char* name)
{
	myFullName = name;
}

TucanMessage_ptr TucanEmailFolder_i::getMessage(CORBA::Long msgnum)
{
	TucanMessage_ptr message;
	long count;
	string data("");
	string str;
	string tmp;
	vector<string> addresses;
	unsigned long i,j;
	char *types[3] = {"to","cc","bcc"};

	count = getMessageCount();
	if (msgnum > count)
		throw TucanException("Error: Requested message out of range");

	CORBA::String_var user = myAuth->getUserName();
	
	if (!exists())
	{
		tmp = "Error: Folder " + myName + " does not exist.";
		throw TucanException(tmp.c_str());
	}

	string msgfilename = findFileName(msgnum);

	tmp = myFullName;
	tmp += "/";
	tmp += msgfilename;

	ifstream infile(tmp.c_str());
	
	if (infile.bad())
	{
		// If we get here then there was a permission problem with opening
		// the message.

		tmp = "Error: Permissions problem reading message from " + myName + ".";
		throw TucanException(tmp.c_str());
	}
	
	i = 0;
	
	while (!infile.eof())
	{
		getline(infile, str);
		data += str + "\n";
	}

	i = 0;

	infile.close();

	TucanParse *myParser = new TucanParse();
	myHash header;

	header = myParser->parseHeader(myParser->getHeader(data));

	message = getNewMessageObject();
	message->setMessageNumber(msgnum);

	message->setSubject(header["subject"].c_str());
	message->setSentDate(header["date"].c_str());
	message->setReceivedDate(header["date"].c_str());

	// Parse the recipients
	for (i = 0; i < sizeof(types)/sizeof(char *); i++)
	{
		addresses = strsplit(header[types[i]], ',');
		
		for (j = 0; j < addresses.size(); j++)
		{
			TucanAddress_var address = message->getNewAddressObject();
			address->setAddress(addresses[j].c_str());
			message->addRecipient(types[i], address);
		}
		addresses.clear();
	}

	TucanAddress_var address = message->getNewAddressObject();
	address->setAddress(header["from"].c_str());
	message->setFrom(address);
	
	TucanData_var myDataObj = message->getNewDataObject();
	string body = myParser->getBody(data);
	vector<string> parts;
	
	myHash item;

	item = myParser->parseItem("content-type", header["content-type"]);

	TucanStringList headerlist;
	string ctype = item["content-type"];

	parts = myParser->parseBody(body, item["boundary"]);

	if ((parts.size() > 0) && (!strncasecmp(item["content-type"].c_str(), "multipart/", 10)))
	{
		// If you only have one part in your multi part message
		// then we should just set it to the body (yes, I've seen
		// this happen.  Weird.)

		string subbody = myParser->getBody(parts[0]);
		myHash subheader = myParser->parseHeader(myParser->getHeader(parts[0]));

		item.clear();
		item = myParser->parseItem("content-transfer-encoding", subheader["content-transfer-encoding"]);
		string encoding = item["content-transfer-encoding"];
		
		item.clear();
		item = myParser->parseItem("content-type", subheader["content-type"]);

		// Let's handle our transfer types properly!
		if (!strcasecmp(encoding.c_str(), "quoted-printable"))
			myDataObj->setText(qpDecode(subbody).c_str());
		else
			myDataObj->setText(subbody.c_str());
		
		myDataObj->setContentType(item["content-type"].c_str());

		item.clear();
		subheader.clear();

		if (parts.size() >= 1)
		{
			for (unsigned long k = 1; k < parts.size(); k++)
			{
				string tmpheader = myParser->getHeader(parts[k]);
				subheader = myParser->parseHeader(tmpheader);
				subbody = myParser->getBody(parts[k]);
				item.clear();

				myHash::iterator keyiterator = subheader.begin();

				headerlist.length(subheader.size());

				for (unsigned int ii = 0; ii < subheader.size(); ii++)
				{
					string key = (*keyiterator).first;
					string value = subheader[key];
					string temp("");

					// Parse the item so that we can decode it
					item = myParser->parseItem(key, value);

					// Get our item key iterator
					myHash::iterator ikeyiterator = item.begin();

					// For each entry of our parsed items
					for (unsigned int jj = 0; jj < item.size(); jj++)
					{
						string ikey = (*ikeyiterator).first;
						string ivalue = item[ikey];
						
						// If its not the first entry, add a semi colon
						if (jj > 0)
							temp += "; ";
	
						// For the first entry use a colon
						if (jj == 0)
						{
							temp = ikey;
							temp += ": ";
							temp += ivalue;
						}
						// And everything else uses an equal mark
						else
						{
							temp += ikey;
							temp += "=\"";
							temp += ivalue;	
							temp += "\"";
						}
	
						ikeyiterator++;
					}
						
					item.clear();

					// Set our final value
					headerlist[ii] = CORBA::string_dup(temp.c_str());
					keyiterator++;
				}
			
				item = myParser->parseItem("content-transfer-encoding", subheader["content-transfer-encoding"]);
				encoding = item["content-transfer-encoding"];

				item.clear();

				item = myParser->parseItem("content-type", subheader["content-type"]);
		
				TucanRawData part;

				// Let's handle our transfer types properly!
				if (!strcasecmp(encoding.c_str(), "quoted-printable"))
				{
					string tmpdata = qpDecode(subbody);

					part.length(tmpdata.length());
					for (unsigned long m = 0; m < tmpdata.length(); m++)
						part[m] = tmpdata[m];
				
					if (parts.size() > 1)
						myDataObj->addPart(item["content-type"].c_str(), headerlist, part);
					else
					{
						myDataObj->setContentType(item["content-type"].c_str());
						myDataObj->setText(tmpdata.c_str());
						myDataObj->setData(part);
					}
				}
				else if (!strcasecmp(encoding.c_str(), "uuencode"))
				{
					char *rd;
				       
					long len = uueDecode(&rd, subbody);

					part.length(len);
					for (long m = 0; m < len; m++)
						part[m] = rd[m];

					if (parts.size() > 1)
						myDataObj->addPart(item["content-type"].c_str(), headerlist, part);
					else
					{
						myDataObj->setContentType(item["content-type"].c_str());
						myDataObj->setText(rd);
						myDataObj->setData(part);
					}

					free(rd);
				}
				else if (!strcasecmp(encoding.c_str(), "base64"))
				{
					char *rd;
				       
					long len = decodeBase64(&rd, subbody);
					

					part.length(len);
					for (long m = 0; m < len; m++)
						part[m] = rd[m];

					if (parts.size() > 1)
						myDataObj->addPart(item["content-type"].c_str(), headerlist, part);
					else
					{
						myDataObj->setContentType(item["content-type"].c_str());
						myDataObj->setText(rd);
						myDataObj->setData(part);
					}

					free(rd);
				}
				else
				{
					string tmpdata = subbody;
					part.length(tmpdata.length());
					for (unsigned long m = 0; m < tmpdata.length(); m++)
						part[m] = tmpdata[m];

					if (parts.size() > 1)
						myDataObj->addPart(item["content-type"].c_str(), headerlist, part);
					else
					{
						myDataObj->setContentType(item["content-type"].c_str());
						myDataObj->setText(tmpdata.c_str());
						myDataObj->setData(part);
					}

				}
	
				subheader.clear();
			}

			parts.clear();
			
			// Now that all of the above is clear, let's set the multipart 
			// content type correctly
			
			if (parts.size() > 1)
				myDataObj->setContentType(ctype.c_str());
		}
		
	}
	else
	{
	
		item.clear();

		item = myParser->parseItem("content-transfer-encoding", header["content-transfer-encoding"]);
		string encoding = item["content-transfer-encoding"];
		
		// Let's handle our transfer types properly!
		if (!strcasecmp(encoding.c_str(), "quoted-printable"))
			myDataObj->setText(qpDecode(body).c_str());

		else if (!strcasecmp(encoding.c_str(), "uuencode"))
		{
			char *rd;
			   
			uueDecode(&rd, body);
			myDataObj->setText(rd);	
			free(rd);

		}
		else if (!strcasecmp(encoding.c_str(), "base64"))
		{
			char *rd;
			   
			decodeBase64(&rd, body);
			myDataObj->setText(rd);	
			free(rd);
		}
		else
			myDataObj->setText(body.c_str());

		if (ctype.length() > 0)
			myDataObj->setContentType(ctype.c_str());
		else
			myDataObj->setContentType("text/plain");
	}


	myDataObj->setXML(data.c_str());
	message->setDataObject(myDataObj);

	header.clear();
	item.clear();

	delete myParser;

	return message;
}

TucanMessageSeq* TucanEmailFolder_i::getMessages()
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

TucanMessageHeaderSeq* TucanEmailFolder_i::getMessageHeaders()
{
	TucanMessageHeader item;
	TucanMessageHeaderSeq *list = new TucanMessageHeaderSeq;
	unsigned int  i = 0;
	TucanParse *myParser = new TucanParse();
	myHash header;
	myHash ctype;
	unsigned int count;
	struct stat *stats = (struct stat *)malloc(sizeof(struct stat));
	
	if (!exists())
	{
		string tmp;
		tmp = "Error: Folder " + myName + " does not exist.";
		throw TucanException(tmp.c_str());
	}
	
	CORBA::String_var tmp = myAuth->getUserName();
	
	count = getMessageCount();
	
	string dirstub = gethomedir(tmp);
	dirstub += "/tucan_email/";
	dirstub += myName;
	dirstub += "/";

	for (i = 0; i < count; i++)
	{
		string filename = dirstub + findFileName(i + 1);

		ifstream infile(filename.c_str());

		string data("");
		string str;

		while (!infile.eof())
		{
			getline(infile, str);
			data += str + "\n";
		}

		infile.close();

		header = myParser->parseHeader(myParser->getHeader(data));
	
		item.fromaddr = CORBA::string_dup(header["from"].c_str());
		item.subject = CORBA::string_dup(header["subject"].c_str());

		if (header["date"].length() > 1)
		{
			struct tm *t_time = parsedate(header["date"].c_str());

			item.date = mktime(t_time);

			free(t_time);
		}
		else
		{
			item.date = 0;
		}
		
		ctype= myParser->parseItem("content-type", header["content-type"]);
		if (!ctype["content-type"].compare("multipart/mixed"))
			item.attach = 1;
		else
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

	header.clear();
	ctype.clear();
	delete myParser;

	return list;
}

TucanMessageSeq* TucanEmailFolder_i::getNewMessages()
{
	TucanMessageSeq* list = new TucanMessageSeq;
	TucanMessage_var message;
	DIR *dirfp;
	struct dirent *dent;
	long count = 0;
	string tmp("");
	long i = 0;
	CORBA::String_var user = myAuth->getUserName();

	tmp = gethomedir(user);
	tmp += "/tucan_email/";
	tmp += myName;

	if (!exists())
	{
		tmp = "Error: Folder " + myName + " does not exist.";
		throw TucanException(tmp.c_str());
	}
	
	dirfp = opendir(tmp.c_str());

	while ((dent = readdir(dirfp)))
	{
		if ((strcmp(".", dent->d_name)) && (strcmp("..", dent->d_name)))
		{
			// Add a length check so we won't segfault if we find something bad
			if (strlen(dent->d_name) > 2) 
			{
				// If the last two values are ,R then we have a read message
				if (strncasecmp(",R", (dent->d_name + (strlen(dent->d_name) - 2)), 2))
					list->length(list->length()+1);
					message = getMessage(count);
					(*list)[i] = TucanMessage::_duplicate(message);
					i++;
			}
			count++;
		}
	}

	closedir(dirfp);

	return list;
}

TucanMessageSeq* TucanEmailFolder_i::getMessagesInRange(CORBA::Long start, CORBA::Long stop)
{
	long count;
	long i;
	
	count = getMessageCount();
	TucanMessageSeq* list = new TucanMessageSeq;
	TucanMessage_var message;

	if (!exists())
	{
		string tmp;

		tmp = "Error: Folder " + myName + " does not exist.";
		throw TucanException(tmp.c_str());
	}

	list->length(count);
	for (i = start; i <= stop; i++)
	{
		message = getMessage(i);
		(*list)[i] = TucanMessage::_duplicate(message);
	}

	return list;
}

TucanMessage_ptr TucanEmailFolder_i::getNewMessageObject()
{
	TucanConfig* config = new TucanConfig();
	string filename = config->get("email", "message");
	delete config;
	string str;
	TucanMessage_ptr message;

	ifstream infile(filename.c_str());
	getline(infile, str);
	infile.close();

	CORBA::Object_var object = myOrb->string_to_object(str.c_str());
	TucanMessageFactory_var fact = TucanMessageFactory::_narrow(object);
	message = fact->create();

	return message;
}

CORBA::Long TucanEmailFolder_i::getMessageCount()
{
	DIR *dirfp;
	struct dirent *dent;
	long count = 0;
	string tmp("");
	CORBA::String_var user = myAuth->getUserName();

	tmp = gethomedir(user);
	tmp += "/tucan_email/";
	tmp += myName;

	if (!exists())
	{
		tmp = "Error: Folder " + myName + " does not exist.";
		throw TucanException(tmp.c_str());
	}

	dirfp = opendir(tmp.c_str());
	
	while ((dent = readdir(dirfp)))
	{
		if ((strcmp(".", dent->d_name)) && (strcmp("..", dent->d_name)))
			count++;
	}

	closedir(dirfp);

	return count;
}

CORBA::Long TucanEmailFolder_i::getNewMessageCount()
{
	CORBA::Long count = 0;
	CORBA::Long flags = 0;

	if (!exists())
	{
		string tmp;
		tmp = "Error: Folder " + myName + " does not exist.";
		throw TucanException(tmp.c_str());
	}
	
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

CORBA::Boolean TucanEmailFolder_i::hasNewMessages()
{
	if (getNewMessageCount() > 0)
		return 1;
	else
		return 0;
}

void TucanEmailFolder_i::moveMessage(CORBA::Long msgnum, TucanFolder_ptr folder)
{
	copyMessage(msgnum, folder);
	deleteMessage(msgnum);
}

void TucanEmailFolder_i::moveMessages(const TucanLongList& msgnums, TucanFolder_ptr folder)
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
	{
		deleteMessage(nums[i-1]);
	}
}

void TucanEmailFolder_i::copyMessage(CORBA::Long msgnum, TucanFolder_ptr folder)
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

void TucanEmailFolder_i::copyMessages(const TucanLongList& msgnums, TucanFolder_ptr folder)
{
	for (unsigned int i = 0; i < msgnums.length(); i++)
	{
		copyMessage(msgnums[i], folder);
	}
}

void TucanEmailFolder_i::appendMessage(TucanMessage_ptr msg)
{
	CORBA::String_var temp;
	CORBA::String_var type;
	CORBA::String_var tmp;
	string data("");
	char *rectypes[2] = {"to", "cc"};
	TucanAddressSeq_var addresses;
	TucanAddress_var addr;
	string msgfilename;

	if (!exists())
	{
		string tmperr;
		tmperr = "Error: Folder " + myName + " does not exist.";
		throw TucanException(tmperr.c_str());
	}

	msgfilename = myFullName;
	msgfilename += "/";

	// Find out who it's from
	addr = msg->getFrom();
	if (!CORBA::is_nil(addr))
	{
		temp = addr->getAddress();
		data += "From: ";
		data += temp;
		data += "\n";
	}

	// Get the recipients
	for (unsigned int i = 0; i < sizeof(rectypes)/(sizeof(char *)); i++)
	{
		addresses = msg->getRecipients(rectypes[i]);
		unsigned int len = addresses->length();

		// If there are any for this type, append the header
		if (len)
		{
			data += rectypes[i];
			data += ": ";
		}
	
		// And fetch each one
		for (unsigned int j = 0; j < len; j++)
		{
			temp = addresses[j]->getAddress();
			
			data += temp;

			// Leaving off the comma on the last one
			if (j < len-1)
				data += ", ";	
		}

		if (len)
			data += "\n";
	}

	// Grab the subject
	temp = msg->getSubject();
	if (strlen(temp) > 0)
	{
		data += "Subject: ";
		data += temp;
		data += "\n";
	}

	// And the sent date
	temp = msg->getSentDate("");
	if (strlen(temp) > 0)
	{
		data += "Date: ";
		data += temp;
		data += "\n";
	}
	else
	{
		time_t tmptime = time((time_t *)NULL);

		data += "Date: ";
		data += asctime(localtime(&tmptime));
	}

	// And now, it's time to play with our data object
	TucanData_var dob = msg->getDataObject();

	// Write the content-type
	type = dob->getContentType();

	unsigned int pc = dob->getPartCount();

	// If there are multiple parts to this data object
	if (pc > 0)
	{
		CORBA::String_var type = dob->getContentType();

		char buf[128];
		
		// FIXME: This needs to be better so that it KNOWS it's boundary is unique
		snprintf(buf, 128, "%ld", time((time_t)NULL));
		string boundary = "--tucan";
		boundary += buf;
		boundary += "MPx--";
		
		
		// If it's not multipart, it should be!
		if (strncasecmp(type, "multipart/", 10))
			type = CORBA::string_dup("multipart/mixed");
			
		data += "Content-Type: ";
		data +=	type;

		data += ";\n\tboundary=\"" + boundary;
		data += "\"\n\n";

		data += "This is a multipart message in MIME format.\n\n";
			
		for (unsigned int i = 0; i < pc; i++)
		{	
			TucanData::TucanPart_var part;
			string part_data("");

			data += "--" + boundary + "\n";
			data += "Content-Type: ";

			part = dob->getPart(i);

			tmp = part->type;

			if (strlen(tmp) > 1)
				data +=	tmp;
			else
				data += "text/plain";

			data += "\n";

			// We should write all of the header stuff now
			unsigned int y;
			
			for (y = 0; y < part->header.length(); y++)
			{
				data += part->header[y];
				data += "\n";
			}
		
			char *rd = (char *)malloc(sizeof(char *) * part->data.length());
			
			for (unsigned long dc = 0; dc < part->data.length(); dc++)
				rd[dc] = part->data[dc];

			if (strlen(part->encoding) > 0)
			{
				if (!strcasecmp(part->encoding, "base64"))
				{
					// Yeah, we should probably encode it.
					part_data += encodeBase64(rd, part->data.length());
				}
			}
			else if (strncasecmp(tmp, "text/", 5))
			{
				data += "Content-transfer-encoding: base64\n";
				part_data += encodeBase64(rd, part->data.length());
			}
			else
			{
				part_data += rd;
			}

			free(rd);
		
			// End the header
			data += "\n";	

			// And add the data
			data += part_data; 

			data += "\n";
		}

		data += "--" + boundary + "--\n";

	}
	else
	{
		// The content Type, if needed
		tmp = dob->getContentType();

		CORBA::String_var temptext = dob->getText();

		string datastr("");

		if (!strncasecmp(tmp, "text/", 5) && (strlen(temptext) > 1))
		{
			datastr = temptext;
		}
		else
		{
			TucanRawData* tempdata = msg->getData();

			char *rd = (char *)malloc(sizeof(char *) * tempdata->length());
			
			for (unsigned long dc = 0; dc < tempdata->length(); dc++)
				rd[dc] = (*tempdata)[dc];

			if (strncasecmp("text/", tmp, 5))
			{
				data += "Content-Transfer-Encoding: base64\n";

				datastr =  encodeBase64(rd, tempdata->length());
			}
			else
			{
				datastr += rd;
			}

			free(rd);

		}

		if (strlen(tmp) > 1)
		{	
			data += "Content-Type: ";
			data +=	tmp;
			data += "\n";
		}

		data += "\n";
		data += datastr;
		data += "\n";

	}

	// Generate a file name
	char filename[1024];

	snprintf(filename, 1024, "%ld.%ld.local.%s", time((time_t*)NULL), ((long)random() % 10000), getenv("HOSTNAME"));
	msgfilename += filename;

	// And write to our file
	ofstream fd;
	fd.open(msgfilename.c_str(), ios::out);
	fd << data;
	fd.close();
}

void TucanEmailFolder_i::appendMessages(const TucanMessageSeq& msgs)
{
	for (unsigned int i = 0; i < msgs.length(); i++)
		appendMessage(msgs[i]);
}

void TucanEmailFolder_i::deleteMessage(CORBA::Long msgnum)
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

	unlink(filename.c_str());
}

CORBA::Long TucanEmailFolder_i::getMessageFlags(CORBA::Long msgnum)
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
void TucanEmailFolder_i::setMessageFlags(CORBA::Long msgnum, CORBA::Long flags)
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
void TucanEmailFolder_i::setMessageFlag(CORBA::Long msgnum, CORBA::Long flag)
{
	// There's no need to duplicate lots of code.  Mmmm, code reuse.

	CORBA::Long flags = getMessageFlags(msgnum);
	flags |= flag;
	setMessageFlags(msgnum, flags);
}

void TucanEmailFolder_i::unsetMessageFlag(CORBA::Long msgnum, CORBA::Long flag)
{
	// There's no need to duplicate lots of code.  Mmmm, code reuse.

	CORBA::Long flags = getMessageFlags(msgnum);
	
	if (flags & flag)
	{
		flags ^= flag;
		setMessageFlags(msgnum, flags);
	}
}

TucanAuth_ptr TucanEmailFolder_i::getAuth()
{
	return TucanAuth::_duplicate(myAuth);
}

void TucanEmailFolder_i::setAuth(TucanAuth_ptr auth)
{
	myAuth = TucanAuth::_duplicate(auth);
}

void TucanEmailFolder_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);
}

// Factory

TucanEmailFolderFactory_i::TucanEmailFolderFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanFolder_ptr TucanEmailFolderFactory_i::create()
{
	TucanEmailFolder_i* myFolder = new TucanEmailFolder_i(myOrb);
	PortableServer::ObjectId_var myId = myPoa->activate_object(myFolder);
	CORBA::Object_var obj = myPoa->servant_to_reference(myFolder);
	myFolder->_remove_ref();

	return TucanEmailFolder::_narrow(obj);
	
}
