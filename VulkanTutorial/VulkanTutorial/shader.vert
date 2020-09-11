#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
	vec4 gl_Position; // vec4 means four-dimensional vector. Four dimensions are: x, y, z and last dimention is always 1 for technical reasons. Explanation will be in the end of tutorial.
};

vec2 positions[3] = vec2[](
	vec2(0.0, -0.5),
	vec2(0.5, 0.5),
	vec2(-0.5, 0.5)
);

// this function will be executed for each vertex
void main()
{
	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}