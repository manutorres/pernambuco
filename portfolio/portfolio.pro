QT += core \ gui \ xml \ webkit \ sql \ network

macx{
    CONFIG +=app_bundle
    QRENCODE_FILE.files = /opt/local/share/qt4/plugins/sqldrivers
    QRENCODE_FILE.path = Contents/MacOS
    QMAKE_BUNDLE_DATA += QRENCODE_FILE
    LIBS +=  -L/usr/local/lib/ -lfreetype -lfontconfig -ljpeg -lz
    INCLUDEPATH += /usr/local/podofo/include

    ICON = icons/osx/portfolio.icns
}

win32{
    LIBS += -LC:\Windows\System32 -lwsock32 \
            -Llibssh2\lib -lssh2 \
            -Lpodofo\lib -lpodofo
    INCLUDEPATH +=  libssh2\include \
                    podofo\include
    RC_FILE = icons/win/icon.rc
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
           src/dbconnection.cpp \
           src/sftp.cpp \
           src/pdfmerge.cpp \
           src/utils.cpp \
           src/setting.cpp

HEADERS  += src/mainwindow.h \            
            src/dbconnection.h \
            src/includes.h \
            src/kpm_settings.h \
            src/sftp.h \
            src/libssh2_config.h \
            src/pdfmerge.h \
            src/utils.h \
            src/setting.h

FORMS    += ui/mainwindow.ui

RESOURCES += ui/rsc.qrc

OTHER_FILES += icons/win.portfolio.rc #icons/win/icon.rc
