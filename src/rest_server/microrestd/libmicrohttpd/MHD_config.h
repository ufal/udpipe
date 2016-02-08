// Compile-time configuration version by Milan Straka <straka@ufal.mff.cuni.cz>

// 1) Detect platform
// One of LINUX, OSX, WINDOWS (no CYGWIN), CYGWIN, OTHEROS

#ifdef __CYGWIN__
# define CYGWIN 1
#elif defined(_WIN32)
# define WINDOWS 1
#elif defined(__APPLE__)
# define OSX 1
#elif defined(__linux__)
# define LINUX 1
#else
# define OTHEROS 1
#endif

// 2) Platform-specific headers
#ifdef WINDOWS
# define HAVE_WINSOCK2_H 1 /* Define to 1 if you have the <winsock2.h> header file. */
# define HAVE_WS2TCPIP_H 1 /* Define to 1 if you have the <ws2tcpip.h> header file. */
#else
# define HAVE_ARPA_INET_H 1 /* Define to 1 if you have the <arpa/inet.h> header file. */
# define HAVE_NETDB_H 1 /* Define to 1 if you have the <netdb.h> header file. */
# define HAVE_NETINET_IN_H 1 /* Define to 1 if you have the <netinet/in.h> header file. */
# define HAVE_NETINET_TCP_H 1 /* Define to 1 if you have the <netinet/tcp.h> header file. */
# define HAVE_POLL_H 1 /* Define to 1 if you have the <poll.h> header file. */
# define HAVE_PTHREAD_H 1 /* Define to 1 if you have the <pthread.h> header file. */
# define HAVE_SYS_MMAN_H 1 /* Define to 1 if you have the <sys/mman.h> header file. */
# define HAVE_SYS_SELECT_H 1 /* Define to 1 if you have the <sys/select.h> header file. */
# define HAVE_SYS_SOCKET_H 1 /* Define to 1 if you have the <sys/socket.h> header file. */
# define HAVE_SYS_STAT_H 1 /* Define to 1 if you have the <sys/stat.h> header file. */
# define HAVE_SYS_TIME_H 1 /* Define to 1 if you have the <sys/time.h> header file. */
# define HAVE_UNISTD_H 1 /* Define to 1 if you have the <unistd.h> header file. */
#endif

// 3) Platform-specific settings
#ifdef WINDOWS
# define MHD_USE_W32_THREADS 1 /* define to use W32 threads */
# define MHD_DONT_USE_PIPES 1 /* Define to use pair of sockets instead of pipes for signaling */
#else
# define MHD_USE_POSIX_THREADS 1 /* define to use pthreads */
#endif

#ifdef LINUX
# define HAVE_DECL_TCP_CORK 1 /* Define to 1 if you have the declaration of `TCP_CORK', and to 0 if you don't. */
# define HAVE_LISTEN_SHUTDOWN 1 /* can use shutdown on listen sockets */
#endif

// 4) Generic settings

#define _MHD_EXTERN /* defines how to decorate public symbols while building */
// #undef BAUTH_SUPPORT /* disable basic Auth support */
// #undef DAUTH_SUPPORT /* disable digest Auth support */
#define EPOLL_SUPPORT 0 /* define to 0 to disable epoll support */
#define HAVE_ACCEPT4 0 /* Define to 1 if you have the `accept4' function. */
// #undef HAVE_CLOCK_GETTIME /* Have clock_gettime */
#define HAVE_INET6 1 /* Provides IPv6 headers */
#define HAVE_MEMORY_H 0 /* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MESSAGES 1 /* Disable error messages */
#define HAVE_POSTPROCESSOR 1 /* define to 1 if MHD was build with postprocessor.c */
// #undef HAVE_PTHREAD_SETNAME_NP /* Define if you have pthread_setname_np function. */
#define HAVE_SEARCH_H 0 /* Define to 1 if you have the <search.h> header file. */
#define HAVE_SYS_MSG_H 0 /* Define to 1 if you have the <sys/msg.h> header file. */
#define HAVE_SYS_TYPES_H 1 /* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_TIME_H 1 /* Define to 1 if you have the <time.h> header file. */
#define HTTPS_SUPPORT 0 /* disable HTTPS support */

// 5) Package information

#define PACKAGE "libmicrohttpd" /* Name of package */
#define PACKAGE_BUGREPORT "libmicrohttpd@gnu.org" /* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_NAME "libmicrohttpd" /* Define to the full name of this package. */
#define PACKAGE_STRING "libmicrohttpd 0.9.39" /* Define to the full name and version of this package. */
#define PACKAGE_TARNAME "libmicrohttpd" /* Define to the one symbol short name of this package. */
#define PACKAGE_URL "" /* Define to the home page for this package. */
#define PACKAGE_VERSION "0.9.39" /* Define to the version of this package. */
#define VERSION "0.9.39" /* Version number of package */
