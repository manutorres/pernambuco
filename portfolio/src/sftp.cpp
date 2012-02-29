#include "sftp.h"

Sftp::Sftp()
{
}

bool Sftp::open(const char *host, const char *username, const char *password){

    unsigned long hostaddr;
    struct sockaddr_in sin;

    #ifdef WIN32
        WSADATA wsadata;
        WSAStartup(MAKEWORD(2,0), &wsadata);
    #endif

    hostaddr =  inet_addr(host);

    rc = libssh2_init (0);

    if (rc != 0) {
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
        return 1;
    }

    /*
     * The application code is responsible for creating the socket
     * and establishing the connection
     */
    sock = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = hostaddr;
    if (connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "failed to connect!\n");
        return -1;
    }

    /* Create a session instance */
    session = libssh2_session_init();

    if(!session)
        return false;

    /* Since we have set non-blocking, tell libssh2 we are blocking */
    libssh2_session_set_blocking(session, 1);


    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    rc = libssh2_session_handshake(session, sock);

    if(rc) {
        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
        return false;
    }

    /* At this point we havn't yet authenticated.  The first thing to do
     * is check the hostkey's fingerprint against our known hosts Your app
     * may have it hard coded, may go to a file, may present it to the
     * user, that's your call
     */

    if (libssh2_userauth_password(session, username, password)) {

        fprintf(stderr, "Authentication by password failed.\n");
        return false;
    }

    fprintf(stderr, "libssh2_sftp_init()!\n");

    sftp_session = libssh2_sftp_init(session);

    if (!sftp_session) {
        fprintf(stderr, "Unable to init SFTP session\n");
    }

    fprintf(stderr, "libssh2_sftp_open()!\n");

    return true;
}

bool Sftp::downloadFile(QString serverFile, QString outputFile){

    /*Request a file via SFTP */
    QByteArray data = "";
    FILE *fd;
    fd = fopen(outputFile.toStdString().data(),"w");

    sftp_handle = libssh2_sftp_open(sftp_session, serverFile.toStdString().data(), LIBSSH2_FXF_READ, 0);

    if (!sftp_handle) {

        fprintf(stderr, "Unable to open file with SFTP: %ld\n", libssh2_sftp_last_error(sftp_session));
        return false;
    }

    fprintf(stderr, "libssh2_sftp_open() is done, now receive data!\n");

    do {
        char mem[131072];

        //loop until we fail
        //fprintf(stderr, "libssh2_sftp_read()!\n");

        rc = libssh2_sftp_read(sftp_handle, mem, sizeof(mem));

        if (rc > 0) {
            fwrite(mem,1,rc,fd);
        } else {
            break;
        }
    } while (1);

    fclose(fd);

   // libssh2_sftp_close(sftp_handle);

    //libssh2_sftp_shutdown(sftp_session);

    return true;
}

void Sftp::disconnect(){

    libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
    libssh2_session_free(session);

    #ifdef WIN32
        closesocket(sock);
    #else
        close(sock);
    #endif

    libssh2_exit();
}
