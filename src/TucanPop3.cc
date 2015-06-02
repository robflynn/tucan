//////////////////////////////////////////////////////////
// TucanPop3.cc (C++ impl)
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
// Filename:    TucanPop3.cc
// Description: POP3 Wrapper written for TUCAN
// Author:      Rob Flynn <rob@difinium.com>
// Created at:  Fri Jan 23 23:35:33 PST 2001
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
#include <TucanPop3.hh>
#include <TucanUtilities.hh>

TucanPop3::TucanPop3(string server)
{
	status = 0;
	PORT = 110;
	mySvrStr = server;
}

void TucanPop3::open(string username, string password)
{
	string msg = "USER " + username + "\r\n";
	string reply("");
	int len = msg.length();
	int num, i;
	
	mySockFd = socket(AF_INET, SOCK_STREAM, 0);
	myHe = gethostbyname(mySvrStr.c_str());	
	
	if (!myHe)
		throw("Hostname '" + mySvrStr + "' could not be resolved.");

	myServer.sin_family = AF_INET;
	myServer.sin_port = htons(PORT);
	myServer.sin_addr = *((struct in_addr *)myHe->h_addr);
	bzero(&(myServer.sin_zero), 8);

	if (connect(mySockFd, (struct sockaddr*)&myServer, sizeof(struct sockaddr)) < 0)
	{
		throw("A connection to '" + mySvrStr + "' could not be established.");
	}
	num = recv(mySockFd, myBuf, MAXSIZE, 0);

	status = 1;

	for(i=0; i<num; i++)
	{
		reply = reply + myBuf[i];
	}

#ifdef DEBUG
	cout << reply;
#endif

	num = send(mySockFd, msg.c_str(), len, 0);
	num = recv(mySockFd, myBuf, MAXSIZE, 0);

	for(i=0; i<num; i++)
	{
		reply = reply + myBuf[i];
	}

#ifdef DEBUG
	cout << reply;
#endif

	msg = "PASS " + password + "\r\n";
	reply = "";
	len = msg.length();

	num = send(mySockFd, msg.c_str(), len, 0);
	num = recv(mySockFd, myBuf, MAXSIZE, 0);

	for(i=0; i<num; i++)
	{
		reply = reply + myBuf[i];
	}

#ifdef DEBUG
	cout << reply;
#endif
}

void TucanPop3::close()
{
	string msg = "QUIT\r\n";
	string reply("");
	int len = msg.length();
	int num;

	if (!status)
		throw("Method can not be called while disconnected.");

	status = 0;

	num = send(mySockFd, msg.c_str(), len, 0);
	num = recv(mySockFd, myBuf, MAXSIZE, 0);
	myBuf[num] = 0;

	reply += myBuf;

#ifdef DEBUG
	cout << "close: " << reply;
#endif
}

int TucanPop3::getMessageCount()
{
	string msg = "STAT\r\n";
	string reply("");
	int len = msg.length();
	int num, i, i2;

	if (!status)
		throw("Method can not be called while disconnected.");

	num = send(mySockFd, msg.c_str(), len, 0);
	num = recv(mySockFd, myBuf, MAXSIZE, 0);
	myBuf[num] = 0;

	reply += myBuf;

	i = reply.find(" ");
	string tmp;
	tmp.assign(reply, i + 1, reply.size());

	i2 = tmp.find(" ");
	string tmp2;
	tmp2.assign(tmp, 0, i2);

	return atoi(tmp2.c_str());
}

string TucanPop3::getMessageId(int mnum)
{
	int num, i;
	string reply("");
	string tmp("");
	int space = 0;
	string msgnum;

	if (!status)
		throw("Method can not be called while disconnected.");

	snprintf(myBuf, 32, "%d", mnum);
	msgnum = myBuf;

	string msg = "UIDL " + msgnum + "\r\n";
	
	num = send(mySockFd, msg.c_str(), msg.length(), 0);
	num = recv(mySockFd, myBuf, MAXSIZE, 0);

	for(i=0; i<num; i++)
	{
		if (space >= 2) {
			tmp = tmp + myBuf[i];
		}

		if (myBuf[i] == ' ')
		{
			space++;
		}
	}

	tmp = strchomp(tmp);
	
	return tmp;
}

int TucanPop3::getMessageSize(int mnum)
{
	int num, i;
	string reply("");
	string tmp("");
	int space = 0;
	string msgnum;

	if (!status)
		throw("Method can not be called while disconnected.");

	snprintf(myBuf, 32, "%d", mnum);
	msgnum = myBuf;

	string msg = "LIST " + msgnum + "\r\n";

	num = send(mySockFd, msg.c_str(), msg.length(), 0);
	num = recv(mySockFd, myBuf, MAXSIZE, 0);

	for(i=0; i<num; i++)
	{
		if (space >= 2) {
			tmp = tmp + myBuf[i];
		}

		if (myBuf[i] == ' ')
		{
			space++;
		}
	}

	return atoi(tmp.c_str());
}
		
void TucanPop3::deleteMessage(int mnum)
{
	string reply("");
	int num;
	string msgnum;

	if (!status)
		throw("Method can not be called while disconnected.");

	snprintf(myBuf, 32, "%d", mnum);
	msgnum = myBuf;

	string msg = "DELE " + msgnum + "\r\n";
	
	num = send(mySockFd, msg.c_str(), msg.length(), 0);
	num = recv(mySockFd, myBuf, MAXSIZE, 0);
}

void TucanPop3::deleteAllMessages()
{
	int num;
	int i, j;
	char* tmp = (char *)malloc(sizeof(char) * 64);

	if (!status)
		throw("Method can not be called while disconnected.");

	i = getMessageCount();

	for (j=0; j<i; j++)
	{
		snprintf(tmp, 63, "DELE %d\r\n", j+1);
		num = send(mySockFd, tmp, strlen(tmp), 0);
		num = recv(mySockFd, myBuf, MAXSIZE, 0);
	}
	
	free(tmp);
}

string TucanPop3::getMessage(int mnum)
{
	string reply("");
	string header("");
	string line("");
	
	unsigned long size;
	unsigned long num, j = 0;
	unsigned long i = 0;
	char c;
	string msgnum;

	if (!status)
		throw("Method can not be called while disconnected.");

	snprintf(myBuf, 32, "%d", mnum);
	msgnum = myBuf;

	string msg = "RETR " + msgnum + "\r\n";

	size = getMessageSize(mnum);

	num = send(mySockFd, msg.c_str(), msg.length(), 0);

	bzero(myBuf, MAXSIZE); 
	do 
	{
		num = recv(mySockFd, &myBuf[i], 1, 0);
	} while (myBuf[i++] != '\n');
		
	myBuf[i] = 0;

	if (!strncasecmp("-ERR", myBuf, 4)) {
		throw("Message " + msgnum + " Not Found");
		return 0;
	}
	do {
		i = recv(mySockFd, &c, 1, MSG_PEEK);
		
		if (c == '\n' || c == '\r')
			i = recv(mySockFd, &c, 1,0);

	} while (c == '\n' ||  c == '\r');

	while (1)
	{
		i = 0;
		do {
			j = recv(mySockFd, &c, 1, 0);
			myBuf[i] = c;
			i++;

		} while (c != '\n');
		myBuf[i] = 0;

		line = strchomp(myBuf);

		if (!strcmp(line.c_str(), "."))
			break;

		reply += myBuf;

	}	

	return reply;
}
