#pragma once
#ifndef  _UIHANDLER_H_
#include"GraphicsDataTypes.h"
namespace eae6320
{

	namespace Graphics
	{
		namespace UIHandler
		{
			extern float desiredWidth;
			extern float desiredHeight;
			void setDesignResolution(float x, float y);
			void convertPointToResoultion(float o_left,float o_right,float o_bottom,float o_top,float i_texHeight,float i_texWidth);
		}
	}
}


#endif // ! _UIHANDLER_H_
