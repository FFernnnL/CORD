QT       += core gui
QT       += network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aichat.cpp \
    aiwidget.cpp \
    audiochat.cpp \
    chatdialog.cpp \
    filedialog.cpp \
    friendmessagedialog.cpp \
    groupchatdialog.cpp \
    headdialog.cpp \
    homedialog.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    movie.cpp \
    rcvdialog.cpp \
    registdialog.cpp \
    user.cpp \
    userdialog.cpp

HEADERS += \
    aichat.h \
    aiwidget.h \
    audiochat.h \
    chatdialog.h \
    filedialog.h \
    friendmessagedialog.h \
    groupchatdialog.h \
    headdialog.h \
    homedialog.h \
    logindialog.h \
    mainwindow.h \
    movie.h \
    rcvdialog.h \
    registdialog.h \
    user.h \
    userdialog.h

FORMS += \
    add_friend_note_dialog.ui \
    aiwidget.ui \
    audiochat.ui \
    chatdialog.ui \
    filedialog.ui \
    friendmessagedialog.ui \
    groupchatdialog.ui \
    headdialog.ui \
    homedialog.ui \
    logindialog.ui \
    mainwindow.ui \
    movie.ui \
    rcvdialog.ui \
    registdialog.ui \
    userdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../../../../mnt/hgfs/SharingFileVM/1.gif \
    ../../../../mnt/hgfs/SharingFileVM/1.jpg \
    ../../../../mnt/hgfs/SharingFileVM/1.png \
    ../../../../mnt/hgfs/SharingFileVM/pic/0.png \
    ../../../../mnt/hgfs/SharingFileVM/pic/1.png \
    ../../../../mnt/hgfs/SharingFileVM/pic/2.png \
    ../../../../mnt/hgfs/SharingFileVM/pic/3.png \
    ../../../../mnt/hgfs/SharingFileVM/pic/4.png \
    1.gif

RESOURCES +=
