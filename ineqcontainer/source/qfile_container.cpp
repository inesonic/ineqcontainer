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
#include <QFile>
#include <QByteArray>
#include <QObject>

#include <container_status.h>
#include <container_virtual_file.h>
#include <container_container.h>

#include "qvirtual_file.h"
#include "qfile_container.h"

QFileContainer::QFileContainer(
        const QString& fileIdentifier,
        QObject*       parent
    ):QObject(
        parent
    ),FileContainer(
        fileIdentifier.toStdString()
    ) {}


QFileContainer::~QFileContainer() {}


bool QFileContainer::open(const QString& filename, OpenMode openMode) {
    bool success;
    QByteArray          localFilename = QFile::encodeName(filename);
    ::Container::Status status        = ::Container::FileContainer::open(localFilename.toStdString(), openMode);

    if (status) {
        success = false;
    } else {
        success = true;
    }

    return success;
}


bool QFileContainer::close() {
    bool success;
    ::Container::Status status = ::Container::FileContainer::close();

    if (status) {
        success = false;
    } else {
        success = true;
    }

    return success;
}


QString QFileContainer::filename() const {
    QByteArray localFilename = QByteArray::fromStdString(::Container::FileContainer::filename());
    return QFile::decodeName(localFilename);
}


QFileContainer::DirectoryMap QFileContainer::directory() {
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


QPointer<QVirtualFile> QFileContainer::newVirtualFile(const QString& newVirtualFileName) {
    QPointer<QVirtualFile> virtualFile;

    std::shared_ptr<::Container::VirtualFile> vf;
    vf = ::Container::Container::newVirtualFile(newVirtualFileName.toStdString());

    if (vf) {
        virtualFile = QPointer<QVirtualFile>(new QVirtualFile(vf, this));
        directoryMap.insert(newVirtualFileName, virtualFile);
    }

    return virtualFile;
}


QString QFileContainer::errorString() const {
    return QString::fromStdString(lastStatus().description());
}


::Container::VirtualFile* QFileContainer::createFile(const std::string& virtualFileName) {
    return ::Container::Container::createFile(virtualFileName);
}
