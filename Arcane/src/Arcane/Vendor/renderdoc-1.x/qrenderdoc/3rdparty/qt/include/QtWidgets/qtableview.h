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

#ifndef QTABLEVIEW_H
#define QTABLEVIEW_H

#include <QtWidgets/qtwidgetsglobal.h>
#include <QtWidgets/qabstractitemview.h>

QT_REQUIRE_CONFIG(tableview);

QT_BEGIN_NAMESPACE

class QHeaderView;
class QTableViewPrivate;

class Q_WIDGETS_EXPORT QTableView : public QAbstractItemView
{
    Q_OBJECT
    Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid)
    Q_PROPERTY(Qt::PenStyle gridStyle READ gridStyle WRITE setGridStyle)
    Q_PROPERTY(bool sortingEnabled READ isSortingEnabled WRITE setSortingEnabled)
    Q_PROPERTY(bool wordWrap READ wordWrap WRITE setWordWrap)
#if QT_CONFIG(abstractbutton)
    Q_PROPERTY(bool cornerButtonEnabled READ isCornerButtonEnabled WRITE setCornerButtonEnabled)
#endif

public:
    explicit QTableView(QWidget *parent = Q_NULLPTR);
    ~QTableView();

    void setModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;
    void setRootIndex(const QModelIndex &index) Q_DECL_OVERRIDE;
    void setSelectionModel(QItemSelectionModel *selectionModel) Q_DECL_OVERRIDE;
    void doItemsLayout() Q_DECL_OVERRIDE;

    QHeaderView *horizontalHeader() const;
    QHeaderView *verticalHeader() const;
    void setHorizontalHeader(QHeaderView *header);
    void setVerticalHeader(QHeaderView *header);

    int rowViewportPosition(int row) const;
    int rowAt(int y) const;

    void setRowHeight(int row, int height);
    int rowHeight(int row) const;

    int columnViewportPosition(int column) const;
    int columnAt(int x) const;

    void setColumnWidth(int column, int width);
    int columnWidth(int column) const;

    bool isRowHidden(int row) const;
    void setRowHidden(int row, bool hide);

    bool isColumnHidden(int column) const;
    void setColumnHidden(int column, bool hide);

    void setSortingEnabled(bool enable);
    bool isSortingEnabled() const;

    bool showGrid() const;

    Qt::PenStyle gridStyle() const;
    void setGridStyle(Qt::PenStyle style);

    void setWordWrap(bool on);
    bool wordWrap() const;

#if QT_CONFIG(abstractbutton)
    void setCornerButtonEnabled(bool enable);
    bool isCornerButtonEnabled() const;
#endif

    QRect visualRect(const QModelIndex &index) const Q_DECL_OVERRIDE;
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) Q_DECL_OVERRIDE;
    QModelIndex indexAt(const QPoint &p) const Q_DECL_OVERRIDE;

    void setSpan(int row, int column, int rowSpan, int columnSpan);
    int rowSpan(int row, int column) const;
    int columnSpan(int row, int column) const;
    void clearSpans();

    void sortByColumn(int column, Qt::SortOrder order);

public Q_SLOTS:
    void selectRow(int row);
    void selectColumn(int column);
    void hideRow(int row);
    void hideColumn(int column);
    void showRow(int row);
    void showColumn(int column);
    void resizeRowToContents(int row);
    void resizeRowsToContents();
    void resizeColumnToContents(int column);
    void resizeColumnsToContents();
    void sortByColumn(int column);
    void setShowGrid(bool show);

protected Q_SLOTS:
    void rowMoved(int row, int oldIndex, int newIndex);
    void columnMoved(int column, int oldIndex, int newIndex);
    void rowResized(int row, int oldHeight, int newHeight);
    void columnResized(int column, int oldWidth, int newWidth);
    void rowCountChanged(int oldCount, int newCount);
    void columnCountChanged(int oldCount, int newCount);

protected:
    QTableView(QTableViewPrivate &, QWidget *parent);
    void scrollContentsBy(int dx, int dy) Q_DECL_OVERRIDE;

    QStyleOptionViewItem viewOptions() const Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;

    int horizontalOffset() const Q_DECL_OVERRIDE;
    int verticalOffset() const Q_DECL_OVERRIDE;
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) Q_DECL_OVERRIDE;

    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command) Q_DECL_OVERRIDE;
    QRegion visualRegionForSelection(const QItemSelection &selection) const Q_DECL_OVERRIDE;
    QModelIndexList selectedIndexes() const Q_DECL_OVERRIDE;

    void updateGeometries() Q_DECL_OVERRIDE;

    QSize viewportSizeHint() const Q_DECL_OVERRIDE;

    int sizeHintForRow(int row) const Q_DECL_OVERRIDE;
    int sizeHintForColumn(int column) const Q_DECL_OVERRIDE;

    void verticalScrollbarAction(int action) Q_DECL_OVERRIDE;
    void horizontalScrollbarAction(int action) Q_DECL_OVERRIDE;

    bool isIndexHidden(const QModelIndex &index) const Q_DECL_OVERRIDE;

    void selectionChanged(const QItemSelection &selected,
                          const QItemSelection &deselected) Q_DECL_OVERRIDE;
    void currentChanged(const QModelIndex &current,
                          const QModelIndex &previous) Q_DECL_OVERRIDE;

private:
    friend class QAccessibleItemView;
    int visualIndex(const QModelIndex &index) const;

    Q_DECLARE_PRIVATE(QTableView)
    Q_DISABLE_COPY(QTableView)
    Q_PRIVATE_SLOT(d_func(), void _q_selectRow(int))
    Q_PRIVATE_SLOT(d_func(), void _q_selectColumn(int))
    Q_PRIVATE_SLOT(d_func(), void _q_updateSpanInsertedRows(QModelIndex,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_updateSpanInsertedColumns(QModelIndex,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_updateSpanRemovedRows(QModelIndex,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_updateSpanRemovedColumns(QModelIndex,int,int))
};

QT_END_NAMESPACE

#endif // QTABLEVIEW_H
