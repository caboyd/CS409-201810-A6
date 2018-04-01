#pragma once
#include "Entity.h"
#include "World.h"
#include "Player.h"


enum class Bat_State
{
	DEAD,
	PURSUE,
	EXPLORE
};

class Bat: public Entity

{
public:
	Player const* player;
	World const* world;
	ModelWithShader const* model;
	CoordinateSystem coordinate_system;
	Bat_State state;
	
	Vector3 velocity;

	Vector3 target_position;

	//Collision properties
	const float radius = 0.5f;
	const float half_height = 0.05f;
	const float S_MAX = 5.0f;
	const float S_MIN = 1.0f;
	const float A_MAX = 8.0f;

	//A timer in milliseconds
	//When bat hits a player the time is set to 1 second
	//Bat will ignore player during this time
	float ignore_timer = 0;

	Bat(const ModelWithShader& model, const Player& player, const World& world, const Vector3& position);
	~Bat() = default;

	void update(double delta_time) override;

	void draw(const glm::mat4x4& view_matrix,
		const glm::mat4x4& projection_matrix,
		const glm::vec3& camera_position) const;

	void drawToDepth(const glm::mat4x4& depth_view_projection_matrix) const;

	void seek(double delta_time_seconds);
	void explore(double delta_time_seconds);
	void pursue(double delta_time_seconds);
};
