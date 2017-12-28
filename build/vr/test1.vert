#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
<<<<<<< HEAD
out vec3 FragPos;
out vec3 Normal;
=======
>>>>>>> 1e57215b089641099e79fd8f7b0b1f408357cc43

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
<<<<<<< HEAD
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Normal = aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
}
=======
    TexCoords = aTexCoords;    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

>>>>>>> 1e57215b089641099e79fd8f7b0b1f408357cc43
