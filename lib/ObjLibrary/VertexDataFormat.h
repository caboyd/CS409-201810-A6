//
//  VertexDataFormat.h
//
//  A module to define the various datastructures needed for
//    vertex data for shaders.
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

#ifndef OBJ_LIBRARY_VERTEX_DATA_FORMAT_H
#define OBJ_LIBRARY_VERTEX_DATA_FORMAT_H



namespace ObjLibrary
{

//
//  VertexDataFormat
//
//  A namespace to define the various datastructures needed for
//    vertex data for shaders.
//
//  The vertex data is stored in an interleaved manner.
//

namespace VertexDataFormat
{
	//
	//  COUNT
	//
	//  The number of data formats.
	//

	static const unsigned int COUNT = 4;

	//
	//  POSITION_ONLY
	//
	//  The data format for a position with no other data.
	//
	//  Data in this format is represented by the PositionOnly
	//    record.
	//

	static const unsigned int POSITION_ONLY = 0;

	//
	//  POSITION_TEXTURE_COORDINATE
	//
	//  The data format for a position and a texture coorinate
	//    pair.
	//
	//  Data in this format is represented by the
	//    PositionTextureCoordinate record.
	//

	static const unsigned int POSITION_TEXTURE_COORDINATE = 1;

	//
	//  POSITION_NORMAL
	//
	//  The data format for a position and a vertex normal.
	//
	//  Data in this format is represented by the PositionNormal
	//    record.
	//

	static const unsigned int POSITION_NORMAL = 2;

	//
	//  POSITION_TEXTURE_COORDINATE_NORMAL
	//
	//  The data format for a position, a texture coorinate
	//    pair, and a vertex normal.
	//
	//  Data in this format is represented by the
	//    PositionTextureCoordinateNormal record.
	//

	static const unsigned int
	                     POSITION_TEXTURE_COORDINATE_NORMAL = 3;



	//
	//  PositionOnly
	//
	//  A record to store a vertex with a position and nothing
	//    else.  It is used for the POSITION_ONLY format.
	//

	struct PositionOnly
	{
		float m_x;
		float m_y;
		float m_z;
	};

	//
	//  PositionTextureCoordinate
	//
	//  A record to store a vertex with a position and a
	//    texture coordinate pair.  It is used for the
	//    POSITION_TEXTURE_COORDINATE format.
	//

	struct PositionTextureCoordinate
	{
		float m_x;
		float m_y;
		float m_z;

		float m_s;
		float m_t;
	};

	//
	//  PositionNormal
	//
	//  A record to store a vertex with a position and a
	//    vertex normal.  It is used for the POSITION_NORMAL
	//    format.
	//

	struct PositionNormal
	{
		float m_x;
		float m_y;
		float m_z;

		float m_nx;
		float m_ny;
		float m_nz;
	};

	//
	//  PositionTextureCoordinateNormal
	//
	//  A record to store a vertex with a position, a
	//    texture coordinate pair, and a vertex normal.  It is
	//    used for the POSITION_TEXTURE_COORDINATE_NORMAL
	//    format.
	//

	struct PositionTextureCoordinateNormal
	{
		float m_x;
		float m_y;
		float m_z;

		float m_s;
		float m_t;

		float m_nx;
		float m_ny;
		float m_nz;
	};



//
//  getComponentCount
//
//  Purpose: To determine the number of components for the
//           specified data format.
//  Parameter(s):
//    <1> data_format: The data format to test
//  Precondition(s):
//    <1> data_format < COUNT
//  Returns: The number of components for a vertex with the
//           specified data format.
//  Side Effect: N/A
//

	inline unsigned int getComponentCount (unsigned int data_format)
	{
		assert(data_format < COUNT);

		static const unsigned int A_COMPONENT_COUNT[COUNT] =
		{	3,	5,	6,	8	};
		assert(A_COMPONENT_COUNT[POSITION_ONLY] ==
		       sizeof(PositionOnly) / sizeof(float));
		assert(A_COMPONENT_COUNT[POSITION_TEXTURE_COORDINATE] ==
		       sizeof(PositionTextureCoordinate) / sizeof(float));
		assert(A_COMPONENT_COUNT[POSITION_NORMAL] ==
		       sizeof(PositionNormal) / sizeof(float));
		assert(A_COMPONENT_COUNT[POSITION_TEXTURE_COORDINATE_NORMAL] ==
		       sizeof(PositionTextureCoordinateNormal) / sizeof(float));

		return A_COMPONENT_COUNT[data_format];
	}

//
//  getStride
//
//  Purpose: To determine thenumber of components for the
//           specified data format.
//  Parameter(s):
//    <1> data_format: The data format to test
//  Precondition(s):
//    <1> data_format < COUNT
//  Returns: The number of components for a vertex with the
//           specified data format.
//  Side Effect: N/A
//

	inline unsigned int getStride (unsigned int data_format)
	{
		assert(data_format < COUNT);

		static const unsigned int A_COMPONENT_COUNT[COUNT] =
		{
			sizeof(PositionOnly),
			sizeof(PositionTextureCoordinate),
			sizeof(PositionNormal),
			sizeof(PositionTextureCoordinateNormal),
		};

		return A_COMPONENT_COUNT[data_format];
	}

};



}  // end of namespace ObjLibrary

#endif
