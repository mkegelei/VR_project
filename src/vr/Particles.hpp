#ifndef PARTICLES_H
#define PARTICLES_H
#define MAX_PARTICLES 30000
// Inspired by http://www.opengl-tutorial.org/fr/intermediate-tutorials/billboards-particles/particles-instancing/
#include <map>
#include <vector>
#include <Shader.hpp>
#include <typeinfo>

struct Particle{
	glm::vec3 pos, speed;
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Particles
{
public:

  Particles() {

    // The VBO containing the 4 vertices of the particles.
    // Thanks to instancing, they will be shared by all particles.


    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &verticesVBO);
    glGenBuffers(1, &VBO);

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, 12*sizeof(float), vertexData, GL_STREAM_DRAW);

    // Position + color
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 *sizeof(float) , data, GL_STREAM_DRAW);

    // Attributes
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    // position + size
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);



    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // positions : one per quad (its center) -> 1

    for(int i=0; i<MAX_PARTICLES; i++){
    		ParticlesContainer[i].life = -1.0f;
    		ParticlesContainer[i].cameradistance = -1.0f;
    }
  }

  void generateParticles(float delta, glm::vec3 emitterPos, glm::vec3 maindir) {
    // Generate 10 new particule each millisecond,
		// but limit this to 16 ms (60 fps), or if you have 1 long frame (1sec),
		// newparticles will be huge and the next frame even longer.
		int newparticles = (int)(delta*10000.0);
		if (newparticles > (int)(0.016f*10000.0))
			newparticles = (int)(0.016f*10000.0);

		for(int i=0; i<newparticles; i++){
			int particleIndex = FindUnusedParticle();
			ParticlesContainer[particleIndex].life = (rand()%300)/100.0f; // This particle will live 5 seconds.
			ParticlesContainer[particleIndex].pos = emitterPos;

			float spread = 0.6f;
			// Very bad way to generate a random direction;
			// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
			// combined with some user-controlled parameters (main direction, spread, etc)
			glm::vec3 randomdir = glm::vec3(
				(rand()%2000 - 1000.0f)/1000.0f,
				(rand()%2000 - 1000.0f)/1000.0f,
				(rand()%2000 - 1000.0f)/1000.0f
			);

			ParticlesContainer[particleIndex].speed = maindir + randomdir*spread;

      //cout << (rand()%1000)/1000.0f << typeid((rand()%1000)/1000.0f).name();
      //cout << ParticlesContainer[particleIndex].r << ", " << ParticlesContainer[particleIndex].g << ", " << ParticlesContainer[particleIndex].b << ", " << ParticlesContainer[particleIndex].a << endl;

			ParticlesContainer[particleIndex].size = ((rand()%1000)/2000.0f + 0.1f)*0.05;
		}
  }

  void simulatePhysics(float delta, glm::vec3 cameraPos) {
    // Simulate all particles
		ParticlesCount = 0;
		for(int i=0; i<MAX_PARTICLES; i++){

			Particle& p = ParticlesContainer[i]; // shortcut

			if(p.life > 0.0f){

				// Decrease life
				p.life -= delta;
				if (p.life > 0.0f){

					// Simulate simple physics : gravity only, no collisions
					//p.speed += glm::vec3(0.0f,-9.81f, 0.0f) * (float)delta * 0.2f;
					p.pos += p.speed * (float)delta;
					p.cameradistance = glm::length( p.pos - cameraPos )*glm::length( p.pos - cameraPos );
					//ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

					// Fill the GPU buffer
					data[4*ParticlesCount+0] = p.pos.x;
					data[4*ParticlesCount+1] = p.pos.y;
					data[4*ParticlesCount+2] = p.pos.z;

					data[4*ParticlesCount+3] = p.size * p.life;


				}else{
					// Particles that just died will be put at the end of the buffer in SortParticles();
					p.cameradistance = -1.0f;
				}
				ParticlesCount++;
			}
    }
    SortParticles();
  }

  void Draw() {
    // Update the buffers that OpenGL uses for rendering.
		// There are much more sophisticated means to stream data from the CPU to the GPU,
		// but this is outside the scope of this tutorial.
		// http://www.opengl.org/wiki/Buffer_Object_Streaming



		//glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

    // Draw the particules !
    // This draws many times a small triangle_strip (which looks like a quad).
    // This is equivalent to :
    // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
    // but faster.
    glBindVertexArray(VAO);
    // Position + color
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 8 *sizeof(float) , NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, data);
    // Attributes
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);


    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // positions : one per quad (its center) -> 1

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);
    glBindVertexArray(0);
  }

private:
  unsigned int ParticlesCount;
  Particle ParticlesContainer[MAX_PARTICLES];

  unsigned int verticesVBO;
  unsigned int VBO;
  unsigned int VAO;

  GLfloat data[MAX_PARTICLES * 8];

  int LastUsedParticle = 0;
  float vertexData[12] = {
   -0.5f, -0.5f, 0.0f,
   0.5f, -0.5f, 0.0f,
   -0.5f, 0.5f, 0.0f,
   0.5f, 0.5f, 0.0f,
  };

  // Finds a Particle in ParticlesContainer which isn't used yet.
  // (i.e. life < 0);
  int FindUnusedParticle(){

      for(int i=LastUsedParticle; i<MAX_PARTICLES; i++){
          if (ParticlesContainer[i].life < 0){
              LastUsedParticle = i;
              return i;
          }
      }

      for(int i=0; i<LastUsedParticle; i++){
          if (ParticlesContainer[i].life < 0){
              LastUsedParticle = i;
              return i;
          }
      }
      return 0; // All particles are taken, override the first one
  }
  void SortParticles(){
	   std::sort(&ParticlesContainer[0], &ParticlesContainer[MAX_PARTICLES]);
  }
};
#endif
