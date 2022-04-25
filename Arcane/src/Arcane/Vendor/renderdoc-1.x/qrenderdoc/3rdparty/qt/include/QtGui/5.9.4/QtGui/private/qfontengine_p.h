/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QFONTENGINE_P_H
#define QFONTENGINE_P_H

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

#include <QtGui/private/qtguiglobal_p.h>
#include "QtCore/qatomic.h"
#include <QtCore/qvarlengtharray.h>
#include <QtCore/QLinkedList>
#include <QtCore/qhashfunctions.h>
#include "private/qtextengine_p.h"
#include "private/qfont_p.h"

QT_BEGIN_NAMESPACE

class QPainterPath;
class QFontEngineGlyphCache;

struct QGlyphLayout;

#define MAKE_TAG(ch1, ch2, ch3, ch4) (\
    (((quint32)(ch1)) << 24) | \
    (((quint32)(ch2)) << 16) | \
    (((quint32)(ch3)) << 8) | \
    ((quint32)(ch4)) \
   )

// ### this only used in getPointInOutline(), refactor it and then remove these magic numbers
enum HB_Compat_Error {
    Err_Ok                           = 0x0000,
    Err_Not_Covered                  = 0xFFFF,
    Err_Invalid_Argument             = 0x1A66,
    Err_Invalid_SubTable_Format      = 0x157F,
    Err_Invalid_SubTable             = 0x1570
};

typedef void (*qt_destroy_func_t) (void *user_data);
typedef bool (*qt_get_font_table_func_t) (void *user_data, uint tag, uchar *buffer, uint *length);

class Q_GUI_EXPORT QFontEngine
{
public:
    enum Type {
        Box,
        Multi,

        // MS Windows types
        Win,

        // Apple Mac OS types
        Mac,

        // QWS types
        Freetype,
        QPF1,
        QPF2,
        Proxy,

        DirectWrite,

        TestFontEngine = 0x1000
    };

    enum GlyphFormat {
        Format_None,
        Format_Render = Format_None,
        Format_Mono,
        Format_A8,
        Format_A32,
        Format_ARGB
    };

    enum ShaperFlag {
        DesignMetrics = 0x0002,
        GlyphIndicesOnly = 0x0004
    };
    Q_DECLARE_FLAGS(ShaperFlags, ShaperFlag)

    virtual ~QFontEngine();

    inline Type type() const { return m_type; }

    // all of these are in unscaled metrics if the engine supports uncsaled metrics,
    // otherwise in design metrics
    struct Properties {
        QByteArray postscriptName;
        QByteArray copyright;
        QRectF boundingBox;
        QFixed emSquare;
        QFixed ascent;
        QFixed descent;
        QFixed leading;
        QFixed italicAngle;
        QFixed capHeight;
        QFixed lineWidth;
    };
    virtual Properties properties() const;
    virtual void getUnscaledGlyph(glyph_t glyph, QPainterPath *path, glyph_metrics_t *metrics);
    QByteArray getSfntTable(uint tag) const;
    virtual bool getSfntTableData(uint tag, uchar *buffer, uint *length) const;

    struct FaceId {
        FaceId() : index(0), encoding(0) {}
        QByteArray filename;
        QByteArray uuid;
        int index;
        int encoding;
    };
    virtual FaceId faceId() const { return FaceId(); }
    enum SynthesizedFlags {
        SynthesizedItalic = 0x1,
        SynthesizedBold = 0x2,
        SynthesizedStretch = 0x4
    };
    virtual int synthesized() const { return 0; }
    virtual bool supportsSubPixelPositions() const { return false; }
    virtual QFixed subPixelPositionForX(QFixed x) const;

    virtual QFixed emSquareSize() const { return ascent(); }

    /* returns 0 as glyph index for non existent glyphs */
    virtual glyph_t glyphIndex(uint ucs4) const = 0;
    virtual bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs, ShaperFlags flags) const = 0;
    virtual void recalcAdvances(QGlyphLayout *, ShaperFlags) const {}
    virtual void doKerning(QGlyphLayout *, ShaperFlags) const;

    virtual void addGlyphsToPath(glyph_t *glyphs, QFixedPoint *positions, int nglyphs,
                                 QPainterPath *path, QTextItem::RenderFlags flags);

    void getGlyphPositions(const QGlyphLayout &glyphs, const QTransform &matrix, QTextItem::RenderFlags flags,
                           QVarLengthArray<glyph_t> &glyphs_out, QVarLengthArray<QFixedPoint> &positions);

    virtual void addOutlineToPath(qreal, qreal, const QGlyphLayout &, QPainterPath *, QTextItem::RenderFlags flags);
    void addBitmapFontToPath(qreal x, qreal y, const QGlyphLayout &, QPainterPath *, QTextItem::RenderFlags);
    /**
     * Create a qimage with the alpha values for the glyph.
     * Returns an image indexed_8 with index values ranging from 0=fully transparent to 255=opaque
     */
    // ### Refactor this into a smaller and more flexible API.
    virtual QImage alphaMapForGlyph(glyph_t);
    virtual QImage alphaMapForGlyph(glyph_t glyph, QFixed subPixelPosition);
    virtual QImage alphaMapForGlyph(glyph_t, const QTransform &t);
    virtual QImage alphaMapForGlyph(glyph_t, QFixed subPixelPosition, const QTransform &t);
    virtual QImage alphaRGBMapForGlyph(glyph_t, QFixed subPixelPosition, const QTransform &t);
    virtual QImage bitmapForGlyph(glyph_t, QFixed subPixelPosition, const QTransform &t);
    virtual QImage *lockedAlphaMapForGlyph(glyph_t glyph, QFixed subPixelPosition,
                                           GlyphFormat neededFormat,
                                           const QTransform &t = QTransform(),
                                           QPoint *offset = 0);
    virtual void unlockAlphaMapForGlyph();
    virtual bool hasInternalCaching() const { return false; }

    virtual glyph_metrics_t alphaMapBoundingBox(glyph_t glyph, QFixed /*subPixelPosition*/, const QTransform &matrix, GlyphFormat /*format*/)
    {
        return boundingBox(glyph, matrix);
    }

    virtual void removeGlyphFromCache(glyph_t);

    virtual glyph_metrics_t boundingBox(const QGlyphLayout &glyphs) = 0;
    virtual glyph_metrics_t boundingBox(glyph_t glyph) = 0;
    virtual glyph_metrics_t boundingBox(glyph_t glyph, const QTransform &matrix);
    glyph_metrics_t tightBoundingBox(const QGlyphLayout &glyphs);

    virtual QFixed ascent() const = 0;
    virtual QFixed capHeight() const = 0;
    virtual QFixed descent() const = 0;
    virtual QFixed leading() const = 0;
    virtual QFixed xHeight() const;
    virtual QFixed averageCharWidth() const;

    virtual QFixed lineThickness() const;
    virtual QFixed underlinePosition() const;

    virtual qreal maxCharWidth() const = 0;
    virtual qreal minLeftBearing() const;
    virtual qreal minRightBearing() const;

    virtual void getGlyphBearings(glyph_t glyph, qreal *leftBearing = 0, qreal *rightBearing = 0);

    inline bool canRender(uint ucs4) const { return glyphIndex(ucs4) != 0; }
    virtual bool canRender(const QChar *str, int len) const;

    virtual bool supportsTransformation(const QTransform &transform) const;

    virtual int glyphCount() const;
    virtual int glyphMargin(GlyphFormat format) { return format == Format_A32 ? 2 : 0; }

    virtual QFontEngine *cloneWithSize(qreal /*pixelSize*/) const { return 0; }

    virtual Qt::HANDLE handle() const;

    void *harfbuzzFont() const;
    void *harfbuzzFace() const;
    bool supportsScript(QChar::Script script) const;

    virtual int getPointInOutline(glyph_t glyph, int flags, quint32 point, QFixed *xpos, QFixed *ypos, quint32 *nPoints);

    void clearGlyphCache(const void *key);
    void setGlyphCache(const void *key, QFontEngineGlyphCache *data);
    QFontEngineGlyphCache *glyphCache(const void *key, GlyphFormat format, const QTransform &transform) const;

    static const uchar *getCMap(const uchar *table, uint tableSize, bool *isSymbolFont, int *cmapSize);
    static quint32 getTrueTypeGlyphIndex(const uchar *cmap, int cmapSize, uint unicode);

    static QByteArray convertToPostscriptFontFamilyName(const QByteArray &fontFamily);

    virtual bool hasUnreliableGlyphOutline() const;
    virtual bool expectsGammaCorrectedBlending() const;

    enum HintStyle {
        HintNone,
        HintLight,
        HintMedium,
        HintFull
    };
    virtual void setDefaultHintStyle(HintStyle) { }

    enum SubpixelAntialiasingType {
        Subpixel_None,
        Subpixel_RGB,
        Subpixel_BGR,
        Subpixel_VRGB,
        Subpixel_VBGR
    };

private:
    const Type m_type;

public:
    QAtomicInt ref;
    QFontDef fontDef;

    class Holder { // replace by std::unique_ptr once available
        void *ptr;
        qt_destroy_func_t destroy_func;
    public:
        Holder() : ptr(nullptr), destroy_func(nullptr) {}
        explicit Holder(void *p, qt_destroy_func_t d) : ptr(p), destroy_func(d) {}
        ~Holder() { if (ptr && destroy_func) destroy_func(ptr); }
        Holder(Holder &&other) Q_DECL_NOTHROW
            : ptr(other.ptr),
              destroy_func(other.destroy_func)
        {
            other.ptr = nullptr;
            other.destroy_func = nullptr;
        }
        Holder &operator=(Holder &&other) Q_DECL_NOTHROW
        { swap(other); return *this; }

        void swap(Holder &other) Q_DECL_NOTHROW
        {
            qSwap(ptr, other.ptr);
            qSwap(destroy_func, other.destroy_func);
        }

        void *get() const Q_DECL_NOTHROW { return ptr; }
        void *release() Q_DECL_NOTHROW {
            void *result = ptr;
            ptr = nullptr;
            destroy_func = nullptr;
            return result;
        }
        void reset() Q_DECL_NOTHROW { Holder().swap(*this); }
        qt_destroy_func_t get_deleter() const Q_DECL_NOTHROW { return destroy_func; }

        bool operator!() const Q_DECL_NOTHROW { return !ptr; }
    };

    mutable Holder font_; // \ NOTE: Declared before m_glyphCaches, so font_, face_
    mutable Holder face_; // / are destroyed _after_ m_glyphCaches is destroyed.

    struct FaceData {
        void *user_data;
        qt_get_font_table_func_t get_font_table;
    } faceData;

    uint cache_cost; // amount of mem used in bytes by the font
    uint fsType : 16;
    bool symbol;
    bool isSmoothlyScalable;
    struct KernPair {
        uint left_right;
        QFixed adjust;

        inline bool operator<(const KernPair &other) const
        {
            return left_right < other.left_right;
        }
    };
    QVector<KernPair> kerning_pairs;
    void loadKerningPairs(QFixed scalingFactor);

    GlyphFormat glyphFormat;
    QImage currentlyLockedAlphaMap;
    int m_subPixelPositionCount; // Number of positions within a single pixel for this cache

    inline QVariant userData() const { return m_userData; }

protected:
    explicit QFontEngine(Type type);

    QFixed lastRightBearing(const QGlyphLayout &glyphs, bool round = false);

    inline void setUserData(const QVariant &userData) { m_userData = userData; }
    QFixed calculatedCapHeight() const;

private:
    struct GlyphCacheEntry {
        GlyphCacheEntry();
        GlyphCacheEntry(const GlyphCacheEntry &);
        ~GlyphCacheEntry();

        GlyphCacheEntry &operator=(const GlyphCacheEntry &);

        QExplicitlySharedDataPointer<QFontEngineGlyphCache> cache;
        bool operator==(const GlyphCacheEntry &other) const { return cache == other.cache; }
    };
    typedef QLinkedList<GlyphCacheEntry> GlyphCaches;
    mutable QHash<const void *, GlyphCaches> m_glyphCaches;

private:
    QVariant m_userData;

    mutable qreal m_minLeftBearing;
    mutable qreal m_minRightBearing;

};
Q_DECLARE_TYPEINFO(QFontEngine::KernPair, Q_PRIMITIVE_TYPE);

Q_DECLARE_OPERATORS_FOR_FLAGS(QFontEngine::ShaperFlags)

inline bool operator ==(const QFontEngine::FaceId &f1, const QFontEngine::FaceId &f2)
{
    return f1.index == f2.index && f1.encoding == f2.encoding && f1.filename == f2.filename && f1.uuid == f2.uuid;
}

inline uint qHash(const QFontEngine::FaceId &f, uint seed = 0)
    Q_DECL_NOEXCEPT_EXPR(noexcept(qHash(f.filename)))
{
    QtPrivate::QHashCombine hash;
    seed = hash(seed, f.filename);
    seed = hash(seed, f.uuid);
    seed = hash(seed, f.index);
    seed = hash(seed, f.encoding);
    return seed;
}


class QGlyph;



class QFontEngineBox : public QFontEngine
{
public:
    QFontEngineBox(int size);
    ~QFontEngineBox();

    virtual glyph_t glyphIndex(uint ucs4) const Q_DECL_OVERRIDE;
    virtual bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs, ShaperFlags flags) const Q_DECL_OVERRIDE;
    virtual void recalcAdvances(QGlyphLayout *, ShaperFlags) const Q_DECL_OVERRIDE;

    void draw(QPaintEngine *p, qreal x, qreal y, const QTextItemInt &si);
    virtual void addOutlineToPath(qreal x, qreal y, const QGlyphLayout &glyphs, QPainterPath *path, QTextItem::RenderFlags flags) Q_DECL_OVERRIDE;

    virtual glyph_metrics_t boundingBox(const QGlyphLayout &glyphs) Q_DECL_OVERRIDE;
    virtual glyph_metrics_t boundingBox(glyph_t glyph) Q_DECL_OVERRIDE;
    virtual QFontEngine *cloneWithSize(qreal pixelSize) const Q_DECL_OVERRIDE;

    virtual QFixed ascent() const Q_DECL_OVERRIDE;
    virtual QFixed capHeight() const Q_DECL_OVERRIDE;
    virtual QFixed descent() const Q_DECL_OVERRIDE;
    virtual QFixed leading() const Q_DECL_OVERRIDE;
    virtual qreal maxCharWidth() const Q_DECL_OVERRIDE;
    virtual qreal minLeftBearing() const Q_DECL_OVERRIDE { return 0; }
    virtual qreal minRightBearing() const Q_DECL_OVERRIDE { return 0; }
    virtual QImage alphaMapForGlyph(glyph_t) Q_DECL_OVERRIDE;

    virtual bool canRender(const QChar *string, int len) const Q_DECL_OVERRIDE;

    inline int size() const { return _size; }

protected:
    explicit QFontEngineBox(Type type, int size);

private:
    friend class QFontPrivate;
    int _size;
};

class Q_GUI_EXPORT QFontEngineMulti : public QFontEngine
{
public:
    explicit QFontEngineMulti(QFontEngine *engine, int script, const QStringList &fallbackFamilies = QStringList());
    ~QFontEngineMulti();

    virtual glyph_t glyphIndex(uint ucs4) const Q_DECL_OVERRIDE;
    virtual bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs, ShaperFlags flags) const Q_DECL_OVERRIDE;

    virtual glyph_metrics_t boundingBox(const QGlyphLayout &glyphs) Q_DECL_OVERRIDE;
    virtual glyph_metrics_t boundingBox(glyph_t glyph) Q_DECL_OVERRIDE;

    virtual void recalcAdvances(QGlyphLayout *, ShaperFlags) const Q_DECL_OVERRIDE;
    virtual void doKerning(QGlyphLayout *, ShaperFlags) const Q_DECL_OVERRIDE;
    virtual void addOutlineToPath(qreal, qreal, const QGlyphLayout &, QPainterPath *, QTextItem::RenderFlags flags) Q_DECL_OVERRIDE;
    virtual void getGlyphBearings(glyph_t glyph, qreal *leftBearing = 0, qreal *rightBearing = 0) Q_DECL_OVERRIDE;

    virtual QFixed ascent() const Q_DECL_OVERRIDE;
    virtual QFixed capHeight() const Q_DECL_OVERRIDE;
    virtual QFixed descent() const Q_DECL_OVERRIDE;
    virtual QFixed leading() const Q_DECL_OVERRIDE;
    virtual QFixed xHeight() const Q_DECL_OVERRIDE;
    virtual QFixed averageCharWidth() const Q_DECL_OVERRIDE;
    virtual QImage alphaMapForGlyph(glyph_t) Q_DECL_OVERRIDE;
    virtual QImage alphaMapForGlyph(glyph_t glyph, QFixed subPixelPosition) Q_DECL_OVERRIDE;
    virtual QImage alphaMapForGlyph(glyph_t, const QTransform &t) Q_DECL_OVERRIDE;
    virtual QImage alphaMapForGlyph(glyph_t, QFixed subPixelPosition, const QTransform &t) Q_DECL_OVERRIDE;
    virtual QImage alphaRGBMapForGlyph(glyph_t, QFixed subPixelPosition, const QTransform &t) Q_DECL_OVERRIDE;

    virtual QFixed lineThickness() const Q_DECL_OVERRIDE;
    virtual QFixed underlinePosition() const Q_DECL_OVERRIDE;
    virtual qreal maxCharWidth() const Q_DECL_OVERRIDE;
    virtual qreal minLeftBearing() const Q_DECL_OVERRIDE;
    virtual qreal minRightBearing() const Q_DECL_OVERRIDE;

    virtual bool canRender(const QChar *string, int len) const Q_DECL_OVERRIDE;

    inline int fallbackFamilyCount() const { return m_fallbackFamilies.size(); }
    inline QString fallbackFamilyAt(int at) const { return m_fallbackFamilies.at(at); }

    void setFallbackFamiliesList(const QStringList &fallbackFamilies);

    inline QFontEngine *engine(int at) const
    { Q_ASSERT(at < m_engines.size()); return m_engines.at(at); }

    void ensureEngineAt(int at);

    static QFontEngine *createMultiFontEngine(QFontEngine *fe, int script);

protected:
    virtual void ensureFallbackFamiliesQueried();
    virtual bool shouldLoadFontEngineForCharacter(int at, uint ucs4) const;
    virtual QFontEngine *loadEngine(int at);

private:
    QVector<QFontEngine *> m_engines;
    QStringList m_fallbackFamilies;
    const int m_script;
    bool m_fallbackFamiliesQueried;
};

class QTestFontEngine : public QFontEngineBox
{
public:
    QTestFontEngine(int size);
};

QT_END_NAMESPACE



#endif // QFONTENGINE_P_H
