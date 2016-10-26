include "mesh.h"

int IS_PAUSED = GLFW_FALSE;
int PAUSE_TIME = 0;

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
  glfwSetFramebufferSizeCallback(window, framebuffer_resize);
  
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);

  while(!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();

    if(!IS_PAUSED || PAUSE_TIME > 0) {
        
      update_pos(A_FLOCK);
      if(glfwGetWindowAttrib(window, GLFW_VISIBLE)){
        draw_mesh();
      }
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

void init(GLFWwindow* window) {
  glClearColor(CLEAR_COLOR[0], CLEAR_COLOR[1], CLEAR_COLOR[2], 1.0);
  glColor3f(0.0, 0.0, 0.0);

  glfwGetWindowSize(window, &WIDTH, &HEIGHT);
  myPerspective(PROJ_MAT, 45, WIDTH*1.0/HEIGHT, CAMERA_NEAR, CAMERA_FAR);
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
      case GLFW_KEY_EQUAL:
      add_a_boid(A_FLOCK);
      break;

      case GLFW_KEY_MINUS:
      remove_a_boid(A_FLOCK);
      break;

      case GLFW_KEY_P:
      IS_PAUSED = GLFW_TRUE;
      PAUSE_TIME++;
      break;

      case GLFW_KEY_R:
      IS_PAUSED = GLFW_FALSE;
      PAUSE_TIME = 0;
      break;

      case GLFW_KEY_V:
      VIEW_MODE  = DEFAULT;
      glfwGetWindowSize(window, &WIDTH, &HEIGHT);
      myPerspective(PROJ_MAT, 45, WIDTH*1.0/HEIGHT, CAMERA_NEAR, CAMERA_FAR);
      break;

      case GLFW_KEY_T:
      VIEW_MODE = TRAILING;
      glfwGetWindowSize(window, &WIDTH, &HEIGHT);
      myPerspective(PROJ_MAT, 30, WIDTH*1.0/HEIGHT, CAMERA_NEAR, CAMERA_FAR);
      break;

      case GLFW_KEY_G:
      VIEW_MODE = SIDE;
      glfwGetWindowSize(window, &WIDTH, &HEIGHT);
      myPerspective(PROJ_MAT, 40, WIDTH*1.0/HEIGHT, CAMERA_NEAR, CAMERA_FAR);
      break;

      case GLFW_KEY_A:
      A_GOAL->MOVE_ALONG_X_NEGATIVE = true;
      break;

      case GLFW_KEY_D:
        A_GOAL->MOVE_ALONG_X_POSITIVE = true;
      break;

      case GLFW_KEY_W:
        A_GOAL->MOVE_ALONG_Y_POSITIVE = true;
      break;

      case GLFW_KEY_S:
        A_GOAL->MOVE_ALONG_Y_NEGATIVE = true;
      break;

      case GLFW_KEY_C:
        INDEX = (INDEX == 0)?1:0;
      break;

      case GLFW_KEY_U:
        if (!GUARDIAN){ A_PREDATOR = create_a_predator(A_FLOCK, A_GOAL, GUARDIAN);}
        else { delete_predator(A_PREDATOR, GUARDIAN);}
      break;

      case GLFW_KEY_RIGHT:
        A_GOAL->ACCELERATE = true;
      break;

      case GLFW_KEY_LEFT:
        A_GOAL->DECELERATE = true;
      break;

      case GLFW_KEY_B:
        teleport_goal(A_GOAL);
      break;

      case GLFW_KEY_Q:
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GLFW_TRUE);
      break;
      default:
      break;
    }
  } else if (action == GLFW_RELEASE) {
    switch(key) {
      case GLFW_KEY_A:
      A_GOAL->MOVE_ALONG_X_NEGATIVE = false;
      break;

      case GLFW_KEY_D:
      A_GOAL->MOVE_ALONG_X_POSITIVE = false;
      break;

      case GLFW_KEY_W:
      A_GOAL->MOVE_ALONG_Y_POSITIVE = false;
      break;

      case GLFW_KEY_S:
      A_GOAL->MOVE_ALONG_Y_NEGATIVE = false;

      case GLFW_KEY_RIGHT:
      A_GOAL->ACCELERATE = false;
      break;

      case GLFW_KEY_LEFT:
      A_GOAL->DECELERATE = false;
      break;

      default:
      break;
    }
  }
}

static GLuint make_bo(GLenum type, const void *buf, GLsizei buf_size) {
  GLuint bufnum;
  glGenBuffers(1, &bufnum);
  glBindBuffer(type, bufnum);
  glBufferData(type, buf_size, buf, GL_STATIC_DRAW);
  return bufnum;
}