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

#ifndef QMARGINS_H
#define QMARGINS_H

#include <QtCore/qnamespace.h>

QT_BEGIN_NAMESPACE

/*****************************************************************************
  QMargins class
 *****************************************************************************/

class QMargins
{
public:
    Q_DECL_CONSTEXPR QMargins() Q_DECL_NOTHROW;
    Q_DECL_CONSTEXPR QMargins(int left, int top, int right, int bottom) Q_DECL_NOTHROW;

    Q_DECL_CONSTEXPR bool isNull() const Q_DECL_NOTHROW;

    Q_DECL_CONSTEXPR int left() const Q_DECL_NOTHROW;
    Q_DECL_CONSTEXPR int top() const Q_DECL_NOTHROW;
    Q_DECL_CONSTEXPR int right() const Q_DECL_NOTHROW;
    Q_DECL_CONSTEXPR int bottom() const Q_DECL_NOTHROW;

    Q_DECL_RELAXED_CONSTEXPR void setLeft(int left) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR void setTop(int top) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR void setRight(int right) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR void setBottom(int bottom) Q_DECL_NOTHROW;

    Q_DECL_RELAXED_CONSTEXPR QMargins &operator+=(const QMargins &margins) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR QMargins &operator-=(const QMargins &margins) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR QMargins &operator+=(int) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR QMargins &operator-=(int) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR QMargins &operator*=(int) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR QMargins &operator/=(int);
    Q_DECL_RELAXED_CONSTEXPR QMargins &operator*=(qreal) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR QMargins &operator/=(qreal);

private:
    int m_left;
    int m_top;
    int m_right;
    int m_bottom;

    friend Q_DECL_CONSTEXPR inline bool operator==(const QMargins &, const QMargins &) Q_DECL_NOTHROW;
    friend Q_DECL_CONSTEXPR inline bool operator!=(const QMargins &, const QMargins &) Q_DECL_NOTHROW;
};

Q_DECLARE_TYPEINFO(QMargins, Q_MOVABLE_TYPE);

/*****************************************************************************
  QMargins stream functions
 *****************************************************************************/
#ifndef QT_NO_DATASTREAM
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QMargins &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QMargins &);
#endif

/*****************************************************************************
  QMargins inline functions
 *****************************************************************************/

Q_DECL_CONSTEXPR inline QMargins::QMargins() Q_DECL_NOTHROW : m_left(0), m_top(0), m_right(0), m_bottom(0) {}

Q_DECL_CONSTEXPR inline QMargins::QMargins(int aleft, int atop, int aright, int abottom) Q_DECL_NOTHROW
    : m_left(aleft), m_top(atop), m_right(aright), m_bottom(abottom) {}

Q_DECL_CONSTEXPR inline bool QMargins::isNull() const Q_DECL_NOTHROW
{ return m_left==0 && m_top==0 && m_right==0 && m_bottom==0; }

Q_DECL_CONSTEXPR inline int QMargins::left() const Q_DECL_NOTHROW
{ return m_left; }

Q_DECL_CONSTEXPR inline int QMargins::top() const Q_DECL_NOTHROW
{ return m_top; }

Q_DECL_CONSTEXPR inline int QMargins::right() const Q_DECL_NOTHROW
{ return m_right; }

Q_DECL_CONSTEXPR inline int QMargins::bottom() const Q_DECL_NOTHROW
{ return m_bottom; }


Q_DECL_RELAXED_CONSTEXPR inline void QMargins::setLeft(int aleft) Q_DECL_NOTHROW
{ m_left = aleft; }

Q_DECL_RELAXED_CONSTEXPR inline void QMargins::setTop(int atop) Q_DECL_NOTHROW
{ m_top = atop; }

Q_DECL_RELAXED_CONSTEXPR inline void QMargins::setRight(int aright) Q_DECL_NOTHROW
{ m_right = aright; }

Q_DECL_RELAXED_CONSTEXPR inline void QMargins::setBottom(int abottom) Q_DECL_NOTHROW
{ m_bottom = abottom; }

Q_DECL_CONSTEXPR inline bool operator==(const QMargins &m1, const QMargins &m2) Q_DECL_NOTHROW
{
    return
            m1.m_left == m2.m_left &&
            m1.m_top == m2.m_top &&
            m1.m_right == m2.m_right &&
            m1.m_bottom == m2.m_bottom;
}

Q_DECL_CONSTEXPR inline bool operator!=(const QMargins &m1, const QMargins &m2) Q_DECL_NOTHROW
{
    return
            m1.m_left != m2.m_left ||
            m1.m_top != m2.m_top ||
            m1.m_right != m2.m_right ||
            m1.m_bottom != m2.m_bottom;
}

Q_DECL_CONSTEXPR inline QMargins operator+(const QMargins &m1, const QMargins &m2) Q_DECL_NOTHROW
{
    return QMargins(m1.left() + m2.left(), m1.top() + m2.top(),
                    m1.right() + m2.right(), m1.bottom() + m2.bottom());
}

Q_DECL_CONSTEXPR inline QMargins operator-(const QMargins &m1, const QMargins &m2) Q_DECL_NOTHROW
{
    return QMargins(m1.left() - m2.left(), m1.top() - m2.top(),
                    m1.right() - m2.right(), m1.bottom() - m2.bottom());
}

Q_DECL_CONSTEXPR inline QMargins operator+(const QMargins &lhs, int rhs) Q_DECL_NOTHROW
{
    return QMargins(lhs.left() + rhs, lhs.top() + rhs,
                    lhs.right() + rhs, lhs.bottom() + rhs);
}

Q_DECL_CONSTEXPR inline QMargins operator+(int lhs, const QMargins &rhs) Q_DECL_NOTHROW
{
    return QMargins(rhs.left() + lhs, rhs.top() + lhs,
                    rhs.right() + lhs, rhs.bottom() + lhs);
}

Q_DECL_CONSTEXPR inline QMargins operator-(const QMargins &lhs, int rhs) Q_DECL_NOTHROW
{
    return QMargins(lhs.left() - rhs, lhs.top() - rhs,
                    lhs.right() - rhs, lhs.bottom() - rhs);
}

Q_DECL_CONSTEXPR inline QMargins operator*(const QMargins &margins, int factor) Q_DECL_NOTHROW
{
    return QMargins(margins.left() * factor, margins.top() * factor,
                    margins.right() * factor, margins.bottom() * factor);
}

Q_DECL_CONSTEXPR inline QMargins operator*(int factor, const QMargins &margins) Q_DECL_NOTHROW
{
    return QMargins(margins.left() * factor, margins.top() * factor,
                    margins.right() * factor, margins.bottom() * factor);
}

Q_DECL_CONSTEXPR inline QMargins operator*(const QMargins &margins, qreal factor) Q_DECL_NOTHROW
{
    return QMargins(qRound(margins.left() * factor), qRound(margins.top() * factor),
                    qRound(margins.right() * factor), qRound(margins.bottom() * factor));
}

Q_DECL_CONSTEXPR inline QMargins operator*(qreal factor, const QMargins &margins) Q_DECL_NOTHROW
{
    return QMargins(qRound(margins.left() * factor), qRound(margins.top() * factor),
                    qRound(margins.right() * factor), qRound(margins.bottom() * factor));
}

Q_DECL_CONSTEXPR inline QMargins operator/(const QMargins &margins, int divisor)
{
    return QMargins(margins.left() / divisor, margins.top() / divisor,
                    margins.right() / divisor, margins.bottom() / divisor);
}

Q_DECL_CONSTEXPR inline QMargins operator/(const QMargins &margins, qreal divisor)
{
    return QMargins(qRound(margins.left() / divisor), qRound(margins.top() / divisor),
                    qRound(margins.right() / divisor), qRound(margins.bottom() / divisor));
}

Q_DECL_RELAXED_CONSTEXPR inline QMargins &QMargins::operator+=(const QMargins &margins) Q_DECL_NOTHROW
{
    return *this = *this + margins;
}

Q_DECL_RELAXED_CONSTEXPR inline QMargins &QMargins::operator-=(const QMargins &margins) Q_DECL_NOTHROW
{
    return *this = *this - margins;
}

Q_DECL_RELAXED_CONSTEXPR inline QMargins &QMargins::operator+=(int margin) Q_DECL_NOTHROW
{
    m_left += margin;
    m_top += margin;
    m_right += margin;
    m_bottom += margin;
    return *this;
}

Q_DECL_RELAXED_CONSTEXPR inline QMargins &QMargins::operator-=(int margin) Q_DECL_NOTHROW
{
    m_left -= margin;
    m_top -= margin;
    m_right -= margin;
    m_bottom -= margin;
    return *this;
}

Q_DECL_RELAXED_CONSTEXPR inline QMargins &QMargins::operator*=(int factor) Q_DECL_NOTHROW
{
    return *this = *this * factor;
}

Q_DECL_RELAXED_CONSTEXPR inline QMargins &QMargins::operator/=(int divisor)
{
    return *this = *this / divisor;
}

Q_DECL_RELAXED_CONSTEXPR inline QMargins &QMargins::operator*=(qreal factor) Q_DECL_NOTHROW
{
    return *this = *this * factor;
}

Q_DECL_RELAXED_CONSTEXPR inline QMargins &QMargins::operator/=(qreal divisor)
{
    return *this = *this / divisor;
}

Q_DECL_CONSTEXPR inline QMargins operator+(const QMargins &margins) Q_DECL_NOTHROW
{
    return margins;
}

Q_DECL_CONSTEXPR inline QMargins operator-(const QMargins &margins) Q_DECL_NOTHROW
{
    return QMargins(-margins.left(), -margins.top(), -margins.right(), -margins.bottom());
}

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug, const QMargins &);
#endif

/*****************************************************************************
  QMarginsF class
 *****************************************************************************/

class QMarginsF
{
public:
    Q_DECL_CONSTEXPR QMarginsF() Q_DECL_NOTHROW;
    Q_DECL_CONSTEXPR QMarginsF(qreal left, qreal top, qreal right, qreal bottom) Q_DECL_NOTHROW;
    Q_DECL_CONSTEXPR QMarginsF(const QMargins &margins) Q_DECL_NOTHROW;

    Q_DECL_CONSTEXPR bool isNull() const Q_DECL_NOTHROW;

    Q_DECL_CONSTEXPR qreal left() const Q_DECL_NOTHROW;
    Q_DECL_CONSTEXPR qreal top() const Q_DECL_NOTHROW;
    Q_DECL_CONSTEXPR qreal right() const Q_DECL_NOTHROW;
    Q_DECL_CONSTEXPR qreal bottom() const Q_DECL_NOTHROW;

    Q_DECL_RELAXED_CONSTEXPR void setLeft(qreal left) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR void setTop(qreal top) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR void setRight(qreal right) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR void setBottom(qreal bottom) Q_DECL_NOTHROW;

    Q_DECL_RELAXED_CONSTEXPR QMarginsF &operator+=(const QMarginsF &margins) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR QMarginsF &operator-=(const QMarginsF &margins) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR QMarginsF &operator+=(qreal addend) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR QMarginsF &operator-=(qreal subtrahend) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR QMarginsF &operator*=(qreal factor) Q_DECL_NOTHROW;
    Q_DECL_RELAXED_CONSTEXPR QMarginsF &operator/=(qreal divisor);

    Q_DECL_CONSTEXPR inline QMargins toMargins() const Q_DECL_NOTHROW;

private:
    qreal m_left;
    qreal m_top;
    qreal m_right;
    qreal m_bottom;
};

Q_DECLARE_TYPEINFO(QMarginsF, Q_MOVABLE_TYPE);

/*****************************************************************************
  QMarginsF stream functions
 *****************************************************************************/

#ifndef QT_NO_DATASTREAM
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QMarginsF &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QMarginsF &);
#endif

/*****************************************************************************
  QMarginsF inline functions
 *****************************************************************************/

Q_DECL_CONSTEXPR inline QMarginsF::QMarginsF() Q_DECL_NOTHROW
    : m_left(0), m_top(0), m_right(0), m_bottom(0) {}

Q_DECL_CONSTEXPR inline QMarginsF::QMarginsF(qreal aleft, qreal atop, qreal aright, qreal abottom) Q_DECL_NOTHROW
    : m_left(aleft), m_top(atop), m_right(aright), m_bottom(abottom) {}

Q_DECL_CONSTEXPR inline QMarginsF::QMarginsF(const QMargins &margins) Q_DECL_NOTHROW
    : m_left(margins.left()), m_top(margins.top()), m_right(margins.right()), m_bottom(margins.bottom()) {}

Q_DECL_CONSTEXPR inline bool QMarginsF::isNull() const Q_DECL_NOTHROW
{ return qFuzzyIsNull(m_left) && qFuzzyIsNull(m_top) && qFuzzyIsNull(m_right) && qFuzzyIsNull(m_bottom); }

Q_DECL_CONSTEXPR inline qreal QMarginsF::left() const Q_DECL_NOTHROW
{ return m_left; }

Q_DECL_CONSTEXPR inline qreal QMarginsF::top() const Q_DECL_NOTHROW
{ return m_top; }

Q_DECL_CONSTEXPR inline qreal QMarginsF::right() const Q_DECL_NOTHROW
{ return m_right; }

Q_DECL_CONSTEXPR inline qreal QMarginsF::bottom() const Q_DECL_NOTHROW
{ return m_bottom; }


Q_DECL_RELAXED_CONSTEXPR inline void QMarginsF::setLeft(qreal aleft) Q_DECL_NOTHROW
{ m_left = aleft; }

Q_DECL_RELAXED_CONSTEXPR inline void QMarginsF::setTop(qreal atop) Q_DECL_NOTHROW
{ m_top = atop; }

Q_DECL_RELAXED_CONSTEXPR inline void QMarginsF::setRight(qreal aright) Q_DECL_NOTHROW
{ m_right = aright; }

Q_DECL_RELAXED_CONSTEXPR inline void QMarginsF::setBottom(qreal abottom) Q_DECL_NOTHROW
{ m_bottom = abottom; }

Q_DECL_CONSTEXPR inline bool operator==(const QMarginsF &lhs, const QMarginsF &rhs) Q_DECL_NOTHROW
{
    return qFuzzyCompare(lhs.left(), rhs.left())
           && qFuzzyCompare(lhs.top(), rhs.top())
           && qFuzzyCompare(lhs.right(), rhs.right())
           && qFuzzyCompare(lhs.bottom(), rhs.bottom());
}

Q_DECL_CONSTEXPR inline bool operator!=(const QMarginsF &lhs, const QMarginsF &rhs) Q_DECL_NOTHROW
{
    return !operator==(lhs, rhs);
}

Q_DECL_CONSTEXPR inline QMarginsF operator+(const QMarginsF &lhs, const QMarginsF &rhs) Q_DECL_NOTHROW
{
    return QMarginsF(lhs.left() + rhs.left(), lhs.top() + rhs.top(),
                     lhs.right() + rhs.right(), lhs.bottom() + rhs.bottom());
}

Q_DECL_CONSTEXPR inline QMarginsF operator-(const QMarginsF &lhs, const QMarginsF &rhs) Q_DECL_NOTHROW
{
    return QMarginsF(lhs.left() - rhs.left(), lhs.top() - rhs.top(),
                     lhs.right() - rhs.right(), lhs.bottom() - rhs.bottom());
}

Q_DECL_CONSTEXPR inline QMarginsF operator+(const QMarginsF &lhs, qreal rhs) Q_DECL_NOTHROW
{
    return QMarginsF(lhs.left() + rhs, lhs.top() + rhs,
                     lhs.right() + rhs, lhs.bottom() + rhs);
}

Q_DECL_CONSTEXPR inline QMarginsF operator+(qreal lhs, const QMarginsF &rhs) Q_DECL_NOTHROW
{
    return QMarginsF(rhs.left() + lhs, rhs.top() + lhs,
                     rhs.right() + lhs, rhs.bottom() + lhs);
}

Q_DECL_CONSTEXPR inline QMarginsF operator-(const QMarginsF &lhs, qreal rhs) Q_DECL_NOTHROW
{
    return QMarginsF(lhs.left() - rhs, lhs.top() - rhs,
                     lhs.right() - rhs, lhs.bottom() - rhs);
}

Q_DECL_CONSTEXPR inline QMarginsF operator*(const QMarginsF &lhs, qreal rhs) Q_DECL_NOTHROW
{
    return QMarginsF(lhs.left() * rhs, lhs.top() * rhs,
                     lhs.right() * rhs, lhs.bottom() * rhs);
}

Q_DECL_CONSTEXPR inline QMarginsF operator*(qreal lhs, const QMarginsF &rhs) Q_DECL_NOTHROW
{
    return QMarginsF(rhs.left() * lhs, rhs.top() * lhs,
                     rhs.right() * lhs, rhs.bottom() * lhs);
}

Q_DECL_CONSTEXPR inline QMarginsF operator/(const QMarginsF &lhs, qreal divisor)
{
    return QMarginsF(lhs.left() / divisor, lhs.top() / divisor,
                     lhs.right() / divisor, lhs.bottom() / divisor);
}

Q_DECL_RELAXED_CONSTEXPR inline QMarginsF &QMarginsF::operator+=(const QMarginsF &margins) Q_DECL_NOTHROW
{
    return *this = *this + margins;
}

Q_DECL_RELAXED_CONSTEXPR inline QMarginsF &QMarginsF::operator-=(const QMarginsF &margins) Q_DECL_NOTHROW
{
    return *this = *this - margins;
}

Q_DECL_RELAXED_CONSTEXPR inline QMarginsF &QMarginsF::operator+=(qreal addend) Q_DECL_NOTHROW
{
    m_left += addend;
    m_top += addend;
    m_right += addend;
    m_bottom += addend;
    return *this;
}

Q_DECL_RELAXED_CONSTEXPR inline QMarginsF &QMarginsF::operator-=(qreal subtrahend) Q_DECL_NOTHROW
{
    m_left -= subtrahend;
    m_top -= subtrahend;
    m_right -= subtrahend;
    m_bottom -= subtrahend;
    return *this;
}

Q_DECL_RELAXED_CONSTEXPR inline QMarginsF &QMarginsF::operator*=(qreal factor) Q_DECL_NOTHROW
{
    return *this = *this * factor;
}

Q_DECL_RELAXED_CONSTEXPR inline QMarginsF &QMarginsF::operator/=(qreal divisor)
{
    return *this = *this / divisor;
}

Q_DECL_CONSTEXPR inline QMarginsF operator+(const QMarginsF &margins) Q_DECL_NOTHROW
{
    return margins;
}

Q_DECL_CONSTEXPR inline QMarginsF operator-(const QMarginsF &margins) Q_DECL_NOTHROW
{
    return QMarginsF(-margins.left(), -margins.top(), -margins.right(), -margins.bottom());
}

Q_DECL_CONSTEXPR inline QMargins QMarginsF::toMargins() const Q_DECL_NOTHROW
{
    return QMargins(qRound(m_left), qRound(m_top), qRound(m_right), qRound(m_bottom));
}

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug, const QMarginsF &);
#endif

QT_END_NAMESPACE

#endif // QMARGINS_H
