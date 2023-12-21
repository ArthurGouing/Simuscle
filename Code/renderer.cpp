#ifndef RENDER_CPP
#define RENDER_CPP

#include "renderer.h"

using namespace glm;

static glm::vec3 color = vec3(0.f, 0.f, 1.f);
static float intensity = 0.5f;

glm::mat4 Renderer::_vpmat;

Renderer::Renderer(Skeleton *skeleton, MuscleSystem *muscles):
  _skel(skeleton), _musclesys(muscles), skel_mode("mesh"), muscle_mode("mesh"),
  zNear(0.01f), zFar(1500.0), fov(45.0),
  _cameradist(3.0f), _camerapos(vec3(0.0f, 0.0f, -1.0f)), 
  _theta(90 * 6.2832/360), _mymatcap(true), is_show_curves(true),
  last_frame(0),
  _rotation(mat4(-0.71f, -0.03f,  -0.65f, 0.0f,
                  0.64f, -0.025f, -0.75f, 0.0f,
                  0.02f, -0.99f,   0.04f, 0.0f,
                  0.0f,   0.0f ,   0.0f, 1.0f))
{
  _skel->set_mode("mesh");
  _musclesys->set_mode("mesh");
}

Renderer::~Renderer()
{
  glDeleteTextures(1, &textureColorbuffer);
  glDeleteTextures(1, &texture_matcap);
  glDeleteTextures(1, &texture_muscle_matcap);
  glDeleteRenderbuffers(1, &rbo);
  glDeleteFramebuffers(1, &framebuffer);
  delete _skel;
  delete _musclesys;
}

void Renderer::Init(int width, int height)
{
  /******** Compile shaders ********/
  Info_Print("Load shaders");
  // Load Shader for the bones and skin
  load_shader("skin_shader.frag", "skin_shader.vert", skin_fragmentShader, skin_vertexShader, skin_shaderProgram);

  // Load Shader for muscles
  // load_shader("muscle_shader.frag", "muscle_shader.vert", muscle_fragmentShader, muscle_vertexShader, muscle_shaderProgram);

  // glEnable(GL_CULL_FACE);  
  glEnable(GL_DEPTH_TEST);

  /******** init geometry buffers ********/
  Info_Print("set Vertex Object Buffers");
  _skel->init_buffers();
  _musclesys->init_geom_buffers();
  _musclesys->init_crv_buffers();

  /******** framebuffer configuration ********/
  Info_Print("Create Framebuffer");
  resize_fbo(width, height);

  /******** Load Texture *******/
  Info_Print("Load Texture : ''./light_matcap.jpg'");
  load_texture("./white_matcap_2.jpg", &texture_matcap);
  Info_Print("Load Texture : ''./red_matcap.png'");
  load_texture("./red_matcap.png", &texture_muscle_matcap);
  // send texture to shader
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_matcap);
  glUniform1i(glGetUniformLocation(muscle_fragmentShader, "MatcapTexture"), 0);
  // glActiveTexture(GL_TEXTURE1);
  // glBindTexture(GL_TEXTURE_2D, texture_muscle_matcap);
  // glUniform1i(glGetUniformLocation(muscle_fragmentShader, "MatcapTextureRed"), 1);

  Info_Print("Done");
}

void Renderer::load_shader(std::string fshader_src, std::string vshader_src, 
    unsigned int& fragmentShader, unsigned int& vertexShader,  unsigned int& shaderProgram)
{
  /******** Vertex shader ********/
  std::ifstream vertexShaderFile(vshader_src);
  std::ostringstream vertexBuffer;
  vertexBuffer << vertexShaderFile.rdbuf();
  std::string vertexBufferStr = vertexBuffer.str();
  // Warning: safe only until vertexBufferStr is destroyed or modified
  const GLchar *source_vert = vertexBufferStr.c_str();

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
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
  std::ifstream fragShaderFile(fshader_src);
  std::ostringstream fragBuffer;
  fragBuffer << fragShaderFile.rdbuf();
  std::string fragBufferStr = fragBuffer.str();
  // Warning: safe only until vertexBufferStr is destroyed or modified
  const GLchar *source_frag = fragBufferStr.c_str();
 
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
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

  /******** Cleaning ********/
  vertexShaderFile.close();
  fragShaderFile.close();
}

void Renderer::load_texture(std::string texture_path, unsigned int* textureid)
{
  glGenTextures(1, textureid);
  glBindTexture(GL_TEXTURE_2D, *textureid); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // load image, create texture and generate mipmaps
  // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
  char* texture_char = new char[texture_path.length()+1];
  strcpy(texture_char, texture_path.c_str());
  std::cout << texture_char << std::endl;
  unsigned char *data = stbi_load(texture_char, &width, &height, &nrChannels, 0); 
  if (data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::cout << "Failed to load texture" << std::endl;
  }

  /******** Cleaning ********/
  stbi_image_free(data);
}


void Renderer::Draw(int frame)
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
    int modelLoc = glGetUniformLocation(skin_shaderProgram, "vp_mat");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(_vpmat));
    int viewLoc = glGetUniformLocation(skin_shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(_view));
    int thetaLoc = glGetUniformLocation(skin_shaderProgram, "theta"); 
    // Info_Print(std::to_string(_theta));
    glUniform1f(thetaLoc, _theta);
    // int matcapLoc = glGetUniformLocation(skin_shaderProgram, "my_matcap");
    // glUniform1i(matcapLoc, _mymatcap);

    Line test_line(vec3(-2,0,0.f), vec3(1.0, 0.f, 0.f));
    int n_line = 10;
    for (int i = -n_line; i <= n_line; i++) {
      test_line.setColor(intensity*vec3(1,1,1));
      test_line.setVector(vec3(i,-n_line,0), vec3(0.f, 2*n_line, 0.f));
      test_line.draw(_vpmat);
      test_line.setVector(vec3(-n_line,i,0), vec3(2*n_line, 0.f, 0.f));
      test_line.draw(_vpmat);
    }

    test_line.setVector(vec3(-2,0,0.2f), vec3(1.f, 0.f, 0.f));
    test_line.setColor(vec3(1,0,0));
    test_line.draw(_vpmat);
    test_line.setVector(vec3(-2,0,0.2f), vec3(0.f, 1.f, 0.f));
    test_line.setColor(vec3(0.f, 1.f, 0.f));
    test_line.draw(_vpmat);
    test_line.setVector(vec3(-2,0,0.2f), vec3(0.f, 0.f, 1.f));
    test_line.setColor(color);
    test_line.draw(_vpmat);

    glUseProgram(skin_shaderProgram);
    // Vec3_Draw(vec3(0.f), vec3(0.f, 0.f, 1.f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_matcap);
    glUniform1i(glGetUniformLocation(muscle_fragmentShader, "MatcapTexture"), 0);
    // if (frame!=last_frame) // avoid sending VBO to GPU if the points haven't changed
    //_skel->update_buffers(frame);
    _skel->draw_skeleton_mesh();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_muscle_matcap);
    glUniform1i(glGetUniformLocation(muscle_fragmentShader, "MatcapTextureRed"), 0);

    _musclesys->update_curve_buffers(frame); // if is simulating 
    if (frame!=last_frame) // avoid sending VBO to GPU if the points haven't changed
    _musclesys->update_geom_buffers(frame);
    _musclesys->draw_muscles();
    if (is_show_curves) {
      // glUseProgram(line_shaderProgram); //TODO make the shader and change the VBO update, and values format for curves
      _musclesys->draw_curves(); // Il faut faire un shader special
    }

    // UnBind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // = unbind framebuffer
    // delete &modelLoc, &viewLoc, &thetaLoc;
    last_frame = frame;
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
  glBindTexture(GL_TEXTURE_2D, texture_matcap);
  glUniform1i(glGetUniformLocation(skin_fragmentShader, "MatcapTexture"), 0);
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

void Renderer::Vec3_Draw(glm::vec3 pos, glm::vec3 vector)
{
  unsigned int VAO, VBO;
  std::vector<float> vertices = {
       pos.x, pos.y, pos.z,
       pos.x + vector.x , pos.y + vector.y, pos.z + vector.z,
  };
  
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
 
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
 
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
 
  glBindBuffer(GL_ARRAY_BUFFER, 0); 
  glBindVertexArray(0); 
 
  glBindVertexArray(VAO);
  glDisable(GL_DEPTH_TEST);
  glDrawArrays(GL_LINES, 0, 2);
  glEnable(GL_DEPTH_TEST);
}

void Renderer::UI_pannel()
{
  ImGui::Begin("Render Settings");
  ImGui::SliderFloat("Light angle", &_theta, -2.f, 2.f);            // Edit 1 float using a slider from 0.0f to 1.0f
  ImGui::Checkbox("Ficed matcap", &_mymatcap);
  ImGui::Checkbox("Show curves", &is_show_curves);
  ImGui::SliderFloat("r", &color.x, 0, 1);
  ImGui::SliderFloat("g", &color.y, 0, 1);
  ImGui::SliderFloat("b", &color.z, 0, 1);
  ImGui::SliderFloat("Intensity", &intensity, 0, 1);
  ImGui::End();
}

#endif // !RENDER_CPP
