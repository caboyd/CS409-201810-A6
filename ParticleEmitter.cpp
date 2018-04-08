#include "ParticleEmitter.h"

void ParticleEmitter::init()
{
	//Init shader
	shader_program_id = ObjLibrary::ObjShader::initProgramStart(shader_folder + shader_vert,
		shader_folder + shader_frag);
	ObjLibrary::ObjShader::initProgramEnd(shader_program_id);

	view_projection_matrix_id = glGetUniformLocation(shader_program_id, "view_projection_matrix");
	camera_right_worldspace_id = glGetUniformLocation(shader_program_id, "camera_right_worldspace");
	camera_up_worldspace_id = glGetUniformLocation(shader_program_id, "camera_up_worldspace");

	// The VBO containing the 4 vertices of the particles.
	static const GLfloat vertex_buffer_data[] = {
	 -0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 -0.5f, 0.5f, 0.0f,
	 0.5f, 0.5f, 0.0f,
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

	// The VBO containing the positions and sizes of the particles
	glGenBuffers(1, &VBO_particles);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_particles);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 8 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//position
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_particles);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 32, (void*)0);
	
	//size
	glEnableVertexAttribArray(2);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO_particles);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 32, (void*)12);
	
	//color
	glEnableVertexAttribArray(3);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO_particles);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 32, (void*)16);

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(3, 1);

	glBindVertexArray(0);


}

void ParticleEmitter::addEffect(unsigned num_particles, glm::vec3 position, float size, glm::vec4 color, float duration,
	Particle_Pattern pattern, float velocity_factor, float gravity_factor)
{
	unsigned p_i = findUnusedParticleIndex();
	for (unsigned i = 0; i < num_particles; i++)
	{
		if (p_i + i > MAX_PARTICLES) p_i = findUnusedParticleIndex() - i;

		Particle& p = particles[p_i + i];
		p.data.position = position;
		p.data.size = size;
		p.data.color = color;
		p.duration_ms = duration;
		p.life = duration;
		p.gravity_factor = gravity_factor;

		if (pattern == Particle_Pattern::Random)
		{
			p.velocity = glm::normalize(glm::vec3(Random::randf(-1, 1), Random::randf(-1, 1), Random::randf(-1, 1))) * velocity_factor *  Random::randf(0,1);
		}else if (pattern == Particle_Pattern::Up)
		{
			p.velocity = glm::normalize(glm::vec3(Random::randf(-1, 1), Random::randf(Random::randf(0,0.7f), 1), Random::randf(-1, 1))) * velocity_factor * Random::randf(0,1);
		}


	}
}

void ParticleEmitter::update(double delta_time_ms, const glm::vec3& camera_position)
{
	particle_count = 0;
	for (int i = 0; i < MAX_PARTICLES; i++)
	{

		Particle& p = particles[i]; // shortcut

		if (p.life <= 0.0f) continue;

		// Decrease life
		p.life -= float(delta_time_ms);
		if (p.life > 0.0f)
		{

			// Simulate simple physics : gravity only, no collisions
			p.velocity += glm::vec3(0.0f, -9.80f/1000.0, 0.0f) * float(delta_time_ms) * p.gravity_factor;
			p.data.position += p.velocity * float(delta_time_ms) / 1000.0f;
			p.camera_distance = glm::length2(p.data.position - camera_position);
			p.data.color.a = p.life/ p.duration_ms;
			//ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

			// Fill the GPU buffer
			for (unsigned j = 0; j < 8; j++)
				particle_data[8 * particle_count + j] = p.array_data[j];

			
		} else
		{
			// Particles that just died will be put at the end of the buffer in SortParticles();
			p.camera_distance = -1.0f;
		}

		particle_count++;


	}

	//glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_particles);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 8 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, particle_count * sizeof(GLfloat) * 8, particle_data);
}

void ParticleEmitter::draw(const glm::mat4& view_matrix, const glm::mat4& projection_matrix) const
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

	glUseProgram(shader_program_id);
	glUniformMatrix4fv(view_projection_matrix_id, 1, GL_FALSE, glm::value_ptr(projection_matrix * view_matrix));
	glUniform3f(camera_right_worldspace_id, view_matrix[0][0], view_matrix[1][0], view_matrix[2][0]);
	glUniform3f(camera_up_worldspace_id, view_matrix[0][1], view_matrix[1][1], view_matrix[2][1]);

	glBindVertexArray(VAO);

	//glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particle_count);

	glBindVertexArray(0);
}

unsigned ParticleEmitter::findUnusedParticleIndex()
{

	for (unsigned i = last_used_particle; i < MAX_PARTICLES; i++)
	{
		if (particles[i].duration_ms <= 0)
		{
			last_used_particle = i;
			return i;
		}
	}

	for (unsigned i = 0; i < last_used_particle; i++)
	{
		if (particles[i].duration_ms <= 0)
		{
			last_used_particle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}
