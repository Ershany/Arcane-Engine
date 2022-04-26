/****************************************************************************
**
** Copyright (C) 2012 Giuseppe D'Angelo <dangelog@gmail.com>.
** Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Giuseppe D'Angelo <giuseppe.dangelo@kdab.com>
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

#ifndef QREGULAREXPRESSION_H
#define QREGULAREXPRESSION_H

#include <QtCore/qglobal.h>

#ifndef QT_NO_REGULAREXPRESSION

#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QRegularExpressionMatch;
class QRegularExpressionMatchIterator;
struct QRegularExpressionPrivate;
class QRegularExpression;

Q_CORE_EXPORT uint qHash(const QRegularExpression &key, uint seed = 0) Q_DECL_NOTHROW;

class Q_CORE_EXPORT QRegularExpression
{
public:
    enum PatternOption {
        NoPatternOption                = 0x0000,
        CaseInsensitiveOption          = 0x0001,
        DotMatchesEverythingOption     = 0x0002,
        MultilineOption                = 0x0004,
        ExtendedPatternSyntaxOption    = 0x0008,
        InvertedGreedinessOption       = 0x0010,
        DontCaptureOption              = 0x0020,
        UseUnicodePropertiesOption     = 0x0040,
        OptimizeOnFirstUsageOption     = 0x0080,
        DontAutomaticallyOptimizeOption = 0x0100
    };
    Q_DECLARE_FLAGS(PatternOptions, PatternOption)

    PatternOptions patternOptions() const;
    void setPatternOptions(PatternOptions options);

    QRegularExpression();
    explicit QRegularExpression(const QString &pattern, PatternOptions options = NoPatternOption);
    QRegularExpression(const QRegularExpression &re);
    ~QRegularExpression();
    QRegularExpression &operator=(const QRegularExpression &re);

#ifdef Q_COMPILER_RVALUE_REFS
    QRegularExpression &operator=(QRegularExpression &&re) Q_DECL_NOTHROW
    { d.swap(re.d); return *this; }
#endif

    void swap(QRegularExpression &other) Q_DECL_NOTHROW { d.swap(other.d); }

    QString pattern() const;
    void setPattern(const QString &pattern);

    bool isValid() const;
    int patternErrorOffset() const;
    QString errorString() const;

    int captureCount() const;
    QStringList namedCaptureGroups() const;

    enum MatchType {
        NormalMatch = 0,
        PartialPreferCompleteMatch,
        PartialPreferFirstMatch,
        NoMatch
    };

    enum MatchOption {
        NoMatchOption              = 0x0000,
        AnchoredMatchOption        = 0x0001,
        DontCheckSubjectStringMatchOption = 0x0002
    };
    Q_DECLARE_FLAGS(MatchOptions, MatchOption)

    QRegularExpressionMatch match(const QString &subject,
                                  int offset                = 0,
                                  MatchType matchType       = NormalMatch,
                                  MatchOptions matchOptions = NoMatchOption) const;

    QRegularExpressionMatch match(const QStringRef &subjectRef,
                                  int offset                = 0,
                                  MatchType matchType       = NormalMatch,
                                  MatchOptions matchOptions = NoMatchOption) const;

    QRegularExpressionMatchIterator globalMatch(const QString &subject,
                                                int offset                = 0,
                                                MatchType matchType       = NormalMatch,
                                                MatchOptions matchOptions = NoMatchOption) const;

    QRegularExpressionMatchIterator globalMatch(const QStringRef &subjectRef,
                                                int offset                = 0,
                                                MatchType matchType       = NormalMatch,
                                                MatchOptions matchOptions = NoMatchOption) const;

    void optimize() const;

    static QString escape(const QString &str);

    bool operator==(const QRegularExpression &re) const;
    inline bool operator!=(const QRegularExpression &re) const { return !operator==(re); }

private:
    friend struct QRegularExpressionPrivate;
    friend class QRegularExpressionMatch;
    friend struct QRegularExpressionMatchPrivate;
    friend class QRegularExpressionMatchIterator;
    friend Q_CORE_EXPORT uint qHash(const QRegularExpression &key, uint seed) Q_DECL_NOTHROW;

    QRegularExpression(QRegularExpressionPrivate &dd);
    QExplicitlySharedDataPointer<QRegularExpressionPrivate> d;
};

Q_DECLARE_SHARED(QRegularExpression)
Q_DECLARE_OPERATORS_FOR_FLAGS(QRegularExpression::PatternOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(QRegularExpression::MatchOptions)

#ifndef QT_NO_DATASTREAM
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &out, const QRegularExpression &re);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &in, QRegularExpression &re);
#endif

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug debug, const QRegularExpression &re);
Q_CORE_EXPORT QDebug operator<<(QDebug debug, QRegularExpression::PatternOptions patternOptions);
#endif

struct QRegularExpressionMatchPrivate;

class Q_CORE_EXPORT QRegularExpressionMatch
{
public:
    QRegularExpressionMatch();
    ~QRegularExpressionMatch();
    QRegularExpressionMatch(const QRegularExpressionMatch &match);
    QRegularExpressionMatch &operator=(const QRegularExpressionMatch &match);

#ifdef Q_COMPILER_RVALUE_REFS
    QRegularExpressionMatch &operator=(QRegularExpressionMatch &&match) Q_DECL_NOTHROW
    { d.swap(match.d); return *this; }
#endif
    void swap(QRegularExpressionMatch &other) Q_DECL_NOTHROW { d.swap(other.d); }

    QRegularExpression regularExpression() const;
    QRegularExpression::MatchType matchType() const;
    QRegularExpression::MatchOptions matchOptions() const;

    bool hasMatch() const;
    bool hasPartialMatch() const;

    bool isValid() const;

    int lastCapturedIndex() const;

    QString captured(int nth = 0) const;
    QStringRef capturedRef(int nth = 0) const;

    QString captured(const QString &name) const;
    QStringRef capturedRef(const QString &name) const;

    QStringList capturedTexts() const;

    int capturedStart(int nth = 0) const;
    int capturedLength(int nth = 0) const;
    int capturedEnd(int nth = 0) const;

    int capturedStart(const QString &name) const;
    int capturedLength(const QString &name) const;
    int capturedEnd(const QString &name) const;

private:
    friend class QRegularExpression;
    friend struct QRegularExpressionMatchPrivate;
    friend class QRegularExpressionMatchIterator;

    QRegularExpressionMatch(QRegularExpressionMatchPrivate &dd);
    QSharedDataPointer<QRegularExpressionMatchPrivate> d;
};

Q_DECLARE_SHARED(QRegularExpressionMatch)

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug debug, const QRegularExpressionMatch &match);
#endif

struct QRegularExpressionMatchIteratorPrivate;

class Q_CORE_EXPORT QRegularExpressionMatchIterator
{
public:
    QRegularExpressionMatchIterator();
    ~QRegularExpressionMatchIterator();
    QRegularExpressionMatchIterator(const QRegularExpressionMatchIterator &iterator);
    QRegularExpressionMatchIterator &operator=(const QRegularExpressionMatchIterator &iterator);
#ifdef Q_COMPILER_RVALUE_REFS
    QRegularExpressionMatchIterator &operator=(QRegularExpressionMatchIterator &&iterator) Q_DECL_NOTHROW
    { d.swap(iterator.d); return *this; }
#endif
    void swap(QRegularExpressionMatchIterator &other) Q_DECL_NOTHROW { d.swap(other.d); }

    bool isValid() const;

    bool hasNext() const;
    QRegularExpressionMatch next();
    QRegularExpressionMatch peekNext() const;

    QRegularExpression regularExpression() const;
    QRegularExpression::MatchType matchType() const;
    QRegularExpression::MatchOptions matchOptions() const;

private:
    friend class QRegularExpression;

    QRegularExpressionMatchIterator(QRegularExpressionMatchIteratorPrivate &dd);
    QSharedDataPointer<QRegularExpressionMatchIteratorPrivate> d;
};

Q_DECLARE_SHARED(QRegularExpressionMatchIterator)

QT_END_NAMESPACE

#endif // QT_NO_REGULAREXPRESSION

#endif // QREGULAREXPRESSION_H
