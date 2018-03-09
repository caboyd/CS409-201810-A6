//
//  KeyframeModelWithShader.d
//

#ifndef OBJ_LIBRARY_KEYFRAME_MODEL_WITH_SHADER_H
#define OBJ_LIBRARY_KEYFRAME_MODEL_WITH_SHADER_H

#include <vector>

#include "ObjSettings.h"
#ifndef OBJ_LIBRARY_GLM_INTERACTION
	#error "ModelWithShader can only be used with OBJ_LIBRARY_GLM_INTERACTION #defined"
#endif
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	#error "ModelWithShader can only be used with OBJ_LIBRARY_SHADER_DISPLAY #defined"
#endif

#include "../glm/glm.hpp"

#include "Vector3.h"
#include "MaterialForShader.h"
#include "ObjVbo.h"
#include "ObjVao.h"
#include "MeshWithShader.h"
#include "ModelWithShader.h"


namespace ObjLibrary
{

class Texture;



//
//  ModelWithShader
//
//  A class to represent a model on the graphics card.  The
//    model contains a number of materials, each of which can
//    have meshes associated with it.  Each mesh is displayed
//    using the material it is associated.
//
//  Note that switching between materials is somewhat slow, so
//    it is better to associate 2 meshes with the same material
//    than to add a copy of the material for each of them.
//

class KeyframeModelWithShader:public ModelWithShader
{
public:
//
//  Default Constructor
//
//  Purpose: To create a ModelWithShader without any materials
//           or meshes.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new ModelWithShader is created that contains
//               no materials or meshes.
//

	KeyframeModelWithShader () 	= default;

//
//  Single Mesh Constructor
//
//  Purpose: To create a ModelWithShader containing the
//           specified material and mesh.
//  Parameter(s):
//    <1> mesh: The MeshWithShader for the mesh
//    <2> material: The Material to display the mesh with
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new ModelWithShader is created containing
//               material material and mesh mesh.
//

	KeyframeModelWithShader (const MeshWithShader& mesh,
	                 const MaterialForShader& material)
	: ModelWithShader(mesh,material){};

//
//  Copy Constructor
//
//  Purpose: To create a new ModelWithShader as a copy of
//           another.
//  Parameter(s):
//    <1> original: The ModelWithShader to copy
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new ModelWithShader is created as a copy of
//               original.  This function creates a shallow copy
//               of the data on the graphics card.
//

	KeyframeModelWithShader (const KeyframeModelWithShader& original)
		: ModelWithShader(original)
	{
	}

//
//  Destructor
//
//  Purpose: To safely destroy this ModelWithShader without
//           memory leaks.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: All dynamically allocated memory is freed.
//               This may include freeing memeory on the
//               graphics card.
//

	~KeyframeModelWithShader () = default;

//
//  Assignment Operator
//
//  Purpose: To modify this ModelWithShader to be a copy of
//           another.
//  Parameter(s):
//    <1> original: The ModelWithShader to copy
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: This ModelWithShader is set to be a copy of
//               original.  This function creates a shallow copy
//               of the data on the graphics card.  Data on the
//               graphics card for the current model may be
//               freed.
//

	KeyframeModelWithShader& operator= (const KeyframeModelWithShader& original)
	{
		if(&original != this)
			mv_materials = original.mv_materials;
		return *this;
	}




//
//  draw
//
//  Purpose: To display this ModelWithShader with the
//           specified transformation matrixes and camera
//           position.  The camera position is needed to
//           calculate specular highlights.
//  Parameter(s):
//    <1> model_matrix: The model transformation matrix
//    <2> view_perspective_matrix: The product of the view and
//                                 projection matrixes
//    <3> camera_position: The position of the camera in world
//                         space
//  Precondition(s):
//    <1> isReady()
//    <2> ObjShader::isLoaded()
//  Returns: N/A
//  Side Effect: The meshes in this ModelWithShader are
//               displayed using the standard ObjLibrary
//               shaders, model transformation matrix
//               model_matrix, and view-projection matrix
//               view_perspective_matrix.  When calculating the
//               specular highlights, the camera is assumed to
//               be at position camera_pos.
//

	void draw (float tween_factor,
			   const glm::mat4x4& model_matrix,
	           const glm::mat4x4& view_matrix,
		       const glm::mat4x4& model_view_projection_matrix,
	           const glm::vec3& camera_position) const;

//
//  draw
//
//  Purpose: A conveniance version of the draw function for use
//           with models without specular highlights.
//  Parameter(s):
//    <1> model_matrix: The model transformation matrix
//    <2> view_perspective_matrix: The product of the view and
//                                 projection matrixes
//  Precondition(s):
//    <1> isReady()
//    <2> ObjShader::isLoaded()
//  Returns: N/A
//  Side Effect: The meshes in this ModelWithShader are
//               displayed using the standard ObjLibrary
//               shaders, model transformation matrix
//               model_matrix, and view-projection matrix
//               view_perspective_matrix.  Specular highlights
//               are calculated as if the camera is at the
//               origin.
//
	void draw(float tween_factor,
		const glm::mat4x4& model_matrix,
		const glm::mat4x4& view_matrix,
		const glm::mat4x4& model_view_projection_matrix) const;

//
//  drawSingleMaterial
//
//  Purpose: To display this ModelWithShader with the
//           specified material, transformation matrixes, and
//           camera position.
//  Parameter(s):
//    <1> material: The material to display with
//    <2> model_matrix: The model transformation matrix
//    <3> view_perspective_matrix: The product of the view and
//                                 projection matrixes
//    <4> camera_position: The position of the camera in world
//                         space
//  Precondition(s):
//    <1> isReady()
//    <2> ObjShader::isLoaded()
//  Returns: N/A
//  Side Effect: The meshes in this ModelWithShader are all
//               displayed using the standard ObjLibrary
//               shaders, and material material.  Model
//               transformation matrix model_matrix,
//               view-projection matrix view_perspective_matrix
//               and camera position camera_position are used.
//


	void drawSingleMaterial (
	                 const MaterialForShader& material,
					 float tween_factor,
	                 const glm::mat4x4& model_matrix,
	                 const glm::mat4x4& view_matrix,
					 const glm::mat4x4& model_view_projection_matrix,
	                 const glm::vec3& camera_position) const;

//
//  drawSingleMaterial
//
//  Purpose: A conveniance version of the drawSingleMaterial
//           function for use with models without specular
//           highlights.
//  Parameter(s):
//    <1> material: The material to display with
//    <2> model_matrix: The model transformation matrix
//    <3> view_perspective_matrix: The product of the view and
//                                 projection matrixes
//  Precondition(s):
//    <1> isReady()
//    <2> ObjShader::isLoaded()
//  Returns: N/A
//  Side Effect: The meshes in this ModelWithShader are all
//               displayed using the standard ObjLibrary
//               shaders, and material material.  Model
//               transformation matrix model_matrix, and
//               view-projection matrix view_perspective_matrix
//               are used. Specular highlights are calculated as
//               if the camera is at the origin.
//

	void drawSingleMaterial (
	          const MaterialForShader& material,
			  float tween_factor,
	          const glm::mat4x4& model_matrix,
	          const glm::mat4x4& view_matrix,
			  const glm::mat4x4& model_view_projection_matrix) const;

	//
//  drawSingleMaterial
//
//  Purpose: A conveniance version of the drawSingleMaterial
//           function for use with models without specular
//           highlights.
//  Parameter(s):
//    <1> material: The material to display with
//    <2> model_matrix: The model transformation matrix
//    <3> view_perspective_matrix: The product of the view and
//                                 projection matrixes
//  Precondition(s):
//    <1> isReady()
//    <2> ObjShader::isLoaded()
//  Returns: N/A
//  Side Effect: The meshes in this ModelWithShader are all
//               displayed using the standard ObjLibrary
//               shaders, and material material.  Model
//               transformation matrix model_matrix, and
//               view-projection matrix view_perspective_matrix
//               are used. Specular highlights are calculated as
//               if the camera is at the origin.
//

	void drawSingleMaterial (
			  float tween_factor,
	          const MaterialForShader& material,
	          const glm::mat4x4& model_matrix,
	          const glm::mat4x4& view_matrix,
			  const glm::mat4x4& model_view_projection_matrix) const;



};



}  // end of namespace ObjLibrary

#endif
