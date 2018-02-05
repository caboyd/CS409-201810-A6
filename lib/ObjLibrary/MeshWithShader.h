//
//  MeshWithShader.h
//
//  A module to represent a mesh for a model on the graphics
//    card.
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

#ifndef OBJ_LIBRARY_MESH_WITH_SHADER_H
#define OBJ_LIBRARY_MESH_WITH_SHADER_H

#include "ObjSettings.h"
#ifndef OBJ_LIBRARY_GLM_INTERACTION
	#error "MeshWithShader can only be used with OBJ_LIBRARY_GLM_INTERACTION #defined"
#endif
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	#error "MeshWithShader can only be used with OBJ_LIBRARY_SHADER_DISPLAY #defined"
#endif

#include "ObjVbo.h"
#include "ObjVao.h"



namespace ObjLibrary
{

//
//  MeshWithShader
//
//  A class to represent a mesh for a model on the graphics
//    card.
//
//  Class Invariant:
//    <1> isValidPrimitiveType(m_primitive_type)
//    <2> m_data_format < VertexDataFormat::COUNT
//    <3> isValidDataFormatForPrimitiveType(m_data_format,
//                                          m_primitive_type)
//    <4> m_vao.isEmpty() == m_vertex_data.isEmpty()
//    <5> m_vao.isEmpty() == m_vertex_indexes.isEmpty()
//

class MeshWithShader
{
public:
//
//  isValidPrimitiveType
//
//  Purpose: To determine if the specified primitive type is a
//           valid in OpenGL.
//  Parameter(s):
//    <1> primitive_type: The primitive type to test
//  Precondition(s): N/A
//  Returns: Whether primitive_type is a valid primitive type.
//           This function oes not consider quads (removed after
//           OpenGL v3.0) and primitives with adjacency lists
//           (only useful with specialized geometry shdaers) to
//           be invalid.
//  Side Effect: N/A
//

	static bool isValidPrimitiveType (
	                               unsigned int primitive_type);

//
//  isValidDataFormatForPrimitiveType
//
//  Purpose: To determine if the specified data format can be
//           combined with the specified primitive type.  The
//           permitted combinations depend on the properties of
//           the ObjLibrary standard shaders.
//  Parameter(s):
//    <1> primitive_type: The primitive type
//    <2> data_format: The data format
//  Precondition(s): N/A
//    <1> isValidPrimitiveType(primitive_type)
//    <2> data_format < VertexDataFormat::COUNT
//  Returns: Whether data format data_format can be used with
//           primitive type primitive_type.
//  Side Effect: N/A
//

	static bool isValidDataFormatForPrimitiveType (
	                               unsigned int data_format,
	                               unsigned int primitive_type);

public:
//
//  Default Constructor
//
//  Purpose: To create a MeshWithShader without initializing it.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new, uninitialized MeshWithShader is created.
//

	MeshWithShader ();

//
//  Copy Constructor
//
//  Purpose: To create a new MeshWithShader as a copy of
//           another.
//  Parameter(s):
//    <1> original: The MeshWithShader to copy
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new MeshWithShader is created as a copy of
//               original.  This function creates a shallow copy
//               of the data in video memeory.
//

	MeshWithShader (const MeshWithShader& original);

//
//  Destructor
//
//  Purpose: To safely destroy this MeshWithShader without
//           memory leaks.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: All dynamically allocated memory is freed.
//               This may include freeing video memory.
//

	~MeshWithShader ();

//
//  Assignment Operator
//
//  Purpose: To modify this MeshWithShader to be a copy of
//           another.
//  Parameter(s):
//    <1> original: The MeshWithShader to copy
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: This MeshWithShader is set to be a copy of
//               original.  This function creates a shallow copy
//               of the data in video memeory.  Video memory
//               data for the old mesh may be freed.
//

	MeshWithShader& operator= (const MeshWithShader& original);

//
//  isInitialized
//
//  Purpose: To determine if the mesh for this MeshWithShader
//           has been set.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this MeshWithShader can be displayed.
//  Side Effect: N/A
//

	bool isInitialized () const
	{
		return !m_vao.isEmpty();
	}

//
//  draw
//
//  Purpose: To display this MeshWithShader.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isInitialized()
//  Returns: N/A
//  Side Effect: This MeshWithShader is displayed using the
//               current ObjLibrary shader and material
//               information.
//

	void draw () const;


//
//  init
//
//  Purpose: To initialize this MeshWithShader with the
//           specified mesh.
//  Parameter(s):
//    <1> primitive_type: The type of primitive to display
//    <2> data_format: The data format
//    <3> vertex_data: The VBO containing the interleaved
//                     vertex data
//    <4> vertex_indexes: The VBO containing the vertex indexes
//  Precondition(s):
//    <1> !isInitialized()
//    <2> isValidPrimitiveType(primitive_type)
//    <3> data_format < VertexDataFormat::COUNT
//    <4> isValidDataFormatForPrimitiveType(data_format,
//                                          primitive_type)
//    <5> !vertex_data.isEmpty()
//    <6> vertex_data.getTarget() == GL_ARRAY_BUFFER
//    <7> !vertex_indexes.isEmpty()
//    <8> vertex_indexes.getTarget() == GL_ELEMENT_ARRAY_BUFFER
//  Returns: N/A
//  Side Effect: A VAO for the mesh of primative type
//               primitive_type is created on the graphics card.
//               The mesh uses the vertex data in vertex_data,
//               interprerated according to data_format, and
//               indexed by the values in vertex_indexes.
//

	void init (unsigned int primitive_type,
	           unsigned int data_format,
	           const ObjVbo<float>& vertex_data,
	           const ObjVbo<unsigned int>& vertex_indexes);

//
//  init
//
//  Purpose: To initialize this MeshWithShader with the
//           specified mesh.
//  Parameter(s):
//    <1> primitive_type: The type of primitive to display
//    <2> data_format: The data format
//    <3> vertex_data: The VBO containing the interleaved
//                     vertex data
//  Precondition(s):
//    <1> !isInitialized()
//    <2> isValidPrimitiveType(primitive_type)
//    <3> data_format < VertexDataFormat::COUNT
//    <4> isValidDataFormatForPrimitiveType(data_format,
//                                          primitive_type)
//    <5> !vertex_data.isEmpty()
//    <6> vertex_data.getTarget() == GL_ARRAY_BUFFER
//  Returns: N/A
//  Side Effect: A VAO for the mesh of primative type
//               primitive_type is created on the graphics card.
//               The mesh uses the vertex data in vertex_data,
//               interprerated according to data_format
//

	void init(unsigned int primitive_type,
		unsigned int data_format,
		const ObjVbo<float>& vertex_data);

//
//  makeEmpty
//
//  Purpose: To remove the contents of this MeshWithShader.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: The mesh for this MeshWithShader is removed,
//               restoring it to state produced by the default
//               constructor.  This may result in freeing video
//               memory.
//

	void makeEmpty ();

private:
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
	// geometry
	unsigned int m_primitive_type;
	unsigned int m_data_format;
	ObjVao m_vao;
	ObjVbo<float> m_vertex_data;
	ObjVbo<unsigned int> m_vertex_indexes;
};



}  // end of namespace ObjLibrary

#endif
