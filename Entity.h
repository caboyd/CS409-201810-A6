#pragma once
#include "lib/ObjLibrary/Vector3.h"
#include "lib/ObjLibrary/ModelWithShader.h"
#include "CoordinateSystem.h"

using ObjLibrary::Vector3;
using ObjLibrary::ModelWithShader;

//Asbtract Entity class that has a model and a position, sclalar, rotation
class Entity
{
public:

	explicit Entity(const ModelWithShader& model)
		: model(&model)
	{}

	explicit Entity(const ModelWithShader& model, const Vector3& pos, const Vector3& forward = Vector3(0,0,-1))
		: coordinate_system(pos,forward), model(&model)
	{}

		explicit Entity(const ModelWithShader& model, const CoordinateSystem& coordinate_system)
		: coordinate_system(coordinate_system), model(&model)
	{}

	virtual ~Entity() = default;
	virtual void update(double delta_time) = 0;


	CoordinateSystem coordinate_system;

	//For drawing the model
	Vector3 model_scalar = {1,1,1};
	Vector3 model_forward = {0,0,-1};

public:
	ModelWithShader const* model;



};


