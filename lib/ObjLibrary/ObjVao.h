//
//  ObjVao.h
//
//  A module to manage a Vertex Array Object (VAO).
//
//  This file is part of the Shaders extension to the
//    ObjLibrary, by Richard Hamilton, which is copyright
//    Hamilton 2009-2016.
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

#ifndef OBJ_LIBRARY_OBJ_VAO_H
#define OBJ_LIBRARY_OBJ_VAO_H

#include "ObjSettings.h"
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	#error "ObjVao can only be used with OBJ_LIBRARY_SHADER_DISPLAY #defined"
#endif



namespace ObjLibrary
{

//
//  ObjVao
//
//  A class to manage a Vertex Array Object (VAO).
//
//  Note: This class only manages creating and destroying the
//        VAO.  It dies not represent an entire mesh in the
//        model and cannot, by itself, be used to display
//        anything.
//

class ObjVao
{
public:
//
//  bindNone
//
//  Purpose: To unbind the current VAO, if any.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: The OpenGL context is set to not have a bound
//               VAO.
//

	static void bindNone ();

public:
//
//  Default Constructor
//
//  Purpose: To create a ObjVao that does not represents an
//           OpenGL VAO.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new ObjVao is created and marked as empty.
//

	ObjVao ();

//
//  Initializing Constructor
//
//  Purpose: To create a ObjVao that may represents a new OpenGL
//           VAO.
//  Parameter(s):
//    <1> is_init: Whether an OpenGL VAO should be created
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new ObjVao is created.  If is_init == true, a
//               new OpenGL VAO is created.  Otherwise, the new
//               ObjVao is marked as empty.  If a new VAO is
//               created, it is not bound.
//

	ObjVao (bool is_init);

//
//  Copy Constructor
//
//  Purpose: To create a new ObjVao as a copy of another.
//  Parameter(s):
//    <1> original: The ObjVao to copy
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new ObjVao is created.  If original is empty,
//               this ObjVao is marked as empty.  Otherwise,
//               this ObjVao is set to refer to the same OpenGL
//               VAO as original.
//

	ObjVao (const ObjVao& original);

//
//  Destructor
//
//  Purpose: To safely destroy this ObjVao without memory
//           leaks.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: All dynamically allocated memory is freed.  If
//               this ObjVao contains the last reference to its
//               OpenGL VAO, that VAO is destroyed.
//

	~ObjVao ();

//
//  Assignment Operator
//
//  Purpose: To modify this ObjVao to be a copy of another.
//  Parameter(s):
//    <1> original: The ObjVao to copy
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: If original is empty, this ObjVao is marked as
//               empty.  Otherwise this ObjVao is set to refer
//               to the same OpenGL VAO as original.  If this
//               ObjVao contained the last reference to its
//               previous OpenGL VAO, that VAO is destroyed.
//

	ObjVao& operator= (const ObjVao& original);

//
//  isEmpty
//
//  Purpose: To determine if this ObjVao is empty.  An ObjVao is
//           empty if it does not correspond to an OpenGL VAO.
//           This is the state of a ObjVao created with the
//           default constructor and after makeEmpty() is
//           called.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this ObjVao is empty.
//  Side Effect: N/A
//

	bool isEmpty () const;

//
//  getId
//
//  Purpose: To determine the OpenGL id stored by this ObjVao.
//  Parameter(s):
//    <1> !isEmpty()
//  Precondition(s): N/A
//  Returns: The number of elements in this ObjVao.
//  Side Effect: N/A
//

	unsigned int getId () const;

//
//  bind
//
//  Purpose: To bind this ObjVao in the current OpenGL context.
//  Parameter(s):
//    <1> !isEmpty()
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: The OpenGL VAO referenced by this ObjVao is
//               bound.
//

	void bind () const;

//
//  makeEmpty
//
//  Purpose: To mark this ObjVao as empty.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: This ObjVao is marked as empty.  If this ObjVao
//               contains the last reference to its OpenGL VAO,
//               that VAO is destroyed.
//

	void makeEmpty ();

//
//  init
//
//  Purpose: To set this ObjVao to contain a new OpenGL VAO.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new OpenGL VAO is created for this ObjVao.
//               If this ObjVao previously contained the last
//               reference to its OpenGL VAO, that VAO is
//               destroyed.  The new VAO is not bound.
//

	void init ();

private:
//
//  copy
//
//  Purpose: To copy the values of another ObjVao to this
//           ObjVao.
//  Parameter(s):
//    <1> original: The ObjVao to copy
//  Precondition(s):
//    <1> isEmpty()
//    <2> !original.isPartial()
//  Returns: N/A
//  Side Effect: If original is empty, this VAO is marked as
//               empty.  Otherwise, this ObjVao is set to
//               contain the same OpenGL VAO as original.
//

	void copy (const ObjVao& original);

private:
	//
	//  InnerData
	//
	//  A record to store the VAO id and current usage count for
	//    an OpenGL VAO.
	//

	struct InnerData
	{
		unsigned int m_vao_id;
		unsigned int m_usage_count;
	};

private:
	InnerData* mp_data;
};



}  // end of namespace ObjLibrary

#endif
