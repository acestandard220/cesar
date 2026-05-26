#pragma once
#include "../cesar_core/cesar_core.h"

#include <SimpleMath/SimpleMath.h>

using namespace DirectX::SimpleMath;

namespace cesar {
	
	//Bug: Camera Data goes in as column major for some reason. 
	//Mesh Transform data goes in as row_major	
	struct FrameData {
		Matrix view;
		Matrix projection;
		Matrix view_projection;

		Matrix inverse_view;
		Matrix inverse_projection;
		Matrix inverse_view_projection;

		Vector4 camera_position;
		Float   camera_fov;
		Float   camera_near;
		Float   camera_far;

		Uint32 screen_width;
		Uint32 screen_height;

		float _pad[23];
	};

}