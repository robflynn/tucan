//////////////////////////////////////////////////////////
// TucanPop3.hh
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
// Filename:    TucanPop3.hh
// Description: POP3 Wrapper written for TUCAN
// Author:      Rob Flynn <rob@difinium.com>
// Created at:  Fri Jan 23 23:35:33 PST 2001
//////////////////////////////////////////////////////////

#ifndef __TUCAN_POP3_H_
#define __TUCAN_POP3_H_

#include <iostream>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "TucanEmail.hh"

class TucanPop3 : public TucanEmail
{
	public:
		TucanPop3(string server);
		virtual ~TucanPop3() {};

		void open(string username, string password);
		void close();
		int getMessageCount();
		int getMessageSize(int mnum);
		string getMessage(int mnum);
		string getMessageId(int mnum);
		void deleteAllMessages();
		void deleteMessage(int mnum);

	private:
		int status;
		int PORT;
		string mySvrStr;
		int mySockFd;
		char myBuf[MAXSIZE];
		struct hostent *myHe;
		struct sockaddr_in myServer;
};

#endif
