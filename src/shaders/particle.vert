#version 330 core
layout(location = 0) in vec3 aPos; //position of the vertex
layout(location = 1) in vec4 xyzs; // Position of the center of the particule and size of the square
layout(location = 2) in vec4 color; // Position of the center of the particule and size of the square

out vec4 fColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos*xyzs.w + xyzs.xyz, 1.0);
  fColor = color;
}
