dnl
dnl This module will check for the existence of omniORB3 and other important
dnl libraries associated with it.  This code is based on code found in omniorb.m4
dnl found in the Berlin project (http://www.sourceforge.net/projects/berlin)
dnl
dnl Rob Flynn <rob@tgflinux.com> TGF Linux Communications
dnl


AC_DEFUN([TUCAN_CHECK_LIB],[

        save_LIBS="$LIBS"
        LIBS="$LIBS $$1 -l$2 -D__x86__"
        changequote(`, ')
        includes="patsubst(`$4', `\(\w\|\.\|/\)+', `#include <\&>
')"

        changequote([, ])
        AC_CACHE_CHECK("for -l$2",
                tucan_cv_lib_$2,
                AC_TRY_LINK($includes,
                        $3, tucan_cv_lib_$2="yes", tucan_cv_lib_$2="no"))

        LIBS="$save_LIBS"
        if test ".$tucan_cv_lib_$2" = ".yes" ; then
                $1="$$1 -l$2"
        fi
])


AC_DEFUN([OMNIORB_CHECK],[

        AC_REQUIRE([AC_PROG_CC])
        AC_REQUIRE([AC_PROG_CXX])
        AC_REQUIRE([AC_PROG_CPP])
        AC_REQUIRE([AC_PROG_CXXCPP])

        AC_LANG_SAVE
        AC_LANG_CPLUSPLUS

        AC_ARG_WITH(omniorb-prefix,
                [  --with-omniorb-prefix  Prefix for omniORB],[
                omniorb_prefix="$withval"])

        dnl Check for omniidl.
        if test ".$omniorb_prefix" != "." ; then
                omniorb_path="$omniorb_prefix/bin:$PATH"
                ORB_LIBS="-L$omniorb_prefix/lib"
                ORB_CPPFLAGS="-I$omniorb_prefix/include"
        else
                omniorb_path="$PATH"
        fi
        AC_PATH_PROG(IDLCXX, omniidl, no, $omniorb_path)
        if test ".$IDLCXX" = ".no" ; then
                no_omniorb="yes"
        fi

        save_CPPFLAGS="$CPPFLAGS"

        dnl Check for omniORB libraries
        if test ".$no_omniorb" = "." ; then
                ORB_LIBS="$ORB_LIBS -ltcpwrapGK"

                TUCAN_CHECK_LIB(ORB_LIBS, omniORB3, [CORBA::ORB_var orb],
                        omniORB3/CORBA.h)

                TUCAN_CHECK_LIB(ORB_LIBS, omnithread, [omni_mutex my_mutex],
                        omnithread.h)

                TUCAN_CHECK_LIB(ORB_LIBS, omniDynamic3, [CORBA::Any_var any;],
                        omniORB3/CORBA.h)

                if test ".$tucan_cv_lib_omniORB3" = ".no" \
                        -a ".$tucan_cv_lib_omniDynamic3" = ".no" \
                        -a ".$tucan_cv_lib_omnithread" = ".no" ; then
                        no_omniorb="yes"
                fi
                LIBS="$ORB_LIBS $LIBS"
        fi

        if test ".$no_omniorb" = "." ; then
                tucan_cv_lib_omniORB="yes"
		has_omniORB3="yes"
        fi

        AC_LANG_RESTORE
])
