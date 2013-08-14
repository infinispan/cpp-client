#ifndef HOTROD_IMPORT_EXPORT_H
#define HOTROD_IMPORT_EXPORT_H 1



//
// Compiler specific mechanisms for managing the import and export of
// symbols between shared objects.
//
// HR_EXPORT         - Export declaration
// HR_IMPORT         - Import declaration
//

#if !defined(HOTROD_DECLARE_STATIC)
  #if defined(WIN32)
    #define HR_EXPORT __declspec(dllexport)
    #define HR_IMPORT __declspec(dllimport)
  #else
    #if __GNUC__ >= 4
      #define HR_EXPORT __attribute__ ((visibility ("default")))
      #define HR_IMPORT __attribute__ ((visibility ("default")))
    #endif
  #endif
#endif

#if !defined(HR_EXPORT)
  // default values
  #define HR_EXPORT
  #define HR_IMPORT
#endif


// value of xxx_EXPORTS is set by CMake
#ifdef hotrod_EXPORTS
  #define HR_EXTERN HR_EXPORT
#else
  #define HR_EXTERN HR_IMPORT
#endif


#endif /* import_export.h */
