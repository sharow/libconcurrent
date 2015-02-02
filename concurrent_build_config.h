/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/*
  libconcurrent
  Copyright (C) 2010-2013 MIURA Shirow (sharow)
*/

#ifndef INCLUDED_CONCURRENT_BUILD_CONFIG_H
#define INCLUDED_CONCURRENT_BUILD_CONFIG_H

/* build config of gcc */

#ifdef __gnu_linux__
# define CONCURRENT_API
#else
# define CONCURRENT_API __declspec(dllexport)
#endif

#ifdef __STDC_VERSION__
# if __STDC_VERSION__ >= 199901L
#  define CONCURRENT_HAS_LONGLONG
# endif
#endif

#define CONCURRENT_VERSION_MAJOR 0
#define CONCURRENT_VERSION_MINOR 3
#define CONCURRENT_VERSION_REVISION 0


#endif
