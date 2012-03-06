#ifndef INCLUDES_H
#define INCLUDES_H
#include "libssh2_config.h"
#include <libssh2.h>
#include <libssh2_sftp.h>

#ifdef Q_WS_WIN
#include <winsock2.h>
#endif

#ifdef Q_OS_UNIX
#include <sys/socket.h>
#endif

#ifdef Q_OS_UNIX
#include <netinet/in.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef Q_OS_UNIX
#include <arpa/inet.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#endif // INCLUDES_H
