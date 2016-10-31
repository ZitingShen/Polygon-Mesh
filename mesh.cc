#include "mesh.h"

using namespace std;

int WIDTH, HEIGHT;
int IS_PAUSED = GLFW_FALSE;
int PAUSE_TIME = 0;
p_mode PROJ_MODE = PERSPECTIVE;
s_mode SHADING_MODE = SMOOTH;
d_mode DRAW_MODE = FACE;
glm::mat4 PROJ_MAT, MV_MAT;
glm::vec3 MIN_XYZ, MAX_XYZ, CURRENT_MIN, CURRENT_MAX;
glm::vec3 LIGHT_POSITION;
vector<string> FILE_NAMES;
vector<MESH> MESHES;
GLfloat[] INTERLEAVED_DATA;

int main(int argc, char *argv[]){
  if (!glfwInit ()) {
    std::cerr << "ERROR: could not start GLFW3" << std::endl;
    exit(EXIT_FAILURE);
  }
  
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  GLFWwindow* window = glfwCreateWindow(500, 500, "Mesh", NULL, NULL);
  if (!window){
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwSetWindowPos(window, 100, 0);
  glfwMakeContextCurrent (window);

  glewExperimental = GL_TRUE;
  glewInit();

  for (int i = 1; i < argc; i++)
    FILE_NAMES.push_back(string(argv[i]));
  read_all_meshes(FILE_NAMES, MESHES, MAX_XYZ, MIN_XYZ);
  init(window);

  glfwMakeContextCurrent(window);
  glfwSetWindowSizeCallback(window, reshape);
  glfwSetKeyCallback(window, keyboard);
  glfwSetMouseButtonCallback(window, mouse);
  glfwSetFramebufferSizeCallback(window, framebuffer_resize);
  
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);

  while(!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();

    if(glfwGetWindowAttrib(window, GLFW_VISIBLE)){
      draw_mesh();
      glfwSwapBuffers(window);
    }

    if(!IS_PAUSED || PAUSE_TIME > 0) {
      //update
      if (IS_PAUSED && PAUSE_TIME > 0) {
        print();
        PAUSE_TIME--;
      }
    }
  }
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

void init(GLFWwindow* window) {
  glClearColor(0, 0, 0, 1.0);
  glColor3f(0.0, 0.0, 0.0);

  load_random_texture();
  CURRENT_MIN = MIN_XYZ;
  CURRENT_MAX = MAX_XYZ;
  LIGHT_POSITION = glm::vec3(MAN_XYZ[0], 
                             MIN_XYZ[1]*2-MAX_XYZ[1], 
                             MAX_XYZ[2]*2-MIN_XYZ[2]);
  change_perspective();
  change_view();

  int start = 0;
  for(auto k = MESHES.begin(); k != MESHES.end(); k++) {
    k->interleave(start);
    start += k->num_v*6;
  }

  bindData();
}

void bindData() {

}

void framebuffer_resize(GLFWwindow* window, int width, int height) {
   glViewport(0, 0, width, height);
}

void reshape(GLFWwindow* window, int w, int h) {
  change_view();
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch(key) {
      case GLFW_KEY_P:
      PROJ_MODE = (p_mode)(1-PROJ_MODE);
      change_perspective(window);
      break;

      case GLFW_KEY_A:
      IS_PAUSED = GLFW_TRUE;
      PAUSE_TIME++;
      break;

      case GLFW_KEY_Z:
      IS_PAUSED = GLFW_FALSE;
      PAUSE_TIME = 0;
      break;

      case GLFW_KEY_E:
      DRAW_MODE = EDGE;
      break;

      case GLFW_KEY_R:
      DRAW_MODE = VERTEX;
      break;

      case GLFW_KEY_T:
      DRAW_MODE = FACE;
      break;

      case GLFW_KEY_S:
      SHADING_MODE = FLAT;
      break;

      case GLFW_KEY_F:
      SHADING_MODE = SMOOTH;
      break;

      case GLFW_KEY_D:
      SHADING_MODE = PHONG;
      break;

      case GLFW_KEY_UP: {
      glm::vec3 diff = (MAX_XYZ - MIN_XYZ)*ZOOM_STEP_RATIO;
      CURRENT_MIN += diff;
      CURRENT_MAX -= diff;
      change_perspective(window);
      }
      break;

      case GLFW_KEY_DOWN: {
      glm::vec3 diff = (MAX_XYZ - MIN_XYZ)*ZOOM_STEP_RATIO;
      CURRENT_MIN -= diff;
      CURRENT_MAX += diff;
      change_perspective(window);
      }
      break;

      case GLFW_KEY_Q:
      case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      break;
      default:
      break;
    }
  }
}

void mouse(GLFWwindow* window, int button, int action, int mods) {
  double mousex, mousey;
  int w, h;
  
  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {

  }
}

static GLuint make_bo(GLenum type, const void *buf, GLsizei buf_size) {
  GLuint bufnum;
  glGenBuffers(1, &bufnum);
  glBindBuffer(type, bufnum);
  glBufferData(type, buf_size, buf, GL_STATIC_DRAW);
  return bufnum;
}

void change_perspective(GLFWwindow* window) {
  if (PROJ_MODE == PARALLEL) {
    PROJ_MAT = glm::ortho(MIN_XYZ[0], MAX_XYZ[0],
                          MIN_XYZ[1], MAX_XYZ[1],
                          MIN_XYZ[2], MAX_XYZ[2]);
  } else if (PROJ_MODE == PERSPECTIVE) {
    glfwGetWindowSize(window, &WIDTH, &HEIGHT);
    PROJ_MAT = glm::perspective(45.0f, WIDTH*1.0f/HEIGHT, CAMERA_NEAR, CAMERA_FAR);
  } else {
    cerr << "Invalid projection mode: " << PROJ_MODE << endl;
  }
}

void change_view() {
  glm::vec3 diff = MAX_XYZ - MIN_XYZ;
  glm::vec3 center = (MAX_XYZ + MIN_XYZ)*0.5f;
  glm::vec3 eye = glm::vec3(center[0] + diff[0]*INITIAL_X_DISPLACEMENT, 
                MIN_XYZ[1] - diff[1]*INITIAL_Y_DISPLACEMENT, MAX_XYZ[2]);
  MV_MAT = glm::lookAt(eye, center, glm::vec3(0, 0, 1));
}

void draw_mesh() {
}

void print() {
  cout << "Current left: " << CURRENT_MIN[0] << endl;
  cout << "Current right: " << CURRENT_MAX[0] << endl;
  cout << "Current near: " << CURRENT_MIN[1] << endl;
  cout << "Current far: " << CURRENT_MAX[1] << endl;
  cout << "Current bottom: " << CURRENT_MIN[2] << endl;
  cout << "Current up: " << CURRENT_MIN[2] << endl;
}