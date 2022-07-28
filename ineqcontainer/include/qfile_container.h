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
* This header defines the \ref QFileContainer class.
***********************************************************************************************************************/

/* .. sphinx-project ineqcontainer */

#ifndef QFILE_CONTAINER_H
#define QFILE_CONTAINER_H

#include <QMap>
#include <QPointer>
#include <QObject>

#include <container_file_container.h>

class QVirtualFile;

/**
 * Class that extends and Qt-ify's the Container::FileContainer class to provide an interface a container stored in a
 * file.  This class is preferential to the QContainer class in that it uses the underlying Container::FileContainer
 * class an engine, supporting file truncation.
 */
class QFileContainer:public QObject, public Container::FileContainer {
    public:
        /**
         * Type used for maps of virtual files by name.
         */
        typedef QMap<QString, QPointer<QVirtualFile>> DirectoryMap;

        /**
         * Constructor
         *
         * \param[in] fileIdentifier A string placed at a fixed location near the beginning of the file.  The string
         *                           can be used as a magic number to identifier the file type and is used as a
         *                           check when opening a new container.
         *
         * \param[in] parent         Pointer to the parent object.
         */
        QFileContainer(const QString& fileIdentifier, QObject* parent = Q_NULLPTR);

        ~QFileContainer() override;

        /**
         * Method that should be called to open the container.  If the container is empty, the method will attempt
         * to create a file header.  If the container is not empty, the method will verify that the file container
         * is valid.
         *
         * You must call this method before performing any operations on the container.  You must also be sure that
         * there are no Container::VirtualFile instances instantiated for this container when this method is called.
         *
         * \param[in] filename The filename of the file to be opened.
         *
         * \param[in] openMode The open mode for the file.
         *
         * \return Returns true on success, returns false on error.
         */
        bool open(const QString& filename, OpenMode openMode = OpenMode::READ_WRITE);

        /**
         * Method that should be called after all file operations are complete.  Forces all underlying virtual files
         * to be flushed and closed and forces any data contained within the container to be flushed.
         *
         * \return Returns true on success, returns false on error.
         */
        bool close();

        /**
         * Method you can use to obtain the filename of the currently open file.  An empty string will be returned
         * if the container is closed.
         *
         * \return Returns the open file's name.
         */
        QString filename() const;

        /**
         * Returns a directory of all the streams in the container.
         *
         * \return Returns a map, keyed by the stream name, of streams in the container.
         */
        DirectoryMap directory();

        /**
         * Method you can call to create a new virtual file in the container.  The newly created file will be
         * added to the directory.
         *
         * \param[in] newVirtualFileName The new name to assign to this file.
         *
         * \return Returns the newly created virtual file.  A null pointer is returned on error.
         */
        QPointer<QVirtualFile> newVirtualFile(const QString& newVirtualFileName);

        /**
         * Method you can use to obtain an error string from the last operation performed.
         *
         * \return Returns a description of the error.
         */
        QString errorString() const;

    private:
        /**
         * Factory method that is called by the streaming API to create new virtual file instances.  You should
         * overload this method if you wish to use the stremaing API to instantiate classes derived from
         * Container::VirtualFile.
         *
         * Upon creation, the newly created stream will be added to the directory.
         *
         * This version marshalls a call to QContainer::createFile(const QString&).
         *
         * \param[in] virtualFileName The name of the virtual file that should be created.
         *
         * \return Returns a newly created virtual file with the assigned name.
         */
        ::Container::VirtualFile* createFile(const std::string& virtualFileName) final;

        /**
         * Current list of translated virtual files.
         */
        DirectoryMap directoryMap;
};

#endif
