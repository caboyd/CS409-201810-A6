#include "PickupManager.h"
#include "Collision.h"
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "LineRenderer.h"
#include "DepthTexture.h"
#include "World.h"
#include "cassert"

extern LineRenderer g_line_renderer;
extern DepthTexture g_depth_texture;

PickupManager::PickupManager()
{}

void PickupManager::init(World* w, MovementGraph* mg, ModelWithShader* rod, ModelWithShader* ring)
{
	assert(w->isInitialized());

	score = 0;
	world = w;
	world_graph = mg;
	rod_model = rod;
	ring_model = ring;

	//Init disks and rods
	for (auto& ptr : world->disks)
	{
		Vector3 pos = ptr->position;
		pos.y = world->getHeightAtPointPosition(float(pos.x), float(pos.z));
		addRod(pos, ptr->type + 1);
		addRing();
	}
}

void PickupManager::update(const double delta_time)
{
	for (auto& ring : rings)
	{
		if (!ring.pickedUp)
			ring.update(delta_time);
	}
}

void PickupManager::checkForPickups(Vector3 player_position)
{
	for (auto& ring : rings)
	{
		if (!ring.pickedUp)
			if (Collision::cylinderIntersection(player_position, 0.25f, 0.8f, ring.position, ring.radius, ring.halfHeight))
			{
				ring.pickedUp = true;
				score += ring.pointValue;
			}
	}

	for (auto& rod : rods)
	{
		if (!rod.pickedUp)
			if (Collision::cylinderIntersection(player_position, 0.25f, 0.8f, rod.position, rod.radius, rod.halfHeight))
			{
				rod.pickedUp = true;
				score += rod.pointValue;
			}
	}
}

int PickupManager::getScore() const
{
	return score;
}

void PickupManager::addRod(Vector3 pos, unsigned score_value)
{
	rods.emplace_back(rod_model, pos, score_value);
}

void PickupManager::addRing()
{
	rings.emplace_back(world, world_graph, ring_model);
}

void PickupManager::draw(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix) const
{
	draw(view_matrix, projection_matrix, glm::vec3(0, 0, 0));
}

void PickupManager::draw(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix, const glm::vec3& camera_pos) const
{
	//Call draw on each rod
	for (auto const& rod : rods)
	{
		if (rod.pickedUp) continue;
		glm::mat4x4 model_matrix = glm::mat4();
		model_matrix = glm::translate(model_matrix, glm::vec3(rod.position));
		glm::mat4x4 mvp_matrix = projection_matrix * view_matrix *  model_matrix;

		rod_model->draw(model_matrix, view_matrix, mvp_matrix, camera_pos);
	}
	for (auto const& ring : rings)
	{
		if (ring.pickedUp) continue;
		glm::mat4x4 model_matrix = glm::mat4();
		model_matrix = glm::translate(model_matrix, glm::vec3(ring.position));
		model_matrix = glm::rotate(model_matrix, (float(atan2(ring.forward.x, ring.forward.z))), glm::vec3(0, 1, 0));
		glm::mat4x4 mvp_matrix = projection_matrix * view_matrix *  model_matrix;
		ring_model->draw(model_matrix, view_matrix, mvp_matrix, camera_pos);
	}

	//std::vector<Vector3> points;

	//Vector3 pos = rings[0].position;
	//points.push_back(pos);
	//pos.y += 1.0f;
	//points.push_back(pos);
	//Vector3 tar = rings[0].targetPosition;
	//tar.y += 1.0f;
	//points.push_back(tar);
	//tar.y -= 1.0f;
	//points.push_back(tar);

	//glm::mat4x4 mvp_matrix = projection_matrix * view_matrix;
	//g_line_renderer.draw(points, glm::vec4(1.0, 1.0, 1.0, 1.0), mvp_matrix);


}

void PickupManager::drawOptimized(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix)
{
	drawOptimized(view_matrix, projection_matrix, glm::vec3(0, 0, 0));
}


void PickupManager::drawOptimized(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix, const glm::vec3& camera_pos)
{
	const ObjLibrary::ObjShader::ShaderUniforms& uniforms = ObjLibrary::ObjShader::activateShader();
	rod_model->getMaterial(0).activate(uniforms);;

	//Call draw on each rod
	for (auto const& rod : rods)
	{
		if (rod.pickedUp) continue;
		glm::mat4x4 model_matrix = glm::mat4();
		model_matrix = glm::translate(model_matrix, glm::vec3(rod.position));
		glm::mat4x4 mvp_matrix = projection_matrix * view_matrix *  model_matrix;

		glUniformMatrix4fv(uniforms.m_model_matrix, 1, false, &(model_matrix[0][0]));
		glUniformMatrix4fv(uniforms.m_model_view_projection_matrix, 1, false, &(mvp_matrix[0][0]));
		rod_model->drawCurrentMaterial(0);
	}

	ring_model->getMaterial(0).activate(uniforms);
	//Call draw on each ring
	for (auto const& ring : rings)
	{
		if (ring.pickedUp) continue;
		glm::mat4x4 model_matrix = glm::mat4();
		model_matrix = glm::translate(model_matrix, glm::vec3(ring.position));
		model_matrix = glm::rotate(model_matrix, (float(atan2(ring.forward.x, ring.forward.z))), glm::vec3(0, 1, 0));
		glm::mat4x4 mvp_matrix = projection_matrix * view_matrix *  model_matrix;

		glUniformMatrix4fv(uniforms.m_model_matrix, 1, false, &(model_matrix[0][0]));
		glUniformMatrix4fv(uniforms.m_model_view_projection_matrix, 1, false, &(mvp_matrix[0][0]));
		ring_model->drawCurrentMaterial(0);
	}

	//std::vector<Vector3> points;

	//Vector3 pos = rings[0].position;
	//points.push_back(pos);
	//pos.y += 1.0f;
	//points.push_back(pos);
	//Vector3 tar = rings[0].targetPosition;
	//tar.y += 1.0f;
	//points.push_back(tar);
	//tar.y -= 1.0f;
	//points.push_back(tar);

	//glm::mat4x4 mvp_matrix = projection_matrix * view_matrix;
	//g_line_renderer.draw(points, glm::vec4(1.0, 1.0, 1.0, 1.0), mvp_matrix);
}

void PickupManager::drawDepth(glm::mat4x4& depth_view_projection_matrix)
{
		MeshWithShader mesh = rings[0].model->getMesh(0, 0);

	for (auto const& ring : rings)
	{
		if (ring.pickedUp) continue;
		glm::mat4x4 model_matrix = glm::mat4();
		model_matrix = glm::translate(model_matrix, glm::vec3(ring.position));
		model_matrix = glm::rotate(model_matrix, (float(atan2(ring.forward.x, ring.forward.z))), glm::vec3(0, 1, 0));
		glm::mat4x4 depth_mvp = depth_view_projection_matrix * model_matrix;
		g_depth_texture.setDepthMVP(depth_mvp);
		mesh.draw();
	}
	
	mesh = rods[0].model->getMesh(0, 0);
	for (auto const& rod : rods)
	{
		if (rod.pickedUp) continue;
		glm::mat4x4 model_matrix = glm::mat4();
		model_matrix = glm::translate(model_matrix, glm::vec3(rod.position));
		model_matrix = glm::rotate(model_matrix, (float(atan2(rod.forward.x, rod.forward.z))), glm::vec3(0, 1, 0));
		glm::mat4x4 depth_mvp = depth_view_projection_matrix * model_matrix;
		g_depth_texture.setDepthMVP(depth_mvp);
		mesh.draw();
	}
}

void PickupManager::destroy()
{
	rings.clear();
	rods.clear();
	score = 0;
}
