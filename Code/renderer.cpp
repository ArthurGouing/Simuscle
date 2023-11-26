#ifndef RENDER_CPP
#define RENDER_CPP

#include "renderer.h"

using namespace glm;

Renderer::Renderer(Skeleton *skeleton):
  _skel(skeleton), skel_mode("wire"),
  zNear(0.1f), zFar(1500.0), fov(45.0),
  _cameradist(3.0f), _camerapos(vec3(0.0f, 0.0f, -1.0f)), 
  _rotation(mat4(-0.71f, -0.03f,  -0.65f, 0.0f,
                  0.64f, -0.025f, -0.75f, 0.0f,
                  0.02f, -0.99f,   0.04f, 0.0f,
                  0.0f,   0.0f ,   0.0f, 1.0f))
{
}

Renderer::~Renderer()
{
  glDeleteRenderbuffers(1, &rbo);
  glDeleteFramebuffers(1, &framebuffer);
}

void Renderer::Init(int width, int height)
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
  // _skel->alloc_buff(); //TODO:
  Info_Print("set Vert Buff");

  /******** framebuffer configuration ********/
  Info_Print("Create Framebuffer");
  resize_fbo(width, height);

  /******** Load Texture *******/
  Info_Print("Load texture");
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
                                         // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load image, create texture and generate mipmaps
  // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
  unsigned char *data = stbi_load("./mshade3.jpg", &width, &height, &nrChannels, 0); 
  if (data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  // send texture to shader
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 2);
  glUniform1i(glGetUniformLocation(fragmentShader, "MatcapTexture"), 0);

  /******** Cleaning ********/
  Info_Print("Cleaning");
  vertexShaderFile.close();
  fragShaderFile.close();
  // glDeleteShader(vertexShader);
  // glDeleteShader(fragmentShader);
  Info_Print("Done");
}

void Renderer::Draw()
{
    // Framebuffer 
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glEnable(GL_DEPTH_TEST);

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

    glUseProgram(shaderProgram);

    _skel->draw_skeleton_mesh();

    // Draw Skeleton
    // glUseProgram(shaderProgram);
    // if (skel_mode=="stick") {
    //   _skel->draw_skeleton_stick(); // replace set_buff + draw
    // } else if (skel_mode=="mesh") {
    //   _skel->draw_skeleton_mesh();
    // } else if (skel_mode=="wire") {
    //   _skel->draw_skeleton_mesh(true);
    // } else {
    //   Info_Print("Default case: the skeleton won't be draw");
    // }

    // Do the same for the skin and the muscles
 
    // UnBind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // = unbind framebuffer
}

void Renderer::resize_fbo(int width, int height)
{
  // Delete FBO
  if (framebuffer) 
  {
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &textureColorbuffer);
    glDeleteRenderbuffers(1, &rbo);
  }
  // create framebuffer
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  // create a color attachment texture
  glGenTextures(1, &textureColorbuffer);
  glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
  // Create depth

  // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind fbo

  // Say which texture had to go to shader
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 2);
  glUniform1i(glGetUniformLocation(fragmentShader, "MatcapTexture"), 0);
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

void Renderer::UI_pannel()
{
  ImGui::Begin("Render Settings");
  ImGui::End();
}

#endif // !RENDER_CPP
