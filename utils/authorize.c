#include<stdio.h>
#include<pwd.h>
#include<sys/types.h>
#include<shadow.h>
#include<string.h>
#define _XOPEN_SOURCE
#include <unistd.h>

int main(int argc, char ** argv)
{
	struct passwd * pw;
	struct passwd * my_pw;
	struct spwd * spw;
	char * encrypted, * unencrypted, *correct;
	char *saveit;

	if( argc < 4 )
	{
		return 1;
	}

	saveit = strdup(argv[2]);	

	unencrypted = strdup( argv[2] );
	memset( argv[2], 0, strlen(argv[2]) );

	pw = getpwnam( argv[1] );
	if(!pw)
	{
		return -1;
	}
	spw = getspnam( argv[1] );
	if( spw )
	{
		correct = spw->sp_pwdp;
	}
	else
	{
		correct = pw->pw_passwd;
	}
	encrypted = (char*)crypt( unencrypted, correct );
	memset( unencrypted, 0, strlen(unencrypted) );
	free(unencrypted);
	
	my_pw = getpwuid(getuid());

	/* FIXME: We don't need this anymore.  This was a quick hack.  This
	 * is only for documentation purposes */

	/*
	if( strcmp( encrypted, correct ) == 0 || !strcmp(my_pw->pw_name, "root") || (database_lookup_pinnum(database_lookup_uid_by_username(argv[1]), saveit))==1)
	*/

	if( strcmp( encrypted, correct ) == 0 || !strcmp(my_pw->pw_name, "root")==1)
	{
		if(setreuid(0,0))
		{
			perror("setuid");
		}
		setregid(0,0);
		setgid(pw->pw_gid);
		if(setuid(pw->pw_uid))
		{
			perror("setuid");
		}
		execv( argv[3], &argv[3] );
	}
	else
	{
		return -1;
	}
	
	return 0;
}

