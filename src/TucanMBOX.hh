//////////////////////////////////////////////////////////
// TucanMBOX.hh
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
// Filename:    TucanMBOX.hh
// Description: MBOX Wrapper written for TUCAN
// Author:      Rob Flynn <rob@difinium.com>
// Created at:  Sun Feb 25 20:40:53 PST 2001
//////////////////////////////////////////////////////////

#ifndef __TUCAN_MBOX_H_
#define __TUCAN_MBOX_H_

#include <iostream>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "TucanEmail.hh"

class TucanMBOX : public TucanEmail
{
	public:
		TucanMBOX(string server);
		TucanMBOX();
		virtual ~TucanMBOX() {};

		void open(string username, string password);
		void close();
		int getMessageCount();
		int getMessageSize(int mnum);
		string getMessage(int mnum);
		string getMessageId(int mnum);
		void deleteAllMessages();
		void deleteMessage(int mnum);

	private:
		int connected;
		char myBuf[MAXSIZE];
		string myFolder;
		string myUID;
		string myHostName;
		string myUserName;
		string myPassword;
};

#endif
