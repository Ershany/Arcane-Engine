/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Copyright (C) 2016 Intel Corporation.
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

#ifndef QSTRING_H
#define QSTRING_H

#if defined(QT_NO_CAST_FROM_ASCII) && defined(QT_RESTRICTED_CAST_FROM_ASCII)
#error QT_NO_CAST_FROM_ASCII and QT_RESTRICTED_CAST_FROM_ASCII must not be defined at the same time
#endif

#include <QtCore/qchar.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qrefcount.h>
#include <QtCore/qnamespace.h>

#include <string>
#include <iterator>

#if defined(Q_OS_ANDROID) && !defined(ANDROID_HAS_WSTRING)
// std::wstring is disabled on android's glibc, as bionic lacks certain features
// that libstdc++ checks for (like mbcslen).
namespace std
{
    typedef basic_string<wchar_t> wstring;
}
#endif

#include <stdarg.h>

#ifdef truncate
#error qstring.h must be included before any header file that defines truncate
#endif

#if defined(Q_OS_DARWIN) || defined(Q_QDOC)
Q_FORWARD_DECLARE_CF_TYPE(CFString);
Q_FORWARD_DECLARE_OBJC_CLASS(NSString);
#endif

QT_BEGIN_NAMESPACE

class QCharRef;
class QRegExp;
class QRegularExpression;
class QRegularExpressionMatch;
class QString;
class QStringList;
class QTextCodec;
class QStringRef;
template <typename T> class QVector;

class QLatin1String
{
public:
    Q_DECL_CONSTEXPR inline QLatin1String() Q_DECL_NOTHROW : m_size(0), m_data(Q_NULLPTR) {}
    Q_DECL_CONSTEXPR inline explicit QLatin1String(const char *s) Q_DECL_NOTHROW : m_size(s ? int(strlen(s)) : 0), m_data(s) {}
    Q_DECL_CONSTEXPR inline explicit QLatin1String(const char *s, int sz) Q_DECL_NOTHROW : m_size(sz), m_data(s) {}
    inline explicit QLatin1String(const QByteArray &s) Q_DECL_NOTHROW : m_size(int(qstrnlen(s.constData(), s.size()))), m_data(s.constData()) {}

    Q_DECL_CONSTEXPR const char *latin1() const Q_DECL_NOTHROW { return m_data; }
    Q_DECL_CONSTEXPR int size() const Q_DECL_NOTHROW { return m_size; }
    Q_DECL_CONSTEXPR const char *data() const Q_DECL_NOTHROW { return m_data; }

    Q_DECL_CONSTEXPR QLatin1Char at(int i) const { return QLatin1Char(m_data[i]); }
    Q_DECL_CONSTEXPR QLatin1Char operator[](int i) const { return at(i); }

    Q_DECL_CONSTEXPR QLatin1String mid(int pos) const
    { return QLatin1String(m_data + pos, m_size - pos); }
    Q_DECL_CONSTEXPR QLatin1String mid(int pos, int n) const
    { return QLatin1String(m_data + pos, n); }
    Q_DECL_CONSTEXPR QLatin1String left(int n) const
    { return QLatin1String(m_data, n); }
    Q_DECL_CONSTEXPR QLatin1String right(int n) const
    { return QLatin1String(m_data + m_size - n, n); }

    inline bool operator==(const QString &s) const Q_DECL_NOTHROW;
    inline bool operator!=(const QString &s) const Q_DECL_NOTHROW;
    inline bool operator>(const QString &s) const Q_DECL_NOTHROW;
    inline bool operator<(const QString &s) const Q_DECL_NOTHROW;
    inline bool operator>=(const QString &s) const Q_DECL_NOTHROW;
    inline bool operator<=(const QString &s) const Q_DECL_NOTHROW;

#if !defined(QT_NO_CAST_FROM_ASCII) && !defined(QT_RESTRICTED_CAST_FROM_ASCII)
    inline QT_ASCII_CAST_WARN bool operator==(const char *s) const;
    inline QT_ASCII_CAST_WARN bool operator!=(const char *s) const;
    inline QT_ASCII_CAST_WARN bool operator<(const char *s) const;
    inline QT_ASCII_CAST_WARN bool operator>(const char *s) const;
    inline QT_ASCII_CAST_WARN bool operator<=(const char *s) const;
    inline QT_ASCII_CAST_WARN bool operator>=(const char *s) const;

    inline QT_ASCII_CAST_WARN bool operator==(const QByteArray &s) const;
    inline QT_ASCII_CAST_WARN bool operator!=(const QByteArray &s) const;
    inline QT_ASCII_CAST_WARN bool operator<(const QByteArray &s) const;
    inline QT_ASCII_CAST_WARN bool operator>(const QByteArray &s) const;
    inline QT_ASCII_CAST_WARN bool operator<=(const QByteArray &s) const;
    inline QT_ASCII_CAST_WARN bool operator>=(const QByteArray &s) const;
#endif // !defined(QT_NO_CAST_FROM_ASCII) && !defined(QT_RESTRICTED_CAST_FROM_ASCII)

private:
    int m_size;
    const char *m_data;
};
Q_DECLARE_TYPEINFO(QLatin1String, Q_MOVABLE_TYPE);

// Qt 4.x compatibility
typedef QLatin1String QLatin1Literal;


typedef QTypedArrayData<ushort> QStringData;

#if defined(Q_OS_WIN) && !defined(Q_COMPILER_UNICODE_STRINGS)
// fall back to wchar_t if the a Windows compiler does not
// support Unicode string literals, assuming wchar_t is 2 bytes
// on that platform (sanity-checked by static_assert further below)

#if defined(Q_CC_MSVC)
#    define QT_UNICODE_LITERAL_II(str) L##str
#else
#    define QT_UNICODE_LITERAL_II(str) L"" str
#endif
typedef wchar_t qunicodechar;

#else
// all our supported compilers support Unicode string literals,
// even if their Q_COMPILER_UNICODE_STRING has been revoked due
// to lacking stdlib support. But QStringLiteral only needs the
// core language feature, so just use u"" here unconditionally:

#define QT_UNICODE_LITERAL_II(str) u"" str
typedef char16_t qunicodechar;

#endif

Q_STATIC_ASSERT_X(sizeof(qunicodechar) == 2,
        "qunicodechar must typedef an integral type of size 2");

#define QT_UNICODE_LITERAL(str) QT_UNICODE_LITERAL_II(str)
#define QStringLiteral(str) \
    ([]() Q_DECL_NOEXCEPT -> QString { \
        enum { Size = sizeof(QT_UNICODE_LITERAL(str))/2 - 1 }; \
        static const QStaticStringData<Size> qstring_literal = { \
            Q_STATIC_STRING_DATA_HEADER_INITIALIZER(Size), \
            QT_UNICODE_LITERAL(str) }; \
        QStringDataPtr holder = { qstring_literal.data_ptr() }; \
        const QString qstring_literal_temp(holder); \
        return qstring_literal_temp; \
    }()) \
    /**/

#define Q_STATIC_STRING_DATA_HEADER_INITIALIZER_WITH_OFFSET(size, offset) \
    { Q_REFCOUNT_INITIALIZE_STATIC, size, 0, 0, offset } \
    /**/

#define Q_STATIC_STRING_DATA_HEADER_INITIALIZER(size) \
    Q_STATIC_STRING_DATA_HEADER_INITIALIZER_WITH_OFFSET(size, sizeof(QStringData)) \
    /**/

template <int N>
struct QStaticStringData
{
    QArrayData str;
    qunicodechar data[N + 1];

    QStringData *data_ptr() const
    {
        Q_ASSERT(str.ref.isStatic());
        return const_cast<QStringData *>(static_cast<const QStringData*>(&str));
    }
};

struct QStringDataPtr
{
    QStringData *ptr;
};

class Q_CORE_EXPORT QString
{
public:
    typedef QStringData Data;

    inline QString() Q_DECL_NOTHROW;
    explicit QString(const QChar *unicode, int size = -1);
    QString(QChar c);
    QString(int size, QChar c);
    inline QString(QLatin1String latin1);
    inline QString(const QString &) Q_DECL_NOTHROW;
    inline ~QString();
    QString &operator=(QChar c);
    QString &operator=(const QString &) Q_DECL_NOTHROW;
    QString &operator=(QLatin1String latin1);
#ifdef Q_COMPILER_RVALUE_REFS
    inline QString(QString && other) Q_DECL_NOTHROW : d(other.d) { other.d = Data::sharedNull(); }
    inline QString &operator=(QString &&other) Q_DECL_NOTHROW
    { qSwap(d, other.d); return *this; }
#endif
    inline void swap(QString &other) Q_DECL_NOTHROW { qSwap(d, other.d); }
    inline int size() const { return d->size; }
    inline int count() const { return d->size; }
    inline int length() const;
    inline bool isEmpty() const;
    void resize(int size);
    void resize(int size, QChar fillChar);

    QString &fill(QChar c, int size = -1);
    void truncate(int pos);
    void chop(int n);

    int capacity() const;
    inline void reserve(int size);
    inline void squeeze();

    inline const QChar *unicode() const;
    inline QChar *data();
    inline const QChar *data() const;
    inline const QChar *constData() const;

    inline void detach();
    inline bool isDetached() const;
    inline bool isSharedWith(const QString &other) const { return d == other.d; }
    void clear();

    inline const QChar at(int i) const;
    const QChar operator[](int i) const;
    QCharRef operator[](int i);
    const QChar operator[](uint i) const;
    QCharRef operator[](uint i);

    Q_REQUIRED_RESULT QString arg(qlonglong a, int fieldwidth=0, int base=10,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(qulonglong a, int fieldwidth=0, int base=10,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(long a, int fieldwidth=0, int base=10,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(ulong a, int fieldwidth=0, int base=10,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(int a, int fieldWidth = 0, int base = 10,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(uint a, int fieldWidth = 0, int base = 10,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(short a, int fieldWidth = 0, int base = 10,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(ushort a, int fieldWidth = 0, int base = 10,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(double a, int fieldWidth = 0, char fmt = 'g', int prec = -1,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(char a, int fieldWidth = 0,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(QChar a, int fieldWidth = 0,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(const QString &a, int fieldWidth = 0,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(const QString &a1, const QString &a2) const;
    Q_REQUIRED_RESULT QString arg(const QString &a1, const QString &a2, const QString &a3) const;
    Q_REQUIRED_RESULT QString arg(const QString &a1, const QString &a2, const QString &a3,
                const QString &a4) const;
    Q_REQUIRED_RESULT QString arg(const QString &a1, const QString &a2, const QString &a3,
                const QString &a4, const QString &a5) const;
    Q_REQUIRED_RESULT QString arg(const QString &a1, const QString &a2, const QString &a3,
                const QString &a4, const QString &a5, const QString &a6) const;
    Q_REQUIRED_RESULT QString arg(const QString &a1, const QString &a2, const QString &a3,
                const QString &a4, const QString &a5, const QString &a6,
                const QString &a7) const;
    Q_REQUIRED_RESULT QString arg(const QString &a1, const QString &a2, const QString &a3,
                const QString &a4, const QString &a5, const QString &a6,
                const QString &a7, const QString &a8) const;
    Q_REQUIRED_RESULT QString arg(const QString &a1, const QString &a2, const QString &a3,
                const QString &a4, const QString &a5, const QString &a6,
                const QString &a7, const QString &a8, const QString &a9) const;

    QString &vsprintf(const char *format, va_list ap) Q_ATTRIBUTE_FORMAT_PRINTF(2, 0);
    QString &sprintf(const char *format, ...) Q_ATTRIBUTE_FORMAT_PRINTF(2, 3);
    static QString vasprintf(const char *format, va_list ap) Q_ATTRIBUTE_FORMAT_PRINTF(1, 0);
    static QString asprintf(const char *format, ...) Q_ATTRIBUTE_FORMAT_PRINTF(1, 2);

    int indexOf(QChar c, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int indexOf(const QString &s, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int indexOf(QLatin1String s, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int indexOf(const QStringRef &s, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int lastIndexOf(QChar c, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int lastIndexOf(const QString &s, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int lastIndexOf(QLatin1String s, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int lastIndexOf(const QStringRef &s, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    inline bool contains(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    inline bool contains(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    inline bool contains(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    inline bool contains(const QStringRef &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int count(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int count(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int count(const QStringRef &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

#ifndef QT_NO_REGEXP
    int indexOf(const QRegExp &, int from = 0) const;
    int lastIndexOf(const QRegExp &, int from = -1) const;
    inline bool contains(const QRegExp &rx) const { return indexOf(rx) != -1; }
    int count(const QRegExp &) const;

    int indexOf(QRegExp &, int from = 0) const;
    int lastIndexOf(QRegExp &, int from = -1) const;
    inline bool contains(QRegExp &rx) const { return indexOf(rx) != -1; }
#endif

#ifndef QT_NO_REGULAREXPRESSION
    int indexOf(const QRegularExpression &re, int from = 0) const;
    int indexOf(const QRegularExpression &re, int from, QRegularExpressionMatch *rmatch) const; // ### Qt 6: merge overloads
    int lastIndexOf(const QRegularExpression &re, int from = -1) const;
    int lastIndexOf(const QRegularExpression &re, int from, QRegularExpressionMatch *rmatch) const; // ### Qt 6: merge overloads
    bool contains(const QRegularExpression &re) const;
    bool contains(const QRegularExpression &re, QRegularExpressionMatch *match) const; // ### Qt 6: merge overloads
    int count(const QRegularExpression &re) const;
#endif

    enum SectionFlag {
        SectionDefault             = 0x00,
        SectionSkipEmpty           = 0x01,
        SectionIncludeLeadingSep   = 0x02,
        SectionIncludeTrailingSep  = 0x04,
        SectionCaseInsensitiveSeps = 0x08
    };
    Q_DECLARE_FLAGS(SectionFlags, SectionFlag)

    QString section(QChar sep, int start, int end = -1, SectionFlags flags = SectionDefault) const;
    QString section(const QString &in_sep, int start, int end = -1, SectionFlags flags = SectionDefault) const;
#ifndef QT_NO_REGEXP
    QString section(const QRegExp &reg, int start, int end = -1, SectionFlags flags = SectionDefault) const;
#endif
#ifndef QT_NO_REGULAREXPRESSION
    QString section(const QRegularExpression &re, int start, int end = -1, SectionFlags flags = SectionDefault) const;
#endif
    Q_REQUIRED_RESULT QString left(int n) const;
    Q_REQUIRED_RESULT QString right(int n) const;
    Q_REQUIRED_RESULT QString mid(int position, int n = -1) const;
    Q_REQUIRED_RESULT QStringRef leftRef(int n) const;
    Q_REQUIRED_RESULT QStringRef rightRef(int n) const;
    Q_REQUIRED_RESULT QStringRef midRef(int position, int n = -1) const;

    bool startsWith(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool startsWith(const QStringRef &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool startsWith(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool startsWith(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool endsWith(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool endsWith(const QStringRef &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool endsWith(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool endsWith(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    Q_REQUIRED_RESULT QString leftJustified(int width, QChar fill = QLatin1Char(' '), bool trunc = false) const;
    Q_REQUIRED_RESULT QString rightJustified(int width, QChar fill = QLatin1Char(' '), bool trunc = false) const;

#if defined(Q_COMPILER_REF_QUALIFIERS) && !defined(QT_COMPILING_QSTRING_COMPAT_CPP) && !defined(Q_CLANG_QDOC)
#  if defined(Q_CC_GNU) && !defined(Q_CC_CLANG) && !defined(Q_CC_INTEL) && !QT_HAS_CPP_ATTRIBUTE(nodiscard)
    // required due to https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61941
#    pragma push_macro("Q_REQUIRED_RESULT")
#    undef Q_REQUIRED_RESULT
#    define Q_REQUIRED_RESULT
#    define Q_REQUIRED_RESULT_pushed
#  endif
    Q_REQUIRED_RESULT Q_ALWAYS_INLINE QString toLower() const &
    { return toLower_helper(*this); }
    Q_REQUIRED_RESULT Q_ALWAYS_INLINE QString toLower() &&
    { return toLower_helper(*this); }
    Q_REQUIRED_RESULT Q_ALWAYS_INLINE QString toUpper() const &
    { return toUpper_helper(*this); }
    Q_REQUIRED_RESULT Q_ALWAYS_INLINE QString toUpper() &&
    { return toUpper_helper(*this); }
    Q_REQUIRED_RESULT Q_ALWAYS_INLINE QString toCaseFolded() const &
    { return toCaseFolded_helper(*this); }
    Q_REQUIRED_RESULT Q_ALWAYS_INLINE QString toCaseFolded() &&
    { return toCaseFolded_helper(*this); }
    Q_REQUIRED_RESULT Q_ALWAYS_INLINE QString trimmed() const &
    { return trimmed_helper(*this); }
    Q_REQUIRED_RESULT Q_ALWAYS_INLINE QString trimmed() &&
    { return trimmed_helper(*this); }
    Q_REQUIRED_RESULT Q_ALWAYS_INLINE QString simplified() const &
    { return simplified_helper(*this); }
    Q_REQUIRED_RESULT Q_ALWAYS_INLINE QString simplified() &&
    { return simplified_helper(*this); }
#  ifdef Q_REQUIRED_RESULT_pushed
#    pragma pop_macro("Q_REQUIRED_RESULT")
#  endif
#else
    Q_REQUIRED_RESULT QString toLower() const;
    Q_REQUIRED_RESULT QString toUpper() const;
    Q_REQUIRED_RESULT QString toCaseFolded() const;
    Q_REQUIRED_RESULT QString trimmed() const;
    Q_REQUIRED_RESULT QString simplified() const;
#endif
    Q_REQUIRED_RESULT QString toHtmlEscaped() const;

    QString &insert(int i, QChar c);
    QString &insert(int i, const QChar *uc, int len);
    inline QString &insert(int i, const QString &s) { return insert(i, s.constData(), s.length()); }
    inline QString &insert(int i, const QStringRef &s);
    QString &insert(int i, QLatin1String s);
    QString &append(QChar c);
    QString &append(const QChar *uc, int len);
    QString &append(const QString &s);
    QString &append(const QStringRef &s);
    QString &append(QLatin1String s);
    inline QString &prepend(QChar c) { return insert(0, c); }
    inline QString &prepend(const QChar *uc, int len) { return insert(0, uc, len); }
    inline QString &prepend(const QString &s) { return insert(0, s); }
    inline QString &prepend(const QStringRef &s) { return insert(0, s); }
    inline QString &prepend(QLatin1String s) { return insert(0, s); }

    inline QString &operator+=(QChar c) {
        if (d->ref.isShared() || uint(d->size) + 2u > d->alloc)
            reallocData(uint(d->size) + 2u, true);
        d->data()[d->size++] = c.unicode();
        d->data()[d->size] = '\0';
        return *this;
    }

    inline QString &operator+=(QChar::SpecialCharacter c) { return append(QChar(c)); }
    inline QString &operator+=(const QString &s) { return append(s); }
    inline QString &operator+=(const QStringRef &s) { return append(s); }
    inline QString &operator+=(QLatin1String s) { return append(s); }

    QString &remove(int i, int len);
    QString &remove(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &remove(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(int i, int len, QChar after);
    QString &replace(int i, int len, const QChar *s, int slen);
    QString &replace(int i, int len, const QString &after);
    QString &replace(QChar before, QChar after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(const QChar *before, int blen, const QChar *after, int alen, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(QLatin1String before, QLatin1String after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(QLatin1String before, const QString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(const QString &before, QLatin1String after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(const QString &before, const QString &after,
                     Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(QChar c, const QString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(QChar c, QLatin1String after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
#ifndef QT_NO_REGEXP
    QString &replace(const QRegExp &rx, const QString &after);
    inline QString &remove(const QRegExp &rx)
    { return replace(rx, QString()); }
#endif
#ifndef QT_NO_REGULAREXPRESSION
    QString &replace(const QRegularExpression &re, const QString  &after);
    inline QString &remove(const QRegularExpression &re)
    { return replace(re, QString()); }
#endif

    enum SplitBehavior { KeepEmptyParts, SkipEmptyParts };

    Q_REQUIRED_RESULT QStringList split(const QString &sep, SplitBehavior behavior = KeepEmptyParts,
                      Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    Q_REQUIRED_RESULT QVector<QStringRef> splitRef(const QString &sep, SplitBehavior behavior = KeepEmptyParts,
                      Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    Q_REQUIRED_RESULT QStringList split(QChar sep, SplitBehavior behavior = KeepEmptyParts,
                      Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    Q_REQUIRED_RESULT QVector<QStringRef> splitRef(QChar sep, SplitBehavior behavior = KeepEmptyParts,
                      Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
#ifndef QT_NO_REGEXP
    Q_REQUIRED_RESULT QStringList split(const QRegExp &sep, SplitBehavior behavior = KeepEmptyParts) const;
    Q_REQUIRED_RESULT QVector<QStringRef> splitRef(const QRegExp &sep, SplitBehavior behavior = KeepEmptyParts) const;
#endif
#ifndef QT_NO_REGULAREXPRESSION
    Q_REQUIRED_RESULT QStringList split(const QRegularExpression &sep, SplitBehavior behavior = KeepEmptyParts) const;
    Q_REQUIRED_RESULT QVector<QStringRef> splitRef(const QRegularExpression &sep, SplitBehavior behavior = KeepEmptyParts) const;
#endif
    enum NormalizationForm {
        NormalizationForm_D,
        NormalizationForm_C,
        NormalizationForm_KD,
        NormalizationForm_KC
    };
    Q_REQUIRED_RESULT QString normalized(NormalizationForm mode, QChar::UnicodeVersion version = QChar::Unicode_Unassigned) const;

    Q_REQUIRED_RESULT QString repeated(int times) const;

    const ushort *utf16() const;

#if defined(Q_COMPILER_REF_QUALIFIERS) && !defined(QT_COMPILING_QSTRING_COMPAT_CPP) && !defined(Q_CLANG_QDOC)
    Q_REQUIRED_RESULT QByteArray toLatin1() const &
    { return toLatin1_helper(*this); }
    Q_REQUIRED_RESULT QByteArray toLatin1() &&
    { return toLatin1_helper_inplace(*this); }
    Q_REQUIRED_RESULT QByteArray toUtf8() const &
    { return toUtf8_helper(*this); }
    Q_REQUIRED_RESULT QByteArray toUtf8() &&
    { return toUtf8_helper(*this); }
    Q_REQUIRED_RESULT QByteArray toLocal8Bit() const &
    { return toLocal8Bit_helper(isNull() ? nullptr : constData(), size()); }
    Q_REQUIRED_RESULT QByteArray toLocal8Bit() &&
    { return toLocal8Bit_helper(isNull() ? nullptr : constData(), size()); }
#else
    Q_REQUIRED_RESULT QByteArray toLatin1() const;
    Q_REQUIRED_RESULT QByteArray toUtf8() const;
    Q_REQUIRED_RESULT QByteArray toLocal8Bit() const;
#endif
    Q_REQUIRED_RESULT QVector<uint> toUcs4() const;

    // note - this are all inline so we can benefit from strlen() compile time optimizations
    static inline QString fromLatin1(const char *str, int size = -1)
    {
        QStringDataPtr dataPtr = { fromLatin1_helper(str, (str && size == -1) ? int(strlen(str)) : size) };
        return QString(dataPtr);
    }
    static inline QString fromUtf8(const char *str, int size = -1)
    {
        return fromUtf8_helper(str, (str && size == -1) ? int(strlen(str)) : size);
    }
    static inline QString fromLocal8Bit(const char *str, int size = -1)
    {
        return fromLocal8Bit_helper(str, (str && size == -1) ? int(strlen(str)) : size);
    }
    static inline QString fromLatin1(const QByteArray &str)
    { return str.isNull() ? QString() : fromLatin1(str.data(), qstrnlen(str.constData(), str.size())); }
    static inline QString fromUtf8(const QByteArray &str)
    { return str.isNull() ? QString() : fromUtf8(str.data(), qstrnlen(str.constData(), str.size())); }
    static inline QString fromLocal8Bit(const QByteArray &str)
    { return str.isNull() ? QString() : fromLocal8Bit(str.data(), qstrnlen(str.constData(), str.size())); }
    static QString fromUtf16(const ushort *, int size = -1);
    static QString fromUcs4(const uint *, int size = -1);
    static QString fromRawData(const QChar *, int size);

#if defined(Q_COMPILER_UNICODE_STRINGS)
    static QString fromUtf16(const char16_t *str, int size = -1)
    { return fromUtf16(reinterpret_cast<const ushort *>(str), size); }
    static QString fromUcs4(const char32_t *str, int size = -1)
    { return fromUcs4(reinterpret_cast<const uint *>(str), size); }
#endif

#if QT_DEPRECATED_SINCE(5, 0)
    QT_DEPRECATED static inline QString fromAscii(const char *str, int size = -1)
    { return fromLatin1(str, size); }
    QT_DEPRECATED static inline QString fromAscii(const QByteArray &str)
    { return fromLatin1(str); }
    Q_REQUIRED_RESULT QByteArray toAscii() const
    { return toLatin1(); }
#endif

    inline int toWCharArray(wchar_t *array) const;
    Q_REQUIRED_RESULT static inline QString fromWCharArray(const wchar_t *string, int size = -1);

    QString &setRawData(const QChar *unicode, int size);
    QString &setUnicode(const QChar *unicode, int size);
    inline QString &setUtf16(const ushort *utf16, int size);

    int compare(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const Q_DECL_NOTHROW;
    int compare(QLatin1String other, Qt::CaseSensitivity cs = Qt::CaseSensitive) const Q_DECL_NOTHROW;

    static inline int compare(const QString &s1, const QString &s2,
                              Qt::CaseSensitivity cs = Qt::CaseSensitive) Q_DECL_NOTHROW
    { return s1.compare(s2, cs); }

    static inline int compare(const QString &s1, QLatin1String s2,
                              Qt::CaseSensitivity cs = Qt::CaseSensitive) Q_DECL_NOTHROW
    { return s1.compare(s2, cs); }
    static inline int compare(QLatin1String s1, const QString &s2,
                              Qt::CaseSensitivity cs = Qt::CaseSensitive) Q_DECL_NOTHROW
    { return -s2.compare(s1, cs); }

    inline int compare(const QStringRef &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const Q_DECL_NOTHROW;
    static int compare(const QString &s1, const QStringRef &s2,
                       Qt::CaseSensitivity = Qt::CaseSensitive) Q_DECL_NOTHROW;

    int localeAwareCompare(const QString& s) const;
    static int localeAwareCompare(const QString& s1, const QString& s2)
    { return s1.localeAwareCompare(s2); }

    int localeAwareCompare(const QStringRef &s) const;
    static int localeAwareCompare(const QString& s1, const QStringRef& s2);

    // ### Qt6: make inline except for the long long versions
    short  toShort(bool *ok=Q_NULLPTR, int base=10) const;
    ushort toUShort(bool *ok=Q_NULLPTR, int base=10) const;
    int toInt(bool *ok=Q_NULLPTR, int base=10) const;
    uint toUInt(bool *ok=Q_NULLPTR, int base=10) const;
    long toLong(bool *ok=Q_NULLPTR, int base=10) const;
    ulong toULong(bool *ok=Q_NULLPTR, int base=10) const;
    qlonglong toLongLong(bool *ok=Q_NULLPTR, int base=10) const;
    qulonglong toULongLong(bool *ok=Q_NULLPTR, int base=10) const;
    float toFloat(bool *ok=Q_NULLPTR) const;
    double toDouble(bool *ok=Q_NULLPTR) const;

    QString &setNum(short, int base=10);
    QString &setNum(ushort, int base=10);
    QString &setNum(int, int base=10);
    QString &setNum(uint, int base=10);
    QString &setNum(long, int base=10);
    QString &setNum(ulong, int base=10);
    QString &setNum(qlonglong, int base=10);
    QString &setNum(qulonglong, int base=10);
    QString &setNum(float, char f='g', int prec=6);
    QString &setNum(double, char f='g', int prec=6);

    static QString number(int, int base=10);
    static QString number(uint, int base=10);
    static QString number(long, int base=10);
    static QString number(ulong, int base=10);
    static QString number(qlonglong, int base=10);
    static QString number(qulonglong, int base=10);
    static QString number(double, char f='g', int prec=6);

    friend Q_CORE_EXPORT bool operator==(const QString &s1, const QString &s2) Q_DECL_NOTHROW;
    friend Q_CORE_EXPORT bool operator<(const QString &s1, const QString &s2) Q_DECL_NOTHROW;
    friend inline bool operator>(const QString &s1, const QString &s2) Q_DECL_NOTHROW { return s2 < s1; }
    friend inline bool operator!=(const QString &s1, const QString &s2) Q_DECL_NOTHROW { return !(s1 == s2); }
    friend inline bool operator<=(const QString &s1, const QString &s2) Q_DECL_NOTHROW { return !(s1 > s2); }
    friend inline bool operator>=(const QString &s1, const QString &s2) Q_DECL_NOTHROW { return !(s1 < s2); }

    bool operator==(QLatin1String s) const Q_DECL_NOTHROW;
    bool operator<(QLatin1String s) const Q_DECL_NOTHROW;
    bool operator>(QLatin1String s) const Q_DECL_NOTHROW;
    inline bool operator!=(QLatin1String s) const Q_DECL_NOTHROW { return !operator==(s); }
    inline bool operator<=(QLatin1String s) const Q_DECL_NOTHROW { return !operator>(s); }
    inline bool operator>=(QLatin1String s) const Q_DECL_NOTHROW { return !operator<(s); }

    // ASCII compatibility
#if defined(QT_RESTRICTED_CAST_FROM_ASCII)
    template <int N>
    inline QString(const char (&ch)[N])
        : d(fromAscii_helper(ch, N - 1))
    {}
    template <int N>
    inline QString &operator=(const char (&ch)[N])
    { return (*this = fromLatin1(ch, N - 1)); }
#endif
#if !defined(QT_NO_CAST_FROM_ASCII) && !defined(QT_RESTRICTED_CAST_FROM_ASCII)
    inline QT_ASCII_CAST_WARN QString(const char *ch)
        : d(fromAscii_helper(ch, ch ? int(strlen(ch)) : -1))
    {}
    inline QT_ASCII_CAST_WARN QString(const QByteArray &a)
        : d(fromAscii_helper(a.constData(), qstrnlen(a.constData(), a.size())))
    {}
    inline QT_ASCII_CAST_WARN QString &operator=(const char *ch)
    { return (*this = fromUtf8(ch)); }
    inline QT_ASCII_CAST_WARN QString &operator=(const QByteArray &a)
    { return (*this = fromUtf8(a)); }
    inline QT_ASCII_CAST_WARN QString &operator=(char c)
    { return (*this = QChar::fromLatin1(c)); }

    // these are needed, so it compiles with STL support enabled
    inline QT_ASCII_CAST_WARN QString &prepend(const char *s)
    { return prepend(QString::fromUtf8(s)); }
    inline QT_ASCII_CAST_WARN QString &prepend(const QByteArray &s)
    { return prepend(QString::fromUtf8(s)); }
    inline QT_ASCII_CAST_WARN QString &append(const char *s)
    { return append(QString::fromUtf8(s)); }
    inline QT_ASCII_CAST_WARN QString &append(const QByteArray &s)
    { return append(QString::fromUtf8(s)); }
    inline QT_ASCII_CAST_WARN QString &insert(int i, const char *s)
    { return insert(i, QString::fromUtf8(s)); }
    inline QT_ASCII_CAST_WARN QString &insert(int i, const QByteArray &s)
    { return insert(i, QString::fromUtf8(s)); }
    inline QT_ASCII_CAST_WARN QString &operator+=(const char *s)
    { return append(QString::fromUtf8(s)); }
    inline QT_ASCII_CAST_WARN QString &operator+=(const QByteArray &s)
    { return append(QString::fromUtf8(s)); }
    inline QT_ASCII_CAST_WARN QString &operator+=(char c)
    { return append(QChar::fromLatin1(c)); }

    inline QT_ASCII_CAST_WARN bool operator==(const char *s) const;
    inline QT_ASCII_CAST_WARN bool operator!=(const char *s) const;
    inline QT_ASCII_CAST_WARN bool operator<(const char *s) const;
    inline QT_ASCII_CAST_WARN bool operator<=(const char *s) const;
    inline QT_ASCII_CAST_WARN bool operator>(const char *s) const;
    inline QT_ASCII_CAST_WARN bool operator>=(const char *s) const;

    inline QT_ASCII_CAST_WARN bool operator==(const QByteArray &s) const;
    inline QT_ASCII_CAST_WARN bool operator!=(const QByteArray &s) const;
    inline QT_ASCII_CAST_WARN bool operator<(const QByteArray &s) const;
    inline QT_ASCII_CAST_WARN bool operator>(const QByteArray &s) const;
    inline QT_ASCII_CAST_WARN bool operator<=(const QByteArray &s) const;
    inline QT_ASCII_CAST_WARN bool operator>=(const QByteArray &s) const;

    friend inline QT_ASCII_CAST_WARN bool operator==(const char *s1, const QString &s2);
    friend inline QT_ASCII_CAST_WARN bool operator!=(const char *s1, const QString &s2);
    friend inline QT_ASCII_CAST_WARN bool operator<(const char *s1, const QString &s2);
    friend inline QT_ASCII_CAST_WARN bool operator>(const char *s1, const QString &s2);
    friend inline QT_ASCII_CAST_WARN bool operator<=(const char *s1, const QString &s2);
    friend inline QT_ASCII_CAST_WARN bool operator>=(const char *s1, const QString &s2);

    friend inline QT_ASCII_CAST_WARN bool operator==(const char *s1, const QStringRef &s2);
    friend inline QT_ASCII_CAST_WARN bool operator!=(const char *s1, const QStringRef &s2);
    friend inline QT_ASCII_CAST_WARN bool operator<(const char *s1, const QStringRef &s2);
    friend inline QT_ASCII_CAST_WARN bool operator>(const char *s1, const QStringRef &s2);
    friend inline QT_ASCII_CAST_WARN bool operator<=(const char *s1, const QStringRef &s2);
    friend inline QT_ASCII_CAST_WARN bool operator>=(const char *s1, const QStringRef &s2);
#endif

    typedef QChar *iterator;
    typedef const QChar *const_iterator;
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    inline iterator begin();
    inline const_iterator begin() const;
    inline const_iterator cbegin() const;
    inline const_iterator constBegin() const;
    inline iterator end();
    inline const_iterator end() const;
    inline const_iterator cend() const;
    inline const_iterator constEnd() const;
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }

    // STL compatibility
    typedef int size_type;
    typedef qptrdiff difference_type;
    typedef const QChar & const_reference;
    typedef QChar & reference;
    typedef QChar *pointer;
    typedef const QChar *const_pointer;
    typedef QChar value_type;
    inline void push_back(QChar c) { append(c); }
    inline void push_back(const QString &s) { append(s); }
    inline void push_front(QChar c) { prepend(c); }
    inline void push_front(const QString &s) { prepend(s); }

    static inline QString fromStdString(const std::string &s);
    inline std::string toStdString() const;
    static inline QString fromStdWString(const std::wstring &s);
    inline std::wstring toStdWString() const;

#if defined(Q_STDLIB_UNICODE_STRINGS) || defined(Q_QDOC)
    static inline QString fromStdU16String(const std::u16string &s);
    inline std::u16string toStdU16String() const;
    static inline QString fromStdU32String(const std::u32string &s);
    inline std::u32string toStdU32String() const;
#endif

#if defined(Q_OS_DARWIN) || defined(Q_QDOC)
    static QString fromCFString(CFStringRef string);
    CFStringRef toCFString() const Q_DECL_CF_RETURNS_RETAINED;
    static QString fromNSString(const NSString *string);
    NSString *toNSString() const Q_DECL_NS_RETURNS_AUTORELEASED;
#endif
    // compatibility
#if QT_DEPRECATED_SINCE(5, 9)
    struct Null { };
    QT_DEPRECATED_X("use QString()")
    static const Null null;
    inline QString(const Null &): d(Data::sharedNull()) {}
    inline QString &operator=(const Null &) { *this = QString(); return *this; }
#endif
    inline bool isNull() const { return d == Data::sharedNull(); }


    bool isSimpleText() const;
    bool isRightToLeft() const;

    QString(int size, Qt::Initialization);
    Q_DECL_CONSTEXPR inline QString(QStringDataPtr dd) : d(dd.ptr) {}

private:
#if defined(QT_NO_CAST_FROM_ASCII)
    QString &operator+=(const char *s);
    QString &operator+=(const QByteArray &s);
    QString(const char *ch);
    QString(const QByteArray &a);
    QString &operator=(const char  *ch);
    QString &operator=(const QByteArray &a);
#endif

    Data *d;

    friend inline bool operator==(QChar, const QString &) Q_DECL_NOTHROW;
    friend inline bool operator< (QChar, const QString &) Q_DECL_NOTHROW;
    friend inline bool operator> (QChar, const QString &) Q_DECL_NOTHROW;
    friend inline bool operator==(QChar, const QStringRef &) Q_DECL_NOTHROW;
    friend inline bool operator< (QChar, const QStringRef &) Q_DECL_NOTHROW;
    friend inline bool operator> (QChar, const QStringRef &) Q_DECL_NOTHROW;
    friend inline bool operator==(QChar, QLatin1String) Q_DECL_NOTHROW;
    friend inline bool operator< (QChar, QLatin1String) Q_DECL_NOTHROW;
    friend inline bool operator> (QChar, QLatin1String) Q_DECL_NOTHROW;

    void reallocData(uint alloc, bool grow = false);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    void expand(int i);
#endif
    QString multiArg(int numArgs, const QString **args) const;
    static int compare_helper(const QChar *data1, int length1,
                              const QChar *data2, int length2,
                              Qt::CaseSensitivity cs = Qt::CaseSensitive) Q_DECL_NOTHROW;
    static int compare_helper(const QChar *data1, int length1,
                              const char *data2, int length2,
                              Qt::CaseSensitivity cs = Qt::CaseSensitive);
    static int compare_helper(const QChar *data1, int length1,
                              QLatin1String s2,
                              Qt::CaseSensitivity cs = Qt::CaseSensitive) Q_DECL_NOTHROW;
    static int localeAwareCompare_helper(const QChar *data1, int length1,
                                         const QChar *data2, int length2);
    static QString toLower_helper(const QString &str);
    static QString toLower_helper(QString &str);
    static QString toUpper_helper(const QString &str);
    static QString toUpper_helper(QString &str);
    static QString toCaseFolded_helper(const QString &str);
    static QString toCaseFolded_helper(QString &str);
    static QString trimmed_helper(const QString &str);
    static QString trimmed_helper(QString &str);
    static QString simplified_helper(const QString &str);
    static QString simplified_helper(QString &str);
    static Data *fromLatin1_helper(const char *str, int size = -1);
    static Data *fromAscii_helper(const char *str, int size = -1);
    static QString fromUtf8_helper(const char *str, int size);
    static QString fromLocal8Bit_helper(const char *, int size);
    static QByteArray toLatin1_helper(const QString &);
    static QByteArray toLatin1_helper(const QChar *data, int size);
    static QByteArray toLatin1_helper_inplace(QString &);
    static QByteArray toUtf8_helper(const QString &);
    static QByteArray toLocal8Bit_helper(const QChar *data, int size);
    static int toUcs4_helper(const ushort *uc, int length, uint *out);
    static qlonglong toIntegral_helper(const QChar *data, int len, bool *ok, int base);
    static qulonglong toIntegral_helper(const QChar *data, uint len, bool *ok, int base);
    void replace_helper(uint *indices, int nIndices, int blen, const QChar *after, int alen);
    friend class QCharRef;
    friend class QTextCodec;
    friend class QStringRef;
    friend class QByteArray;
    friend class QCollator;
    friend struct QAbstractConcatenable;

    template <typename T> static
    T toIntegral_helper(const QChar *data, int len, bool *ok, int base)
    {
        // ### Qt6: use std::conditional<std::is_unsigned<T>::value, qulonglong, qlonglong>::type
        const bool isUnsigned = T(0) < T(-1);
        typedef typename QtPrivate::QConditional<isUnsigned, qulonglong, qlonglong>::Type Int64;
        typedef typename QtPrivate::QConditional<isUnsigned, uint, int>::Type Int32;

        // we select the right overload by casting size() to int or uint
        Int64 val = toIntegral_helper(data, Int32(len), ok, base);
        if (T(val) != val) {
            if (ok)
                *ok = false;
            val = 0;
        }
        return T(val);
    }

public:
    typedef Data * DataPtr;
    inline DataPtr &data_ptr() { return d; }
};

inline QString::QString(QLatin1String aLatin1) : d(fromLatin1_helper(aLatin1.latin1(), aLatin1.size()))
{ }
inline int QString::length() const
{ return d->size; }
inline const QChar QString::at(int i) const
{ Q_ASSERT(uint(i) < uint(size())); return d->data()[i]; }
inline const QChar QString::operator[](int i) const
{ Q_ASSERT(uint(i) < uint(size())); return d->data()[i]; }
inline const QChar QString::operator[](uint i) const
{ Q_ASSERT(i < uint(size())); return d->data()[i]; }
inline bool QString::isEmpty() const
{ return d->size == 0; }
inline const QChar *QString::unicode() const
{ return reinterpret_cast<const QChar*>(d->data()); }
inline const QChar *QString::data() const
{ return reinterpret_cast<const QChar*>(d->data()); }
inline QChar *QString::data()
{ detach(); return reinterpret_cast<QChar*>(d->data()); }
inline const QChar *QString::constData() const
{ return reinterpret_cast<const QChar*>(d->data()); }
inline void QString::detach()
{ if (d->ref.isShared() || (d->offset != sizeof(QStringData))) reallocData(uint(d->size) + 1u); }
inline bool QString::isDetached() const
{ return !d->ref.isShared(); }
inline void QString::clear()
{ if (!isNull()) *this = QString(); }
inline QString::QString(const QString &other) Q_DECL_NOTHROW : d(other.d)
{ Q_ASSERT(&other != this); d->ref.ref(); }
inline int QString::capacity() const
{ return d->alloc ? d->alloc - 1 : 0; }
inline QString &QString::setNum(short n, int base)
{ return setNum(qlonglong(n), base); }
inline QString &QString::setNum(ushort n, int base)
{ return setNum(qulonglong(n), base); }
inline QString &QString::setNum(int n, int base)
{ return setNum(qlonglong(n), base); }
inline QString &QString::setNum(uint n, int base)
{ return setNum(qulonglong(n), base); }
inline QString &QString::setNum(long n, int base)
{ return setNum(qlonglong(n), base); }
inline QString &QString::setNum(ulong n, int base)
{ return setNum(qulonglong(n), base); }
inline QString &QString::setNum(float n, char f, int prec)
{ return setNum(double(n),f,prec); }
inline QString QString::arg(int a, int fieldWidth, int base, QChar fillChar) const
{ return arg(qlonglong(a), fieldWidth, base, fillChar); }
inline QString QString::arg(uint a, int fieldWidth, int base, QChar fillChar) const
{ return arg(qulonglong(a), fieldWidth, base, fillChar); }
inline QString QString::arg(long a, int fieldWidth, int base, QChar fillChar) const
{ return arg(qlonglong(a), fieldWidth, base, fillChar); }
inline QString QString::arg(ulong a, int fieldWidth, int base, QChar fillChar) const
{ return arg(qulonglong(a), fieldWidth, base, fillChar); }
inline QString QString::arg(short a, int fieldWidth, int base, QChar fillChar) const
{ return arg(qlonglong(a), fieldWidth, base, fillChar); }
inline QString QString::arg(ushort a, int fieldWidth, int base, QChar fillChar) const
{ return arg(qulonglong(a), fieldWidth, base, fillChar); }
inline QString QString::arg(const QString &a1, const QString &a2) const
{ const QString *args[2] = { &a1, &a2 }; return multiArg(2, args); }
inline QString QString::arg(const QString &a1, const QString &a2, const QString &a3) const
{ const QString *args[3] = { &a1, &a2, &a3 }; return multiArg(3, args); }
inline QString QString::arg(const QString &a1, const QString &a2, const QString &a3,
                            const QString &a4) const
{ const QString *args[4] = { &a1, &a2, &a3, &a4 }; return multiArg(4, args); }
inline QString QString::arg(const QString &a1, const QString &a2, const QString &a3,
                            const QString &a4, const QString &a5) const
{ const QString *args[5] = { &a1, &a2, &a3, &a4, &a5 }; return multiArg(5, args); }
inline QString QString::arg(const QString &a1, const QString &a2, const QString &a3,
                            const QString &a4, const QString &a5, const QString &a6) const
{ const QString *args[6] = { &a1, &a2, &a3, &a4, &a5, &a6 }; return multiArg(6, args); }
inline QString QString::arg(const QString &a1, const QString &a2, const QString &a3,
                            const QString &a4, const QString &a5, const QString &a6,
                            const QString &a7) const
{ const QString *args[7] = { &a1, &a2, &a3, &a4, &a5, &a6,  &a7 }; return multiArg(7, args); }
inline QString QString::arg(const QString &a1, const QString &a2, const QString &a3,
                            const QString &a4, const QString &a5, const QString &a6,
                            const QString &a7, const QString &a8) const
{ const QString *args[8] = { &a1, &a2, &a3, &a4, &a5, &a6,  &a7, &a8 }; return multiArg(8, args); }
inline QString QString::arg(const QString &a1, const QString &a2, const QString &a3,
                            const QString &a4, const QString &a5, const QString &a6,
                            const QString &a7, const QString &a8, const QString &a9) const
{ const QString *args[9] = { &a1, &a2, &a3, &a4, &a5, &a6,  &a7, &a8, &a9 }; return multiArg(9, args); }

inline QString QString::section(QChar asep, int astart, int aend, SectionFlags aflags) const
{ return section(QString(asep), astart, aend, aflags); }

QT_WARNING_PUSH
QT_WARNING_DISABLE_MSVC(4127)   // "conditional expression is constant"
QT_WARNING_DISABLE_INTEL(111)   // "statement is unreachable"

inline int QString::toWCharArray(wchar_t *array) const
{
    if (sizeof(wchar_t) == sizeof(QChar)) {
        memcpy(array, d->data(), sizeof(QChar) * size());
        return size();
    } else {
        return toUcs4_helper(d->data(), size(), reinterpret_cast<uint *>(array));
    }
}

QT_WARNING_POP

inline QString QString::fromWCharArray(const wchar_t *string, int size)
{
    return sizeof(wchar_t) == sizeof(QChar) ? fromUtf16(reinterpret_cast<const ushort *>(string), size)
                                            : fromUcs4(reinterpret_cast<const uint *>(string), size);
}


class Q_CORE_EXPORT QCharRef {
    QString &s;
    int i;
    inline QCharRef(QString &str, int idx)
        : s(str),i(idx) {}
    friend class QString;
public:

    // most QChar operations repeated here

    // all this is not documented: We just say "like QChar" and let it be.
    inline operator QChar() const
    { return i < s.d->size ? s.d->data()[i] : 0; }
    inline QCharRef &operator=(QChar c)
    { if (i >= s.d->size) s.resize(i + 1, QLatin1Char(' ')); else s.detach();
      s.d->data()[i] = c.unicode(); return *this; }

    // An operator= for each QChar cast constructors
#ifndef QT_NO_CAST_FROM_ASCII
    inline QT_ASCII_CAST_WARN QCharRef &operator=(char c)
    { return operator=(QChar::fromLatin1(c)); }
    inline QT_ASCII_CAST_WARN QCharRef &operator=(uchar c)
    { return operator=(QChar::fromLatin1(c)); }
#endif
    inline QCharRef &operator=(const QCharRef &c) { return operator=(QChar(c)); }
    inline QCharRef &operator=(ushort rc) { return operator=(QChar(rc)); }
    inline QCharRef &operator=(short rc) { return operator=(QChar(rc)); }
    inline QCharRef &operator=(uint rc) { return operator=(QChar(rc)); }
    inline QCharRef &operator=(int rc) { return operator=(QChar(rc)); }

    // each function...
    inline bool isNull() const { return QChar(*this).isNull(); }
    inline bool isPrint() const { return QChar(*this).isPrint(); }
    inline bool isPunct() const { return QChar(*this).isPunct(); }
    inline bool isSpace() const { return QChar(*this).isSpace(); }
    inline bool isMark() const { return QChar(*this).isMark(); }
    inline bool isLetter() const { return QChar(*this).isLetter(); }
    inline bool isNumber() const { return QChar(*this).isNumber(); }
    inline bool isLetterOrNumber() { return QChar(*this).isLetterOrNumber(); }
    inline bool isDigit() const { return QChar(*this).isDigit(); }
    inline bool isLower() const { return QChar(*this).isLower(); }
    inline bool isUpper() const { return QChar(*this).isUpper(); }
    inline bool isTitleCase() const { return QChar(*this).isTitleCase(); }

    inline int digitValue() const { return QChar(*this).digitValue(); }
    QChar toLower() const { return QChar(*this).toLower(); }
    QChar toUpper() const { return QChar(*this).toUpper(); }
    QChar toTitleCase () const { return QChar(*this).toTitleCase(); }

    QChar::Category category() const { return QChar(*this).category(); }
    QChar::Direction direction() const { return QChar(*this).direction(); }
    QChar::JoiningType joiningType() const { return QChar(*this).joiningType(); }
#if QT_DEPRECATED_SINCE(5, 3)
    QT_DEPRECATED QChar::Joining joining() const
    {
        switch (QChar(*this).joiningType()) {
        case QChar::Joining_Causing: return QChar::Center;
        case QChar::Joining_Dual: return QChar::Dual;
        case QChar::Joining_Right: return QChar::Right;
        case QChar::Joining_None:
        case QChar::Joining_Left:
        case QChar::Joining_Transparent:
        default: return QChar::OtherJoining;
        }
    }
#endif
    bool hasMirrored() const { return QChar(*this).hasMirrored(); }
    QChar mirroredChar() const { return QChar(*this).mirroredChar(); }
    QString decomposition() const { return QChar(*this).decomposition(); }
    QChar::Decomposition decompositionTag() const { return QChar(*this).decompositionTag(); }
    uchar combiningClass() const { return QChar(*this).combiningClass(); }

    inline QChar::Script script() const { return QChar(*this).script(); }

    QChar::UnicodeVersion unicodeVersion() const { return QChar(*this).unicodeVersion(); }

    inline uchar cell() const { return QChar(*this).cell(); }
    inline uchar row() const { return QChar(*this).row(); }
    inline void setCell(uchar cell);
    inline void setRow(uchar row);

#if QT_DEPRECATED_SINCE(5, 0)
    QT_DEPRECATED  char toAscii() const { return QChar(*this).toLatin1(); }
#endif
    char toLatin1() const { return QChar(*this).toLatin1(); }
    ushort unicode() const { return QChar(*this).unicode(); }
    ushort& unicode() { return s.data()[i].unicode(); }

};
Q_DECLARE_TYPEINFO(QCharRef, Q_MOVABLE_TYPE);

inline void QCharRef::setRow(uchar arow) { QChar(*this).setRow(arow); }
inline void QCharRef::setCell(uchar acell) { QChar(*this).setCell(acell); }


inline QString::QString() Q_DECL_NOTHROW : d(Data::sharedNull()) {}
inline QString::~QString() { if (!d->ref.deref()) Data::deallocate(d); }

inline void QString::reserve(int asize)
{
    if (d->ref.isShared() || uint(asize) >= d->alloc)
        reallocData(qMax(asize, d->size) + 1u);

    if (!d->capacityReserved) {
        // cannot set unconditionally, since d could be the shared_null/shared_empty (which is const)
        d->capacityReserved = true;
    }
}

inline void QString::squeeze()
{
    if (d->ref.isShared() || uint(d->size) + 1u < d->alloc)
        reallocData(uint(d->size) + 1u);

    if (d->capacityReserved) {
        // cannot set unconditionally, since d could be shared_null or
        // otherwise static.
        d->capacityReserved = false;
    }
}

inline QString &QString::setUtf16(const ushort *autf16, int asize)
{ return setUnicode(reinterpret_cast<const QChar *>(autf16), asize); }
inline QCharRef QString::operator[](int i)
{ Q_ASSERT(i >= 0); return QCharRef(*this, i); }
inline QCharRef QString::operator[](uint i)
{ return QCharRef(*this, i); }
inline QString::iterator QString::begin()
{ detach(); return reinterpret_cast<QChar*>(d->data()); }
inline QString::const_iterator QString::begin() const
{ return reinterpret_cast<const QChar*>(d->data()); }
inline QString::const_iterator QString::cbegin() const
{ return reinterpret_cast<const QChar*>(d->data()); }
inline QString::const_iterator QString::constBegin() const
{ return reinterpret_cast<const QChar*>(d->data()); }
inline QString::iterator QString::end()
{ detach(); return reinterpret_cast<QChar*>(d->data() + d->size); }
inline QString::const_iterator QString::end() const
{ return reinterpret_cast<const QChar*>(d->data() + d->size); }
inline QString::const_iterator QString::cend() const
{ return reinterpret_cast<const QChar*>(d->data() + d->size); }
inline QString::const_iterator QString::constEnd() const
{ return reinterpret_cast<const QChar*>(d->data() + d->size); }
inline bool QString::contains(const QString &s, Qt::CaseSensitivity cs) const
{ return indexOf(s, 0, cs) != -1; }
inline bool QString::contains(const QStringRef &s, Qt::CaseSensitivity cs) const
{ return indexOf(s, 0, cs) != -1; }
inline bool QString::contains(QLatin1String s, Qt::CaseSensitivity cs) const
{ return indexOf(s, 0, cs) != -1; }
inline bool QString::contains(QChar c, Qt::CaseSensitivity cs) const
{ return indexOf(c, 0, cs) != -1; }

#if QT_DEPRECATED_SINCE(5, 9)
inline bool operator==(QString::Null, QString::Null) { return true; }
QT_DEPRECATED_X("use QString::isNull()")
inline bool operator==(QString::Null, const QString &s) { return s.isNull(); }
QT_DEPRECATED_X("use QString::isNull()")
inline bool operator==(const QString &s, QString::Null) { return s.isNull(); }
inline bool operator!=(QString::Null, QString::Null) { return false; }
QT_DEPRECATED_X("use !QString::isNull()")
inline bool operator!=(QString::Null, const QString &s) { return !s.isNull(); }
QT_DEPRECATED_X("use !QString::isNull()")
inline bool operator!=(const QString &s, QString::Null) { return !s.isNull(); }
#endif

inline bool operator==(QLatin1String s1, QLatin1String s2) Q_DECL_NOTHROW
{ return s1.size() == s2.size() && (!s1.size() || !memcmp(s1.latin1(), s2.latin1(), s1.size())); }
inline bool operator!=(QLatin1String s1, QLatin1String s2) Q_DECL_NOTHROW
{ return !operator==(s1, s2); }
inline bool operator<(QLatin1String s1, QLatin1String s2) Q_DECL_NOTHROW
{
    const int len = qMin(s1.size(), s2.size());
    const int r = len ? memcmp(s1.latin1(), s2.latin1(), len) : 0;
    return r < 0 || (r == 0 && s1.size() < s2.size());
}
inline bool operator>(QLatin1String s1, QLatin1String s2) Q_DECL_NOTHROW
{ return operator<(s2, s1); }
inline bool operator<=(QLatin1String s1, QLatin1String s2) Q_DECL_NOTHROW
{ return !operator>(s1, s2); }
inline bool operator>=(QLatin1String s1, QLatin1String s2) Q_DECL_NOTHROW
{ return !operator<(s1, s2); }

inline bool QLatin1String::operator==(const QString &s) const Q_DECL_NOTHROW
{ return s == *this; }
inline bool QLatin1String::operator!=(const QString &s) const Q_DECL_NOTHROW
{ return s != *this; }
inline bool QLatin1String::operator>(const QString &s) const Q_DECL_NOTHROW
{ return s < *this; }
inline bool QLatin1String::operator<(const QString &s) const Q_DECL_NOTHROW
{ return s > *this; }
inline bool QLatin1String::operator>=(const QString &s) const Q_DECL_NOTHROW
{ return s <= *this; }
inline bool QLatin1String::operator<=(const QString &s) const Q_DECL_NOTHROW
{ return s >= *this; }

#if !defined(QT_NO_CAST_FROM_ASCII) && !defined(QT_RESTRICTED_CAST_FROM_ASCII)
inline bool QString::operator==(const char *s) const
{ return QString::compare_helper(constData(), size(), s, -1) == 0; }
inline bool QString::operator!=(const char *s) const
{ return QString::compare_helper(constData(), size(), s, -1) != 0; }
inline bool QString::operator<(const char *s) const
{ return QString::compare_helper(constData(), size(), s, -1) < 0; }
inline bool QString::operator>(const char *s) const
{ return QString::compare_helper(constData(), size(), s, -1) > 0; }
inline bool QString::operator<=(const char *s) const
{ return QString::compare_helper(constData(), size(), s, -1) <= 0; }
inline bool QString::operator>=(const char *s) const
{ return QString::compare_helper(constData(), size(), s, -1) >= 0; }

inline QT_ASCII_CAST_WARN bool operator==(const char *s1, const QString &s2)
{ return QString::compare_helper(s2.constData(), s2.size(), s1, -1) == 0; }
inline QT_ASCII_CAST_WARN bool operator!=(const char *s1, const QString &s2)
{ return QString::compare_helper(s2.constData(), s2.size(), s1, -1) != 0; }
inline QT_ASCII_CAST_WARN bool operator<(const char *s1, const QString &s2)
{ return QString::compare_helper(s2.constData(), s2.size(), s1, -1) > 0; }
inline QT_ASCII_CAST_WARN bool operator>(const char *s1, const QString &s2)
{ return QString::compare_helper(s2.constData(), s2.size(), s1, -1) < 0; }
inline QT_ASCII_CAST_WARN bool operator<=(const char *s1, const QString &s2)
{ return QString::compare_helper(s2.constData(), s2.size(), s1, -1) >= 0; }
inline QT_ASCII_CAST_WARN bool operator>=(const char *s1, const QString &s2)
{ return QString::compare_helper(s2.constData(), s2.size(), s1, -1) <= 0; }

inline QT_ASCII_CAST_WARN bool operator==(const char *s1, QLatin1String s2)
{ return QString::fromUtf8(s1) == s2; }
inline QT_ASCII_CAST_WARN bool operator!=(const char *s1, QLatin1String s2)
{ return QString::fromUtf8(s1) != s2; }
inline QT_ASCII_CAST_WARN bool operator<(const char *s1, QLatin1String s2)
{ return (QString::fromUtf8(s1) < s2); }
inline QT_ASCII_CAST_WARN bool operator>(const char *s1, QLatin1String s2)
{ return (QString::fromUtf8(s1) > s2); }
inline QT_ASCII_CAST_WARN bool operator<=(const char *s1, QLatin1String s2)
{ return (QString::fromUtf8(s1) <= s2); }
inline QT_ASCII_CAST_WARN bool operator>=(const char *s1, QLatin1String s2)
{ return (QString::fromUtf8(s1) >= s2); }

inline QT_ASCII_CAST_WARN bool QLatin1String::operator==(const char *s) const
{ return QString::fromUtf8(s) == *this; }
inline QT_ASCII_CAST_WARN bool QLatin1String::operator!=(const char *s) const
{ return QString::fromUtf8(s) != *this; }
inline QT_ASCII_CAST_WARN bool QLatin1String::operator<(const char *s) const
{ return QString::fromUtf8(s) > *this; }
inline QT_ASCII_CAST_WARN bool QLatin1String::operator>(const char *s) const
{ return QString::fromUtf8(s) < *this; }
inline QT_ASCII_CAST_WARN bool QLatin1String::operator<=(const char *s) const
{ return QString::fromUtf8(s) >= *this; }
inline QT_ASCII_CAST_WARN bool QLatin1String::operator>=(const char *s) const
{ return QString::fromUtf8(s) <= *this; }

inline QT_ASCII_CAST_WARN bool QLatin1String::operator==(const QByteArray &s) const
{ return QString::fromUtf8(s) == *this; }
inline QT_ASCII_CAST_WARN bool QLatin1String::operator!=(const QByteArray &s) const
{ return QString::fromUtf8(s) != *this; }
inline QT_ASCII_CAST_WARN bool QLatin1String::operator<(const QByteArray &s) const
{ return QString::fromUtf8(s) > *this; }
inline QT_ASCII_CAST_WARN bool QLatin1String::operator>(const QByteArray &s) const
{ return QString::fromUtf8(s) < *this; }
inline QT_ASCII_CAST_WARN bool QLatin1String::operator<=(const QByteArray &s) const
{ return QString::fromUtf8(s) >= *this; }
inline QT_ASCII_CAST_WARN bool QLatin1String::operator>=(const QByteArray &s) const
{ return QString::fromUtf8(s) <= *this; }

inline QT_ASCII_CAST_WARN bool QString::operator==(const QByteArray &s) const
{ return QString::compare_helper(constData(), size(), s.constData(), qstrnlen(s.constData(), s.size())) == 0; }
inline QT_ASCII_CAST_WARN bool QString::operator!=(const QByteArray &s) const
{ return QString::compare_helper(constData(), size(), s.constData(), qstrnlen(s.constData(), s.size())) != 0; }
inline QT_ASCII_CAST_WARN bool QString::operator<(const QByteArray &s) const
{ return QString::compare_helper(constData(), size(), s.constData(), s.size()) < 0; }
inline QT_ASCII_CAST_WARN bool QString::operator>(const QByteArray &s) const
{ return QString::compare_helper(constData(), size(), s.constData(), s.size()) > 0; }
inline QT_ASCII_CAST_WARN bool QString::operator<=(const QByteArray &s) const
{ return QString::compare_helper(constData(), size(), s.constData(), s.size()) <= 0; }
inline QT_ASCII_CAST_WARN bool QString::operator>=(const QByteArray &s) const
{ return QString::compare_helper(constData(), size(), s.constData(), s.size()) >= 0; }

inline bool QByteArray::operator==(const QString &s) const
{ return QString::compare_helper(s.constData(), s.size(), constData(), qstrnlen(constData(), size())) == 0; }
inline bool QByteArray::operator!=(const QString &s) const
{ return QString::compare_helper(s.constData(), s.size(), constData(), qstrnlen(constData(), size())) != 0; }
inline bool QByteArray::operator<(const QString &s) const
{ return QString::compare_helper(s.constData(), s.size(), constData(), size()) > 0; }
inline bool QByteArray::operator>(const QString &s) const
{ return QString::compare_helper(s.constData(), s.size(), constData(), size()) < 0; }
inline bool QByteArray::operator<=(const QString &s) const
{ return QString::compare_helper(s.constData(), s.size(), constData(), size()) >= 0; }
inline bool QByteArray::operator>=(const QString &s) const
{ return QString::compare_helper(s.constData(), s.size(), constData(), size()) <= 0; }
#endif // !defined(QT_NO_CAST_FROM_ASCII) && !defined(QT_RESTRICTED_CAST_FROM_ASCII)

#ifndef QT_NO_CAST_TO_ASCII
inline QByteArray &QByteArray::append(const QString &s)
{ return append(s.toUtf8()); }
inline QByteArray &QByteArray::insert(int i, const QString &s)
{ return insert(i, s.toUtf8()); }
inline QByteArray &QByteArray::replace(char c, const QString &after)
{ return replace(c, after.toUtf8()); }
inline QByteArray &QByteArray::replace(const QString &before, const char *after)
{ return replace(before.toUtf8(), after); }
inline QByteArray &QByteArray::replace(const QString &before, const QByteArray &after)
{ return replace(before.toUtf8(), after); }
inline QByteArray &QByteArray::operator+=(const QString &s)
{ return operator+=(s.toUtf8()); }
inline int QByteArray::indexOf(const QString &s, int from) const
{ return indexOf(s.toUtf8(), from); }
inline int QByteArray::lastIndexOf(const QString &s, int from) const
{ return lastIndexOf(s.toUtf8(), from); }
#endif // QT_NO_CAST_TO_ASCII

#if !defined(QT_USE_FAST_OPERATOR_PLUS) && !defined(QT_USE_QSTRINGBUILDER)
inline const QString operator+(const QString &s1, const QString &s2)
{ QString t(s1); t += s2; return t; }
inline const QString operator+(const QString &s1, QChar s2)
{ QString t(s1); t += s2; return t; }
inline const QString operator+(QChar s1, const QString &s2)
{ QString t(s1); t += s2; return t; }
#  if !defined(QT_NO_CAST_FROM_ASCII) && !defined(QT_RESTRICTED_CAST_FROM_ASCII)
inline QT_ASCII_CAST_WARN const QString operator+(const QString &s1, const char *s2)
{ QString t(s1); t += QString::fromUtf8(s2); return t; }
inline QT_ASCII_CAST_WARN const QString operator+(const char *s1, const QString &s2)
{ QString t = QString::fromUtf8(s1); t += s2; return t; }
inline QT_ASCII_CAST_WARN const QString operator+(char c, const QString &s)
{ QString t = s; t.prepend(QChar::fromLatin1(c)); return t; }
inline QT_ASCII_CAST_WARN const QString operator+(const QString &s, char c)
{ QString t = s; t += QChar::fromLatin1(c); return t; }
inline QT_ASCII_CAST_WARN const QString operator+(const QByteArray &ba, const QString &s)
{ QString t = QString::fromUtf8(ba); t += s; return t; }
inline QT_ASCII_CAST_WARN const QString operator+(const QString &s, const QByteArray &ba)
{ QString t(s); t += QString::fromUtf8(ba); return t; }
#  endif // QT_NO_CAST_FROM_ASCII
#endif // QT_USE_QSTRINGBUILDER

inline std::string QString::toStdString() const
{ return toUtf8().toStdString(); }

inline QString QString::fromStdString(const std::string &s)
{ return fromUtf8(s.data(), int(s.size())); }

inline std::wstring QString::toStdWString() const
{
    std::wstring str;
    str.resize(length());

#if defined(_MSC_VER) && _MSC_VER >= 1400
    // VS2005 crashes if the string is empty
    if (!length())
        return str;
#endif

    str.resize(toWCharArray(&(*str.begin())));
    return str;
}

inline QString QString::fromStdWString(const std::wstring &s)
{ return fromWCharArray(s.data(), int(s.size())); }

#if defined(Q_STDLIB_UNICODE_STRINGS)
inline QString QString::fromStdU16String(const std::u16string &s)
{ return fromUtf16(s.data(), int(s.size())); }

inline std::u16string QString::toStdU16String() const
{ return std::u16string(reinterpret_cast<const char16_t*>(utf16()), length()); }

inline QString QString::fromStdU32String(const std::u32string &s)
{ return fromUcs4(s.data(), int(s.size())); }

inline std::u32string QString::toStdU32String() const
{
    std::u32string u32str(length(), char32_t(0));
    int len = toUcs4_helper(d->data(), length(), reinterpret_cast<uint*>(&u32str[0]));
    u32str.resize(len);
    return u32str;
}
#endif

#if !defined(QT_NO_DATASTREAM) || (defined(QT_BOOTSTRAPPED) && !defined(QT_BUILD_QMAKE))
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QString &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QString &);
#endif

Q_DECLARE_SHARED(QString)
Q_DECLARE_OPERATORS_FOR_FLAGS(QString::SectionFlags)


class Q_CORE_EXPORT QStringRef {
    const QString *m_string;
    int m_position;
    int m_size;
public:
    typedef QString::size_type size_type;
    typedef QString::value_type value_type;
    typedef const QChar *const_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef QString::const_pointer const_pointer;
    typedef QString::const_reference const_reference;

    // ### Qt 6: make this constructor constexpr, after the destructor is made trivial
    inline QStringRef() : m_string(Q_NULLPTR), m_position(0), m_size(0) {}
    inline QStringRef(const QString *string, int position, int size);
    inline QStringRef(const QString *string);

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    // ### Qt 6: remove all of these, the implicit ones are fine
    QStringRef(const QStringRef &other) Q_DECL_NOTHROW
        :m_string(other.m_string), m_position(other.m_position), m_size(other.m_size)
        {}
#ifdef Q_COMPILER_RVALUE_REFS
    QStringRef(QStringRef &&other) Q_DECL_NOTHROW : m_string(other.m_string), m_position(other.m_position), m_size(other.m_size) {}
    QStringRef &operator=(QStringRef &&other) Q_DECL_NOTHROW { return *this = other; }
#endif
    QStringRef &operator=(const QStringRef &other) Q_DECL_NOTHROW
    {
        m_string = other.m_string; m_position = other.m_position;
        m_size = other.m_size; return *this;
    }
    inline ~QStringRef(){}
#endif // Qt < 6.0.0

    inline const QString *string() const { return m_string; }
    inline int position() const { return m_position; }
    inline int size() const { return m_size; }
    inline int count() const { return m_size; }
    inline int length() const { return m_size; }

    int indexOf(const QString &str, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int indexOf(QChar ch, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int indexOf(QLatin1String str, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int indexOf(const QStringRef &str, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int lastIndexOf(const QString &str, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int lastIndexOf(QChar ch, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int lastIndexOf(QLatin1String str, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int lastIndexOf(const QStringRef &str, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    inline bool contains(const QString &str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    inline bool contains(QChar ch, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    inline bool contains(QLatin1String str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    inline bool contains(const QStringRef &str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    int count(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int count(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int count(const QStringRef &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    Q_REQUIRED_RESULT QVector<QStringRef> split(const QString &sep, QString::SplitBehavior behavior = QString::KeepEmptyParts,
                      Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    Q_REQUIRED_RESULT QVector<QStringRef> split(QChar sep, QString::SplitBehavior behavior = QString::KeepEmptyParts,
                      Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    Q_REQUIRED_RESULT QStringRef left(int n) const;
    Q_REQUIRED_RESULT QStringRef right(int n) const;
    Q_REQUIRED_RESULT QStringRef mid(int pos, int n = -1) const;

    void truncate(int pos) Q_DECL_NOTHROW { m_size = qBound(0, pos, m_size); }
    void chop(int n) Q_DECL_NOTHROW
    {
        if (n >= m_size)
            m_size = 0;
        else if (n > 0)
            m_size -= n;
    }

    bool isRightToLeft() const;

    bool startsWith(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool startsWith(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool startsWith(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool startsWith(const QStringRef &c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    bool endsWith(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool endsWith(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool endsWith(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool endsWith(const QStringRef &c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    inline QStringRef &operator=(const QString *string);

    inline const QChar *unicode() const
    {
        if (!m_string)
            return reinterpret_cast<const QChar *>(QString::Data::sharedNull()->data());
        return m_string->unicode() + m_position;
    }
    inline const QChar *data() const { return unicode(); }
    inline const QChar *constData() const {  return unicode(); }

    inline const_iterator begin() const { return unicode(); }
    inline const_iterator cbegin() const { return unicode(); }
    inline const_iterator constBegin() const { return unicode(); }
    inline const_iterator end() const { return unicode() + size(); }
    inline const_iterator cend() const { return unicode() + size(); }
    inline const_iterator constEnd() const { return unicode() + size(); }
    inline const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    inline const_reverse_iterator crbegin() const { return rbegin(); }
    inline const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    inline const_reverse_iterator crend() const { return rend(); }

#if QT_DEPRECATED_SINCE(5, 0)
    Q_REQUIRED_RESULT QT_DEPRECATED QByteArray toAscii() const
    { return toLatin1(); }
#endif
    Q_REQUIRED_RESULT QByteArray toLatin1() const;
    Q_REQUIRED_RESULT QByteArray toUtf8() const;
    Q_REQUIRED_RESULT QByteArray toLocal8Bit() const;
    Q_REQUIRED_RESULT QVector<uint> toUcs4() const;

    inline void clear() { m_string = Q_NULLPTR; m_position = m_size = 0; }
    QString toString() const;
    inline bool isEmpty() const { return m_size == 0; }
    inline bool isNull() const { return m_string == Q_NULLPTR || m_string->isNull(); }

    QStringRef appendTo(QString *string) const;

    inline const QChar at(int i) const
        { Q_ASSERT(uint(i) < uint(size())); return m_string->at(i + m_position); }
    QChar operator[](int i) const { return at(i); }

#if !defined(QT_NO_CAST_FROM_ASCII) && !defined(QT_RESTRICTED_CAST_FROM_ASCII)
    // ASCII compatibility
    inline QT_ASCII_CAST_WARN bool operator==(const char *s) const;
    inline QT_ASCII_CAST_WARN bool operator!=(const char *s) const;
    inline QT_ASCII_CAST_WARN bool operator<(const char *s) const;
    inline QT_ASCII_CAST_WARN bool operator<=(const char *s) const;
    inline QT_ASCII_CAST_WARN bool operator>(const char *s) const;
    inline QT_ASCII_CAST_WARN bool operator>=(const char *s) const;
#endif

    int compare(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const Q_DECL_NOTHROW;
    int compare(const QStringRef &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const Q_DECL_NOTHROW;
    int compare(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const Q_DECL_NOTHROW;
#if !defined(QT_NO_CAST_FROM_ASCII) && !defined(QT_RESTRICTED_CAST_FROM_ASCII)
    int compare(const QByteArray &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const
    { return QString::compare_helper(unicode(), size(), s.data(), qstrnlen(s.data(), s.size()), cs); }
#endif
    static int compare(const QStringRef &s1, const QString &s2,
                       Qt::CaseSensitivity = Qt::CaseSensitive) Q_DECL_NOTHROW;
    static int compare(const QStringRef &s1, const QStringRef &s2,
                       Qt::CaseSensitivity = Qt::CaseSensitive) Q_DECL_NOTHROW;
    static int compare(const QStringRef &s1, QLatin1String s2,
                       Qt::CaseSensitivity cs = Qt::CaseSensitive) Q_DECL_NOTHROW;

    int localeAwareCompare(const QString &s) const;
    int localeAwareCompare(const QStringRef &s) const;
    static int localeAwareCompare(const QStringRef &s1, const QString &s2);
    static int localeAwareCompare(const QStringRef &s1, const QStringRef &s2);

    Q_REQUIRED_RESULT QStringRef trimmed() const;
    short  toShort(bool *ok = Q_NULLPTR, int base = 10) const;
    ushort toUShort(bool *ok = Q_NULLPTR, int base = 10) const;
    int toInt(bool *ok = Q_NULLPTR, int base = 10) const;
    uint toUInt(bool *ok = Q_NULLPTR, int base = 10) const;
    long toLong(bool *ok = Q_NULLPTR, int base = 10) const;
    ulong toULong(bool *ok = Q_NULLPTR, int base = 10) const;
    qlonglong toLongLong(bool *ok = Q_NULLPTR, int base = 10) const;
    qulonglong toULongLong(bool *ok = Q_NULLPTR, int base = 10) const;
    float toFloat(bool *ok = Q_NULLPTR) const;
    double toDouble(bool *ok = Q_NULLPTR) const;
};
Q_DECLARE_TYPEINFO(QStringRef, Q_PRIMITIVE_TYPE);

inline QStringRef &QStringRef::operator=(const QString *aString)
{ m_string = aString; m_position = 0; m_size = aString?aString->size():0; return *this; }

inline QStringRef::QStringRef(const QString *aString, int aPosition, int aSize)
        :m_string(aString), m_position(aPosition), m_size(aSize){}

inline QStringRef::QStringRef(const QString *aString)
    :m_string(aString), m_position(0), m_size(aString?aString->size() : 0){}

// QStringRef <> QStringRef
Q_CORE_EXPORT bool operator==(const QStringRef &s1, const QStringRef &s2) Q_DECL_NOTHROW;
inline bool operator!=(const QStringRef &s1, const QStringRef &s2) Q_DECL_NOTHROW
{ return !(s1 == s2); }
Q_CORE_EXPORT bool operator<(const QStringRef &s1, const QStringRef &s2) Q_DECL_NOTHROW;
inline bool operator>(const QStringRef &s1, const QStringRef &s2) Q_DECL_NOTHROW
{ return s2 < s1; }
inline bool operator<=(const QStringRef &s1, const QStringRef &s2) Q_DECL_NOTHROW
{ return !(s1 > s2); }
inline bool operator>=(const QStringRef &s1, const QStringRef &s2) Q_DECL_NOTHROW
{ return !(s1 < s2); }

// QString <> QStringRef
Q_CORE_EXPORT bool operator==(const QString &lhs, const QStringRef &rhs) Q_DECL_NOTHROW;
inline bool operator!=(const QString &lhs, const QStringRef &rhs) Q_DECL_NOTHROW { return lhs.compare(rhs) != 0; }
inline bool operator< (const QString &lhs, const QStringRef &rhs) Q_DECL_NOTHROW { return lhs.compare(rhs) <  0; }
inline bool operator> (const QString &lhs, const QStringRef &rhs) Q_DECL_NOTHROW { return lhs.compare(rhs) >  0; }
inline bool operator<=(const QString &lhs, const QStringRef &rhs) Q_DECL_NOTHROW { return lhs.compare(rhs) <= 0; }
inline bool operator>=(const QString &lhs, const QStringRef &rhs) Q_DECL_NOTHROW { return lhs.compare(rhs) >= 0; }

inline bool operator==(const QStringRef &lhs, const QString &rhs) Q_DECL_NOTHROW { return rhs == lhs; }
inline bool operator!=(const QStringRef &lhs, const QString &rhs) Q_DECL_NOTHROW { return rhs != lhs; }
inline bool operator< (const QStringRef &lhs, const QString &rhs) Q_DECL_NOTHROW { return rhs >  lhs; }
inline bool operator> (const QStringRef &lhs, const QString &rhs) Q_DECL_NOTHROW { return rhs <  lhs; }
inline bool operator<=(const QStringRef &lhs, const QString &rhs) Q_DECL_NOTHROW { return rhs >= lhs; }
inline bool operator>=(const QStringRef &lhs, const QString &rhs) Q_DECL_NOTHROW { return rhs <= lhs; }

inline int QString::compare(const QStringRef &s, Qt::CaseSensitivity cs) const Q_DECL_NOTHROW
{ return QString::compare_helper(constData(), length(), s.constData(), s.length(), cs); }
inline int QString::compare(const QString &s1, const QStringRef &s2, Qt::CaseSensitivity cs) Q_DECL_NOTHROW
{ return QString::compare_helper(s1.constData(), s1.length(), s2.constData(), s2.length(), cs); }
inline int QStringRef::compare(const QString &s, Qt::CaseSensitivity cs) const Q_DECL_NOTHROW
{ return QString::compare_helper(constData(), length(), s.constData(), s.length(), cs); }
inline int QStringRef::compare(const QStringRef &s, Qt::CaseSensitivity cs) const Q_DECL_NOTHROW
{ return QString::compare_helper(constData(), length(), s.constData(), s.length(), cs); }
inline int QStringRef::compare(QLatin1String s, Qt::CaseSensitivity cs) const Q_DECL_NOTHROW
{ return QString::compare_helper(constData(), length(), s, cs); }
inline int QStringRef::compare(const QStringRef &s1, const QString &s2, Qt::CaseSensitivity cs) Q_DECL_NOTHROW
{ return QString::compare_helper(s1.constData(), s1.length(), s2.constData(), s2.length(), cs); }
inline int QStringRef::compare(const QStringRef &s1, const QStringRef &s2, Qt::CaseSensitivity cs) Q_DECL_NOTHROW
{ return QString::compare_helper(s1.constData(), s1.length(), s2.constData(), s2.length(), cs); }
inline int QStringRef::compare(const QStringRef &s1, QLatin1String s2, Qt::CaseSensitivity cs) Q_DECL_NOTHROW
{ return QString::compare_helper(s1.constData(), s1.length(), s2, cs); }

// QLatin1String <> QStringRef
Q_CORE_EXPORT bool operator==(QLatin1String lhs, const QStringRef &rhs) Q_DECL_NOTHROW;
inline bool operator!=(QLatin1String lhs, const QStringRef &rhs) Q_DECL_NOTHROW { return rhs.compare(lhs) != 0; }
inline bool operator< (QLatin1String lhs, const QStringRef &rhs) Q_DECL_NOTHROW { return rhs.compare(lhs) >  0; }
inline bool operator> (QLatin1String lhs, const QStringRef &rhs) Q_DECL_NOTHROW { return rhs.compare(lhs) <  0; }
inline bool operator<=(QLatin1String lhs, const QStringRef &rhs) Q_DECL_NOTHROW { return rhs.compare(lhs) >= 0; }
inline bool operator>=(QLatin1String lhs, const QStringRef &rhs) Q_DECL_NOTHROW { return rhs.compare(lhs) <= 0; }

inline bool operator==(const QStringRef &lhs, QLatin1String rhs) Q_DECL_NOTHROW { return rhs == lhs; }
inline bool operator!=(const QStringRef &lhs, QLatin1String rhs) Q_DECL_NOTHROW { return rhs != lhs; }
inline bool operator< (const QStringRef &lhs, QLatin1String rhs) Q_DECL_NOTHROW { return rhs >  lhs; }
inline bool operator> (const QStringRef &lhs, QLatin1String rhs) Q_DECL_NOTHROW { return rhs <  lhs; }
inline bool operator<=(const QStringRef &lhs, QLatin1String rhs) Q_DECL_NOTHROW { return rhs >= lhs; }
inline bool operator>=(const QStringRef &lhs, QLatin1String rhs) Q_DECL_NOTHROW { return rhs <= lhs; }

// QChar <> QString
inline bool operator==(QChar lhs, const QString &rhs) Q_DECL_NOTHROW
{ return QString::compare_helper(&lhs, 1, rhs.data(), rhs.size()) == 0; }
inline bool operator< (QChar lhs, const QString &rhs) Q_DECL_NOTHROW
{ return QString::compare_helper(&lhs, 1, rhs.data(), rhs.size()) <  0; }
inline bool operator> (QChar lhs, const QString &rhs) Q_DECL_NOTHROW
{ return QString::compare_helper(&lhs, 1, rhs.data(), rhs.size()) >  0; }

inline bool operator!=(QChar lhs, const QString &rhs) Q_DECL_NOTHROW { return !(lhs == rhs); }
inline bool operator<=(QChar lhs, const QString &rhs) Q_DECL_NOTHROW { return !(lhs >  rhs); }
inline bool operator>=(QChar lhs, const QString &rhs) Q_DECL_NOTHROW { return !(lhs <  rhs); }

inline bool operator==(const QString &lhs, QChar rhs) Q_DECL_NOTHROW { return   rhs == lhs; }
inline bool operator!=(const QString &lhs, QChar rhs) Q_DECL_NOTHROW { return !(rhs == lhs); }
inline bool operator< (const QString &lhs, QChar rhs) Q_DECL_NOTHROW { return   rhs >  lhs; }
inline bool operator> (const QString &lhs, QChar rhs) Q_DECL_NOTHROW { return   rhs <  lhs; }
inline bool operator<=(const QString &lhs, QChar rhs) Q_DECL_NOTHROW { return !(rhs <  lhs); }
inline bool operator>=(const QString &lhs, QChar rhs) Q_DECL_NOTHROW { return !(rhs >  lhs); }

// QChar <> QStringRef
inline bool operator==(QChar lhs, const QStringRef &rhs) Q_DECL_NOTHROW
{ return QString::compare_helper(&lhs, 1, rhs.data(), rhs.size()) == 0; }
inline bool operator< (QChar lhs, const QStringRef &rhs) Q_DECL_NOTHROW
{ return QString::compare_helper(&lhs, 1, rhs.data(), rhs.size()) <  0; }
inline bool operator> (QChar lhs, const QStringRef &rhs) Q_DECL_NOTHROW
{ return QString::compare_helper(&lhs, 1, rhs.data(), rhs.size()) >  0; }

inline bool operator!=(QChar lhs, const QStringRef &rhs) Q_DECL_NOTHROW { return !(lhs == rhs); }
inline bool operator<=(QChar lhs, const QStringRef &rhs) Q_DECL_NOTHROW { return !(lhs >  rhs); }
inline bool operator>=(QChar lhs, const QStringRef &rhs) Q_DECL_NOTHROW { return !(lhs <  rhs); }

inline bool operator==(const QStringRef &lhs, QChar rhs) Q_DECL_NOTHROW { return   rhs == lhs; }
inline bool operator!=(const QStringRef &lhs, QChar rhs) Q_DECL_NOTHROW { return !(rhs == lhs); }
inline bool operator< (const QStringRef &lhs, QChar rhs) Q_DECL_NOTHROW { return   rhs >  lhs; }
inline bool operator> (const QStringRef &lhs, QChar rhs) Q_DECL_NOTHROW { return   rhs <  lhs; }
inline bool operator<=(const QStringRef &lhs, QChar rhs) Q_DECL_NOTHROW { return !(rhs <  lhs); }
inline bool operator>=(const QStringRef &lhs, QChar rhs) Q_DECL_NOTHROW { return !(rhs >  lhs); }

// QChar <> QLatin1String
inline bool operator==(QChar lhs, QLatin1String rhs) Q_DECL_NOTHROW
{ return QString::compare_helper(&lhs, 1, rhs) == 0; }
inline bool operator< (QChar lhs, QLatin1String rhs) Q_DECL_NOTHROW
{ return QString::compare_helper(&lhs, 1, rhs) <  0; }
inline bool operator> (QChar lhs, QLatin1String rhs) Q_DECL_NOTHROW
{ return QString::compare_helper(&lhs, 1, rhs) >  0; }

inline bool operator!=(QChar lhs, QLatin1String rhs) Q_DECL_NOTHROW { return !(lhs == rhs); }
inline bool operator<=(QChar lhs, QLatin1String rhs) Q_DECL_NOTHROW { return !(lhs >  rhs); }
inline bool operator>=(QChar lhs, QLatin1String rhs) Q_DECL_NOTHROW { return !(lhs <  rhs); }

inline bool operator==(QLatin1String lhs, QChar rhs) Q_DECL_NOTHROW { return   rhs == lhs; }
inline bool operator!=(QLatin1String lhs, QChar rhs) Q_DECL_NOTHROW { return !(rhs == lhs); }
inline bool operator< (QLatin1String lhs, QChar rhs) Q_DECL_NOTHROW { return   rhs >  lhs; }
inline bool operator> (QLatin1String lhs, QChar rhs) Q_DECL_NOTHROW { return   rhs <  lhs; }
inline bool operator<=(QLatin1String lhs, QChar rhs) Q_DECL_NOTHROW { return !(rhs <  lhs); }
inline bool operator>=(QLatin1String lhs, QChar rhs) Q_DECL_NOTHROW { return !(rhs >  lhs); }

#if !defined(QT_NO_CAST_FROM_ASCII) && !defined(QT_RESTRICTED_CAST_FROM_ASCII)
// QStringRef <> QByteArray
inline QT_ASCII_CAST_WARN bool operator==(const QStringRef &lhs, const QByteArray &rhs) { return lhs.compare(rhs) == 0; }
inline QT_ASCII_CAST_WARN bool operator!=(const QStringRef &lhs, const QByteArray &rhs) { return lhs.compare(rhs) != 0; }
inline QT_ASCII_CAST_WARN bool operator< (const QStringRef &lhs, const QByteArray &rhs) { return lhs.compare(rhs) <  0; }
inline QT_ASCII_CAST_WARN bool operator> (const QStringRef &lhs, const QByteArray &rhs) { return lhs.compare(rhs) >  0; }
inline QT_ASCII_CAST_WARN bool operator<=(const QStringRef &lhs, const QByteArray &rhs) { return lhs.compare(rhs) <= 0; }
inline QT_ASCII_CAST_WARN bool operator>=(const QStringRef &lhs, const QByteArray &rhs) { return lhs.compare(rhs) >= 0; }

inline QT_ASCII_CAST_WARN bool operator==(const QByteArray &lhs, const QStringRef &rhs) { return rhs.compare(lhs) == 0; }
inline QT_ASCII_CAST_WARN bool operator!=(const QByteArray &lhs, const QStringRef &rhs) { return rhs.compare(lhs) != 0; }
inline QT_ASCII_CAST_WARN bool operator< (const QByteArray &lhs, const QStringRef &rhs) { return rhs.compare(lhs) >  0; }
inline QT_ASCII_CAST_WARN bool operator> (const QByteArray &lhs, const QStringRef &rhs) { return rhs.compare(lhs) <  0; }
inline QT_ASCII_CAST_WARN bool operator<=(const QByteArray &lhs, const QStringRef &rhs) { return rhs.compare(lhs) >= 0; }
inline QT_ASCII_CAST_WARN bool operator>=(const QByteArray &lhs, const QStringRef &rhs) { return rhs.compare(lhs) <= 0; }

// QStringRef <> const char *
inline QT_ASCII_CAST_WARN bool QStringRef::operator==(const char *s) const
{ return QString::compare_helper(constData(), size(), s, -1) == 0; }
inline QT_ASCII_CAST_WARN bool QStringRef::operator!=(const char *s) const
{ return QString::compare_helper(constData(), size(), s, -1) != 0; }
inline QT_ASCII_CAST_WARN bool QStringRef::operator<(const char *s) const
{ return QString::compare_helper(constData(), size(), s, -1) < 0; }
inline QT_ASCII_CAST_WARN bool QStringRef::operator<=(const char *s) const
{ return QString::compare_helper(constData(), size(), s, -1) <= 0; }
inline QT_ASCII_CAST_WARN bool QStringRef::operator>(const char *s) const
{ return QString::compare_helper(constData(), size(), s, -1) > 0; }
inline QT_ASCII_CAST_WARN bool QStringRef::operator>=(const char *s) const
{ return QString::compare_helper(constData(), size(), s, -1) >= 0; }

inline QT_ASCII_CAST_WARN bool operator==(const char *s1, const QStringRef &s2)
{ return QString::compare_helper(s2.constData(), s2.size(), s1, -1) == 0; }
inline QT_ASCII_CAST_WARN bool operator!=(const char *s1, const QStringRef &s2)
{ return QString::compare_helper(s2.constData(), s2.size(), s1, -1) != 0; }
inline QT_ASCII_CAST_WARN bool operator<(const char *s1, const QStringRef &s2)
{ return QString::compare_helper(s2.constData(), s2.size(), s1, -1) > 0; }
inline QT_ASCII_CAST_WARN bool operator<=(const char *s1, const QStringRef &s2)
{ return QString::compare_helper(s2.constData(), s2.size(), s1, -1) >= 0; }
inline QT_ASCII_CAST_WARN bool operator>(const char *s1, const QStringRef &s2)
{ return QString::compare_helper(s2.constData(), s2.size(), s1, -1) < 0; }
inline QT_ASCII_CAST_WARN bool operator>=(const char *s1, const QStringRef &s2)
{ return QString::compare_helper(s2.constData(), s2.size(), s1, -1) <= 0; }
#endif // !defined(QT_NO_CAST_FROM_ASCII) && !defined(QT_RESTRICTED_CAST_FROM_ASCII)

inline int QString::localeAwareCompare(const QStringRef &s) const
{ return localeAwareCompare_helper(constData(), length(), s.constData(), s.length()); }
inline int QString::localeAwareCompare(const QString& s1, const QStringRef& s2)
{ return localeAwareCompare_helper(s1.constData(), s1.length(), s2.constData(), s2.length()); }
inline int QStringRef::localeAwareCompare(const QString &s) const
{ return QString::localeAwareCompare_helper(constData(), length(), s.constData(), s.length()); }
inline int QStringRef::localeAwareCompare(const QStringRef &s) const
{ return QString::localeAwareCompare_helper(constData(), length(), s.constData(), s.length()); }
inline int QStringRef::localeAwareCompare(const QStringRef &s1, const QString &s2)
{ return QString::localeAwareCompare_helper(s1.constData(), s1.length(), s2.constData(), s2.length()); }
inline int QStringRef::localeAwareCompare(const QStringRef &s1, const QStringRef &s2)
{ return QString::localeAwareCompare_helper(s1.constData(), s1.length(), s2.constData(), s2.length()); }

inline bool QStringRef::contains(const QString &s, Qt::CaseSensitivity cs) const
{ return indexOf(s, 0, cs) != -1; }
inline bool QStringRef::contains(QLatin1String s, Qt::CaseSensitivity cs) const
{ return indexOf(s, 0, cs) != -1; }
inline bool QStringRef::contains(QChar c, Qt::CaseSensitivity cs) const
{ return indexOf(c, 0, cs) != -1; }
inline bool QStringRef::contains(const QStringRef &s, Qt::CaseSensitivity cs) const
{ return indexOf(s, 0, cs) != -1; }

inline QString &QString::insert(int i, const QStringRef &s)
{ return insert(i, s.constData(), s.length()); }

#if !defined(QT_USE_FAST_OPERATOR_PLUS) && !defined(QT_USE_QSTRINGBUILDER)
inline QString operator+(const QString &s1, const QStringRef &s2)
{ QString t; t.reserve(s1.size() + s2.size()); t += s1; t += s2; return t; }
inline QString operator+(const QStringRef &s1, const QString &s2)
{ QString t; t.reserve(s1.size() + s2.size()); t += s1; t += s2; return t; }
inline QString operator+(const QStringRef &s1, QLatin1String s2)
{ QString t; t.reserve(s1.size() + s2.size()); t += s1; t += s2; return t; }
inline QString operator+(QLatin1String s1, const QStringRef &s2)
{ QString t; t.reserve(s1.size() + s2.size()); t += s1; t += s2; return t; }
inline QString operator+(const QStringRef &s1, const QStringRef &s2)
{ QString t; t.reserve(s1.size() + s2.size()); t += s1; t += s2; return t; }
inline QString operator+(const QStringRef &s1, QChar s2)
{ QString t; t.reserve(s1.size() + 1); t += s1; t += s2; return t; }
inline QString operator+(QChar s1, const QStringRef &s2)
{ QString t; t.reserve(1 + s2.size()); t += s1; t += s2; return t; }
#endif // !(QT_USE_FAST_OPERATOR_PLUS || QT_USE_QSTRINGBUILDER)

namespace Qt {
#if QT_DEPRECATED_SINCE(5, 0)
QT_DEPRECATED inline QString escape(const QString &plain) {
    return plain.toHtmlEscaped();
}
#endif
}

QT_END_NAMESPACE

#if defined(QT_USE_FAST_OPERATOR_PLUS) || defined(QT_USE_QSTRINGBUILDER)
#include <QtCore/qstringbuilder.h>
#endif

#endif // QSTRING_H
