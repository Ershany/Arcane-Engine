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

#ifndef QGRAPHICSVIEW_H
#define QGRAPHICSVIEW_H

#include <QtWidgets/qtwidgetsglobal.h>
#include <QtCore/qmetatype.h>
#include <QtGui/qpainter.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qgraphicsscene.h>

QT_REQUIRE_CONFIG(graphicsview);

QT_BEGIN_NAMESPACE

class QGraphicsItem;
class QPainterPath;
class QPolygonF;
class QStyleOptionGraphicsItem;

class QGraphicsViewPrivate;
class Q_WIDGETS_EXPORT QGraphicsView : public QAbstractScrollArea
{
    Q_OBJECT
    Q_FLAGS(QPainter::RenderHints CacheMode OptimizationFlags)
    Q_PROPERTY(QBrush backgroundBrush READ backgroundBrush WRITE setBackgroundBrush)
    Q_PROPERTY(QBrush foregroundBrush READ foregroundBrush WRITE setForegroundBrush)
    Q_PROPERTY(bool interactive READ isInteractive WRITE setInteractive)
    Q_PROPERTY(QRectF sceneRect READ sceneRect WRITE setSceneRect)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)
    Q_PROPERTY(QPainter::RenderHints renderHints READ renderHints WRITE setRenderHints)
    Q_PROPERTY(DragMode dragMode READ dragMode WRITE setDragMode)
    Q_PROPERTY(CacheMode cacheMode READ cacheMode WRITE setCacheMode)
    Q_PROPERTY(ViewportAnchor transformationAnchor READ transformationAnchor WRITE setTransformationAnchor)
    Q_PROPERTY(ViewportAnchor resizeAnchor READ resizeAnchor WRITE setResizeAnchor)
    Q_PROPERTY(ViewportUpdateMode viewportUpdateMode READ viewportUpdateMode WRITE setViewportUpdateMode)
#if QT_CONFIG(rubberband)
    Q_PROPERTY(Qt::ItemSelectionMode rubberBandSelectionMode READ rubberBandSelectionMode WRITE setRubberBandSelectionMode)
#endif
    Q_PROPERTY(OptimizationFlags optimizationFlags READ optimizationFlags WRITE setOptimizationFlags)

public:
    enum ViewportAnchor {
        NoAnchor,
        AnchorViewCenter,
        AnchorUnderMouse
    };
    Q_ENUM(ViewportAnchor)

    enum CacheModeFlag {
        CacheNone = 0x0,
        CacheBackground = 0x1
    };
    Q_DECLARE_FLAGS(CacheMode, CacheModeFlag)

    enum DragMode {
        NoDrag,
        ScrollHandDrag,
        RubberBandDrag
    };
    Q_ENUM(DragMode)

    enum ViewportUpdateMode {
        FullViewportUpdate,
        MinimalViewportUpdate,
        SmartViewportUpdate,
        NoViewportUpdate,
        BoundingRectViewportUpdate
    };
    Q_ENUM(ViewportUpdateMode)

    enum OptimizationFlag {
        DontClipPainter = 0x1, // obsolete
        DontSavePainterState = 0x2,
        DontAdjustForAntialiasing = 0x4,
        IndirectPainting = 0x8
    };
    Q_DECLARE_FLAGS(OptimizationFlags, OptimizationFlag)

    QGraphicsView(QWidget *parent = Q_NULLPTR);
    QGraphicsView(QGraphicsScene *scene, QWidget *parent = Q_NULLPTR);
    ~QGraphicsView();

    QSize sizeHint() const Q_DECL_OVERRIDE;

    QPainter::RenderHints renderHints() const;
    void setRenderHint(QPainter::RenderHint hint, bool enabled = true);
    void setRenderHints(QPainter::RenderHints hints);

    Qt::Alignment alignment() const;
    void setAlignment(Qt::Alignment alignment);

    ViewportAnchor transformationAnchor() const;
    void setTransformationAnchor(ViewportAnchor anchor);

    ViewportAnchor resizeAnchor() const;
    void setResizeAnchor(ViewportAnchor anchor);

    ViewportUpdateMode viewportUpdateMode() const;
    void setViewportUpdateMode(ViewportUpdateMode mode);

    OptimizationFlags optimizationFlags() const;
    void setOptimizationFlag(OptimizationFlag flag, bool enabled = true);
    void setOptimizationFlags(OptimizationFlags flags);

    DragMode dragMode() const;
    void setDragMode(DragMode mode);

#if QT_CONFIG(rubberband)
    Qt::ItemSelectionMode rubberBandSelectionMode() const;
    void setRubberBandSelectionMode(Qt::ItemSelectionMode mode);
    QRect rubberBandRect() const;
#endif

    CacheMode cacheMode() const;
    void setCacheMode(CacheMode mode);
    void resetCachedContent();

    bool isInteractive() const;
    void setInteractive(bool allowed);

    QGraphicsScene *scene() const;
    void setScene(QGraphicsScene *scene);

    QRectF sceneRect() const;
    void setSceneRect(const QRectF &rect);
    inline void setSceneRect(qreal x, qreal y, qreal w, qreal h);

    QMatrix matrix() const;
    void setMatrix(const QMatrix &matrix, bool combine = false);
    void resetMatrix();
    QTransform transform() const;
    QTransform viewportTransform() const;
    bool isTransformed() const;
    void setTransform(const QTransform &matrix, bool combine = false);
    void resetTransform();
    void rotate(qreal angle);
    void scale(qreal sx, qreal sy);
    void shear(qreal sh, qreal sv);
    void translate(qreal dx, qreal dy);

    void centerOn(const QPointF &pos);
    inline void centerOn(qreal x, qreal y);
    void centerOn(const QGraphicsItem *item);
    void ensureVisible(const QRectF &rect, int xmargin = 50, int ymargin = 50);
    inline void ensureVisible(qreal x, qreal y, qreal w, qreal h, int xmargin = 50, int ymargin = 50);
    void ensureVisible(const QGraphicsItem *item, int xmargin = 50, int ymargin = 50);
    void fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRadioMode = Qt::IgnoreAspectRatio);
    inline void fitInView(qreal x, qreal y, qreal w, qreal h,
                          Qt::AspectRatioMode aspectRadioMode = Qt::IgnoreAspectRatio);
    void fitInView(const QGraphicsItem *item,
                   Qt::AspectRatioMode aspectRadioMode = Qt::IgnoreAspectRatio);

    void render(QPainter *painter, const QRectF &target = QRectF(), const QRect &source = QRect(),
                Qt::AspectRatioMode aspectRatioMode = Qt::KeepAspectRatio);

    QList<QGraphicsItem *> items() const;
    QList<QGraphicsItem *> items(const QPoint &pos) const;
    inline QList<QGraphicsItem *> items(int x, int y) const;
    QList<QGraphicsItem *> items(const QRect &rect, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) const;
    inline QList<QGraphicsItem *> items(int x, int y, int w, int h, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) const;
    QList<QGraphicsItem *> items(const QPolygon &polygon, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) const;
    QList<QGraphicsItem *> items(const QPainterPath &path, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) const;
    QGraphicsItem *itemAt(const QPoint &pos) const;
    inline QGraphicsItem *itemAt(int x, int y) const;

    QPointF mapToScene(const QPoint &point) const;
    QPolygonF mapToScene(const QRect &rect) const;
    QPolygonF mapToScene(const QPolygon &polygon) const;
    QPainterPath mapToScene(const QPainterPath &path) const;
    QPoint mapFromScene(const QPointF &point) const;
    QPolygon mapFromScene(const QRectF &rect) const;
    QPolygon mapFromScene(const QPolygonF &polygon) const;
    QPainterPath mapFromScene(const QPainterPath &path) const;
    inline QPointF mapToScene(int x, int y) const;
    inline QPolygonF mapToScene(int x, int y, int w, int h) const;
    inline QPoint mapFromScene(qreal x, qreal y) const;
    inline QPolygon mapFromScene(qreal x, qreal y, qreal w, qreal h) const;

    QVariant inputMethodQuery(Qt::InputMethodQuery query) const Q_DECL_OVERRIDE;

    QBrush backgroundBrush() const;
    void setBackgroundBrush(const QBrush &brush);

    QBrush foregroundBrush() const;
    void setForegroundBrush(const QBrush &brush);

public Q_SLOTS:
    void updateScene(const QList<QRectF> &rects);
    void invalidateScene(const QRectF &rect = QRectF(), QGraphicsScene::SceneLayers layers = QGraphicsScene::AllLayers);
    void updateSceneRect(const QRectF &rect);

#if QT_CONFIG(rubberband)
Q_SIGNALS:
    void rubberBandChanged(QRect viewportRect, QPointF fromScenePoint, QPointF toScenePoint);
#endif

protected Q_SLOTS:
    void setupViewport(QWidget *widget) Q_DECL_OVERRIDE;

protected:
    QGraphicsView(QGraphicsViewPrivate &, QWidget *parent = Q_NULLPTR);
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    bool viewportEvent(QEvent *event) Q_DECL_OVERRIDE;

#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;
#endif
#if QT_CONFIG(draganddrop)
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dragLeaveEvent(QDragLeaveEvent *event) Q_DECL_OVERRIDE;
    void dragMoveEvent(QDragMoveEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
#endif
    void focusInEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    bool focusNextPrevChild(bool next) Q_DECL_OVERRIDE;
    void focusOutEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
#endif
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void scrollContentsBy(int dx, int dy) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void inputMethodEvent(QInputMethodEvent *event) Q_DECL_OVERRIDE;

    virtual void drawBackground(QPainter *painter, const QRectF &rect);
    virtual void drawForeground(QPainter *painter, const QRectF &rect);
    virtual void drawItems(QPainter *painter, int numItems,
                           QGraphicsItem *items[],
                           const QStyleOptionGraphicsItem options[]);

private:
    Q_DECLARE_PRIVATE(QGraphicsView)
    Q_DISABLE_COPY(QGraphicsView)
#ifndef QT_NO_CURSOR
    Q_PRIVATE_SLOT(d_func(), void _q_setViewportCursor(const QCursor &))
    Q_PRIVATE_SLOT(d_func(), void _q_unsetViewportCursor())
#endif
    friend class QGraphicsSceneWidget;
    friend class QGraphicsScene;
    friend class QGraphicsScenePrivate;
    friend class QGraphicsItemPrivate;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGraphicsView::CacheMode)
Q_DECLARE_OPERATORS_FOR_FLAGS(QGraphicsView::OptimizationFlags)

inline void QGraphicsView::setSceneRect(qreal ax, qreal ay, qreal aw, qreal ah)
{ setSceneRect(QRectF(ax, ay, aw, ah)); }
inline void QGraphicsView::centerOn(qreal ax, qreal ay)
{ centerOn(QPointF(ax, ay)); }
inline void QGraphicsView::ensureVisible(qreal ax, qreal ay, qreal aw, qreal ah, int xmargin, int ymargin)
{ ensureVisible(QRectF(ax, ay, aw, ah), xmargin, ymargin); }
inline void QGraphicsView::fitInView(qreal ax, qreal ay, qreal w, qreal h, Qt::AspectRatioMode mode)
{ fitInView(QRectF(ax, ay, w, h), mode); }
inline QList<QGraphicsItem *> QGraphicsView::items(int ax, int ay) const
{ return items(QPoint(ax, ay)); }
inline QList<QGraphicsItem *> QGraphicsView::items(int ax, int ay, int w, int h, Qt::ItemSelectionMode mode) const
{ return items(QRect(ax, ay, w, h), mode); }
inline QGraphicsItem *QGraphicsView::itemAt(int ax, int ay) const
{ return itemAt(QPoint(ax, ay)); }
inline QPointF QGraphicsView::mapToScene(int ax, int ay) const
{ return mapToScene(QPoint(ax, ay)); }
inline QPolygonF QGraphicsView::mapToScene(int ax, int ay, int w, int h) const
{ return mapToScene(QRect(ax, ay, w, h)); }
inline QPoint QGraphicsView::mapFromScene(qreal ax, qreal ay) const
{ return mapFromScene(QPointF(ax, ay)); }
inline QPolygon QGraphicsView::mapFromScene(qreal ax, qreal ay, qreal w, qreal h) const
{ return mapFromScene(QRectF(ax, ay, w, h)); }

QT_END_NAMESPACE

#endif // QGRAPHICSVIEW_H
