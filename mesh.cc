#include "mesh.h"

using namespace std;

int WIDTH, HEIGHT;
int IS_PAUSED = GLFW_TRUE;
int IS_ROTATED = GLFW_FALSE;
int PAUSE_TIME = 0;
int ID = -1;
int TOTAL_MESHES = 0;
GLfloat PARALLEL_SCALE = 1.0;
GLfloat ALL_ROTATE_X = 0.0, ALL_ROTATE_Z = 0.0;
double pmousex, pmousey;
p_mode PROJ_MODE = PERSPECTIVE;
d_mode DRAW_MODE = FACE;
glm::mat4 PROJ_MAT, MV_MAT;
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
  read_all_meshes(FILE_NAMES, MESHES, SHADER);
  init(window);

  glfwMakeContextCurrent(window);
  glfwSetWindowSizeCallback(window, reshape);
  glfwSetKeyCallback(window, keyboard);
  glfwSetMouseButtonCallback(window, mouse);
  glfwSetCursorPosCallback(window, cursor);
  glfwSetFramebufferSizeCallback(window, framebuffer_resize);
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glShadeModel(GL_SMOOTH);

  while(!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();
    for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
      itr_mesh->rotate();

    if(glfwGetWindowAttrib(window, GLFW_VISIBLE)){
      for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
        itr_mesh->draw(SHADER, PROJ_MAT, MV_MAT, THE_LIGHT, DRAW_MODE, SHADING_MODE);
      glfwSwapBuffers(window);
    }

    if(!IS_PAUSED || PAUSE_TIME > 0) {
      change_view(PROJ_MODE, NONE);
      for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
        itr_mesh->update();
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
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glColor3f(0.0, 0.0, 0.0);
  glPointSize(1);
  glLineWidth(0.01);

  change_perspective(window);
  //load_random_texture(MESHES);
  CENTER = glm::vec3(0.7*BLOCK, 0, 0.7*BLOCK);
  EYE = glm::vec3(EYE_X_DISPLACEMENT, EYE_Y_DISPLACEMENT, EYE_Z_DISPLACEMENT);
  UP = glm::vec3(0, 0, 1);
  MV_MAT = glm::lookAt(EYE, CENTER, UP);

  THE_LIGHT.light0 = glm::vec4(LIGHT_X, LIGHT_Y, LIGHT_Z, 0);
  for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
    itr_mesh->compute_light_product(THE_LIGHT);
  glfwGetCursorPos(window, &pmousex, &pmousey);

  for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++) {
    TOTAL_MESHES += itr_mesh->spin.size();
  }
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

      case GLFW_KEY_F:
      SHADING_MODE = FLAT;
      SHADER = flat_shader;
      for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++)
        itr_mesh->bind_flat(SHADER);
      break;

      case GLFW_KEY_S:
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
      change_view(PROJ_MODE, ZOOM_IN);
      }
      break;

      case GLFW_KEY_DOWN: {
      change_view(PROJ_MODE, ZOOM_OUT);
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
  if (action == GLFW_PRESS) {
    IS_ROTATED = GLFW_TRUE;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      ID = -1;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      if (ID < 0)
        ID = 0;
      else {
        ID++;
        ID = ID % TOTAL_MESHES;
      }
    }
  } else if (action == GLFW_RELEASE) {
    IS_ROTATED = GLFW_FALSE;
  }
}

void cursor(GLFWwindow* window, double xpos, double ypos) {
  if (IS_ROTATED == GLFW_TRUE) {
    int count = 0;
    for (auto itr_mesh = MESHES.begin(); itr_mesh != MESHES.end(); itr_mesh++) {
      for (int i = 0; i < itr_mesh->spin.size(); i++) {
        if (ID == -1 || count == ID) {
          itr_mesh->spin[i][2] += ROTATE_STEP_Z*(xpos - pmousex);
          itr_mesh->spin[i][0] += ROTATE_STEP_X*(ypos - pmousey);
        }
        count++;
      }
    }
  }
  pmousex = xpos;
  pmousey = ypos;
}

void change_perspective(GLFWwindow* window) {
  if (PROJ_MODE == PARALLEL) {
    //Don't modify the coefficient here
    PROJ_MAT = glm::ortho(CENTER[0] - 2.3f*BLOCK, CENTER[0] + 0.7f*BLOCK,
                          CENTER[1] - 1.5f*BLOCK, CENTER[1] + 1.5f*BLOCK,
                          CENTER[2] - 1.0f*BLOCK, CENTER[2] + 4.0f*BLOCK);
  } else if (PROJ_MODE == PERSPECTIVE) {
    glfwGetWindowSize(window, &WIDTH, &HEIGHT);
    PROJ_MAT = glm::perspective(45.0f, WIDTH*1.0f/HEIGHT, CAMERA_NEAR, CAMERA_FAR);
  } else {
    cerr << "Invalid projection mode: " << PROJ_MODE << endl;
  }
}

void change_view(p_mode PROJ_MODE, z_direction z) {
  if (PROJ_MODE == PERSPECTIVE) {
    glm::vec3 zoom_step = (EYE - CENTER)*ZOOM_STEP_RATIO;
    if (z == ZOOM_IN) {
      EYE -= zoom_step;
    } else if (z == ZOOM_OUT) {
      EYE += zoom_step;
    }
    glm::mat4 rotation_mat = glm::rotate(ALL_ROTATE_X*DEGREE_TO_RADIAN, glm::vec3(1, 0, 0));
    rotation_mat = glm::rotate(rotation_mat, ALL_ROTATE_Z*DEGREE_TO_RADIAN, glm::vec3(0, 0, 1));
    glm::vec3 rotated_center(glm::vec4(CENTER, 1.0f)*rotation_mat);
    
    MV_MAT = glm::lookAt(EYE, rotated_center, UP);
    MV_MAT = MV_MAT*rotation_mat;
  } else {
    if (z == ZOOM_IN) {
      PARALLEL_SCALE += ZOOM_STEP_RATIO;
    } else if (z == ZOOM_OUT) {
      PARALLEL_SCALE -= ZOOM_STEP_RATIO;
    }
    glm::mat4 rotation_mat = glm::rotate(ALL_ROTATE_X*DEGREE_TO_RADIAN, glm::vec3(1, 0, 0));
    rotation_mat = glm::rotate(rotation_mat, ALL_ROTATE_Z*DEGREE_TO_RADIAN, glm::vec3(0, 0, 1));
    glm::vec3 rotated_center(glm::vec4(CENTER, 1.0f)*rotation_mat);
    
    MV_MAT = glm::lookAt(EYE, rotated_center, UP);
    MV_MAT = glm::scale(MV_MAT, glm::vec3(PARALLEL_SCALE, PARALLEL_SCALE, PARALLEL_SCALE));
    MV_MAT = MV_MAT*rotation_mat;
  }
}

void print() {
  cout << "Eye: (" << EYE[0] << ", " << EYE[1] << ", " << EYE[2] << ")" << endl;
  cout << "Center: (" << CENTER[0] << ", " << CENTER[1] << ", " << CENTER[2] << ")" << endl;
  cout << "Up: (" << UP[0] << ", " << UP[1] << ", " << UP[2] << ")" << endl;
}