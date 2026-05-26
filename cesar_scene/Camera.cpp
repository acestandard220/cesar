#include "Camera.h"
#include "../cesar_core/Event/Input.h"
#include "../cesar_core/Core/MathConstants.h"

using namespace DirectX::SimpleMath;

namespace cesar
{
	Camera::Camera(CameraParameters const& desc) 
		: position(desc.position), aspect_ratio(1.0f), fov(desc.fov), near_plane(desc.near_plane), far_plane(desc.far_plane), enabled(true), changed(false)
	{
		Vector3 look_vector = desc.look_at - position;
		look_vector.Normalize();

		Float yaw = std::atan2(look_vector.x, look_vector.z);
		Float pitch = std::asin(std::clamp(-look_vector.y, -1.0f, 1.0f));
		Quaternion pitch_quat = Quaternion::CreateFromYawPitchRoll(0, pitch, 0);
		Quaternion yaw_quat = Quaternion::CreateFromYawPitchRoll(yaw, 0, 0);
		orientation = pitch_quat * orientation * yaw_quat;
	}

	Vector3 Camera::Forward() const
	{
		return Vector3::Transform(Vector3::Forward, orientation);
	}

	Float Camera::Near() const
	{
		return near_plane;
	}
	Float Camera::Far() const
	{
		return far_plane;
	}
	Float Camera::Fov() const
	{
		return fov;
	}
	Float Camera::AspectRatio() const
	{
		return aspect_ratio;
	}

	void Camera::Update(Float dt)
	{
		changed = false;
		if (!enabled || gInput.GetKey(Key::Space))
		{
			return;
		}

		if (gInput.GetKey(Key::MouseRight))
		{
			Float sensitivity = 0.2f;
			if (gInput.GetKey(Key::Shift)) sensitivity *= 1.50f;
			if (gInput.GetKey(Key::Control))  sensitivity *= 0.5f;

			Float dx = gInput.GetMouseDeltaX();
			Float dy = gInput.GetMouseDeltaY();
			Quaternion pitch_quat = Quaternion::CreateFromYawPitchRoll(0, dy * sensitivity * 0.25f, 0);
			Quaternion yaw_quat = Quaternion::CreateFromYawPitchRoll(dx * sensitivity * 0.25f, 0, 0);
			orientation = pitch_quat * orientation * yaw_quat;
			changed = true;
		}

		Vector3 movement{};
		if (gInput.GetKey(Key::W)) movement.z += 1.0f;
		if (gInput.GetKey(Key::S)) movement.z -= 1.0f;
		if (gInput.GetKey(Key::D)) movement.x += 1.0f;
		if (gInput.GetKey(Key::A)) movement.x -= 1.0f;
		if (gInput.GetKey(Key::Q)) movement.y += 1.0f;
		if (gInput.GetKey(Key::E)) movement.y -= 1.0f;
		movement = Vector3::Transform(movement, orientation);
		velocity = Vector3::SmoothStep(velocity, movement, 0.35f);

		if (velocity.LengthSquared() > 1e-4)
		{
			Float speed_factor = 1.0f;
			if (gInput.GetKey(Key::Shift)) speed_factor *= 2.0f;
			if (gInput.GetKey(Key::Control))  speed_factor *= 0.05f;
			position += velocity * dt * speed_factor * 25.0f;
			changed = true;
		}
		
		Matrix view_inverse = Matrix::CreateFromQuaternion(orientation) * Matrix::CreateTranslation(position);
		view_inverse.Invert(view_matrix);
		SetProjectionMatrix(fov, aspect_ratio, near_plane, far_plane);
	}
	void Camera::Zoom(Int32 increment)
	{
		if (!enabled) return;
		fov -= XMConvertToRadians(increment * 1.0f);
		fov = std::clamp(fov, 0.00005f, pi_div_2<Float>);
		SetProjectionMatrix(fov, aspect_ratio, near_plane, far_plane);
	}
	void Camera::OnResize(Uint32 w, Uint32 h)
	{
		SetAspectRatio(static_cast<Float>(w) / h);
	}

	void Camera::SetAspectRatio(Float ar)
	{
		aspect_ratio = ar;
		SetProjectionMatrix(fov, aspect_ratio, near_plane, far_plane);
	}
	void Camera::SetFov(Float _fov)
	{
		fov = _fov;
		SetProjectionMatrix(fov, aspect_ratio, near_plane, far_plane);
	}
	void Camera::SetNearAndFar(Float n, Float f)
	{
		near_plane = n;
		far_plane = f;
		SetProjectionMatrix(fov, aspect_ratio, near_plane, far_plane);
	}
	void Camera::SetPosition(Vector3 const& pos)
	{
		position = pos;
	}
	void Camera::SetOrientation(Quaternion const& q)
	{
		orientation = q;
	}

	const Matrix& Camera::View() const
	{
		return view_matrix;
	}
	const Matrix& Camera::Projection() const
	{
		return projection_matrix;
	}
	const Matrix& Camera::ViewProjection() const
	{
		return view_matrix * projection_matrix;
	}

	const Matrix& Camera::InverseView() const
	{
		return view_matrix.Invert();
	}

	const Matrix& Camera::InverseProjection()const
	{
		return projection_matrix.Invert();
	}

	const Matrix& Camera::InverseViewProjection()const
	{
		return ViewProjection().Invert();
	}

	BoundingFrustum Camera::Frustum() const
	{
		BoundingFrustum frustum(Projection());
		if (frustum.Far < frustum.Near) std::swap(frustum.Far, frustum.Near);
		frustum.Transform(frustum, view_matrix.Invert());
		return frustum;
	}
	void Camera::SetProjectionMatrix(Float fov, Float aspect, Float zn, Float zf)
	{
		projection_matrix = XMMatrixPerspectiveFovLH(fov, aspect, zn, zf);
	}
}