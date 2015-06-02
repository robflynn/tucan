//////////////////////////////////////////////////////////
// TucanIMAP.cc (C++ impl)
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
// PURPOSE.     See the GNU General Public License for more 
// details.
//
// You should have received a copy of the GNU General Public 
// License along with this program; if not, write to the Free 
// Software Foundation, Inc., 59 Temple Place, Suite 330, 
// Boston, MA   02111-1307      USA
// 
//////////////////////////////////////////////////////////
// Filename:    TucanIMAP.cc
// Description: IMAP Wrapper written for TUCAN
// Author:      Rob Flynn <rob@difinium.com>
// Created at:  Sun Feb 25 20:40:40 PST 2001
//////////////////////////////////////////////////////////

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <TucanIMAP.hh>
#include <TucanUtilities.hh>
#include <sys/time.h>

string trId()
{
	char buf[32];
	string trid;
	
	snprintf(buf, 32, "%ld", time((time_t)NULL));
	trid = buf;

	return trid;
}

string readLine(int sock)
{
	string buf;
	int i;
	char c;

	i = recv(sock, &c, 1, 0);

	while (i > 0 && c != 10)
	{
		buf += c;
		i = recv(sock, &c, 1, 0);
	}

#ifdef DEBUG
	cout << buf << endl;
#endif

	return buf;
}

void TucanIMAP::processFailure(string command)
{
	// Process various command failures
	if ((!command.compare("LOGIN")) || (!command.compare("AUTHENTICATE")))
	{
		throw("IMAP Error: Invalid authentication");
	}
	else if (!command.compare("SELECT"))
	{
		throw("IMAP Error: Could not open the requested mailbox folder.");
	}
}

void TucanIMAP::processSuccess(string command)
{
	// When we first sign on, we should select the Inbox.
	if ((!command.compare("LOGIN")) || (!command.compare("AUTHENTICATE")))
	{
		selectFolder("INBOX");
	}
}

void TucanIMAP::parseCommand(string command)
{
	vector<string> response = strsplit(command, ' ');

	if (!response[1].compare("NO"))
	{
		processFailure(response[2]);
	}
	else if (!response[1].compare("OK"))
	{
		processSuccess(response[2]);
	}
	
	response.clear();
}


TucanIMAP::TucanIMAP(string server)
{
	PORT = 143;
	connected = 0;
	myHostName = server;

	mySockFd = socket(AF_INET, SOCK_STREAM, 0);
	myHe = gethostbyname(server.c_str());	
	myServer.sin_family = AF_INET;
	myServer.sin_port = htons(PORT);
	myServer.sin_addr = *((struct in_addr *)myHe->h_addr);
	bzero(&(myServer.sin_zero), 8);

	connect(mySockFd, (struct sockaddr*)&myServer, sizeof(struct sockaddr));
}

void TucanIMAP::open(string username, string password)
{
	string tid = trId();
	string etid = tid + " ";
	string msg = tid + " LOGIN " + username + " " + password + "\r\n";
	string reply;
	int num;

	if (connected)
		return;
	
	mySockFd = socket(AF_INET, SOCK_STREAM, 0);
	myHe = gethostbyname(myHostName.c_str());	

	// Make sure we resolved something
	if (!myHe)
		throw("Host name " + myHostName + " could not be resolved.");
	
	myServer.sin_family = AF_INET;
	myServer.sin_port = htons(PORT);
	myServer.sin_addr = *((struct in_addr *)myHe->h_addr);
	bzero(&(myServer.sin_zero), 8);

	if (connect(mySockFd, (struct sockaddr*)&myServer, sizeof(struct sockaddr)) < 0)
	{
		throw("A connection to '" + myHostName + "' could not be made.");
	}

	connected = 1;


	// Send our request
	num = send(mySockFd, msg.c_str(), msg.length(), 0);

	// Receive our repsonse
	do
	{
		reply = readLine(mySockFd);
	} while (!strstr(reply.c_str(), etid.c_str()));

	parseCommand(reply);
}


void TucanIMAP::selectFolder(string folder)
{
	string tid = trId();
	string etid = tid + " ";
	string msg = tid + " SELECT " + folder + "\r\n";
	string reply;
	int num;

	if (!connected)
		throw("Method cannot be called while disconnected.");

	myFolder = folder;
	
	// Send our request
	num = send(mySockFd, msg.c_str(), msg.length(), 0);

	// Receive our repsonse
	do
	{
		reply = readLine(mySockFd);
		if (strstr(reply.c_str(), "* OK [UIDVALIDITY "))
		{
			int start, end;
			string tmp;
			
			start = reply.find("* OK [UIDVALIDITY ")+18;
			end = reply.find("]");

			tmp = reply.substr(start, end-start);
			myUID = tmp;

		}
	} while (!strstr(reply.c_str(), etid.c_str()));
	
	parseCommand(reply);

}

void TucanIMAP::close()
{
	string msg = trId() + " STATUS " + myFolder + " (Messages)\r\n";
	int num;

	if (!connected)
		throw("Method cannot be called while disconnected.");

	// Send our request
	num = send(mySockFd, msg.c_str(), msg.length(), 0);

	connected = 0;
}

int TucanIMAP::getMessageCount()
{
	string tid = trId();
	string etid = tid + " OK";
	string msg = tid + " STATUS " + myFolder + " (Messages)\r\n";
	string reply;
	int num;
	string tmp2;
	vector<string>tmp;
	int numOfMsgs = 0;

	if (!connected)
		throw("Method cannot be called while disconnected.");

	// Send our request
	num = send(mySockFd, msg.c_str(), msg.length(), 0);

	// Receive our repsonse
	do
	{
		reply = readLine(mySockFd);

		if (strstr(reply.c_str(), "* STATUS INBOX (MESSAGES"))
		{
			tmp = strsplit(reply, ' ');

			tmp2 = tmp[tmp.size()-1];
			tmp2[tmp2.length()-2] = 0;

			tmp.clear();
		
			numOfMsgs = atoi(tmp2.c_str());
			
		}

	} while (!strstr(reply.c_str(), etid.c_str()));

	return numOfMsgs;
}

string TucanIMAP::getMessageId(int mnum)
{
	string id;
	
	if (!connected)
		throw("Method cannot be called while disconnected.");

	snprintf(myBuf, 32, "%d", mnum);
	string msgnum = myBuf;	

	id = myHostName + myUID + msgnum;

	return id;
}

int TucanIMAP::getMessageSize(int mnum)
{
	string tid = trId();
	string etid = tid + " OK";
	string reply;
	int num;
	string tmp2;
	vector<string>tmp;
	int size = 0;
	
	if (!connected)
		throw("Method cannot be called while disconnected.");

	snprintf(myBuf, 32, "%d", mnum);
	string msgnum = myBuf;	

	string msg = tid + " FETCH " + msgnum + " RFC822.SIZE\r\n";

	//FIXME: Exception if messaghe number is too high

	// Send our request
	num = send(mySockFd, msg.c_str(), msg.length(), 0);

	// Receive our repsonse
	do
	{
		reply = readLine(mySockFd);

		if (strstr(reply.c_str(), " FETCH (RFC822.SIZE "))
		{
			tmp = strsplit(reply, ' ');

			tmp2 = tmp[tmp.size()-1];
			tmp2[tmp2.length()-2] = 0;

			tmp.clear();
		
			size = atoi(tmp2.c_str());
			
		}

	} while (!strstr(reply.c_str(), etid.c_str()));

	return size;
}
	
void TucanIMAP::expunge()
{
	string tid = trId();
	string etid = tid + " OK";
	string reply;
	string msg = tid + " EXPUNGE\r\n";
	int num;
	
	if (!connected)
		throw("Method cannot be called while disconnected.");

	// Send our request
	num = send(mySockFd, msg.c_str(), msg.length(), 0);
	
	// Receive our repsonse
	do
	{
		reply = readLine(mySockFd);

	} while (!strstr(reply.c_str(), etid.c_str()));
}

void TucanIMAP::deleteMessage(int mnum)
{
	string tid = trId();
	string etid = tid + " OK";
	string reply;
	int num;

	if (!connected)
		throw("Method cannot be called while disconnected.");
	
	snprintf(myBuf, 32, "%d", mnum);
	string msgnum = myBuf;	

	string msg = tid + " STORE " + msgnum + " +FLAGS (\\Deleted)\r\n";
	
	//FIXME: Exception if messaghe number is too high

	// Send our request
	num = send(mySockFd, msg.c_str(), msg.length(), 0);

	// Receive our repsonse
	do
	{
		reply = readLine(mySockFd);

	} while (!strstr(reply.c_str(), etid.c_str()));

	expunge();

}

void TucanIMAP::deleteAllMessages()
{
	string tid = trId();
	string etid = tid + " OK";
	string reply;
	int num;

	if (!connected)
		throw("Method cannot be called while disconnected.");
	
	snprintf(myBuf, 32, "%d", getMessageCount());
	string msg = tid + " STORE 1:";
       	msg += myBuf;
	msg += " +FLAGS (\\Deleted)\r\n";

	// Send our request
	num = send(mySockFd, msg.c_str(), msg.length(), 0);

	// Receive our repsonse
	do
	{
		reply = readLine(mySockFd);

	} while (!strstr(reply.c_str(), etid.c_str()));

	expunge();
}

string TucanIMAP::getMessage(int mnum)
{
	string tid = trId();
	string etid = tid + " OK";
	string reply;
	int num;
	string data("");
	unsigned long size;
	int start, end;
	string tmp;
	unsigned char c;

	if (!connected)
		throw("Method cannot be called while disconnected.");

	snprintf(myBuf, 32, "%d", mnum);
	string msgnum = myBuf;
	
	string msg = tid + " FETCH " + msgnum + " RFC822\r\n";


	//FIXME: Exception if messaghe number is too high
	
	// Send our request
	num = send(mySockFd, msg.c_str(), msg.length(), 0);

	// Receive our repsonse
	do
	{
		reply = readLine(mySockFd);
	} while (reply[0] != '*');

	// Now we know some info about our message
	
	start = reply.find("{") + 1;
	end = reply.find("}");

	tmp = reply.substr(start, end-start);
	size = atol(tmp.c_str());	

	// Make sure we read only the message data
	for (unsigned long i = 0; i < size; i++)	
	{
		num = recv(mySockFd, &c, 1, 0);
		data += c;
	}

	// And now read the rest of the stuff
	do
	{
		reply = readLine(mySockFd);
	} while (!strstr(reply.c_str(), etid.c_str()));
	
	
	return data;
}
