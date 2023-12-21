#ifndef LINE_CPP
#define LINE_CPP

#include "line.h"

using namespace glm;

Line::Line(vec3 pos, vec3 vector)
{
  lineColor = vec3(1,0,0);

  const char *vertexShaderSource = "#version 330 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "uniform mat4 MVP;\n"
      "void main()\n"
      "{\n"
      "   gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
      "}\0";
  const char *fragmentShaderSource = "#version 330 core\n"
      "out vec4 FragColor;\n"
      "uniform vec3 color;\n"
      "void main()\n"
      "{\n"
      "   FragColor = vec4(color, 1.0f);\n"
      "}\n\0";
 
  // vertex shader
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  // check for shader compile errors
 
  // fragment shader
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
 
  // link shaders
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
 
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
 
  vertices = {
       pos.x, pos.y, pos.z,
       pos.x + vector.x, pos.y + vector.y, pos.z + vector.z
  };
 
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
 
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
 
  glBindBuffer(GL_ARRAY_BUFFER, 0); 
  glBindVertexArray(0); 
}

int Line::draw(glm::mat4 VPMat)
{
  glUseProgram(shaderProgram);
 

  int modelLoc = glGetUniformLocation(shaderProgram, "MVP");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(VPMat));
  // glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
  glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &lineColor[0]);
 
  glBindVertexArray(VAO);
  glDrawArrays(GL_LINES, 0, 2);
  return 1;
}

#endif // !LINE_CPP
