#ifndef MUSCLE_SYSTEM_CPP
#define MUSCLE_SYSTEM_CPP
#include "muscle_system.h"

using namespace glm;


MuscleSystem::MuscleSystem(std::string project, Renderer* renderer, Skeleton *skel) :
  _renderer(renderer), _skel(skel), line_width(4.f), point_width(8.f), gravity(true)
{
  Title_Print("Init Muscle System");

  /******** Read Muscles parameters ********/
  std::string muscle_file = project+"Muscles/muscles_info.json";
  Info_Print("Reading Muscle info file : " + muscle_file);
  read_muscles_parameters(muscle_file, project);

  /******** Load Muscles meshes ********/
  int offset = 0;
  for (size_t i = 0; i < muscles.size(); i++) {
    muscles[i].create_geometry(&offset);  // can be dones ine the read muscles parameters
    Info_Print("offset: "+std::to_string(offset));
  }

  /******** Link meshes to the renderer ********/
  for (size_t i = 0; i < muscles.size(); i++) {
    _renderer->add_object(&(muscles[i]._mesh));
  }

  /******** Other parameters ********/
  _nb_frames = skel->_nb_frames;

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
    vec3 P0, P1, P2, P3;

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
    } else { Err_Print("Bad File structure", "muscle_system.cpp"); }
    info >> buff;
    if (buff == "\"insertion_1\":"){
      info >> buff;
      bone_insertion_1 = _skel->find_bone(buff.substr(1, buff.size()-3));
    } else { Err_Print("Bad File structure", "muscle_system.cpp"); }
    info >> buff;
    if (buff == "\"insertion_2\":"){
      info >> buff;
      bone_insertion_2 = _skel->find_bone(buff.substr(1, buff.size()-3));
    } else { Err_Print("Bad File structure", "muscle_system.cpp"); }
    info >> buff;
    if (buff == "\"curve\":"){
      info >> buff; // First degree "["
      P0 = read_point(info);
      P1 = read_point(info);
      P2 = read_point(info);
      P3 = read_point(info);
      info >> buff; // First degree "]"
    } else { Err_Print("Bad File structure", "muscle_system.cpp"); }

    // Create curve
    int n_point = 5; 

    // Add muscle to the muscles list
    Muscle muscle(name, geometry_path, bone_insertion_1, bone_insertion_2, n_point, P0, P1, P2, P3, &solver_param);
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


void MuscleSystem::update_geom_buffers()
  // TODO: faire un create et un update car la taille ne changera jamais, peut être faire ce que je voulais à la base 
  // c'est à dire, quand les points de la geom sont modifier, les modifiers directement à l'adresse indiqué dans VBO
{
  _renderer->update_VBO();
}

void MuscleSystem::init_crv_buffers()
{
  // Create buffers
  glGenVertexArrays(1, &crv_VAO);
  glGenBuffers(1, &crv_VBO);
  glGenBuffers(1, &crv_EBO);

  // Init max values size
  int values_size = 0;
  for (size_t i = 0; i < muscles.size(); i++) {
    values_size += muscles[i]._curve.n_points;
    Info_Print(muscles[i]._curve.name);
  }
  values_crv.resize(values_size);

  n_values_crv = 3 * values_size;

  // Init indices
  int id_offset = 0;
  for (size_t i = 0; i < muscles.size(); i++) {
    int n_point = muscles[i]._curve.n_points; // TODO correct curve so nb_el is the number of element and nb_pts the number of points
    for (int j = 0; j < n_point-1; j++) {
      indices_crv.push_back(id_offset + j);
      indices_crv.push_back(id_offset + j+1);
    }
    muscles[i]._curve.set_id_offset(id_offset);
    // muscles[i]._curve.id_offset = id_offset;
    id_offset += n_point;
  }

  update_curve_buffers(0);
  Info_Print("draw curve");
  draw_curves();
  Info_Print("draw curve done");
}

void MuscleSystem::update_curve_buffers(int frame)
{
  for (int i = 0; i < 1 /*muscles.size()*/; i++) {
    Deformations* deformation;
    deformation = muscles[i]._solver.get_solution();
    // if (muscles[i]._name=="Biceps")
    // deformation->print();
    muscles[i]._curve.update_values(&values_crv, deformation);
  }
}

void MuscleSystem::draw_curves()
{
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(crv_VAO);


  glBindBuffer(GL_ARRAY_BUFFER, crv_VBO);
  glBufferData(GL_ARRAY_BUFFER, values_crv.size() * 3 * sizeof(float), &(values_crv[0]), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, crv_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_crv.size() * sizeof(float), indices_crv.data(), GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
  glEnableVertexAttribArray(0);
  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(crv_VAO);
  glDisable(GL_DEPTH_TEST);
  glLineWidth(line_width);
  glDrawElements(GL_LINES, n_values_crv, GL_UNSIGNED_INT, 0);
  glPointSize(point_width);
  glDrawElements(GL_POINTS, n_values_crv, GL_UNSIGNED_INT, 0);
  glEnable(GL_DEPTH_TEST);
}

void MuscleSystem::solve(int frame)
{
  for (size_t i = 0; i < muscles.size(); i++) {
    muscles[i].solve(frame);
  }
  _renderer->update_VBO();
}

void MuscleSystem::UI_pannel()
{
  static ImGuiComboFlags flags=0;
  ImGui::Begin("Muscles parameters");
  ImGui::PushItemWidth(200);

  ImGui::SeparatorText("Render parameters");
  ImGui::DragInt("line width", &line_width, 0.05f, 0.f, 20.f);
  ImGui::DragInt("point width", &point_width, 0.05f, 0.f, 20.f);

  ImGui::SeparatorText("Solver parameters");
  ImGui::Checkbox("Gravity", &solver_param.gravity);
  ImGui::SliderInt("Kmax", &solver_param.kmax, 1, 100);
  if (ImGui::InputInt("Substep", &solver_param.n_substep)) {
    for (size_t i = 0; i < muscles.size(); i++) {
      muscles[i]._solver.update_matrices();
    }
  }

  ImGui::SliderFloat("Epsilon", &solver_param.epsilon, 0.0000001, 10);
  const char* simu_type_choice[] = { "static", "dynamic_implicit", "dynamic_visc_implicit", "dynamic_explicit", "dynamic_visc_explicit"};
  const char* preview = simu_type_choice[solver_param.methode];
  if (ImGui::BeginCombo("Simulation type", preview, flags)) {
    for (int i = 0; i < 5; i++) {
      const bool is_selected = (solver_param.methode==i);
      if (ImGui::Selectable(simu_type_choice[i], is_selected)) {
        solver_param.methode = i;
        for (size_t i = 0; i < muscles.size(); i++) {
          muscles[i]._solver.update_matrices();
        }
      }
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  // ImGui::Combo("Simulation type", &solver_param.methode, simu_type_choice, IM_ARRAYSIZE(simu_type_choice));
  const char* solver_type_choice[] = { "iteratif", "direct"};
  const char* preview2 = solver_type_choice[solver_param.solver];
  if (ImGui::BeginCombo("Solver type", preview2, flags)) {
    for (int i = 0; i < 2; i++) {
      const bool is_selected_2 = (solver_param.solver==i);
      if (ImGui::Selectable(solver_type_choice[i], is_selected_2)) {
        std::cout << i << " " << solver_param.solver << std::endl;
        solver_param.solver = i;
        std::cout << i << " " << solver_param.solver << std::endl;
        for (size_t i = 0; i < muscles.size(); i++) {
          muscles[i]._solver.update_matrices();
        }
      }
      if (is_selected_2)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  std::cout << "soolver: "<<solver_param.solver<<std::endl;

  ImGui::SeparatorText("Muscles parameters");
  // get char length
  ImGui::Separator();
  static int item_current_idx = 0; // Here we store our selection data as an index.
  std::string s = muscles[item_current_idx]._name;
  if (ImGui::BeginCombo("Select muscle", s.c_str(), flags))
  {
      for (int i = 0; i < int(muscles.size()); i++) // WARN: pas sur que ce soit une bonne idée de cast in long signed int en int
      {
          const bool is_selected = (item_current_idx == i);
          s = muscles[i]._name;
          if (ImGui::Selectable(s.c_str(), is_selected))
              item_current_idx = i;

          // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
          if (is_selected)
              ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
  }
  muscles[item_current_idx].UI_pannel();
  ImGui::End();
}

MuscleSystem::~MuscleSystem()
{}

#endif // !MUSCLE_SYSTEM_CPP
