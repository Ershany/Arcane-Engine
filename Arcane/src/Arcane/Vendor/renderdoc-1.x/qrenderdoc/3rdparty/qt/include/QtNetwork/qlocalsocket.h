/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtNetwork module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QLOCALSOCKET_H
#define QLOCALSOCKET_H

#include <QtNetwork/qtnetworkglobal.h>
#include <QtCore/qiodevice.h>
#include <QtNetwork/qabstractsocket.h>

QT_REQUIRE_CONFIG(localserver);

QT_BEGIN_NAMESPACE

class QLocalSocketPrivate;

class Q_NETWORK_EXPORT QLocalSocket : public QIODevice
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QLocalSocket)

public:
    enum LocalSocketError
    {
        ConnectionRefusedError = QAbstractSocket::ConnectionRefusedError,
        PeerClosedError = QAbstractSocket::RemoteHostClosedError,
        ServerNotFoundError = QAbstractSocket::HostNotFoundError,
        SocketAccessError = QAbstractSocket::SocketAccessError,
        SocketResourceError = QAbstractSocket::SocketResourceError,
        SocketTimeoutError = QAbstractSocket::SocketTimeoutError,
        DatagramTooLargeError = QAbstractSocket::DatagramTooLargeError,
        ConnectionError = QAbstractSocket::NetworkError,
        UnsupportedSocketOperationError = QAbstractSocket::UnsupportedSocketOperationError,
        UnknownSocketError = QAbstractSocket::UnknownSocketError,
        OperationError = QAbstractSocket::OperationError
    };

    enum LocalSocketState
    {
        UnconnectedState = QAbstractSocket::UnconnectedState,
        ConnectingState = QAbstractSocket::ConnectingState,
        ConnectedState = QAbstractSocket::ConnectedState,
        ClosingState = QAbstractSocket::ClosingState
    };

    QLocalSocket(QObject *parent = Q_NULLPTR);
    ~QLocalSocket();

    void connectToServer(OpenMode openMode = ReadWrite);
    void connectToServer(const QString &name, OpenMode openMode = ReadWrite);
    void disconnectFromServer();

    void setServerName(const QString &name);
    QString serverName() const;
    QString fullServerName() const;

    void abort();
    virtual bool isSequential() const Q_DECL_OVERRIDE;
    virtual qint64 bytesAvailable() const Q_DECL_OVERRIDE;
    virtual qint64 bytesToWrite() const Q_DECL_OVERRIDE;
    virtual bool canReadLine() const Q_DECL_OVERRIDE;
    virtual bool open(OpenMode openMode = ReadWrite) Q_DECL_OVERRIDE;
    virtual void close() Q_DECL_OVERRIDE;
    LocalSocketError error() const;
    bool flush();
    bool isValid() const;
    qint64 readBufferSize() const;
    void setReadBufferSize(qint64 size);

    bool setSocketDescriptor(qintptr socketDescriptor,
                             LocalSocketState socketState = ConnectedState,
                             OpenMode openMode = ReadWrite);
    qintptr socketDescriptor() const;

    LocalSocketState state() const;
    bool waitForBytesWritten(int msecs = 30000) Q_DECL_OVERRIDE;
    bool waitForConnected(int msecs = 30000);
    bool waitForDisconnected(int msecs = 30000);
    bool waitForReadyRead(int msecs = 30000) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void connected();
    void disconnected();
    void error(QLocalSocket::LocalSocketError socketError);
    void stateChanged(QLocalSocket::LocalSocketState socketState);

protected:
    virtual qint64 readData(char*, qint64) Q_DECL_OVERRIDE;
    virtual qint64 writeData(const char*, qint64) Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(QLocalSocket)
#if defined(QT_LOCALSOCKET_TCP)
    Q_PRIVATE_SLOT(d_func(), void _q_stateChanged(QAbstractSocket::SocketState))
    Q_PRIVATE_SLOT(d_func(), void _q_error(QAbstractSocket::SocketError))
#elif defined(Q_OS_WIN)
    Q_PRIVATE_SLOT(d_func(), void _q_canWrite())
    Q_PRIVATE_SLOT(d_func(), void _q_pipeClosed())
    Q_PRIVATE_SLOT(d_func(), void _q_winError(ulong, const QString &))
#else
    Q_PRIVATE_SLOT(d_func(), void _q_stateChanged(QAbstractSocket::SocketState))
    Q_PRIVATE_SLOT(d_func(), void _q_error(QAbstractSocket::SocketError))
    Q_PRIVATE_SLOT(d_func(), void _q_connectToSocket())
    Q_PRIVATE_SLOT(d_func(), void _q_abortConnectionAttempt())
#endif
};

#ifndef QT_NO_DEBUG_STREAM
#include <QtCore/qdebug.h>
Q_NETWORK_EXPORT QDebug operator<<(QDebug, QLocalSocket::LocalSocketError);
Q_NETWORK_EXPORT QDebug operator<<(QDebug, QLocalSocket::LocalSocketState);
#endif

QT_END_NAMESPACE

#endif // QLOCALSOCKET_H
