#pragma once
#include "lib/glm/glm.hpp"
#include "lib/glm/gtx/norm.hpp"
#include "lib/glm/gtc/type_ptr.hpp"
#include "lib/GetGlutWithShaders.h"
#include "lib/ObjLibrary/ObjShader.h"
#include "lib/ObjLibrary/Vector3.h"
#include "Random.h"


constexpr unsigned MAX_PARTICLES = 100000;

enum Particle_Pattern
{
	Random,
	Up,
};

#pragma pack(push, 1)
struct Particle_Data
{
	glm::vec3 position{ 0.0f };
	float size{};
	glm::vec4 color{ 1.0f };
	Particle_Data() = default;
};
#pragma pack(pop)

struct Particle
{
	union
	{
		struct Particle_Data data {};
		float array_data[8];
	};
	glm::vec3 velocity{ 0.0f };
	float duration_ms{};
	float life{};
	float camera_distance{};
	float gravity_factor{};
	Particle() {};
};

class ParticleEmitter
{

	const std::string shader_folder = "assets/shaders/";
	const std::string shader_vert = "particle.vert";
	const std::string shader_frag = "particle.frag";

	GLuint VAO{};
	GLuint VBO_vertex{}, VBO_particles{};
	GLuint shader_program_id{};

	//Uniform location
	GLuint camera_right_worldspace_id{};
	GLuint camera_up_worldspace_id{};
	GLuint view_projection_matrix_id{};

	Particle particles[MAX_PARTICLES];
	float particle_data[MAX_PARTICLES * 8] = {};
	unsigned particle_count{};
	unsigned last_used_particle{};

public:
	ParticleEmitter() = default;

	void init();

	void addEffect(unsigned num_particles, glm::vec3 position, float size, glm::vec4 color, float duration, Particle_Pattern pattern, float velocity_factor, float gravity_factor);

	void update(double delta_time_ms, const glm::vec3& camera_position);

	void draw(const glm::mat4& view_matrix, const glm::mat4& projection_matrix) const;
private:
	unsigned findUnusedParticleIndex();

};




