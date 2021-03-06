dnl
dnl $Id: pkg.m4 2663 2009-04-18 14:22:06Z karijes $
dnl
dnl Modified a little bit so returned values does not have ending spaces. (Sanel)
dnl Note: if you do not set 'action-not', if package not found, it will stop configure
dnl script with "Library XY requirements not met..." message
dnl	
dnl PKG_CHECK_MODULES(GSTUFF, gtk+-2.0 >= 1.3 glib = 1.3.4, action-if, action-not)
dnl defines GSTUFF_LIBS, GSTUFF_CFLAGS, see pkg-config man page
dnl also defines GSTUFF_PKG_ERRORS on error
AC_DEFUN([PKG_CHECK_MODULES], [
  succeeded=no

  if test -z "$PKG_CONFIG"; then
    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  fi

  if test "$PKG_CONFIG" = "no" ; then
     echo "*** The pkg-config script could not be found. Make sure it is"
     echo "*** in your path, or set the PKG_CONFIG environment variable"
     echo "*** to the full path to pkg-config."
     echo "*** Or see http://www.freedesktop.org/software/pkgconfig to get pkg-config."
  else
     PKG_CONFIG_MIN_VERSION=0.9.0
     if $PKG_CONFIG --atleast-pkgconfig-version $PKG_CONFIG_MIN_VERSION; then
        AC_MSG_CHECKING(for $2)

        if $PKG_CONFIG --exists "$2" ; then
            AC_MSG_RESULT(yes)
            succeeded=yes

            AC_MSG_CHECKING($1_CFLAGS)
            $1_CFLAGS=`$PKG_CONFIG --cflags "$2"`
            AC_MSG_RESULT($$1_CFLAGS)

            AC_MSG_CHECKING($1_LIBS)
            ## don't use --libs since that can do evil things like add -Wl,--export-dynamic
            if test "$SHELL" = "/bin/ksh"; then
                ## ksh does not do escaping properly
                $1_LIBS="`$PKG_CONFIG --libs-only-L "$2"` `$PKG_CONFIG --libs-only-l "$2"`"
            else
                $1_LIBS="`$PKG_CONFIG --libs-only-L \"$2\"` `$PKG_CONFIG --libs-only-l \"$2\"`"
            fi

            $1_VERSION=`$PKG_CONFIG --modversion "$2"`
            AC_MSG_RESULT($$1_LIBS)
        else
            $1_CFLAGS=""
            $1_LIBS=""
			$1_VERSION=""
            ## If we have a custom action on failure, don't print errors, but 
            ## do set a variable so people can do so.
            $1_PKG_ERRORS=`$PKG_CONFIG --errors-to-stdout --print-errors "$2"`
            ifelse([$4], ,echo $$1_PKG_ERRORS,)
        fi

        dnl remove ending spaces so jam variables could be accessed nicely
        $1_CFLAGS=`echo $$1_CFLAGS | sed 's/[ ]*$//g'`
        $1_LIBS=`echo $$1_LIBS | sed 's/[ ]*$//g'`

        AC_SUBST($1_CFLAGS)
        AC_SUBST($1_LIBS)
        AC_SUBST($1_VERSION)
     else
        echo "*** Your version of pkg-config is too old. You need version $PKG_CONFIG_MIN_VERSION or newer."
        echo "*** See http://www.freedesktop.org/software/pkgconfig"
     fi
  fi

  if test $succeeded = yes; then
     ifelse([$3], , :, [$3])
  else
     ifelse([$4], , AC_MSG_ERROR([Library requirements ($2) not met; consider adjusting the PKG_CONFIG_PATH environment variable if your libraries are in a nonstandard prefix so pkg-config can find them.]), [$4])
  fi
])
