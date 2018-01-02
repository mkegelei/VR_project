#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VERT_OUT {
	vec4 fragPos;
	vec3 normal;
	vec2 texCoords;
	vec4 fragPosLightSpace;
} vert_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 lightSpaceMatrix;

uniform mat3 normalMatrix;

void main()
{
    vert_out.fragPos = model * vec4(aPos, 1.0);
    vert_out.normal = normalMatrix * aNormal;  
    vert_out.texCoords = aTexCoords;
    vert_out.fragPosLightSpace = lightSpaceMatrix * vert_out.fragPos;
    gl_Position = view * vert_out.fragPos;
}
