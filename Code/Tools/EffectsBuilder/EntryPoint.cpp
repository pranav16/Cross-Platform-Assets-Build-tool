/*
	The main() function is where the program starts execution
*/

// Header Files
//=============

#include "cEffectsBuilder.h"

// Entry Point
//============

int main( int i_argumentCount, char** i_arguments )
{
	
	return eae6320::AssetBuild::Build<eae6320::AssetBuild::cEffectsBuilder>( i_arguments, i_argumentCount );
}
