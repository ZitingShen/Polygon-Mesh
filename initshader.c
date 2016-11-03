#include "initshader.h"

// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile) {
  FILE* fp = fopen(shaderFile, "r");

  if ( fp == NULL ) { return NULL; }

  fseek(fp, 0L, SEEK_END);
  long size = ftell(fp);

  fseek(fp, 0L, SEEK_SET);
  char* buf = (char *) malloc(size + 1);
  fread(buf, 1, size, fp);

  buf[size] = '\0';
  fclose(fp);

  return buf;
}


// Create a GLSL program object from vertex and fragment shader files
GLuint initshader(const char* vShaderFile, const char* fShaderFile){
  struct Shader {
    const char*  filename;
    GLenum       type;
    GLchar*      source;
  }  shaders[2] = {
    {vShaderFile, GL_VERTEX_SHADER, NULL},
    {fShaderFile, GL_FRAGMENT_SHADER, NULL}
  };

  GLuint program = glCreateProgram();
  int i;
  struct Shader *s;
  GLint  shader, compiled, linked, logSize;
  char   *logMsg;


  for (i = 0; i < 2; ++i) {
    s = &shaders[i];
    s->source = readShaderSource(s->filename);
    if (shaders[i].source == NULL) {
      fprintf(stderr, "Failed to read %s\n", s->filename);
      exit(EXIT_FAILURE);
    }

    shader = glCreateShader(s->type);
    glShaderSource(shader, 1, (const GLchar**) &s->source, NULL);
    glCompileShader(shader );

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
      fprintf(stderr, "%s failed to compile\n", s->filename);
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
      logMsg = (char *) malloc(logSize);
      glGetShaderInfoLog(shader, logSize, NULL, logMsg);
      fprintf(stderr, "%s\n", logMsg);
      free(logMsg);

      exit( EXIT_FAILURE );
    }

    free(s->source);

    glAttachShader(program, shader);
  }

  return program;
}
