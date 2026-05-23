#pragma once
#include "../cesar_core/cesar_core.h"

#include <SimpleMath/SimpleMath.h>

using namespace DirectX::SimpleMath;

namespace cesar {
	
	struct FrameData {
		Matrix view;
		Matrix projection;
		Matrix view_projection;

		Vector4 camera_position;
		Float camera_fov;
		Float camera_near;
		Float camera_far;
		
		float p[9];
	};

}