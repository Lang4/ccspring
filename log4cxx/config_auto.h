/* include/log4cxx/config.h.  Generated from config.h.in by configure.  */
/* include/log4cxx/config.h.in.  Generated from configure.in by autoheader.  */

/* Define to 1 if you have the <boost/regex.hpp> header file. */
/* #undef HAVE_BOOST_REGEX_HPP */

/* SMTP support through Microsoft CDO. */
/* #undef HAVE_CDO_SMTP */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the `ftime' function. */
#define HAVE_FTIME 1

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <io.h> header file. */
/* #undef HAVE_IO_H */

/* ODBC support through iODBC. */
/* #undef HAVE_I_ODBC */

/* Define to 1 if you have the `boost_regex' library (-lboost_regex). */
/* #undef HAVE_LIBBOOST_REGEX */

/* SMTP support through libsmtp library. */
/* #undef HAVE_LIBSMTP */

/* Define to 1 if you have the `smtp_mime' library (-lsmtp_mime). */
/* #undef HAVE_LIBSMTP_MIME */

/* XML support through libxml2. */
#undef HAVE_LIBXML2 

/* Linux atomic operations support. */
/* #undef HAVE_LINUX_ATOMIC_OPERATIONS */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* ODBC support through Microsoft ODBC. */
/* #undef HAVE_MS_ODBC */

/* thread support through Microsoft threads. */
/* #undef HAVE_MS_THREAD */

/* ODBC support through Microsoft XML. */
/* #undef HAVE_MS_XML */

/* no explicit exports */
#define LOG4CXX_EXPORT

/* ODBC support */
/* #undef HAVE_ODBC */

/* thread support through pthread library. */
#define HAVE_PTHREAD 1

/* SMTP support */
/* #undef HAVE_SMTP */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* std::min & std::max */
#include <algorithm>
//#define _min std::min
//#define _max std::max
#define _min(x,y) ((x)>(y)?(y):(x))
#define _max(x,y) ((x)>(y)?(x):(y))

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* thread support */
#undef HAVE_THREAD 

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* ODBC support through unixODBC. */
/* #undef HAVE_UNIX_ODBC */

/* XML support */
#undef HAVE_XML 

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "log4cxx"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* UTF-16 Unicode support. */
/* #undef UNICODE */

/* Version number of package */
#define VERSION "0.9.5"
