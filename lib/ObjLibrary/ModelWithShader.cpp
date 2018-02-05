//
//  ModelWithShader.cppp
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
#include <iostream>
#include <vector>

#include "ObjSettings.h"
#ifndef OBJ_LIBRARY_GLM_INTERACTION
	#error "ModelWithShader can only be used with OBJ_LIBRARY_GLM_INTERACTION #defined"
#endif
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	#error "ModelWithShader can only be used with OBJ_LIBRARY_SHADER_DISPLAY #defined"
#endif

#include "../GetGlutWithShaders.h"
#include "../glm/glm.hpp"

#include "Vector3.h"
#include "Material.h"
#include "Texture.h"
#include "ObjShader.h"
#include "ObjVbo.h"
#include "ObjVao.h"
#include "MeshWithShader.h"
#include "ModelWithShader.h"

using namespace std;
using namespace ObjLibrary;



ModelWithShader :: ModelWithShader (const MeshWithShader& mesh,
                                    const MaterialForShader& material)
	: mv_materials()
{
	assert(mv_materials.empty());
	mv_materials.push_back(material);
	assert(mv_materials.size() == 1);

	assert(0 < mv_materials.size());
	mv_materials[0].mv_meshes.push_back(mesh);
}



unsigned int ModelWithShader :: getMeshCountTotal () const
{
	unsigned int total = 0;
	for(unsigned int i = 0; i < mv_materials.size(); i++)
		total += mv_materials[i].mv_meshes.size();
	return total;
}

bool ModelWithShader :: isReady () const
{
	for(unsigned int i = 0; i < mv_materials.size(); i++)
	{
		const std::vector<MeshWithShader>& v_meshes = mv_materials[i].mv_meshes;
		for(unsigned int j = 0; j < v_meshes.size(); j++)
			if(!v_meshes[j].isInitialized())
				return false;
	}
	return true;
}

void ModelWithShader :: draw (const glm::mat4x4& model_matrix,
                              const glm::mat4x4& view_matrix,
							  const glm::mat4x4& model_view_projection_matrix,
                              const glm::vec3& camera_position) const
{
	assert(isReady());
	assert(ObjShader::isLoaded());

	// draw the meshes, grouped by material
	for(unsigned int i = 0; i < mv_materials.size(); i++)
	{
		const std::vector<MeshWithShader>& v_meshes = mv_materials[i].mv_meshes;

		if(v_meshes.empty())
			continue;  // don't activate program if nothing to draw

		mv_materials[i].m_material.activate(model_matrix,
											view_matrix,
											model_view_projection_matrix,
		                                    camera_position);
		drawCurrentMaterial(i);
	}

	ObjVao::bindNone();  // we don't want to accidentally change our meshes elsewhere
}


void ModelWithShader :: draw (const glm::mat4x4& model_matrix,
                              const glm::mat4x4& view_matrix,
							  const glm::mat4x4& model_view_projection_matrix) const
{
	assert(isReady());

	draw(model_matrix, view_matrix, model_view_projection_matrix, glm::vec3());
}



void ModelWithShader :: drawSingleMaterial (const MaterialForShader& material,
                                            const glm::mat4x4& model_matrix,
                                            const glm::mat4x4& view_matrix,
											const glm::mat4x4& model_view_projection_matrix,
                                            const glm::vec3& camera_position) const
{
	assert(isReady());
	assert(ObjShader::isLoaded());

	material.activate(model_matrix, view_matrix, model_view_projection_matrix, camera_position);
	drawCurrentMaterial();
}

void ModelWithShader :: drawSingleMaterial (const MaterialForShader& material,
                                            const glm::mat4x4& model_matrix,
                                            const glm::mat4x4& view_matrix,
											const glm::mat4x4& model_view_projection_matrix) const
{
	assert(isReady());
	assert(ObjShader::isLoaded());

	material.activate(model_matrix, view_matrix, model_view_projection_matrix);
	drawCurrentMaterial();
}

void ModelWithShader :: drawCurrentMaterial () const
{
	assert(isReady());
	assert(ObjShader::isLoaded());

	for(unsigned int i = 0; i < mv_materials.size(); i++)
		drawCurrentMaterial(i);

	ObjVao::bindNone();  // we don't want to accidentally change our meshes elsewhere
}

void ModelWithShader::drawCurrentMaterial(unsigned int material) const
{
	assert(isReady());
	assert(ObjShader::isLoaded());

	static const glm::vec2 ZERO2(0.0, 0.0);
	static const glm::vec3 ZERO3(0.0, 0.0, 0.0);

	const std::vector<MeshWithShader>& v_meshes = mv_materials[material].mv_meshes;
	for(unsigned int j = 0; j < v_meshes.size(); j++)
	{
		//
		//  We need to set the global state (used as a fallback)
		//    in case one or more of the meshes don't have
		//    texture coordinates and/or normals.
		//
		//  These can become undefined during the draw call, so
		//    we have to reset them every time.
		//

		glVertexAttrib2fv(1, &(ZERO2.x));  // texture coordinates
		glVertexAttrib3fv(2, &(ZERO3.x));  // normals

		assert(v_meshes[j].isInitialized());
		v_meshes[j].draw();
	}

	ObjVao::bindNone();  // we don't want to accidentally change our meshes elsewhere
}



unsigned int ModelWithShader :: addMaterial (const MaterialForShader& material)
{
	mv_materials.push_back(material);

	assert(mv_materials.size() >= 1);
	return mv_materials.size() - 1;
}

void ModelWithShader :: setMaterial (unsigned int index,
                                     const MaterialForShader& material)
{
	assert(index < getMaterialCount());

	mv_materials[index].m_material = material;
}

void ModelWithShader :: setSingleMaterial (const MaterialForShader& material)
{
	assert(isSingleMaterial());

	assert(mv_materials.size() == 1);
	assert(0 < mv_materials.size());
	mv_materials[0].m_material = material;
}

void ModelWithShader :: addMesh (unsigned int material,
                                 const MeshWithShader& mesh)
{
	assert(material < getMaterialCount());

	assert(material < mv_materials.size());
	mv_materials[material].mv_meshes.push_back(mesh);
}





ModelWithShader :: MaterialWithMeshes :: MaterialWithMeshes ()
		: m_material(),
		  mv_meshes()
{
}

ModelWithShader :: MaterialWithMeshes :: MaterialWithMeshes (const MaterialForShader& material)
		: m_material(material),
		  mv_meshes()
{
}

ModelWithShader :: MaterialWithMeshes :: MaterialWithMeshes (const MaterialWithMeshes& original)
		: m_material(original.m_material),
		  mv_meshes(original.mv_meshes)
{
}

ModelWithShader :: MaterialWithMeshes& ModelWithShader :: MaterialWithMeshes :: operator= (const MaterialWithMeshes& original)
{
	if(&original != this)
	{
		m_material = original.m_material;
		mv_meshes  = original.mv_meshes;
	}
	return *this;
}
