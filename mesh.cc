include "mesh.h"

using namespace std;

int WIDTH, HEIGHT;
int IS_PAUSED = GLFW_FALSE;
int PAUSE_TIME = 0;
p_mode PROJ_MODE = PERSPECTIVE;
s_mode SHADING_MODE = SMOOTH;
d_mode DRAW_MODE = FACE;
glm::mat4 PROJ_MAT, MV_MAT;
glm::vec3 MIN_XYZ, MAX_XYZ;
GLfloat zoom_step;
vector<MESH> MESHES;

int main(){
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
    }

    if(!IS_PAUSED || PAUSE_TIME > 0) { 
      update_pos(A_FLOCK);
      
      glfwSwapBuffers(window);
      if (IS_PAUSED && PAUSE_TIME > 0) {
        //print_mesh();
        PAUSE_TIME--;
      }
    }
  }
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

void init() {
  glClearColor(CLEAR_COLOR[0], CLEAR_COLOR[1], CLEAR_COLOR[2], 1.0);
  glColor3f(0.0, 0.0, 0.0);

  change_perspective();
  change_view();
}

void framebuffer_resize(GLFWwindow* window, int width, int height) {
   glViewport(0, 0, width, height);
}

void reshape(GLFWwindow* window, int w, int h) {
  change_view(MV_MAT, VIEW_MODE, A_FLOCK, A_GOAL, INDEX);
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch(key) {
      case GLFW_KEY_P:
      PROJ_MODE = 1 - PROJ_MODE;
      change_perspective();
      break;

      case GLFW_KEY_A:
      IS_PAUSED = GLFW_TRUE;
      PAUSE_TIME++;
      break;

      case GLFW_KEY_Q:
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

      case GLFW_KEY_UP:
      glm::vec3 diff = (MAX_XYZ - MIN_XYZ)*ZOOM_STEP_RATIO;
      MIN_XYZ += diff;
      MAX_XYZ -= diff;
      change_perspective();
      break;

      case GLFW_KEY_DOWN:
      glm::vec3 diff = (MAX_XYZ - MIN_XYZ)*ZOOM_STEP_RATIO;
      MIN_XYZ -= diff;
      MAX_XYZ += diff;
      change_perspective();
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

void change_perspective() {
  if (PROJ_MODE == PARALLEL) {
    PROJ_MAT = glm::ortho(MIN_XYZ[0], MAX_XYZ[0],
                          MIN_XYZ[1], MAX_XYZ[1],
                          MIN_XYZ[2], MAX_XYZ[2]);
  } else if (PROJ_MODE == PERSPECTIVE) {
    glfwGetWindowSize(window, &WIDTH, &HEIGHT);
    PROJ_MAT = glm::perspective(45, WIDTH*1.0/HEIGHT, CAMERA_NEAR, CAMERA_FAR);
  } else {
    cerr << "Invalid projection mode: " << PROJ_MODE << endl;
  }
}

void change_view() {
  glm::vec3 diff = MAX_XYZ - MIN_XYZ;
  glm::vec3 center = (MAX_XYZ + MIN_XYZ)*0.5;
  glm::vec3 eye(center[0] + diff*INITIAL_X_DISPLACEMENT, 
                MIN_XYZ[1] - diff*INITIAL_X_DISPLACEMENT, MAX_XYZ[2]);
  MV_MAT = glm::lookAt(eye, center, glm::vec3(0, 0, 1));
}