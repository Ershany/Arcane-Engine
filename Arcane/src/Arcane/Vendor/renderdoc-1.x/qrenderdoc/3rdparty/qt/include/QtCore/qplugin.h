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

#ifndef QPLUGIN_H
#define QPLUGIN_H

#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>
#include <QtCore/qjsonobject.h>

QT_BEGIN_NAMESPACE


#ifndef Q_EXTERN_C
#  ifdef __cplusplus
#    define Q_EXTERN_C extern "C"
#  else
#    define Q_EXTERN_C extern
#  endif
#endif

typedef QObject *(*QtPluginInstanceFunction)();
typedef const char *(*QtPluginMetaDataFunction)();

struct Q_CORE_EXPORT QStaticPlugin
{
    // Note: This struct is initialized using an initializer list.
    // As such, it cannot have any new constructors or variables.
#ifndef Q_QDOC
    QtPluginInstanceFunction instance;
    QtPluginMetaDataFunction rawMetaData;
#else
    // Since qdoc gets confused by the use of function
    // pointers, we add these dummes for it to parse instead:
    QObject *instance();
    const char *rawMetaData();
#endif
    QJsonObject metaData() const;
};
Q_DECLARE_TYPEINFO(QStaticPlugin, Q_PRIMITIVE_TYPE);

void Q_CORE_EXPORT qRegisterStaticPluginFunction(QStaticPlugin staticPlugin);

#if (defined(Q_OF_ELF) || defined(Q_OS_WIN)) && (defined (Q_CC_GNU) || defined(Q_CC_CLANG))
#  define QT_PLUGIN_METADATA_SECTION \
    __attribute__ ((section (".qtmetadata"))) __attribute__((used))
#elif defined(Q_OS_MAC)
// TODO: Implement section parsing on Mac
#  define QT_PLUGIN_METADATA_SECTION \
    __attribute__ ((section ("__TEXT,qtmetadata"))) __attribute__((used))
#elif defined(Q_CC_MSVC)
// TODO: Implement section parsing for MSVC
#pragma section(".qtmetadata",read,shared)
#  define QT_PLUGIN_METADATA_SECTION \
    __declspec(allocate(".qtmetadata"))
#else
#  define QT_PLUGIN_VERIFICATION_SECTION
#  define QT_PLUGIN_METADATA_SECTION
#endif


#define Q_IMPORT_PLUGIN(PLUGIN) \
        extern const QT_PREPEND_NAMESPACE(QStaticPlugin) qt_static_plugin_##PLUGIN(); \
        class Static##PLUGIN##PluginInstance{ \
        public: \
                Static##PLUGIN##PluginInstance() { \
                    qRegisterStaticPluginFunction(qt_static_plugin_##PLUGIN()); \
                } \
        }; \
       static Static##PLUGIN##PluginInstance static##PLUGIN##Instance;

#define Q_PLUGIN_INSTANCE(IMPLEMENTATION) \
        { \
            static QT_PREPEND_NAMESPACE(QPointer)<QT_PREPEND_NAMESPACE(QObject)> _instance; \
            if (!_instance)      \
                _instance = new IMPLEMENTATION; \
            return _instance; \
        }

#if defined(QT_STATICPLUGIN)

#  define QT_MOC_EXPORT_PLUGIN(PLUGINCLASS, PLUGINCLASSNAME) \
    static QT_PREPEND_NAMESPACE(QObject) *qt_plugin_instance_##PLUGINCLASSNAME() \
    Q_PLUGIN_INSTANCE(PLUGINCLASS) \
    static const char *qt_plugin_query_metadata_##PLUGINCLASSNAME() { return reinterpret_cast<const char *>(qt_pluginMetaData); } \
    const QT_PREPEND_NAMESPACE(QStaticPlugin) qt_static_plugin_##PLUGINCLASSNAME() { \
        QT_PREPEND_NAMESPACE(QStaticPlugin) plugin = { qt_plugin_instance_##PLUGINCLASSNAME, qt_plugin_query_metadata_##PLUGINCLASSNAME}; \
        return plugin; \
    }

#else

#  define QT_MOC_EXPORT_PLUGIN(PLUGINCLASS, PLUGINCLASSNAME)      \
            Q_EXTERN_C Q_DECL_EXPORT \
            const char *qt_plugin_query_metadata() \
            { return reinterpret_cast<const char *>(qt_pluginMetaData); } \
            Q_EXTERN_C Q_DECL_EXPORT QT_PREPEND_NAMESPACE(QObject) *qt_plugin_instance() \
            Q_PLUGIN_INSTANCE(PLUGINCLASS)

#endif


#define Q_EXPORT_PLUGIN(PLUGIN) \
            Q_EXPORT_PLUGIN2(PLUGIN, PLUGIN)
#  define Q_EXPORT_PLUGIN2(PLUGIN, PLUGINCLASS)      \
    Q_STATIC_ASSERT_X(false, "Old plugin system used")

#  define Q_EXPORT_STATIC_PLUGIN2(PLUGIN, PLUGINCLASS) \
    Q_STATIC_ASSERT_X(false, "Old plugin system used")


QT_END_NAMESPACE

#endif // Q_PLUGIN_H
