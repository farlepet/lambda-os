#ifndef LAMBDA_LIB_STDINT_H
#define LAMBDA_LIB_STDINT_H

typedef unsigned char      uint8_t;  ///< Unsigned 8 bit integer
typedef unsigned short     uint16_t; ///< Unsigned 16 bit integer
typedef unsigned int       uint32_t; ///< Unsigned 32 bit integer
typedef unsigned long long uint64_t; ///< Unsigned 64 bit integer

typedef char      int8_t;  ///< Signed 8 bit integer
typedef short     int16_t; ///< Signed 16 bit integer
typedef int       int32_t; ///< Signed 32 bit integer
typedef long long int64_t; ///< Signed 64 bit integer

/* @todo Base on target */
typedef uint32_t uintptr_t;

#endif
