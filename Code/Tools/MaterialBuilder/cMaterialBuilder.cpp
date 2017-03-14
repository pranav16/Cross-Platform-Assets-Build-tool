// Header Files
//=============

#include "cMaterialBuilder.h"
#include "../AssetBuildLibrary/UtilityFunctions.h"
#include "../../Engine/Platform/Platform.h"
#include "../../External/Lua/Includes.h"
#include "../../Engine/Logging/Logging.h"
#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Graphics/GraphicsDataTypes.h"
#include <iostream>
#include <sstream>
#include <fstream>
// Inherited Implementation
//=========================

// Build
//------

uint8_t eae6320::AssetBuild::cMaterialBuilder::normalizeColor(uint16_t component)
{
	return static_cast<uint8_t>(component * 255.0f);
}

bool eae6320::AssetBuild::cMaterialBuilder::Build(const std::vector<std::string>&)
{

	bool wereThereErrors = false;
	std::string _effectspath="";
	std::string _texturePath = "";
	eae6320::Graphics::sMaterialConstants _matConstants;
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

		//get parameters
		{
			const char* const key = "Parameters";
			lua_pushstring(luaState, key);
			lua_gettable(luaState, -2);
			if (lua_istable(luaState, -1))
			{
				//get color
				{
					const char* const color = "Color";
					lua_pushstring(luaState, color);
					lua_gettable(luaState, -2);
					if (lua_istable(luaState, -1))
					{
						const int colorLength = luaL_len(luaState, -1);
						EAE6320_ASSERT(colorLength == 4)
							lua_pushnil(luaState);
						int k = 0;
						while (lua_next(luaState, -2))
						{

							if (k == 0)
								_matConstants.r = normalizeColor((uint16_t)lua_tonumber(luaState, -1));
							else if (k == 1)
								_matConstants.g = normalizeColor((uint16_t)lua_tonumber(luaState, -1));
							else if (k == 2)
								_matConstants.b = normalizeColor((uint16_t)lua_tonumber(luaState, -1));
							else if (k == 3)
								_matConstants.a = normalizeColor((uint16_t)lua_tonumber(luaState, -1));
							k++;
							lua_pop(luaState, 1);
						}
						lua_pop(luaState, 1);
					}

				}
				//getting texture path
				{
					const char* const path = "Texture";
					lua_pushstring(luaState, path);
					lua_gettable(luaState, -2);
					if (lua_isnil(luaState, -1))
					{
						std::cerr << "No value for effect shader path";
						lua_pop(luaState, 1);
						const char* value = "default.dds";
						std::string texture = value;
						std::string errormsg = " ";
						bool issucess = eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(value, "textures", texture, &errormsg);
						std::cerr << "\n " << errormsg.c_str();
						if (!issucess)
						{
							wereThereErrors = true;

						}
						_texturePath = texture.c_str();
					}
					else
					{
						if (lua_type(luaState, -1) != LUA_TSTRING)
						{
							std::cerr << "The value for effect path must be a string";
							// Pop the value
							lua_pop(luaState, 1);
							wereThereErrors = true;
							EAE6320_ASSERT(true);
							// (The asset table is now at -1)
							return false;
						}
						{
							const char* value = lua_tostring(luaState, -1);

							std::string texture = value;
							std::string errormsg = " ";

							bool issucess = eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(value, "textures", texture, &errormsg);
							std::cerr << "\n " << errormsg.c_str();
							if (!issucess)
							{
								wereThereErrors = true;

							}
							_texturePath = texture.c_str();
							lua_pop(luaState, 1);
						}
						
					}
				}
				lua_pop(luaState, 1);
			}
		}
		//getting effects path
		{
			const char* const path = "Effect";
			lua_pushstring(luaState, path);
			lua_gettable(luaState, -2);
			if (lua_isnil(luaState, -1))
			{
				std::cerr << "No value for effect shader path";
				lua_pop(luaState, 1);
				wereThereErrors = true;
				EAE6320_ASSERT(true);
				return false;
			}
			if (lua_type(luaState, -1) != LUA_TSTRING)
			{
				std::cerr << "The value for effect path must be a string";
				// Pop the value
				lua_pop(luaState, 1);
				wereThereErrors = true;
				EAE6320_ASSERT(true);
				// (The asset table is now at -1)
				return false;
			}
			{
				const char* value = lua_tostring(luaState, -1);

				std::string effectpath = value;
				std::string errormsg = " ";

				bool issucess = eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(value, "effects", effectpath, &errormsg);
				std::cerr << "\n " << errormsg.c_str();
				if (!issucess)
				{
					wereThereErrors = true;

				}
				_effectspath = effectpath.c_str();
				lua_pop(luaState, 1);
			}
			lua_pop(luaState, 1);
		}



		//writing the values on to .meshfile
		if (!wereThereErrors)
		{
			std::string effect = "data/";
			effect += _effectspath;
			effect += "\0";
			std::string texture = "data/";
			texture += _texturePath;
			texture += "\0";

			uint16_t size = static_cast<uint16_t>(strlen(effect.c_str()));
			size++;
			uint16_t sizet = static_cast<uint16_t>(strlen(texture.c_str()));
			sizet++;
			FILE * pFile;
			pFile = fopen(m_path_target, "wb");
			if (pFile != NULL)
			{
				fwrite(&_matConstants, sizeof(eae6320::Graphics::sMaterialConstants), 1, pFile);
				fwrite(&size, sizeof(uint16_t), 1, pFile);
				fwrite(effect.c_str(), sizeof(char), size, pFile);
				fwrite(texture.c_str(), sizeof(char), sizet, pFile);
				fclose(pFile);
			}
		}
			else
				std::cerr << ("Can not write into %s", m_path_target);

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
