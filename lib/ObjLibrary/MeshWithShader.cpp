//
//  MeshWithShader.cpp
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

#include "ObjSettings.h"
#ifndef OBJ_LIBRARY_GLM_INTERACTION
	#error "MeshWithShader can only be used with OBJ_LIBRARY_GLM_INTERACTION #defined"
#endif
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	#error "MeshWithShader can only be used with OBJ_LIBRARY_SHADER_DISPLAY #defined"
#endif

#include "../GetGlutWithShaders.h"
#include "../glm/glm.hpp"

#include "VertexDataFormat.h"
#include "ObjVbo.h"
#include "ObjVao.h"
#include "MeshWithShader.h"

using namespace ObjLibrary;



bool MeshWithShader :: isValidPrimitiveType (unsigned int primitive_type)
{
	switch(primitive_type)
	{
	case GL_POINTS:
	case GL_LINES:
	case GL_LINE_STRIP:
	case GL_LINE_LOOP:
	case GL_TRIANGLES:
	case GL_TRIANGLE_STRIP:
	case GL_TRIANGLE_FAN:
		return true;
	default:
		return false;
	}
}

bool MeshWithShader :: isValidDataFormatForPrimitiveType (unsigned int data_format,
                                                          unsigned int primitive_type)
{
	assert(isValidPrimitiveType(primitive_type));
	assert(data_format < VertexDataFormat::COUNT);

	switch(primitive_type)
	{
	case GL_POINTS:
		return (data_format == VertexDataFormat::POSITION_ONLY);
	case GL_LINES:
	case GL_LINE_STRIP:
	case GL_LINE_LOOP:
		switch(data_format)
		{
		case VertexDataFormat::POSITION_ONLY:
		case VertexDataFormat::POSITION_TEXTURE_COORDINATE:
			return true;
		}
		return false;
	case GL_TRIANGLES:
	case GL_TRIANGLE_STRIP:
	case GL_TRIANGLE_FAN:
		return true;  // valid with all data formats
	}

	// no way to get here
	return false;
}



MeshWithShader :: MeshWithShader ()
		: m_primitive_type(GL_POINTS)
		, m_data_format(VertexDataFormat::POSITION_ONLY)
		, m_vao()
		, m_vertex_data()
		, m_vertex_indexes()
{
	assert(invariant());
}

MeshWithShader :: MeshWithShader (const MeshWithShader& original)
		: m_primitive_type(original.m_primitive_type)
		, m_data_format(original.m_data_format)
		, m_vao(original.m_vao)
		, m_vertex_data(original.m_vertex_data)
		, m_vertex_indexes(original.m_vertex_indexes)
{
	assert(invariant());
}

MeshWithShader :: ~MeshWithShader ()
{
}

MeshWithShader& MeshWithShader :: operator= (const MeshWithShader& original)
{
	if(&original != this)
	{
		m_primitive_type = original.m_primitive_type;
		m_data_format    = original.m_data_format;
		m_vao            = original.m_vao;
		m_vertex_data    = original.m_vertex_data;
		m_vertex_indexes = original.m_vertex_indexes;
	}

	assert(invariant());
	return *this;
}



void MeshWithShader :: draw () const
{
	assert(isInitialized());

	m_vao.bind();

	glDrawElements(m_primitive_type, m_vertex_indexes.getElementCount(), GL_UNSIGNED_INT, 0);
}


void MeshWithShader :: init (unsigned int primitive_type,
                             unsigned int data_format,
                             const ObjVbo<float>& vertex_data,
                             const ObjVbo<unsigned int>& vertex_indexes)
{
	assert(!isInitialized());
	assert(isValidPrimitiveType(primitive_type));
	assert(data_format < VertexDataFormat::COUNT);
	assert(isValidDataFormatForPrimitiveType(data_format, primitive_type));
	assert(!vertex_data.isEmpty());
	assert(vertex_data.getTarget() == GL_ARRAY_BUFFER);
	assert(!vertex_indexes.isEmpty());
	assert(vertex_indexes.getTarget() == GL_ELEMENT_ARRAY_BUFFER);

	m_primitive_type = primitive_type;
	m_data_format    = data_format;
	m_vertex_data    = vertex_data;
	m_vertex_indexes = vertex_indexes;

	// start VAO
	m_vao.init();
	m_vao.bind();

	// attach vertex data to VAO
	assert(m_vertex_data.getTarget() == GL_ARRAY_BUFFER);
	m_vertex_data.bind();

	unsigned int stride = VertexDataFormat::getStride(data_format);

	//
	//  glVertexAttribPointer(slot index,
	//                        how many components per vertex,
	//                        component type,
	//                        always GL_FALSE,
	//                        stride in bytes (0 for tightly packed),
	//                        pointer offset typcast to void pointer);
	//
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);  // vertex position
	glEnableVertexAttribArray(0);

	unsigned int offset;
	switch(m_data_format)
	{
	case VertexDataFormat::POSITION_ONLY:
		// nothing else
		break;
	case VertexDataFormat::POSITION_TEXTURE_COORDINATE:
		offset = offsetof(VertexDataFormat::PositionTextureCoordinate, m_s);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(offset));  // texture coordinates
		glEnableVertexAttribArray(1);
		break;
	case VertexDataFormat::POSITION_NORMAL:
		offset = offsetof(VertexDataFormat::PositionNormal, m_nx);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(offset));  // vertex normal
		glEnableVertexAttribArray(2);
		break;
	case VertexDataFormat::POSITION_TEXTURE_COORDINATE_NORMAL:
		offset = offsetof(VertexDataFormat::PositionTextureCoordinateNormal, m_s);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(offset));  // texture coordinates
		glEnableVertexAttribArray(1);
		offset = offsetof(VertexDataFormat::PositionTextureCoordinateNormal, m_nx);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(offset));  // vertex normal
		glEnableVertexAttribArray(2);
		break;
	}

	//  The attributes that are not set here have to be set at
	//    draw time using a glVertexAttrib* function, which
	//    changes the global state.
	//

	// attach vertex idexes to VAO - no options
	assert(m_vertex_indexes.getTarget() == GL_ELEMENT_ARRAY_BUFFER);
	m_vertex_indexes.bind();

	// finished VAO
	m_vao.bindNone();  // we don't want to accidentally change it elsewhere

	assert(invariant());
}


void MeshWithShader :: makeEmpty ()
{
	// m_primitive_type
	// m_data_format
	m_vao.makeEmpty();
	m_vertex_data.makeEmpty();
	m_vertex_indexes.makeEmpty();

	assert(invariant());
}



bool MeshWithShader :: invariant () const
{
	if(!isValidPrimitiveType(m_primitive_type)) return false;
	if(m_data_format >= VertexDataFormat::COUNT) return false;
	if(!isValidDataFormatForPrimitiveType(m_data_format, m_primitive_type)) return false;
	if(m_vao.isEmpty() != m_vertex_data.isEmpty()) return false;
	if(m_vao.isEmpty() != m_vertex_indexes.isEmpty()) return false;
	return true;
}
