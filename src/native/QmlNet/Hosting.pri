INCLUDEPATH += $$PWD

HEADERS += $$PWD/Hosting/coreclrhost.h \
    $$PWD/Hosting/CoreHost.h

SOURCES += \
    $$PWD/Hosting/CoreHost.cpp

unix {
    LIBS += -ldl
}


#QMAKE_LFLAGS += -Wl,-export_dynamic
