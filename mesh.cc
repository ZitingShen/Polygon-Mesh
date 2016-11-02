#include "mesh.h"

using namespace std;

int WIDTH, HEIGHT;
int IS_PAUSED = GLFW_FALSE;
int PAUSE_TIME = 0;
p_mode PROJ_MODE = PERSPECTIVE;
d_mode DRAW_MODE = FACE;
glm::mat4 PROJ_MAT, MV_MAT;
glm::vec3 MIN_XYZ, MAX_XYZ, CURRENT_MIN, CURRENT_MAX;
LIGHT THE_LIGHT;
map<string, int> FILE_NAMES;
vector<MESH> MESHES;
GLuint SHADER, flat_shader, gouraud_shader, phong_shader;
glm::vec3 center, eye, up;

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
    if (FILE_NAMES.find(new_file) != FILE_NAMES.end())
      FILE_NAMES[new_file] = 1;
    else
      FILE_NAMES[new_file]++;
  }
  
  //flat_shader = initshader("flat_vs.glsl", "flat_fs.glsl");
  gouraud_shader = initshader("gouraud_vs.glsl", "gouraud_fs.glsl");
  phong_shader = initshader("phong_vs.glsl", "phong_fs.glsl");
  SHADER = gouraud_shader;
  MESHES.reserve(FILE_NAMES.size());
  MESHES.resize(FILE_NAMES.size());
  read_all_meshes(FILE_NAMES, MESHES, MAX_XYZ, MIN_XYZ, SHADER);
  
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
      for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
        itr_mesh->draw(SHADER, PROJ_MAT, MV_MAT, THE_LIGHT, DRAW_MODE);
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

  //load_random_texture(MESHES);
  CURRENT_MIN = MIN_XYZ;
  CURRENT_MAX = MAX_XYZ;
  THE_LIGHT.light0 = glm::vec4(MAX_XYZ[0], 
                               MIN_XYZ[1]*2-MAX_XYZ[1], 
                               MAX_XYZ[2]*5-MIN_XYZ[2],
                               0);
  //THE_LIGHT.light0 = glm::vec4(10, 10, 10, 0);
  for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
    itr_mesh->compute_light_product(THE_LIGHT);
  change_perspective(window);
  change_view();
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

      case GLFW_KEY_E: //TODO
      DRAW_MODE = EDGE;
      //for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
      //  itr_mesh->bind_edge(SHADER);
      break;

      case GLFW_KEY_R:
      DRAW_MODE = POINT;
      break;

      case GLFW_KEY_T:
      DRAW_MODE = FACE;
      break;

      case GLFW_KEY_S:
      SHADER = flat_shader;
      for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
        itr_mesh->bind_flat(SHADER);
      break;

      case GLFW_KEY_F:
      SHADER = gouraud_shader;
      for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
        itr_mesh->bind_other(SHADER);
      break;

      case GLFW_KEY_D:
      SHADER = phong_shader;
      for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
        itr_mesh->bind_other(SHADER);
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

void change_perspective(GLFWwindow* window) {
  if (PROJ_MODE == PARALLEL) {
    //PROJ_MAT = glm::ortho (-10.f, 10.f, -20.f, 20.f, -20.f, 20.f);
    PROJ_MAT = glm::ortho(MIN_XYZ[0]-2, MAX_XYZ[0]+2,
                          MIN_XYZ[1]-2, MAX_XYZ[1]+2,
                          MIN_XYZ[2]-2, MAX_XYZ[2]+2);
    cout << glm::to_string(PROJ_MAT) << endl;
  } else if (PROJ_MODE == PERSPECTIVE) {
    glfwGetWindowSize(window, &WIDTH, &HEIGHT);
    PROJ_MAT = glm::perspective(45.0f, WIDTH*1.0f/HEIGHT, CAMERA_NEAR, CAMERA_FAR);
  } else {
    cerr << "Invalid projection mode: " << PROJ_MODE << endl;
  }
}

void change_view() {
  glm::vec3 diff = MAX_XYZ - MIN_XYZ;
  center = (MAX_XYZ + MIN_XYZ)*0.5f;
  eye = glm::vec3(center[0] + diff[0]*INITIAL_X_DISPLACEMENT, 
                  MIN_XYZ[1] - diff[1]*INITIAL_Y_DISPLACEMENT, 3*MAX_XYZ[2]);
  up = glm::vec3(0, 0, 1);
  MV_MAT = glm::lookAt(eye, center, up);
}

void print() {
  cout << "Current left: " << CURRENT_MIN[0] << endl;
  cout << "Current right: " << CURRENT_MAX[0] << endl;
  cout << "Current near: " << CURRENT_MIN[1] << endl;
  cout << "Current far: " << CURRENT_MAX[1] << endl;
  cout << "Current bottom: " << CURRENT_MIN[2] << endl;
  cout << "Current up: " << CURRENT_MIN[2] << endl;
  cout << "Eye: (" << eye[0] << ", " << eye[1] << ", " << eye[2] << ")" << endl;
  cout << "Center: (" << center[0] << ", " << center[1] << ", " << center[2] << ")" << endl;
  cout << "Up: (" << up[0] << ", " << up[1] << ", " << up[2] << ")" << endl;
}