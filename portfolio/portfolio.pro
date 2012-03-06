QT += core gui \
        xml \
        webkit \
        sql

macx{
    CONFIG-=app_bundle
}

win32{
    LIBS += -LC:\Windows\System32 -lwsock32 \
            -L"C:\Program Files (x86)\libssh2\lib" -lssh2 \
            -L"C:\Program Files (x86)\PoDoFo\lib" -lpodofo
    INCLUDEPATH +=  "C:\Program Files (x86)\libssh2\include" \
                    "C:\Program Files (x86)\PoDoFo\include"
}

unix{
    LIBS += -lpodofo \
            -lssh2
}

TARGET = portfolio
OBJECTS_DIR = obj
MOC_DIR = moc
TEMPLATE = app
DESTDIR = bin

SOURCES += src/main.cpp \
           src/mainwindow.cpp \
           src/xmlassigment.cpp \
           src/dbconnection.cpp \
           src/sftp.cpp \
           src/pdfmerge.cpp

HEADERS  += src/mainwindow.h \
            src/xmlassigment.h \
            src/dbconnection.h \
            src/includes.h \
            src/kpm_settings.h \
            src/sftp.h \
            src/libssh2_config.h \
            src/pdfmerge.h

FORMS    += ui/mainwindow.ui
