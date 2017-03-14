#pragma once
#ifndef _CAMERA_H
#define _CAMERA_H
#include "../Math/cQuaternion.h"
#include "../Math/cVector.h"
#include "../Math/cMatrix_transformation.h"
namespace eae6320
{

	namespace Graphics
	{
		class Camera
		{
		public:
			 float getFieldOfView() const;
			 float getNearClippingPlane() const;
			 float getFarClippinPlane() const;
			 float getAspectRatio() const;
			 Math::cQuaternion getRotation() const;
			 Math::cVector getPosition() const;


			 void setFieldOfView(float i_fieldOfView);
			 void setNearClippingPlane(float i_nearPlane);
			 void setFarClippinPlane(float i_farClippingPlane);
			 void setAspectRation(float i_aspectRatio);
			 void setRotation(Math::cQuaternion i_roation);
			 void setPosition(Math::cVector i_postion);

			Math::cMatrix_transformation getWorldToCamera()const;
			Math::cMatrix_transformation getCameraToScreen() const;
			Camera();



		private:
			float m_fieldOfViewInRadians;
			float m_nearFieldPlane;
			float m_farClippingPlane;
			float m_aspectRatio;
			Math::cQuaternion m_roatation;
			Math::cVector m_position;


		};
	}//namespace graphics
}//namespace eae

#include "Camera.Ini.h"

#endif // !_CAMERA_H
