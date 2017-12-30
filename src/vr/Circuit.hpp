#ifndef CIRCUIT_H
#define CIRCUIT_H
// Inspired from http://devmag.org.za/2011/04/05/bzier-curves-a-tutorial/ and https://gist.github.com/epatel/1107754
// Continuous bezier curve http://www.algosome.com/articles/continuous-bezier-curve-line.html
#define SEGMENTS_PER_CURVE 30
#include <vector>
#include <Shader.hpp>


struct point {
  float x;
  float y;
  float z;
};

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
  void setup(point p0, point p1, point p2, point p3);
  point calc(float t);
};

void bezier::setup(point p0, point p1, point p2, point p3) {
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

point bezier::calc(float t) {
  point p;
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
    vector<point> controls; // temp control points
    for (int i = 0; i < num_points; i++) {
      point p;
      p.x = points[i*3];
      p.y = points[i*3+1];
      p.z = points[i*3+2];
      controls.push_back(p);
    }

    // Need to add control points to ensure the gradient is the same.
    this->controlPoints.push_back(controls[0]);
    for (unsigned int i = 1; i < controls.size() - 2; i+=2 ){
      this->controlPoints.push_back(controls[i]);
      this->controlPoints.push_back(controls[i+1]);
  		this->controlPoints.push_back(center(controls[i+1], controls[i+2]));
  	}
    this->controlPoints.push_back(controls[controls.size()-1]);
    point p; // point to connect the start and the end of the loop with the same gradient
    point start = controls[0];
    point second = controls[1];
    p.x = 2*start.x - second.x;
    p.y = 2*start.y - second.y;
    p.z = 2*start.z - second.z;
    this->controlPoints.push_back(p);
    this->controlPoints.push_back(start);
    cout << "Control points (" << this->controlPoints.size() << ") :" << endl << flush;
    for (unsigned int i = 0; i < this->controlPoints.size(); i++) {
      cout << "P" << i << " (" << this->controlPoints[i].x << ", " << this->controlPoints[i].y << ", " << this->controlPoints[i].z << ")" << endl;
    }

    this->setup();

  }

  void Draw() {
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_POINTS, 0, this->num_points);
  }


  private:
    vector<point> controlPoints;
    unsigned int VBO, VAO;
    unsigned int num_points;

    point center(point p1, point p2) {
      point p;
      p.x = (p1.x + p2.x)/2;
      p.y = (p1.y + p2.y)/2;
      p.z = (p1.z + p2.z)/2;
      return p;
    }

    // Compute points for the Bézier curves
    vector<point> getPoints()
    {
      vector<point> drawingPoints;
      for(unsigned int i = 0; i < this->controlPoints.size() - 3; i+=3) {
        point p0 = this->controlPoints[i];
        point p1 = this->controlPoints[i + 1];
        point p2 = this->controlPoints[i + 2];
        point p3 = this->controlPoints[i + 3];

        bezier b;
        b.setup(p0, p1, p2, p3);

        if(i == 0) //Only do this for the first endpoint.
                   //When i != 0, this coincides with the end
                   //point of the previous segment
        {
          drawingPoints.push_back(b.calc(0));
        }

        for(int j = 1; j <= SEGMENTS_PER_CURVE; j++)
        {
          float t = j / (float) SEGMENTS_PER_CURVE;
          point p = b.calc(t);
          addBTN(drawingPoints, p, drawingPoints[drawingPoints.size()-1]);
          drawingPoints.push_back(p);
        }
      }
      // this line adds NANs ??? :
      addBTN(drawingPoints, drawingPoints[drawingPoints.size()-2], drawingPoints[0]); // BTN for the last point : connect end with start
      this->num_points = (int) drawingPoints.size()/4;
      cout << "num_points" << this->num_points << endl;
      return drawingPoints;
    }

    void addBTN(vector<point> &points, point p_current, point p_previous) {

      glm::vec3 current = pointToVec3(p_current);
      glm::vec3 previous = pointToVec3(p_previous);
      //cout << "current" << " (" << current.x << ", " << current.y << ", " << current.z << ")" << endl;
      //cout << "previous" << " (" << previous.x << ", " << previous.y << ", " << previous.z << ")" << endl;
      glm::vec3 T = normalize(current - previous); // Tangent
      glm::vec3 B = normalize(cross(T, current + previous)); // Binormal
      glm::vec3 N = normalize(cross(B,T));  // Normal
      points.push_back(vec3ToPoint(B));
      points.push_back(vec3ToPoint(T));
      points.push_back(vec3ToPoint(N));

    }

    void setup() {
      vector<point> points = this->getPoints();
      float vertices[points.size()*3];
      // Convert vector<point> to float array
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

    glm::vec3 pointToVec3(point p) {
      return glm::vec3(p.x, p.y, p.z);
    }

    point vec3ToPoint(glm::vec3 v) {
      point p;
      p.x = v[0];
      p.y = v[1];
      p.z = v[2];
      return p;
    }
};
#endif
