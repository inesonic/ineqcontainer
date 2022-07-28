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
* This file implements the \ref QVirtualFile class.
***********************************************************************************************************************/

#include <QMap>
#include <QIODevice>
#include <QObject>
#include <QString>

#include <container_status.h>
#include <container_virtual_file.h>
#include <container_container.h>

#include "qcontainer.h"
#include "qvirtual_file.h"

QVirtualFile::QVirtualFile(
        std::shared_ptr<Container::VirtualFile> containerVirtualFile,
        QObject*                                parent
    ):QIODevice(
        parent
    ) {
    currentVirtualFile = containerVirtualFile;
}


QVirtualFile::~QVirtualFile() {}


bool QVirtualFile::erase() {
    bool                success;
    ::Container::Status status = currentVirtualFile->erase();

    if (status) {
        setErrorString(QString::fromStdString(status.description()));
        success = false;
    } else {
        success = true;
    }

    return success;
}


bool QVirtualFile::atEnd() const {
    return currentVirtualFile->size() == currentVirtualFile->position();
}


qint64 QVirtualFile::bytesAvailable() const {
    return currentVirtualFile->size() - currentVirtualFile->position();
}


qint64 QVirtualFile::bytesToWrite() const {
    return QIODevice::bytesToWrite() + currentVirtualFile->bytesInWriteCache();
}


void QVirtualFile::close() {
    QIODevice::close();

    ::Container::Status status = currentVirtualFile->flush();
    if (status) {
        setErrorString(QString::fromStdString(status.description()));
    }
}


bool QVirtualFile::isSequential() const {
    return false;
}


bool QVirtualFile::seek(qint64 pos) {
    bool success = QIODevice::seek(pos);

    if (success) {
        ::Container::Status status = currentVirtualFile->setPosition(pos);

        if (status) {
            setErrorString(QString::fromStdString(status.description()));
            success = false;
        }
    }

    return success;
}


qint64 QVirtualFile::size() const {
    return currentVirtualFile->size();
}


QVirtualFile& QVirtualFile::operator=(const QVirtualFile& other) {
    currentVirtualFile = other.currentVirtualFile;
    return *this;
}


std::shared_ptr<Container::VirtualFile> QVirtualFile::virtualFile() {
    return currentVirtualFile;
}


qint64 QVirtualFile::readData(char* data, qint64 maxSize) {
    qint64 bytesRead;

    if (maxSize > 0) {
        ::Container::Status status = currentVirtualFile->read(reinterpret_cast<std::uint8_t*>(data), maxSize);

        if (status.success()) {
            bytesRead = ::Container::ReadSuccessful(status).bytesRead();
        } else {
            setErrorString(QString::fromStdString(status.description()));
            bytesRead = -1;
        }
    } else {
        bytesRead = 0;
    }

    return bytesRead;
}


qint64 QVirtualFile::writeData(const char* data, qint64 maxSize) {
    qint64 bytesWritten;

    if (maxSize > 0) {
        ::Container::Status status = currentVirtualFile->write(reinterpret_cast<const std::uint8_t*>(data), maxSize);

        if (status.success()) {
            bytesWritten = ::Container::WriteSuccessful(status).bytesWritten();
        } else {
            setErrorString(QString::fromStdString(status.description()));
            bytesWritten = -1;
        }
    } else {
        bytesWritten = 0;
    }

    return bytesWritten;
}
