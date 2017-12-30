#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
  vec3 binormal;
  vec3 tangent;
  vec3 normal;
} gs_in[];

out vec3 fColor;

const float MAGNITUDE = 0.2;

void GenerateLine(vec3 vector)
{
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(vector, 0.0) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    /*
    fColor = vec3(1.0, 1.0, 1.0);
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    EndPrimitive();
    */

    fColor = vec3(1.0, 0.0, 0.0);
    GenerateLine(gs_in[0].binormal);
    fColor = vec3(0.0, 1.0, 0.0);
    GenerateLine(gs_in[0].tangent);
    fColor = vec3(0.0, 0.0, 1.0);
    GenerateLine(gs_in[0].normal);

}