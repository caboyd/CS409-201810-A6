
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

#define DEFAULT_SHADOW_DISTANCE 35.0

extern const double CLIP_NEAR;
extern const double CLIP_FAR;
extern const double FOV;
extern int win_width;
extern int win_height;

class ShadowBox
{

private:
	double SHADOW_DISTANCE;


	float OFFSET = 7.0;

	glm::vec4 UP = glm::vec4(0, 1, 0, 0);
	glm::vec4 FORWARD = glm::vec4(0, 0, -1, 0);

	float minX, maxX;
	float minY, maxY;
	float minZ, maxZ;
	glm::mat4x4* lightViewMatrix;
	CoordinateSystem** cam;

	float farHeight, farWidth, nearHeight, nearWidth;
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
	ShadowBox() {}
	void init(glm::mat4x4* light_view_matrix, CoordinateSystem** camera, const float shadow_distance = DEFAULT_SHADOW_DISTANCE)
	{
		this->SHADOW_DISTANCE = shadow_distance;
		this->lightViewMatrix = light_view_matrix;
		this->cam = camera;
		calculateWidthsAndHeights();
	}

	/**
	* Updates the bounds of the shadow box based on the light direction and the
	* camera's view frustum, to make sure that the box covers the smallest area
	* possible while still ensuring that everything inside the camera's view
	* (within a certain range) will cast shadows.
	*/
	void update()
	{
		const glm::mat4 rotation = glm::transpose(calculateCameraRotationMatrix());

		const Vector3 forward_vector(rotation * -FORWARD);
		const Vector3 to_far = forward_vector * SHADOW_DISTANCE;
		const Vector3 to_near = forward_vector * CLIP_NEAR;

		const Vector3 center_near = to_near + (*cam)->getPosition();
		const Vector3 center_far = to_far + (*cam)->getPosition();

		glm::vec4* points = calculateFrustumVertices(rotation, forward_vector, center_near,
			center_far);

		bool first = true;
		for (unsigned int i = 0; i < 8; i++)
		{
			if (first)
			{
				minX = points[i].x;
				maxX = points[i].x;
				minY = points[i].y;
				maxY = points[i].y;
				minZ = points[i].z;
				maxZ = points[i].z;
				first = false;
				continue;
			}
			if (points[i].x > maxX)
			{
				maxX = points[i].x;
			} else if (points[i].x < minX)
			{
				minX = points[i].x;
			}
			if (points[i].y > maxY)
			{
				maxY = points[i].y;
			} else if (points[i].y < minY)
			{
				minY = points[i].y;
			}
			if (points[i].z > maxZ)
			{
				maxZ = points[i].z;
			} else if (points[i].z < minZ)
			{
				minZ = points[i].z;
			}
		}
		maxZ += OFFSET;
	}

	/**
	* Calculates the center of the "view cuboid" in light space first, and then
	* converts this to world space using the inverse light's view matrix.
	*
	* @return The center of the "view cuboid" in world space.
	*/
	Vector3 getCenter() const
	{
		const float x = (minX + maxX) / 2.0f;
		const float y = (minY + maxY) / 2.0f;
		const float z = (minZ + maxZ) / 2.0f;
		const glm::vec4 cen = glm::vec4(x, y, z, 1);

		const glm::mat4x4 inverted_light = glm::inverse(*lightViewMatrix);
		Vector3 result(inverted_light * cen);
		return result;
	}

	/**
	* @return The width of the "view cuboid" (orthographic projection area).
	*/
	float getWidth() const
	{
		return maxX - minX;
	}

	/**
	* @return The height of the "view cuboid" (orthographic projection area).
	*/
	float getHeight() const
	{
		return maxY - minY;
	}

	/**
	* @return The length of the "view cuboid" (orthographic projection area).
	*/
	float getLength() const
	{
		return maxZ - minZ;
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
	glm::vec4* calculateFrustumVertices(const glm::mat4x4& rotation, const Vector3& forward_vector,
		const Vector3& center_near, const Vector3& center_far) const
	{
		const Vector3 up_vector(rotation * UP);
		const Vector3 right_vector = forward_vector.crossProduct(up_vector);
		const Vector3 down_vector = -up_vector;
		const Vector3 left_vector = -right_vector;
		const Vector3 far_top = center_far + Vector3(up_vector.x * farHeight,
			up_vector.y * farHeight, up_vector.z * farHeight);
		const Vector3 far_bottom = center_far + Vector3(down_vector.x * farHeight,
			down_vector.y * farHeight, down_vector.z * farHeight);
		const Vector3 near_top = center_near + Vector3(up_vector.x * nearHeight,
			up_vector.y * nearHeight, up_vector.z * nearHeight);
		const Vector3 near_bottom = center_near + Vector3(down_vector.x * nearHeight,
			down_vector.y * nearHeight, down_vector.z * nearHeight);
		glm::vec4* points = new glm::vec4[8];
		points[0] = calculateLightSpaceFrustumCorner(far_top, right_vector, farWidth);
		points[1] = calculateLightSpaceFrustumCorner(far_top, left_vector, farWidth);
		points[2] = calculateLightSpaceFrustumCorner(far_bottom, right_vector, farWidth);
		points[3] = calculateLightSpaceFrustumCorner(far_bottom, left_vector, farWidth);
		points[4] = calculateLightSpaceFrustumCorner(near_top, right_vector, nearWidth);
		points[5] = calculateLightSpaceFrustumCorner(near_top, left_vector, nearWidth);
		points[6] = calculateLightSpaceFrustumCorner(near_bottom, right_vector, nearWidth);
		points[7] = calculateLightSpaceFrustumCorner(near_bottom, left_vector, nearWidth);
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
		point4f = *lightViewMatrix * point4f;
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
		farWidth = float(SHADOW_DISTANCE * tan(glm::radians(FOV)));
		nearWidth = float(CLIP_NEAR * tan(glm::radians(FOV)));
		farHeight = farWidth / getAspectRatio();
		nearHeight = nearWidth / getAspectRatio();
	}

	/**
	* @return The aspect ratio of the display (width:height ratio).
	*/
	static float getAspectRatio()
	{
		return float(win_width) / float(win_height);
	}


	void setShadowDistance(const float d)
	{
		SHADOW_DISTANCE = d;
	}

	float getShadowDistance() const
	{
		return SHADOW_DISTANCE;
	}
};
