#include "mesh.h"

using namespace std;

int WIDTH, HEIGHT;
int IS_PAUSED = GLFW_TRUE;
int PAUSE_TIME = 0;
p_mode PROJ_MODE = PERSPECTIVE;
d_mode DRAW_MODE = FACE;
glm::mat4 PROJ_MAT, MV_MAT;
glm::vec3 MIN_XYZ, MAX_XYZ, CURRENT_MIN, CURRENT_MAX;
LIGHT THE_LIGHT;
map<string, int> FILE_NAMES;
vector<MESH> MESHES;
GLuint SHADER, flat_shader, gouraud_shader, phong_shader;
s_mode SHADING_MODE;
glm::vec3 CENTER, EYE, UP;

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

  for (int i = 1; i < argc; i++) {
    string new_file(argv[i]);
    if (FILE_NAMES.find(new_file) == FILE_NAMES.end())
      FILE_NAMES[new_file] = 1;
    else
      FILE_NAMES[new_file]++;
  }
  
  gouraud_shader = initshader("gouraud_vs.glsl", "gouraud_fs.glsl");
  flat_shader = gouraud_shader;
  phong_shader = initshader("phong_vs.glsl", "phong_fs.glsl");
  SHADER = gouraud_shader;
  SHADING_MODE = SMOOTH;
  MESHES.resize(FILE_NAMES.size());
  read_all_meshes(FILE_NAMES, MESHES, MAX_XYZ, MIN_XYZ, SHADER);
  init(window);

  glfwMakeContextCurrent(window);
  glfwSetWindowSizeCallback(window, reshape);
  glfwSetKeyCallback(window, keyboard);
  glfwSetMouseButtonCallback(window, mouse);
  glfwSetFramebufferSizeCallback(window, framebuffer_resize);
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glShadeModel(GL_SMOOTH);

  while(!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();

    if(glfwGetWindowAttrib(window, GLFW_VISIBLE)){
      for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
        itr_mesh->draw(SHADER, PROJ_MAT, MV_MAT, THE_LIGHT, DRAW_MODE, SHADING_MODE);
      glfwSwapBuffers(window);
    }

    if(!IS_PAUSED || PAUSE_TIME > 0) {
      for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
        itr_mesh->rotate();
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
  glClearColor(0, 1.0, 1.0, 1.0);
  glColor3f(0.0, 0.0, 0.0);
  glPointSize(1);
  glLineWidth(0.01);

  change_perspective(window);
  //load_random_texture(MESHES);
  //glm::vec3 diff = MAX_XYZ - MIN_XYZ;
  //CENTER = (MAX_XYZ + MIN_XYZ)*0.5f;
  //EYE = glm::vec3(CENTER[0] + diff[0]*INITIAL_X_DISPLACEMENT, 
  //                CENTER[1] + diff[1]*INITIAL_Y_DISPLACEMENT, 
  //                CENTER[2] + diff[2]*INITIAL_Z_DISPLACEMENT);
  CENTER = glm::vec3(0.7*BLOCK_X, 0, 0.7BLOCK_Z);
  EYE = glm::vec3(EYE_X_DISPLACEMENT, EYE_Y_DISPLACEMENT, EYE_Z_DISPLACEMENT);
  UP = glm::vec3(0, 0, 1);
  MV_MAT = glm::lookAt(EYE, CENTER, UP);

  THE_LIGHT.light0 = glm::vec4(LIGHT_X, LIGHT_Y, LIGHT_Z, 0);
  for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
    itr_mesh->compute_light_product(THE_LIGHT);
  
}

void framebuffer_resize(GLFWwindow* window, int width, int height) {
   glViewport(0, 0, width, height);
}

void reshape(GLFWwindow* window, int w, int h) {
  MV_MAT = glm::lookAt(EYE, CENTER, UP);
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch(key) {
      case GLFW_KEY_P:
      PROJ_MODE = (p_mode)(1-PROJ_MODE);
      change_perspective(window);
      break;

      case GLFW_KEY_A:
      IS_PAUSED = 1 - IS_PAUSED;
      if (IS_PAUSED)
        PAUSE_TIME++;
      else
        PAUSE_TIME = 0;
      break;

      case GLFW_KEY_E:
      DRAW_MODE = EDGE;
      break;

      case GLFW_KEY_R:
      DRAW_MODE = POINT;
      break;

      case GLFW_KEY_T:
      DRAW_MODE = FACE;
      break;

      case GLFW_KEY_S:
      SHADING_MODE = FLAT;
      SHADER = flat_shader;
      for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
        itr_mesh->bind_flat(SHADER);
      break;

      case GLFW_KEY_F:
      SHADING_MODE = SMOOTH;
      SHADER = gouraud_shader;
      for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
        itr_mesh->bind_other(SHADER);
      break;

      case GLFW_KEY_D:
      SHADING_MODE = PHONG;
      SHADER = phong_shader;
      for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
        itr_mesh->bind_other(SHADER);
      break;

      case GLFW_KEY_UP: {
      change_view(ZOOM_IN);
      }
      break;

      case GLFW_KEY_DOWN: {
      change_view(ZOOM_OUT);
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

void change_perspective(GLFWwindow* window) {
  if (PROJ_MODE == PARALLEL) {
    //PROJ_MAT = glm::ortho (-10.f, 10.f, -20.f, 20.f, -20.f, 20.f);
    PROJ_MAT = glm::ortho(CENTER[0] - 2*BLOCK, CENTER[0] + BLOCK,
                          CENTER[1] - 2*BLOCK, CENTER[1] + 2*BLOCK,
                          CENTER[2] - 1*BLOCK, CENTER[2] + 2*BLOCK);
  } else if (PROJ_MODE == PERSPECTIVE) {
    glfwGetWindowSize(window, &WIDTH, &HEIGHT);
    PROJ_MAT = glm::perspective(45.0f, WIDTH*1.0f/HEIGHT, CAMERA_NEAR, CAMERA_FAR);
  } else {
    cerr << "Invalid projection mode: " << PROJ_MODE << endl;
  }
}

void change_view(z_direction z) {
  glm::vec3 zoom_step = (EYE - CENTER)*ZOOM_STEP_RATIO;
  if (z == ZOOM_IN) {
    EYE -= zoom_step;
  } else {
    EYE += zoom_step;
  }
  MV_MAT = glm::lookAt(EYE, CENTER, UP);
}

void print() {
  cout << "Current left: " << CURRENT_MIN[0] << endl;
  cout << "Current right: " << CURRENT_MAX[0] << endl;
  cout << "Current near: " << CURRENT_MIN[1] << endl;
  cout << "Current far: " << CURRENT_MAX[1] << endl;
  cout << "Current bottom: " << CURRENT_MIN[2] << endl;
  cout << "Current up: " << CURRENT_MIN[2] << endl;
  cout << "Eye: (" << EYE[0] << ", " << EYE[1] << ", " << EYE[2] << ")" << endl;
  cout << "Center: (" << CENTER[0] << ", " << CENTER[1] << ", " << CENTER[2] << ")" << endl;
  cout << "Up: (" << UP[0] << ", " << UP[1] << ", " << UP[2] << ")" << endl;
}