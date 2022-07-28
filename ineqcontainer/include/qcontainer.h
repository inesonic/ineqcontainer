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
* This header defines the \ref QContainer class.
***********************************************************************************************************************/

/* .. sphinx-project ineqcontainer */

#ifndef QCONTAINER_H
#define QCONTAINER_H

#include <QMap>
#include <QIODevice>
#include <QPointer>
#include <QObject>

#include <container_container.h>

class QVirtualFile;

/**
 * Class that extends and Qt-ify's the Container::Container class to provide an interface to an underlying QIODevice.
 * Note that the default implementation does not support file truncation.
 */
class QContainer:public QObject, public Container::Container {
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
        QContainer(const QString& fileIdentifier, QObject* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] device         The device to use for I/O.
         *
         * \param[in] fileIdentifier A string placed at a fixed location near the beginning of the file.  The string
         *                           can be used as a magic number to identifier the file type and is used as a
         *                           check when opening a new container.
         *
         * \param[in] parent         Pointer to the parent object.
         */
        QContainer(QIODevice* device, const QString& fileIdentifier, QObject* parent = Q_NULLPTR);

        ~QContainer() override;

        /**
         * Method you can call to set the QIODevice used for I/O.  The class will take ownership of the device.
         *
         * \param[in] device The device used for I/O.
         */
        void setDevice(QIODevice* device);

        /**
         * Method you can use to obtain the device used for I/O.
         *
         * \return Returns the QIODevice used to access the container.
         */
        QIODevice* device();

        /**
         * Method that should be called to open the container.  If the container is empty, the method will attempt
         * to create a file header.  If the container is not empty, the method will verify that the file container
         * is valid.
         *
         * You must call this method before performing any operations on the container.  You must also be sure that
         * there are no Container::VirtualFile instances instantiated for this container when this method is called.
         *
         * \return Returns true on success, returns false on error.
         */
        bool open();

        /**
         * Method that should be called after all file operations are complete.  Forces all underlying virtual files
         * to be flushed and closed and forces any data contained within the container to be flushed.
         *
         * \return Returns true on success, returns false on error.
         */
        bool close();

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

    protected:
        /**
         * Method that is called to determine the current size of the underlying data store, in bytes.
         *
         * \return Returns the size of the underlying data store, in bytes.  A negative value should be returned if
         *         an error occurs.
         */
        long long size() final;

        /**
         * Method that is called to seek to a position in the underlying data store prior to performing a call to
         * Container::Container::read, Container::Container::write, or Container::Container::truncate.
         *
         * \param[in] newOffset The new position in the underlying data store to seek to.
         *
         * \return Returns the status from this operation.
         */
        ::Container::Status setPosition(unsigned long long newOffset) final;

        /**
         * Method that is called to seek to the last just past the last byte in the underlying data store.
         *
         * \return Returns the status from this operation.
         */
        ::Container::Status setPositionLast() final;

        /**
         * Method that is called to determine the current byte offset from the beginning of the container.
         *
         * \return Returns the current byte offset from the beginning of the container.
         */
        unsigned long long position() const final;

        /**
         * Method that is called to read a specified number of bytes of data from the underlying data store.
         *
         * \param[in] buffer       The buffer to receive the data.  The buffer is guaranteed to be large enough to
         *                         hold all the requested data.
         *
         * \param[in] desiredCount The number of bytes that are expected to be read.
         *
         * \return Returns the status from the read operation.  An instance of Container::ReadSuccessful should be
         *         returned on success.
         */
        ::Container::Status read(std::uint8_t* buffer, unsigned desiredCount) final;

        /**
         * Method that is called to write a specified number of bytes of data to the underlying data store.
         *
         * \param[in] buffer The buffer holding the data to be written.
         *
         * \param[in] count  The number of bytes to be written.
         *
         * \return Returns the status from the write operation.  An instance of Container::Status should be returned
         *         on success.
         */
        ::Container::Status write(const std::uint8_t* buffer, unsigned count) final;

        /**
         * Method you can overload to indicate whether the derived class supports file truncation.
         *
         * \return Returns true if file truncation is supported.  Returns false if file truncation is not supported.
         */
        bool supportsTruncation() const final;

        /**
         * Method that is called to truncate the container at the current file position.
         *
         * \return Returns the status from the truncate operation.
         */
        ::Container::Status truncate() final;

        /**
         * Method that is called to force any written data to be flushed to the media.
         *
         * \return Returns the status from the flush operation.
         */
        ::Container::Status flush() final;

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

        /**
         * Pointer to the underlying device being used for I/O.
         */
        QIODevice* currentDevice;
};

#endif
