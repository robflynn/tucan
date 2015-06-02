//////////////////////////////////////////////////////////
// TucanMBOX.cc (C++ impl)
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
// Filename:    TucanMBOX.cc
// Description: MBOX Wrapper written for TUCAN
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
#include <TucanMBOX.hh>
#include <TucanUtilities.hh>
#include <sys/time.h>
#include <fstream>

TucanMBOX::TucanMBOX(string server)
{
	connected = 0;
}

TucanMBOX::TucanMBOX()
{
	connected = 0;
}

void TucanMBOX::open(string username, string password)
{

	if (connected)
		return;

	connected = 1;

	myUserName = username;
	myPassword = password;
}


void TucanMBOX::close()
{
	if (!connected)
		throw("Method cannot be called while disconnected.");

	connected = 0;
}

int TucanMBOX::getMessageCount()
{
	string filename;
	string line;
	unsigned int counter = 0;

	if (!connected)
		throw("Method cannot be called while disconnected.");

	filename = "/var/spool/mail/" + myUserName;

	ifstream infile(filename.c_str());

	// Read each line
	while (infile)
	{
		getline(infile, line);

		// If we have at least 5 characters
		if (line.length() > 5)
			// And the line is "From ", then increase our counter
			if (!strncmp(line.c_str(), "From ", 5))
				counter++;
	}

	infile.close();

	return counter;
}

string TucanMBOX::getMessageId(int mnum)
{
	if (!connected)
		throw("Method cannot be called while disconnected.");

	snprintf(myBuf, 32, "%d", mnum);
	string msgnum = myBuf;	

	string dir = "var/spool/mail/" + myUserName;
	dir += "." + msgnum;
	dir = strreplace(dir, '/', '.');

	return dir;
}

int TucanMBOX::getMessageSize(int mnum)
{

	string msg;
	
	if (!connected)
		throw("Method cannot be called while disconnected.");

	// Simple.  Get the message
	msg = getMessage(mnum);

	// And return the length
	return msg.length();
}
	
void TucanMBOX::deleteMessage(int mnum)
{
	throw("Error: Method deleteMessage() is not yet supported.");
}

void TucanMBOX::deleteAllMessages()
{
	throw("Error: Method deleteAllMessages() is not yet supported.");
}

string TucanMBOX::getMessage(int mnum)
{
	string filename;
	string line;
	string msg("");
	int counter = 0;
	int from = 0;
	
	if (!connected)
		throw("Method cannot be called while disconnected.");

	filename = "/var/spool/mail/" + myUserName;

	// Open the file
	ifstream infile(filename.c_str());

	// While we have data ...
	while (infile)
	{
		getline(infile, line);

		// Loop until we have the right number
		if (line.length() > 5)
			if (!strncmp(line.c_str(), "From ", 5))
				counter++;

		// If we have it ...
		if (counter == mnum)
		{
			if (from == 1)
			{
				// If we have a > in front of From, delete it.
				// This means we've already read our "From"
				if (!strncmp(line.c_str(), ">From ", 6))
					line = line.c_str() + 1;

				// If we already have lines, append it, otherwise, set it.
				if (msg.length() > 0)
					msg += "\n" + line;
				else
					msg = line;
			}

			from = 1;
		}

		// When we're finished, exit.
		if (counter > mnum)
			break;
	}

	infile.close();

	return msg;
}
