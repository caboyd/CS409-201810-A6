
/**
* Represents the 3D cuboidal area of the world in which objects will cast
* shadows (basically represents the orthographic projection area for the shadow
* render pass). It is updated each frame to optimise the area, making it as
* small as possible (to allow for optimal shadow map resolution) while not
* being too small to avoid objects not having shadows when they should.
* Everything inside the cuboidal area represented by this object will be
* rendered to the shadow map in the shadow render pass. Everything outside the
* area won't be.
*
* @author Karl
* Site: https://www.dropbox.com/sh/g9vnfiubdglojuh/AACpq1KDpdmB8ZInYxhsKj2Ma/shadows?dl=0
* Youtube tutorial:
*		https://www.youtube.com/watch?v=o6zDfDkOFIc
*		https://www.youtube.com/watch?v=9sEHkT7N7RM
*
* Modified by Chris Boyd
* Date Jan 27,2018
*
*/
#pragma once
#include "lib/ObjLibrary/Vector3.h"
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/type_ptr.hpp"
#include "CoordinateSystem.h"
using namespace ObjLibrary;

#define DEFAULT_SHADOW_DISTANCE 25.0



extern int g_win_width;
extern int g_win_height;

class ShadowBox
{

public:
	float SHADOW_DISTANCE;

	const float FOV = 45.0f;
	const float OFFSET = 4.0f;
	const float CLIP_NEAR = 1.0f;
	const float CLIP_FAR = 96.0f;
	const float TRANSITION_DISTANCE = 5.0f;

	glm::vec4 UP = glm::vec4(0, 1, 0, 0);
	glm::vec4 FORWARD = glm::vec4(0, 0, -1, 0);

	float min_x, max_x;
	float min_y, max_y;
	float min_z, max_z;
	glm::mat4x4* light_view_matrix;
	CoordinateSystem** cam;

	float far_height, far_width, near_height, near_width;
public:

	/**
	* Creates a new shadow box and calculates some initial values relating to
	* the camera's view frustum, namely the width and height of the near plane
	* and (possibly adjusted) far plane.
	*
	* @param light_view_matrix
	*            - basically the "view matrix" of the light. Can be used to
	*            transform a point from world space into "light" space (i.e.
	*            changes a point's coordinates from being in relation to the
	*            world's axis to being in terms of the light's local axis).
	* @param camera
	*            - pointer to the pointer of the in-game camera. So it can be swapped.
	* @param shadow_distance
	*			 - The edge of the shadow box, how far we want to see shadows.
	*			 a higher value will have lower quality shadows.
	*/
	void init(glm::mat4x4* light_view_matrix, CoordinateSystem** camera, const float shadow_distance = DEFAULT_SHADOW_DISTANCE)
	{
		this->SHADOW_DISTANCE = shadow_distance;
		this->light_view_matrix = light_view_matrix;
		this->cam = camera;
		calculateWidthsAndHeights();
	}
	ShadowBox() = default;
	/**
	* Updates the bounds of the shadow box based on the light direction and the
	* camera's view frustum, to make sure that the box covers the smallest area
	* possible while still ensuring that everything inside the camera's view
	* (within a certain range) will cast shadows.
	*/
	void update()
	{
		//const glm::mat4 rotation = glm::transpose(calculateCameraRotationMatrix());

		const Vector3 forward_vector = (*cam)->getForward();
		const Vector3 up_vector = (*cam)->getUp();
		const Vector3 to_far = forward_vector * SHADOW_DISTANCE;
		const Vector3 to_near = forward_vector * (CLIP_NEAR );

		const Vector3 center_near = to_near + (*cam)->getPosition();
		const Vector3 center_far = to_far + (*cam)->getPosition();

		glm::vec4* points = calculateFrustumVertices(up_vector, forward_vector, center_near,
			center_far);

		bool first = true;
		for (unsigned int i = 0; i < 8; i++)
		{
			if (first)
			{
				min_x = points[i].x;
				max_x = points[i].x;
				min_y = points[i].y;
				max_y = points[i].y;
				min_z = points[i].z;
				max_z = points[i].z;
				first = false;
				continue;
			}
			if (points[i].x > max_x)
			{
				max_x = points[i].x;
			} else if (points[i].x < min_x)
			{
				min_x = points[i].x;
			}
			if (points[i].y > max_y)
			{
				max_y = points[i].y;
			} else if (points[i].y < min_y)
			{
				min_y = points[i].y;
			}
			if (points[i].z > max_z)
			{
				max_z = points[i].z;
			} else if (points[i].z < min_z)
			{
				min_z = points[i].z;
			}
		}
		max_z += OFFSET;
	}

	/**
	* Calculates the center of the "view cuboid" in light space first, and then
	* converts this to world space using the inverse light's view matrix.
	*
	* @return The center of the "view cuboid" in world space.
	*/
	Vector3 getCenter() const
	{
		const float x = (min_x + max_x) / 2.0f;
		const float y = (min_y + max_y) / 2.0f;
		const float z = (min_z + max_z) / 2.0f;
		const glm::vec4 cen = glm::vec4(x, y, z, 1);

		const glm::mat4x4 inverted_light = glm::inverse(*light_view_matrix);
		Vector3 result(inverted_light * cen);
		return result;
	}

	/**
	* @return The width of the "view cuboid" (orthographic projection area).
	*/
	float getWidth() const
	{
		return max_x - min_x;
	}

	/**
	* @return The height of the "view cuboid" (orthographic projection area).
	*/
	float getHeight() const
	{
		return max_y - min_y;
	}

	/**
	* @return The length of the "view cuboid" (orthographic projection area).
	*/
	float getLength() const
	{
		return max_z - min_z;
	}

	/**
	* Calculates the position of the vertex at each corner of the view frustum
	* in light space (8 vertices in total, so this returns 8 positions).
	*
	* @param rotation
	*            - camera's rotation.
	* @param forward_vector
	*            - the direction that the camera is aiming, and thus the
	*            direction of the frustum.
	* @param center_near
	*            - the center point of the frustum's near plane.
	* @param center_far
	*            - the center point of the frustum's (possibly adjusted) far
	*            plane.
	* @return The positions of the vertices of the frustum in light space.
	*/
	glm::vec4* calculateFrustumVertices(const Vector3& up_vector, const Vector3& forward_vector,
		const Vector3& center_near, const Vector3& center_far) const
	{

		const Vector3 right_vector = forward_vector.crossProduct(up_vector);
		const Vector3 down_vector = -up_vector;
		const Vector3 left_vector = -right_vector;
		const Vector3 far_top = center_far + up_vector * far_height;
		const Vector3 far_bottom = center_far + down_vector * far_height;
		const Vector3 near_top = center_near + up_vector * near_height;
		const Vector3 near_bottom = center_near + down_vector *  near_height;

		auto* points = new glm::vec4[8];
		points[0] = calculateLightSpaceFrustumCorner(far_top, right_vector, far_width);
		points[1] = calculateLightSpaceFrustumCorner(far_top, left_vector, far_width);
		points[2] = calculateLightSpaceFrustumCorner(far_bottom, right_vector, far_width);
		points[3] = calculateLightSpaceFrustumCorner(far_bottom, left_vector, far_width);
		points[4] = calculateLightSpaceFrustumCorner(near_top, right_vector, near_width);
		points[5] = calculateLightSpaceFrustumCorner(near_top, left_vector, near_width);
		points[6] = calculateLightSpaceFrustumCorner(near_bottom, right_vector, near_width);
		points[7] = calculateLightSpaceFrustumCorner(near_bottom, left_vector, near_width);
		return points;
	}

	/**
	* Calculates one of the corner vertices of the view frustum in world space
	* and converts it to light space.
	*
	* @param start_point
	*            - the starting center point on the view frustum.
	* @param direction
	*            - the direction of the corner from the start point.
	* @param width
	*            - the distance of the corner from the start point.
	* @return - The relevant corner vertex of the view frustum in light space.
	*/
	glm::vec4 calculateLightSpaceFrustumCorner(const Vector3& start_point, const Vector3& direction,
		const float width) const
	{
		const Vector3 point = start_point + Vector3(direction.x * width, direction.y * width, direction.z * width);
		glm::vec4 point4f = glm::vec4(point.x, point.y, point.z, 1.0f);
		point4f = *light_view_matrix * point4f;
		return point4f;
	}

	/**
	* @return The rotation of the camera represented as a matrix.
	*/
	glm::mat4x4 calculateCameraRotationMatrix() const
	{
		double mat[16];
		(*cam)->calculateRotationMatrix(mat);
		glm::mat4 rotation = glm::make_mat4(mat);
		return rotation;
	}

	/**
	* Calculates the width and height of the near and far planes of the
	* camera's view frustum. However, this doesn't have to use the "actual" far
	* plane of the view frustum. It can use a shortened view frustum if desired
	* by bringing the far-plane closer, which would increase shadow resolution
	* but means that distant objects wouldn't cast shadows.
	*/
	void calculateWidthsAndHeights()
	{
		far_width = float(SHADOW_DISTANCE * tan(glm::radians(FOV)));
		near_width = float((CLIP_NEAR) * tan(glm::radians(FOV)));
		far_height = far_width / getAspectRatio();
		near_height = near_width / getAspectRatio();
	}

	/**
	* @return The aspect ratio of the display (width:height ratio).
	*/
	static float getAspectRatio()
	{
		return float(g_win_width) / float(g_win_height);
	}


	void setShadowDistance(const float d)
	{
		SHADOW_DISTANCE = d;
	}

	float getShadowDistance() const
	{
		return SHADOW_DISTANCE;
	}

	float getShadowFadeDistance() const
	{
		return SHADOW_DISTANCE*2 + TRANSITION_DISTANCE;
	}
};
