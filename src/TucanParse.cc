//////////////////////////////////////////////////////////
// TucanParse.hh
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
// Filename:    TucanParse.hh 
// Description: E-mail parser written for TUCAN
// Author:      Rob Flynn <rob@difinium.com>
// Created at:  Fri Jan 23 23:35:33 PST 2001
//////////////////////////////////////////////////////////


#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <hash_map>
#include <ctype.h>
#include <TucanParse.hh>
#include <TucanUtilities.hh>

string TucanParse::getHeader(string message)
{
	unsigned long pos;
	string header;
	
	pos = message.find("\r\n\r\n");

	if (pos > message.length())
		pos = message.find("\n\n");

	header = message.substr(0, pos);

	return message.substr(0, pos);
}

string TucanParse::getBody(string message)
{
	unsigned long pos;
	int pc;
	vector<string> hmm;

	pos = message.find("\r\n\r\n");
	pc = 4;
	
	if (pos > message.length())
	{
		pos = message.find("\n\n");
		pc = 2;
	}
		
	return message.substr(pos + pc, message.length());
}


myHash TucanParse::parseHeader(string header)
{
	vector <string> lines;
	vector <string> tmp;
	string key;
	string value;
	myHash items;
	string newvalue;
	unsigned long i = 0;
	unsigned long j = 0;
	unsigned int k = 0;

	// Split the string by new lines
	tmp = strsplit(header, '\n');

	// Parse the entries, combining double lines if needed
	for (i = 0; i < tmp.size(); i++)
	{
		// Multiline entry?
		tmp[i] = strchomp(tmp[i]);

		if ((tmp[i][0] != ' ') && (tmp[i][0] != '\t'))
		{
			lines.push_back(tmp[i]);
			j++;
		}
		else
		{
			// If so, append it to the previous line
			if (j == 0)
				lines.push_back(tmp[i]);
			else
				lines[j-1] += tmp[i];
		}

	}

	// Build our hash
	for (i = 0; i < lines.size(); i++)
	{
		
		key = lines[i].substr(0, lines[i].find(":"));
		value = lines[i].substr(lines[i].find(":")+1, lines[i].length());

		newvalue = "";
		j = 0;

		for (k = 0; k < value.length(); k++)
		{
			if (j == 1)
				newvalue += value[k];

			// Be sure to remove all white space
			if ((value[k] != ' ') && (value[k] != '\t') && (j == 0))
			{
				j = 1;
				newvalue += value[k];
			}
		}

		// And make our key lowercase
		for (k = 0; k < key.length(); k++)
			key[k] = tolower(key[k]);

		// Now, we should check to see if this value is encoded in intl format.

		if (!strncmp(newvalue.c_str(), "=?", 2))
		{
			vector<string> bits = strsplit(newvalue, '?', 4);

			if (bits[4].compare("="))
				continue;

			// If we had less than three question marks, abort
			if (bits.size() < 4)
				continue;

			string nv = bits[3];

			// Now that we have the file name, we should decide what to do.  If
			// the encoding type isn't valid, we should get out of here

			bits[2][0] = tolower(bits[2][0]);

			if (bits[2].compare("Q"))
				newvalue = strchomp(qpDecode(nv));  // Quoted Printable
			else if (bits[2].compare("B"))
			{
				// Base64
				;
			}
			else
				continue;
				
			bits.clear();
		}

		// I guess I should chop off white spaces
		unsigned long len = key.length();

		// While we don't have our bad characters
		while ((key[len - 1] == ' ') || (key[len - 1] == '\t'))
		{
			// Decrease our length and get a new substring
			len--;
			key = key.substr(0, len);

			// If we kill the whole string, abort.
			if (len == 0)
				break;
		}

		// Insert them into the hash
		items.insert(pair<string,string>(key,newvalue));
	}

	tmp.clear();
	lines.clear();
	
	// And return
	return items;
}

myHash TucanParse::parseItem(string name, string line)
{
	myHash item;
	vector<string> parts;
	string newstr;
	string key, value;
	unsigned int i, j, k;
	unsigned int len;
	
	// Split the item by its delimiter
	parts = strsplit(line, ';');

	len = parts.size();

	for (i = 0; i < len; i++)
	{
		parts[i] = strchomp(parts[i]);

		newstr = "";
		j = 0;

		// Let's remove all white spaces
		for (k = 0; k < parts[i].length(); k++)
		{
			if (j == 1)
				newstr += parts[i][k];

			if ((parts[i][k] != ' ') && (parts[i][k] != '\t') && (j == 0))
			{
				j = 1;
				newstr += parts[i][k];
			}
		}

		// Now we should split on the equals mark.

		if (newstr.find("=") <= newstr.length())
		{
			key = newstr.substr(0, newstr.find("="));
			value = newstr.substr(newstr.find("=") + 1, newstr.length());

			// This sucks. I didnt think you could have white space 
			// here.  I guess you can.
			
			while ((value[0] == ' ' || value[0] == '\t')&&(value.length() > 0))
				value = value.substr(1, value.length());	

			if (value[0] == '\"')
			{
				for (k=1,j=value.length(); j > 0; j--, k++)
				{
					if (value[j] == '\"')
						break;
				}

				value = value.substr(1, value.length()-k);
			}

		
		}
		else
		{
			value = parts[i];

			while ((value[0] == ' ' || value[0] == '\t')&&(value.length() > 0))
				value = value.substr(1, value.length());	

			if (value[0] == '\"')
			{
				for (k=1,j=value.length(); j > 0; j--, k++)
				{
					if (value[j] == '\"')
						break;
				}

				value = value.substr(1, value.length()-k);
			}

			key = name;
			value = newstr;
		}
	
		// Make sure the key is lower case			

		for (k = 0; k < key.length(); k++)
			key[k] = tolower(key[k]);

		// Now, we should check to see if this value is encoded in intl format.

		if (!strncmp(value.c_str(), "=?", 2))
		{
			vector<string> bits = strsplit(value, '?', 4);

			if (bits[4].compare("="))
				continue;

			// If we had less than three question marks, abort
			if (bits.size() < 4)
				continue;

			string nv = bits[3];

			// Now that we have the file name, we should decide what to do.  If
			// the encoding type isn't valid, we should get out of here

			bits[2][0] = tolower(bits[2][0]);

			if (bits[2].compare("Q"))
				value = strchomp(qpDecode(nv));  // Quoted Printable
			else if (bits[2].compare("B"))
			{
				// Base64
				;
			}
			else
				continue;
				
			bits.clear();
		}
		
		item.insert(pair<string,string>(key,value));
	}

	parts.clear();
	
	return item;
}


vector<string> TucanParse::parseBody(string body, string boundary)
{
	string sboundary;
	string eboundary;
	string subbody("");
	vector<string> lines;
	vector<string> parts;
	vector<string> newparts;
	string line;

	myHash subheader;
	myHash item;

	unsigned long len;
	unsigned long i;

	char saving = 0;
	
	// Set up our starting and ending boundaries
	sboundary = "--" + boundary;
	eboundary = "--" + boundary + "--";

	// Split the body into lines of text
	lines = strsplit(body, '\n');

	len = lines.size();

	// Step through each line of the body
	for (i = 0; i < len; i++)
	{
		line = strchomp(lines[i]);

		// We've found the start of a block of text
		if (strncmp(line.c_str(), sboundary.c_str(), sboundary.length()) == 0)
		{
			// Are we already saving data? 
			if (saving == 0)
			{
				// Looks like no.  Let's do it.
				saving = 1;
				continue;
			}
			else
			{
				// If we manage to make it here then it means
				// we have been saving text.  It also means
				// that we've started another section and
				// are at the end of the previous block
				// of text.
				
				parts.push_back(subbody);
				subbody = "";
			}
			
		}
		else if (strncmp(line.c_str(), eboundary.c_str(), eboundary.length()) == 0)
		{
			// Looks like we've reached the end of our
			// multipart message.
			
			parts.push_back(subbody);	
		}

		else
		{
			if (saving == 1)
			{
				// Otherwise, if we're saving data, let's
				// continue to do so.

				subbody += line + "\n";
			}
		}
	}	
	
	lines.clear();

	len = parts.size();
	newparts.clear();

	// Loop once for each part we previously found
	for (i = 0; i < len; i++)
	{
		// Now, we really have to rely on our methods being fast
		
		subbody = parts[i];
		subheader = parseHeader(getHeader(subbody));

		item = parseItem("content-type", subheader["content-type"]);

		if (strncasecmp(item["content-type"].c_str(), "multipart/", 10) == 0)
		{
			// Looks like we have another multipart message. 
			string newb;

			newb = item["boundary"];

			newparts = parseBody(subbody, newb);
		}
		else
		{
			// We're finished parsing this messagte
			newparts.push_back(subbody);
		}

		item.clear();
		subheader.clear();
		
	}	
	
	parts.clear();

	return newparts;
}
