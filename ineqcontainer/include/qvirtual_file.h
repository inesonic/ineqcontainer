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
* This header defines the \ref QVirtualFile class.
***********************************************************************************************************************/

/* .. sphinx-project ineqcontainer */

#ifndef QVIRTUAL_FILE_H
#define QVIRTUAL_FILE_H

#include <QtGlobal>
#include <QMap>
#include <QIODevice>
#include <QObject>

#include <memory>

#include <container_status.h>
#include <container_virtual_file.h>

class QContainer;
class QFileContainer;

/**
 * Class that provides a QIODevice compatible API for a Container::VirtualFile instance.
 */
class QVirtualFile:public QIODevice {
    friend class QContainer;
    friend class QFileContainer;

    private:
        /**
         * Private constructor.
         *
         * \param[in] containerVirtualFile The underlying virtual file being marshalled by this class instance.
         *
         * \param[in] parent               Pointer to the parent object.
         */
        QVirtualFile(std::shared_ptr<Container::VirtualFile> containerVirtualFile, QObject* parent);

    public:
        ~QVirtualFile() override;

        /**
         * Method that deletes this file.  This virtual file object will no longer be valid after calling this
         * method.
         *
         * \return Returns true on success, returns false on error.
         */
        bool erase();

        /**
         * Method that determines if the position points to the end of the virtual file.
         *
         * \return Returns true if the end of the virtual file has been reached.  Returns false if there is more data
         *         after the current location in the virtual file.
         */
        bool atEnd() const final;

        /**
         * Returns the maximum number of bytes that are available for reading.
         *
         * \return Returns the number of available bytes of data before the end of the virtual file.
         */
        qint64 bytesAvailable() const final;

        /**
         * Returns the number of bytes of data pending write to the media.
         *
         * \return Returns the number of bytes of data pending write to the underlying media.
         */
        qint64 bytesToWrite() const final;

        /**
         * Closes the virtual file.
         */
        void close() final;

        /**
         * Determines if the QIODevice is a sequential access device such as a TCP socket.
         *
         * \return Returns true if the device is a sequential device.  Returns false if the device supports random
         *         access.  This implementation always returns false.
         */
        bool isSequential() const final;

        /**
         * Method you can call to seek to a specific location in the file.
         *
         * \param[in] pos The desired position.
         *
         * \return Returns true on success, returns false on error.
         */
        bool seek(qint64 pos) final;

        /**
         * Method you can use to determine the size of the virtual file, in bytes.
         *
         * \return Returns the size of the virtual file, in bytes.
         */
        qint64 size() const final;

        /**
         * Method you can use to copy this QIODevice.  Copies of the same QIODevice will operate on the same virtual
         * file instance.
         *
         * \param[in] other The instance to be copied.
         */
        QVirtualFile& operator=(const QVirtualFile& other);

        /**
         * Method you can use to obtain a shared pointer to the underlying Container::VirtualFile instance.
         *
         * \return Returns a pointer to the underlying virtual file instance.
         */
        std::shared_ptr<Container::VirtualFile> virtualFile();

    protected:
        /**
         * This method is called by the QIODevice to perform all read functions and is used to tie the QIODevice to the
         * underlying Container::VirtualFile instance.
         *
         * \param[in] data    The data buffer to hold the read data.
         *
         * \param[in] maxSize The maximum number of bytes to be read.
         *
         * \return Returns the number of bytes read or -1 on error.
         */
        qint64 readData(char* data, qint64 maxSize) final;

        /**
         * This method is called by the QIODevice to perform all write function and is used to tie the QIODevice to the
         * underlying Container::VirtualFile instance.
         *
         * \param[in] data    THe buffer containing the write data.
         *
         * \param[in] maxSize The maximum number of bytes available to be written.
         *
         * \return Returns the actual number of bytes written or -1 if an error occurred.
         */
        qint64 writeData(const char* data, qint64 maxSize) final;

    private:
        std::shared_ptr<Container::VirtualFile> currentVirtualFile;
};

#endif
