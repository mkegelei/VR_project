#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aBinormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aNormal;

out VS_OUT {
  vec3 binormal;
  vec3 tangent;
  vec3 normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.binormal = normalize(vec3(projection * vec4(normalMatrix * aBinormal, 1.0)));
    vs_out.tangent = normalize(vec3(projection * vec4(normalMatrix * aTangent, 1.0)));
    vs_out.normal = normalize(vec3(projection * vec4(normalMatrix * aNormal, 1.0)));
}
