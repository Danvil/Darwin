/*
 * Candy_Dll.h
 *
 *  Created on: 30.07.2011
 *      Author: David
 */

#ifndef CANDY_DLL_H_
#define CANDY_DLL_H_

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
  #define CANDY_HELPER_DLL_IMPORT __declspec(dllimport)
  #define CANDY_HELPER_DLL_EXPORT __declspec(dllexport)
  #define CANDY_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define CANDY_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define CANDY_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define CANDY_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define CANDY_HELPER_DLL_IMPORT
    #define CANDY_HELPER_DLL_EXPORT
    #define CANDY_HELPER_DLL_LOCAL
  #endif
#endif

// Now we use the generic helper definitions above to define CANDY_API and CANDY_LOCAL.
// CANDY_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// CANDY_LOCAL is used for non-api symbols.

#ifdef CANDY_DLL // defined if CANDY is compiled as a DLL
  #ifdef CANDY_DLL_EXPORTS // defined if we are building the CANDY DLL (instead of using it)
    #define CANDY_API CANDY_HELPER_DLL_EXPORT
  #else
    #define CANDY_API CANDY_HELPER_DLL_IMPORT
  #endif // CANDY_DLL_EXPORTS
  #define CANDY_LOCAL CANDY_HELPER_DLL_LOCAL
#else // CANDY_DLL is not defined: this means CANDY is a static lib.
  #define CANDY_API
  #define CANDY_LOCAL
#endif // CANDY_DLL

#endif /* CANDY_DLL_H_ */
