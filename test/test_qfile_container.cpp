/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
*
* MIT License:
*   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
*   documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
*   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
*   permit persons to whom the Software is furnished to do so, subject to the following conditions:
*   
*   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
*   Software.
*   
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
*   WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
*   OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
*   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
********************************************************************************************************************//**
* \file
*
* This file implements tests of the QFileContainer class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>
#include <QIODevice>
#include <QPointer>

#include <cstring>
#include <random>

#include <qfile_container.h>
#include <qvirtual_file.h>

#include "test_qfile_container.h"

/***********************************************************************************************************************
 * TestQContainer
 */

void TestQFileContainer::testQFileContainerApi() {
    // Open an empty container and insert a virtual file into it.

    QFileContainer writeContainer(QString("Inesonic, LLC.\nAion Test"));

    bool success = writeContainer.open(QString("test_container.dat"), QFileContainer::OpenMode::OVERWRITE);
    QVERIFY(success);

    char buffer[bufferSizeInBytes];
    for (unsigned i=0 ; i<bufferSizeInBytes ; ++i) {
        buffer[i] = static_cast<char>(i % 254);
    }

    QPointer<QVirtualFile> vf = writeContainer.newVirtualFile(QString("test.dat"));
    QVERIFY(!vf.isNull());

    vf->open(QIODevice::ReadWrite);

    qint64 bytesWritten = vf->write(buffer, bufferSizeInBytes);
    QVERIFY(bytesWritten == bufferSizeInBytes);

    success = writeContainer.close();
    QVERIFY(success);

    std::memset(buffer, 0, bufferSizeInBytes);

    QFileContainer readContainer(QString("Inesonic, LLC.\nAion Test"));

    success = readContainer.open(QString("test_container.dat"), QFileContainer::OpenMode::READ_ONLY);
    QVERIFY(success);

    QFileContainer::DirectoryMap directory = readContainer.directory();
    QVERIFY(directory.size() == 1);

    vf = directory.value(QString("test.dat"));
    QVERIFY(!vf.isNull());

    vf->open(QIODevice::ReadOnly);

    qint64 bytesRead = vf->read(buffer, bufferSizeInBytes);
    QVERIFY(bytesRead == bufferSizeInBytes);

    for (unsigned i=0 ; i<bufferSizeInBytes ; ++i) {
        QVERIFY(buffer[i] == static_cast<char>(i % 254));
    }

    success = readContainer.close();
    QVERIFY(success);
}
