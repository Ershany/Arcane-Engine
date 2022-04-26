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

#ifndef QPROXYSTYLE_H
#define QPROXYSTYLE_H

#include <QtWidgets/qtwidgetsglobal.h>
#include <QtWidgets/QCommonStyle>

QT_BEGIN_NAMESPACE


#if !defined(QT_NO_STYLE_PROXY)

class QProxyStylePrivate;
class Q_WIDGETS_EXPORT QProxyStyle : public QCommonStyle
{
    Q_OBJECT

public:
    QProxyStyle(QStyle *style = Q_NULLPTR);
    QProxyStyle(const QString &key);
    ~QProxyStyle();

    QStyle *baseStyle() const;
    void setBaseStyle(QStyle *style);

    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = Q_NULLPTR) const Q_DECL_OVERRIDE;
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = Q_NULLPTR) const Q_DECL_OVERRIDE;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = Q_NULLPTR) const Q_DECL_OVERRIDE;
    void drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled,
                      const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const Q_DECL_OVERRIDE;
    virtual void drawItemPixmap(QPainter *painter, const QRect &rect, int alignment, const QPixmap &pixmap) const Q_DECL_OVERRIDE;

    QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const Q_DECL_OVERRIDE;

    QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const Q_DECL_OVERRIDE;
    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *widget) const Q_DECL_OVERRIDE;
    QRect itemTextRect(const QFontMetrics &fm, const QRect &r, int flags, bool enabled, const QString &text) const Q_DECL_OVERRIDE;
    QRect itemPixmapRect(const QRect &r, int flags, const QPixmap &pixmap) const Q_DECL_OVERRIDE;

    SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option, const QPoint &pos, const QWidget *widget = Q_NULLPTR) const Q_DECL_OVERRIDE;
    int styleHint(StyleHint hint, const QStyleOption *option = Q_NULLPTR, const QWidget *widget = Q_NULLPTR, QStyleHintReturn *returnData = Q_NULLPTR) const Q_DECL_OVERRIDE;
    int pixelMetric(PixelMetric metric, const QStyleOption *option = Q_NULLPTR, const QWidget *widget = Q_NULLPTR) const Q_DECL_OVERRIDE;
    int layoutSpacing(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2,
                      Qt::Orientation orientation, const QStyleOption *option = Q_NULLPTR, const QWidget *widget = Q_NULLPTR) const Q_DECL_OVERRIDE;

    QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption *option = Q_NULLPTR, const QWidget *widget = Q_NULLPTR) const Q_DECL_OVERRIDE;
    QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt, const QWidget *widget = Q_NULLPTR) const Q_DECL_OVERRIDE;
    QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap, const QStyleOption *opt) const Q_DECL_OVERRIDE;
    QPalette standardPalette() const Q_DECL_OVERRIDE;

    void polish(QWidget *widget) Q_DECL_OVERRIDE;
    void polish(QPalette &pal) Q_DECL_OVERRIDE;
    void polish(QApplication *app) Q_DECL_OVERRIDE;

    void unpolish(QWidget *widget) Q_DECL_OVERRIDE;
    void unpolish(QApplication *app) Q_DECL_OVERRIDE;

protected:
    bool event(QEvent *e) Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(QProxyStyle)
    Q_DECLARE_PRIVATE(QProxyStyle)
};

#endif // QT_NO_STYLE_PROXY

QT_END_NAMESPACE

#endif // QPROXYSTYLE_H
