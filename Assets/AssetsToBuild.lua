--[[
	This file lists every asset that must be built by the AssetBuildSystem
--]]

return
{
	-- The top-level table is a dictionary whose keys
	-- correspond to the asset types (inherited from cbAssetTypeInfo)
	-- that you have defined in AssetBuildSystem.lua

	meshes =
	{
		-- The actual assets can be defined simply as relative path strings (the common case)
	  "Meshes/Plane.mesh","Meshes/floor.mesh"
	},
	materials=
	{
		
		 "Materials/Material1.mat","Materials/Material2.mat","Materials/Material3.mat"
		
	},
}
