//
//  ObjVao.cpp
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

#include <cassert>
#include <cstddef>	// for NULL

#include "ObjSettings.h"
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	#error "ObjVao can only be used with OBJ_LIBRARY_SHADER_DISPLAY #defined"
#endif

#include "../GetGlutWithShaders.h"

#include "ObjVao.h"

using namespace ObjLibrary;



void ObjVao :: bindNone ()
{
	glBindVertexArray(0);
}



ObjVao :: ObjVao ()
		: mp_data(NULL)
{
}

ObjVao :: ObjVao (bool is_init)
		: mp_data(NULL)
{
	if(is_init)
	{
		assert(mp_data == NULL);
		init();
		assert(!isEmpty());
	}
}

ObjVao :: ObjVao (const ObjVao& original)
		: mp_data(NULL)
{
	copy(original);
}

ObjVao :: ~ObjVao ()
{
	makeEmpty();
}

ObjVao& ObjVao :: operator= (const ObjVao& original)
{
	if(&original != this)
	{
		makeEmpty();

		assert(isEmpty());
		copy(original);
	}
	return *this;
}



bool ObjVao :: isEmpty () const
{
	return (mp_data == NULL);
}

unsigned int ObjVao :: getId () const
{
	assert(!isEmpty());

	assert(mp_data != NULL);
	return mp_data->m_vao_id;
}

void ObjVao :: bind () const
{
	assert(!isEmpty());

	assert(mp_data != NULL);
	glBindVertexArray(mp_data->m_vao_id);
}



void ObjVao :: makeEmpty ()
{
	if(mp_data != NULL)
	{
		assert(mp_data->m_usage_count >= 1);
		mp_data->m_usage_count--;

		if(mp_data->m_usage_count == 0)
		{
			glDeleteVertexArrays(1, &(mp_data->m_vao_id));

			delete mp_data;
		}

		mp_data = NULL;
	}

	assert(isEmpty());
}

void ObjVao :: init ()
{
	makeEmpty();

	assert(mp_data == NULL);
	mp_data = new InnerData;

	glGenVertexArrays(1, &(mp_data->m_vao_id));

	mp_data->m_usage_count = 1;

	assert(!isEmpty());
}



void ObjVao :: copy (const ObjVao& original)
{
	assert(isEmpty());

	mp_data = original.mp_data;

	if(mp_data != NULL)
		mp_data->m_usage_count++;

	assert(isEmpty() == original.isEmpty());
}
