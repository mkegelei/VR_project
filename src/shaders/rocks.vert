#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out VERT_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} vert_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 lightSpaceMatrix;


void main()
{
    vert_out.FragPos = vec3(aInstanceMatrix * vec4(aPos, 1.0));
    vert_out.Normal = aNormal;  
    vert_out.TexCoords = aTexCoords;
    vert_out.FragPosLightSpace = lightSpaceMatrix * vec4(vert_out.FragPos, 1.0);
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0f); 
}