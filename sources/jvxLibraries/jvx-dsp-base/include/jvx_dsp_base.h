#ifndef __JVX_BASE_DSP_TYPES_H__
#define __JVX_BASE_DSP_TYPES_H__

#include <float.h>
#include <stdarg.h>
#include <stdio.h>

#include "jvx_system.h"

// =========================================================
// Backward compatibility
// =========================================================

#define JVX_DSP_LIB_BEGIN JVX_SYSTEM_LIB_BEGIN
#define JVX_DSP_LIB_END JVX_SYSTEM_LIB_END

#ifdef JVX_DATA_FORMAT_FLOAT
#define JVX_DSP_DATA_FORMAT_FLOAT
#endif

#ifdef JVX_DATA_FORMAT_DOUBLE
#define JVX_DSP_DATA_FORMAT_DOUBLE 
#endif

#define jvxDspBaseErrorType jvxErrorType

#define JVX_DSP_NO_ERROR JVX_NO_ERROR
#define JVX_DSP_ERROR_UNKNOWN JVX_ERROR_UNKNOWN
#define JVX_DSP_ERROR_INVALID_ARGUMENT JVX_ERROR_INVALID_ARGUMENT
#define JVX_DSP_ERROR_WRONG_STATE JVX_ERROR_WRONG_STATE
#define JVX_DSP_ERROR_ID_OUT_OF_BOUNDS JVX_ERROR_ID_OUT_OF_BOUNDS
#define JVX_DSP_ERROR_UNSUPPORTED JVX_ERROR_UNSUPPORTED
#define JVX_DSP_ERROR_ELEMENT_NOT_FOUND JVX_ERROR_ELEMENT_NOT_FOUND
#define JVX_DSP_ERROR_OPEN_FILE_FAILED JVX_ERROR_OPEN_FILE_FAILED
#define JVX_DSP_ERROR_BUFFER_OVERFLOW JVX_ERROR_BUFFER_OVERFLOW
#define JVX_DSP_ERROR_INTERNAL JVX_ERROR_INTERNAL
#define JVX_DSP_ERROR_CALL_SUB_COMPONENT_FAILED JVX_ERROR_CALL_SUB_COMPONENT_FAILED
#define JVX_DSP_ERROR_SIZE_MISMATCH JVX_ERROR_SIZE_MISMATCH
#define JVX_DSP_ERROR_INVALID_SETTING JVX_ERROR_INVALID_SETTING
#define JVX_DSP_ERROR_ABORT JVX_ERROR_ABORT 
#define JVX_DSP_ERROR_REMOTE JVX_ERROR_REMOTE
#define JVX_DSP_ERROR_ALLOCATION JVX_ERROR_ALLOCATION
#define JVX_DSP_ERROR_INVALID_FORMAT JVX_ERROR_INVALID_FORMAT

#ifdef __cplusplus

// IMPORTANT: WE NEED TO BE CAREFUL IN SETTING THE MEMORY TO ZERO IF ALLOCATED IN C++!!
// We need to be careful to NOT set the content to zero if, e.g., we have strings in the list/object
#define JVX_DSP_SAFE_ALLOCATE_OBJECT JVX_SAFE_ALLOCATE_OBJECT
#define JVX_DSP_SAFE_ALLOCATE_OBJECT_CPP_Z JVX_SAFE_ALLOCATE_OBJECT_CPP_Z
#define JVX_DSP_SAFE_ALLOCATE_FIELD JVX_SAFE_ALLOCATE_FIELD
#define JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z JVX_SAFE_ALLOCATE_FIELD_CPP_Z
#define JVX_DSP_SAFE_DELETE_OBJECT JVX_SAFE_DELETE_OBJECT
#define JVX_DSP_SAFE_DELETE_FIELD JVX_SAFE_DELETE_FIELD
#define JVX_DSP_SAFE_DELETE_FIELD_TYPE JVX_SAFE_DELETE_FIELD_TYPE

#else

// In C, 
#define JVX_DSP_SAFE_ALLOCATE_OBJECT_Z JVX_SAFE_ALLOCATE_OBJECT_Z
#define JVX_DSP_SAFE_ALLOCATE_FIELD_Z JVX_SAFE_ALLOCATE_FIELD_Z
#define JVX_DSP_SAFE_DELETE_OBJECT JVX_SAFE_DELETE_OBJECT
#define JVX_DSP_SAFE_DELETE_FIELD JVX_SAFE_DELETE_FIELD
#define JVX_DSP_SAFE_DELETE_FIELD_TYPE JVX_SAFE_DELETE_FIELD_TYPE

#endif

// =========================================================
// =========================================================

#define JVX_DSP_UPDATE_ASYNC JVX_LIB_UPDATE_ASYNC
#define JVX_DSP_UPDATE_SYNC JVX_LIB_UPDATE_SYNC
#define JVX_DSP_UPDATE_INIT JVX_LIB_UPDATE_INIT
#define JVX_DSP_UPDATE_FEEDBACK JVX_LIB_UPDATE_FEEDBACK
#define JVX_DSP_UPDATE_USER_OFFSET JVX_LIB_UPDATE_USER_OFFSET
#define JVX_DSP_UPDATE_ALL16  JVX_LIB_UPDATE_ALL16
#define JVX_DSP_UPDATE_ALL32  JVX_LIB_UPDATE_ALL32

//! Shortcut for complex datatypes, actually defined to support fftw
typedef struct
{
	jvxData re;
	jvxData im;
} jvxDataCplx;

#endif
