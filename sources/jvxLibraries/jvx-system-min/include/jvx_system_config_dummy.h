#ifndef __JVX_SYSTEM_CONFIG_H__
#define __JVX_SYSTEM_CONFIG_H__

#ifndef JVX_SYSTEM_USE_DATA_FORMAT_FLOAT
/* #undef JVX_SYSTEM_USE_DATA_FORMAT_FLOAT */
#endif

#define JVX_IDENT_STRING "jvx-rt-750619"
#define JVX_VERSION_STRING "dummy-hash"
#define JVX_DATA_STRING "dummy-date"

#ifdef JVX_SYSTEM_USE_DATA_FORMAT_FLOAT
#define JVX_DATA_FORMAT_FLOAT
#else
#define JVX_DATA_FORMAT_DOUBLE
#endif


#endif
