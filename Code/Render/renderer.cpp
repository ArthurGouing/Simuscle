#ifndef RENDER_CPP
#define RENDER_CPP

#include "renderer.h"

using namespace glm;

//******** ABSTRACT CLASS ******** 

Renderer::Renderer(std::string name, std::string vert_source, std::string frag_source) :
  _name(name), _vshader_path(vert_source), _fshader_path(frag_source)
{
}

void Renderer::update_projection(int width, int height, float fov, float zNear, float zFar)
{
  float aspect = float(width) / float(height ? height : 1);
  _viewport_size = vec2(width, height);
  _projection = perspective(radians(fov), aspect, zNear, zFar);
}

void Renderer::load_shader()
{
  Info_Print("Compite shaders: "+_vshader_path+" and "+_fshader_path);
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

void Renderer::UI_pannel()
{
  std::string title, text;
  title = "Renderer "+ _name;
  ImGui::Begin(title.c_str());
  text = "Render vertex shader: " + _vshader_path;
  ImGui::Text(text.c_str());
  text = "Render frag shader:   " + _fshader_path;
  ImGui::Text(text.c_str());
  ImGui::End();
}


Renderer::~Renderer()
{
  glDeleteProgram(_shaderProgram);
}

//******** GEOMETRY RENDER CLASS ******** 


template <typename Object>
MatcapRenderer<Object>::MatcapRenderer(std::string name, std::string vert_path, std::string frag_path, std::string texture_path, std::vector<Object*> geometries):
  Renderer(name, vert_path, frag_path), _allgeom(geometries), render_mode(mesh)
{
  _texture_path = texture_path; // already init in Renderer constructor
}

template <typename Object>
MatcapRenderer<Object>::MatcapRenderer(std::string name, std::string vert_path, std::string frag_path, std::string texture_path):
  Renderer(name, vert_path, frag_path), _texture_path(texture_path), render_mode(mesh)
{}

template <typename Object>
void MatcapRenderer<Object>::Init()
{
  Info_Print("Load Texture : "+_texture_path);
  load_texture(_texture_path);

  Info_Print("Create VBO");
  init_VBO();
}

template <typename Object>
void MatcapRenderer<Object>::load_texture(std::string texture_path)
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

  /******** Cleaning ********/
  stbi_image_free(data);
}

template<typename Object>
void MatcapRenderer<Object>::init_VBO()
{
  // Create VBO
  glGenVertexArrays(1, &_VAO);
  glGenBuffers(1, &_VBO);
  glGenBuffers(1, &_EBO);

  // Create values and indices from the geometries (or at least give the max size)
  Info_Print("Nb of geometry to render: "+std::to_string(_allgeom.size()));

  for (const auto& obj : _allgeom)
  {
    init_val_id(obj);
  }

  _n_values = ValueFormat::size * _values.size(); // TODO; a templater, depend du type et donc ValueFormat

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
}

template <>
void MatcapRenderer<Geometry>::init_val_id(Geometry* geom)
{
  // Values
  for (int i = 0; i < geom->n_verts; i++)
  {
    // TODO: surement une spécialisation pour les curves
    ValueFormat arr;
    arr = geom->compute_value(i);
    _values.push_back(arr);
  }
  // Indices
  for (int i = 0; i < geom->n_faces; i++)
  { 
    // TODO: surement une spécialisation pour les curves
    _indices.push_back(geom->offset_id + geom->face_list[i].get_v1_id());
    _indices.push_back(geom->offset_id + geom->face_list[i].get_v2_id());
    _indices.push_back(geom->offset_id + geom->face_list[i].get_v3_id());
  }
}
template <>
void MatcapRenderer<Curve>::init_val_id(Curve* crv)
{
  // Values
  for (int i = 0; i < crv->n_verts; i++)
  {
    ValueFormat arr;
    arr = crv->compute_value(i);
    _values.push_back(arr);
  }
  // Indices
  for (int vert_id = 0; vert_id < crv->n_verts-1; vert_id++)
  {
    _indices.push_back(crv->offset_id + vert_id);
    _indices.push_back(crv->offset_id + vert_id+1);
  }
}

template <typename Object>
void MatcapRenderer<Object>::update_VBO()
{
  Info_Print("Update VBO");
  // update values and indices
  for(const auto& geom : _allgeom)
  {
    for (int i = 0; i < geom->n_verts; i++)
    {
      // TODO: surement des specialisations pour la curve
      _values[i + geom->offset_id] = geom->compute_value(i);// For curve, what is the new point of the curve
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

template<typename Object>
void MatcapRenderer<Object>::draw(vec3 camera_pos, mat4 rotation, float camera_dist)
{
  // Bind
  glUseProgram(_shaderProgram);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _textureid);
  glBindVertexArray(_VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

  // Compute Camera transform
  mat4 view(1.0f);
  view = translate(view, vec3(0.0f, 0.0f, -camera_dist));
  view = view * rotation;
  view = translate(view, camera_pos);
  
  mat4 vpmat = _projection * view;
  vpmat = scale(vpmat, vec3(1.f, 1.f, 1.f));

  // Send variables to the GPU
  int modelLoc = glGetUniformLocation(_shaderProgram, "vp_mat");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(vpmat));
  int viewLoc = glGetUniformLocation(_shaderProgram, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

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
  draw_elements();

  glBindVertexArray(0);
}
template<>
void MatcapRenderer<Geometry>::draw_elements()
{
  glDrawElements(GL_TRIANGLES, _n_values, GL_UNSIGNED_INT, 0);
}
template<>
void MatcapRenderer<Curve>::draw_elements()
{
  glDrawElements(GL_LINES, _n_values, GL_UNSIGNED_INT, 0);
}
template<>
void MatcapRenderer<Line>::draw_elements()
{
  glDrawElements(GL_LINES, _n_values, GL_UNSIGNED_INT, 0);
}

template<typename Object>
MatcapRenderer<Object>::~MatcapRenderer()
{
  // Renderer interface part
  glDeleteProgram(_shaderProgram);

  glDeleteTextures(1, &_textureid);
  glDeleteVertexArrays(1, &_VAO);
  glDeleteBuffers(1, &_VBO);
  glDeleteBuffers(1, &_EBO);
}


//******** GROUND RENDERER ********


GroundRenderer::GroundRenderer(std::string name, std::string vert_path, std::string frag_path):
  Renderer(name, vert_path, frag_path), _ground_size(50.f)
{
}

void GroundRenderer::Init()
{
  Info_Print(_name + " : Create VBO");
  init_VBO();
}

void GroundRenderer::init_VBO()
{
  float vertices[] = {
       _ground_size,  _ground_size, 0.0f,  // top right
       _ground_size, -_ground_size, 0.0f,  // bottom right
      -_ground_size, -_ground_size, 0.0f,  // bottom left
      -_ground_size,  _ground_size, 0.0f   // top left 
  };
  unsigned int indices[] = {  // note that we start from 0!
      0, 1, 3,  // first Triangle
      1, 2, 3   // second Triangle
  };
  glGenVertexArrays(1, &_VAO);
  glGenBuffers(1, &_VBO);
  glGenBuffers(1, &_EBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GroundRenderer::draw(glm::vec3 camera_pos, glm::mat4 rotation, float camera_dist)
{
  // Bind
  glUseProgram(_shaderProgram);
  glBindVertexArray(_VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

  // Compute Camera transform
  mat4 view(1.0f);
  view = translate(view, vec3(0.0f, 0.0f, -camera_dist));
  view = view * rotation;
  view = translate(view, camera_pos);
  
  mat4 vpmat = _projection * view;
  vpmat = scale(vpmat, vec3(1.f, 1.f, 1.f));

  // Send variables to the GPU
  glUniform3fv(glGetUniformLocation(_shaderProgram, "camera_pos"), 1, &camera_pos[0]);
  int modelLoc = glGetUniformLocation(_shaderProgram, "vp_mat");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(vpmat));

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glCheckError();

  // Draw elements
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
}

GroundRenderer::~GroundRenderer()
{
  glDeleteProgram(_shaderProgram);
  glDeleteBuffers(1, &_VBO);
  glDeleteBuffers(1, &_EBO);
  glDeleteVertexArrays(1, &_VAO);
}



//******** RAY MARCHING RENDER CLASS ******** 


MarchingRenderer::MarchingRenderer(std::string name, std::string vert_path, std::string frag_path):
  Renderer(name, vert_path, frag_path),
  background_color(0.2, 0.5, 0.5),
  fog_factor_sky(5.), fog_factor_ground(0.003),
  white_color(0.25), black_color(0.4), grid_size(0.),
  is_gamma_correction(true)
{
}

void MarchingRenderer::Init()
{
  Info_Print("Create VBO");
  init_VBO();
}

void MarchingRenderer::init_VBO()
{
  // Create VBO
  glGenVertexArrays(1, &_VAO);
  glGenBuffers(1, &_VBO);

  glBindVertexArray(_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  const static GLfloat vertices[] = {
	-1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,
	1.0f,  -1.0f, -1.0f, -1.0f, -1.0f,  1.0f};
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // float resolution[2] = {SCREEN_WIDTH, SCREEN_HEIGHT};
  // glUniform2fv(glGetUniformLocation(shader, "iResolution"), 1, &resolution[0]);

  // Unbind
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MarchingRenderer::draw(glm::vec3 camera_pos, glm::mat4 rotation, float camera_dist)
{
  glBindVertexArray(_VAO); 
  glUseProgram(_shaderProgram);
  glDepthMask(GL_FALSE);

  // Send parameters to GPU
  float resolution[2] = {_viewport_size.x, _viewport_size.y}; //TODO:faire directement un ptr du _viewport_size
  Info_Print("resolution: "+std::to_string(_viewport_size.x)+", "+std::to_string(_viewport_size.y));
  rotation = transpose(rotation);
  glUniform2fv(glGetUniformLocation(_shaderProgram, "i_resolution"), 1, &_viewport_size[0]);
  glUniform3fv(glGetUniformLocation(_shaderProgram, "camera_pos"), 1, &camera_pos[0]);
  glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "rotation"), 1, GL_FALSE, glm::value_ptr(rotation));
  glUniform1fv(glGetUniformLocation(_shaderProgram, "camera_dist"), 1, &camera_dist);

  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
  glDepthMask(GL_TRUE);
}


MarchingRenderer::~MarchingRenderer()
{
  // Renderer interface part
  glDeleteProgram(_shaderProgram);

  glDeleteVertexArrays(1, &_VAO);
  glDeleteBuffers(1, &_VBO);
}

#endif // !RENDER_CPP
