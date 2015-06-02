//////////////////////////////////////////////////////////
// TucanUtilities.cc
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
// Filename:            TucanUtilities.cc
// Description:         Various utilities for TUCAN 
// Author:                      Rob Flynn <rob@difinium.com>
// Created at:          Fri Jan 23 23:35:33 PST 2001
//////////////////////////////////////////////////////////

#include <TucanUtilities.hh>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>

// The gethomedir() methid will return, in a string, the home
// directory of the user given.

char * gethomedir(string username)
{
	struct passwd *pstruct;

	pstruct = getpwnam(username.c_str());

	if (pstruct)
		return pstruct->pw_dir;
	
	return "";
}

char * gethomedir(char *username)
{
	struct passwd *pstruct;

	pstruct = getpwnam(username);

	if (pstruct)
		return pstruct->pw_dir;
	
	return "";
}


// The strreplace() method will take a string, and replace
// every instanvce of oldch with a new instance of newch.

string strreplace(string text, char oldch, char newch)
{
	string tmp("");

	for (unsigned long i = 0; i < text.length(); i++)
	{
		if (text[i] != oldch)
			tmp += text[i];
		else
			tmp += newch;
	}

	return tmp;
}

// The strwstrip() method will take a string and strip white space from
// the beginning and end of the string.

string strwstrip(string text)
{
	string temp;

	temp = text;

	// Get whitespace from the beginning
	while ( (temp[0] == ' ' || temp[0] == '\t') && (temp.length() > 0))
		temp = temp.substr(1, temp.length());
	
	// And from the end
	while ( (temp[temp.length()-1] == ' ' || temp[temp.length()-1] == '\t') && (temp.length() > 0))
		temp = temp.substr(0, temp.length()-1);

	return temp;
}

// The strsplit() method takes a block of text as well as a single char
// delimiter.  It steps through the string, splitting it on your delimiter
// and builds a vector of strings. 

vector < string > strsplit(string header, char split)
{
    vector < string > tmp;
    string line("");
    unsigned long i;

    for (i = 0; i < header.length(); i++)
    {
	// If we're not at our delimiter, keep building
	if (header[i] != split)
	    line += header[i];
	else
	{
	    // Otherwise, push it into our array and
	    // reset our counter
	    tmp.push_back(line);
	    line = "";
	}
    }

    // Save the left overs if we have any
    if (line.length() > 0)
	tmp.push_back(line);

    return tmp;
}

vector < string > strsplit(string header, char split, int num)
{
    vector < string > tmp;
    string line("");
    unsigned long i;
    int count = 0;

    for (i = 0; i < header.length(); i++)
    {
	if (count < num)
	{
	    if (header[i] != split)
	    {
		line += header[i];
	    } else
	    {
		tmp.push_back(line);
		count++;
		line = "";
	    }
	} else
	{
	    line += header[i];
	}
    }

    if (line.length() > 0)
    {
	tmp.push_back(line);
    }

    return tmp;
}

string strjoin(vector<string> strings, char delim)
{
	string tmp("");

	for (unsigned long i = 0; i < strings.size(); i++)
	{
		tmp += strings[i];
		if (i < strings.size()-1)
			tmp += delim;
	}

	return tmp;
}

// The strchomp() method will strip trailing \r and \n characters
// from the end of a string.

string strchomp(string orig)
{
    unsigned long len;

    len = orig.length();

    // While we don't have our bad characters
    while ((orig[len - 1] == '\n') || (orig[len - 1] == '\r'))
    {
	// Decrease our length and get a new substring
	len--;
	orig = orig.substr(0, len);

	// If we kill the whole string, abort.
	if (len == 0)
	    break;
    }

    return orig;
}

// The uppercase() method will take a string and convert it to uppercase.
string uppercase(const char *str)
{
	char *temp = strdup(str);

	for (unsigned long i = 0; i < strlen(temp); i++)
		temp[i] = toupper(temp[i]);

	string temp2(temp);
	free(temp);

	return temp2;
}

string uppercase(string str)
{
	char *temp = strdup(str.c_str());

	for (unsigned long i = 0; i < strlen(temp); i++)
		temp[i] = toupper(temp[i]);

	string temp2(temp);
	free(temp);

	return temp2;
}

// The escape() method will take a string and escape all special characters
// with an \.  This is good to do before writing to a database. 

string escape(string str)
{
	unsigned long len = str.length();
	string tmp("");

	for (unsigned long i = 0; i < len; i++)
	{
		switch (str[i])
		{
			case '\\':
			case '\'':
				tmp += "\\";
		};

		tmp += str[i];
	}

	return tmp;
}

// The escape() method will take a string and escape all special characters
// with an \.  This is good to do before writing to a database. 

const char* escape(char* str)
{
	unsigned long len = strlen(str);
	string tmp("");

	for (unsigned long i = 0; i < len; i++)
	{
		switch (str[i])
		{
			case '\\':
			case '\'':
				tmp += "\\";
		};

		tmp += str[i];
	}

	return tmp.c_str();
}

// The unhex() method will decode a string hex value and convert it into
// and integer.

int unhex(string hex)
{
    string decode("0123456789abcdef");
    int bits = 0;
    unsigned int i;

    // Convert our string to lowercase
    for (i = 0; i < hex.length(); i++)
	hex[i] = tolower(hex[i]);

    // Find our value, for each character, in the deocoding string above.
    // Since hex is base 16, it's fairly simple from here.
    for (i = 0; i < hex.length(); i++)
	bits = (bits * 16) + (decode.find(hex[i]));

    return bits;
}

// The ishex() method takes a single character and returns 1 or 0 depending on 
// the validity of the character.  If the char is a valid hex digit, a true
// response will be returned.

int ishex(char c)
{
    return (('0' <= c <= '9') || ('a' <= c <= 'f') || ('A' <= c <= 'F'));
}


// The qpDecode() method takes in a string and returns a string.  The resulting string
// is a decoded version of the inputted quotable printed message.

string qpDecode(string data)
{
    string final("");
    string str;
    string line;
    vector < string > list;
    unsigned int i;
    unsigned int j;
    unsigned int n;
    int partial;

    // I can't really explain this one.  It's based on some format specs that I read
    // about quotable printed messages.  I'll try to go back and comment this one
    // later.  If you need help understanding it, just look up how QP messages
    // work. 

    list = strsplit(data, '\n');

    for (j = 0; j < list.size(); j++)
    {
	line = list[j] + "\n";

	i = 0;
	n = line.length();

	if (n > 0 && line[n - 1] == '\n')
	{
	    partial = 0;
	    n = n - 1;
	}

	while (i < n)
	{
	    if (line[i] != '=')
	    {
		str += line[i];
		i++;
	    } else if (((i + 1) == n) && (!partial))
	    {
		partial = 1;
		break;
	    } else if (((i + 1) < n) && (line[i + 1] == '='))
	    {
		str += "=";
		i += 2;
	    }
		else if (((i + 2) < n) && (ishex(line[i + 1]))
			 && (ishex(line[i + 2])))
	    {
		string temp;
		char c[3];

		temp = line.substr(i + 1, 2);
		snprintf(c, 3, "%c", unhex(temp));

		str += c;

		i += 3;
	    } else
	    {
		str += line[i];
		i++;
	    }

	}

	if (!partial)
	{
	    final += str + "\n";
	    str = "";
	}
    }

    if (str.length() > 0)
	final += str + "\n";

    list.clear();

    return final;
}

// The uueDecode() method takes in a string and returns a string.  The resulting string
// is a decoded version of the given uuencoded data

long uueDecode(char **dst, string data)
{
	int state;
	string final("");
	long size = 0;
	
	*dst = (char *)malloc(data.length());

	vector<string> lines;

	// First, split the data up into a group of lines
	lines = strsplit(data, '\n');

	// Now that we have that, let's hack
	state = 0;

	for (unsigned int i = 0; i < lines.size(); i++)
	{
//		string line = strchomp(lines[i]);
		string line = lines[i];
		int j = 0;

		if (state == 0)
		{
			if (!strncasecmp(line.c_str(), "begin ", 6))
			{
				// Looks like we hit a begin block.  We're all good to begin decoding.
				state = 1;
			}
		}
		else if (state == 1)
		{
			if (!line.compare("end"))
			{
				// We're finished.
				(*dst)[size] = 0;
				return size;
			}

			// If we're here then we should be decoding.  Our first step is to check the
			// length of the line.

			int len = ((line[j] - ' ') & 077);

			for (++j; len > 0; j += 4, len -= 3)
			{
				char c;
				
				if (len >= 3)
				{
					// Now, we should convert those 4 groups of 6 bits into
					// three groups of 8bits.

					c = ((line[j] - ' ') & 077) << 2 | ((line[j+1] - ' ') & 077) >> 4;
					(*dst)[size++] = c;
					
					c = ((line[j+1] - ' ') & 077) << 4 | ((line[j+2] - ' ') & 077) >> 2;
					(*dst)[size++] = c;

					c = ((line[j+2] - ' ') & 077) << 6 | ((line[j+3] - ' ') & 077);
					(*dst)[size++] = c;

				}
				else
				{
					// If we get here, then our line is a funky length.
					c = ((line[j] - ' ') & 077) << 2 | ((line[j+1] - ' ') & 077) >> 4;
					(*dst)[size++] = c;

					if (len == 2)
					{
						// If we have another one left over, get it too
						c = ((line[j+1] - ' ') & 077) << 4 | ((line[j+2] - ' ') & 077) >> 2;
						(*dst)[size++] = c;
					}
				}
			}
		}


	}

	(*dst)[size] = 0;
	return size;
}

int is_date_delim(char c)
{
		switch (c)
		{
			case ',':
			case '-':
			case '+':
			case '/':
			case ';':
			case '(':
			case ')':
			case '[':
			case ']':
			case ' ':
			case '\t':
				return 1;
				break;
			default:
				return 0;
				break;
		}
}

// The parsedate() method takes a date in char* format and parses it into
// a struct tm *.  The date can be in any valid date format.  

struct tm *parsedate(const char *date)
{
    int month = -1;
    int day = -1;
    int year = -1;
    int hour = -1;
    int min = -1;
    int sec = -1;

    char *lc = strdup(date);

    for (unsigned int i = 0; i < strlen(lc); i++)
	lc[i] = tolower(lc[i]);

    const char *str = lc;

    // FIXME: Yes, things are about to get REALLY messy.  I want to go back and rewrite
    // some of this.  Unfortunately, it was 3am when I first did this so, it might not
    // mkae a lot of sense.  

    while (*str)
    {
	switch (*str)
	{
	case 'a':
	    if (month < 0)
	    {
		if ((str[1] == 'p') && (str[2] == 'r'))
		    month = 4;
		else if ((str[1] == 'u') && (str[2] == 'g'))
		    month = 8;
	    }
	    break;
	case 'd':
	    if (month < 0)
	    {
		if ((str[1] == 'e') && (str[2] == 'c'))
		    month = 12;
	    }
	    break;
	case 'f':
	    if (month < 0)
	    {
		if ((str[1] == 'e') && (str[2] == 'b'))
		    month = 2;
	    }
	    break;
	case 'j':
	    if (month < 0)
	    {
		if ((str[1] == 'a') && (str[2] == 'n'))
		    month = 1;
		else if ((str[1] == 'u') && (str[2] == 'l'))
		    month = 7;
		else if ((str[1] == 'u') && (str[2] == 'n'))
		    month = 6;
	    }
	    break;
	case 'm':
	    if (month < 0)
	    {
		if ((str[1] == 'a') && (str[2] == 'r'))
		    month = 3;
		else if ((str[1] == 'a') && (str[2] == 'y'))
		    month = 5;
	    }
	    break;
	case 'n':
	    if (month < 0)
	    {
		if ((str[1] == 'o') && (str[2] == 'v'))
		    month = 11;
	    }
	    break;
	case 'o':
	    if (month < 0)
	    {
		if ((str[1] == 'c') && (str[2] == 't'))
		    month = 10;
	    }
	    break;
	case 's':
	    if (month < 0)
	    {
		if ((str[1] == 'e') && (str[2] == 'p'))
		    month = 9;
	    }
	    break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	    int th = -1;
	    int tm = -1;
	    int ts = -1;
	    const char *tmp = str + 1;

	    // Skip across all of the numbers until we reach
	    // something else

	    while (*tmp >= '0' && *tmp <= '9')
		tmp++;

	    // Check to see if we have any slashes or dashes.  If so, we have
	    // a date.  

	    if (*tmp == '/' || *tmp == '-')
	    {
		// Make sure we have a valid number in there.  I could have put
		// this on the line above but it looke too messy. Also, if we have
		// already found a month, we should abort.

		if ((*tmp <= '0' && *tmp >= '9') || month > -1)
		    break;

		int x, y, z;
		const char *tmp2 = str;

		// Get the first digit
		x = (*tmp2++ - '0');

		// If we have two digits, get the second one
		if (*tmp2 >= '0' && *tmp2 <= '9')
		{
		    x *= 10;
		    x += (*tmp2++ - '0');
		}
		// Note, we've read what should be a month or a day or a year.
		// If we dont find a date delimiter next then we obviously do not
		// have a date.  If everything is ok, move to the next digit.

		if (*tmp2 != '/' && *tmp2 != '-')
		    break;

		tmp2++;

		// If there's no more numbers, bail out.
		if (*tmp2 < ' ' || *tmp2 > '9')
		    break;

		// Get the first digit
		y = (*tmp2++ - '0');

		// If we have two digits, get the second one
		if (*tmp2 >= '0' && *tmp2 <= '9')
		{
		    y *= 10;
		    y += (*tmp2++ - '0');
		}
		// Note, we've read what should be a month or a day or a year.
		// If we dont find a date delimiter next then we obviously do not
		// have a date.

		if (*tmp2 != '/' && *tmp2 != '-')
		    break;

		tmp2++;

		// If there's no more numbers, bail out.
		if (*tmp2 < ' ' || *tmp2 > '9')
		    break;

		// Get the first digit
		z = (*tmp2++ - '0');

		// If we have two digits, get the second one
		if (*tmp2 >= '0' && *tmp2 <= '9')
		{
		    z *= 10;
		    z += (*tmp2++ - '0');
		}
		// Now, the interesting thing here is, we could have four
		// digits or two digits.  Remember, a year can be either 2001
		// or simply 01.

		if (*tmp2 >= '0' && *tmp2 <= '9')
		{
		    z *= 10;
		    z += (*tmp2++ - '0');

		    // If there's no more numbers, bail out.  We _have_ to have
		    // four digits.
		    if (*tmp2 < ' ' || *tmp2 > '9')
			break;

		    z *= 10;
		    z += (*tmp2++ - '0');
		}
		// If any more numbers or words follow then we obviously
		// don't have a date.

		if ((*tmp >= '0' && *tmp <= '9') ||
		    (*tmp >= 'a' && *tmp <= 'z') ||
		    (*tmp >= 'A' && *tmp <= 'Z'))
		    break;

		// Now an even more fun part.  We have to determine what
		// order our month, day, and year are in.

		// If  the first number is very big, its gotta be the year. 
		// This means y is our month and z is our day.

		if (x < 1 || x > 31)
		{
		    // If the next line happens, then it must not be a date afterall.
		    if (y > 12 || z > 31)
			break;

		    year = x;
		    month = y;
		    day = z;

		    str = tmp2;	// Again, dont forget to update this.

		    break;
		}
		// Well, we've either got mm/dd or dd/mm.  Check to see if
		// they're valid.

		if (x > 12 && y > 12)
		{
		    // This can't happen
		    str = tmp2;
		    break;
		}
		// If x > 12 then x must be our day.
		if (x > 12)
		{
		    day = x;
		    month = y;
		    year = z;
		} else
		{
		    month = x;
		    day = y;
		    year = z;
		}

		str = tmp2;
	    }
	    // Now, if what we find is a colon, we're probably dealing with
	    // time.  That, or we received something really weird.

	    else if (*tmp == ':')
	    {
		// If we have an hour and a minute then we've already
		// parsed some time.  We should just jump out of the
		// switch statement.

		if (hour > -1 && min > -1)
		    break;

		// We've found at least one digit.  If we've found any more
		// than two, we've got trouble. 

		if ((tmp - str) > 2)
		    break;

		// Otherwise, we should just parse the data
		if ((tmp - str) == 2)
		{
		    th = (str[0] - '0') * 10;
		    th += (str[1] - '0');
		} else
		    th = (str[0] - '0');


		// Now, we've parsed our hour.  We should move on to 
		// the next key.  This should be our minute.

		while (*str && *str != ':')
		    str++;

		str++;		// Skip over that last colon

		tmp = str + 1;

		while (*tmp >= '0' && *tmp <= '9')
		    tmp++;

		// Now we have to see if we had any digits at all.  If we didn't then
		// we have issues.  Also, if we have more than two digits, lets
		// abort.

		if ((tmp == str) || ((tmp - str) > 2))
		    break;

		if ((tmp - str) == 2)
		{
		    tm = (str[0] - '0') * 10;
		    tm += (str[1] - '0');
		} else
		    tm = (str[0] - '0');

		// Jump to the seconds, if we have it.
		str = tmp;

		// If this happens, then we had some seconds.
		if (*str == ':')
		    str++;

		tmp = str;

		while (*tmp >= '0' && *tmp <= '9')
		    tmp++;

		if ((tmp == str) || ((tmp - str) > 2))
		    break;

		if ((tmp - str) == 2)
		{
		    ts = (str[0] - '0') * 10;
		    ts += (str[1] - '0');
		} else
		    ts = (str[0] - '0');

		// Well, we've read hours, minutes and, if they exist, seconds.

		hour = th;
		min = tm;
		sec = ts;
		str = tmp;	// Don't forget to increment our pointer
		break;

	    }
	    // Otherwise, we don't have :, / or -.  We should use the
	    // old goat brain to figure this one out.

	    // If we have four digits, it must be a year.
	    else if ((tmp - str) == 4)
	    {
		if (year > 0)
		    break;

		year = (str[0] - '0') * 1000;
		year += (str[1] - '0') * 100;
		year += (str[2] - '0') * 10;
		year += (str[3] - '0');
	    }
	    // Otherwise, this could be day or a year.

	    else if ((tmp - str) == 2)
	    {
		int x;

		x = (str[0] - '0') * 10;
		x += (str[1] - '0');

		// If we don't have a day, this must be the day.  Also,
		// we should make sure its a valid possibility.

		if (day < 0 && x < 32)
		    day = x;
		else
		{
		    // Otherwise, it has to be the year.
		    year = x;
		}
	    }
	    // And if we only find one digit, it must be the date. 

	    else if ((tmp - str) == 1 && day < 0)
		day = str[0] - '0';

	    // Get the heck out of dodge!
	    break;
	}



	// Well, now that we've done a loop through this, we don't need
	// the previous text.  We'll skip to our next delimiter.

	while (*str && !is_date_delim(*str))
	    str++;

	// Now that we've hit a delimiter, let's kill ALL of our delimiters until
	// we reach something else.  This will help us eliminate white space and
	// other such goodies.

	while (*str && is_date_delim(*str)) 
	    str++;

    }


    // Let's make sure our year is in the right four digit format

    if (year < 50)
	year += 2000;
    else if (year < 100)
	year += 1900;

    free(lc);

    // Now we should generate a nice little time structure

    struct tm *newtime = new struct tm;

    // Set our time
    newtime->tm_sec = sec;
    newtime->tm_min = min;
    newtime->tm_hour = hour;

    // And our date
    newtime->tm_mday = day;
    newtime->tm_mon = month - 1;
    newtime->tm_year = year - 1900;

    return newtime;
}

// The decodBase64() method takes in char ** and a string of text and returns the size of the 
// newly decoded data.  The dst char ** will contain the raw data.

long decodeBase64(char **dst, string data)
{
	long count = 0;
	
	*dst = (char *)malloc(data.length());

	vector<string> lines;

	// First, split the data up into a group of lines
	lines = strsplit(data, '\n');

	for (unsigned int i = 0; i < lines.size(); i++)
	{
		unsigned int j, k, ec;
		unsigned long c = 0;
		
		string line = lines[i];
		string token("");
	
		for (j = 0, k = 0; j < line.length(); j++)
		{
			token += line[j];
			k++;
			
			if (k == 4)
			{
				// Looks like we have a token! Hooray!
				
				ec = 0;
				
				for (int l = 0; l < 4; l++)
				{
					unsigned char nc = 0;
					
					if (token[l] >= 'A' && token[l] <= 'Z')
						nc = token[l] - 'A';
					else if (token[l] >= 'a' && token[l] <= 'z')
						nc = token[l] - ('a' - 26);
					else if (token[l] >= '0' && token[l] <= '9')
						nc = token[l] - ('0' - 52);
					else if (token[l] == '+')
						nc = 62;
					else if (token[l] == '/')
						nc = 63;
					else if (token[l] == '=')
					{
						nc = 0;
						ec++;
					 }

					// Shift the bits back around to where they go
					c = (c << 6) | nc;
				}

				// Now that we're finished shifting things around, let's unpack this
				// booger.

				(*dst)[count++] = (char)(c >> 16);
				(*dst)[count++] = (char)((c >> 8) & 0xFF);
				(*dst)[count++] = (char)(c & 0xFF);

				token = "";
				k = 0;
				ec = 0;
			}

		}

	}
	(*dst)[count] = 0;
	return count;
}

string encodeBase64(char *text, unsigned long size)
{
	char *c;
	unsigned int n = 0;
	char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	string bb("");
	c = text;
	long long x  = size;
	int count = 0;
	unsigned char tbuf[3];
	unsigned char ebuf[3];

	while (x >= 0) {

		bzero(tbuf, sizeof(tbuf));
		
		// We wanna read three bytes (24 bit) at a time.
		for (n = 0; n < 3; n++)
		{
			x--;
			if (x < 0) break;
			tbuf[n] = *c++;
		}

		if (n)
		{
			// If we have anything to encode, do it to it baby.
			ebuf[0] = alphabet[tbuf[0] >> 2];
			ebuf[1] = alphabet[((tbuf[0] & 3) << 4) | (tbuf[1] >> 4)];
			ebuf[2] = alphabet[((tbuf[1] & 0xF) << 2) | (tbuf[2] >> 6)];
			ebuf[3] = alphabet[tbuf[2] & 0x3F];

			// Well, we did some things .. so, let's check to see if
			// we need to pad stuff.

			if (n < 3)
				ebuf[3] = '=';

			if (n < 2)
				ebuf[2] = '=';

			if (n < 1)
				ebuf[1] = '=';


			// Build our output
			for (n = 0; n <= sizeof(ebuf); n++)
			{
				bb += ebuf[n];
				count++;

				// If it's too long, break
				if (count == 72)
				{
					// And reset the counter
					count = 0;
					bb += '\n';
				}
			}
		}
	}

	return bb;
}

