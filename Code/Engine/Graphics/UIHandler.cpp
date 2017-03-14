#include "UIHandler.h"
#include "../UserSettings/UserSettings.h"
namespace eae6320
{

	namespace Graphics
	{
		namespace UIHandler
		{
			float desiredWidth = 512.0f;
			float desiredHeight = 512.0f;
			void setDesignResolution(float x, float y)
			{
				desiredWidth = x;
				desiredHeight = y;
			}
			void convertPointToResoultion(float o_left, float o_right, float o_bottom, float o_top, float i_texHeight, float i_texWidth)
			{
			  unsigned int currentWidth =	UserSettings::GetResolutionHeight();
			  unsigned int currentHeight =UserSettings::GetResolutionWidth();
			  float aspectX = currentWidth / desiredWidth;
			  float aspectY = currentHeight / desiredHeight;
			  float hightRatio = i_texHeight * aspectX;
			  float widthRation = i_texWidth * aspectY;
			  
			  
			  o_left *= aspectX;
			  o_right += widthRation;
			  o_bottom += hightRatio;
			  o_top *= aspectY;



			}
		}
	}
}