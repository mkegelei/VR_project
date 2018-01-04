#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;


out VERT_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
	mat3 TBN;
} vert_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

uniform mat3 normalMatrix;

void main()
{
    vert_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vert_out.Normal = normalMatrix * aNormal;  
    vert_out.TexCoords = aTexCoords;
    vert_out.FragPosLightSpace = lightSpaceMatrix * vec4(vert_out.FragPos, 1.0);
    
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));
    TBN = mat3(1.0f);
    vert_out.TBN = TBN;

    gl_Position = projection * view * vec4(vert_out.FragPos, 1.0);
}
