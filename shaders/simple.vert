#version 450 core

in layout(location = 0) vec4 pos;

void main() 
{
	gl_Position = pos;
}
