QT += core gui \
        xml \
        webkit \
        sql

macx{
    CONFIG-=app_bundle
}

win32{
    #LIBS += -LC:\Windows\System32 -lwsock32
    #LIBS += -L"C:\Program Files (x86)\libssh2\lib" -lssh2
    #INCLUDEPATH += "C:\Program Files (x86)\libssh2\include"
}

unix:{
    LIBS += -lpodofo \
            -lssh2
    SOURCES += src/pdfmerge.cpp \
                src/sftp.cpp
    HEADERS  += src/pdfmerge.h \
                src/sftp.h \
                src/libssh2_config.h
}

TARGET = portfolio
OBJECTS_DIR = obj
MOC_DIR = moc
TEMPLATE = app
DESTDIR = bin

SOURCES += src/main.cpp\
           src/mainwindow.cpp \
           src/xmlassigment.cpp \
           src/dbconnection.cpp

HEADERS  += src/mainwindow.h \
            src/xmlassigment.h \
            src/dbconnection.h \
            src/includes.h \
            src/kpm_settings.h

FORMS    += ui/mainwindow.ui
