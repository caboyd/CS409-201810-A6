#pragma once
#include "lib/glm/glm.hpp"
#include "lib/ObjLibrary/Vector3.h"

//Leverages the Vector3 class by managing a position, forward, and up vector
//	as a Coordinate System

class CoordinateSystem
{
public:
	//Camera position
	ObjLibrary::Vector3 position;
	//Camera forward
	ObjLibrary::Vector3 forward;
	//Camera up
	ObjLibrary::Vector3 up;

	explicit CoordinateSystem();
	//Initializing Contructor
	explicit CoordinateSystem(
		const ObjLibrary::Vector3& pos,
		const ObjLibrary::Vector3& forward,
		const ObjLibrary::Vector3& up);

	explicit CoordinateSystem(
		const ObjLibrary::Vector3& pos,
		const ObjLibrary::Vector3& forward);

	//Copy Constructor
	CoordinateSystem(const CoordinateSystem& other) = default;
	CoordinateSystem(CoordinateSystem&& other) = default;

	//Destructor
	~CoordinateSystem() = default;

	//Assignmnent Operator
	CoordinateSystem& operator=(const CoordinateSystem& other) = default;
	CoordinateSystem& operator=(CoordinateSystem&& other) = default;

	//Getters
	const ObjLibrary::Vector3& getPosition() const;
	const ObjLibrary::Vector3& getForward() const;
	const ObjLibrary::Vector3& getUp() const;
	ObjLibrary::Vector3 getRight() const;

	//Setters the position
	void setPosition(const ObjLibrary::Vector3& position);
	void setOrientation(const ObjLibrary::Vector3& forward, const ObjLibrary::Vector3& up);
	void setOrientation(const ObjLibrary::Vector3& forward);

	//Get the view matrix using glm::lookAt
	glm::mat4 getViewMatrix() const;

	//Initialize a coordinate system
	void init(const ObjLibrary::Vector3& position, const ObjLibrary::Vector3& forward, const ObjLibrary::Vector3& up);
	void init(const ObjLibrary::Vector3& position, const ObjLibrary::Vector3& forward);

	//Move the coordinate system a distance
	void moveForward(float distance);
	void moveUp(float distance);
	void moveRight(float distance);

	//Rotate around the axis
	void rotateAroundForward(double radians);
	void rotateAroundUp(double radians);
	void rotateAroundRight(double radians);

	//Arcball around a target
	void rotateAroundTarget(const ObjLibrary::Vector3& target, double radiansUp, double radiansRight);

	//Arcball around a target to a position
	//TODO doesnt work
	void rotateAroundTargetToPosition(const ObjLibrary::Vector3& target, const ObjLibrary::Vector3& position, double max_radians);

	//Set the forward to lookAt a target
	void lookAt(const ObjLibrary::Vector3& target);

	//Calculates an orientation matrix
	void calculateOrientationMatrix(double a_matrix[]) const;
	//Calculates a rotation matrix
	void calculateRotationMatrix(double a_matrix[]) const;

	//Calculates an up vector from a forward
	ObjLibrary::Vector3 calculateUpVector(const ObjLibrary::Vector3& local_forward) const;

	//Rotates the coordinate system towards upright
	void rotateUpright(double max_radians);

	//Sets a random up vector
	void setRandomUp();
};
