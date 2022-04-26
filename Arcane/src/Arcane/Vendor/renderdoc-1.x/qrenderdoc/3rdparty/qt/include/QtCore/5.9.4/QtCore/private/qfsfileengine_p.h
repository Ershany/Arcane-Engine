/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QFSFILEENGINE_P_H
#define QFSFILEENGINE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qplatformdefs.h"
#include "QtCore/private/qabstractfileengine_p.h"
#include <QtCore/private/qfilesystementry_p.h>
#include <QtCore/private/qfilesystemmetadata_p.h>
#include <qhash.h>

#ifndef QT_NO_FSFILEENGINE

QT_BEGIN_NAMESPACE

class QFSFileEnginePrivate;

class Q_CORE_EXPORT QFSFileEngine : public QAbstractFileEngine
{
    Q_DECLARE_PRIVATE(QFSFileEngine)
public:
    QFSFileEngine();
    explicit QFSFileEngine(const QString &file);
    ~QFSFileEngine();

    bool open(QIODevice::OpenMode openMode) Q_DECL_OVERRIDE;
    bool open(QIODevice::OpenMode flags, FILE *fh);
    bool close() Q_DECL_OVERRIDE;
    bool flush() Q_DECL_OVERRIDE;
    bool syncToDisk() Q_DECL_OVERRIDE;
    qint64 size() const Q_DECL_OVERRIDE;
    qint64 pos() const Q_DECL_OVERRIDE;
    bool seek(qint64) Q_DECL_OVERRIDE;
    bool isSequential() const Q_DECL_OVERRIDE;
    bool remove() Q_DECL_OVERRIDE;
    bool copy(const QString &newName) Q_DECL_OVERRIDE;
    bool rename(const QString &newName) Q_DECL_OVERRIDE;
    bool renameOverwrite(const QString &newName) Q_DECL_OVERRIDE;
    bool link(const QString &newName) Q_DECL_OVERRIDE;
    bool mkdir(const QString &dirName, bool createParentDirectories) const Q_DECL_OVERRIDE;
    bool rmdir(const QString &dirName, bool recurseParentDirectories) const Q_DECL_OVERRIDE;
    bool setSize(qint64 size) Q_DECL_OVERRIDE;
    bool caseSensitive() const Q_DECL_OVERRIDE;
    bool isRelativePath() const Q_DECL_OVERRIDE;
    QStringList entryList(QDir::Filters filters, const QStringList &filterNames) const Q_DECL_OVERRIDE;
    FileFlags fileFlags(FileFlags type) const Q_DECL_OVERRIDE;
    bool setPermissions(uint perms) Q_DECL_OVERRIDE;
    QByteArray id() const override;
    QString fileName(FileName file) const Q_DECL_OVERRIDE;
    uint ownerId(FileOwner) const Q_DECL_OVERRIDE;
    QString owner(FileOwner) const Q_DECL_OVERRIDE;
    QDateTime fileTime(FileTime time) const Q_DECL_OVERRIDE;
    void setFileName(const QString &file) Q_DECL_OVERRIDE;
    int handle() const Q_DECL_OVERRIDE;

#ifndef QT_NO_FILESYSTEMITERATOR
    Iterator *beginEntryList(QDir::Filters filters, const QStringList &filterNames) Q_DECL_OVERRIDE;
    Iterator *endEntryList() Q_DECL_OVERRIDE;
#endif

    qint64 read(char *data, qint64 maxlen) Q_DECL_OVERRIDE;
    qint64 readLine(char *data, qint64 maxlen) Q_DECL_OVERRIDE;
    qint64 write(const char *data, qint64 len) Q_DECL_OVERRIDE;

    bool extension(Extension extension, const ExtensionOption *option = 0, ExtensionReturn *output = 0) Q_DECL_OVERRIDE;
    bool supportsExtension(Extension extension) const Q_DECL_OVERRIDE;

    //FS only!!
    bool open(QIODevice::OpenMode flags, int fd);
    bool open(QIODevice::OpenMode flags, int fd, QFile::FileHandleFlags handleFlags);
    bool open(QIODevice::OpenMode flags, FILE *fh, QFile::FileHandleFlags handleFlags);
    static bool setCurrentPath(const QString &path);
    static QString currentPath(const QString &path = QString());
    static QString homePath();
    static QString rootPath();
    static QString tempPath();
    static QFileInfoList drives();

protected:
    QFSFileEngine(QFSFileEnginePrivate &dd);
};

class Q_AUTOTEST_EXPORT QFSFileEnginePrivate : public QAbstractFileEnginePrivate
{
    Q_DECLARE_PUBLIC(QFSFileEngine)

public:
#ifdef Q_OS_WIN
    static QString longFileName(const QString &path);
#endif

    QFileSystemEntry fileEntry;
    QIODevice::OpenMode openMode;

    bool nativeOpen(QIODevice::OpenMode openMode);
    bool openFh(QIODevice::OpenMode flags, FILE *fh);
    bool openFd(QIODevice::OpenMode flags, int fd);
    bool nativeClose();
    bool closeFdFh();
    bool nativeFlush();
    bool nativeSyncToDisk();
    bool flushFh();
    qint64 nativeSize() const;
#ifndef Q_OS_WIN
    qint64 sizeFdFh() const;
#endif
    qint64 nativePos() const;
    qint64 posFdFh() const;
    bool nativeSeek(qint64);
    bool seekFdFh(qint64);
    qint64 nativeRead(char *data, qint64 maxlen);
    qint64 readFdFh(char *data, qint64 maxlen);
    qint64 nativeReadLine(char *data, qint64 maxlen);
    qint64 readLineFdFh(char *data, qint64 maxlen);
    qint64 nativeWrite(const char *data, qint64 len);
    qint64 writeFdFh(const char *data, qint64 len);
    int nativeHandle() const;
    bool nativeIsSequential() const;
#ifndef Q_OS_WIN
    bool isSequentialFdFh() const;
#endif

    uchar *map(qint64 offset, qint64 size, QFile::MemoryMapFlags flags);
    bool unmap(uchar *ptr);
    void unmapAll();

    mutable QFileSystemMetaData metaData;

    FILE *fh;

#ifdef Q_OS_WIN
    HANDLE fileHandle;
    HANDLE mapHandle;
    QHash<uchar *, DWORD /* offset % AllocationGranularity */> maps;

    mutable int cachedFd;
    mutable DWORD fileAttrib;
#else
    QHash<uchar *, QPair<int /*offset % PageSize*/, size_t /*length + offset % PageSize*/> > maps;
#endif
    int fd;

    enum LastIOCommand
    {
        IOFlushCommand,
        IOReadCommand,
        IOWriteCommand
    };
    LastIOCommand  lastIOCommand;
    bool lastFlushFailed;
    bool closeFileHandle;

    mutable uint is_sequential : 2;
    mutable uint tried_stat : 1;
    mutable uint need_lstat : 1;
    mutable uint is_link : 1;

#if defined(Q_OS_WIN)
    bool doStat(QFileSystemMetaData::MetaDataFlags flags) const;
#else
    bool doStat(QFileSystemMetaData::MetaDataFlags flags = QFileSystemMetaData::PosixStatFlags) const;
#endif
    bool isSymlink() const;

#if defined(Q_OS_WIN32)
    int sysOpen(const QString &, int flags);
#endif

protected:
    QFSFileEnginePrivate();

    void init();

    QAbstractFileEngine::FileFlags getPermissions(QAbstractFileEngine::FileFlags type) const;
};

QT_END_NAMESPACE

#endif // QT_NO_FSFILEENGINE

#endif // QFSFILEENGINE_P_H
