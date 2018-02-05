//
//  ObjVbo.inl
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

#ifndef OBJ_LIBRARY_OBJ_VBO_INL
#define OBJ_LIBRARY_OBJ_VBO_INL

#include <cassert>
#include <cstddef>	// for NULL

#include "ObjSettings.h"
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	#error "ObjVbo can only be used with OBJ_LIBRARY_SHADER_DISPLAY #defined"
#endif

#include "../GetGlutWithShaders.h"

#include "ObjVbo.h"



template <typename DataType>
bool ObjLibrary :: ObjVbo<DataType> :: isValidTarget (unsigned int target)
{
	switch(target)
	{
	case GL_ARRAY_BUFFER:
	case GL_ATOMIC_COUNTER_BUFFER:
	case GL_COPY_READ_BUFFER:
	case GL_COPY_WRITE_BUFFER:
	case GL_DISPATCH_INDIRECT_BUFFER:
	case GL_DRAW_INDIRECT_BUFFER:
	case GL_ELEMENT_ARRAY_BUFFER:
	case GL_PIXEL_PACK_BUFFER:
	case GL_PIXEL_UNPACK_BUFFER:
#ifdef GL_QUERY_BUFFER
	case GL_QUERY_BUFFER:  // maybe my shader #includes are too old?
#endif
	case GL_SHADER_STORAGE_BUFFER:
	case GL_TEXTURE_BUFFER:
	case GL_TRANSFORM_FEEDBACK_BUFFER:
	case GL_UNIFORM_BUFFER:
		return true;
	default:
		return false;
	}
}

template <typename DataType>
bool ObjLibrary :: ObjVbo<DataType> :: isValidExpectedUsagePattern (unsigned int expected_usage_pattern)
{
	switch(expected_usage_pattern)
	{
	case GL_STREAM_DRAW:
	case GL_STREAM_READ:
	case GL_STREAM_COPY:
	case GL_STATIC_DRAW:
	case GL_STATIC_READ:
	case GL_STATIC_COPY:
	case GL_DYNAMIC_DRAW:
	case GL_DYNAMIC_READ:
	case GL_DYNAMIC_COPY:
		return true;
	default:
		return false;
	}
}



template <typename DataType>
ObjLibrary :: ObjVbo<DataType> :: ObjVbo (unsigned int target,
                                          unsigned int element_count,
                                          const DataType a_elements[],
                                          unsigned int expected_usage_pattern)
		: mp_data(NULL)
{
	assert(isValidTarget(target));
	assert(a_elements != NULL);
	assert(isValidExpectedUsagePattern(expected_usage_pattern));

	assert(mp_data == NULL);
	init(target, element_count, a_elements, expected_usage_pattern);

	assert(!isEmpty());
	assert(invariant());
}

template <typename DataType>
ObjLibrary::ObjVbo<DataType>& ObjLibrary :: ObjVbo<DataType> :: operator= (const ObjLibrary::ObjVbo<DataType>& original)
{
	if(&original != this)
	{
		makeEmpty();

		assert(isEmpty());
		copy(original);
	}

	assert(invariant());
	return *this;
}



template <typename DataType>
void ObjLibrary :: ObjVbo<DataType> :: bind () const
{
	assert(!isEmpty());

	assert(mp_data != NULL);
	glBindBuffer(mp_data->m_target, mp_data->m_vbo_id);
}

template <typename DataType>
void ObjLibrary :: ObjVbo<DataType> :: bindNone () const
{
	assert(!isEmpty());

	assert(mp_data != NULL);
	glBindBuffer(mp_data->m_target, 0);  // bind nothing
}



template <typename DataType>
void ObjLibrary :: ObjVbo<DataType> :: makeEmpty ()
{
	if(mp_data != NULL)
	{
		assert(mp_data->m_usage_count >= 1);
		mp_data->m_usage_count--;

		if(mp_data->m_usage_count == 0)
		{
			glDeleteBuffers(1, &(mp_data->m_vbo_id));

			delete mp_data;
		}

		mp_data = NULL;
	}

	assert(isEmpty());
	assert(invariant());
}

template <typename DataType>
void ObjLibrary :: ObjVbo<DataType> :: init (unsigned int target,
                                             unsigned int element_count,
                                             const DataType a_elements[],
                                             unsigned int expected_usage_pattern)
{
	assert(isValidTarget(target));
	assert(a_elements != NULL);
	assert(isValidExpectedUsagePattern(expected_usage_pattern));

	makeEmpty();

	assert(mp_data == NULL);
	mp_data = new InnerData;
	mp_data->m_target                 = target;
	mp_data->m_element_count          = element_count;
	mp_data->m_expected_usage_pattern = expected_usage_pattern;

	glGenBuffers(1, &(mp_data->m_vbo_id));
	glBindBuffer(target, mp_data->m_vbo_id);
	glBufferData(target, element_count * sizeof(DataType), a_elements, expected_usage_pattern);

	mp_data->m_usage_count = 1;

	bindNone();  // everything should be set up by now

	assert(!isEmpty());
	assert(invariant());
}

template <typename DataType>
void ObjLibrary :: ObjVbo<DataType> :: update (const DataType a_elements[])
{
	assert(!isEmpty());
	assert(a_elements != NULL);

	bind();

	// void glBufferSubData(enum target,
	//                      intptr offset in bytes,
	//                      sizeiptr size in bytes,
	//                      const void *data)
	glBufferSubData(mp_data->m_target,
	                0,
	                mp_data->m_element_count * sizeof(DataType),
	                a_elements);

	bindNone();

	assert(!isEmpty());
	assert(invariant());
}

template <typename DataType>
void ObjLibrary :: ObjVbo<DataType> :: update (const DataType a_elements[],
                                               unsigned int start,
                                               unsigned int count)
{
	assert(!isEmpty());
	assert(a_elements != NULL);
	assert(start + count <= getElementCount());

	bind();

	// void glBufferSubData(enum target,
	//                      intptr offset in bytes,
	//                      sizeiptr size in bytes,
	//                      const void *data)
	glBufferSubData(mp_data->m_target,
	                start * sizeof(DataType),
	                count * sizeof(DataType),
	                a_elements);

	bindNone();

	assert(!isEmpty());
	assert(invariant());
}



template <typename DataType>
void ObjLibrary :: ObjVbo<DataType> :: copy (const ObjVbo& original)
{
	assert(isEmpty());

	mp_data = original.mp_data;
	if(mp_data != NULL)
		mp_data->m_usage_count++;

	assert(isEmpty() == original.isEmpty());
	assert(invariant());
}

template <typename DataType>
bool ObjLibrary :: ObjVbo<DataType> :: invariant () const
{
	if(mp_data != NULL && !isValidTarget(mp_data->m_target)) return false;
	if(mp_data != NULL && !isValidExpectedUsagePattern(mp_data->m_expected_usage_pattern)) return false;
	if(mp_data != NULL && mp_data->m_usage_count < 1) return false;
	return true;
}



#endif
