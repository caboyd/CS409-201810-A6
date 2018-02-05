#include "CoordinateSystem.h"
#include "lib/ObjLibrary/Vector3.h"
#include "MathHelper.h"
#include "lib/ObjLibrary/Vector2.h"

CoordinateSystem::CoordinateSystem()
{
	init(ObjLibrary::Vector3(0, 0, 0), ObjLibrary::Vector3(0, 0, 0), ObjLibrary::Vector3(0, 0, 0));
}

CoordinateSystem::CoordinateSystem(const ObjLibrary::Vector3& pos, const ObjLibrary::Vector3& forward, const ObjLibrary::Vector3& up)
{
	init(pos, forward, up);
}

CoordinateSystem::CoordinateSystem(const ObjLibrary::Vector3& pos, const ObjLibrary::Vector3& forward)
{
	init(pos, forward, calculateUpVector(forward));
}

CoordinateSystem::CoordinateSystem(const CoordinateSystem& other)
{
	position = other.position;
	forward = other.forward;
	up = other.up;
}

CoordinateSystem::~CoordinateSystem()
{}

CoordinateSystem& CoordinateSystem::operator=(const CoordinateSystem& other)
{
	if (&other != this)
	{
		position = other.position;
		forward = other.forward;
		up = other.up;

	}
	return *this;
}

const ObjLibrary::Vector3& CoordinateSystem::getPosition() const
{
	return position;
}

const ObjLibrary::Vector3& CoordinateSystem::getForward() const
{
	return forward;
}

const ObjLibrary::Vector3& CoordinateSystem::getUp() const
{
	return up;
}

ObjLibrary::Vector3 CoordinateSystem::getRight() const
{
	return forward.crossProduct(up);
}

glm::mat4 CoordinateSystem::getViewMatrix() const
{
	return glm::lookAt(glm::vec3(position),
		glm::vec3(position + forward), glm::vec3(up));
}

void CoordinateSystem::init(const ObjLibrary::Vector3& position, const ObjLibrary::Vector3& forward, const ObjLibrary::Vector3& up)
{
	this->position = position;
	this->forward = forward;
	this->up = up;
}

void CoordinateSystem::init(const ObjLibrary::Vector3& position, const ObjLibrary::Vector3& forward)
{
	this->position = position;
	this->forward = forward;
	this->up = calculateUpVector(forward);
}

void CoordinateSystem::setPosition(const ObjLibrary::Vector3& position)
{
	this->position = position;
}

void CoordinateSystem::moveForward(float distance)
{
	position += forward * distance;
}

void CoordinateSystem::moveUp(float distance)
{
	position += up * distance;
}

void CoordinateSystem::moveRight(float distance)
{
	position += getRight() * distance;
}

void CoordinateSystem::setOrientation(const ObjLibrary::Vector3& forward, const ObjLibrary::Vector3& up)
{
	this->forward = forward;
	this->up = up;
}

void CoordinateSystem::setOrientation(const ObjLibrary::Vector3& forward)
{
	this->forward = forward;
	this->up = calculateUpVector(forward);
}

void CoordinateSystem::rotateAroundForward(double radians)
{
	up.rotateArbitrary(forward, radians);
}

void CoordinateSystem::rotateAroundUp(double radians)
{
	forward.rotateArbitrary(up, radians);
}

void CoordinateSystem::rotateAroundRight(double radians)
{
	ObjLibrary::Vector3 right = getRight();
	forward.rotateArbitrary(right, radians);
	//up.rotateArbitrary(right, radians);
}

void CoordinateSystem::rotateAroundTarget(const ObjLibrary::Vector3& target, double radiansUp, double radiansRight)
{
	//pitch up down
	ObjLibrary::Vector3 diff = position - target;
	ObjLibrary::Vector3 newPos = position - target;

	newPos.rotateArbitrary(getRight().getNormalized(), radiansRight);
	//yaw left right
	newPos.rotateArbitrary(up.getNormalized(), radiansUp);
	position = newPos + target;
	//Look at target
	lookAt(target);
}

void CoordinateSystem::rotateAroundTargetToPosition(const ObjLibrary::Vector3& target,
	const ObjLibrary::Vector3& to_position, double max_radians)
{
	ObjLibrary::Vector3 target_towards_position = -(position - target).getNormalized();
	ObjLibrary::Vector3 target_towards_to_position = -(to_position - target).getNormalized();
	ObjLibrary::Vector3 cross = target_towards_position.crossProduct((target_towards_to_position));
	double angle = target_towards_position.getAngleSafe(target_towards_to_position);


	if(angle > MathHelper::M_PI_2) {
		double temp = cross.x;
		if ( cross.y < 0 && cross.z < 0) temp = -temp;
		cross.x = cross.z;
		cross.z = temp;
	}
	

	if (angle > max_radians) angle = max_radians;
	else if (angle < -max_radians) angle = -max_radians;

	ObjLibrary::Vector3 newPos = position - target;
	newPos.rotateArbitrary(cross, angle);
	position = newPos + target;
	lookAt(target);

}

void CoordinateSystem::lookAt(const ObjLibrary::Vector3& target)
{
	setOrientation(
		(target - this->position).getNormalized()
	);
}


void CoordinateSystem::calculateOrientationMatrix(double a_matrix[]) const
{
	ObjLibrary::Vector3 right = getRight();

	a_matrix[0] = right.x;
	a_matrix[1] = right.y;
	a_matrix[2] = right.z;
	a_matrix[3] = 0.0;
	a_matrix[4] = forward.x;
	a_matrix[5] = forward.y;
	a_matrix[6] = forward.z;
	a_matrix[7] = 0.0;
	a_matrix[8] = up.x;
	a_matrix[9] = up.y;
	a_matrix[10] = up.z;
	a_matrix[11] = 0.0;
	a_matrix[12] = 0.0;
	a_matrix[13] = 0.0;
	a_matrix[14] = 0.0;
	a_matrix[15] = 1.0;
}


void CoordinateSystem::calculateRotationMatrix(double a_matrix[]) const
{
	ObjLibrary::Vector3 xaxis = up.crossProduct(forward);
	xaxis.normalize();

	ObjLibrary::Vector3 yaxis = forward.crossProduct(xaxis);
	yaxis.normalize();

	a_matrix[0] = xaxis.x;
	a_matrix[1] = yaxis.x;
	a_matrix[2] = forward.x;
	a_matrix[3] = 0.0;
	a_matrix[4] = xaxis.y;
	a_matrix[5] = yaxis.y;
	a_matrix[6] = forward.y;
	a_matrix[7] = 0.0;
	a_matrix[8] = xaxis.z;
	a_matrix[9] = yaxis.z;
	a_matrix[10] = forward.z;
	a_matrix[11] = 0.0;
	a_matrix[12] = 0.0;
	a_matrix[13] = 0.0;
	a_matrix[14] = 0.0;
	a_matrix[15] = 1.0;
}


ObjLibrary::Vector3 CoordinateSystem::calculateUpVector(const ObjLibrary::Vector3& local_forward) const
{
	static const ObjLibrary::Vector3 IDEAL_UP_VECTOR(0.0, 1.0, 0.0);
	static const double HALF_PI = 1.5707963267948966;
	// code will go here

	assert(!local_forward.isZero());

	ObjLibrary::Vector3 axis = local_forward.crossProduct(IDEAL_UP_VECTOR);

	if (axis.isZero())
		return ObjLibrary::Vector3(1.0, 0.0, 0.0);

	axis.normalize();
	// more code here
	ObjLibrary::Vector3 local_up = local_forward.getRotatedArbitrary(axis, HALF_PI);
	return local_up;



}

void CoordinateSystem::rotateUpright(double max_radians)
{
	// code will go here
	ObjLibrary::Vector3 desired_up = calculateUpVector(forward);
	ObjLibrary::Vector3 axis = up.crossProduct(desired_up);
	if (axis.isZero())
		axis = forward;
	else
		axis.normalize();

	double radians = up.getAngleSafe(desired_up);
	if (radians > max_radians)
		radians = max_radians;

	up.rotateArbitrary(axis, radians);
}

void CoordinateSystem::setRandomUp()
{
	static const double PI = 3.14159265358979323846;
	double random0to1 = rand() / (RAND_MAX + 1.0);
	up.rotateArbitrary(forward, random0to1 * 2 * PI);
}
