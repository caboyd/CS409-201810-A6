//
//  GetGlutWithShaders.h
//
//  A header file in import OpenGL, GLU, and GLUT on Windows,
//    Mac OSX, Linux, or Solaris.  The intermediate file is
//    helpful because there is only one line to change when
//    you move between enviroments.
//
//  This file is part of the ObjLibrary, by Richard Hamilton,
//    which is copyright Hamilton 2009-2016.
//
//  You may use these files for any purpose as long as you do
//    not explicitly claim them as your own work or object to
//    other people using them.
//
//  If you are destributing the source files, you must not
//    remove this notice.  If you are only destributing compiled
//    code, no credit is required.
//
//  A (theoretically) up-to-date version of the ObjLibrary can
//    be found at:
//  http://infiniplix.ca/resources/obj_library/
//

#ifndef OBJ_LIBRARY_GET_GLUT_WITH_SHADERS_H
#define OBJ_LIBRARY_GET_GLUT_WITH_SHADERS_H


// Unix
#ifdef unix
#error "What do I put to get shaders?"
#include <GL/freeglut.h>
#elif __unix
#include <GL/freeglut.h>
#elif __unix__
#include <GL/freeglut.h>

// Linux
#elif linux
#error "What do I put to get shaders?"
#include <GL/freeglut.h>
#elif __linux
#include <GL/freeglut.h>
#elif __linux__
#include <GL/freeglut.h>

// Mac OSX
#elif __APPLE__
#error "What do I put to get shaders?"
#include <GL/freeglut.h>
#elif __MACH__
#include <GL/freeglut.h>

// Solaris
#elif sun
#error "OpenGL Includes for solaris"
#elif __sun
#error "OpenGL Includes for solaris"

// FreeBSD
#elif __FreeBSD__
#error "OpenGL Includes for FreeBSD"

// Windows
#elif _WIN32	// include 64-bit enviroments
#include "gl3w.h"  // I don't know where this is from
#include "freeglut.h"  // http://freeglut.sourceforge.net/
#elif __WIN32__
#include "gl3w.h"
#include "freeglut.h"
#endif



#endif
