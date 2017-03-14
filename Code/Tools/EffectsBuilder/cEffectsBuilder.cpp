// Header Files
//=============

#include "cEffectsBuilder.h"
#include "../AssetBuildLibrary/UtilityFunctions.h"
#include "../../Engine/Platform/Platform.h"
#include "../../External/Lua/Includes.h"
#include "../../Engine/Logging/Logging.h"
#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Graphics/Graphics.h"
#include <iostream>
#include <sstream>
#include <fstream>
// Inherited Implementation
//=========================

// Build
//------

uint8_t eae6320::AssetBuild::cEffectsBuilder::normalizeColor(uint16_t component)
{
	return static_cast<uint8_t>(component * 255.0f);
}

bool eae6320::AssetBuild::cEffectsBuilder::Build(const std::vector<std::string>&)
{

	bool wereThereErrors = false;
	std::string _vertexPath;
	std::string _fragmentPath;
	bool alpha, depth, culling = false;
	std::cerr << "\n comes here ";
	printf("hello");
	// Copy the source to the target
	{
		lua_State* luaState = nullptr;
		{
			luaState = luaL_newstate();
			if (luaState == NULL)
			{
				EAE6320_ASSERTF(false, "Failed to create a new Lua state");
				eae6320::Logging::OutputError("Failed to create a new Lua state for the user settings");
				wereThereErrors = true;
				EAE6320_ASSERT(true);
				goto onExit;
			}
		}
		luaL_openlibs(luaState);

		if (eae6320::Platform::DoesFileExist(m_path_source))
		{

			const int luaResult = luaL_loadfile(luaState, m_path_source);
			if (luaResult == LUA_OK)
			{
				// Execute the "chunk"
				const int argumentCount = 0;
				const int returnValueCount = LUA_MULTRET;
				const int noErrorHandler = 0;
				const int luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noErrorHandler);
				if (luaResult != LUA_OK)
				{
					std::cerr << lua_tostring(luaState, -1) << "\n";
					// Pop the error message
					lua_pop(luaState, 1);
					wereThereErrors = true;
					EAE6320_ASSERT(true);
					goto onExit;
				}
			}
			else
			{
				std::string msg = lua_tostring(luaState, -1);
				std::cerr << lua_tostring(luaState, -1) << "\n";
				// Pop the error message
				lua_pop(luaState, 1);
				wereThereErrors = true;
				EAE6320_ASSERT(true);
				goto onExit;
			}
		}
		else
		{
			wereThereErrors = true;
			eae6320::Logging::OutputError("%s does not exist", m_path_source);

			goto onExit;

		}
		//render states
		{
			const char* const key = "RenderStates";
			lua_pushstring(luaState, key);
			lua_gettable(luaState, -2);
			if (lua_istable(luaState, -1))
			{
				// get alpha
				{
					const char* const key = "AlphaTransparency";
					lua_pushstring(luaState, key);
					lua_gettable(luaState, -2);
					if (!lua_isnil(luaState, -1))
					{
						lua_toboolean(luaState, -1) > 0 ? alpha = true  : alpha = false;
						 lua_pop(luaState ,1);
					}
				}
				//depth
				{
					const char* const key = "DepthBuffer";
					lua_pushstring(luaState, key);
					lua_gettable(luaState, -2);
					if (!lua_isnil(luaState, -1))
					{
						lua_toboolean(luaState, -1) > 0 ? depth = true : depth = false;
						lua_pop(luaState, 1);
					}
				}
				//depth
				{
					const char* const key = "Culling";
					lua_pushstring(luaState, key);
					lua_gettable(luaState, -2);
					if (!lua_isnil(luaState, -1))
					{
						 lua_toboolean(luaState, -1) > 0 ? culling = true : culling = false;
						lua_pop(luaState, 1);
					}
				}

				lua_pop(luaState, 1);
			}
		
		}


		const char* const key = "shaders";
		lua_pushstring(luaState, key);
		lua_gettable(luaState, -2);
		if (lua_istable(luaState, -1))
		{
			{
				const char* const vertexShaderPath = "vertexShader";
				lua_pushstring(luaState, vertexShaderPath);
				lua_gettable(luaState, -2);
				if (lua_isnil(luaState, -1))
				{
					std::cerr << "No value for vertex shader path";
					lua_pop(luaState, 1);
					wereThereErrors = true;
					EAE6320_ASSERT(true);
					return false;
				}
				if (lua_type(luaState, -1) != LUA_TSTRING)
				{
					std::cerr << "The value for vertex shader path must be a string";
					// Pop the value
					lua_pop(luaState, 1);
					// (The asset table is now at -1)
					wereThereErrors = true;
					EAE6320_ASSERT(true);
					return false;
				}
				{
					const char* value = lua_tostring(luaState, -1);

					std::string vertexpath = value;
					std::string errormsg = " ";

					bool issucess = eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(value, "shaders", vertexpath, &errormsg);
					std::cerr << "\n " << errormsg.c_str();
					if (!issucess)
					{
						//wereThereErrors = true;
					
					}
					
					_vertexPath = vertexpath;
					lua_pop(luaState, 1);
				}
			}

			{
				const char* const fragmentShaderPath = "fragmentShader";
				lua_pushstring(luaState, fragmentShaderPath);
				lua_gettable(luaState, -2);
				if (lua_isnil(luaState, -1))
				{
					std::cerr << "No value for fragment shader path";
					lua_pop(luaState, 1);
					wereThereErrors = true;
					EAE6320_ASSERT(true);
					return false;
				}
				if (lua_type(luaState, -1) != LUA_TSTRING)
				{
					std::cerr << "The value for fragment shader path must be a string";
					// Pop the value
					lua_pop(luaState, 1);
					wereThereErrors = true;
					EAE6320_ASSERT(true);
					// (The asset table is now at -1)
					return false;
				}
				{
					const char* value = lua_tostring(luaState, -1);

					std::string fragmentpath = value;
					std::string errormsg = " ";

					bool issucess = eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(value, "shaders", fragmentpath, &errormsg);
					std::cerr << "\n " << errormsg.c_str();
					if (!issucess)
					{
						//wereThereErrors = true;
					
					}
					_fragmentPath = fragmentpath.c_str();
					lua_pop(luaState, 1);
				}
			}
		}
		else
			std::cerr << "Shaders not found";
		if (!wereThereErrors)
		{
			//for shaders and actual resource table
			lua_pop(luaState, 2);
		}


		//writing the values on to .meshfile
		if (!wereThereErrors)
		{
		
				uint8_t alphaT = 0, depthB = 0, cullingE =0;
				if (alpha)
				{
					alphaT = 1;
				}
				if (depth)
					depthB = 2;
				if (culling)
					cullingE = 4;



				uint8_t renderState = alphaT + depthB + cullingE;
				std::ofstream binFile(m_path_target, std::ofstream::binary);
				std::string vertexpath = "data/";
				vertexpath += _vertexPath;
				vertexpath += "\0";

				uint16_t size = static_cast<uint16_t>(strlen(vertexpath.c_str()));
				size++;
				char* csize = reinterpret_cast<char*>(&size);
				char* render = reinterpret_cast<char*>(&renderState);
				binFile.write(render, sizeof(uint8_t));
				binFile.write(csize, sizeof(uint16_t));
				binFile.write(vertexpath.c_str(), size);
			
				
				std::string fragmentPath = "data/";
				fragmentPath += _fragmentPath;
				fragmentPath += "\0";

				uint16_t fsize = static_cast<uint16_t>(strlen(fragmentPath.c_str()));
				fsize++;
				char* cfsize = reinterpret_cast<char*>(&fsize);
				binFile.write(cfsize, sizeof(uint16_t));
				binFile.write(fragmentPath.c_str(), fsize);
			
			

		}

	onExit:
		if (luaState)
		{
			// If I haven't made any mistakes
			// there shouldn't be anything on the stack,
			// regardless of any errors encountered while loading the file:
			EAE6320_ASSERT(lua_gettop(luaState) == 0);
			lua_close(luaState);
			luaState = NULL;
		}
		if (wereThereErrors)
			std::cerr << "failed to load %s!", m_path_source;
	}

	return !wereThereErrors;
}
