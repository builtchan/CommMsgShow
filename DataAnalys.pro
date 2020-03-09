QT       += core gui xml network

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
    CStateManager.cpp \
    CTextParse.cpp \
    CUIConfig.cpp \
    main.cpp

HEADERS += \
    CMyTextEdit.h \
    CTextParse.h \
    CUIConfig.h \
    commonDef.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


# 版本号
VERSION = 1.0.0

RC_ICONS = logo.ico
# 语言
# 0x0004 表示 简体中文
# 详见 https://msdn.microsoft.com/en-us/library/dd318693%28vs.85%29.aspx
RC_LANG = 0x0004

# 产品名称
QMAKE_TARGET_PRODUCT = DataAnalys
# 详细描述
QMAKE_TARGET_DESCRIPTION = data Parse
# 版权
QMAKE_TARGET_COPYRIGHT = Copyright(C) 20200226

