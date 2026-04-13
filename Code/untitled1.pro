QT += widgets

CONFIG += c++17

SOURCES += \
    Unit_test.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Scheduler.h \
    FCFS.h \
    SJF.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target