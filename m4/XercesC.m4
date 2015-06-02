dnl
dnl Try to determine which libxerces the user has.  I really wish the developers
dnl of this library would not include the version number in the application.  They
dnl could at least do a symlink.  *sigh*
dnl
dnl Written by Rob Flynn <rob@tgflinux.com> at TGF Linux Communications
dnl

AC_DEFUN([XERCESC_CHECK],[

	dnl If we don't find it by natural means, let's seaarch
	dnl for it

	FOUND=yes

	AC_CHECK_LIB(xerces-c,main,LIBS="$LIBS -lxerces-c",FOUND=no)

	if test "$FOUND" = no ; then
	
		echo -n "searching for libxerces-c... "
	
		XERCES_LIBS=`for x in \`cat /etc/ld.so.conf\` ; do ls $x/libxerces* 2>&1 | sed 's/\// /g' | awk '{print $NF}' | sed 's/lib//' | sed 's/.so//' | grep xerces; done`
	
		if test "$XERCES_LIBS" = "" ; then
		        echo "not found"
		        AC_MSG_ERROR([
		        You do not have the xerces-c XML parser installed.  Please install this
		        library before continuing.])
		fi

		echo "found"

		AC_CHECK_LIB($XERCES_LIBS,main,LIBS="$LIBS -l$XERCES_LIBS",
		        AC_MSG_ERROR([
		        You do not have the xerces-c XML parser installed.  Please install this
		        library before continuing.]))

		AC_SUBST(XERCES_LIBS)
	fi

])
