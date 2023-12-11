#include "muscle_system.h"

using namespace glm;


MuscleSystem::MuscleSystem(std::string project, Skeleton *skel) :
  _skel(skel)
{
  Title_Print("Init Muscle System");

  /******** Read Muscles parameters ********/
  std::string muscle_file = project+"Muscles/muscles_info.json";
  Info_Print("Reading Muscle info file : " + muscle_file);
  read_muscles_parameters(muscle_file, project);

  /******** Load Muscles meshes ********/
  int offset = 0;
  for (int i = 0; i < muscles.size(); i++) {
    muscles[i].create_geometry(&offset);  // can be dones ine the read muscles parameters
  }

  Info_Print("Done");
}

void MuscleSystem::read_muscles_parameters(std::string file_name, std::string project)
{
  // Open muscle_info file
  std::ifstream info(file_name);
  std::string buff;
  Info_Print("Read '"+file_name+"'");
  info >> buff;
  while(info.is_open())
  {
    // Init
    std::string name, geometry_path;
    Bone *bone_insertion_1, *bone_insertion_2;

    // Read
    info >> buff; // {
    info >> buff;
    if (buff == "\"name\":"){
      while(buff.substr(buff.size()-2, buff.size()-1) != "\",")
      {
        info >> buff;
        if (buff.substr(0, 1)=="\""){
          name += buff.substr(1, buff.size()) + " ";
        } else {
          name += buff + " ";
        }
        if (name.substr(name.size()-3, name.size()-1) =="\", "){
          name = name.substr(0, name.size()-3);
        }
      }
    } else if (buff == "]") {
      return;
    }
    Info_Print("name: "+name);
    info >> buff;
    if (buff == "\"geometry\":"){
      info >> buff;
      geometry_path = project + "Muscles/" + buff.substr(1, buff.size()-3);
    }
    Info_Print("geometry : "+geometry_path);
    info >> buff;
    if (buff == "\"insertion_1\":"){
      info >> buff;
      bone_insertion_1 = _skel->find_bone(buff.substr(1, buff.size()-3));
    }
    Info_Print("Bone insertion 1: " + bone_insertion_1->_name);
    info >> buff;
    if (buff == "\"insertion_2\":"){
      info >> buff;
      bone_insertion_2 = _skel->find_bone(buff.substr(1, buff.size()-3));
    }
    Info_Print("Bone insertion 2: " + bone_insertion_2->_name);
    info >> buff;
    if (buff == "\"curve\":"){
      Info_Print("TODO: read curve info");
    }
    info >> buff; // First degree "["
    vec3 P0, P1, P2, P3;
    P0 = read_point(info);
    P1 = read_point(info);
    P2 = read_point(info);
    P3 = read_point(info);
    info >> buff; // First degree "]"

    int n_point = 32; // Choose nel ici !!!
    std::string curve_name = name+"_curve";
    Curve muscle_curve(curve_name, n_point, P0, P1, P2, P3);

    // Add muscle to the muscles list
    Muscle muscle(name, geometry_path, muscle_curve, bone_insertion_1, bone_insertion_2);
    muscles.push_back(muscle);


    info >> buff; // buff == },

    if(buff == "}"){
      info.close();
      break;
    }
  }
}

vec3 MuscleSystem::read_point(std::ifstream& info)
{
  std::string buff;
  glm::vec3 P;
  info >> buff;
  info >> buff;
  std::stringstream ssbuffx(buff.substr(0, buff.size()-1));
  ssbuffx >> P.x;
  info >> buff;
  std::stringstream ssbuffy(buff.substr(0, buff.size()-1));
  ssbuffy >> P.y;
  info >> buff;
  std::stringstream ssbuffz(buff);
  ssbuffz >> P.z;
  info >> buff;
  return P;
}

void MuscleSystem::update_VBO()
  // TODO: faire un create et un update car la taille ne changera jamais, peut être faire ce que je voulais à la base 
  // c'est à dire, quand les points de la geom sont modifier, les modifiers directement à l'adresse indiqué dans VBO
{
  // Create VBO
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(VAO);

  // Update bonne position to the VBO
  std::vector<glm::vert_arr> values;
  std::vector<int>           indices;

  for (int i = 0; i < muscles.size(); i++) {
    // Info_Print(muscles[i]._name);
    values.insert(values.end(), muscles[i]._mesh.vert_values.begin(), muscles[i]._mesh.vert_values.end());
    indices.insert(indices.end(), muscles[i]._mesh.face_indices.begin(), muscles[i]._mesh.face_indices.end());
    // Info_Print(std::to_string(values.size()));
  }

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, values.size() * 6 * sizeof(float), &(values[0]), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  n_values = 6 * values.size();

  std::vector<glm::vert_arr>().swap( values );
  std::vector<int>().swap( indices );
  // values.clear();
  // values.shrink_to_fit();
  // indices.clear();
  // indices.shrink_to_fit();
}

void MuscleSystem::draw_muscles()
{
  // Draw the muscles
  glBindVertexArray(VAO);  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Set parameters
  if (render_mode == "wire") {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else if (render_mode == "mesh") {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  } else {
    Err_Print("The render_mode '" + render_mode + "' is not valid. Choose between ['mesh', 'wire', 'stick']", "muscle_system.cpp");
  }

  glDrawElements(GL_TRIANGLES, n_values, GL_UNSIGNED_INT, 0);
}

void MuscleSystem::draw_curves(int frame){
  glGenVertexArrays(1, &crv_VAO);
  glGenBuffers(1, &crv_VBO);
  glGenBuffers(1, &crv_EBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(crv_VAO);

  // Update bonne position to the VBO
  std::vector<glm::vec3> values;
  std::vector<int>       indices;
  //
  values = muscles[0]._curve.curve_points;
  for (int i = 0; i < values.size(); i++) {
    values[i].z = muscles[0]._curve.curve_points[i].z - frame * 0.001;
  }
  n_point = values.size();
  for (int i = 0; i < n_point-1; i++) {
    indices.push_back(n_point-i-1);
  }

  glBindBuffer(GL_ARRAY_BUFFER, crv_VBO);
  glBufferData(GL_ARRAY_BUFFER, values.size() * 3 * sizeof(float), &(values[0]), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, crv_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
  glEnableVertexAttribArray(0);
  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  n_values = 3 * n_point;

  // std::vector<glm::vec3>().swap( values );
  // std::vector<int>().swap( indices );

  glBindVertexArray(crv_VAO);
  glDisable(GL_DEPTH_TEST);
  glLineWidth(4.f);
  glDrawElements(GL_LINE_STRIP, n_values, GL_UNSIGNED_INT, 0);
  glPointSize(8.f);
  glDrawElements(GL_POINTS, n_values, GL_UNSIGNED_INT, 0);
  glEnable(GL_DEPTH_TEST);
}

MuscleSystem::~MuscleSystem()
{}
