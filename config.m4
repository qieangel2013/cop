dnl $Id$
dnl config.m4 for extension cop

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(cop,cop support,
[  --with-cop             Include cop support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(cop,enable cop support,
[  --enable-cop           Enable cop support])

if test "$PHP_COP" != "no"; then
  PHP_REQUIRE_CXX()
  PHP_SUBST(COP_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, COP_SHARED_LIBADD)

  dnl # --with-cop -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/cop.h"  # you most likely want to change this
  dnl if test -r $PHP_COP/$SEARCH_FOR; then # path given as parameter
  dnl   COP_DIR=$PHP_COP
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for cop files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       COP_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$COP_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the cop distribution])
  dnl fi

  dnl # --with-cop -> add include path
  dnl PHP_ADD_INCLUDE($COP_DIR/include)

  dnl # --with-cop -> check for lib and symbol presence
  dnl LIBNAME=cop # you may want to change this
  dnl LIBSYMBOL=cop # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $COP_DIR/$PHP_LIBDIR, COP_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_COPLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong cop lib version or lib not found])
  dnl ],[
  dnl   -L$COP_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  

  PHP_NEW_EXTENSION(cop,cop.cpp lib/co_epoll.cpp lib/co_hook_sys_call.cpp lib/co_routine.cpp lib/coctx_swap.S lib/coctx.cpp, $ext_shared)
fi
