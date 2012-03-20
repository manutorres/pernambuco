QT += core \ gui \ xml \ webkit \ sql \ network

macx{
    CONFIG +=app_bundle
    QRENCODE_FILE.files = /opt/local/share/qt4/plugins/sqldrivers
    QRENCODE_FILE.path = Contents/MacOS
    QMAKE_BUNDLE_DATA += QRENCODE_FILE
    LIBS +=  -L/usr/local/lib/ -lfreetype -lfontconfig -ljpeg -lz
    INCLUDEPATH += /usr/local/podofo/include
}

win32{
    LIBS += -LC:\Windows\System32 -lwsock32 \
            -Llibssh2\lib -lssh2 \
            -Lpodofo\lib -lpodofo
    INCLUDEPATH +=  libssh2\include \
                    podofo\include
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
           src/pdfmerge.cpp \
           src/webmanager.cpp

HEADERS  += src/mainwindow.h \
            src/xmlassigment.h \
            src/dbconnection.h \
            src/includes.h \
            src/kpm_settings.h \
            src/sftp.h \
            src/libssh2_config.h \
            src/pdfmerge.h \
            src/webmanager.h

FORMS    += ui/mainwindow.ui

RESOURCES += ui/rsc.qrc
