//
//  ObjVbo.h
//
//  A module to encapsulate an OpenGL vertex buffer object
//    (VBO).
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
//

#ifndef OBJ_LIBRARY_OBJ_VBO_H
#define OBJ_LIBRARY_OBJ_VBO_H

#include "ObjSettings.h"
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	#error "ObjVbo can only be used with OBJ_LIBRARY_SHADER_DISPLAY #defined"
#endif



namespace ObjLibrary
{

//
//  ObjVbo
//
//  A templated wrapper class to encapsulate an OpenGL vertex
//    buffer object (VBO), intended for interbal use by
//    ObjLibrary shader-handling code.  An ObjVbo can be
//    created, copied, and destroyed.  If a ObjVbo is copied,
//    the internal VBO will not be copied.  However, the
//    underlying VBO will not be destroyed until the last
//    reference is removed.  It's a lot like (C++11 or later)
//    shared_ptr<>s.
//
//  A ObjVbo can be empty or initialized.
//
//  A VBO is an OpenGL concept that allows a block of data to be
//    stored on the graphics card.  They are used to represent
//    model geometry (among other things) when working with
//    shaders.
//
//  Class Invariant:
//    <1> mp_data == NULL || isValidTarget(mp_data->m_target)
//    <2> mp_data == NULL || isValidExpectedUsagePattern(
//                            mp_data->m_expected_usage_pattern)
//    <3> mp_data == NULL || mp_data->m_usage_count >= 1
//

template <typename DataType>
class ObjVbo
{
public:
//
//  isValidTarget
//
//  Purpose: To determine if the specified value is a valid
//           OpenGL VBO target.
//  Parameter(s):
//    <1> target: The target to test
//  Precondition(s): N/A
//  Returns: Whether target is a valid VBO target.
//  Side Effect: N/A
//

	static bool isValidTarget (unsigned int target);

//
//  isValidExpectedUsagePattern
//
//  Purpose: To determine if the specified value is a valid
//           OpenGL VBO expected usage pattern.
//  Parameter(s):
//    <1> target: The target to test
//  Precondition(s): N/A
//  Returns: Whether target is a valid VBO expected usage
//           pattern.
//  Side Effect: N/A
//

	static bool isValidExpectedUsagePattern (
	                       unsigned int expected_usage_pattern);

public:
//
//  Default Constructor
//
//  Purpose: To create a new empty ObjVbo.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new ObjVbo is created and marked as empty.
//

	ObjVbo ()
			: mp_data(NULL)
	{
		assert(invariant());
	}

//
//  Initializing Constructor
//
//  Purpose: To create a new ObjVbo containing the specified
//           expected usage pattern, data, and target.  For
//           vertex data, the target should be GL_ARRAY_BUFFER.
//           For vertex indexes, the target should be
//           GL_ELEMENT_ARRAY_BUFFER.  The expected usage
//           pattern should normally be GL_STATIC_DRAW.
//  Parameter(s):
//    <1> target: The expected usage pattern for
//                                the data
//    <2> element_count: The number of elements
//    <3> a_elements: An array containing the elements to store
//    <4> expected_usage_pattern: The expected usage pattern for
//                                the data
//  Precondition(s):
//    <1> isValidTarget(target)
//    <2> a_elements != NULL
//    <3> isValidExpectedUsagePattern(expected_usage_pattern)
//  Returns: N/A
//  Side Effect: A new ObjVbo is created and filled with the
//               first element_count elements of a_elements.
//               It has a target of target and an expected
//               usage pattern of expected_usage_pattern.  When
//               this function terminates, no VBO is bound.
//

	ObjVbo (unsigned int target,
	        unsigned int element_count,
	        const DataType a_elements[],
	        unsigned int expected_usage_pattern);

//
//  Copy Constructor
//
//  Purpose: To create a new ObjVbo as a copy of another.
//  Parameter(s):
//    <1> original: The ObjVbo to copy
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new ObjVbo is created.  If original is empty,
//               the new ObjVbo is marked as empty.  Otherwise,
//               this ObjVbo is set to refer to the same OpenGL
//               VBO as original.
//

	ObjVbo (const ObjVbo<DataType>& original)
			: mp_data(NULL)
	{
		copy(original);

		assert(invariant());
	}

//
//  Destructor
//
//  Purpose: To safely destroy this ObjVbo without memory
//           leaks.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: All dynamically allocated memory is freed.  If
//               this ObjVbo contains the last reference to its
//               OpenGL VBO, that VBO is destroyed.
//

	~ObjVbo ()
	{
		makeEmpty();
	}

//
//  Assignment Operator
//
//  Purpose: To modify this ObjVbo to be a copy of another.
//  Parameter(s):
//    <1> original: The ObjVbo to copy
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: If original is empty, this ObjVbo is marked as
//               empty.  Otherwise this ObjVbo is set to refer
//               to the same OpenGL VBO as original.  If this
//               ObjVbo contained the last reference to its
//               previous OpenGL VBO, that VBO is destroyed.
//

	ObjVbo& operator= (const ObjVbo<DataType>& original);

//
//  isEmpty
//
//  Purpose: To determine if this ObjVbo is empty.  An ObjVbo is
//           empty if it does not correspond to an OpenGL VBO.
//           This is the state of a ObjVbo created with the
//           default constructor and after makeEmpty() is
//           called.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this ObjVbo is empty.
//  Side Effect: N/A
//

	bool isEmpty () const
	{
		return (mp_data == NULL);
	}

//
//  getTarget
//
//  Purpose: To determine the target for this this ObjVbo.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> !isEmpty()
//  Returns: The target for this ObjVbo.  This will be
//           GL_ARRAY_BUFFER for vertex data and
//           GL_ELEMENT_ARRAY_BUFFER for vertex indexes.
//  Side Effect: N/A
//

	unsigned int getTarget () const
	{
		assert(!isEmpty());

		assert(mp_data != NULL);
		return mp_data->m_target;
	}

//
//  getId
//
//  Purpose: To determine the OpenGL id stored by this ObjVbo.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> !isEmpty()
//  Returns: The number of elements in this ObjVbo.
//  Side Effect: N/A
//

	unsigned int getId () const
	{
		assert(!isEmpty());

		assert(mp_data != NULL);
		return mp_data->m_vbo_id;
	}

//
//  bind
//
//  Purpose: To bind this ObjVbo in the current OpenGL context.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> !isEmpty()
//  Returns: N/A
//  Side Effect: The OpenGL VBO referenced by this ObjVao is
//               bound.
//

	void bind () const;

//
//  bindNone
//
//  Purpose: To unbind this ObjVbo in the current OpenGL
//           context.  This is normally not necessary, but can
//           reduce the chance of actidentally modifying it.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> !isEmpty()
//  Returns: N/A
//  Side Effect: If OpenGL VBO referenced by this ObjVao is
//               bound, it is unbound.  If OpenGL VBO referenced
//               by this ObjVbo was not bound, the OpenGL VBO
//               with the same target as this ObjVbo is unbound.
//               If no VBO is bound to that target, there is no
//               effect.
//

	void bindNone () const;

//
//  getElementCount
//
//  Purpose: To determine the number of elements stored in this
//           ObjVbo.  This should not be confused with the
//           number of bytes used to store them.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> !isEmpty()
//  Returns: The number of elements in this ObjVbo.
//  Side Effect: N/A
//

	unsigned int getElementCount () const
	{
		assert(!isEmpty());

		assert(mp_data != NULL);
		return mp_data->m_element_count;
	}

//
//  getByteCount
//
//  Purpose: To determine the number of bytes used to store the
//           elements in this ObjVbo.  This should not be
//           confused with the number of elements.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> !isEmpty()
//  Returns: The number of bytes used to store the elements in
//           this ObjVbo.
//  Side Effect: N/A
//

	unsigned int getByteCount () const
	{
		assert(!isEmpty());

		assert(mp_data != NULL);
		return mp_data->m_element_count * sizeof(DataType);
	}

//
//  getExpectedUsagePattern
//
//  Purpose: To determine the expected usage pattern for this
//           ObjVbo.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> !isEmpty()
//  Returns: The expected usage pattern.  This will probably be
//           GL_STATIC_DRAW.
//  Side Effect: N/A
//

	unsigned int getExpectedUsagePattern () const
	{
		assert(!isEmpty());

		assert(mp_data != NULL);
		return mp_data->m_expected_usage_pattern;
	}

//
//  makeEmpty
//
//  Purpose: To mark this ObjVbo as empty.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: This ObjVbo is marked as empty.  If this ObjVbo
//               contains the last reference to its OpenGL VBO,
//               that VBO is destroyed.
//

	void makeEmpty ();

//
//  init
//
//  Purpose: To set this ObjVbo to contain the specified
//           expected usage pattern, data, and target.  For
//           vertex data, the target should be GL_ARRAY_BUFFER.
//           For vertex indexes, the target should be
//           GL_ELEMENT_ARRAY_BUFFER.  The expected usage
//           pattern should be GL_STATIC_DRAW.
//  Parameter(s):
//    <1> target: The expected usage pattern for
//                                the data
//    <2> element_count: The number of elements
//    <3> a_elements: An array containing the elements to store
//    <4> expected_usage_pattern: The expected usage pattern for
//                                the data
//  Precondition(s):
//    <1> isValidTarget(target)
//    <2> a_elements != NULL
//    <3> isValidExpectedUsagePattern(expected_usage_pattern)
//  Returns: N/A
//  Side Effect: This ObjVbo is created and filled with the
//               first element_count elements of a_elements.
//               It is set to have a target of target and an
//               expected usage pattern of
//               expected_usage_pattern.  If this ObjVbo
//               perviously contained the last reference to its
//               OpenGL VBO, that VBO is destroyed.  When this
//               function terminates, no VBO is bound.
//

	void init (unsigned int target,
	           unsigned int element_count,
	           const DataType a_elements[],
	           unsigned int expected_usage_pattern);

//
//  update
//
//  Purpose: To modify this ObjVbo to contain the specified
//           data.  The target, element count, and expected
//           usage pattern are unchanged.
//  Parameter(s):
//    <1> a_elements: An array containing the elements to store
//  Precondition(s):
//    <1> !isEmpty()
//    <2> a_elements != NULL
//  Returns: N/A
//  Side Effect: This ObjVbo is set to contain the first
//               getElementCount() elements of a_elements.  When
//               this function terminates, no VBO is bound.
//

	void update (const DataType a_elements[]);

//
//  update
//
//  Purpose: To modify this ObjVbo to replace part of the
//           current data with the specified data.  The target,
//           element count, and expected usage pattern are
//           unchanged.
//  Parameter(s):
//    <1> a_elements: An array containing the elements to store
//    <2> start: The first element to replace
//    <3> count: The number of elements to replace
//  Precondition(s):
//    <1> !isEmpty()
//    <2> a_elements != NULL
//    <3> start + count <= getElementCount()
//  Returns: N/A
//  Side Effect: The first count elements starting at position
//               start in this ObjVbo are replaced with the
//               first count elements of a_elements.  When
//               this function terminates, no VBO is bound.
//

	void update (const DataType a_elements[],
	             unsigned int start,
	             unsigned int count);

private:
//
//  copy
//
//  Purpose: To copy the values of another ObjVbo to this
//           ObjVbo.
//  Parameter(s):
//    <1> original: The ObjVbo to copy
//  Precondition(s):
//    <1> isEmpty()
//    <2> !original.isPartial()
//  Returns: N/A
//  Side Effect: If original is empty, this VBO is marked as
//               empty.  Otherwise, this ObjVbo is set to
//               contain the same OpenGL VBO as original.
//

	void copy (const ObjVbo& original);

//
//  invariant
//
//  Purpose: To determine if the class invariant is true.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: Whether the class invarint is true.
//

	bool invariant () const;

private:
	//
	//  InnerData
	//
	//  A record to store information about an OpenGL VBO.  The
	//    target, VBO id, element count, expected usage pattern,
	//    and current usage count are stored.
	//

	struct InnerData
	{
		unsigned int m_target;
		unsigned int m_vbo_id;
		unsigned int m_element_count;
		unsigned int m_expected_usage_pattern;
		unsigned int m_usage_count;
	};

private:
	InnerData* mp_data;
};



}  // end of namespace ObjLibrary



//
//  #includes need to be outside namespaces.  Otherwise, we get
//    a copy of (part of) the standard library inside our
//    namespace, which is a Bad Thing.
//

#include "ObjVbo.inl"



#endif
