##-*-makefile-*-########################################################################################################
# Copyright 2016 Inesonic, LLC
#
# MIT License:
#   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
#   documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
#   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
#   permit persons to whom the Software is furnished to do so, subject to the following conditions:
#   
#   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
#   Software.
#   
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#   WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
#   OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
#   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
########################################################################################################################

########################################################################################################################
# Basic build characteristics
#

TEMPLATE = app
QT += core testlib
CONFIG += testcase c++14

HEADERS = test_qcontainer.h \
          test_qfile_container.h

SOURCES = test_ineqcontainer.cpp \
          test_qcontainer.cpp \
          test_qfile_container.cpp

########################################################################################################################
# Libraries
#

defined(SETTINGS_PRI, var) {
    include($${SETTINGS_PRI})
}

INEQCONTAINER_BASE = $${OUT_PWD}/../ineqcontainer

INCLUDEPATH += $${PWD}/../ineqcontainer/include
INCLUDEPATH += $${PWD}/../ineqcontainer/source # Needed for white-box tests

INCLUDEPATH += $${INECONTAINER_INCLUDE}
INCLUDEPATH += $${BOOST_INCLUDE}

unix {
    CONFIG(debug, debug|release) {
        LIBS += -L$${INEQCONTAINER_BASE}/build/debug/ -lineqcontainer
        PRE_TARGETDEPS += $${INEQCONTAINER_BASE}/build/debug/libineqcontainer.a
    } else {
        LIBS += -L$${INEQCONTAINER_BASE}/build/release/ -lineqcontainer
        PRE_TARGETDEPS += $${INEQCONTAINER_BASE}/build/release/libineqcontainer.a
   }

   LIBS += -L$${INECONTAINER_LIBDIR} -linecontainer
}

win32 {
    CONFIG(debug, debug|release) {
        LIBS += $${INEQCONTAINER_BASE}/build/Debug/ineqcontainer.lib
        PRE_TARGETDEPS += $${INEQCONTAINER_BASE}/build/Debug/ineqcontainer.lib
    } else {
        LIBS += $${INEQCONTAINER_BASE}/build/Release/ineqcontainer.lib
        PRE_TARGETDEPS += $${INEQCONTAINER_BASE}/build/Release/ineqcontainer.lib
    }

    LIBS += $${INECONTAINER_LIBDIR}/inecontainer.lib
}

########################################################################################################################
# Locate build intermediate and output products
#

TARGET = test_ineqcontainer

CONFIG(debug, debug|release) {
    unix:DESTDIR = build/debug
    win32:DESTDIR = build/Debug
} else {
    unix:DESTDIR = build/release
    win32:DESTDIR = build/Release
}

OBJECTS_DIR = $${DESTDIR}/objects
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/rcc
UI_DIR = $${DESTDIR}/ui
