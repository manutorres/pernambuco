#-------------------------------------------------
#
# Project created by QtCreator 2012-02-21T16:59:03
#
#-------------------------------------------------

QT += core gui \
        xml \
        webkit \
        sql

unix{
LIBS += -lpodofo \
        -lssh2
}

!unix{

#Falta PoDoFo

#INCLUDEPATH += "C:\Program Files (x86)\libssh2-1.2.5\include\

#LIBS += "C:\Program Files (x86)\libssh2-1.2.5\lib\libgcrypt.dll.a" \
#        "C:\Program Files (x86)\libssh2-1.2.5\lib\libgpg-error.dll.a" \
#        "C:\Program Files (x86)\libssh2-1.2.5\lib\libssh2.dll.a"
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
            src/dbconnection.h

unix{
SOURCES += src/pdfmerge.cpp \
           src/sftp.cpp
HEADERS  += src/pdfmerge.h \
            src/sftp.h
}

FORMS    += ui/mainwindow.ui
