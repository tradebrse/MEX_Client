#-------------------------------------------------
#
# Project created by QtCreator 2016-02-01T18:26:13
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MEX_Client
TEMPLATE = app


SOURCES += main.cpp\
        mex_login.cpp \
    mex_main.cpp \
    mex_tcpclientsocket.cpp \
    mex_trader.cpp \
    mex_product.cpp \
    mex_order.cpp \
    mex_myaccount.cpp \
    mex_tradelog.cpp \
    mex_adminpanel.cpp

HEADERS  += mex_login.h \
    mex_main.h \
    mex_tcpclientsocket.h \
    mex_trader.h \
    mex_product.h \
    mex_order.h \
    mex_myaccount.h \
    mex_tradelog.h \
    mex_adminpanel.h

FORMS    += mex_login.ui \
    mex_main.ui \
    mex_myaccount.ui \
    mex_tradelog.ui \
    mex_adminpanel.ui
