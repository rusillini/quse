#ifndef __GNUC__
# error This is not a GCC environment. Please check your configuration.
#endif

#undef CC_OLD_IOSTREAM
#if __GNUC__ > 2
# define CC_HAS_SSTREAM 1
#else
# undef CC_HAS_SSTREAM
#endif
#undef CC_NO_STD_NS
#undef CC_NO_WCHAR_T

#define OSP_FM_UNIX 1
#define OSP_API_POSIX 1
#define OSP_ENCODING "utf-8"
#define OSP_SPACES " \t\r\n"
#define OSP_DLL_EXT ".so"
#define OSP_EXE_EXT ""
#define OSP_PATH_DELIM "/"

#define STR_UI64 "%llu"
#define STR_I64 "%lld"
#define STR_I64_FIXED "%016llX"
#define STR_I64_LEN 16
#define STR_UIP "%ld"
#define STR_IP "%lu"

#define DLL_EXPORT_API
#define DLL_IMPORT_API

#define CC_LONGLONG_T long long
#define CC_INLINE inline
#define OSP_NEED_I64_SEPARATE 1

typedef short int16_ot;
typedef unsigned short u_int16_ot;
typedef int int32_ot;
typedef unsigned int u_int32_ot;

typedef CC_LONGLONG_T int64_ot;
typedef unsigned CC_LONGLONG_T u_int64_ot;
typedef int intp_ot;
typedef unsigned int u_intp_ot;
