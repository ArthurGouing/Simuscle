#ifndef RENDER_CPP
#define RENDER_CPP

#include "renderer.h"

using namespace glm;

Renderer::Renderer():
  zNear(0.1f), zFar(1500.0), fov(45.0),
  _cameradist(3.0f), _camerapos(vec3(0.0f, 0.0f, -1.0f)), 
  _rotation(mat4(0.61f, -0.215f, 0.7262f, 0.0f,
                 0.79f, 0.16f,  -0.589,  0.0f, 
                 0.0042f, 0.9632f, 0.268f, 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f))
{
}

Renderer::~Renderer()
{

}

void Renderer::Init()
{
  /******** Vertex shader ********/
  Info_Print("Compile vertex shader");
  std::string vshader_src = "base_shader.vert";
  std::ifstream vertexShaderFile("base_shader.vert");
  std::ostringstream vertexBuffer;
  vertexBuffer << vertexShaderFile.rdbuf();
  std::string vertexBufferStr = vertexBuffer.str();
  // Warning: safe only until vertexBufferStr is destroyed or modified
  const GLchar *source_vert = vertexBufferStr.c_str();

  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &source_vert, 0);
  glCompileShader(vertexShader);
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      Err_Print(infoLog, source_vert);
      exit(1);
  }

  /******** Fragment shader ********/
  Info_Print("Compile fragment shader");
  std::string fshader_src = "base_shader.frag";
  std::ifstream fragShaderFile(fshader_src);
  std::ostringstream fragBuffer;
  fragBuffer << fragShaderFile.rdbuf();
  std::string fragBufferStr = fragBuffer.str();
  // Warning: safe only until vertexBufferStr is destroyed or modified
  const GLchar *source_frag = fragBufferStr.c_str();
 
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &source_frag, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      Err_Print(infoLog, source_frag);
      exit(1);
  }

  /******** Link shader ********/
  Info_Print("Link shader");
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    exit(1);
  }


  // glEnable(GL_CULL_FACE);  
  glEnable(GL_DEPTH_TEST);
  _geom->set_Buffers();
  Info_Print("set buff");

  /******** Load Texture *******/
  Info_Print("Load texture");
  unsigned char *data = stbi_load("./mshade3.jpg", &width, &height, &nrChannels, 0); 
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  /******** Cleaning ********/
  Info_Print("Cleaning");
  vertexShaderFile.close();
  fragShaderFile.close();
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  stbi_image_free(data);
  Info_Print("Done");
}

void Renderer::Draw()
{
    // Background color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

    // Camera transform
    _view = mat4(1.0f);
    _view = glm::translate(_view, vec3(0.0f, 0.0f, -_cameradist));
    _view = _view * _rotation;
    _view = translate(_view, _camerapos);
    
    _vpmat = _projection * _view;

    // Send values to shaders
    int modelLoc = glGetUniformLocation(shaderProgram, "vp_mat");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(_vpmat));
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(_view));
    // vec3 campos = _camerapos - vec3(_rotation * vec4(vec3(0.0f, 0.0f, -_cameradist), 0.0f));
    // int camposLoc = glGetUniformLocation(shaderProgram, "campos");
    // glUniform3fv(camposLoc, 1, glm::value_ptr(campos));

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind

    _geom->set_Buffers();
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glUseProgram(shaderProgram);
    glBindVertexArray(_geom->VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    // if (is_wireframe) {
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
    // } else {
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL).
    // }
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
    glDrawElements(GL_TRIANGLES, 3 * _geom->n_faces, GL_UNSIGNED_INT, 0);
}

void Renderer::add_geom(Geometry *new_geom)
{
  _geom = new_geom;
  Info_Print("n_vert of square in renderer: "+std::to_string(_geom->n_verts));
}

void Renderer::update_rotation(vec2 mouse_offset, float angle)
{
  // Add a rotation of the angle rotationAngle from the axis rotationAxis to rotation, the transformation matrix
  vec3 rotationAxis = transpose(_rotation) * vec4(-mouse_offset[1], mouse_offset[0], 0.0f, 1.0f); 
  _rotation = glm::rotate(_rotation, glm::radians(angle), rotationAxis);
}

void Renderer::update_camerapos(vec2 mouse_offset)
{
  // Add a translation to the camera position from the mouse offset input.
  _camerapos += _cameradist * vec3(transpose(_rotation) * vec4(mouse_offset[0], mouse_offset[1], 0.0f, 1.0f));
  // TODO: Optimize la transposition de la rotation et les cast inutiles...
}

void Renderer::update_cameradist(float dist)
{
  // Zoom the camera according to the scroll values
  _cameradist += dist;
  // TODO : CHoisir le signe en fonction du rapprochement avec le centre de l'écran(comme Blender)
}

void Renderer::update_projection(float aspect)
{
  _projection = perspective(radians(fov), aspect, zNear, zFar);
}

void Renderer::reset_view()
{
  _rotation = mat4(0.61f, -0.215f, 0.7262f, 0.0f,
      0.79f, 0.16f,  -0.589,  0.0f, 
      0.0042f, 0.9632f, 0.268f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  _camerapos = vec3(0.0f, 0.0f, -1.0f);
  _cameradist = 3.0f;
}

#endif // !RENDER_CPP
