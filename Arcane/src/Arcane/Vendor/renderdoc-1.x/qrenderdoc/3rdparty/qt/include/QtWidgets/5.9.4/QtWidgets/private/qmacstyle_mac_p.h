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

#ifndef QMACSTYLE_MAC_P_H
#define QMACSTYLE_MAC_P_H

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

#include <QtWidgets/private/qtwidgetsglobal_p.h>
#include <QtWidgets/qcommonstyle.h>

QT_BEGIN_NAMESPACE


#if QT_CONFIG(style_mac)

class QPalette;

class QPushButton;
class QStyleOptionButton;
class QMacStylePrivate;
class QMacStyle : public QCommonStyle
{
    Q_OBJECT
public:
    QMacStyle();
    virtual ~QMacStyle();

    void polish(QWidget *w);
    void unpolish(QWidget *w);

    void polish(QApplication*);
    void unpolish(QApplication*);

    void polish(QPalette &pal);

    void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
                       const QWidget *w = 0) const;
    void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p,
                     const QWidget *w = 0) const;
    QRect subElementRect(SubElement r, const QStyleOption *opt, const QWidget *widget = 0) const;
    void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
                            const QWidget *w = 0) const;
    SubControl hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                               const QPoint &pt, const QWidget *w = 0) const;
    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc,
                         const QWidget *w = 0) const;
    QSize sizeFromContents(ContentsType ct, const QStyleOption *opt,
                           const QSize &contentsSize, const QWidget *w = 0) const;

    int pixelMetric(PixelMetric pm, const QStyleOption *opt = 0, const QWidget *widget = 0) const;

    QPalette standardPalette() const;

    virtual int styleHint(StyleHint sh, const QStyleOption *opt = 0, const QWidget *w = 0,
                          QStyleHintReturn *shret = 0) const;

    enum WidgetSizePolicy { SizeSmall, SizeLarge, SizeMini, SizeDefault
    };

    static void setWidgetSizePolicy(const QWidget *w, WidgetSizePolicy policy);
    static WidgetSizePolicy widgetSizePolicy(const QWidget *w, const QStyleOption *opt = 0);

    QPixmap standardPixmap(StandardPixmap sp, const QStyleOption *opt,
                           const QWidget *widget = 0) const;

    QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                const QStyleOption *opt) const;

    virtual void drawItemText(QPainter *p, const QRect &r, int flags, const QPalette &pal,
                              bool enabled, const QString &text, QPalette::ColorRole textRole  = QPalette::NoRole) const;

    bool event(QEvent *e);

    QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption *opt = 0,
                       const QWidget *widget = 0) const;
    int layoutSpacing(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2,
                      Qt::Orientation orientation, const QStyleOption *option = 0,
                      const QWidget *widget = 0) const;

private:
    Q_DISABLE_COPY(QMacStyle)
    Q_DECLARE_PRIVATE(QMacStyle)

#if QT_CONFIG(pushbutton)
    friend bool qt_mac_buttonIsRenderedFlat(const QPushButton *pushButton, const QStyleOptionButton *option);
#endif
};

#endif

QT_END_NAMESPACE

#endif // QMACSTYLE_MAC_P_H
