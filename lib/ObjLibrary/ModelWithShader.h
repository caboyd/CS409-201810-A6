//
//  ModelWithShader.h
//
//  A module to represent a model on the graphics card.
//
//  This file is part of the Shaders extension to the
//    ObjLibrary, by Richard Hamilton, which is copyright
//    Hamilton 2009-2016.
//
//  You may use these files for any purpose as long as you do
//    not explicitly claim them as your own work or object to
//    other people using them.
//
//  If you are destributing the source files, you must not
//    remove this notice.  If you are only destributing compiled
//    code, no credit is required.
//
//  A (theoretically) up-to-date version of the ObjLibrary can
//    be found at:
//  http://infiniplix.ca/resources/obj_library/
//

#ifndef OBJ_LIBRARY_MODEL_WITH_SHADER_H
#define OBJ_LIBRARY_MODEL_WITH_SHADER_H

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

class ModelWithShader
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

	ModelWithShader ()
		: mv_materials()
	{
	}

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

	ModelWithShader (const MeshWithShader& mesh,
	                 const MaterialForShader& material);

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

	ModelWithShader (const ModelWithShader& original)
		: mv_materials(original.mv_materials)
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

	~ModelWithShader ()
	{
	}

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

	ModelWithShader& operator= (const ModelWithShader& original)
	{
		if(&original != this)
			mv_materials = original.mv_materials;
		return *this;
	}

//
//  getMaterialCount
//
//  Purpose: To determine the number of materials in this
//           ModelWithShader.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The number of materials.
//  Side Effect: N/A
//

	unsigned int getMaterialCount () const
	{
		return mv_materials.size();
	}

//
//  isSingleMaterial
//
//  Purpose: To determine if there is exactly one material in
//           this ModelWithShader.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this ModelWithShader contians exactly one
//           material.
//  Side Effect: N/A
//

	bool isSingleMaterial () const
	{
		return (mv_materials.size() == 1);
	}

//
//  getMaterial
//
//  Purpose: To retreive the specified material in this
//           ModelWithShader.  This function is intended for use
//           with instancing.
//  Parameter(s):
//    <1> material: Which material
//  Precondition(s):
//    <1> material < getMaterialCount()
//  Returns: Material material in this ModelWithShader.
//  Side Effect: N/A
//

	const MaterialForShader& getMaterial (
	                                unsigned int material) const
	{
		assert(material < getMaterialCount());

		assert(material < mv_materials.size());
		return mv_materials[material].m_material;
	}

//
//  getSingleMaterial
//
//  Purpose: To retreive the single material in this
//           ModelWithShader.  This function is intended for use
//           with instancing.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isSingleMaterial()
//  Returns: The single material in this ModelWithShader.
//  Side Effect: N/A
//

	const MaterialForShader& getSingleMaterial () const
	{
		assert(isSingleMaterial());

		assert(mv_materials.size() == 1);
		assert(0 < mv_materials.size());
		return mv_materials[0].m_material;
	}

//
//  getMeshCount
//
//  Purpose: To determine the number of meshes in this
//           ModelWithShader that are associated with the
//           specified material.
//  Parameter(s):
//    <1> material: Which material
//  Precondition(s):
//    <1> material < getMaterialCount()
//  Returns: The number of meshes.
//  Side Effect: N/A
//

	unsigned int getMeshCount (unsigned int material) const
	{
		assert(material < getMaterialCount());

		assert(material < mv_materials.size());
		return mv_materials[material].mv_meshes.size();
	}

//
//  getMesh
//
//  Purpose: To retreive the specified mesh associated with the
//           specified material in this ModelWithShader.  This
//           function is intended for use with instancing.
//  Parameter(s):
//    <1> material: Which material
//    <2> mesh: Which mesh
//  Precondition(s):
//    <1> material < getMaterialCount()
//    <2> mesh < getMeshCount(material)
//  Returns: Mesh mesh in Material material of this
//           ModelWithShader.
//  Side Effect: N/A
//

	const MeshWithShader& getMesh (unsigned int material,
	                               unsigned int mesh) const
	{
		assert(material < getMaterialCount());
		assert(mesh < getMeshCount(material));

		assert(material < mv_materials.size());
		assert(mesh < mv_materials[material].mv_meshes.size());
		return mv_materials[material].mv_meshes[mesh];
	}

//
//  getMeshCountTotal
//
//  Purpose: To determine the total number of meshes associated
//           with all materials in this ModelWithShader.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The tota; number of meshes.
//  Side Effect: N/A
//

	unsigned int getMeshCountTotal () const;

//
//  isReady
//
//  Purpose: To determine if all the meshes in this
//           ModelWithShader are ready to display.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Wheether all the meshes are ready to display.
//  Side Effect: N/A
//

	bool isReady () const;

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

	void draw (const glm::mat4x4& model_matrix,
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
	void draw(
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
	          const glm::mat4x4& model_matrix,
	          const glm::mat4x4& view_matrix,
			  const glm::mat4x4& model_view_projection_matrix) const;

//
//  drawCurrentMaterial
//
//  Purpose: To display the meshes in this ModelWithShader with
//           the current shader program and material.  Don't use
//           this function with your own shaders unless you
//           understand the data format used by the
//           MeshWithShader class.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isReady()
//    <2> ObjShader::isLoaded()
//  Returns: N/A
//  Side Effect: The meshes in this ModelWithShader are
//               displayed using the current shaders and uniform
//               values.
//

	void drawCurrentMaterial () const;

//
//  drawCurrentMaterial
//
//  Purpose: To display the meshes in this ModelWithShader and
//           associated the specified material with the current
//           shader program and material.  Don't use this
//           function with your own shaders unless you
//           understand the data format used by the
//           MeshWithShader class.
//  Parameter(s):
//    <1> material: The Material to associate the mesh with
//  Precondition(s):
//    <1> isReady()
//    <2> ObjShader::isLoaded()
//    <3> material < getMaterialCount()
//  Returns: N/A
//  Side Effect: The meshes in this ModelWithShader are
//               displayed using the current shaders and uniform
//               values.
//

	void drawCurrentMaterial (unsigned int material) const;

//
//  addMaterial
//
//  Purpose: To add a material to this ModelWithShader.
//  Parameter(s):
//    <1> material: The MaterialForShader to add
//  Precondition(s): N/A
//  Returns: The index of the new material
//  Side Effect: MaterialForShader material is added to this
//               ModelWithShader.
//

	unsigned int addMaterial (
	                         const MaterialForShader& material);

//
//  setMaterial
//
//  Purpose: To replace a material in this ModelWithShader
//           without changing the meshes associated with it.
//  Parameter(s):
//    <1> index: Which material to replace
//    <2> material: The new MaterialForShader
//  Precondition(s):
//    <1> index < getMaterialCount()
//  Returns: N/A
//  Side Effect: MaterialForShader index in this ModelWithShader
//               is replaced with MaterialForShader material.
//               All meshes previously associated with the old
//               MaterialForShader are now associated with
//               material.
//

	void setMaterial (unsigned int index,
	                  const MaterialForShader& material);

//
//  setSingleMaterial
//
//  Purpose: To replace the single material in this
//           ModelWithShader without changing the meshes
//           associated with it.
//  Parameter(s):
//    <1> material: The new MaterialForShader
//  Precondition(s):
//    <1> isSingleMaterial()
//  Returns: N/A
//  Side Effect: The single material in this ModelWithShader
//               is replaced with MaterialForShader material.
//               All meshes in this ModelWithShader are now
//               associated with material.
//

	void setSingleMaterial (const MaterialForShader& material);

//
//  addMesh
//
//  Purpose: To add a mesh to this ModelWithShader, associated
//           with the specified material.
//  Parameter(s):
//    <1> material: The Material to associate the mesh with
//    <2> mesh: The MeshWithShader for the mesh to add
//  Precondition(s):
//    <1> material < getMaterialCount()
//  Returns: N/A
//  Side Effect: Mesh mesh is added to this ModelWithShader and
//               associated with material material.
//

	void addMesh (unsigned int material,
	              const MeshWithShader& mesh);

//
//  makeEmpty
//
//  Purpose: To remove all materials and meshes in this
//           ModelWithShader.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: All materials and all meshes in this
//               ModelWithShader are removed, restoring it to
//               state produced by the default constructor.
//               This may result in freeing memory on the
//               graphics card.
//

	void makeEmpty ()
	{
		mv_materials.clear();
	}

protected:
	//
	//  MaterialWithMeshes
	//
	//  A record to represent a material and all the meshes
	//    displayed with it.
	//

	struct MaterialWithMeshes
	{
		MaterialWithMeshes ();
		MaterialWithMeshes (const MaterialForShader& material);
		MaterialWithMeshes (const MaterialWithMeshes& original);
		MaterialWithMeshes& operator= (
		                    const MaterialWithMeshes& original);

		MaterialForShader m_material;
		std::vector<MeshWithShader> mv_meshes;
	};

public:
	std::vector<MaterialWithMeshes> mv_materials;
};



}  // end of namespace ObjLibrary

#endif
