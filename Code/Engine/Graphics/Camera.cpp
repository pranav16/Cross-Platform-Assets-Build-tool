#include "Camera.h"
namespace eae6320
{
	namespace Graphics
	{
		 float Camera::getFieldOfView() const
		{
			return m_fieldOfViewInRadians;
		}
		 float Camera::getNearClippingPlane() const
		{
			return m_nearFieldPlane;
		}
		 float Camera::getFarClippinPlane() const
		{
			return m_farClippingPlane;
		}
		 float Camera::getAspectRatio() const
		{
			return m_aspectRatio;
		}
		 Math::cQuaternion Camera::getRotation() const
		{
			return m_roatation;
		}
		 Math::cVector Camera::getPosition() const
		{
			return m_position;
		}
		 void Camera::setAspectRation(float i_aspectRatio)
		{
			m_aspectRatio = i_aspectRatio;
		}
		 void Camera::setFieldOfView(float i_fieldOfView)
		{
			m_fieldOfViewInRadians = i_fieldOfView;
		}
		 void Camera::setNearClippingPlane(float i_nearPlane)
		{
			m_nearFieldPlane = i_nearPlane;
		}
		 void Camera::setFarClippinPlane(float i_farClippingPlane)
		{
			m_farClippingPlane = i_farClippingPlane;
		}
		 void Camera::setRotation(Math::cQuaternion i_roation)
		{
			m_roatation = i_roation;
		}
		 void Camera::setPosition(Math::cVector i_postion)
		{
			m_position = i_postion;
		}

		Math::cMatrix_transformation Camera::getWorldToCamera()const
		{
			return Math::cMatrix_transformation::CreateWorldToCameraTransform(m_roatation, m_position);
		}
		Math::cMatrix_transformation Camera::getCameraToScreen() const
		{
			return Math::cMatrix_transformation::CreateCameraToScreenTransform_perspectiveProjection(m_fieldOfViewInRadians, m_aspectRatio, m_nearFieldPlane, m_farClippingPlane);
		}
		Camera::Camera()
		{
			m_position.z = 10.0f;
			m_aspectRatio = 1.0f;
			m_nearFieldPlane = 0.1f;
			m_fieldOfViewInRadians = 1.0472f;
			m_farClippingPlane = 100.0f;
			m_nearFieldPlane = 0.1f;

		}

	}
}