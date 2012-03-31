#include "sftp.h"
#include <QDebug>

Sftp::Sftp(){}

//Crea una conexion ssh con el server especificado en host
bool Sftp::open(QString host, QString username, QString password){
    unsigned long hostaddr;
    struct sockaddr_in sin;

    #ifdef Q_WS_WIN
        WSADATA wsadata;
        WSAStartup(MAKEWORD(2,0), &wsadata);
    #endif

    //hostaddr =  inet_addr(host);
    hostaddr =  inet_addr(host.toStdString().data());

    this->rc = libssh2_init (0);

    if (this->rc != 0) {
        //fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
        return 1;
    }

    /*
     * The application code is responsible for creating the socket
     * and establishing the connection
     */
    this->sock = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = hostaddr;
    if (connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0) {
        //fprintf(stderr, "failed to connect!\n");
        return -1;
    }

    /* Create a session instance */
    this->session = libssh2_session_init();

    if(!this->session)
        return false;

    /* Since we have set non-blocking, tell libssh2 we are blocking */
    libssh2_session_set_blocking(this->session, 1);


    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    //this->rc = libssh2_session_handshake(this->session, this->sock);
    #ifdef Q_WS_WIN
        rc =libssh2_session_startup(session, sock);
    #else
        rc = libssh2_session_handshake(session, sock);
    #endif

    if(this->rc) {
        //fprintf(stderr, "Failure establishing SSH session: %d\n", this->rc);
        return false;
    }

    /* At this point we havn't yet authenticated.  The first thing to do
     * is check the hostkey's fingerprint against our known hosts Your app
     * may have it hard coded, may go to a file, may present it to the
     * user, that's your call
     */

    //if (libssh2_userauth_password(session, username, password)) {
    if (libssh2_userauth_password(this->session, username.toStdString().data(), password.toStdString().data())) {

        //fprintf(stderr, "Authentication by password failed.\n");
        return false;
    }

    //fprintf(stderr, "libssh2_sftp_init()!\n");

    this->sftp_session = libssh2_sftp_init(this->session);

    if (!this->sftp_session) {
        //fprintf(stderr, "Unable to init SFTP session\n");
    }

    //fprintf(stderr, "libssh2_sftp_open()!\n");

    return true;
}

//Cierra la conexión ssh
void Sftp::disconnect(){

    /*libssh2_sftp_close(this->sftp_handle);

    libssh2_sftp_shutdown(this->sftp_session);

    libssh2_session_disconnect(this->session, "Normal Shutdown, Thank you for playing");
    libssh2_session_free(this->session);*/

    #ifdef Q_WS_WIN
        closesocket(this->sock);
    #else
        close(this->sock);
    #endif

    libssh2_exit();
}

//Descarga el archivo serverFile y lo almacena en outputFile
bool Sftp::downloadFile(QString serverFile, QString outputFile){
    /*Request a file via SFTP */
    FILE *fd;
    fd = fopen(outputFile.toStdString().data(),"w");

    this->sftp_handle = libssh2_sftp_open(this->sftp_session, serverFile.toStdString().data(), LIBSSH2_FXF_READ, 0);
    if (!this->sftp_handle) {

        //fprintf(stderr, "Unable to open file with SFTP: %ld\n", libssh2_sftp_last_error(this->sftp_session));
        return false;
    }

    //fprintf(stderr, "libssh2_sftp_open() is done, now receive data!\n");

    do {
        char mem[131072];

        //loop until we fail
        this->rc = libssh2_sftp_read(this->sftp_handle, mem, sizeof(mem));

        if (this->rc > 0) {
            fwrite(mem,1,this->rc,fd);
        } else {
            break;
        }
    } while (1);

    fclose(fd);

    return true;
}

//Retorna una lista de archivos de un directorio especificado
QStringList Sftp::getListOfHandouts(QString handsoutDirectory){

    /* Request a dir listing via SFTP */
    this->sftp_handle = libssh2_sftp_opendir(this->sftp_session, handsoutDirectory.toStdString().data());

    if (!this->sftp_handle) {
        //fprintf(stderr, "Unable to open dir with SFTP\n");
    }

    QStringList result;

    do {
        char mem[512];
        char longentry[512];
        LIBSSH2_SFTP_ATTRIBUTES attrs;

        /* loop until we fail */
        this->rc = libssh2_sftp_readdir_ex(this->sftp_handle, mem, sizeof(mem), longentry, sizeof(longentry), &attrs);

        if(this->rc > 0) {
        /* rc is the length of the file name in the mem buffer */

            if (longentry[0] != '\0') {
                //printf("%s\n", longentry);
                result.append(mem);
            }
        }
        else
            break;

    } while (1);

    result.removeAll(".");
    result.removeAll("..");
    return result;
}
