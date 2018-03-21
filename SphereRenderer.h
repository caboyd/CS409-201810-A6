#pragma once
#include "lib/ObjLibrary/ModelWithShader.h"
#include "lib/ObjLibrary/ObjModel.h"
#include "lib/ObjLibrary/Material.h"
using namespace ObjLibrary;

class SphereRenderer
{
	ObjLibrary::ModelWithShader sphere;
	ObjLibrary::MaterialForShader mat;

public:
	void init()
	{
		ObjModel temp;
		temp.load("assets/Models/sphere.obj");
		sphere = temp.getModelWithShader(false, false);
		mat.initDefault();
	}


	void draw(const glm::vec3& color, const glm::mat4x4& model_matrix,
		const glm::mat4x4& view_matrix,
		const glm::mat4x4& model_view_projection_matrix)const
	{

		const ObjShader::ShaderUniforms t_uniforms = mat.activate(model_matrix,view_matrix,model_view_projection_matrix);
		glUniform3f(t_uniforms.m_emission, color.x, color.y, color.z);
		glUniform3f(t_uniforms.m_ambient	, color.x, color.y, color.z);
		glUniform3f(t_uniforms.m_diffuse, color.x, color.y, color.z);
		glUniform1f(t_uniforms.m_transparency, 1.0f);
		sphere.drawCurrentMaterial(0);
	//	sphere.draw(model_matrix,view_matrix,model_view_projection_matrix);
	}
};
