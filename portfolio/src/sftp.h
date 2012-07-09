#ifndef SFTP_H
#define SFTP_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>

#include "libssh2_config.h"
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <stdio.h>
#include <QObject>
#include "includes.h"

/*
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ARPA_INET_H
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
*/

class Sftp
{

public:
    Sftp();

    bool open(QString host, QString username, QString password);
    void disconnect();
    bool downloadFile(QString serverFile, QString outputFile);
    QStringList getListOfHandouts(QString handsoutDirectory);
    void setTransfersEnabled(bool value);
    QString downloadFileContent(QString serverFile);

private:

    LIBSSH2_SFTP_HANDLE *sftp_handle;
    LIBSSH2_SESSION *session;
    LIBSSH2_SFTP *sftp_session;
    int rc;
    int sock;
    bool transfersEnabled;
};

#endif // SFTP_H
