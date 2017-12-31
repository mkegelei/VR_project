#version 330 core

#define M_PI 3.1415926535897932384626433832795
layout (lines) in;
layout (triangle_strip, max_vertices = 100) out;

in VS_OUT {
  vec3 binormal;
  vec3 tangent;
  vec3 normal;
} gs_in[];

out vec4 fColor;

const float MAGNITUDE = 0.2;


void drawCylinder(float radius, float offset)  {

  vec4 offset_start = vec4(gs_in[0].normal, 0.0)*offset;
  vec4 offset_end = vec4(gs_in[1].normal, 0.0)*offset;
  vec4 start = gl_in[0].gl_Position + offset_start;
  vec4 end = gl_in[1].gl_Position + offset_end;
  int slices = 6;
  float dtheta = 2.0*M_PI/float(slices);
  for(int i=0; i<slices; i++) {

    float theta = dtheta*float(i);
    float nextTheta = dtheta*(float(i)+1);
    // A--------C
    // |        |
    // B--------D

    //vertices at edges of circle
    vec4 A = start + normalize(vec4(gs_in[0].normal, 0.0)*cos(theta) + vec4(gs_in[0].binormal, 0.0)*sin(theta)) * radius;
    vec4 B = start + normalize(vec4(gs_in[0].normal, 0.0)*cos(nextTheta) + vec4(gs_in[0].binormal, 0.0)*sin(nextTheta)) * radius;

    // the same vertices at the bottom of the cylinder
    vec4 C = end + normalize(vec4(gs_in[1].normal, 0.0)*cos(theta) + vec4(gs_in[1].binormal, 0.0)*sin(theta)) * radius;
    vec4 D = end + normalize(vec4(gs_in[1].normal, 0.0)*cos(nextTheta) + vec4(gs_in[1].binormal, 0.0)*sin(nextTheta)) * radius;
    fColor = vec4(1.0, 0.0, 0.0, 1.0);
    gl_Position = A;
    EmitVertex();
    fColor = vec4(1.0, 0.0, 1.0, 1.0);
    gl_Position = B;
    EmitVertex();
    fColor = vec4(1.0, 0.0, 0.0, 1.0);
    gl_Position = C;
    EmitVertex();
    fColor = vec4(1.0, 0.0, 1.0, 1.0);
    gl_Position = D;
    EmitVertex();
    fColor = vec4(1.0, 0.0, 0.0, 1.0);
    gl_Position = A;
    EmitVertex();

    EndPrimitive();
  }
}

void main()
{
    drawCylinder(0.3, 0.0);
    //drawCylinder(0.3, 1.0);

}
