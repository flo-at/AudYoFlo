// #ifndef __JVX_SYSTEM_H___DSPACE__
// #define __JVX_SYSTEM_H___DSPACE__
// #define JVX_OS_DSPACE

#ifndef __JVX_SYSTEM_H__LINUX__
#define __JVX_SYSTEM_H__LINUX__

#include <sys/types.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>
#include <sys/utsname.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>

// Define the operating system
#ifndef JVX_OS_LINUX
#define JVX_OS_LINUX
#endif

#include "../../jvx_system_defines.h"

#include "../common_ux/jvx_system_types_ux.h"
#include "../common_ux/jvx_system_helpers_ux.h"

// For all conditional waits, we need to specify the right clock source
#define JVX_GLOBAL_CLOCK_SOURCE CLOCK_MONOTONIC
// CLOCK_BOOTTIME
// CLOCK_MONOTONIC
// CLOCK_REALTIME

#include "../common_ux/jvx_system_time_ux.h"
// #include "../common_ux/jvx_system_threads_ux.h"
// #include "../common_ux/jvx_system_mutex_ux.h"
// #include "../common_ux/jvx_system_compute_ux.h"
// #include "../common_ux/jvx_system_misc_ux.h"

#include "../../jvx_system_includes.h"

#endif


