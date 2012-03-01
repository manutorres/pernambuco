#-------------------------------------------------
#
# Project created by QtCreator 2012-02-21T16:59:03
#
#-------------------------------------------------

QT       += core gui\
            xml \
            webkit \
            sql

#QT += xml

unix{
LIBS += -lpodofo \
        -lssh2
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

unix{
SOURCES += src/pdfmerge.cpp \
           src/sftp.cpp
}

HEADERS  += src/mainwindow.h \
            src/xmlassigment.h \
            src/dbconnection.h

unix{
HEADERS  += src/pdfmerge.h \
            src/sftp.h
}

FORMS    += ui/mainwindow.ui
