#version 450

//output variable
layout (location = 0) out vec4 outColor;
// the fragment shader runs on a per fragment basis which are determined by what pixels 
// our geometry mostly contains during the rasterization stage

void main() {
	outColor = vec4(1.0, 0.0, 0.0, 1.0);
}