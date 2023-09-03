#version 450

//triangle vertices
vec2 positions[3] = vec2[] (
	vec2(0.0, -0.5),
	vec2(0.5, 0.5),
	vec2(-0.5, 0.5)
); 

// this main function will be executed once for each vertex that we have.
// Vertex shader gets groups of numbers from input assembler and 
// performs transformations like translations or rotations 
// and returns positions.
// Our vert shader gets each vertex and then returns a position
void main() {
    
	// fisrst parameter is x and y, sceond param is z, fourth is not used so far
	// gl_VertexIndex is a built-in index that holds current vertex value (we process one vertex at a time)
	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}