//
//  KeyframeModelWithShader.cpp
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
#include "KeyframeModelWithShader.h"

using namespace std;
using namespace ObjLibrary;



void KeyframeModelWithShader :: draw (const float tween_factor, 
							  const glm::mat4x4& model_matrix,
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

		mv_materials[i].m_material.activate(tween_factor,
											model_matrix,
											view_matrix,
											model_view_projection_matrix,
		                                    camera_position);
		drawCurrentMaterial(i);
	}

	ObjVao::bindNone();  // we don't want to accidentally change our meshes elsewhere
}


void KeyframeModelWithShader :: draw (const float tween_factor,
	                          const glm::mat4x4& model_matrix,
                              const glm::mat4x4& view_matrix,
							  const glm::mat4x4& model_view_projection_matrix) const
{
	assert(isReady());

	draw(tween_factor, model_matrix, view_matrix, model_view_projection_matrix, glm::vec3());
}



void KeyframeModelWithShader :: drawSingleMaterial (const MaterialForShader& material,
	                                        const float tween_factor,
                                            const glm::mat4x4& model_matrix,
                                            const glm::mat4x4& view_matrix,
											const glm::mat4x4& model_view_projection_matrix,
                                            const glm::vec3& camera_position) const
{
	assert(isReady());
	assert(ObjShader::isLoaded());

	material.activate(tween_factor, model_matrix, view_matrix, model_view_projection_matrix, camera_position);
	drawCurrentMaterial();
}

void KeyframeModelWithShader :: drawSingleMaterial (const MaterialForShader& material,
	                                        const float tween_factor,
                                            const glm::mat4x4& model_matrix,
                                            const glm::mat4x4& view_matrix,
											const glm::mat4x4& model_view_projection_matrix) const
{
	assert(isReady());
	assert(ObjShader::isLoaded());

	material.activate(tween_factor, model_matrix, view_matrix, model_view_projection_matrix);
	drawCurrentMaterial();
}

