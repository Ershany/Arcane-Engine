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

#ifndef QDRAWINGPRIMITIVE_SSE2_P_H
#define QDRAWINGPRIMITIVE_SSE2_P_H

#include <QtGui/private/qtguiglobal_p.h>
#include <private/qsimd_p.h>
#include "qdrawhelper_p.h"

#ifdef __SSE2__

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

QT_BEGIN_NAMESPACE

/*
 * Multiply the components of pixelVector by alphaChannel
 * Each 32bits components of alphaChannel must be in the form 0x00AA00AA
 * colorMask must have 0x00ff00ff on each 32 bits component
 * half must have the value 128 (0x80) for each 32 bits compnent
 */
#define BYTE_MUL_SSE2(result, pixelVector, alphaChannel, colorMask, half) \
{ \
    /* 1. separate the colors in 2 vectors so each color is on 16 bits \
       (in order to be multiplied by the alpha \
       each 32 bit of dstVectorAG are in the form 0x00AA00GG \
       each 32 bit of dstVectorRB are in the form 0x00RR00BB */\
    __m128i pixelVectorAG = _mm_srli_epi16(pixelVector, 8); \
    __m128i pixelVectorRB = _mm_and_si128(pixelVector, colorMask); \
 \
    /* 2. multiply the vectors by the alpha channel */\
    pixelVectorAG = _mm_mullo_epi16(pixelVectorAG, alphaChannel); \
    pixelVectorRB = _mm_mullo_epi16(pixelVectorRB, alphaChannel); \
 \
    /* 3. divide by 255, that's the tricky part. \
       we do it like for BYTE_MUL(), with bit shift: X/255 ~= (X + X/256 + rounding)/256 */ \
    /** so first (X + X/256 + rounding) */\
    pixelVectorRB = _mm_add_epi16(pixelVectorRB, _mm_srli_epi16(pixelVectorRB, 8)); \
    pixelVectorRB = _mm_add_epi16(pixelVectorRB, half); \
    pixelVectorAG = _mm_add_epi16(pixelVectorAG, _mm_srli_epi16(pixelVectorAG, 8)); \
    pixelVectorAG = _mm_add_epi16(pixelVectorAG, half); \
 \
    /** second divide by 256 */\
    pixelVectorRB = _mm_srli_epi16(pixelVectorRB, 8); \
    /** for AG, we could >> 8 to divide followed by << 8 to put the \
        bytes in the correct position. By masking instead, we execute \
        only one instruction */\
    pixelVectorAG = _mm_andnot_si128(colorMask, pixelVectorAG); \
 \
    /* 4. combine the 2 pairs of colors */ \
    result = _mm_or_si128(pixelVectorAG, pixelVectorRB); \
}

/*
 * Each 32bits components of alphaChannel must be in the form 0x00AA00AA
 * oneMinusAlphaChannel must be 255 - alpha for each 32 bits component
 * colorMask must have 0x00ff00ff on each 32 bits component
 * half must have the value 128 (0x80) for each 32 bits compnent
 */
#define INTERPOLATE_PIXEL_255_SSE2(result, srcVector, dstVector, alphaChannel, oneMinusAlphaChannel, colorMask, half) { \
    /* interpolate AG */\
    __m128i srcVectorAG = _mm_srli_epi16(srcVector, 8); \
    __m128i dstVectorAG = _mm_srli_epi16(dstVector, 8); \
    __m128i srcVectorAGalpha = _mm_mullo_epi16(srcVectorAG, alphaChannel); \
    __m128i dstVectorAGoneMinusAlphalpha = _mm_mullo_epi16(dstVectorAG, oneMinusAlphaChannel); \
    __m128i finalAG = _mm_add_epi16(srcVectorAGalpha, dstVectorAGoneMinusAlphalpha); \
    finalAG = _mm_add_epi16(finalAG, _mm_srli_epi16(finalAG, 8)); \
    finalAG = _mm_add_epi16(finalAG, half); \
    finalAG = _mm_andnot_si128(colorMask, finalAG); \
 \
    /* interpolate RB */\
    __m128i srcVectorRB = _mm_and_si128(srcVector, colorMask); \
    __m128i dstVectorRB = _mm_and_si128(dstVector, colorMask); \
    __m128i srcVectorRBalpha = _mm_mullo_epi16(srcVectorRB, alphaChannel); \
    __m128i dstVectorRBoneMinusAlphalpha = _mm_mullo_epi16(dstVectorRB, oneMinusAlphaChannel); \
    __m128i finalRB = _mm_add_epi16(srcVectorRBalpha, dstVectorRBoneMinusAlphalpha); \
    finalRB = _mm_add_epi16(finalRB, _mm_srli_epi16(finalRB, 8)); \
    finalRB = _mm_add_epi16(finalRB, half); \
    finalRB = _mm_srli_epi16(finalRB, 8); \
 \
    /* combine */\
    result = _mm_or_si128(finalAG, finalRB); \
}

// same as BLEND_SOURCE_OVER_ARGB32_SSE2, but for one vector srcVector
#define BLEND_SOURCE_OVER_ARGB32_SSE2_helper(dst, srcVector, nullVector, half, one, colorMask, alphaMask) { \
        const __m128i srcVectorAlpha = _mm_and_si128(srcVector, alphaMask); \
        if (_mm_movemask_epi8(_mm_cmpeq_epi32(srcVectorAlpha, alphaMask)) == 0xffff) { \
            /* all opaque */ \
            _mm_store_si128((__m128i *)&dst[x], srcVector); \
        } else if (_mm_movemask_epi8(_mm_cmpeq_epi32(srcVectorAlpha, nullVector)) != 0xffff) { \
            /* not fully transparent */ \
            /* extract the alpha channel on 2 x 16 bits */ \
            /* so we have room for the multiplication */ \
            /* each 32 bits will be in the form 0x00AA00AA */ \
            /* with A being the 1 - alpha */ \
            __m128i alphaChannel = _mm_srli_epi32(srcVector, 24); \
            alphaChannel = _mm_or_si128(alphaChannel, _mm_slli_epi32(alphaChannel, 16)); \
            alphaChannel = _mm_sub_epi16(one, alphaChannel); \
 \
            const __m128i dstVector = _mm_load_si128((__m128i *)&dst[x]); \
            __m128i destMultipliedByOneMinusAlpha; \
            BYTE_MUL_SSE2(destMultipliedByOneMinusAlpha, dstVector, alphaChannel, colorMask, half); \
 \
            /* result = s + d * (1-alpha) */\
            const __m128i result = _mm_add_epi8(srcVector, destMultipliedByOneMinusAlpha); \
            _mm_store_si128((__m128i *)&dst[x], result); \
        } \
    }


// Basically blend src over dst with the const alpha defined as constAlphaVector.
// nullVector, half, one, colorMask are constant across the whole image/texture, and should be defined as:
//const __m128i nullVector = _mm_set1_epi32(0);
//const __m128i half = _mm_set1_epi16(0x80);
//const __m128i one = _mm_set1_epi16(0xff);
//const __m128i colorMask = _mm_set1_epi32(0x00ff00ff);
//const __m128i alphaMask = _mm_set1_epi32(0xff000000);
//
// The computation being done is:
// result = s + d * (1-alpha)
// with shortcuts if fully opaque or fully transparent.
#define BLEND_SOURCE_OVER_ARGB32_SSE2(dst, src, length, nullVector, half, one, colorMask, alphaMask) { \
    int x = 0; \
\
    /* First, get dst aligned. */ \
    ALIGNMENT_PROLOGUE_16BYTES(dst, x, length) { \
        blend_pixel(dst[x], src[x]); \
    } \
\
    for (; x < length-3; x += 4) { \
        const __m128i srcVector = _mm_loadu_si128((const __m128i *)&src[x]); \
        BLEND_SOURCE_OVER_ARGB32_SSE2_helper(dst, srcVector, nullVector, half, one, colorMask, alphaMask) \
    } \
    SIMD_EPILOGUE(x, length, 3) { \
        blend_pixel(dst[x], src[x]); \
    } \
}

// Basically blend src over dst with the const alpha defined as constAlphaVector.
// nullVector, half, one, colorMask are constant across the whole image/texture, and should be defined as:
//const __m128i nullVector = _mm_set1_epi32(0);
//const __m128i half = _mm_set1_epi16(0x80);
//const __m128i one = _mm_set1_epi16(0xff);
//const __m128i colorMask = _mm_set1_epi32(0x00ff00ff);
//
// The computation being done is:
// dest = (s + d * sia) * ca + d * cia
//      = s * ca + d * (sia * ca + cia)
//      = s * ca + d * (1 - sa*ca)
#define BLEND_SOURCE_OVER_ARGB32_WITH_CONST_ALPHA_SSE2(dst, src, length, nullVector, half, one, colorMask, constAlphaVector) \
{ \
    int x = 0; \
\
    ALIGNMENT_PROLOGUE_16BYTES(dst, x, length) { \
        blend_pixel(dst[x], src[x], const_alpha); \
    } \
\
    for (; x < length-3; x += 4) { \
        __m128i srcVector = _mm_loadu_si128((const __m128i *)&src[x]); \
        if (_mm_movemask_epi8(_mm_cmpeq_epi32(srcVector, nullVector)) != 0xffff) { \
            BYTE_MUL_SSE2(srcVector, srcVector, constAlphaVector, colorMask, half); \
\
            __m128i alphaChannel = _mm_srli_epi32(srcVector, 24); \
            alphaChannel = _mm_or_si128(alphaChannel, _mm_slli_epi32(alphaChannel, 16)); \
            alphaChannel = _mm_sub_epi16(one, alphaChannel); \
 \
            const __m128i dstVector = _mm_load_si128((__m128i *)&dst[x]); \
            __m128i destMultipliedByOneMinusAlpha; \
            BYTE_MUL_SSE2(destMultipliedByOneMinusAlpha, dstVector, alphaChannel, colorMask, half); \
 \
            const __m128i result = _mm_add_epi8(srcVector, destMultipliedByOneMinusAlpha); \
            _mm_store_si128((__m128i *)&dst[x], result); \
        } \
    } \
    SIMD_EPILOGUE(x, length, 3) { \
        blend_pixel(dst[x], src[x], const_alpha); \
    } \
}

QT_END_NAMESPACE

#endif // __SSE2__

QT_BEGIN_NAMESPACE
#if QT_COMPILER_SUPPORTS_HERE(SSE4_1)
QT_FUNCTION_TARGET(SSE4_1)
inline QRgb qUnpremultiply_sse4(QRgb p)
{
    const uint alpha = qAlpha(p);
    if (alpha == 255 || alpha == 0)
        return p;
    const uint invAlpha = qt_inv_premul_factor[alpha];
    const __m128i via = _mm_set1_epi32(invAlpha);
    const __m128i vr = _mm_set1_epi32(0x8000);
    __m128i vl = _mm_cvtepu8_epi32(_mm_cvtsi32_si128(p));
    vl = _mm_mullo_epi32(vl, via);
    vl = _mm_add_epi32(vl, vr);
    vl = _mm_srai_epi32(vl, 16);
    vl = _mm_insert_epi32(vl, alpha, 3);
    vl = _mm_packus_epi32(vl, vl);
    vl = _mm_packus_epi16(vl, vl);
    return _mm_cvtsi128_si32(vl);
}

template<enum QtPixelOrder PixelOrder>
QT_FUNCTION_TARGET(SSE4_1)
inline uint qConvertArgb32ToA2rgb30_sse4(QRgb p)
{
    const uint alpha = qAlpha(p);
    if (alpha == 255)
        return qConvertRgb32ToRgb30<PixelOrder>(p);
    if (alpha == 0)
        return 0;
    Q_CONSTEXPR  uint mult = 255 / (255 >> 6);
    const uint invAlpha = qt_inv_premul_factor[alpha];
    const uint newalpha = (alpha >> 6);
    const __m128i via = _mm_set1_epi32(invAlpha);
    const __m128i vna = _mm_set1_epi32(mult * newalpha);
    const __m128i vr1 = _mm_set1_epi32(0x1000);
    const __m128i vr2 = _mm_set1_epi32(0x80);
    __m128i vl = _mm_cvtepu8_epi32(_mm_cvtsi32_si128(p));
    vl = _mm_mullo_epi32(vl, via);
    vl = _mm_add_epi32(vl, vr1);
    vl = _mm_srli_epi32(vl, 14);
    vl = _mm_mullo_epi32(vl, vna);
    vl = _mm_add_epi32(vl, _mm_srli_epi32(vl, 8));
    vl = _mm_add_epi32(vl, vr2);
    vl = _mm_srli_epi32(vl, 8);
    vl = _mm_packus_epi32(vl, vl);
    uint rgb30 = (newalpha << 30);
    rgb30 |= ((uint)_mm_extract_epi16(vl, 1)) << 10;
    if (PixelOrder == PixelOrderRGB) {
        rgb30 |= ((uint)_mm_extract_epi16(vl, 2)) << 20;
        rgb30 |= ((uint)_mm_extract_epi16(vl, 0));
    } else {
        rgb30 |= ((uint)_mm_extract_epi16(vl, 0)) << 20;
        rgb30 |= ((uint)_mm_extract_epi16(vl, 2));
    }
    return rgb30;
}
#endif
QT_END_NAMESPACE

#endif // QDRAWINGPRIMITIVE_SSE2_P_H
