#pragma once
#include "lib/ObjLibrary/Vector3.h"
#include "lib/ObjLibrary/ModelWithShader.h"

using ObjLibrary::Vector3;
using ObjLibrary::ModelWithShader;

//Asbtract Entity class that has a model and a position, sclalar, rotation
class Entity
{
public:

	explicit Entity(ModelWithShader* model)
		: model(model)
	{}

	explicit Entity(ModelWithShader* model, const Vector3& pos)
		: position(pos), model(model)
	{}

	virtual ~Entity() = default;
	virtual void update(double delta_time) = 0;


	Vector3 position;

	//For drawing the model
	Vector3 scalar;
	Vector3 forward;

public:
	ModelWithShader * model;



};


