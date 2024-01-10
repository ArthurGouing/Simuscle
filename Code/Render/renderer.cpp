#ifndef RENDER_CPP
#define RENDER_CPP

#include "renderer.h"

using namespace glm;

//******** ABSTRACT CLASS ******** 

Renderer::Renderer(std::string name, std::string vert_source, std::string frag_source) :
  _name(name), _vshader_path(vert_source), _fshader_path(frag_source), _textureid(-1)
{
}

void Renderer::Init() // TODO: a virer, seul load shader suffit finalement
{
  Info_Print(_name+": Compite shaders: "+_vshader_path+" and "+_fshader_path);
  load_shader();

  Info_Print(_name+": Create VBO");
  init_VBO();
  Info_Print("Done.\n");
}

void Renderer::load_shader()
{
  // Format frag end vert
  std::ifstream vertexShaderFile(_vshader_path);
  std::ostringstream vertexBuffer;
  vertexBuffer << vertexShaderFile.rdbuf();
  std::string vertexBufferStr = vertexBuffer.str();
  // Warning: safe only until vertexBufferStr is destroyed or modified
  const GLchar* _vshader = vertexBufferStr.c_str(); //TODO: enlevé le _

  std::ifstream fragShaderFile(_fshader_path);
  std::ostringstream fragBuffer;
  fragBuffer << fragShaderFile.rdbuf();
  std::string fragBufferStr = fragBuffer.str();
  // Warning: safe only until vertexBufferStr is destroyed or modified
  const GLchar* _fshader = fragBufferStr.c_str();

  /******** Vertex shader ********/
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &_vshader, 0);
  glCompileShader(vertexShader);
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      Err_Print(infoLog, _vshader);
      exit(1);
  }

  /******** Fragment shader ********/
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &_fshader, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      Err_Print(infoLog, _fshader);
      exit(1);
  }

  /******** Link shader ********/
  _shaderProgram = glCreateProgram();
  glAttachShader(_shaderProgram, vertexShader);
  glAttachShader(_shaderProgram, fragmentShader);
  glLinkProgram(_shaderProgram);

  GLenum err;
  err = glGetError();
  std::cout << "OpenGL Error: " << err << std::endl;

  // check for linking errors
  glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(_shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    exit(1);
  }

  Info_Print("\nOpenGL Error: ");
  glCheckError();

  /******** Cleaning ********/
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  vertexShaderFile.close();
  fragShaderFile.close();
}


void Renderer::load_texture(std::string texture_path)
{
  Info_Print("Load texture");
  glGenTextures(1, &_textureid);
  Info_Print("Bind texture");
  glBindTexture(GL_TEXTURE_2D, _textureid); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
  Info_Print("Load parameters");
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  Info_Print("Done");

  // load image, create texture and generate mipmaps
  // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
  char* texture_char = new char[texture_path.length()+1];
  strcpy(texture_char, texture_path.c_str());

  int width, height, nrChannels; // just en local dans la fonction load ???
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

  GLenum err;
  err = glGetError();
  std::cout << "OpenGL Error: " << err << std::endl;
  Info_Print("\nOpenGL Error: ");
  glCheckError();

  /******** Cleaning ********/
  stbi_image_free(data);
}

Renderer::~Renderer()
{
  glDeleteTextures(1, &_textureid);
  glDeleteProgram(_shaderProgram);
}




//******** GEOMETRY RENDER CLASS ******** 


GeomRenderer::GeomRenderer(std::string name, std::string vert_path, std::string frag_path, std::string texture_path, std::vector<Geometry*> geometries):
  Renderer(name, vert_path, frag_path), _allgeom(geometries), render_mode(mesh)
{
  _texture_path = texture_path; // already init in Renderer constructor
}

GeomRenderer::GeomRenderer(std::string name, std::string vert_path, std::string frag_path, std::string texture_path):
  Renderer(name, vert_path, frag_path), _texture_path(texture_path), render_mode(mesh)
{}

void GeomRenderer::Init()
{
  Title_Print("Init "+_name);
  Info_Print("Compite shaders: "+_vshader_path+" and "+_fshader_path);
  load_shader();

  Info_Print("Load Texture : "+_texture_path);
  load_texture(_texture_path);

  Info_Print("Create VBO");
  init_VBO();
}

void GeomRenderer::init_VBO()
{
   
  // Create VBO
  glGenVertexArrays(1, &_VAO);
  glGenBuffers(1, &_VBO);
  glGenBuffers(1, &_EBO);

  // Create values and indices from the geometries (or at least give the max size)
  Info_Print("Nb of geometry to render: "+std::to_string(_allgeom.size()));
  for (const auto& geom : _allgeom)
  {
    for (int i = 0; i < geom->n_verts; i++)
    {
      vert_arr arr;
      arr.pos = geom->vertex_list[i].pos;
      arr.normal = geom->vertex_list[i].normal;
      _values.push_back(arr);
    }
    for (int i = 0; i < geom->n_faces; i++)
    { 
      // std::cout << geom->face_list[i].get_v1_id() << std::endl;
      // std::cout << geom->offset_id << std::endl;
      // std::cout << geom->offset_id + geom->face_list[i].get_v1_id() << std::endl;
      _indices.push_back(geom->offset_id + geom->face_list[i].get_v1_id());
      _indices.push_back(geom->offset_id + geom->face_list[i].get_v2_id());
      _indices.push_back(geom->offset_id + geom->face_list[i].get_v3_id());
    }
  }

  _n_values = 6 * _values.size();

  glBindVertexArray(_VAO);

  // Allocate values
  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  glBufferData(GL_ARRAY_BUFFER, _values.size() * 6 * sizeof(float), &(_values[0]), GL_DYNAMIC_DRAW); //TODO: change to dynamic draw
  // Allocate indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(float), _indices.data(), GL_DYNAMIC_DRAW);
  // Specify vertex attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  Info_Print("Unbind EBO ???");

  GLenum err;
  err = glGetError();
  std::cout << "OpenGL Error: " << err << std::endl;
  Info_Print("\nOpenGL Error: ");
}

void GeomRenderer::update_VBO()
{
  Info_Print("Update VBO");
  // update values and indices
  for(const auto& geom : _allgeom)
  {
    for (int i = 0; i < geom->n_verts; i++)
    {
      _values[i + geom->offset_id] = geom->compute_value(i); // Peut etre metre le offset dans 
    }
  }

  // Send new values to VBO
  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

  glBufferSubData(GL_ARRAY_BUFFER, 0, _values.size() * 6 * sizeof(float), _values.data()); // cf la doc pour l'utiliser correctement
  // glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, _indices.size() * sizeof(int), _indices.data());

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GeomRenderer::draw(mat4 vp_mat_ptr, mat4 view_ptr)
{
  // Bind
  glUseProgram(_shaderProgram);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _textureid);
  glBindVertexArray(_VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

  Info_Print("Drawing skeleton");
  Mat4_Print("vp mat: ", vp_mat_ptr);

  // Send variables to the GPU
  int modelLoc = glGetUniformLocation(_shaderProgram, "vp_mat");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(vp_mat_ptr));
  int viewLoc = glGetUniformLocation(_shaderProgram, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_ptr));
  Info_Print("Uniform mat4 error: ");
  glCheckError();

  // Set draw parameters
  if (render_mode == wire) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else if (render_mode == mesh) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  } else {
    Err_Print("The render_mode '" + std::to_string(render_mode) + "' is not valid. Choose between ['mesh', 'wire', 'stick'].", "muscle_system.cpp");
  }
  // n_values: 
  Info_Print(std::to_string(_n_values));
  // _n_values = 5000 * 6;

  // Draw elements
  glCheckError();
  glDrawElements(GL_TRIANGLES, _n_values, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
}

GeomRenderer::~GeomRenderer()
{
  glDeleteVertexArrays(1, &_VAO);
  glDeleteBuffers(1, &_VBO);
  glDeleteBuffers(1, &_EBO);
}

#endif // !RENDER_CPP
