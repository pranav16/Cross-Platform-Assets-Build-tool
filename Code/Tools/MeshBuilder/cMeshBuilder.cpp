// Header Files
//=============

#include "cMeshBuilder.h"
#include "../AssetBuildLibrary/UtilityFunctions.h"
#include "../../Engine/Platform/Platform.h"
#include "../../External/Lua/Includes.h"
#include "../../Engine/Logging/Logging.h"
#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Graphics/Graphics.h"
#include <iostream>

// Inherited Implementation
//=========================

// Build
//------

uint8_t eae6320::AssetBuild::cMeshBuilder::normalizeColor(uint16_t component)
{
	return static_cast<uint8_t>(component * 255.0f);
}

bool eae6320::AssetBuild::cMeshBuilder::Build(const std::vector<std::string>&)
{

	bool wereThereErrors = false;
	uint16_t indexCount;
	uint16_t vertexCount;
	eae6320::Graphics::sVertex* vertex = nullptr;
	eae6320::Graphics::sIndex* indicies = nullptr;
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
				goto onExit;
			}
		}
		else
		{
			wereThereErrors = true;
			eae6320::Logging::OutputError("%s does not exist", m_path_source);
			goto onExit;

		}
		const char* const key = "verticies";
		lua_pushstring(luaState, key);
		lua_gettable(luaState, -2);
		if (lua_istable(luaState, -1))
		{
			vertexCount = luaL_len(luaState, -1);
			vertex = new eae6320::Graphics::sVertex[vertexCount];
			for (int i = 1; i <= vertexCount; ++i)
			{
				lua_pushinteger(luaState, i);
				lua_gettable(luaState, -2);

				if (lua_istable(luaState, -1))
				{
					const char* position = "position";
					lua_pushstring(luaState, position);
					lua_gettable(luaState, -2);
					if (lua_istable(luaState, -1))
					{
						const int postionLength = luaL_len(luaState, -1);
						EAE6320_ASSERT(postionLength == 3)
							lua_pushnil(luaState);
						int j = 0;
						while (lua_next(luaState, -2))
						{

							if (j == 0)
								vertex[i - 1].x = float(lua_tonumber(luaState, -1));
							else if (j == 1)
								vertex[i - 1].y = float(lua_tonumber(luaState, -1));
							else
								vertex[i - 1].z = float(lua_tonumber(luaState, -1));
							j++;
							lua_pop(luaState, 1);
						}
						lua_pop(luaState, 1);
					}
					const char* color = "color";
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
								vertex[i - 1].r = normalizeColor((uint16_t)lua_tonumber(luaState, -1));
							else if (k == 1)
								vertex[i - 1].g = normalizeColor((uint16_t)lua_tonumber(luaState, -1));
							else if (k == 2)
								vertex[i - 1].b = normalizeColor((uint16_t)lua_tonumber(luaState, -1));
							else if (k == 3)
								vertex[i - 1].a = normalizeColor((uint16_t)lua_tonumber(luaState, -1));
							k++;
							lua_pop(luaState, 1);
						}
						lua_pop(luaState, 1);
					}
					const char* uvs = "uvs";
					lua_pushstring(luaState, uvs);
					lua_gettable(luaState, -2);
					if (lua_istable(luaState, -1))
					{
						const int uvlength = luaL_len(luaState, -1);
						EAE6320_ASSERT(uvlength == 2)
							lua_pushnil(luaState);
						int k = 0;
						while (lua_next(luaState, -2))
						{
							#ifdef EAE6320_PLATFORM_D3D
							if (k == 0)
								vertex[i - 1].u = 1.0f - static_cast<float>( lua_tonumber(luaState, -1));
							else if (k == 1)
								vertex[i - 1].v = 1.0f- static_cast<float>(lua_tonumber(luaState, -1));
							k++;
                        #else
							if (k == 0)
								vertex[i - 1].u = static_cast<float>(lua_tonumber(luaState, -1));
							else if (k == 1)
								vertex[i - 1].v = static_cast<float>(lua_tonumber(luaState, -1));
							k++;
						#endif
							lua_pop(luaState, 1);
						}
						lua_pop(luaState, 1);
					}
					lua_pop(luaState, 1);

				}
			}
			lua_pop(luaState, 1);
		}


		const char* const indexKey = "Indicies";
		lua_pushstring(luaState, indexKey);
		lua_gettable(luaState, -2);
		if (lua_istable(luaState, -1))
		{
			indexCount = luaL_len(luaState, -1);
			indicies = new eae6320::Graphics::sIndex[indexCount];
			for (int i = 1; i <= indexCount; ++i)
			{
				lua_pushinteger(luaState, i);
				lua_gettable(luaState, -2);
				lua_pushnil(luaState);
				int j = 0;
				while (lua_next(luaState, -2))
				{
					if (j == 0)
						indicies[i - 1].index1 = (uint16_t)lua_tonumber(luaState, -1);
					else if (j == 1)
						indicies[i - 1].index2 = (uint16_t)lua_tonumber(luaState, -1);
					else
						indicies[i - 1].index3 = (uint16_t)lua_tonumber(luaState, -1);
					j++;
					lua_pop(luaState, 1);
				}
				lua_pop(luaState, 1);

			}
			lua_pop(luaState, 1);
		}
		lua_pop(luaState, 1);


		//writing the values on to .meshfile
		if (!wereThereErrors)
		{
			std::cerr << "\n vertext count is for  " << m_path_source << vertexCount;
			std::cerr << " \n index count is for  " << m_path_source << indexCount;
#ifdef EAE6320_PLATFORM_D3D
			for (int i = 0; i < indexCount; i++)
			{
				uint16_t temp = indicies[i].index3;
				indicies[i].index3 = indicies[i].index2;
				indicies[i].index2 = temp;

			}
#endif // EAE6320_PLATFORM_D3D


			FILE * pFile;
			pFile = fopen(m_path_target, "wb");
			if (pFile != NULL)
			{
				fwrite(&indexCount, sizeof(uint16_t), 1, pFile);
				fwrite(&vertexCount, sizeof(uint16_t), 1, pFile);
				fwrite(vertex, sizeof(eae6320::Graphics::sVertex), vertexCount, pFile);
				fwrite(indicies, sizeof(eae6320::Graphics::sIndex), indexCount, pFile);
				fclose(pFile);
			}
			else
				std::cerr << ("Can not write into %s", m_path_target);

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

		EAE6320_ASSERTF(!wereThereErrors, "failed to load %s!", m_path_source);
	}

	return !wereThereErrors;
}
