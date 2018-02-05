#include "Disk.h"
#include "lib/ObjLibrary/VertexDataFormat.h"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "PerformanceCounter.h"

void Disk::generateHeightMapModel()
{
	assert(heightMapSize % 2 == 0);
	assert(!heightMap.empty());

	const unsigned int vert_buffer_size = (heightMapSize + 1) * (heightMapSize + 1);

	auto* verts = new ObjLibrary::VertexDataFormat::PositionTextureCoordinateNormal[vert_buffer_size];
	int count = 0;
	for (unsigned int x0 = 0; x0 < (heightMapSize + 1); x0++)
	{
		for (unsigned int z = 0; z < (heightMapSize + 1); z++)
		{
			//Position
			verts[count].m_x = (float)x0 - float(heightMapSize/2); verts[count].m_y = heightMap[x0][z]; verts[count].m_z = (float)z - float(heightMapSize/2);
			//Texture coords
			verts[count].m_s = (float)x0 / 16.0f; verts[count].m_t = (float)z / 16.0f;
			//Normals are 0, will be calculated later
			verts[count].m_nx = 0; verts[count].m_ny = 0; verts[count].m_nz = 0;
			count++;
		}
	}

	const unsigned int index_buffer_size = heightMapSize * (heightMapSize) * 6;
	auto* indices = new unsigned int[index_buffer_size];
	count = 0;

	//Create triangles using indices which reference vertices in the triangle strip
	for (unsigned int i = 0; i < vert_buffer_size - (heightMapSize + 1); i += (heightMapSize + 1))
	{
		for (unsigned int j = 0; j < heightMapSize; j++)
		{
			const unsigned int v = i + j;

			//0,1,2
			indices[count++] = v + (heightMapSize + 1);
			indices[count++] = v;
			indices[count++] = v + (heightMapSize + 1) + 1;

			//2,1,3
			indices[count++] = v + (heightMapSize + 1) + 1;
			indices[count++] = v;
			indices[count++] = v + 1;
		}

	}

	//Calculate the normals. For each faces normal we add it to the normals of the vertices used.
	//Normals will be normalized in fragment shader
	for (unsigned int i = 0; i < index_buffer_size - 2; i += 3)
	{
		const int i1 = indices[i];
		const int i2 = indices[i + 1];
		const int i3 = indices[i + 2];

		//Calculate normal for this face
		Vector3 v1 = Vector3(verts[i1].m_x, verts[i1].m_y, verts[i1].m_z);
		Vector3 v2 = Vector3(verts[i2].m_x, verts[i2].m_y, verts[i2].m_z);
		Vector3 v3 = Vector3(verts[i3].m_x, verts[i3].m_y, verts[i3].m_z);
		Vector3 dir = (v2 - v1).crossProduct(v3 - v1);

		//Add to the existing normals
		verts[i1].m_nx += (float)dir.x; verts[i1].m_ny += (float)dir.y; verts[i1].m_nz += (float)dir.z;
		verts[i2].m_nx += (float)dir.x; verts[i2].m_ny += (float)dir.y; verts[i2].m_nz += (float)dir.z;
		verts[i3].m_nx += (float)dir.x; verts[i3].m_ny += (float)dir.y; verts[i3].m_nz += (float)dir.z;
	}

	//Generate a vbo with the postion, tex coord, normal data
	ObjLibrary::ObjVbo<float> vbo_vertex_data;
	vbo_vertex_data.init(GL_ARRAY_BUFFER,
		vert_buffer_size * 8,
		reinterpret_cast<float*>(verts),
		GL_STATIC_DRAW);

	//Generate a cbo with the index data
	ObjLibrary::ObjVbo<unsigned int> vbo_index_data;
	vbo_index_data.init(GL_ELEMENT_ARRAY_BUFFER,
		index_buffer_size,
		indices,
		GL_STATIC_DRAW);

	//Get the material for the top of the disk
	ObjLibrary::MaterialForShader mat = model->getMaterial(2);
	//Initialize the mesh
	ObjLibrary::MeshWithShader mesh;
	mesh.init(GL_TRIANGLES,
		ObjLibrary::VertexDataFormat::POSITION_TEXTURE_COORDINATE_NORMAL,
		vbo_vertex_data,
		vbo_index_data);

	//Add material and mesh to model
	heightMapModel.addMaterial(mat);
	heightMapModel.addMesh(0, mesh);

	delete[] verts;
	delete[] indices;
}

void Disk::draw(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix) const
{
	glm::mat4x4 model_matrix = glm::mat4();
	model_matrix = glm::translate(model_matrix, glm::vec3(position));
	model_matrix = glm::scale(model_matrix, glm::vec3(radius, 1, radius));
	glm::mat4x4 mvp_matrix = projection_matrix * view_matrix *  model_matrix;
	model->draw(model_matrix, view_matrix, mvp_matrix);

	if (heightMapModel.getMeshCountTotal() > 0)
	{
		model_matrix = glm::mat4();
		glm::vec3 pos = position;
		const float corner = float(radius * 0.70710678118);
		//Fix for overlapping faces causing flicker
		pos.y += 0.00001f;

		model_matrix = glm::translate(model_matrix, pos);
		model_matrix = glm::scale(model_matrix, glm::vec3(corner * 2 / heightMapSize, 1, corner * 2 / heightMapSize));
		mvp_matrix = projection_matrix * view_matrix *  model_matrix;

		heightMapModel.draw(model_matrix, view_matrix, mvp_matrix);
	}

}

void Disk::drawDepth(const unsigned int depth_matrix_id, const glm::mat4x4& depth_view_projection_matrix) const
{
	glm::mat4x4 model_matrix = glm::mat4();
	model_matrix = glm::translate(model_matrix, glm::vec3(position));
	model_matrix = glm::scale(model_matrix, glm::vec3(radius, 1, radius));

	glm::mat4x4 depth_mvp = depth_view_projection_matrix * model_matrix;
	glUniformMatrix4fv(depth_matrix_id, 1, GL_FALSE, &depth_mvp[0][0]);

	unsigned int mat_count = model->getMaterialCount();
	for (unsigned int i = 0; i < mat_count; i++)
	{
		const unsigned int mesh_count = model->getMeshCount(i);
		for (unsigned int j = 0; j < mesh_count; j++)
		{
			const ObjLibrary::MeshWithShader& m = model->getMesh(i, j);
			m.draw();
		}

	}



	if (heightMapModel.getMeshCountTotal() > 0)
	{
		model_matrix = glm::mat4();
		glm::vec3 pos = position;
		const float corner = float(radius * 0.70710678118);

		model_matrix = glm::translate(model_matrix, pos);
		model_matrix = glm::scale(model_matrix, glm::vec3(corner * 2 / heightMapSize, 1, corner * 2 / heightMapSize));

		depth_mvp = depth_view_projection_matrix * model_matrix;
		glUniformMatrix4fv(depth_matrix_id, 1, GL_FALSE, &depth_mvp[0][0]);

		mat_count = heightMapModel.getMaterialCount();
		for (unsigned int i = 0; i < mat_count; i++)
		{
			const unsigned int mesh_count = heightMapModel.getMeshCount(i);
			for (unsigned int j = 0; j < mesh_count; j++)
			{
				const ObjLibrary::MeshWithShader& m = heightMapModel.getMesh(i, j);
				m.draw();
			}

		}
	}
}
