#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VERT_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} vert_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

uniform mat3 normalCorrection;

void main()
{
    vert_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vert_out.Normal = normalCorrection * aNormal;  
    vert_out.TexCoords = aTexCoords;
    vert_out.FragPosLightSpace = lightSpaceMatrix * vec4(vert_out.FragPos, 1.0);
    gl_Position = projection * view * vec4(vert_out.FragPos, 1.0);
}
