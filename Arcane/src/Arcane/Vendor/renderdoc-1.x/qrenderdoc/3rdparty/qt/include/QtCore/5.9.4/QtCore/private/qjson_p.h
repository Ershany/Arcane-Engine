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

#ifndef QJSON_P_H
#define QJSON_P_H

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

#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qjsondocument.h>
#include <qjsonarray.h>
#include <qatomic.h>
#include <qstring.h>
#include <qendian.h>
#include <qnumeric.h>

#include "private/qsimd_p.h"

#include <limits.h>
#include <limits>

QT_BEGIN_NAMESPACE

/*
  This defines a binary data structure for Json data. The data structure is optimised for fast reading
  and minimum allocations. The whole data structure can be mmap'ed and used directly.

  In most cases the binary structure is not as space efficient as a utf8 encoded text representation, but
  much faster to access.

  The size requirements are:

  String:
    Latin1 data: 2 bytes header + string.length()
    Full Unicode: 4 bytes header + 2*(string.length())

  Values: 4 bytes + size of data (size can be 0 for some data)
    bool: 0 bytes
    double: 8 bytes (0 if integer with less than 27bits)
    string: see above
    array: size of array
    object: size of object
  Array: 12 bytes + 4*length + size of Value data
  Object: 12 bytes + 8*length + size of Key Strings + size of Value data

  For an example such as

    {                                           // object: 12 + 5*8                   = 52
         "firstName": "John",                   // key 12, value 8                    = 20
         "lastName" : "Smith",                  // key 12, value 8                    = 20
         "age"      : 25,                       // key 8, value 0                     = 8
         "address"  :                           // key 12, object below               = 140
         {                                      // object: 12 + 4*8
             "streetAddress": "21 2nd Street",  // key 16, value 16
             "city"         : "New York",       // key 8, value 12
             "state"        : "NY",             // key 8, value 4
             "postalCode"   : "10021"           // key 12, value 8
         },                                     // object total: 128
         "phoneNumber":                         // key: 16, value array below         = 172
         [                                      // array: 12 + 2*4 + values below: 156
             {                                  // object 12 + 2*8
               "type"  : "home",                // key 8, value 8
               "number": "212 555-1234"         // key 8, value 16
             },                                 // object total: 68
             {                                  // object 12 + 2*8
               "type"  : "fax",                 // key 8, value 8
               "number": "646 555-4567"         // key 8, value 16
             }                                  // object total: 68
         ]                                      // array total: 156
    }                                           // great total:                         412 bytes

    The uncompressed text file used roughly 500 bytes, so in this case we end up using about
    the same space as the text representation.

    Other measurements have shown a slightly bigger binary size than a compact text
    representation where all possible whitespace was stripped out.
*/
#define Q_DECLARE_JSONPRIVATE_TYPEINFO(Class, Flags) } Q_DECLARE_TYPEINFO(QJsonPrivate::Class, Flags); namespace QJsonPrivate {
namespace QJsonPrivate {

class Array;
class Object;
class Value;
class Entry;

template<typename T>
class q_littleendian
{
public:
    T val;

    q_littleendian &operator =(T i) { val = qToLittleEndian(i); return *this; }
    operator T() const { return qFromLittleEndian(val); }

    bool operator ==(T i) { return qFromLittleEndian(val) == i; }
    bool operator !=(T i) { return qFromLittleEndian(val) != i; }
    bool operator ==(q_littleendian<T> i) { return val == i.val; }
    bool operator !=(q_littleendian<T> i) { return val != i.val; }
    bool operator <(T i) { return qFromLittleEndian(val) < i; }
    bool operator >(T i) { return qFromLittleEndian(val) > i; }
    bool operator <=(T i) { return qFromLittleEndian(val) <= i; }
    bool operator >=(T i) { return qFromLittleEndian(val) >= i; }
    q_littleendian &operator +=(T i) {
        val = qToLittleEndian(qFromLittleEndian(val) + i);
        return *this;
    }
    q_littleendian &operator |=(T i) {
        val = qToLittleEndian(qFromLittleEndian(val) | i);
        return *this;
    }
    q_littleendian &operator &=(T i) {
        val = qToLittleEndian(qFromLittleEndian(val) & i);
        return *this;
    }
};
} // namespace QJsonPrivate

template <typename T>
class QTypeInfo<QJsonPrivate::q_littleendian<T> >
    : public QTypeInfoMerger<QJsonPrivate::q_littleendian<T>, T> {};

namespace QJsonPrivate {

typedef q_littleendian<short> qle_short;
typedef q_littleendian<unsigned short> qle_ushort;
typedef q_littleendian<int> qle_int;
typedef q_littleendian<unsigned int> qle_uint;

template<int pos, int width>
class qle_bitfield
{
public:
    uint val;

    enum {
        mask = ((1u << width) - 1) << pos
    };

    void operator =(uint t) {
        uint i = qFromLittleEndian(val);
        i &= ~mask;
        i |= t << pos;
        val = qToLittleEndian(i);
    }
    operator uint() const {
        uint t = qFromLittleEndian(val);
        t &= mask;
        t >>= pos;
        return t;
    }
    bool operator !() const {
        return !operator uint();
    }

    bool operator ==(uint t) { return uint(*this) == t; }
    bool operator !=(uint t) { return uint(*this) != t; }
    bool operator <(uint t) { return uint(*this) < t; }
    bool operator >(uint t) { return uint(*this) > t; }
    bool operator <=(uint t) { return uint(*this) <= t; }
    bool operator >=(uint t) { return uint(*this) >= t; }
    qle_bitfield &operator +=(uint i) {
        *this = (uint(*this) + i);
        return *this;
    }
    qle_bitfield &operator -=(uint i) {
        *this = (uint(*this) - i);
        return *this;
    }
    qle_bitfield &operator |=(uint i) {
        *this = (uint(*this) | i);
        return *this;
    }
    qle_bitfield &operator &=(uint i) {
        *this = (uint(*this) & i);
        return *this;
    }
};

template<int pos, int width>
class qle_signedbitfield
{
public:
    uint val;

    enum {
        mask = ((1u << width) - 1) << pos
    };

    void operator =(int t) {
        uint i = qFromLittleEndian(val);
        i &= ~mask;
        i |= t << pos;
        val = qToLittleEndian(i);
    }
    operator int() const {
        uint i = qFromLittleEndian(val);
        i <<= 32 - width - pos;
        int t = (int) i;
        t >>= 32 - width;
        return t;
    }
    bool operator !() const {
        return !operator int();
    }

    bool operator ==(int t) { return int(*this) == t; }
    bool operator !=(int t) { return int(*this) != t; }
    bool operator <(int t) { return int(*this) < t; }
    bool operator >(int t) { return int(*this) > t; }
    bool operator <=(int t) { return int(*this) <= t; }
    bool operator >=(int t) { return int(*this) >= t; }
    qle_signedbitfield &operator +=(int i) {
        *this = (int(*this) + i);
        return *this;
    }
    qle_signedbitfield &operator -=(int i) {
        *this = (int(*this) - i);
        return *this;
    }
};

typedef qle_uint offset;

// round the size up to the next 4 byte boundary
inline int alignedSize(int size) { return (size + 3) & ~3; }

static inline bool useCompressed(const QString &s)
{
    if (s.length() >= 0x8000)
        return false;
    const ushort *uc = (const ushort *)s.constData();
    const ushort *e = uc + s.length();
    while (uc < e) {
        if (*uc > 0xff)
            return false;
        ++uc;
    }
    return true;
}

static inline int qStringSize(const QString &string, bool compress)
{
    int l = 2 + string.length();
    if (!compress)
        l *= 2;
    return alignedSize(l);
}

// returns INT_MAX if it can't compress it into 28 bits
static inline int compressedNumber(double d)
{
    // this relies on details of how ieee floats are represented
    const int exponent_off = 52;
    const quint64 fraction_mask = 0x000fffffffffffffull;
    const quint64 exponent_mask = 0x7ff0000000000000ull;

    quint64 val;
    memcpy (&val, &d, sizeof(double));
    int exp = (int)((val & exponent_mask) >> exponent_off) - 1023;
    if (exp < 0 || exp > 25)
        return INT_MAX;

    quint64 non_int = val & (fraction_mask >> exp);
    if (non_int)
        return INT_MAX;

    bool neg = (val >> 63) != 0;
    val &= fraction_mask;
    val |= ((quint64)1 << 52);
    int res = (int)(val >> (52 - exp));
    return neg ? -res : res;
}

class Latin1String;

class String
{
public:
    explicit String(const char *data) { d = (Data *)data; }

    struct Data {
        qle_uint length;
        qle_ushort utf16[1];
    };

    Data *d;

    int byteSize() const { return sizeof(uint) + sizeof(ushort) * d->length; }
    bool isValid(int maxSize) const {
        // Check byteSize() <= maxSize, avoiding integer overflow
        maxSize -= sizeof(uint);
        return maxSize >= 0 && uint(d->length) <= maxSize / sizeof(ushort);
    }

    inline String &operator=(const QString &str)
    {
        d->length = str.length();
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        const ushort *uc = (const ushort *)str.unicode();
        for (int i = 0; i < str.length(); ++i)
            d->utf16[i] = uc[i];
#else
        memcpy(d->utf16, str.unicode(), str.length()*sizeof(ushort));
#endif
        if (str.length() & 1)
            d->utf16[str.length()] = 0;
        return *this;
    }

    inline bool operator ==(const QString &str) const {
        int slen = str.length();
        int l = d->length;
        if (slen != l)
            return false;
        const ushort *s = (const ushort *)str.constData();
        const qle_ushort *a = d->utf16;
        const ushort *b = s;
        while (l-- && *a == *b)
            a++,b++;
        return (l == -1);
    }
    inline bool operator !=(const QString &str) const {
        return !operator ==(str);
    }
    inline bool operator >=(const QString &str) const {
        // ###
        return toString() >= str;
    }

    inline bool operator<(const Latin1String &str) const;
    inline bool operator>=(const Latin1String &str) const { return !operator <(str); }
    inline bool operator ==(const Latin1String &str) const;

    inline bool operator ==(const String &str) const {
        if (d->length != str.d->length)
            return false;
        return !memcmp(d->utf16, str.d->utf16, d->length*sizeof(ushort));
    }
    inline bool operator<(const String &other) const;
    inline bool operator >=(const String &other) const { return !(*this < other); }

    inline QString toString() const {
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
        return QString((QChar *)d->utf16, d->length);
#else
        int l = d->length;
        QString str(l, Qt::Uninitialized);
        QChar *ch = str.data();
        for (int i = 0; i < l; ++i)
            ch[i] = QChar(d->utf16[i]);
        return str;
#endif
    }

};

class Latin1String
{
public:
    explicit Latin1String(const char *data) { d = (Data *)data; }

    struct Data {
        qle_ushort length;
        char latin1[1];
    };
    Data *d;

    int byteSize() const { return sizeof(ushort) + sizeof(char)*(d->length); }
    bool isValid(int maxSize) const {
        return byteSize() <= maxSize;
    }

    inline Latin1String &operator=(const QString &str)
    {
        int len = d->length = str.length();
        uchar *l = (uchar *)d->latin1;
        const ushort *uc = (const ushort *)str.unicode();
        int i = 0;
#ifdef __SSE2__
        for ( ; i + 16 <= len; i += 16) {
            __m128i chunk1 = _mm_loadu_si128((__m128i*)&uc[i]); // load
            __m128i chunk2 = _mm_loadu_si128((__m128i*)&uc[i + 8]); // load
            // pack the two vector to 16 x 8bits elements
            const __m128i result = _mm_packus_epi16(chunk1, chunk2);
            _mm_storeu_si128((__m128i*)&l[i], result); // store
        }
#  ifdef Q_PROCESSOR_X86_64
        // we can do one more round, of 8 characters
        if (i + 8 <= len) {
            __m128i chunk = _mm_loadu_si128((__m128i*)&uc[i]); // load
            // pack with itself, we'll discard the high part anyway
            chunk = _mm_packus_epi16(chunk, chunk);
            // unaligned 64-bit store
            qToUnaligned(_mm_cvtsi128_si64(chunk), l + i);
            i += 8;
        }
#  endif
#endif
        for ( ; i < len; ++i)
            l[i] = uc[i];
        for ( ; (quintptr)(l+i) & 0x3; ++i)
            l[i] = 0;
        return *this;
    }

    QLatin1String toQLatin1String() const Q_DECL_NOTHROW {
        return QLatin1String(d->latin1, d->length);
    }

    inline bool operator<(const String &str) const
    {
        const qle_ushort *uc = (qle_ushort *) str.d->utf16;
        if (!uc || *uc == 0)
            return false;

        const uchar *c = (uchar *)d->latin1;
        const uchar *e = c + qMin((int)d->length, (int)str.d->length);

        while (c < e) {
            if (*c != *uc)
                break;
            ++c;
            ++uc;
        }
        return (c == e ? (int)d->length < (int)str.d->length : *c < *uc);

    }
    inline bool operator ==(const String &str) const {
        return (str == *this);
    }
    inline bool operator >=(const String &str) const {
        return !(*this < str);
    }

    inline QString toString() const {
        return QString::fromLatin1(d->latin1, d->length);
    }
};

#define DEF_OP(op) \
    inline bool operator op(Latin1String lhs, Latin1String rhs) Q_DECL_NOTHROW \
    { \
        return lhs.toQLatin1String() op rhs.toQLatin1String(); \
    } \
    inline bool operator op(QLatin1String lhs, Latin1String rhs) Q_DECL_NOTHROW \
    { \
        return lhs op rhs.toQLatin1String(); \
    } \
    inline bool operator op(Latin1String lhs, QLatin1String rhs) Q_DECL_NOTHROW \
    { \
        return lhs.toQLatin1String() op rhs; \
    } \
    inline bool operator op(const QString &lhs, Latin1String rhs) Q_DECL_NOTHROW \
    { \
        return lhs op rhs.toQLatin1String(); \
    } \
    inline bool operator op(Latin1String lhs, const QString &rhs) Q_DECL_NOTHROW \
    { \
        return lhs.toQLatin1String() op rhs; \
    } \
    /*end*/
DEF_OP(==)
DEF_OP(!=)
DEF_OP(< )
DEF_OP(> )
DEF_OP(<=)
DEF_OP(>=)
#undef DEF_OP

inline bool String::operator ==(const Latin1String &str) const
{
    if ((int)d->length != (int)str.d->length)
        return false;
    const qle_ushort *uc = d->utf16;
    const qle_ushort *e = uc + d->length;
    const uchar *c = (uchar *)str.d->latin1;

    while (uc < e) {
        if (*uc != *c)
            return false;
        ++uc;
        ++c;
    }
    return true;
}

inline bool String::operator <(const String &other) const
{
    int alen = d->length;
    int blen = other.d->length;
    int l = qMin(alen, blen);
    qle_ushort *a = d->utf16;
    qle_ushort *b = other.d->utf16;

    while (l-- && *a == *b)
        a++,b++;
    if (l==-1)
        return (alen < blen);
    return (ushort)*a < (ushort)*b;
}

inline bool String::operator<(const Latin1String &str) const
{
    const uchar *c = (uchar *) str.d->latin1;
    if (!c || *c == 0)
        return false;

    const qle_ushort *uc = d->utf16;
    const qle_ushort *e = uc + qMin((int)d->length, (int)str.d->length);

    while (uc < e) {
        if (*uc != *c)
            break;
        ++uc;
        ++c;
    }
    return (uc == e ? (int)d->length < (int)str.d->length : (ushort)*uc < *c);

}

static inline void copyString(char *dest, const QString &str, bool compress)
{
    if (compress) {
        Latin1String string(dest);
        string = str;
    } else {
        String string(dest);
        string = str;
    }
}


/*
 Base is the base class for both Object and Array. Both classe work more or less the same way.
 The class starts with a header (defined by the struct below), then followed by data (the data for
 values in the Array case and Entry's (see below) for objects.

 After the data a table follows (tableOffset points to it) containing Value objects for Arrays, and
 offsets from the beginning of the object to Entry's in the case of Object.

 Entry's in the Object's table are lexicographically sorted by key in the table(). This allows the usage
 of a binary search over the keys in an Object.
 */
class Base
{
public:
    qle_uint size;
    union {
        uint _dummy;
        qle_bitfield<0, 1> is_object;
        qle_bitfield<1, 31> length;
    };
    offset tableOffset;
    // content follows here

    inline bool isObject() const { return !!is_object; }
    inline bool isArray() const { return !isObject(); }

    inline offset *table() const { return (offset *) (((char *) this) + tableOffset); }

    int reserveSpace(uint dataSize, int posInTable, uint numItems, bool replace);
    void removeItems(int pos, int numItems);
};

class Object : public Base
{
public:
    Entry *entryAt(int i) const {
        return reinterpret_cast<Entry *>(((char *)this) + table()[i]);
    }
    int indexOf(const QString &key, bool *exists) const;
    int indexOf(QLatin1String key, bool *exists) const;

    bool isValid(int maxSize) const;
};


class Array : public Base
{
public:
    inline Value at(int i) const;
    inline Value &operator [](int i);

    bool isValid(int maxSize) const;
};


class Value
{
public:
    enum {
        MaxSize = (1<<27) - 1
    };
    union {
        uint _dummy;
        qle_bitfield<0, 3> type;
        qle_bitfield<3, 1> latinOrIntValue;
        qle_bitfield<4, 1> latinKey;
        qle_bitfield<5, 27> value;
        qle_signedbitfield<5, 27> int_value;
    };

    inline char *data(const Base *b) const { return ((char *)b) + value; }
    int usedStorage(const Base *b) const;

    bool toBoolean() const;
    double toDouble(const Base *b) const;
    QString toString(const Base *b) const;
    String asString(const Base *b) const;
    Latin1String asLatin1String(const Base *b) const;
    Base *base(const Base *b) const;

    bool isValid(const Base *b) const;

    static int requiredStorage(QJsonValue &v, bool *compressed);
    static uint valueToStore(const QJsonValue &v, uint offset);
    static void copyData(const QJsonValue &v, char *dest, bool compressed);
};
Q_DECLARE_JSONPRIVATE_TYPEINFO(Value, Q_PRIMITIVE_TYPE)

inline Value Array::at(int i) const
{
    return *(Value *) (table() + i);
}

inline Value &Array::operator [](int i)
{
    return *(Value *) (table() + i);
}



class Entry {
public:
    Value value;
    // key
    // value data follows key

    uint size() const {
        int s = sizeof(Entry);
        if (value.latinKey)
            s += shallowLatin1Key().byteSize();
        else
            s += shallowKey().byteSize();
        return alignedSize(s);
    }

    int usedStorage(Base *b) const {
        return size() + value.usedStorage(b);
    }

    String shallowKey() const
    {
        Q_ASSERT(!value.latinKey);
        return String((const char *)this + sizeof(Entry));
    }
    Latin1String shallowLatin1Key() const
    {
        Q_ASSERT(value.latinKey);
        return Latin1String((const char *)this + sizeof(Entry));
    }
    QString key() const
    {
        if (value.latinKey) {
            return shallowLatin1Key().toString();
        }
        return shallowKey().toString();
    }

    bool isValid(int maxSize) const {
        if (maxSize < (int)sizeof(Entry))
            return false;
        maxSize -= sizeof(Entry);
        if (value.latinKey)
            return shallowLatin1Key().isValid(maxSize);
        return shallowKey().isValid(maxSize);
    }

    bool operator ==(const QString &key) const;
    inline bool operator !=(const QString &key) const { return !operator ==(key); }
    inline bool operator >=(const QString &key) const;

    bool operator==(QLatin1String key) const;
    inline bool operator!=(QLatin1String key) const { return !operator ==(key); }
    inline bool operator>=(QLatin1String key) const;

    bool operator ==(const Entry &other) const;
    bool operator >=(const Entry &other) const;
};

inline bool Entry::operator >=(const QString &key) const
{
    if (value.latinKey)
        return (shallowLatin1Key() >= key);
    else
        return (shallowKey() >= key);
}

inline bool Entry::operator >=(QLatin1String key) const
{
    if (value.latinKey)
        return shallowLatin1Key() >= key;
    else
        return shallowKey() >= key;
}

inline bool operator <(const QString &key, const Entry &e)
{ return e >= key; }

inline bool operator<(QLatin1String key, const Entry &e)
{ return e >= key; }


class Header {
public:
    qle_uint tag; // 'qbjs'
    qle_uint version; // 1
    Base *root() { return (Base *)(this + 1); }
};


inline bool Value::toBoolean() const
{
    Q_ASSERT(type == QJsonValue::Bool);
    return value != 0;
}

inline double Value::toDouble(const Base *b) const
{
    Q_ASSERT(type == QJsonValue::Double);
    if (latinOrIntValue)
        return int_value;

    quint64 i = qFromLittleEndian<quint64>((const uchar *)b + value);
    double d;
    memcpy(&d, &i, sizeof(double));
    return d;
}

inline String Value::asString(const Base *b) const
{
    Q_ASSERT(type == QJsonValue::String && !latinOrIntValue);
    return String(data(b));
}

inline Latin1String Value::asLatin1String(const Base *b) const
{
    Q_ASSERT(type == QJsonValue::String && latinOrIntValue);
    return Latin1String(data(b));
}

inline QString Value::toString(const Base *b) const
{
    if (latinOrIntValue)
        return asLatin1String(b).toString();
    else
        return asString(b).toString();
}

inline Base *Value::base(const Base *b) const
{
    Q_ASSERT(type == QJsonValue::Array || type == QJsonValue::Object);
    return reinterpret_cast<Base *>(data(b));
}

class Data {
public:
    enum Validation {
        Unchecked,
        Validated,
        Invalid
    };

    QAtomicInt ref;
    int alloc;
    union {
        char *rawData;
        Header *header;
    };
    uint compactionCounter : 31;
    uint ownsData : 1;

    inline Data(char *raw, int a)
        : alloc(a), rawData(raw), compactionCounter(0), ownsData(true)
    {
    }
    inline Data(int reserved, QJsonValue::Type valueType)
        : rawData(0), compactionCounter(0), ownsData(true)
    {
        Q_ASSERT(valueType == QJsonValue::Array || valueType == QJsonValue::Object);

        alloc = sizeof(Header) + sizeof(Base) + reserved + sizeof(offset);
        header = (Header *)malloc(alloc);
        Q_CHECK_PTR(header);
        header->tag = QJsonDocument::BinaryFormatTag;
        header->version = 1;
        Base *b = header->root();
        b->size = sizeof(Base);
        b->is_object = (valueType == QJsonValue::Object);
        b->tableOffset = sizeof(Base);
        b->length = 0;
    }
    inline ~Data()
    { if (ownsData) free(rawData); }

    uint offsetOf(const void *ptr) const { return (uint)(((char *)ptr - rawData)); }

    QJsonObject toObject(Object *o) const
    {
        return QJsonObject(const_cast<Data *>(this), o);
    }

    QJsonArray toArray(Array *a) const
    {
        return QJsonArray(const_cast<Data *>(this), a);
    }

    Data *clone(Base *b, int reserve = 0)
    {
        int size = sizeof(Header) + b->size;
        if (b == header->root() && ref.load() == 1 && alloc >= size + reserve)
            return this;

        if (reserve) {
            if (reserve < 128)
                reserve = 128;
            size = qMax(size + reserve, qMin(size *2, (int)Value::MaxSize));
            if (size > Value::MaxSize) {
                qWarning("QJson: Document too large to store in data structure");
                return 0;
            }
        }
        char *raw = (char *)malloc(size);
        Q_CHECK_PTR(raw);
        memcpy(raw + sizeof(Header), b, b->size);
        Header *h = (Header *)raw;
        h->tag = QJsonDocument::BinaryFormatTag;
        h->version = 1;
        Data *d = new Data(raw, size);
        d->compactionCounter = (b == header->root()) ? compactionCounter : 0;
        return d;
    }

    void compact();
    bool valid() const;

private:
    Q_DISABLE_COPY(Data)
};

}

QT_END_NAMESPACE

#endif // QJSON_P_H
