/****************************************************************************
**
** Copyright (C) 2011 Thiago Macieira <thiago@kde.org>
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

#include <QtCore/qatomic.h>

#ifndef QBASICATOMIC_H
#define QBASICATOMIC_H

#if defined(QT_BOOTSTRAPPED)
#  include <QtCore/qatomic_bootstrap.h>

// If C++11 atomics are supported, use them!
// Note that constexpr support is sometimes disabled in QNX builds but its
// library has <atomic>.
#elif defined(Q_COMPILER_ATOMICS) && (defined(Q_COMPILER_CONSTEXPR) || defined(Q_OS_QNX))
#  include <QtCore/qatomic_cxx11.h>

// We only support one fallback: MSVC, because even on version 2015, it lacks full constexpr support
#elif defined(Q_CC_MSVC)
#  include <QtCore/qatomic_msvc.h>

// No fallback
#else
#  error "Qt requires C++11 support"
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_MSVC(4522)

QT_BEGIN_NAMESPACE

#if 0
// silence syncqt warnings
QT_END_NAMESPACE
#pragma qt_no_master_include
#pragma qt_sync_stop_processing
#endif

// New atomics

#if defined(Q_COMPILER_CONSTEXPR) && defined(Q_COMPILER_DEFAULT_MEMBERS) && defined(Q_COMPILER_DELETE_MEMBERS)
# if defined(Q_CC_CLANG) && Q_CC_CLANG < 303
   /*
      Do not define QT_BASIC_ATOMIC_HAS_CONSTRUCTORS for Clang before version 3.3.
      For details about the bug: see http://llvm.org/bugs/show_bug.cgi?id=12670
    */
# else
#  define QT_BASIC_ATOMIC_HAS_CONSTRUCTORS
# endif
#endif

template <typename T>
class QBasicAtomicInteger
{
public:
    typedef QAtomicOps<T> Ops;
    // static check that this is a valid integer
    Q_STATIC_ASSERT_X(QTypeInfo<T>::isIntegral, "template parameter is not an integral type");
    Q_STATIC_ASSERT_X(QAtomicOpsSupport<sizeof(T)>::IsSupported, "template parameter is an integral of a size not supported on this platform");

    typename Ops::Type _q_value;

    // Everything below is either implemented in ../arch/qatomic_XXX.h or (as fallback) in qgenericatomic.h

    T load() const Q_DECL_NOTHROW { return Ops::load(_q_value); }
    void store(T newValue) Q_DECL_NOTHROW { Ops::store(_q_value, newValue); }

    T loadAcquire() const Q_DECL_NOTHROW { return Ops::loadAcquire(_q_value); }
    void storeRelease(T newValue) Q_DECL_NOTHROW { Ops::storeRelease(_q_value, newValue); }
    operator T() const Q_DECL_NOTHROW { return loadAcquire(); }
    T operator=(T newValue) Q_DECL_NOTHROW { storeRelease(newValue); return newValue; }

    static Q_DECL_CONSTEXPR bool isReferenceCountingNative() Q_DECL_NOTHROW { return Ops::isReferenceCountingNative(); }
    static Q_DECL_CONSTEXPR bool isReferenceCountingWaitFree() Q_DECL_NOTHROW { return Ops::isReferenceCountingWaitFree(); }

    bool ref() Q_DECL_NOTHROW { return Ops::ref(_q_value); }
    bool deref() Q_DECL_NOTHROW { return Ops::deref(_q_value); }

    static Q_DECL_CONSTEXPR bool isTestAndSetNative() Q_DECL_NOTHROW { return Ops::isTestAndSetNative(); }
    static Q_DECL_CONSTEXPR bool isTestAndSetWaitFree() Q_DECL_NOTHROW { return Ops::isTestAndSetWaitFree(); }

    bool testAndSetRelaxed(T expectedValue, T newValue) Q_DECL_NOTHROW
    { return Ops::testAndSetRelaxed(_q_value, expectedValue, newValue); }
    bool testAndSetAcquire(T expectedValue, T newValue) Q_DECL_NOTHROW
    { return Ops::testAndSetAcquire(_q_value, expectedValue, newValue); }
    bool testAndSetRelease(T expectedValue, T newValue) Q_DECL_NOTHROW
    { return Ops::testAndSetRelease(_q_value, expectedValue, newValue); }
    bool testAndSetOrdered(T expectedValue, T newValue) Q_DECL_NOTHROW
    { return Ops::testAndSetOrdered(_q_value, expectedValue, newValue); }

    bool testAndSetRelaxed(T expectedValue, T newValue, T &currentValue) Q_DECL_NOTHROW
    { return Ops::testAndSetRelaxed(_q_value, expectedValue, newValue, &currentValue); }
    bool testAndSetAcquire(T expectedValue, T newValue, T &currentValue) Q_DECL_NOTHROW
    { return Ops::testAndSetAcquire(_q_value, expectedValue, newValue, &currentValue); }
    bool testAndSetRelease(T expectedValue, T newValue, T &currentValue) Q_DECL_NOTHROW
    { return Ops::testAndSetRelease(_q_value, expectedValue, newValue, &currentValue); }
    bool testAndSetOrdered(T expectedValue, T newValue, T &currentValue) Q_DECL_NOTHROW
    { return Ops::testAndSetOrdered(_q_value, expectedValue, newValue, &currentValue); }

    static Q_DECL_CONSTEXPR bool isFetchAndStoreNative() Q_DECL_NOTHROW { return Ops::isFetchAndStoreNative(); }
    static Q_DECL_CONSTEXPR bool isFetchAndStoreWaitFree() Q_DECL_NOTHROW { return Ops::isFetchAndStoreWaitFree(); }

    T fetchAndStoreRelaxed(T newValue) Q_DECL_NOTHROW
    { return Ops::fetchAndStoreRelaxed(_q_value, newValue); }
    T fetchAndStoreAcquire(T newValue) Q_DECL_NOTHROW
    { return Ops::fetchAndStoreAcquire(_q_value, newValue); }
    T fetchAndStoreRelease(T newValue) Q_DECL_NOTHROW
    { return Ops::fetchAndStoreRelease(_q_value, newValue); }
    T fetchAndStoreOrdered(T newValue) Q_DECL_NOTHROW
    { return Ops::fetchAndStoreOrdered(_q_value, newValue); }

    static Q_DECL_CONSTEXPR bool isFetchAndAddNative() Q_DECL_NOTHROW { return Ops::isFetchAndAddNative(); }
    static Q_DECL_CONSTEXPR bool isFetchAndAddWaitFree() Q_DECL_NOTHROW { return Ops::isFetchAndAddWaitFree(); }

    T fetchAndAddRelaxed(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndAddRelaxed(_q_value, valueToAdd); }
    T fetchAndAddAcquire(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndAddAcquire(_q_value, valueToAdd); }
    T fetchAndAddRelease(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndAddRelease(_q_value, valueToAdd); }
    T fetchAndAddOrdered(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndAddOrdered(_q_value, valueToAdd); }

    T fetchAndSubRelaxed(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndSubRelaxed(_q_value, valueToAdd); }
    T fetchAndSubAcquire(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndSubAcquire(_q_value, valueToAdd); }
    T fetchAndSubRelease(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndSubRelease(_q_value, valueToAdd); }
    T fetchAndSubOrdered(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndSubOrdered(_q_value, valueToAdd); }

    T fetchAndAndRelaxed(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndAndRelaxed(_q_value, valueToAdd); }
    T fetchAndAndAcquire(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndAndAcquire(_q_value, valueToAdd); }
    T fetchAndAndRelease(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndAndRelease(_q_value, valueToAdd); }
    T fetchAndAndOrdered(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndAndOrdered(_q_value, valueToAdd); }

    T fetchAndOrRelaxed(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndOrRelaxed(_q_value, valueToAdd); }
    T fetchAndOrAcquire(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndOrAcquire(_q_value, valueToAdd); }
    T fetchAndOrRelease(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndOrRelease(_q_value, valueToAdd); }
    T fetchAndOrOrdered(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndOrOrdered(_q_value, valueToAdd); }

    T fetchAndXorRelaxed(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndXorRelaxed(_q_value, valueToAdd); }
    T fetchAndXorAcquire(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndXorAcquire(_q_value, valueToAdd); }
    T fetchAndXorRelease(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndXorRelease(_q_value, valueToAdd); }
    T fetchAndXorOrdered(T valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndXorOrdered(_q_value, valueToAdd); }

    T operator++() Q_DECL_NOTHROW
    { return fetchAndAddOrdered(1) + 1; }
    T operator++(int) Q_DECL_NOTHROW
    { return fetchAndAddOrdered(1); }
    T operator--() Q_DECL_NOTHROW
    { return fetchAndSubOrdered(1) - 1; }
    T operator--(int) Q_DECL_NOTHROW
    { return fetchAndSubOrdered(1); }

    T operator+=(T v) Q_DECL_NOTHROW
    { return fetchAndAddOrdered(v) + v; }
    T operator-=(T v) Q_DECL_NOTHROW
    { return fetchAndSubOrdered(v) - v; }
    T operator&=(T v) Q_DECL_NOTHROW
    { return fetchAndAndOrdered(v) & v; }
    T operator|=(T v) Q_DECL_NOTHROW
    { return fetchAndOrOrdered(v) | v; }
    T operator^=(T v) Q_DECL_NOTHROW
    { return fetchAndXorOrdered(v) ^ v; }


#ifdef QT_BASIC_ATOMIC_HAS_CONSTRUCTORS
    QBasicAtomicInteger() = default;
    constexpr QBasicAtomicInteger(T value) Q_DECL_NOTHROW : _q_value(value) {}
    QBasicAtomicInteger(const QBasicAtomicInteger &) = delete;
    QBasicAtomicInteger &operator=(const QBasicAtomicInteger &) = delete;
    QBasicAtomicInteger &operator=(const QBasicAtomicInteger &) volatile = delete;
#endif
};
typedef QBasicAtomicInteger<int> QBasicAtomicInt;

template <typename X>
class QBasicAtomicPointer
{
public:
    typedef X *Type;
    typedef QAtomicOps<Type> Ops;
    typedef typename Ops::Type AtomicType;

    AtomicType _q_value;

    Type load() const Q_DECL_NOTHROW { return Ops::load(_q_value); }
    void store(Type newValue) Q_DECL_NOTHROW { Ops::store(_q_value, newValue); }
    operator Type() const Q_DECL_NOTHROW { return loadAcquire(); }
    Type operator=(Type newValue) Q_DECL_NOTHROW { storeRelease(newValue); return newValue; }

    // Atomic API, implemented in qatomic_XXX.h
    Type loadAcquire() const Q_DECL_NOTHROW { return Ops::loadAcquire(_q_value); }
    void storeRelease(Type newValue) Q_DECL_NOTHROW { Ops::storeRelease(_q_value, newValue); }

    static Q_DECL_CONSTEXPR bool isTestAndSetNative() Q_DECL_NOTHROW { return Ops::isTestAndSetNative(); }
    static Q_DECL_CONSTEXPR bool isTestAndSetWaitFree() Q_DECL_NOTHROW { return Ops::isTestAndSetWaitFree(); }

    bool testAndSetRelaxed(Type expectedValue, Type newValue) Q_DECL_NOTHROW
    { return Ops::testAndSetRelaxed(_q_value, expectedValue, newValue); }
    bool testAndSetAcquire(Type expectedValue, Type newValue) Q_DECL_NOTHROW
    { return Ops::testAndSetAcquire(_q_value, expectedValue, newValue); }
    bool testAndSetRelease(Type expectedValue, Type newValue) Q_DECL_NOTHROW
    { return Ops::testAndSetRelease(_q_value, expectedValue, newValue); }
    bool testAndSetOrdered(Type expectedValue, Type newValue) Q_DECL_NOTHROW
    { return Ops::testAndSetOrdered(_q_value, expectedValue, newValue); }

    bool testAndSetRelaxed(Type expectedValue, Type newValue, Type &currentValue) Q_DECL_NOTHROW
    { return Ops::testAndSetRelaxed(_q_value, expectedValue, newValue, &currentValue); }
    bool testAndSetAcquire(Type expectedValue, Type newValue, Type &currentValue) Q_DECL_NOTHROW
    { return Ops::testAndSetAcquire(_q_value, expectedValue, newValue, &currentValue); }
    bool testAndSetRelease(Type expectedValue, Type newValue, Type &currentValue) Q_DECL_NOTHROW
    { return Ops::testAndSetRelease(_q_value, expectedValue, newValue, &currentValue); }
    bool testAndSetOrdered(Type expectedValue, Type newValue, Type &currentValue) Q_DECL_NOTHROW
    { return Ops::testAndSetOrdered(_q_value, expectedValue, newValue, &currentValue); }

    static Q_DECL_CONSTEXPR bool isFetchAndStoreNative() Q_DECL_NOTHROW { return Ops::isFetchAndStoreNative(); }
    static Q_DECL_CONSTEXPR bool isFetchAndStoreWaitFree() Q_DECL_NOTHROW { return Ops::isFetchAndStoreWaitFree(); }

    Type fetchAndStoreRelaxed(Type newValue) Q_DECL_NOTHROW
    { return Ops::fetchAndStoreRelaxed(_q_value, newValue); }
    Type fetchAndStoreAcquire(Type newValue) Q_DECL_NOTHROW
    { return Ops::fetchAndStoreAcquire(_q_value, newValue); }
    Type fetchAndStoreRelease(Type newValue) Q_DECL_NOTHROW
    { return Ops::fetchAndStoreRelease(_q_value, newValue); }
    Type fetchAndStoreOrdered(Type newValue) Q_DECL_NOTHROW
    { return Ops::fetchAndStoreOrdered(_q_value, newValue); }

    static Q_DECL_CONSTEXPR bool isFetchAndAddNative() Q_DECL_NOTHROW { return Ops::isFetchAndAddNative(); }
    static Q_DECL_CONSTEXPR bool isFetchAndAddWaitFree() Q_DECL_NOTHROW { return Ops::isFetchAndAddWaitFree(); }

    Type fetchAndAddRelaxed(qptrdiff valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndAddRelaxed(_q_value, valueToAdd); }
    Type fetchAndAddAcquire(qptrdiff valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndAddAcquire(_q_value, valueToAdd); }
    Type fetchAndAddRelease(qptrdiff valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndAddRelease(_q_value, valueToAdd); }
    Type fetchAndAddOrdered(qptrdiff valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndAddOrdered(_q_value, valueToAdd); }

    Type fetchAndSubRelaxed(qptrdiff valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndSubRelaxed(_q_value, valueToAdd); }
    Type fetchAndSubAcquire(qptrdiff valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndSubAcquire(_q_value, valueToAdd); }
    Type fetchAndSubRelease(qptrdiff valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndSubRelease(_q_value, valueToAdd); }
    Type fetchAndSubOrdered(qptrdiff valueToAdd) Q_DECL_NOTHROW
    { return Ops::fetchAndSubOrdered(_q_value, valueToAdd); }

    Type operator++() Q_DECL_NOTHROW
    { return fetchAndAddOrdered(1) + 1; }
    Type operator++(int) Q_DECL_NOTHROW
    { return fetchAndAddOrdered(1); }
    Type operator--() Q_DECL_NOTHROW
    { return fetchAndSubOrdered(1) - 1; }
    Type operator--(int) Q_DECL_NOTHROW
    { return fetchAndSubOrdered(1); }
    Type operator+=(qptrdiff valueToAdd) Q_DECL_NOTHROW
    { return fetchAndAddOrdered(valueToAdd) + valueToAdd; }
    Type operator-=(qptrdiff valueToSub) Q_DECL_NOTHROW
    { return fetchAndSubOrdered(valueToSub) - valueToSub; }

#ifdef QT_BASIC_ATOMIC_HAS_CONSTRUCTORS
    QBasicAtomicPointer() = default;
    constexpr QBasicAtomicPointer(Type value) Q_DECL_NOTHROW : _q_value(value) {}
    QBasicAtomicPointer(const QBasicAtomicPointer &) = delete;
    QBasicAtomicPointer &operator=(const QBasicAtomicPointer &) = delete;
    QBasicAtomicPointer &operator=(const QBasicAtomicPointer &) volatile = delete;
#endif
};

#ifndef Q_BASIC_ATOMIC_INITIALIZER
#  define Q_BASIC_ATOMIC_INITIALIZER(a) { (a) }
#endif

QT_END_NAMESPACE

QT_WARNING_POP

#endif // QBASICATOMIC_H
