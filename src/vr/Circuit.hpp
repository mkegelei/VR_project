#ifndef CIRCUIT_H
#define CIRCUIT_H
// Inspired from http://devmag.org.za/2011/04/05/bzier-curves-a-tutorial/ and https://gist.github.com/epatel/1107754
// Continuous bezier curve http://www.algosome.com/articles/continuous-bezier-curve-line.html
#define SEGMENTS_PER_CURVE 30
#define M_PI 3.1415926535897932384626433832795

#include <map>
#include <vector>
#include <Shader.hpp>

struct bezier {
  float x0;
  float y0;
  float z0;
  float ax;
  float bx;
  float cx;
  float ay;
  float by;
  float cy;
  float az;
  float bz;
  float cz;
  void setup(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
  glm::vec3 calc(float t);
};

void bezier::setup(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
  x0 = p0.x;
  y0 = p0.y;
  z0 = p0.z;
  cx = 3.0*(p1.x - p0.x);
  bx = 3.0*(p2.x - p1.x) - cx;
  ax = p3.x - p0.x - cx - bx;
  cy = 3.0*(p1.y - p0.y);
  by = 3.0*(p2.y - p1.y) - cy;
  ay = p3.y - p0.y - cy - by;
  cz = 3.0*(p1.z - p0.z);
  bz = 3.0*(p2.z - p1.z) - cz;
  az = p3.z - p0.z - cz - bz;
}

glm::vec3 bezier::calc(float t) {
  glm::vec3 p;
  float t2 = t*t;
  float t3 = t2*t;
  p.x = ax*t3 + bx*t2 + cx*t + x0;
  p.y = ay*t3 + by*t2 + cy*t + y0;
  p.z = az*t3 + bz*t2 + cz*t + z0;
  return p;
}

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Circuit
{
public:

  Circuit() {
    const int num_points = 10;
    float points[(const int)(num_points*3)] = {
      5.0, 4.0, 1.0,
      3.0, 3.0, 1.0,
      2.0, 1.0, 1.0,

      1.0, 1.0, 2.0,
      0.0, 4.0, 0.0,

      -2.0, 3.0, -1.0,
      -2.0, -1, -1.5,

      0.0, -1.0, 1.0,
      2.0, -2.0, 1.0,

      4.0, -2.0, 1.0

    };


    // convert array of floats to vector of points
    vector<glm::vec3>controls; // temp control points
    for (int i = 0; i < num_points; i++) {
      glm::vec3 p;
      p.x = points[i*3];
      p.y = points[i*3+1];
      p.z = points[i*3+2];
      controls.push_back(p);
    }

    this->setupControlPoints(controls);
    this->computePoints();
    this->computeCylindersVertices(this->cylinderVertices1, 0.2);
    this->computeCylindersVertices(this->cylinderVertices2, -0.2);
    this->setupCylinderDrawing(this->VAOCylinder1, this->VBOCylinder1, this->cylinderVertices1);
    this->setupCylinderDrawing(this->VAOCylinder2, this->VBOCylinder2, this->cylinderVertices2);
    this->setupDrawing();

  }

  void Draw() {
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_LINE_STRIP, 0, this->num_points);
  }

  void DrawCylinders() {
    glBindVertexArray(this->VAOCylinder1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, this->num_vertices_cylinder);
    glBindVertexArray(this->VAOCylinder2);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, this->num_vertices_cylinder);
  }

  void DrawBTN() {
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_POINTS, 0, this->num_points);
  }


  private:
    vector<glm::vec3>controlPoints;
    vector<glm::vec3>points;
    vector<glm::vec3>cylinderVertices1;
    vector<glm::vec3>cylinderVertices2;
    unsigned int VBO, VAO; // Main curve
    unsigned int VAOCylinder1, VAOCylinder2;
    unsigned int VBOCylinder1, VBOCylinder2;
    unsigned int num_points;
    unsigned int num_vertices_cylinder;

    void setupControlPoints(vector<glm::vec3>controls) {
      // Need to add control points to ensure the gradient is the same between two bézier curves.
      this->controlPoints.push_back(controls[0]);
      for (unsigned int i = 1; i < controls.size() - 2; i+=2 ){
        this->controlPoints.push_back(controls[i]);
        this->controlPoints.push_back(controls[i+1]);
    		this->controlPoints.push_back(center(controls[i+1], controls[i+2]));
    	}
      this->controlPoints.push_back(controls[controls.size()-1]);
      glm::vec3 p; // glm::vec3 to connect the start and the end of the loop with the same gradient
      glm::vec3 start = controls[0];
      glm::vec3 second = controls[1];
      p.x = 2*start.x - second.x;
      p.y = 2*start.y - second.y;
      p.z = 2*start.z - second.z;
      this->controlPoints.push_back(p);
      this->controlPoints.push_back(start);
      cout << "Control points (" << this->controlPoints.size() << ") :" << endl << flush;
      for (unsigned int i = 0; i < this->controlPoints.size(); i++) {
        cout << "P" << i << " (" << this->controlPoints[i].x << ", " << this->controlPoints[i].y << ", " << this->controlPoints[i].z << ")" << endl;
      }
    }

    // Compute points for the Bézier curves
    vector<glm::vec3> computePoints()
    {
      for(unsigned int i = 0; i < this->controlPoints.size() - 3; i+=3) {
        glm::vec3 p0 = this->controlPoints[i];
        glm::vec3 p1 = this->controlPoints[i + 1];
        glm::vec3 p2 = this->controlPoints[i + 2];
        glm::vec3 p3 = this->controlPoints[i + 3];
        bezier b;
        b.setup(p0, p1, p2, p3);

        if(i == 0) //Only do this for the first endpoint.
                   //When i != 0, this coincides with the end
                   //glm::vec3 of the previous segment
        {
          points.push_back(b.calc(0));
        }

        for(int j = 1; j <= SEGMENTS_PER_CURVE; j++)
        {
          float t = j / (float) SEGMENTS_PER_CURVE;
          glm::vec3 p = b.calc(t);
          addBTN(points, p, points[points.size()-1]);
          points.push_back(p);
        }
      }
      // this line adds NANs ??? :
      addBTN(points, points[points.size()-2], points[0]); // BTN for the last glm::vec3 : connect end with start
      this->num_points = (int) points.size()/4;
      cout << "num_points " << this->num_points << endl;
      return points;
    }

    void computeCylindersVertices(vector<glm::vec3> &cylinderVertices, const float offset) {
      const float radius = 0.1;
      const int slices = 6;
      const float dtheta = 2.0*M_PI/float(slices);
      for (int j=0;  j < this->points.size()-4; j+=4) {

        glm::vec3 binormalStart = this->points[j+1];
        glm::vec3 binormalEnd = this->points[j+5];

        glm::vec3 normalStart = this->points[j+3];
        glm::vec3 normalEnd = this->points[j+7];

        glm::vec3 offset_start = normalStart*offset;
        glm::vec3 offset_end = normalEnd*offset;

        glm::vec3 start = this->points[j] + offset_start;
        glm::vec3 end = this->points[j+4] + offset_end;
        //cout << "P start" << j << " (" << start.x << ", " << start.y << ", " << start.z << ")" << endl;
        //cout << "P end" << j << " (" << end.x << ", " << end.y << ", " << end.z << ")" << endl;


        //cout << "Cylinder " << j << endl << flush;
        for(int i=0; i<slices; i++) {

          float theta = dtheta*float(i);
          float nextTheta = dtheta*(float(i)+1);
          // A--------C
          // |        |
          // B--------D

          //vertices at edges of circle
          glm::vec3 A = start + normalize(normalStart*cos(theta) + binormalStart*sin(theta)) * radius;
          glm::vec3 B = start + normalize(normalStart*cos(nextTheta) + binormalStart*sin(nextTheta)) * radius;

          // the same vertices at the bottom of the cylinder
          glm::vec3 C = end + normalize(normalEnd*cos(theta) + binormalEnd*sin(theta)) * radius;
          glm::vec3 D = end + normalize(normalEnd*cos(nextTheta) + binormalEnd*sin(nextTheta)) * radius;
          cylinderVertices.push_back(A);
          cylinderVertices.push_back(B);
          cylinderVertices.push_back(C);
          cylinderVertices.push_back(D);
          cylinderVertices.push_back(A);

          //cout << "PA" << i << " (" << A.x << ", " << A.y << ", " << A.z << ")" << endl;
          //cout << "PB" << i << " (" << B.x << ", " << B.y << ", " << B.z << ")" << endl;
          //cout << "PC" << i << " (" << C.x << ", " << C.y << ", " << C.z << ")" << endl;
          //cout << "PD" << i << " (" << D.x << ", " << D.y << ", " << D.z << ")" << endl;

        }
      }
      this->num_vertices_cylinder = cylinderVertices.size();
      cout << "num_vertices_cylinder:  " << cylinderVertices.size() << endl << flush;
    }

    void addBTN(vector<glm::vec3>&points, glm::vec3 current, glm::vec3 previous) {

      //cout << "current" << " (" << current.x << ", " << current.y << ", " << current.z << ")" << endl;
      //cout << "previous" << " (" << previous.x << ", " << previous.y << ", " << previous.z << ")" << endl;
      glm::vec3 T = normalize(current - previous); // Tangent
      glm::vec3 B = normalize(cross(T, current + previous)); // Binormal
      glm::vec3 N = normalize(cross(B,T));  // Normal
      points.push_back(B);
      points.push_back(T);
      points.push_back(N);
    }

    void setupDrawing() {

      float vertices[points.size()*3];
      // Convert vector<glm::vec3>to float array
      for (unsigned int i = 0; i < points.size(); i++) {
        vertices[i*3] = points[i].x;
        vertices[i*3+1] = points[i].y;
        vertices[i*3+2] = points[i].z;
        //cout << "P" << i << " (" << points[i].x << ", " << points[i].y << ", " << points[i].z << ")" << endl;
      }
      // For each point, we have 4 vectors: Position, Binormal, Tangent, Normal
      cout << "Bézier points:  " << points.size() << endl << flush;
      glGenVertexArrays(1, &this->VAO);
      glGenBuffers(1, &this->VBO);

      glBindVertexArray(this->VAO);

      glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

      // position attribute
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);
      // Binormal
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
      glEnableVertexAttribArray(1);
      // Tangent
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(6 * sizeof(float)));
      glEnableVertexAttribArray(2);
      // Normal
      glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(9 * sizeof(float)));
      glEnableVertexAttribArray(3);
    }

    void setupCylinderDrawing(unsigned int &VAO, unsigned int &VBO, vector<glm::vec3> &points) {

      float vertices[points.size()*3];
      // Convert vector<glm::vec3>to float array
      for (unsigned int i = 0; i < points.size(); i++) {
        vertices[i*3] = points[i].x;
        vertices[i*3+1] = points[i].y;
        vertices[i*3+2] = points[i].z;
        //cout << "P" << i << " (" << points[i].x << ", " << points[i].y << ", " << points[i].z << ")" << endl;
      }
      // For each point, we have 4 vectors: Position, Binormal, Tangent, Normal
      glGenVertexArrays(1, &VAO);
      glGenBuffers(1, &VBO);

      glBindVertexArray(VAO);

      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

      // position attribute
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);
      // Normal
      //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
      //glEnableVertexAttribArray(1);
    }

    glm::vec3 center(glm::vec3 p1, glm::vec3 p2) {
      glm::vec3 p;
      p.x = (p1.x + p2.x)/2;
      p.y = (p1.y + p2.y)/2;
      p.z = (p1.z + p2.z)/2;
      return p;
    }
};
#endif
