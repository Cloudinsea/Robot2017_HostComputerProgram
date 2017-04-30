#-------------------------------------------------
#
# Project created by QtCreator 2017-03-08T13:33:57
#
#-------------------------------------------------

QT       += core gui serialport
#win32:CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UsartAssistant
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        usartassistant.cpp

HEADERS  += usartassistant.h


FORMS    += usartassistant.ui


INCLUDEPATH += "D:\soft\HALCON-13.0/include//"
INCLUDEPATH += "D:\soft\HALCON-13.0/include/halconcpp//"
LIBS        += "D:\soft\HALCON-13.0/lib/x64-win64/halcon.lib"
LIBS        += "D:\soft\HALCON-13.0/lib/x64-win64/halconcpp.lib"
LIBS        += "D:\soft\HALCON-13.0/lib/x64-win64/halconc.lib"
LIBS        += "D:\soft\HALCON-13.0/lib/x64-win64/halconcpp10.lib"
LIBS        += "D:\soft\HALCON-13.0/lib/x64-win64/halconcpp10xl.lib"
LIBS        += "D:\soft\HALCON-13.0/lib/x64-win64/halconcppxl.lib"
LIBS        += "D:\soft\HALCON-13.0/lib/x64-win64/halconcxl.lib"
LIBS        += "D:\soft\HALCON-13.0/lib/x64-win64/halconx.lib"
LIBS        += "D:\soft\HALCON-13.0/lib/x64-win64/halconxl.lib"
LIBS        += "D:\soft\HALCON-13.0/lib/x64-win64/halconxxl.lib"
LIBS        += "D:\soft\HALCON-13.0/lib/x64-win64/hdevenginecpp.lib"
LIBS        += "D:\soft\HALCON-13.0/lib/x64-win64/hdevenginecpp10.lib"
LIBS        += "D:\soft\HALCON-13.0/lib/x64-win64/hdevenginecpp10xl.lib"
LIBS        += "D:\soft\HALCON-13.0/lib/x64-win64/hdevenginecppxl.lib"
LIBS        += "D:\soft\HALCON-13.0/lib/x64-win64/hdevenginex.lib"
LIBS        += "D:\soft\HALCON-13.0/lib/x64-win64/hdevenginexxl.lib"

DISTFILES += \
    ReadMe.txt \
    AlternateCode.txt
