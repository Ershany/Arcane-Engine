/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
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

#ifndef QSPLITTER_H
#define QSPLITTER_H

#include <QtWidgets/qtwidgetsglobal.h>
#include <QtWidgets/qframe.h>
#include <QtWidgets/qsizepolicy.h>

QT_REQUIRE_CONFIG(splitter);

QT_BEGIN_NAMESPACE

class QSplitterPrivate;
class QTextStream;
template <typename T> class QList;

class QSplitterHandle;

class Q_WIDGETS_EXPORT QSplitter : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(bool opaqueResize READ opaqueResize WRITE setOpaqueResize)
    Q_PROPERTY(int handleWidth READ handleWidth WRITE setHandleWidth)
    Q_PROPERTY(bool childrenCollapsible READ childrenCollapsible WRITE setChildrenCollapsible)

public:
    explicit QSplitter(QWidget* parent = Q_NULLPTR);
    explicit QSplitter(Qt::Orientation, QWidget* parent = Q_NULLPTR);
    ~QSplitter();

    void addWidget(QWidget *widget);
    void insertWidget(int index, QWidget *widget);
    QWidget *replaceWidget(int index, QWidget *widget);

    void setOrientation(Qt::Orientation);
    Qt::Orientation orientation() const;

    void setChildrenCollapsible(bool);
    bool childrenCollapsible() const;

    void setCollapsible(int index, bool);
    bool isCollapsible(int index) const;
    void setOpaqueResize(bool opaque = true);
    bool opaqueResize() const;
    void refresh();

    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

    QList<int> sizes() const;
    void setSizes(const QList<int> &list);

    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);

    int handleWidth() const;
    void setHandleWidth(int);

    int indexOf(QWidget *w) const;
    QWidget *widget(int index) const;
    int count() const;

    void getRange(int index, int *, int *) const;
    QSplitterHandle *handle(int index) const;

    void setStretchFactor(int index, int stretch);

Q_SIGNALS:
    void splitterMoved(int pos, int index);

protected:
    virtual QSplitterHandle *createHandle();

    void childEvent(QChildEvent *) Q_DECL_OVERRIDE;

    bool event(QEvent *) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;

    void changeEvent(QEvent *) Q_DECL_OVERRIDE;
    void moveSplitter(int pos, int index);
    void setRubberBand(int position);
    int closestLegalPosition(int, int);


private:
    Q_DISABLE_COPY(QSplitter)
    Q_DECLARE_PRIVATE(QSplitter)
private:
    friend class QSplitterHandle;
};

Q_WIDGETS_EXPORT QTextStream& operator<<(QTextStream&, const QSplitter&);
Q_WIDGETS_EXPORT QTextStream& operator>>(QTextStream&, QSplitter&);

class QSplitterHandlePrivate;
class Q_WIDGETS_EXPORT QSplitterHandle : public QWidget
{
    Q_OBJECT
public:
    explicit QSplitterHandle(Qt::Orientation o, QSplitter *parent);
    ~QSplitterHandle();

    void setOrientation(Qt::Orientation o);
    Qt::Orientation orientation() const;
    bool opaqueResize() const;
    QSplitter *splitter() const;

    QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
    bool event(QEvent *) Q_DECL_OVERRIDE;

    void moveSplitter(int p);
    int closestLegalPosition(int p);

private:
    Q_DISABLE_COPY(QSplitterHandle)
    Q_DECLARE_PRIVATE(QSplitterHandle)
};

QT_END_NAMESPACE

#endif // QSPLITTER_H
