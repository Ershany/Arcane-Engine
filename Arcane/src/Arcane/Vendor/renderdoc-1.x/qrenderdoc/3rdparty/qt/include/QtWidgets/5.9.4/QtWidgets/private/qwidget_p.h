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

#ifndef QWIDGET_P_H
#define QWIDGET_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#include <QtWidgets/private/qtwidgetsglobal_p.h>
#include "QtWidgets/qwidget.h"
#include "private/qobject_p.h"
#include "QtCore/qrect.h"
#include "QtCore/qlocale.h"
#include "QtCore/qset.h"
#include "QtGui/qregion.h"
#include "QtGui/qinputmethod.h"
#include "QtGui/qopengl.h"
#include "QtGui/qsurfaceformat.h"
#include "QtWidgets/qsizepolicy.h"
#include "QtWidgets/qstyle.h"
#include "QtWidgets/qapplication.h"
#if QT_CONFIG(graphicseffect)
#include <private/qgraphicseffect_p.h>
#endif
#if QT_CONFIG(graphicsview)
#include "QtWidgets/qgraphicsproxywidget.h"
#include "QtWidgets/qgraphicsscene.h"
#include "QtWidgets/qgraphicsview.h"
#endif
#include <private/qgesture_p.h>

QT_BEGIN_NAMESPACE

// Extra QWidget data
//  - to minimize memory usage for members that are seldom used.
//  - top-level widgets have extra extra data to reduce cost further
class QPaintEngine;
class QPixmap;
class QWidgetBackingStore;
class QGraphicsProxyWidget;
class QWidgetItemV2;
class QOpenGLContext;
class QPlatformTextureList;

class QStyle;

class QUnifiedToolbarSurface;

// implemented in qshortcut.cpp
bool qWidgetShortcutContextMatcher(QObject *object, Qt::ShortcutContext context);

class QUpdateLaterEvent : public QEvent
{
public:
    explicit QUpdateLaterEvent(const QRegion& paintRegion)
        : QEvent(UpdateLater), m_region(paintRegion)
    {
    }

    ~QUpdateLaterEvent()
    {
    }

    inline const QRegion &region() const { return m_region; }

protected:
    QRegion m_region;
};



class Q_AUTOTEST_EXPORT QWidgetBackingStoreTracker
{

public:
    QWidgetBackingStoreTracker();
    ~QWidgetBackingStoreTracker();

    void create(QWidget *tlw);
    void destroy();

    void registerWidget(QWidget *w);
    void unregisterWidget(QWidget *w);
    void unregisterWidgetSubtree(QWidget *w);

    inline QWidgetBackingStore* data()
    {
        return m_ptr;
    }

    inline QWidgetBackingStore* operator->()
    {
        return m_ptr;
    }

    inline QWidgetBackingStore& operator*()
    {
        return *m_ptr;
    }

    inline operator bool() const
    {
        return (0 != m_ptr);
    }

private:
    Q_DISABLE_COPY(QWidgetBackingStoreTracker)

private:
    QWidgetBackingStore* m_ptr;
    QSet<QWidget *> m_widgets;
};

struct QTLWExtra {
    // *************************** Cross-platform variables *****************************

    // Regular pointers (keep them together to avoid gaps on 64 bits architectures).
    QIcon *icon; // widget icon
    QWidgetBackingStoreTracker backingStoreTracker;
    QBackingStore *backingStore;
    QPainter *sharedPainter;
    QWindow *window;
    QOpenGLContext *shareContext;

    // Implicit pointers (shared_null).
    QString caption; // widget caption
    QString iconText; // widget icon text
    QString role; // widget role
    QString filePath; // widget file path

    // Other variables.
    short incw, inch; // size increments
    short basew, baseh; // base sizes
     // frame strut, don't use these directly, use QWidgetPrivate::frameStrut() instead.
    QRect frameStrut;
    QRect normalGeometry; // used by showMin/maximized/FullScreen
    Qt::WindowFlags savedFlags; // Save widget flags while showing fullscreen
    int initialScreenIndex; // Screen number when passing a QDesktop[Screen]Widget as parent.

    QVector<QPlatformTextureList *> widgetTextures;

    // *************************** Cross-platform bit fields ****************************
    uint opacity : 8;
    uint posIncludesFrame : 1;
    uint sizeAdjusted : 1;
    uint inTopLevelResize : 1;
    uint inRepaint : 1;
    uint embedded : 1;

    // *************************** Platform specific values (bit fields first) **********
#if 0 /* Used to be included in Qt4 for Q_WS_X11 */ // <----------------------------------------------------------- X11
    uint spont_unmapped: 1; // window was spontaneously unmapped
    uint dnd : 1; // DND properties installed
    uint validWMState : 1; // is WM_STATE valid?
    uint waitingForMapNotify : 1; // show() has been called, haven't got the MapNotify yet
    WId parentWinId; // parent window Id (valid after reparenting)
    WId userTimeWindow; // window id that contains user-time timestamp when WM supports a _NET_WM_USER_TIME_WINDOW atom
    QPoint fullScreenOffset;
#ifndef QT_NO_XSYNC
    WId syncUpdateCounter;
    ulong syncRequestTimestamp;
    qint32 newCounterValueHi;
    quint32 newCounterValueLo;
#endif
#elif 0 /* Used to be included in Qt4 for Q_WS_WIN */ // <--------------------------------------------------------- WIN
    uint hotkeyRegistered: 1; // Hot key from the STARTUPINFO has been registered.
    HICON winIconBig; // internal big Windows icon
    HICON winIconSmall; // internal small Windows icon
#elif 0 /* Used to be included in Qt4 for Q_WS_MAC */ // <--------------------------------------------------------- MAC
    uint resizer : 4;
    uint isSetGeometry : 1;
    uint isMove : 1;
    quint32 wattr;
    quint32 wclass;
    WindowGroupRef group;
    IconRef windowIcon; // the current window icon, if set with setWindowIcon_sys.
    quint32 savedWindowAttributesFromMaximized; // Saved attributes from when the calling updateMaximizeButton_sys()
    // This value is just to make sure we maximize and restore to the right location, yet we allow apps to be maximized and
    // manually resized.
    // The name is misleading, since this is set when maximizing the window. It is a hint to saveGeometry(..) to record the
    // starting position as 0,0 instead of the normal starting position.
    bool wasMaximized;
#endif
};

struct QWExtra {
    // *************************** Cross-platform variables *****************************

    // Regular pointers (keep them together to avoid gaps on 64 bits architectures).
    void *glContext; // if the widget is hijacked by QGLWindowSurface
    QTLWExtra *topextra; // only useful for TLWs
#if QT_CONFIG(graphicsview)
    QGraphicsProxyWidget *proxyWidget; // if the widget is embedded
#endif
#ifndef QT_NO_CURSOR
    QCursor *curs;
#endif
    QPointer<QStyle> style;
    QPointer<QWidget> focus_proxy;

    // Implicit pointers (shared_empty/shared_null).
    QRegion mask; // widget mask
    QString styleSheet;

    // Other variables.
    qint32 minw;
    qint32 minh; // minimum size
    qint32 maxw;
    qint32 maxh; // maximum size
    quint16 customDpiX;
    quint16 customDpiY;
    QSize staticContentsSize;

    // *************************** Cross-platform bit fields ****************************
    uint explicitMinSize : 2;
    uint explicitMaxSize : 2;
    uint autoFillBackground : 1;
    uint nativeChildrenForced : 1;
    uint inRenderWithPainter : 1;
    uint hasMask : 1;
    uint hasWindowContainer : 1;

    // *************************** Platform specific values (bit fields first) **********
#if 0 /* Used to be included in Qt4 for Q_WS_WIN */ // <----------------------------------------------------------- WIN
#ifndef QT_NO_DRAGANDDROP
    QOleDropTarget *dropTarget; // drop target
    QList<QPointer<QWidget> > oleDropWidgets;
#endif
#elif 0 /* Used to be included in Qt4 for Q_WS_X11 */ // <--------------------------------------------------------- X11
    uint compress_events : 1;
    WId xDndProxy; // XDND forwarding to embedded windows
#elif 0 /* Used to be included in Qt4 for Q_WS_MAC */ // <------------------------------------------------------ MAC
    // Cocoa Mask stuff
    QImage maskBits;
    CGImageRef imageMask;
#endif
};

/*!
    \internal

    Returns \c true if \a p or any of its parents enable the
    Qt::BypassGraphicsProxyWidget window flag. Used in QWidget::show() and
    QWidget::setParent() to determine whether it's necessary to embed the
    widget into a QGraphicsProxyWidget or not.
*/
static inline bool bypassGraphicsProxyWidget(const QWidget *p)
{
    while (p) {
        if (p->windowFlags() & Qt::BypassGraphicsProxyWidget)
            return true;
        p = p->parentWidget();
    }
    return false;
}

class Q_WIDGETS_EXPORT QWidgetPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QWidget)

public:
    // *************************** Cross-platform ***************************************
    enum DrawWidgetFlags {
        DrawAsRoot = 0x01,
        DrawPaintOnScreen = 0x02,
        DrawRecursive = 0x04,
        DrawInvisible = 0x08,
        DontSubtractOpaqueChildren = 0x10,
        DontDrawOpaqueChildren = 0x20,
        DontDrawNativeChildren = 0x40,
        DontSetCompositionMode = 0x80
    };

    enum CloseMode {
        CloseNoEvent,
        CloseWithEvent,
        CloseWithSpontaneousEvent
    };

    enum Direction {
        DirectionNorth = 0x01,
        DirectionEast = 0x10,
        DirectionSouth = 0x02,
        DirectionWest = 0x20
    };

    // Functions.
    explicit QWidgetPrivate(int version = QObjectPrivateVersion);
    ~QWidgetPrivate();

    static QWidgetPrivate *get(QWidget *w) { return w->d_func(); }
    static const QWidgetPrivate *get(const QWidget *w) { return w->d_func(); }

    QWExtra *extraData() const;
    QTLWExtra *topData() const;
    QTLWExtra *maybeTopData() const;
    QPainter *sharedPainter() const;
    void setSharedPainter(QPainter *painter);
    QWidgetBackingStore *maybeBackingStore() const;

    template <typename T>
    void repaint(T t);

    template <typename T>
    void update(T t);

    void init(QWidget *desktopWidget, Qt::WindowFlags f);
    void create_sys(WId window, bool initializeWindow, bool destroyOldWindow);
    void createRecursively();
    void createWinId();

    void createTLExtra();
    void createExtra();
    void deleteExtra();
    void createSysExtra();
    void deleteSysExtra();
    void createTLSysExtra();
    void deleteTLSysExtra();
    void updateSystemBackground();
    void propagatePaletteChange();

    void setPalette_helper(const QPalette &);
    void resolvePalette();
    QPalette naturalWidgetPalette(uint inheritedMask) const;

    void setMask_sys(const QRegion &);

    void raise_sys();
    void lower_sys();
    void stackUnder_sys(QWidget *);

    void setFocus_sys();
    void updateFocusChild();

    void updateFont(const QFont &);
    inline void setFont_helper(const QFont &font) {
        if (data.fnt.resolve() == font.resolve() && data.fnt == font)
            return;
        updateFont(font);
    }
    void resolveFont();
    QFont naturalWidgetFont(uint inheritedMask) const;

    void setLayoutDirection_helper(Qt::LayoutDirection);
    void resolveLayoutDirection();

    void setLocale_helper(const QLocale &l, bool forceUpdate = false);
    void resolveLocale();

    void setStyle_helper(QStyle *newStyle, bool propagate, bool metalHack = false);
    void inheritStyle();

    void setUpdatesEnabled_helper(bool );

    void paintBackground(QPainter *, const QRegion &, int flags = DrawAsRoot) const;
    bool isAboutToShow() const;
    QRegion prepareToRender(const QRegion &region, QWidget::RenderFlags renderFlags);
    void render_helper(QPainter *painter, const QPoint &targetOffset, const QRegion &sourceRegion,
                       QWidget::RenderFlags renderFlags);
    void render(QPaintDevice *target, const QPoint &targetOffset, const QRegion &sourceRegion,
                QWidget::RenderFlags renderFlags);
    void drawWidget(QPaintDevice *pdev, const QRegion &rgn, const QPoint &offset, int flags,
                    QPainter *sharedPainter = 0, QWidgetBackingStore *backingStore = 0);
    void sendPaintEvent(const QRegion &toBePainted);


    void paintSiblingsRecursive(QPaintDevice *pdev, const QObjectList& children, int index,
                                const QRegion &rgn, const QPoint &offset, int flags,
                                QPainter *sharedPainter, QWidgetBackingStore *backingStore);

#if QT_CONFIG(graphicsview)
    static QGraphicsProxyWidget * nearestGraphicsProxyWidget(const QWidget *origin);
#endif
    void repaint_sys(const QRegion &rgn);

    QRect clipRect() const;
    QRegion clipRegion() const;
    void setSystemClip(QPaintDevice *paintDevice, const QRegion &region);
    void subtractOpaqueChildren(QRegion &rgn, const QRect &clipRect) const;
    void subtractOpaqueSiblings(QRegion &source, bool *hasDirtySiblingsAbove = 0,
                                bool alsoNonOpaque = false) const;
    void clipToEffectiveMask(QRegion &region) const;
    void updateIsOpaque();
    void setOpaque(bool opaque);
    void updateIsTranslucent();
    bool paintOnScreen() const;
#if QT_CONFIG(graphicseffect)
    void invalidateGraphicsEffectsRecursively();
#endif // QT_CONFIG(graphicseffect)

    const QRegion &getOpaqueChildren() const;
    void setDirtyOpaqueRegion();

    bool close_helper(CloseMode mode);

    void setWindowIcon_helper();
    void setWindowIcon_sys();
    void setWindowOpacity_sys(qreal opacity);
    void adjustQuitOnCloseAttribute();

    void scrollChildren(int dx, int dy);
    void moveRect(const QRect &, int dx, int dy);
    void scrollRect(const QRect &, int dx, int dy);
    void invalidateBuffer_resizeHelper(const QPoint &oldPos, const QSize &oldSize);
    // ### Qt 4.6: Merge into a template function (after MSVC isn't supported anymore).
    void invalidateBuffer(const QRegion &);
    void invalidateBuffer(const QRect &);
    bool isOverlapped(const QRect&) const;
    void syncBackingStore();
    void syncBackingStore(const QRegion &region);

    // tells the input method about the widgets transform
    void updateWidgetTransform(QEvent *event);

    void reparentFocusWidgets(QWidget *oldtlw);

    static int pointToRect(const QPoint &p, const QRect &r);

    void setWinId(WId);
    void showChildren(bool spontaneous);
    void hideChildren(bool spontaneous);
    void setParent_sys(QWidget *parent, Qt::WindowFlags);
    void scroll_sys(int dx, int dy);
    void scroll_sys(int dx, int dy, const QRect &r);
    void deactivateWidgetCleanup();
    void setGeometry_sys(int, int, int, int, bool);
    void fixPosIncludesFrame();
    void sendPendingMoveAndResizeEvents(bool recursive = false, bool disableUpdates = false);
    void activateChildLayoutsRecursively();
    void show_recursive();
    void show_helper();
    void show_sys();
    void hide_sys();
    void hide_helper();
    void _q_showIfNotHidden();

    void setEnabled_helper(bool);
    void registerDropSite(bool);
    static void adjustFlags(Qt::WindowFlags &flags, QWidget *w = 0);

    void updateFrameStrut();
    QRect frameStrut() const;

#ifdef QT_KEYPAD_NAVIGATION
    static bool navigateToDirection(Direction direction);
    static QWidget *widgetInNavigationDirection(Direction direction);
    static bool canKeypadNavigate(Qt::Orientation orientation);
    static bool inTabWidget(QWidget *widget);
#endif

    void setWindowIconText_sys(const QString &cap);
    void setWindowIconText_helper(const QString &cap);
    void setWindowTitle_sys(const QString &cap);
    void setWindowFilePath_sys(const QString &filePath);

#ifndef QT_NO_CURSOR
    void setCursor_sys(const QCursor &cursor);
    void unsetCursor_sys();
#endif

    void setWindowTitle_helper(const QString &cap);
    void setWindowFilePath_helper(const QString &filePath);
    void setWindowModified_helper();
    virtual void setWindowFlags(Qt::WindowFlags windowFlags);

    bool setMinimumSize_helper(int &minw, int &minh);
    bool setMaximumSize_helper(int &maxw, int &maxh);
    void setConstraints_sys();
    bool pointInsideRectAndMask(const QPoint &) const;
    QWidget *childAt_helper(const QPoint &, bool) const;
    QWidget *childAtRecursiveHelper(const QPoint &p, bool) const;
    void updateGeometry_helper(bool forceUpdate);

    void getLayoutItemMargins(int *left, int *top, int *right, int *bottom) const;
    void setLayoutItemMargins(int left, int top, int right, int bottom);
    void setLayoutItemMargins(QStyle::SubElement element, const QStyleOption *opt = 0);

    void updateContentsRect();
    QMargins safeAreaMargins() const;

    // aboutToDestroy() is called just before the contents of
    // QWidget::destroy() is executed. It's used to signal QWidget
    // sub-classes that their internals are about to be released.
    virtual void aboutToDestroy() {}

    inline QWidget *effectiveFocusWidget() {
        QWidget *w = q_func();
        while (w->focusProxy())
            w = w->focusProxy();
        return w;
    }

    void setModal_sys();

    // This is an helper function that return the available geometry for
    // a widget and takes care is this one is in QGraphicsView.
    // If the widget is not embed in a scene then the geometry available is
    // null, we let QDesktopWidget decide for us.
    static QRect screenGeometry(const QWidget *widget)
    {
        QRect screen;
#if QT_CONFIG(graphicsview)
        QGraphicsProxyWidget *ancestorProxy = widget->d_func()->nearestGraphicsProxyWidget(widget);
        //It's embedded if it has an ancestor
        if (ancestorProxy) {
            if (!bypassGraphicsProxyWidget(widget) && ancestorProxy->scene() != 0) {
                // One view, let be smart and return the viewport rect then the popup is aligned
                if (ancestorProxy->scene()->views().size() == 1) {
                    QGraphicsView *view = ancestorProxy->scene()->views().at(0);
                    screen = view->mapToScene(view->viewport()->rect()).boundingRect().toRect();
                } else {
                    screen = ancestorProxy->scene()->sceneRect().toRect();
                }
            }
        }
#else
        Q_UNUSED(widget);
#endif
        return screen;
    }

    inline void setRedirected(QPaintDevice *replacement, const QPoint &offset)
    {
        Q_ASSERT(q_func()->testAttribute(Qt::WA_WState_InPaintEvent));
        redirectDev = replacement;
        redirectOffset = offset;
    }

    inline QPaintDevice *redirected(QPoint *offset) const
    {
        if (offset)
            *offset = redirectDev ? redirectOffset : QPoint();
        return redirectDev;
    }

    inline void restoreRedirected()
    { redirectDev = 0; }

    inline void enforceNativeChildren()
    {
        if (!extra)
            createExtra();

        if (extra->nativeChildrenForced)
            return;
        extra->nativeChildrenForced = 1;

        for (int i = 0; i < children.size(); ++i) {
            if (QWidget *child = qobject_cast<QWidget *>(children.at(i)))
                child->setAttribute(Qt::WA_NativeWindow);
        }
    }

    inline bool nativeChildrenForced() const
    {
        return extra ? extra->nativeChildrenForced : false;
    }

    inline QRect effectiveRectFor(const QRect &rect) const
    {
#if QT_CONFIG(graphicseffect)
        if (graphicsEffect && graphicsEffect->isEnabled())
            return graphicsEffect->boundingRectFor(rect).toAlignedRect();
#endif // QT_CONFIG(graphicseffect)
        return rect;
    }

    QSize adjustedSize() const;

    inline void handleSoftwareInputPanel(Qt::MouseButton button, bool clickCausedFocus)
    {
        Q_Q(QWidget);
        if (button == Qt::LeftButton && qApp->autoSipEnabled()) {
            QStyle::RequestSoftwareInputPanel behavior = QStyle::RequestSoftwareInputPanel(
                    q->style()->styleHint(QStyle::SH_RequestSoftwareInputPanel));
            if (!clickCausedFocus || behavior == QStyle::RSIP_OnMouseClick) {
                QGuiApplication::inputMethod()->show();
            }
        }
    }

    void setWSGeometry();

    inline QPoint mapToWS(const QPoint &p) const
    { return p - data.wrect.topLeft(); }

    inline QPoint mapFromWS(const QPoint &p) const
    { return p + data.wrect.topLeft(); }

    inline QRect mapToWS(const QRect &r) const
    { return r.translated(-data.wrect.topLeft()); }

    inline QRect mapFromWS(const QRect &r) const
    { return r.translated(data.wrect.topLeft()); }

    QOpenGLContext *shareContext() const;

    virtual QObject *focusObject() { return 0; }

#ifndef QT_NO_OPENGL
    virtual GLuint textureId() const { return 0; }
    virtual QImage grabFramebuffer() { return QImage(); }
    virtual void beginBackingStorePainting() { }
    virtual void endBackingStorePainting() { }
    virtual void beginCompose() { }
    virtual void endCompose() { }
    void setRenderToTexture() { renderToTexture = true; setTextureChildSeen(); }
    void setTextureChildSeen()
    {
        Q_Q(QWidget);
        if (textureChildSeen)
            return;
        textureChildSeen = 1;

        if (!q->isWindow()) {
            QWidget *parent = q->parentWidget();
            if (parent)
                get(parent)->setTextureChildSeen();
        }
    }
    static void sendComposeStatus(QWidget *w, bool end);
    // Called on setViewport().
    virtual void initializeViewportFramebuffer() { }
    // When using a QOpenGLWidget as viewport with QAbstractScrollArea, resize events are
    // filtered away from the widget. This is fine for QGLWidget but bad for QOpenGLWidget
    // since the fbo must be resized. We need an alternative way to notify.
    virtual void resizeViewportFramebuffer() { }
    // Called after each paint event.
    virtual void resolveSamples() { }
#endif

    static void setWidgetParentHelper(QObject *widgetAsObject, QObject *newParent);

    // Variables.
    // Regular pointers (keep them together to avoid gaps on 64 bit architectures).
    QWExtra *extra;
    QWidget *focus_next;
    QWidget *focus_prev;
    QWidget *focus_child;
    QLayout *layout;
    QRegion *needsFlush;
    QPaintDevice *redirectDev;
    QWidgetItemV2 *widgetItem;
    QPaintEngine *extraPaintEngine;
    mutable const QMetaObject *polished;
    QGraphicsEffect *graphicsEffect;
    // All widgets are added into the allWidgets set. Once
    // they receive a window id they are also added to the mapper.
    // This should just ensure that all widgets are deleted by QApplication
    static QWidgetMapper *mapper;
    static QWidgetSet *allWidgets;
#if !defined(QT_NO_IM)
    Qt::InputMethodHints imHints;
#endif
#ifdef QT_KEYPAD_NAVIGATION
    static QPointer<QWidget> editingWidget;
#endif

    // Implicit pointers (shared_null/shared_empty).
    QRegion opaqueChildren;
    QRegion dirty;
#ifndef QT_NO_TOOLTIP
    QString toolTip;
    int toolTipDuration;
#endif
#if QT_CONFIG(statustip)
    QString statusTip;
#endif
#if QT_CONFIG(whatsthis)
    QString whatsThis;
#endif
#ifndef QT_NO_ACCESSIBILITY
    QString accessibleName;
    QString accessibleDescription;
#endif

    // Other variables.
    uint inheritedFontResolveMask;
    uint inheritedPaletteResolveMask;
    short leftmargin;
    short topmargin;
    short rightmargin;
    short bottommargin;
    signed char leftLayoutItemMargin;
    signed char topLayoutItemMargin;
    signed char rightLayoutItemMargin;
    signed char bottomLayoutItemMargin;
    static int instanceCounter; // Current number of widget instances
    static int maxInstances; // Maximum number of widget instances
    Qt::HANDLE hd;
    QWidgetData data;
    QSizePolicy size_policy;
    QLocale locale;
    QPoint redirectOffset;
#ifndef QT_NO_ACTION
    QList<QAction*> actions;
#endif
#ifndef QT_NO_GESTURES
    QMap<Qt::GestureType, Qt::GestureFlags> gestureContext;
#endif

    // Bit fields.
    uint high_attributes[4]; // the low ones are in QWidget::widget_attributes
    QPalette::ColorRole fg_role : 8;
    QPalette::ColorRole bg_role : 8;
    uint dirtyOpaqueChildren : 1;
    uint isOpaque : 1;
    uint retainSizeWhenHiddenChanged : 1;
    uint inDirtyList : 1;
    uint isScrolled : 1;
    uint isMoved : 1;
    uint usesDoubleBufferedGLContext : 1;
    uint mustHaveWindowHandle : 1;
    uint renderToTexture : 1;
    uint textureChildSeen : 1;
#ifndef QT_NO_IM
    uint inheritsInputMethodHints : 1;
#endif
#ifndef QT_NO_OPENGL
    uint renderToTextureReallyDirty : 1;
    uint renderToTextureComposeActive : 1;
#endif
    uint childrenHiddenByWState : 1;
    uint childrenShownByExpose : 1;

    // *************************** Platform specific ************************************
#if defined(Q_OS_WIN)
    uint noPaintOnScreen : 1; // see qwidget.cpp ::paintEngine()
#endif
#if 0 /* Used to be included in Qt4 for Q_WS_X11 */ // <----------------------------------------------------------- X11
    Qt::HANDLE picture;
    static QWidget *mouseGrabber;
    static QWidget *keyboardGrabber;

    void setWindowRole();
    void sendStartupMessage(const char *message) const;
    void x11UpdateIsOpaque();
    bool isBackgroundInherited() const;
    void updateX11AcceptFocus();
    QPoint mapToGlobal(const QPoint &pos) const;
    QPoint mapFromGlobal(const QPoint &pos) const;
#elif 0 /* Used to be included in Qt4 for Q_WS_WIN */ // <--------------------------------------------------------- WIN
#ifndef QT_NO_GESTURES
    uint nativeGesturePanEnabled : 1;
#endif
    bool shouldShowMaximizeButton();
    void winUpdateIsOpaque();
    void reparentChildren();
#ifndef QT_NO_DRAGANDDROP
    QOleDropTarget *registerOleDnd(QWidget *widget);
    void unregisterOleDnd(QWidget *widget, QOleDropTarget *target);
#endif
    void grabMouseWhileInWindow();
    void registerTouchWindow();
    void winSetupGestures();
#elif defined(Q_OS_MAC) // <--------------------------------------------------------- MAC
    void macUpdateSizeAttribute();
#elif 0 /* Used to be included in Qt4 for Q_WS_MAC */ // <--------------------------------------------------------- MAC (old stuff)
    // This is new stuff
    uint needWindowChange : 1;

    // Each wiget keeps a list of all its child and grandchild OpenGL widgets.
    // This list is used to update the gl context whenever a parent and a granparent
    // moves, and also to check for intersections with gl widgets within the window
    // when a widget moves.
    struct GlWidgetInfo
    {
        GlWidgetInfo(QWidget *widget) : widget(widget), lastUpdateWidget(0) { }
        bool operator==(const GlWidgetInfo &other) const { return (widget == other.widget); }
        QWidget * widget;
        QWidget * lastUpdateWidget;
    };

    // dirtyOnWidget contains the areas in the widget that needs to be repained,
    // in the same way as dirtyOnScreen does for the window. Areas are added in
    // dirtyWidget_sys and cleared in the paint event. In scroll_sys we then use
    // this information repaint invalid areas when widgets are scrolled.
    QRegion dirtyOnWidget;
    EventHandlerRef window_event;
    QList<GlWidgetInfo> glWidgets;

    //these are here just for code compat (HIViews)
    Qt::HANDLE qd_hd;

    void macUpdateHideOnSuspend();
    void macUpdateOpaqueSizeGrip();
    void macUpdateIgnoreMouseEvents();
    void macUpdateMetalAttribute();
    void macUpdateIsOpaque();
    void macSetNeedsDisplay(QRegion region);
    void setEnabled_helper_sys(bool enable);
    bool isRealWindow() const;
    void adjustWithinMaxAndMinSize(int &w, int &h);
    void applyMaxAndMinSizeOnWindow();
    void update_sys(const QRect &rect);
    void update_sys(const QRegion &rgn);
    void setGeometry_sys_helper(int, int, int, int, bool);
    void updateMaximizeButton_sys();
    void createWindow_sys();
    void recreateMacWindow();
    void setSubWindowStacking(bool set);
    void setWindowLevel();
    void finishCreateWindow_sys_Cocoa(void * /*NSWindow * */ windowRef);
    void syncCocoaMask();
    void finishCocoaMaskSetup();
    // Did we add the drawRectOriginal method?
    bool drawRectOriginalAdded;
    // Is the original drawRect method available?
    bool originalDrawMethod;
    // Do we need to change the methods?
    bool changeMethods;

    // Unified toolbar variables
    bool isInUnifiedToolbar;
    QUnifiedToolbarSurface *unifiedSurface;
    QPoint toolbar_offset;
    QWidget *toolbar_ancestor;
    bool flushRequested;
    bool touchEventsEnabled;
    void determineWindowClass();
    void transferChildren();
    bool qt_mac_dnd_event(uint, DragRef);
    void toggleDrawers(bool);
    //mac event functions
    static bool qt_create_root_win();
    static void qt_clean_root_win();
    static bool qt_mac_update_sizer(QWidget *, int up = 0);
    static OSStatus qt_window_event(EventHandlerCallRef er, EventRef event, void *);
    static OSStatus qt_widget_event(EventHandlerCallRef er, EventRef event, void *);
    static bool qt_widget_rgn(QWidget *, short, RgnHandle, bool);
    void registerTouchWindow(bool enable = true);
#endif
    void setNetWmWindowTypes(bool skipIfMissing = false);

    bool stealKeyboardGrab(bool grab);
    bool stealMouseGrab(bool grab);
};

struct QWidgetPaintContext
{
    inline QWidgetPaintContext(QPaintDevice *d, const QRegion &r, const QPoint &o, int f,
                               QPainter *p, QWidgetBackingStore *b)
        : pdev(d), rgn(r), offset(o), flags(f), sharedPainter(p), backingStore(b), painter(0) {}

    QPaintDevice *pdev;
    QRegion rgn;
    QPoint offset;
    int flags;
    QPainter *sharedPainter;
    QWidgetBackingStore *backingStore;
    QPainter *painter;
};

#if QT_CONFIG(graphicseffect)
class QWidgetEffectSourcePrivate : public QGraphicsEffectSourcePrivate
{
public:
    QWidgetEffectSourcePrivate(QWidget *widget)
        : QGraphicsEffectSourcePrivate(), m_widget(widget), context(0), updateDueToGraphicsEffect(false)
    {}

    void detach() Q_DECL_OVERRIDE
    { m_widget->d_func()->graphicsEffect = 0; }

    const QGraphicsItem *graphicsItem() const Q_DECL_OVERRIDE
    { return 0; }

    const QWidget *widget() const Q_DECL_OVERRIDE
    { return m_widget; }

    void update() Q_DECL_OVERRIDE
    {
        updateDueToGraphicsEffect = true;
        m_widget->update();
        updateDueToGraphicsEffect = false;
    }

    bool isPixmap() const Q_DECL_OVERRIDE
    { return false; }

    void effectBoundingRectChanged() Q_DECL_OVERRIDE
    {
        // ### This function should take a rect parameter; then we can avoid
        // updating too much on the parent widget.
        if (QWidget *parent = m_widget->parentWidget())
            parent->update();
        else
            update();
    }

    const QStyleOption *styleOption() const Q_DECL_OVERRIDE
    { return 0; }

    QRect deviceRect() const Q_DECL_OVERRIDE
    { return m_widget->window()->rect(); }

    QRectF boundingRect(Qt::CoordinateSystem system) const Q_DECL_OVERRIDE;
    void draw(QPainter *p) Q_DECL_OVERRIDE;
    QPixmap pixmap(Qt::CoordinateSystem system, QPoint *offset,
                   QGraphicsEffect::PixmapPadMode mode) const Q_DECL_OVERRIDE;

    QWidget *m_widget;
    QWidgetPaintContext *context;
    QTransform lastEffectTransform;
    bool updateDueToGraphicsEffect;
};
#endif // QT_CONFIG(graphicseffect)

inline QWExtra *QWidgetPrivate::extraData() const
{
    return extra;
}

inline QTLWExtra *QWidgetPrivate::topData() const
{
    const_cast<QWidgetPrivate *>(this)->createTLExtra();
    return extra->topextra;
}

inline QTLWExtra *QWidgetPrivate::maybeTopData() const
{
    return extra ? extra->topextra : 0;
}

inline QPainter *QWidgetPrivate::sharedPainter() const
{
    Q_Q(const QWidget);
    QTLWExtra *x = q->window()->d_func()->maybeTopData();
    return x ? x->sharedPainter : 0;
}

inline void QWidgetPrivate::setSharedPainter(QPainter *painter)
{
    Q_Q(QWidget);
    QTLWExtra *x = q->window()->d_func()->topData();
    x->sharedPainter = painter;
}

inline bool QWidgetPrivate::pointInsideRectAndMask(const QPoint &p) const
{
    Q_Q(const QWidget);
    return q->rect().contains(p) && (!extra || !extra->hasMask || q->testAttribute(Qt::WA_MouseNoMask)
                                     || extra->mask.contains(p));
}

inline QWidgetBackingStore *QWidgetPrivate::maybeBackingStore() const
{
    Q_Q(const QWidget);
    QTLWExtra *x = q->window()->d_func()->maybeTopData();
    return x ? x->backingStoreTracker.data() : 0;
}

QT_END_NAMESPACE

#endif // QWIDGET_P_H
