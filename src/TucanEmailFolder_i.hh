//////////////////////////////////////////////////////////
// TucanEmailFolder_i (c++ header)	
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
// PURPOSE.	See the GNU General Public License for more 
// details.
//
// You should have received a copy of the GNU General Public 
// License along with this program; if not, write to the Free 
// Software Foundation, Inc., 59 Temple Place, Suite 330, 
// Boston, MA	02111-1307	USA
// 
//////////////////////////////////////////////////////////
// Filename:		TucanEmailFolder_i.hh 
// Description:		TucanEmailFolder for CORBA implementation 
// Author:			Rob Flynn <rob@difinium.com> 
// Created at:		Tue Dec 12 13:44:29 PST 2000 
//////////////////////////////////////////////////////////

#include <Tucan.hh>
#include <dirent.h>
#include <iostream.h>
#include <fstream.h>
#include <string>

class TucanEmailFolder_i: public POA_TucanEmailFolder,
	public PortableServer::RefCountServantBase 
{

	public:
		TucanEmailFolder_i(CORBA::ORB_var orb);
		virtual ~TucanEmailFolder_i() {};
	
		CORBA::Boolean exists();
		CORBA::Boolean create();
		CORBA::Boolean deleteFolder();
		TucanMessageSeq* expunge();
		TucanFolder_ptr getFolder(const char* name);
		TucanFolder_ptr getNewFolderObject();
		void setParent(TucanFolder_ptr folder);
		TucanFolder_ptr getParent();
		TucanStringList* listFolders();
		TucanFolderSeq* list();
		CORBA::Boolean renameTo(const char* name);
		char* getName();
		void setName(const char* name);
		char* getFullName();
		void setFullName(const char* name);
		TucanMessage_ptr getMessage(CORBA::Long msgnum);
		TucanMessageSeq* getMessages();
		TucanMessageSeq* getNewMessages();
		TucanMessageSeq* getMessagesInRange(CORBA::Long start, CORBA::Long stop);
		TucanMessage_ptr getNewMessageObject();
		TucanMessageHeaderSeq* getMessageHeaders();
		CORBA::Long getMessageCount();
		CORBA::Long getNewMessageCount();
		CORBA::Boolean hasNewMessages();
		void moveMessage(CORBA::Long msgnum, TucanFolder_ptr folder);
		void moveMessages(const TucanLongList& msgnums, TucanFolder_ptr folder);
		void copyMessage(CORBA::Long msgnum, TucanFolder_ptr folder);
		void copyMessages(const TucanLongList& msgnums, TucanFolder_ptr folder);
		void appendMessage(TucanMessage_ptr msg);
		void appendMessages(const TucanMessageSeq& msgs);
		void deleteMessage(CORBA::Long msgnum);
		CORBA::Long getMessageFlags(CORBA::Long msgnum);
		void setMessageFlags(CORBA::Long msgnum, CORBA::Long flags);
		void setMessageFlag(CORBA::Long msgnum, CORBA::Long flag);
		void unsetMessageFlag(CORBA::Long msgnum, CORBA::Long flag);
		TucanAuth_ptr getAuth();
		void setAuth(TucanAuth_ptr auth);
		void destroy();
 
	private:
		string myName;
		string myFullName;
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
		TucanAuth_var myAuth;
		TucanMessageSeq* myExpunged;
		TucanFolder_var myParent;

		string TucanEmailFolder_i::findFileName(CORBA::ULong msgnum);
};

class TucanEmailFolderFactory_i: public POA_TucanEmailFolderFactory,
	public PortableServer::RefCountServantBase 
{

	public:
		TucanEmailFolderFactory_i(CORBA::ORB_var orb);
		virtual ~TucanEmailFolderFactory_i() {};

		TucanFolder_ptr create();

	private:
		 CORBA::ORB_var myOrb;
		 PortableServer::POA_var myPoa;
};
