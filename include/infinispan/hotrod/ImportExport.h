#ifndef HOTROD_IMPORT_EXPORT_H
#define HOTROD_IMPORT_EXPORT_H 1

/*
 * JBoss, Home of Professional Open Source
 * Copyright 2010 Red Hat Inc. and/or its affiliates and other
 * contributors as indicated by the @author tags. All rights reserved.
 * See the copyright.txt in the distribution for a full listing of
 * individual contributors.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 */

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
