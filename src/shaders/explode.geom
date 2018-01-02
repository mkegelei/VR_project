#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VERT_OUT {
    vec4 fragPos;
    vec3 normal;
    vec2 texCoords;
    vec4 fragPosLightSpace;
} geom_in[];

out GEOM_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} geom_out;

uniform float time;
uniform mat4 projection;

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    vec3 direction = -normal * ((sin(5*time) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction, 0.0);
}

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main() {  
    vec3 norm = GetNormal();  
    gl_Position = projection * explode(gl_in[0].gl_Position, norm);
    geom_out.FragPos = explode(geom_in[0].fragPos, norm).xyz;
    geom_out.Normal = geom_in[0].normal;
    geom_out.TexCoords = geom_in[0].texCoords;
    geom_out.FragPosLightSpace = geom_in[0].fragPosLightSpace;
    EmitVertex();
    gl_Position = projection * explode(gl_in[1].gl_Position, norm);
    geom_out.FragPos = explode(geom_in[1].fragPos, norm).xyz;
    geom_out.Normal = geom_in[1].normal;
    geom_out.TexCoords = geom_in[1].texCoords;
    geom_out.FragPosLightSpace = geom_in[1].fragPosLightSpace;
    EmitVertex();
    gl_Position = projection * explode(gl_in[2].gl_Position, norm);
    geom_out.FragPos = explode(geom_in[2].fragPos, norm).xyz;
    geom_out.Normal = geom_in[2].normal;
    geom_out.TexCoords = geom_in[2].texCoords;
    geom_out.FragPosLightSpace = geom_in[2].fragPosLightSpace;
    EmitVertex();
    EndPrimitive();
}