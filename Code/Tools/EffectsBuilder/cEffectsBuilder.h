/*
	This class builds meshes
*/

#ifndef EAE6320_CMESHBUILDER_H
#define EAE6320_CMESHBUILDER_H

// Header Files
//=============

#include "../AssetBuildLibrary/cbBuilder.h"

// Class Declaration
//==================

namespace eae6320
{
	namespace AssetBuild
	{
		class cEffectsBuilder : public cbBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------
			uint8_t normalizeColor(uint16_t component);
			virtual bool Build( const std::vector<std::string>& i_arguments );
		};
	}
}

#endif	// EAE6320_CMESHBUILDER_H
