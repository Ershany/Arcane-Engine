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

#ifndef QGLOBAL_H
#define QGLOBAL_H

#ifdef __cplusplus
#  include <type_traits>
#  include <cstddef>
#  include <utility>
#endif

#include <stddef.h>

/*
   QT_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define QT_VERSION      QT_VERSION_CHECK(QT_VERSION_MAJOR, QT_VERSION_MINOR, QT_VERSION_PATCH)
/*
   can be used like #if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
*/
#define QT_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

#ifdef QT_BOOTSTRAPPED
#include <QtCore/qconfig-bootstrapped.h>
#else
#include <QtCore/qconfig.h>
#include <QtCore/qtcore-config.h>
#endif

// The QT_SUPPORTS macro is deprecated. Don't use it in new code.
// Instead, use QT_CONFIG(feature)
// ### Qt6: remove macro
#ifdef _MSC_VER
#  define QT_SUPPORTS(FEATURE) (!defined QT_NO_##FEATURE)
#else
#  define QT_SUPPORTS(FEATURE) (!defined(QT_NO_##FEATURE))
#endif

/*
    The QT_CONFIG macro implements a safe compile time check for features of Qt.
    Features can be in three states:
        0 or undefined: This will lead to a compile error when testing for it
        -1: The feature is not available
        1: The feature is available
*/
#define QT_CONFIG(feature) (1/QT_FEATURE_##feature == 1)
#define QT_REQUIRE_CONFIG(feature) Q_STATIC_ASSERT_X(QT_FEATURE_##feature == 1, "Required feature " #feature " for file " __FILE__ " not available.")

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
#  define QT_NO_UNSHARABLE_CONTAINERS
#endif

/* These two macros makes it possible to turn the builtin line expander into a
 * string literal. */
#define QT_STRINGIFY2(x) #x
#define QT_STRINGIFY(x) QT_STRINGIFY2(x)

#include <QtCore/qsystemdetection.h>
#include <QtCore/qprocessordetection.h>
#include <QtCore/qcompilerdetection.h>

#if defined (__ELF__)
#  define Q_OF_ELF
#endif
#if defined (__MACH__) && defined (__APPLE__)
#  define Q_OF_MACH_O
#endif

#ifdef __cplusplus

#include <algorithm>

#if !defined(QT_NAMESPACE) || defined(Q_MOC_RUN) /* user namespace */

# define QT_PREPEND_NAMESPACE(name) ::name
# define QT_USE_NAMESPACE
# define QT_BEGIN_NAMESPACE
# define QT_END_NAMESPACE
# define QT_BEGIN_INCLUDE_NAMESPACE
# define QT_END_INCLUDE_NAMESPACE
#ifndef QT_BEGIN_MOC_NAMESPACE
# define QT_BEGIN_MOC_NAMESPACE
#endif
#ifndef QT_END_MOC_NAMESPACE
# define QT_END_MOC_NAMESPACE
#endif
# define QT_FORWARD_DECLARE_CLASS(name) class name;
# define QT_FORWARD_DECLARE_STRUCT(name) struct name;
# define QT_MANGLE_NAMESPACE(name) name

#else /* user namespace */

# define QT_PREPEND_NAMESPACE(name) ::QT_NAMESPACE::name
# define QT_USE_NAMESPACE using namespace ::QT_NAMESPACE;
# define QT_BEGIN_NAMESPACE namespace QT_NAMESPACE {
# define QT_END_NAMESPACE }
# define QT_BEGIN_INCLUDE_NAMESPACE }
# define QT_END_INCLUDE_NAMESPACE namespace QT_NAMESPACE {
#ifndef QT_BEGIN_MOC_NAMESPACE
# define QT_BEGIN_MOC_NAMESPACE QT_USE_NAMESPACE
#endif
#ifndef QT_END_MOC_NAMESPACE
# define QT_END_MOC_NAMESPACE
#endif
# define QT_FORWARD_DECLARE_CLASS(name) \
    QT_BEGIN_NAMESPACE class name; QT_END_NAMESPACE \
    using QT_PREPEND_NAMESPACE(name);

# define QT_FORWARD_DECLARE_STRUCT(name) \
    QT_BEGIN_NAMESPACE struct name; QT_END_NAMESPACE \
    using QT_PREPEND_NAMESPACE(name);

# define QT_MANGLE_NAMESPACE0(x) x
# define QT_MANGLE_NAMESPACE1(a, b) a##_##b
# define QT_MANGLE_NAMESPACE2(a, b) QT_MANGLE_NAMESPACE1(a,b)
# define QT_MANGLE_NAMESPACE(name) QT_MANGLE_NAMESPACE2( \
        QT_MANGLE_NAMESPACE0(name), QT_MANGLE_NAMESPACE0(QT_NAMESPACE))

namespace QT_NAMESPACE {}

# ifndef QT_BOOTSTRAPPED
# ifndef QT_NO_USING_NAMESPACE
   /*
    This expands to a "using QT_NAMESPACE" also in _header files_.
    It is the only way the feature can be used without too much
    pain, but if people _really_ do not want it they can add
    DEFINES += QT_NO_USING_NAMESPACE to their .pro files.
    */
   QT_USE_NAMESPACE
# endif
# endif

#endif /* user namespace */

#else /* __cplusplus */

# define QT_BEGIN_NAMESPACE
# define QT_END_NAMESPACE
# define QT_USE_NAMESPACE
# define QT_BEGIN_INCLUDE_NAMESPACE
# define QT_END_INCLUDE_NAMESPACE

#endif /* __cplusplus */

// ### Qt6: remove me.
#define QT_BEGIN_HEADER
#define QT_END_HEADER

#if defined(Q_OS_DARWIN) && !defined(QT_LARGEFILE_SUPPORT)
#  define QT_LARGEFILE_SUPPORT 64
#endif

QT_BEGIN_NAMESPACE

/*
   Size-dependent types (architechture-dependent byte order)

   Make sure to update QMetaType when changing these typedefs
*/

typedef signed char qint8;         /* 8 bit signed */
typedef unsigned char quint8;      /* 8 bit unsigned */
typedef short qint16;              /* 16 bit signed */
typedef unsigned short quint16;    /* 16 bit unsigned */
typedef int qint32;                /* 32 bit signed */
typedef unsigned int quint32;      /* 32 bit unsigned */
#if defined(Q_OS_WIN) && !defined(Q_CC_GNU)
#  define Q_INT64_C(c) c ## i64    /* signed 64 bit constant */
#  define Q_UINT64_C(c) c ## ui64   /* unsigned 64 bit constant */
typedef __int64 qint64;            /* 64 bit signed */
typedef unsigned __int64 quint64;  /* 64 bit unsigned */
#else
#  define Q_INT64_C(c) static_cast<long long>(c ## LL)     /* signed 64 bit constant */
#  define Q_UINT64_C(c) static_cast<unsigned long long>(c ## ULL) /* unsigned 64 bit constant */
typedef long long qint64;           /* 64 bit signed */
typedef unsigned long long quint64; /* 64 bit unsigned */
#endif

typedef qint64 qlonglong;
typedef quint64 qulonglong;

/*
   Useful type definitions for Qt
*/

QT_BEGIN_INCLUDE_NAMESPACE
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
QT_END_INCLUDE_NAMESPACE

#if defined(QT_COORD_TYPE)
typedef QT_COORD_TYPE qreal;
#else
typedef double qreal;
#endif

#if defined(QT_NO_DEPRECATED)
#  undef QT_DEPRECATED
#  undef QT_DEPRECATED_X
#  undef QT_DEPRECATED_VARIABLE
#  undef QT_DEPRECATED_CONSTRUCTOR
#elif defined(QT_DEPRECATED_WARNINGS)
#  undef QT_DEPRECATED
#  define QT_DEPRECATED Q_DECL_DEPRECATED
#  undef QT_DEPRECATED_X
#  define QT_DEPRECATED_X(text) Q_DECL_DEPRECATED_X(text)
#  undef QT_DEPRECATED_VARIABLE
#  define QT_DEPRECATED_VARIABLE Q_DECL_VARIABLE_DEPRECATED
#  undef QT_DEPRECATED_CONSTRUCTOR
#  define QT_DEPRECATED_CONSTRUCTOR explicit Q_DECL_CONSTRUCTOR_DEPRECATED
#else
#  undef QT_DEPRECATED
#  define QT_DEPRECATED
#  undef QT_DEPRECATED_X
#  define QT_DEPRECATED_X(text)
#  undef QT_DEPRECATED_VARIABLE
#  define QT_DEPRECATED_VARIABLE
#  undef QT_DEPRECATED_CONSTRUCTOR
#  define QT_DEPRECATED_CONSTRUCTOR
#endif

#ifndef QT_DISABLE_DEPRECATED_BEFORE
#define QT_DISABLE_DEPRECATED_BEFORE QT_VERSION_CHECK(5, 0, 0)
#endif

/*
    QT_DEPRECATED_SINCE(major, minor) evaluates as true if the Qt version is greater than
    the deprecation point specified.

    Use it to specify from which version of Qt a function or class has been deprecated

    Example:
        #if QT_DEPRECATED_SINCE(5,1)
            QT_DEPRECATED void deprecatedFunction(); //function deprecated since Qt 5.1
        #endif

*/
#ifdef QT_DEPRECATED
#define QT_DEPRECATED_SINCE(major, minor) (QT_VERSION_CHECK(major, minor, 0) > QT_DISABLE_DEPRECATED_BEFORE)
#else
#define QT_DEPRECATED_SINCE(major, minor) 0
#endif

/*
   The Qt modules' export macros.
   The options are:
    - defined(QT_STATIC): Qt was built or is being built in static mode
    - defined(QT_SHARED): Qt was built or is being built in shared/dynamic mode
   If neither was defined, then QT_SHARED is implied. If Qt was compiled in static
   mode, QT_STATIC is defined in qconfig.h. In shared mode, QT_STATIC is implied
   for the bootstrapped tools.
*/

#ifdef QT_BOOTSTRAPPED
#  ifdef QT_SHARED
#    error "QT_SHARED and QT_BOOTSTRAPPED together don't make sense. Please fix the build"
#  elif !defined(QT_STATIC)
#    define QT_STATIC
#  endif
#endif

#if defined(QT_SHARED) || !defined(QT_STATIC)
#  ifdef QT_STATIC
#    error "Both QT_SHARED and QT_STATIC defined, please make up your mind"
#  endif
#  ifndef QT_SHARED
#    define QT_SHARED
#  endif
#  if defined(QT_BUILD_CORE_LIB)
#    define Q_CORE_EXPORT Q_DECL_EXPORT
#  else
#    define Q_CORE_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_CORE_EXPORT
#endif

/*
   Some classes do not permit copies to be made of an object. These
   classes contains a private copy constructor and assignment
   operator to disable copying (the compiler gives an error message).
*/
#define Q_DISABLE_COPY(Class) \
    Class(const Class &) Q_DECL_EQ_DELETE;\
    Class &operator=(const Class &) Q_DECL_EQ_DELETE;

/*
   No, this is not an evil backdoor. QT_BUILD_INTERNAL just exports more symbols
   for Qt's internal unit tests. If you want slower loading times and more
   symbols that can vanish from version to version, feel free to define QT_BUILD_INTERNAL.
*/
#if defined(QT_BUILD_INTERNAL) && defined(QT_BUILDING_QT) && defined(QT_SHARED)
#    define Q_AUTOTEST_EXPORT Q_DECL_EXPORT
#elif defined(QT_BUILD_INTERNAL) && defined(QT_SHARED)
#    define Q_AUTOTEST_EXPORT Q_DECL_IMPORT
#else
#    define Q_AUTOTEST_EXPORT
#endif

#define Q_INIT_RESOURCE(name) \
    do { extern int QT_MANGLE_NAMESPACE(qInitResources_ ## name) ();       \
        QT_MANGLE_NAMESPACE(qInitResources_ ## name) (); } while (false)
#define Q_CLEANUP_RESOURCE(name) \
    do { extern int QT_MANGLE_NAMESPACE(qCleanupResources_ ## name) ();    \
        QT_MANGLE_NAMESPACE(qCleanupResources_ ## name) (); } while (false)

/*
 * If we're compiling C++ code:
 *  - and this is a non-namespace build, declare qVersion as extern "C"
 *  - and this is a namespace build, declare it as a regular function
 *    (we're already inside QT_BEGIN_NAMESPACE / QT_END_NAMESPACE)
 * If we're compiling C code, simply declare the function. If Qt was compiled
 * in a namespace, qVersion isn't callable anyway.
 */
#if !defined(QT_NAMESPACE) && defined(__cplusplus) && !defined(Q_QDOC)
extern "C"
#endif
Q_CORE_EXPORT const char *qVersion(void) Q_DECL_NOTHROW;

#if defined(__cplusplus)

#ifndef Q_CONSTRUCTOR_FUNCTION
# define Q_CONSTRUCTOR_FUNCTION0(AFUNC) \
    namespace { \
    static const struct AFUNC ## _ctor_class_ { \
        inline AFUNC ## _ctor_class_() { AFUNC(); } \
    } AFUNC ## _ctor_instance_; \
    }

# define Q_CONSTRUCTOR_FUNCTION(AFUNC) Q_CONSTRUCTOR_FUNCTION0(AFUNC)
#endif

#ifndef Q_DESTRUCTOR_FUNCTION
# define Q_DESTRUCTOR_FUNCTION0(AFUNC) \
    namespace { \
    static const struct AFUNC ## _dtor_class_ { \
        inline AFUNC ## _dtor_class_() { } \
        inline ~ AFUNC ## _dtor_class_() { AFUNC(); } \
    } AFUNC ## _dtor_instance_; \
    }
# define Q_DESTRUCTOR_FUNCTION(AFUNC) Q_DESTRUCTOR_FUNCTION0(AFUNC)
#endif

namespace QtPrivate {
    template <class T>
    struct AlignOfHelper
    {
        char c;
        T type;

        AlignOfHelper();
        ~AlignOfHelper();
    };

    template <class T>
    struct AlignOf_Default
    {
        enum { Value = sizeof(AlignOfHelper<T>) - sizeof(T) };
    };

    template <class T> struct AlignOf : AlignOf_Default<T> { };
    template <class T> struct AlignOf<T &> : AlignOf<T> {};
    template <size_t N, class T> struct AlignOf<T[N]> : AlignOf<T> {};

#ifdef Q_COMPILER_RVALUE_REFS
    template <class T> struct AlignOf<T &&> : AlignOf<T> {};
#endif

#if defined(Q_PROCESSOR_X86_32) && !defined(Q_OS_WIN)
    template <class T> struct AlignOf_WorkaroundForI386Abi { enum { Value = sizeof(T) }; };

    // x86 ABI weirdness
    // Alignment of naked type is 8, but inside struct has alignment 4.
    template <> struct AlignOf<double>  : AlignOf_WorkaroundForI386Abi<double> {};
    template <> struct AlignOf<qint64>  : AlignOf_WorkaroundForI386Abi<qint64> {};
    template <> struct AlignOf<quint64> : AlignOf_WorkaroundForI386Abi<quint64> {};
#ifdef Q_CC_CLANG
    // GCC and Clang seem to disagree wrt to alignment of arrays
    template <size_t N> struct AlignOf<double[N]>   : AlignOf_Default<double> {};
    template <size_t N> struct AlignOf<qint64[N]>   : AlignOf_Default<qint64> {};
    template <size_t N> struct AlignOf<quint64[N]>  : AlignOf_Default<quint64> {};
#endif
#endif
} // namespace QtPrivate

#define QT_EMULATED_ALIGNOF(T) \
    (size_t(QT_PREPEND_NAMESPACE(QtPrivate)::AlignOf<T>::Value))

#ifndef Q_ALIGNOF
#define Q_ALIGNOF(T) QT_EMULATED_ALIGNOF(T)
#endif


/*
  quintptr and qptrdiff is guaranteed to be the same size as a pointer, i.e.

      sizeof(void *) == sizeof(quintptr)
      && sizeof(void *) == sizeof(qptrdiff)
*/
template <int> struct QIntegerForSize;
template <>    struct QIntegerForSize<1> { typedef quint8  Unsigned; typedef qint8  Signed; };
template <>    struct QIntegerForSize<2> { typedef quint16 Unsigned; typedef qint16 Signed; };
template <>    struct QIntegerForSize<4> { typedef quint32 Unsigned; typedef qint32 Signed; };
template <>    struct QIntegerForSize<8> { typedef quint64 Unsigned; typedef qint64 Signed; };
#if defined(Q_CC_GNU) && defined(__SIZEOF_INT128__)
template <>    struct QIntegerForSize<16> { __extension__ typedef unsigned __int128 Unsigned; __extension__ typedef __int128 Signed; };
#endif
template <class T> struct QIntegerForSizeof: QIntegerForSize<sizeof(T)> { };
typedef QIntegerForSize<Q_PROCESSOR_WORDSIZE>::Signed qregisterint;
typedef QIntegerForSize<Q_PROCESSOR_WORDSIZE>::Unsigned qregisteruint;
typedef QIntegerForSizeof<void*>::Unsigned quintptr;
typedef QIntegerForSizeof<void*>::Signed qptrdiff;
typedef qptrdiff qintptr;

/* moc compats (signals/slots) */
#ifndef QT_MOC_COMPAT
#  define QT_MOC_COMPAT
#else
#  undef QT_MOC_COMPAT
#  define QT_MOC_COMPAT
#endif

#ifdef QT_ASCII_CAST_WARNINGS
#  define QT_ASCII_CAST_WARN Q_DECL_DEPRECATED_X("Use fromUtf8, QStringLiteral, or QLatin1String")
#else
#  define QT_ASCII_CAST_WARN
#endif

#if defined(__i386__) || defined(_WIN32)
#  if defined(Q_CC_GNU)
#    define QT_FASTCALL __attribute__((regparm(3)))
#  elif defined(Q_CC_MSVC)
#    define QT_FASTCALL __fastcall
#  else
#     define QT_FASTCALL
#  endif
#else
#  define QT_FASTCALL
#endif

// enable gcc warnings for printf-style functions
#if defined(Q_CC_GNU) && !defined(__INSURE__)
#  if defined(Q_CC_MINGW) && !defined(Q_CC_CLANG)
#    define Q_ATTRIBUTE_FORMAT_PRINTF(A, B) \
         __attribute__((format(gnu_printf, (A), (B))))
#  else
#    define Q_ATTRIBUTE_FORMAT_PRINTF(A, B) \
         __attribute__((format(printf, (A), (B))))
#  endif
#else
#  define Q_ATTRIBUTE_FORMAT_PRINTF(A, B)
#endif

#ifdef Q_CC_MSVC
#  define Q_NEVER_INLINE __declspec(noinline)
#  define Q_ALWAYS_INLINE __forceinline
#elif defined(Q_CC_GNU)
#  define Q_NEVER_INLINE __attribute__((noinline))
#  define Q_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#  define Q_NEVER_INLINE
#  define Q_ALWAYS_INLINE inline
#endif

//defines the type for the WNDPROC on windows
//the alignment needs to be forced for sse2 to not crash with mingw
#if defined(Q_OS_WIN)
#  if defined(Q_CC_MINGW) && !defined(Q_OS_WIN64)
#    define QT_ENSURE_STACK_ALIGNED_FOR_SSE __attribute__ ((force_align_arg_pointer))
#  else
#    define QT_ENSURE_STACK_ALIGNED_FOR_SSE
#  endif
#  define QT_WIN_CALLBACK CALLBACK QT_ENSURE_STACK_ALIGNED_FOR_SSE
#endif

typedef int QNoImplicitBoolCast;

/*
   Utility macros and inline functions
*/

template <typename T>
Q_DECL_CONSTEXPR inline T qAbs(const T &t) { return t >= 0 ? t : -t; }

Q_DECL_CONSTEXPR inline int qRound(double d)
{ return d >= 0.0 ? int(d + 0.5) : int(d - double(int(d-1)) + 0.5) + int(d-1); }
Q_DECL_CONSTEXPR inline int qRound(float d)
{ return d >= 0.0f ? int(d + 0.5f) : int(d - float(int(d-1)) + 0.5f) + int(d-1); }

Q_DECL_CONSTEXPR inline qint64 qRound64(double d)
{ return d >= 0.0 ? qint64(d + 0.5) : qint64(d - double(qint64(d-1)) + 0.5) + qint64(d-1); }
Q_DECL_CONSTEXPR inline qint64 qRound64(float d)
{ return d >= 0.0f ? qint64(d + 0.5f) : qint64(d - float(qint64(d-1)) + 0.5f) + qint64(d-1); }

template <typename T>
Q_DECL_CONSTEXPR inline const T &qMin(const T &a, const T &b) { return (a < b) ? a : b; }
template <typename T>
Q_DECL_CONSTEXPR inline const T &qMax(const T &a, const T &b) { return (a < b) ? b : a; }
template <typename T>
Q_DECL_CONSTEXPR inline const T &qBound(const T &min, const T &val, const T &max)
{ return qMax(min, qMin(max, val)); }

#ifndef Q_FORWARD_DECLARE_OBJC_CLASS
#  ifdef __OBJC__
#    define Q_FORWARD_DECLARE_OBJC_CLASS(classname) @class classname
#  else
#    define Q_FORWARD_DECLARE_OBJC_CLASS(classname) typedef struct objc_object classname
#  endif
#endif
#ifndef Q_FORWARD_DECLARE_CF_TYPE
#  define Q_FORWARD_DECLARE_CF_TYPE(type) typedef const struct __ ## type * type ## Ref
#endif
#ifndef Q_FORWARD_DECLARE_MUTABLE_CF_TYPE
#  define Q_FORWARD_DECLARE_MUTABLE_CF_TYPE(type) typedef struct __ ## type * type ## Ref
#endif
#ifndef Q_FORWARD_DECLARE_CG_TYPE
#define Q_FORWARD_DECLARE_CG_TYPE(type) typedef const struct type *type ## Ref;
#endif
#ifndef Q_FORWARD_DECLARE_MUTABLE_CG_TYPE
#define Q_FORWARD_DECLARE_MUTABLE_CG_TYPE(type) typedef struct type *type ## Ref;
#endif

#ifdef Q_OS_DARWIN
#  define QT_DARWIN_PLATFORM_SDK_EQUAL_OR_ABOVE(macos, ios, tvos, watchos) \
    ((defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && macos != __MAC_NA && __MAC_OS_X_VERSION_MAX_ALLOWED >= macos) || \
     (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && ios != __IPHONE_NA && __IPHONE_OS_VERSION_MAX_ALLOWED >= ios) || \
     (defined(__TV_OS_VERSION_MAX_ALLOWED) && tvos != __TVOS_NA && __TV_OS_VERSION_MAX_ALLOWED >= tvos) || \
     (defined(__WATCH_OS_VERSION_MAX_ALLOWED) && watchos != __WATCHOS_NA && __WATCH_OS_VERSION_MAX_ALLOWED >= watchos))

#  define QT_DARWIN_DEPLOYMENT_TARGET_BELOW(macos, ios, tvos, watchos) \
    ((defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && macos != __MAC_NA && __MAC_OS_X_VERSION_MIN_REQUIRED < macos) || \
     (defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && ios != __IPHONE_NA && __IPHONE_OS_VERSION_MIN_REQUIRED < ios) || \
     (defined(__TV_OS_VERSION_MIN_REQUIRED) && tvos != __TVOS_NA && __TV_OS_VERSION_MIN_REQUIRED < tvos) || \
     (defined(__WATCH_OS_VERSION_MIN_REQUIRED) && watchos != __WATCHOS_NA && __WATCH_OS_VERSION_MIN_REQUIRED < watchos))

#  define QT_MACOS_IOS_PLATFORM_SDK_EQUAL_OR_ABOVE(macos, ios) \
      QT_DARWIN_PLATFORM_SDK_EQUAL_OR_ABOVE(macos, ios, __TVOS_NA, __WATCHOS_NA)
#  define QT_MACOS_PLATFORM_SDK_EQUAL_OR_ABOVE(macos) \
      QT_DARWIN_PLATFORM_SDK_EQUAL_OR_ABOVE(macos, __IPHONE_NA, __TVOS_NA, __WATCHOS_NA)
#  define QT_IOS_PLATFORM_SDK_EQUAL_OR_ABOVE(ios) \
      QT_DARWIN_PLATFORM_SDK_EQUAL_OR_ABOVE(__MAC_NA, ios, __TVOS_NA, __WATCHOS_NA)
#  define QT_TVOS_PLATFORM_SDK_EQUAL_OR_ABOVE(tvos) \
      QT_DARWIN_PLATFORM_SDK_EQUAL_OR_ABOVE(__MAC_NA, __IPHONE_NA, tvos, __WATCHOS_NA)
#  define QT_WATCHOS_PLATFORM_SDK_EQUAL_OR_ABOVE(watchos) \
      QT_DARWIN_PLATFORM_SDK_EQUAL_OR_ABOVE(__MAC_NA, __IPHONE_NA, __TVOS_NA, watchos)

#  define QT_MACOS_IOS_DEPLOYMENT_TARGET_BELOW(macos, ios) \
      QT_DARWIN_DEPLOYMENT_TARGET_BELOW(macos, ios, __TVOS_NA, __WATCHOS_NA)
#  define QT_MACOS_DEPLOYMENT_TARGET_BELOW(macos) \
      QT_DARWIN_DEPLOYMENT_TARGET_BELOW(macos, __IPHONE_NA, __TVOS_NA, __WATCHOS_NA)
#  define QT_IOS_DEPLOYMENT_TARGET_BELOW(ios) \
      QT_DARWIN_DEPLOYMENT_TARGET_BELOW(__MAC_NA, ios, __TVOS_NA, __WATCHOS_NA)
#  define QT_TVOS_DEPLOYMENT_TARGET_BELOW(tvos) \
      QT_DARWIN_DEPLOYMENT_TARGET_BELOW(__MAC_NA, __IPHONE_NA, tvos, __WATCHOS_NA)
#  define QT_WATCHOS_DEPLOYMENT_TARGET_BELOW(watchos) \
      QT_DARWIN_DEPLOYMENT_TARGET_BELOW(__MAC_NA, __IPHONE_NA, __TVOS_NA, watchos)

// Compatibility synonyms, do not use
#  define QT_MAC_PLATFORM_SDK_EQUAL_OR_ABOVE(osx, ios) QT_MACOS_IOS_PLATFORM_SDK_EQUAL_OR_ABOVE(osx, ios)
#  define QT_MAC_DEPLOYMENT_TARGET_BELOW(osx, ios) QT_MACOS_IOS_DEPLOYMENT_TARGET_BELOW(osx, ios)
#  define QT_OSX_PLATFORM_SDK_EQUAL_OR_ABOVE(osx) QT_MACOS_PLATFORM_SDK_EQUAL_OR_ABOVE(osx)
#  define QT_OSX_DEPLOYMENT_TARGET_BELOW(osx) QT_MACOS_DEPLOYMENT_TARGET_BELOW(osx)

// Implemented in qcore_mac_objc.mm
class Q_CORE_EXPORT QMacAutoReleasePool
{
public:
    QMacAutoReleasePool();
    ~QMacAutoReleasePool();
private:
    Q_DISABLE_COPY(QMacAutoReleasePool)
    void *pool;
};

#endif // Q_OS_DARWIN

/*
   Data stream functions are provided by many classes (defined in qdatastream.h)
*/

class QDataStream;

inline void qt_noop(void) {}

/* These wrap try/catch so we can switch off exceptions later.

   Beware - do not use more than one QT_CATCH per QT_TRY, and do not use
   the exception instance in the catch block.
   If you can't live with those constraints, don't use these macros.
   Use the QT_NO_EXCEPTIONS macro to protect your code instead.
*/

#if !defined(QT_NO_EXCEPTIONS)
#  if !defined(Q_MOC_RUN)
#    if (defined(Q_CC_CLANG) && !defined(Q_CC_INTEL) && !QT_HAS_FEATURE(cxx_exceptions)) || \
        (defined(Q_CC_GNU) && !defined(__EXCEPTIONS))
#      define QT_NO_EXCEPTIONS
#    endif
#  elif defined(QT_BOOTSTRAPPED)
#    define QT_NO_EXCEPTIONS
#  endif
#endif

#ifdef QT_NO_EXCEPTIONS
#  define QT_TRY if (true)
#  define QT_CATCH(A) else
#  define QT_THROW(A) qt_noop()
#  define QT_RETHROW qt_noop()
#  define QT_TERMINATE_ON_EXCEPTION(expr) do { expr; } while (false)
#else
#  define QT_TRY try
#  define QT_CATCH(A) catch (A)
#  define QT_THROW(A) throw A
#  define QT_RETHROW throw
Q_NORETURN Q_CORE_EXPORT void qTerminate() Q_DECL_NOTHROW;
#  ifdef Q_COMPILER_NOEXCEPT
#    define QT_TERMINATE_ON_EXCEPTION(expr) do { expr; } while (false)
#  else
#    define QT_TERMINATE_ON_EXCEPTION(expr) do { try { expr; } catch (...) { qTerminate(); } } while (false)
#  endif
#endif

Q_CORE_EXPORT bool qSharedBuild() Q_DECL_NOTHROW;

#ifndef Q_OUTOFLINE_TEMPLATE
#  define Q_OUTOFLINE_TEMPLATE
#endif
#ifndef Q_INLINE_TEMPLATE
#  define Q_INLINE_TEMPLATE inline
#endif

/*
   Avoid "unused parameter" warnings
*/
#define Q_UNUSED(x) (void)x;

/*
   Debugging and error handling
*/

#if !defined(QT_NO_DEBUG) && !defined(QT_DEBUG)
#  define QT_DEBUG
#endif

#ifndef qPrintable
#  define qPrintable(string) QString(string).toLocal8Bit().constData()
#endif

#ifndef qUtf8Printable
#  define qUtf8Printable(string) QString(string).toUtf8().constData()
#endif

/*
    Wrap QString::utf16() with enough casts to allow passing it
    to QString::asprintf("%ls") without warnings.
*/
#ifndef qUtf16Printable
#  define qUtf16Printable(string) \
    static_cast<const wchar_t*>(static_cast<const void*>(QString(string).utf16()))
#endif

class QString;
Q_CORE_EXPORT QString qt_error_string(int errorCode = -1);

#ifndef Q_CC_MSVC
Q_NORETURN
#endif
Q_CORE_EXPORT void qt_assert(const char *assertion, const char *file, int line) Q_DECL_NOTHROW;

#if !defined(Q_ASSERT)
#  if defined(QT_NO_DEBUG) && !defined(QT_FORCE_ASSERTS)
#    define Q_ASSERT(cond) do { } while ((false) && (cond))
#  else
#    define Q_ASSERT(cond) ((!(cond)) ? qt_assert(#cond,__FILE__,__LINE__) : qt_noop())
#  endif
#endif

#if defined(QT_NO_DEBUG) && !defined(QT_PAINT_DEBUG)
#define QT_NO_PAINT_DEBUG
#endif

#ifndef Q_CC_MSVC
Q_NORETURN
#endif
Q_CORE_EXPORT void qt_assert_x(const char *where, const char *what, const char *file, int line) Q_DECL_NOTHROW;

#if !defined(Q_ASSERT_X)
#  if defined(QT_NO_DEBUG) && !defined(QT_FORCE_ASSERTS)
#    define Q_ASSERT_X(cond, where, what) do { } while ((false) && (cond))
#  else
#    define Q_ASSERT_X(cond, where, what) ((!(cond)) ? qt_assert_x(where, what,__FILE__,__LINE__) : qt_noop())
#  endif
#endif


#ifdef Q_COMPILER_STATIC_ASSERT
#define Q_STATIC_ASSERT(Condition) static_assert(bool(Condition), #Condition)
#define Q_STATIC_ASSERT_X(Condition, Message) static_assert(bool(Condition), Message)
#else
// Intentionally undefined
template <bool Test> class QStaticAssertFailure;
template <> class QStaticAssertFailure<true> {};

#define Q_STATIC_ASSERT_PRIVATE_JOIN(A, B) Q_STATIC_ASSERT_PRIVATE_JOIN_IMPL(A, B)
#define Q_STATIC_ASSERT_PRIVATE_JOIN_IMPL(A, B) A ## B
#ifdef __COUNTER__
#define Q_STATIC_ASSERT(Condition) \
    enum {Q_STATIC_ASSERT_PRIVATE_JOIN(q_static_assert_result, __COUNTER__) = sizeof(QStaticAssertFailure<!!(Condition)>)}
#else
#define Q_STATIC_ASSERT(Condition) \
    enum {Q_STATIC_ASSERT_PRIVATE_JOIN(q_static_assert_result, __LINE__) = sizeof(QStaticAssertFailure<!!(Condition)>)}
#endif /* __COUNTER__ */
#define Q_STATIC_ASSERT_X(Condition, Message) Q_STATIC_ASSERT(Condition)
#endif

Q_NORETURN Q_CORE_EXPORT void qt_check_pointer(const char *, int) Q_DECL_NOTHROW;
Q_CORE_EXPORT void qBadAlloc();

#ifdef QT_NO_EXCEPTIONS
#  if defined(QT_NO_DEBUG) && !defined(QT_FORCE_ASSERTS)
#    define Q_CHECK_PTR(p) qt_noop()
#  else
#    define Q_CHECK_PTR(p) do {if (!(p)) qt_check_pointer(__FILE__,__LINE__);} while (false)
#  endif
#else
#  define Q_CHECK_PTR(p) do { if (!(p)) qBadAlloc(); } while (false)
#endif

template <typename T>
inline T *q_check_ptr(T *p) { Q_CHECK_PTR(p); return p; }

typedef void (*QFunctionPointer)();

#if !defined(Q_UNIMPLEMENTED)
#  define Q_UNIMPLEMENTED() qWarning("Unimplemented code.")
#endif

Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED bool qFuzzyCompare(double p1, double p2)
{
    return (qAbs(p1 - p2) * 1000000000000. <= qMin(qAbs(p1), qAbs(p2)));
}

Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED bool qFuzzyCompare(float p1, float p2)
{
    return (qAbs(p1 - p2) * 100000.f <= qMin(qAbs(p1), qAbs(p2)));
}

Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED bool qFuzzyIsNull(double d)
{
    return qAbs(d) <= 0.000000000001;
}

Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED  bool qFuzzyIsNull(float f)
{
    return qAbs(f) <= 0.00001f;
}

/*
   This function tests a double for a null value. It doesn't
   check whether the actual value is 0 or close to 0, but whether
   it is binary 0, disregarding sign.
*/
Q_REQUIRED_RESULT static inline Q_DECL_UNUSED bool qIsNull(double d)
{
    union U {
        double d;
        quint64 u;
    };
    U val;
    val.d = d;
    return (val.u & Q_UINT64_C(0x7fffffffffffffff)) == 0;
}

/*
   This function tests a float for a null value. It doesn't
   check whether the actual value is 0 or close to 0, but whether
   it is binary 0, disregarding sign.
*/
Q_REQUIRED_RESULT static inline Q_DECL_UNUSED bool qIsNull(float f)
{
    union U {
        float f;
        quint32 u;
    };
    U val;
    val.f = f;
    return (val.u & 0x7fffffff) == 0;
}

/*
   Compilers which follow outdated template instantiation rules
   require a class to have a comparison operator to exist when
   a QList of this type is instantiated. It's not actually
   used in the list, though. Hence the dummy implementation.
   Just in case other code relies on it we better trigger a warning
   mandating a real implementation.
*/

#ifdef Q_FULL_TEMPLATE_INSTANTIATION
#  define Q_DUMMY_COMPARISON_OPERATOR(C) \
    bool operator==(const C&) const { \
        qWarning(#C"::operator==(const "#C"&) was called"); \
        return false; \
    }
#else

#  define Q_DUMMY_COMPARISON_OPERATOR(C)
#endif

namespace QtPrivate
{
namespace SwapExceptionTester { // insulate users from the "using std::swap" below
    using std::swap; // import std::swap
    template <typename T>
    void checkSwap(T &t)
        Q_DECL_NOEXCEPT_EXPR(noexcept(swap(t, t)));
    // declared, but not implemented (only to be used in unevaluated contexts (noexcept operator))
}
} // namespace QtPrivate

template <typename T>
inline void qSwap(T &value1, T &value2)
    Q_DECL_NOEXCEPT_EXPR(noexcept(QtPrivate::SwapExceptionTester::checkSwap(value1)))
{
    using std::swap;
    swap(value1, value2);
}

#if QT_DEPRECATED_SINCE(5, 0)
Q_CORE_EXPORT QT_DEPRECATED void *qMalloc(size_t size) Q_ALLOC_SIZE(1);
Q_CORE_EXPORT QT_DEPRECATED void qFree(void *ptr);
Q_CORE_EXPORT QT_DEPRECATED void *qRealloc(void *ptr, size_t size) Q_ALLOC_SIZE(2);
Q_CORE_EXPORT QT_DEPRECATED void *qMemCopy(void *dest, const void *src, size_t n);
Q_CORE_EXPORT QT_DEPRECATED void *qMemSet(void *dest, int c, size_t n);
#endif
Q_CORE_EXPORT void *qMallocAligned(size_t size, size_t alignment) Q_ALLOC_SIZE(1);
Q_CORE_EXPORT void *qReallocAligned(void *ptr, size_t size, size_t oldsize, size_t alignment) Q_ALLOC_SIZE(2);
Q_CORE_EXPORT void qFreeAligned(void *ptr);


/*
   Avoid some particularly useless warnings from some stupid compilers.
   To get ALL C++ compiler warnings, define QT_CC_WARNINGS or comment out
   the line "#define QT_NO_WARNINGS".
*/
#if !defined(QT_CC_WARNINGS)
#  define QT_NO_WARNINGS
#endif
#if defined(QT_NO_WARNINGS)
#  if defined(Q_CC_MSVC)
QT_WARNING_DISABLE_MSVC(4251) /* class 'type' needs to have dll-interface to be used by clients of class 'type2' */
QT_WARNING_DISABLE_MSVC(4244) /* conversion from 'type1' to 'type2', possible loss of data */
QT_WARNING_DISABLE_MSVC(4275) /* non - DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier' */
QT_WARNING_DISABLE_MSVC(4514) /* unreferenced inline function has been removed */
QT_WARNING_DISABLE_MSVC(4800) /* 'type' : forcing value to bool 'true' or 'false' (performance warning) */
QT_WARNING_DISABLE_MSVC(4097) /* typedef-name 'identifier1' used as synonym for class-name 'identifier2' */
QT_WARNING_DISABLE_MSVC(4706) /* assignment within conditional expression */
#    if _MSC_VER <= 1310 // MSVC 2003
QT_WARNING_DISABLE_MSVC(4786) /* 'identifier' : identifier was truncated to 'number' characters in the debug information */
#    endif
QT_WARNING_DISABLE_MSVC(4355) /* 'this' : used in base member initializer list */
#    if _MSC_VER < 1800 // MSVC 2013
QT_WARNING_DISABLE_MSVC(4231) /* nonstandard extension used : 'identifier' before template explicit instantiation */
#    endif
QT_WARNING_DISABLE_MSVC(4710) /* function not inlined */
QT_WARNING_DISABLE_MSVC(4530) /* C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc */
#  elif defined(Q_CC_BOR)
#    pragma option -w-inl
#    pragma option -w-aus
#    pragma warn -inl
#    pragma warn -pia
#    pragma warn -ccc
#    pragma warn -rch
#    pragma warn -sig
#  endif
#endif

namespace QtPrivate {
template <typename T> struct QAddConst { typedef const T Type; };
}

// this adds const to non-const objects (like std::as_const)
template <typename T>
Q_DECL_CONSTEXPR typename QtPrivate::QAddConst<T>::Type &qAsConst(T &t) Q_DECL_NOTHROW { return t; }
// prevent rvalue arguments:
template <typename T>
void qAsConst(const T &&) Q_DECL_EQ_DELETE;

#ifndef QT_NO_FOREACH

namespace QtPrivate {

template <typename T>
class QForeachContainer {
    Q_DISABLE_COPY(QForeachContainer)
public:
    QForeachContainer(const T &t) : c(t), i(qAsConst(c).begin()), e(qAsConst(c).end()) {}
    QForeachContainer(T &&t) : c(std::move(t)), i(qAsConst(c).begin()), e(qAsConst(c).end())  {}

    QForeachContainer(QForeachContainer &&other)
        : c(std::move(other.c)),
          i(qAsConst(c).begin()),
          e(qAsConst(c).end()),
          control(std::move(other.control))
    {
    }

    QForeachContainer &operator=(QForeachContainer &&other)
    {
        c = std::move(other.c);
        i = qAsConst(c).begin();
        e = qAsConst(c).end();
        control = std::move(other.control);
        return *this;
    }

    T c;
    typename T::const_iterator i, e;
    int control = 1;
};

template<typename T>
QForeachContainer<typename std::decay<T>::type> qMakeForeachContainer(T &&t)
{
    return QForeachContainer<typename std::decay<T>::type>(std::forward<T>(t));
}

}
// Explanation of the control word:
//  - it's initialized to 1
//  - that means both the inner and outer loops start
//  - if there were no breaks, at the end of the inner loop, it's set to 0, which
//    causes it to exit (the inner loop is run exactly once)
//  - at the end of the outer loop, it's inverted, so it becomes 1 again, allowing
//    the outer loop to continue executing
//  - if there was a break inside the inner loop, it will exit with control still
//    set to 1; in that case, the outer loop will invert it to 0 and will exit too
#define Q_FOREACH(variable, container)                                \
for (auto _container_ = QtPrivate::qMakeForeachContainer(container); \
     _container_.control && _container_.i != _container_.e;         \
     ++_container_.i, _container_.control ^= 1)                     \
    for (variable = *_container_.i; _container_.control; _container_.control = 0)

#endif // QT_NO_FOREACH

#define Q_FOREVER for(;;)
#ifndef QT_NO_KEYWORDS
# ifndef QT_NO_FOREACH
#  ifndef foreach
#    define foreach Q_FOREACH
#  endif
# endif // QT_NO_FOREACH
#  ifndef forever
#    define forever Q_FOREVER
#  endif
#endif

template <typename T> static inline T *qGetPtrHelper(T *ptr) { return ptr; }
template <typename Wrapper> static inline typename Wrapper::pointer qGetPtrHelper(const Wrapper &p) { return p.data(); }

#define Q_DECLARE_PRIVATE(Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(qGetPtrHelper(d_ptr)); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(qGetPtrHelper(d_ptr)); } \
    friend class Class##Private;

#define Q_DECLARE_PRIVATE_D(Dptr, Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(qGetPtrHelper(Dptr)); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(qGetPtrHelper(Dptr)); } \
    friend class Class##Private;

#define Q_DECLARE_PUBLIC(Class)                                    \
    inline Class* q_func() { return static_cast<Class *>(q_ptr); } \
    inline const Class* q_func() const { return static_cast<const Class *>(q_ptr); } \
    friend class Class;

#define Q_D(Class) Class##Private * const d = d_func()
#define Q_Q(Class) Class * const q = q_func()

#define QT_TR_NOOP(x) x
#define QT_TR_NOOP_UTF8(x) x
#define QT_TRANSLATE_NOOP(scope, x) x
#define QT_TRANSLATE_NOOP_UTF8(scope, x) x
#define QT_TRANSLATE_NOOP3(scope, x, comment) {x, comment}
#define QT_TRANSLATE_NOOP3_UTF8(scope, x, comment) {x, comment}

#ifndef QT_NO_TRANSLATION // ### This should enclose the NOOPs above

// Defined in qcoreapplication.cpp
// The better name qTrId() is reserved for an upcoming function which would
// return a much more powerful QStringFormatter instead of a QString.
Q_CORE_EXPORT QString qtTrId(const char *id, int n = -1);

#define QT_TRID_NOOP(id) id

#endif // QT_NO_TRANSLATION

/*
   When RTTI is not available, define this macro to force any uses of
   dynamic_cast to cause a compile failure.
*/

#if defined(QT_NO_DYNAMIC_CAST) && !defined(dynamic_cast)
#  define dynamic_cast QT_PREPEND_NAMESPACE(qt_dynamic_cast_check)

  template<typename T, typename X>
  T qt_dynamic_cast_check(X, T* = 0)
  { return T::dynamic_cast_will_always_fail_because_rtti_is_disabled; }
#endif


#ifdef Q_QDOC

// Just for documentation generation
template<typename T>
auto qOverload(T functionPointer);
template<typename T>
auto qConstOverload(T memberFunctionPointer);
template<typename T>
auto qNonConstOverload(T memberFunctionPointer);

#elif defined(Q_COMPILER_VARIADIC_TEMPLATES)

template <typename... Args>
struct QNonConstOverload
{
    template <typename R, typename T>
    Q_DECL_CONSTEXPR auto operator()(R (T::*ptr)(Args...)) const Q_DECL_NOTHROW -> decltype(ptr)
    { return ptr; }

    template <typename R, typename T>
    static Q_DECL_CONSTEXPR auto of(R (T::*ptr)(Args...)) Q_DECL_NOTHROW -> decltype(ptr)
    { return ptr; }
};

template <typename... Args>
struct QConstOverload
{
    template <typename R, typename T>
    Q_DECL_CONSTEXPR auto operator()(R (T::*ptr)(Args...) const) const Q_DECL_NOTHROW -> decltype(ptr)
    { return ptr; }

    template <typename R, typename T>
    static Q_DECL_CONSTEXPR auto of(R (T::*ptr)(Args...) const) Q_DECL_NOTHROW -> decltype(ptr)
    { return ptr; }
};

template <typename... Args>
struct QOverload : QConstOverload<Args...>, QNonConstOverload<Args...>
{
    using QConstOverload<Args...>::of;
    using QConstOverload<Args...>::operator();
    using QNonConstOverload<Args...>::of;
    using QNonConstOverload<Args...>::operator();

    template <typename R>
    Q_DECL_CONSTEXPR auto operator()(R (*ptr)(Args...)) const Q_DECL_NOTHROW -> decltype(ptr)
    { return ptr; }

    template <typename R>
    static Q_DECL_CONSTEXPR auto of(R (*ptr)(Args...)) Q_DECL_NOTHROW -> decltype(ptr)
    { return ptr; }
};

#if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304 // C++14
template <typename... Args> Q_CONSTEXPR Q_DECL_UNUSED QOverload<Args...> qOverload = {};
template <typename... Args> Q_CONSTEXPR Q_DECL_UNUSED QConstOverload<Args...> qConstOverload = {};
template <typename... Args> Q_CONSTEXPR Q_DECL_UNUSED QNonConstOverload<Args...> qNonConstOverload = {};
#endif

#endif


class QByteArray;
Q_CORE_EXPORT QByteArray qgetenv(const char *varName);
Q_CORE_EXPORT bool qputenv(const char *varName, const QByteArray& value);
Q_CORE_EXPORT bool qunsetenv(const char *varName);

Q_CORE_EXPORT bool qEnvironmentVariableIsEmpty(const char *varName) Q_DECL_NOEXCEPT;
Q_CORE_EXPORT bool qEnvironmentVariableIsSet(const char *varName) Q_DECL_NOEXCEPT;
Q_CORE_EXPORT int  qEnvironmentVariableIntValue(const char *varName, bool *ok=Q_NULLPTR) Q_DECL_NOEXCEPT;

inline int qIntCast(double f) { return int(f); }
inline int qIntCast(float f) { return int(f); }

/*
  Reentrant versions of basic rand() functions for random number generation
*/
Q_CORE_EXPORT void qsrand(uint seed);
Q_CORE_EXPORT int qrand();

#define QT_MODULE(x)

#if !defined(QT_BOOTSTRAPPED) && defined(QT_REDUCE_RELOCATIONS) && defined(__ELF__) && \
    (!defined(__PIC__) || (defined(__PIE__) && defined(Q_CC_GNU) && Q_CC_GNU >= 500))
#  error "You must build your code with position independent code if Qt was built with -reduce-relocations. "\
         "Compile your code with -fPIC (-fPIE is not enough)."
#endif

namespace QtPrivate {
//like std::enable_if
template <bool B, typename T = void> struct QEnableIf;
template <typename T> struct QEnableIf<true, T> { typedef T Type; };

template <bool B, typename T, typename F> struct QConditional { typedef T Type; };
template <typename T, typename F> struct QConditional<false, T, F> { typedef F Type; };
}

QT_END_NAMESPACE

// We need to keep QTypeInfo, QSysInfo, QFlags, qDebug & family in qglobal.h for compatibility with Qt 4.
// Be careful when changing the order of these files.
#include <QtCore/qtypeinfo.h>
#include <QtCore/qsysinfo.h>
#include <QtCore/qlogging.h>

#include <QtCore/qflags.h>

#include <QtCore/qatomic.h>
#include <QtCore/qglobalstatic.h>
#include <QtCore/qnumeric.h>
#include <QtCore/qversiontagging.h>

#endif /* __cplusplus */

#endif /* QGLOBAL_H */
