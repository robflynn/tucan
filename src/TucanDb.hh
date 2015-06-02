////////////////////////////////////////////////////////////
// TucanDb (Database wrapper for Postgresl)
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
////////////////////////////////////////////////////////////
// Filename:        TucanDb.hh 
// Description:     Abstraction of actual db interface 
// Author:          Jared Peterson <jared@difinium.com>
// Created at:      Tue Dec 12 13:44:29 PST 2000 
////////////////////////////////////////////////////////////

#ifndef __TUCAN_DB_H_
#define __TUCAN_DB_H_

#include <iostream.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <exception>
#include <libpq-fe.h>
#include <libpq-int.h>

class cursor
{
	public:
		cursor(PGconn* conn);
		~cursor(){};

		void execute(string exestr);
		vector<string> fetchone() throw(exception);
		vector< vector<string> > fetchall();
		int rowcount();
		void close();

	private:
		PGconn* myConn;
		PGresult* myResult;
		int myRows;
		int myColumns;
		int fetch;
};

class connection 
{
	public:
		connection(string db);
		~connection() {};

		cursor* getCursor();
		void close();

	private:
		PGconn* myConn;
};

#endif
