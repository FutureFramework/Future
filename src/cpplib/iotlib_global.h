#ifndef IOTLIB_GLOBAL_H
#define IOTLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MAKE_IOTLIB)
#  define IOTLIB_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define IOTLIB_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // IOTLIB_GLOBAL_H
