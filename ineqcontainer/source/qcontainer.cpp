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
* This file implements the \ref QContainer class.
***********************************************************************************************************************/

#include <QMap>
#include <QList>
#include <QIODevice>
#include <QObject>

#include <container_status.h>
#include <container_virtual_file.h>
#include <container_container.h>

#include "qvirtual_file.h"
#include "qcontainer.h"

QContainer::QContainer(
        const QString& fileIdentifier,
        QObject*       parent
    ):QObject(
        parent
    ),Container(
        fileIdentifier.toStdString()
    ) {
    currentDevice = Q_NULLPTR;
}


QContainer::QContainer(
        QIODevice*     device,
        const QString& fileIdentifier,
        QObject*       parent
    ):QObject(
        parent
    ),Container(
        fileIdentifier.toStdString()
    ) {
    setDevice(device);
}


QContainer::~QContainer() {}


void QContainer::setDevice(QIODevice* device) {
    currentDevice = device;
    setParent(currentDevice);
}


QIODevice* QContainer::device() {
    return currentDevice;
}


bool QContainer::open() {
    bool success;

    ::Container::Status status = ::Container::Container::open();

    if (status) {
        success = false;
    } else {
        success = true;
    }

    return success;
}


bool QContainer::close() {
    bool success;

    ::Container::Status status = ::Container::Container::close();

    if (status) {
        success = false;
    } else {
        success = true;
    }

    return success;
}


QContainer::DirectoryMap QContainer::directory() {
    ::Container::Container::DirectoryMap directory = ::Container::Container::directory();

    ::Container::Container::DirectoryMap::iterator pos = directory.begin();
    ::Container::Container::DirectoryMap::iterator end = directory.end();
    while (pos != end) {
        QString filename = QString::fromStdString(pos->first);
        if (!directoryMap.contains(filename)) {
            QVirtualFile* virtualFile = new QVirtualFile(pos->second, this);
            directoryMap.insert(filename, QPointer<QVirtualFile>(virtualFile));
        }

        ++pos;
    }

    QList<QString> keys = directoryMap.keys();
    for (QList<QString>::const_iterator it=keys.begin() ; it!=keys.end() ; ++it) {
        if (directory.find(it->toStdString()) == directory.end()) {
            QVirtualFile* qvf = directoryMap.value(*it).data();
            directoryMap.remove(*it);
            delete qvf;
        }
    }

    return directoryMap;
}


QPointer<QVirtualFile> QContainer::newVirtualFile(const QString& newVirtualFileName) {
    QPointer<QVirtualFile> virtualFile;

    std::shared_ptr<::Container::VirtualFile> vf;
    vf = ::Container::Container::newVirtualFile(newVirtualFileName.toStdString());

    if (vf) {
        virtualFile = QPointer<QVirtualFile>(new QVirtualFile(vf, this));
        directoryMap.insert(newVirtualFileName, virtualFile);
    }

    return virtualFile;
}


QString QContainer::errorString() const {
    return QString::fromStdString(lastStatus().description());
}


long long QContainer::size() {
    long long result;

    if (currentDevice == Q_NULLPTR) {
        result = -1;
    } else {
        result = currentDevice->size();
    }

    return result;
}


::Container::Status QContainer::setPosition(unsigned long long newOffset) {
    ::Container::Status status;

    if (currentDevice == Q_NULLPTR) {
        status = ::Container::FileContainerNotOpen();
    } else {
        unsigned long long currentSize = currentDevice->size();
        if (newOffset <= currentSize) {
            bool result = currentDevice->seek(newOffset);

            if (!result) {
                status = ::Container::SeekError(newOffset, currentSize);
            }
        } else {
            status = ::Container::SeekError(newOffset, currentSize);
        }
    }

    return status;
}


::Container::Status QContainer::setPositionLast() {
    ::Container::Status status;

    if (currentDevice == Q_NULLPTR) {
        status = ::Container::FileContainerNotOpen();
    } else {
        unsigned long long currentSize = currentDevice->size();
        bool result = currentDevice->seek(currentSize);

        if (!result) {
            status = ::Container::SeekError(currentSize, currentSize);
        }
    }

    return status;
}


unsigned long long QContainer::position() const {
    unsigned long long currentPosition;

    if (currentDevice == Q_NULLPTR) {
        currentPosition = 0;
    } else {
        currentPosition = currentDevice->pos();
    }

    return currentPosition;
}


::Container::Status QContainer::read(std::uint8_t* buffer, unsigned desiredCount) {
    ::Container::Status status;

    if (currentDevice == Q_NULLPTR) {
        status = ::Container::FileContainerNotOpen();
    } else {
        unsigned long long currentPosition = currentDevice->pos();
        long long bytesRead = currentDevice->read(reinterpret_cast<char*>(buffer), desiredCount);

        if (bytesRead < 0) {
            status = ::Container::FileReadError("", currentPosition, 0);
        } else {
            status = ::Container::ReadSuccessful(bytesRead);
        }
    }

    return status;
}


::Container::Status QContainer::write(const std::uint8_t* buffer, unsigned count) {
    ::Container::Status status;

    if (currentDevice == Q_NULLPTR) {
        status = ::Container::FileContainerNotOpen();
    } else {
        unsigned long long currentPosition = currentDevice->pos();
        long long bytesWritten = currentDevice->write(reinterpret_cast<const char*>(buffer), count);

        if (bytesWritten < 0) {
            status = ::Container::FileWriteError("", currentPosition, 0);
        } else {
            status = ::Container::WriteSuccessful(bytesWritten);
        }
    }

    return status;
}


bool QContainer::supportsTruncation() const {
    return false;
}


::Container::Status QContainer::truncate() {
    return ::Container::NoStatus();
}


::Container::Status QContainer::flush() {
    return ::Container::NoStatus();
}


::Container::VirtualFile* QContainer::createFile(const std::string& virtualFileName) {
    return ::Container::Container::createFile(virtualFileName);
}
