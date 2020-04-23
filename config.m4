PHP_ARG_ENABLE(rocksdb, rocksdb support,
[  --enable-rocksdb           Enable rocksdb support], [enable_rocksdb="yes"])

PHP_ARG_ENABLE(asan, enable asan,
[  --enable-asan             Enable asan], no, no)

PHP_ARG_ENABLE(gcov, enable gcov,
[  --enable-gcov             Enable gcov], no, no)

AC_DEFUN([ROCKSDB_HAVE_PHP_EXT], [
    extname=$1
    haveext=$[PHP_]translit($1,a-z_-,A-Z__)

    AC_MSG_CHECKING([for ext/$extname support])
    if test -x "$PHP_EXECUTABLE"; then
        grepext=`$PHP_EXECUTABLE -m | $EGREP ^$extname\$`
        if test "$grepext" = "$extname"; then
            [PHP_HTTP_HAVE_EXT_]translit($1,a-z_-,A-Z__)=1
            AC_MSG_RESULT([yes])
            $2
        else
            [PHP_HTTP_HAVE_EXT_]translit($1,a-z_-,A-Z__)=
            AC_MSG_RESULT([no])
            $3
        fi
    elif test "$haveext" != "no" && test "x$haveext" != "x"; then
        [PHP_HTTP_HAVE_EXT_]translit($1,a-z_-,A-Z__)=1
        AC_MSG_RESULT([yes])
        $2
    else
        [PHP_HTTP_HAVE_EXT_]translit($1,a-z_-,A-Z__)=
        AC_MSG_RESULT([no])
        $3
    fi
])

AC_MSG_CHECKING([if compiling with clang])
AC_COMPILE_IFELSE([
    AC_LANG_PROGRAM([], [[
        #ifndef __clang__
            not clang
        #endif
    ]])],
    [CLANG=yes], [CLANG=no]
)
AC_MSG_RESULT([$CLANG])

if test "$CLANG" = "yes"; then
    CFLAGS="$CFLAGS -std=gnu89"
fi

AC_CANONICAL_HOST

if test "$PHP_ROCKSDB" != "no"; then

    PHP_ADD_LIBRARY(pthread)
    PHP_SUBST(ROCKSDB_SHARED_LIBADD)

    AC_ARG_ENABLE(debug,
        [  --enable-debug,         compile with debug symbols],
        [PHP_DEBUG=$enableval],
        [PHP_DEBUG=0]
    )

    if test "$PHP_DEBUG_LOG" != "no"; then
        AC_DEFINE(ROCKSDB_DEBUG, 1, [do we enable rocksdb debug])
        PHP_DEBUG=1
    fi

    if test "$PHP_ASAN" != "no"; then
        PHP_DEBUG=1
        CFLAGS="$CFLAGS -fsanitize=address -fno-omit-frame-pointer"
        CXXFLAGS="$CXXFLAGS -fsanitize=address -fno-omit-frame-pointer"
    fi
    
    if test "$PHP_GCOV" != "no"; then
        PHP_DEBUG=1
        CFLAGS="$CFLAGS -fprofile-arcs -ftest-coverage"
        CXXFLAGS="$CXXFLAGS -fprofile-arcs -ftest-coverage"
    fi

    AS_CASE([$host_os],
      [darwin*], [ROCKSDB_OS="MAC"],
      [cygwin*], [ROCKSDB_OS="CYGWIN"],
      [mingw*], [ROCKSDB_OS="MINGW"],
      [linux*], [ROCKSDB_OS="LINUX"],
      []
    )

    CFLAGS="-Wall -pthread $CFLAGS"
    LDFLAGS="$LDFLAGS -lpthread"

    if test "$ROCKSDB_OS" = "MAC"; then
        AC_CHECK_LIB(c, clock_gettime, AC_DEFINE(HAVE_CLOCK_GETTIME, 1, [have clock_gettime]))
    else
        AC_CHECK_LIB(rt, clock_gettime, AC_DEFINE(HAVE_CLOCK_GETTIME, 1, [have clock_gettime]))
        PHP_ADD_LIBRARY(rt, 1, ROCKSDB_SHARED_LIBADD)
    fi
    if test "$ROCKSDB_OS" = "LINUX"; then
        LDFLAGS="$LDFLAGS -z now"
    fi

    PHP_ADD_LIBRARY_WITH_PATH(rocksdb, /usr/local/lib/, ROCKSDB_SHARED_LIBADD)

    PHP_ADD_LIBRARY(pthread, 1, ROCKSDB_SHARED_LIBADD)

    rocksdb_source_file=" \
        rocksdb_db.cc \
        rocksdb_options.cc \
        rocksdb_transaction_db.cc \
        rocksdb_exception.cc \
        rocksdb_iterator.cc \
        rocksdb_write_batch.cc
    "

    PHP_NEW_EXTENSION(rocksdb, $rocksdb_source_file, $ext_shared,,, cxx)

    PHP_ADD_INCLUDE([$ext_srcdir])
    PHP_ADD_INCLUDE([$ext_srcdir/include])

    PHP_INSTALL_HEADERS([ext/rocksdb], [*.h config.h include/*.h])

    PHP_REQUIRE_CXX()

    CXXFLAGS="$CXXFLAGS -Wall -Wno-unused-function -Wno-deprecated -Wno-deprecated-declarations"

    if test "$ROCKSDB_OS" = "CYGWIN" || test "$ROCKSDB_OS" = "MINGW"; then
        CXXFLAGS="$CXXFLAGS -std=gnu++11"
    else
        CXXFLAGS="$CXXFLAGS -std=c++11"
    fi
fi
