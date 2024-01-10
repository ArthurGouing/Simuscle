#ifndef RENDER_MANAGER_CPP
#define RENDER_MANAGER_CPP

#include "render_manager.h"

using namespace glm;

RenderManager::RenderManager():
  _clear_color(0.2f, 0.3f, 0.3f, 1.0f),
  _zNear(0.0001f), _zFar(1500.0), _fov(45.0), _projection(mat4(1.f)),
  _camerapos(vec3(0.0f, 0.0f, -1.0f)), _cameradist(3.0f),
  _r0(mat4(-0.71f, -0.03f,  -0.65f, 0.0f,
            0.64f, -0.025f, -0.75f, 0.0f,
            0.02f, -0.99f,   0.04f, 0.0f,
            0.0f,   0.0f ,   0.0f, 1.0f)),
  _del_fbo(false)
{
  _rotation = _r0;
}

void RenderManager::Init(int width, int height)
{
  /******** framebuffer configuration ********/
  Info_Print("Create Framebuffer");
  resize_fbo(width, height);

  Info_Print("Init all renderer");
  for(const auto& renderer : _renderers)
  {
    renderer->Init();
  }

  Info_Print("Done");
}

void RenderManager::draw_all()
{
    // Bind Framebuffer 
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    // Set Background color
    glEnable(GL_DEPTH_TEST); // a mettre dans les renderers plutot ??
    glClearColor(_clear_color.x, _clear_color.y, _clear_color.z, _clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Compute Camera transform
    _view = mat4(1.0f);
    _view = glm::translate(_view, vec3(0.0f, 0.0f, -_cameradist));
    _view = _view * _rotation;
    _view = translate(_view, _camerapos);
    
    _vpmat = _projection * _view;

    // Draw all the renderer
    for (const auto& renderer : _renderers)
    {
      renderer->draw(_vpmat, _view);
    }

    // UnBind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // = unbind framebuffer
}

void RenderManager::resize_fbo(int width, int height)
{
  // Delete FBO
  if (_del_fbo) 
  {
    glDeleteFramebuffers(1, &_framebuffer);
    glDeleteTextures(1, &textureColorbuffer);
    glDeleteRenderbuffers(1, &_rbo);
  }
  // create framebuffer
  glGenFramebuffers(1, &_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
  // create a color attachment texture
  glGenTextures(1, &textureColorbuffer);
  glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
  // Create depth

  // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
  glGenRenderbuffers(1, &_rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo); // now actually attach it
  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind fbo

  _del_fbo = true;

  // TODO: Comprendre pk je dois reactiver les tectures !!! et comment faire quand j'ai plusieurs shaders ?
  // glActiveTexture(GL_TEXTURE0);
  // glBindTexture(GL_TEXTURE_2D, _renderers[0]->get_texture_id());
  // glUniform1i(glGetUniformLocation(_fragmentShader, "MatcapTexture"), 0); //TODO: _fragmentShader doesn't exist
  // Say which texture had to go to shader
  // glActiveTexture(GL_TEXTURE0);
  // glBindTexture(GL_TEXTURE_2D, _textureid);
  // glUniform1i(glGetUniformLocation(_fragmentShader, "MatcapTexture"), 0);
}

void RenderManager::update_rotation(vec2 mouse_offset, float angle)
{
  // Add a rotation of the angle rotationAngle from the axis rotationAxis to rotation, the transformation matrix
  vec3 rotationAxis = transpose(_rotation) * vec4(-mouse_offset[1], mouse_offset[0], 0.0f, 1.0f); 
  _rotation = glm::rotate(_rotation, glm::radians(angle), rotationAxis);
}

void RenderManager::update_camerapos(vec2 mouse_offset)
{
  // Add a translation to the camera position from the mouse offset input.
  _camerapos += _cameradist * vec3(transpose(_rotation) * vec4(mouse_offset[0], mouse_offset[1], 0.0f, 1.0f));
  // TODO: Optimize la transposition de la rotation et les cast inutiles...
}

void RenderManager::update_cameradist(float dist)
{
  // Zoom the camera according to the scroll values
  _cameradist += 0.5 * dist; // TODO: mettre le faccteur quand on calcul la view matrix plutot ??
  // TODO : CHoisir le signe en fonction du rapprochement avec le centre de l'écran(comme Blender)
}

void RenderManager::update_projection(float aspect)
{
  _projection = perspective(radians(_fov), aspect, _zNear, _zFar);
}

void RenderManager::reset_view()
{
  _rotation = _r0;
  _camerapos = vec3(0.0f, 0.0f, -1.0f);
  _cameradist = 3.0f;
}

void RenderManager::UI_pannel()
{
  ImGui::Begin("Render Settings");
  // ImGUI color picker for the background color
  ImGui::End();
}

RenderManager::~RenderManager()
{
  glDeleteTextures(1, &textureColorbuffer);
  glDeleteRenderbuffers(1, &_rbo);
  glDeleteFramebuffers(1, &_framebuffer);
}

#endif // !RENDER_MANAGER_CPP
