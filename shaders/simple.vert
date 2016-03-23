#version 450 core

in layout(location = 0) vec3 pos;

out vec3 vertexPos;

layout (set=0, binding=0) uniform Transform {
	mat4 perspective;
};

void main() 
{
	vertexPos = normalize(pos.xyz);

	gl_Position = perspective * vec4(pos.xyz, 1.0);
	//gl_Position = vec4(pos.xyz, 1.0);
}
