////////////////////////////////////////////////////////////
// TucanDb (Database wrapper for Postgresql)  
//
// Copyright (C) 2000, Difinium 
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

#include <TucanDb.hh>
#include <TucanConfig.hh>

cursor::cursor(PGconn* conn)
{
	myConn = conn;
	fetch = 0;
	myResult = NULL;
	myRows = 0;
}

void cursor::execute(string exestr)
{
	if (myResult)
	{
		PQclear(myResult);
		fetch = 0;
	}

	myResult = PQexec(myConn, exestr.c_str());
	myRows = PQntuples(myResult);
	myColumns = PQnfields(myResult);
}

vector<string> cursor::fetchone() throw(exception)
{
	string temp;
	vector<string> vec;
	int i;

	if(myRows == 0)
	{
		throw exception();
	}
	else if(fetch > (myRows - 1))
	{
		throw exception();
	}
	
	for(i = 0; i < myColumns; i++)
	{
		temp = PQgetvalue(myResult, fetch, i);
		vec.push_back(temp);
	}

	fetch++;
	
	return vec;
}

vector< vector<string> > cursor::fetchall()
{
	string temp;
	vector<string> tempvec;
	vector< vector<string> > vec;
	int i;
	int j;

	for(i = fetch; i < myRows; i++)
	{
		for(j = 0; j < myColumns; j++)
		{
			temp = PQgetvalue(myResult, i, j);
			tempvec.push_back(temp);
		}

		vec.push_back(tempvec);
		tempvec.clear();
	}

	return vec;
}

int cursor::rowcount()
{
	return myRows;
}

void cursor::close()
{
	if (myResult)
		PQclear(myResult);
}

connection::connection(string db)
{
	TucanConfig* config = new TucanConfig();
	string dbip = config->get("dbip", "");
	string dbusername = config->get("dbusername", "");
	string dbpassword = config->get("dbpassword", "");
	delete config;

	myConn = PQsetdbLogin(dbip.c_str(), NULL, NULL, NULL, 
			db.c_str(), dbusername.c_str(), dbpassword.c_str());

	if(PQstatus(myConn) == CONNECTION_BAD)
	{
		PQfinish(myConn);

		if (PQstatus(myConn) == CONNECTION_BAD)
		{
			cerr << "Unable to connect to SQL server" << endl;
			exit(1);
		}
	}
}

cursor* connection::getCursor()
{
	cursor* c = new cursor(myConn);
	return c;
}

void connection::close()
{
	if (myConn)
		PQfinish(myConn);
}
