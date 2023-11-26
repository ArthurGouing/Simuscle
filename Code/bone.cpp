#ifndef BONE_CPP
#define BONE_CPP

#include "bone.h"

#include <chrono>
#include <typeinfo>

using namespace glm;
using namespace std::chrono;

enum Token {Hierarchy, Motion, Offset, Channels, Joint, End, Return, Default};
Token find_tok(std::string buff)
{
  if (buff=="HIERARCHY") return Hierarchy;
  if (buff=="MOTION")    return Motion;
  if (buff=="OFFSET")    return Offset;
  if (buff=="CHANNELS")  return Channels;
  if (buff=="JOINT")     return Joint;
  if (buff=="END")       return End;
  if (buff=="}")         return Return;
  return Default;
}


Bone::Bone()
{}

Bone::Bone(std::string file_name)
{
  create_from_file(file_name);
}

Bone::Bone(std::ifstream& anim_file, std::string name, Bone *parent)
{
  parse_bone(anim_file, name, parent);
}


void Bone::create_from_file(std::string file_name)
{
  // Init
  std::ifstream anim_file(file_name);
  std::string   buff; 
  Info_Print("Open '"+ file_name +"'");
  if (!anim_file.is_open()) {
    Err_Print("Cannot open "+file_name, "bone.cpp");
    return;// ou exit ??
  }
  // Read Tokens
  while (anim_file.is_open())
  {
    anim_file >> buff; 
    Info_Print("Maine while loop; "+buff);
    switch (find_tok(buff)) {
      case Hierarchy:
        anim_file >> buff;
        if(buff != "ROOT") {
          Err_Print("Bad structure of BVH file (line 2: exepted ROOT)", "bone.cpp");
          return;
        }
        anim_file >> buff;
        parse_bone(anim_file, buff, nullptr);
        break;
      case Motion:
        Info_Print("Parse MOTION");
        int nb_frame;
        float frame_time;
        anim_file >> buff; anim_file >> nb_frame;
        anim_file >> buff; anim_file >> buff; anim_file >> frame_time;
        _nb_frames = nb_frame;
        _frame_time = frame_time;
        for (int frame = 0; frame < nb_frame; frame++)
        {
          parse_frames(anim_file);
        }
        Info_Print("End Read Data");
        anim_file.close();
        break;
      default:
          Err_Print("Bad structure of BVH file (line 2: exepted HIERARCHY/MOTION)", "bone.cpp");
          return;
    }
  }
  print_bone(0);
}

void Bone::parse_bone(std::ifstream& anim_file, std::string name, Bone* parent)
{
  // Vérifier que anim_file est bien passer, c'était compliquer dans le projet...
  std::string buff; 
  // Read name
  _name = name;
  bool parse_end = false;
  anim_file >> buff; // throw '{' bracket
  while (!parse_end)
  {
    anim_file >> buff;

    if (buff=="OFFSET") {
      float offX, offY, offZ;
      // Extract offset values
      anim_file >> offX; anim_file >> offY; anim_file >> offZ;
      // Store values
      _offset = vec3(offX, offY, offZ);

    } else if (buff=="CHANNELS") {
      // Get channels size
      int ndof;
      anim_file >> ndof;
      _dofs.resize(ndof);
      // Store name and order
      int order_count = 0;
      for (int i = 0; i < ndof; i++) 
      {
        anim_file >> buff;
        _dofs[i].name = buff;
        if (buff=="Xrotation")
          order_count += 100*(i%3+1); // TODO: improve, will buf if nbuff != 3 or 6
        if (buff=="Yrotation")
          order_count += 10*(i%3+1);
        if (buff=="Zrotation")
          order_count += 1*(i%3+1);
      }
      
      _rorder = RotateOrder(order_count);

    } else if (buff=="JOINT") {
      // add a children
      anim_file >> buff;
      Bone child(anim_file, buff, this);
      _childrens.push_back(child);

    } else if (buff=="End") {
      // Last Bone of the chains
      anim_file >> buff; // Site
      Bone child(anim_file, _name+"_END", this);
      _childrens.push_back(child);
 
    } else if (buff=="}") {
      parse_end = true;
      return;
    } else {
      Err_Print("Error: Bad structure of BVH file. The token '"+buff+"' is unknow. Reading bvh file cancelled", "bone.cpp");
      parse_end = true;
      return;
    }
  }
}

void Bone::parse_frames(std::ifstream& anim_file)
{
  // Add Dof value
  for (int dof = 0; dof < _dofs.size(); dof++)
  {
    double dof_value;
    anim_file >> dof_value;
    _dofs[dof]._values.push_back(dof_value);
  }

  // Recursive to childs
  for (int ichild = 0; ichild < _childrens.size(); ichild++)
  {
    _childrens[ichild].parse_frames(anim_file);
  }
}

void Bone::create_geometry(BonesInfo info, std::string project_path, int *indice_offset)
// Read bone_inf.json file to assigne the correct geometry to each bone
{
  for (int i = 0; i < info.list_info.size(); i++)
  {
    // Info_Print(info.list_info[i].parent);
    // Info_Print(_name);
    if (info.list_info[i].parent == _name) {
      std::string geometry_file = project_path+"Bones/"+info.list_info[i].name+".off";
      Info_Print(geometry_file);
      _mesh.create_from_file(geometry_file);
      for (int i = 0; i < _mesh.face_indices.size(); i++){
        _mesh.face_indices[i] += *indice_offset;
      }
      *indice_offset += _mesh.n_verts;
      break;
    }
  }
  // Recursive child function
  for (int ichild = 0; ichild < _childrens.size(); ichild++) {
    _childrens[ichild].create_geometry(info, project_path, indice_offset);
  }
}


void Bone::update_values(std::vector<glm::vert_arr>* values,std::vector<int>* indices)
  // Update recursively geometry to the VAO for every bones
{
  values->insert(values->end(), _mesh.vert_values.begin(), _mesh.vert_values.end());
  indices->insert(indices->end(), _mesh.face_indices.begin(), _mesh.face_indices.end());
  for (int ichild = 0; ichild < _childrens.size() ; ichild++) {
    _childrens[ichild].update_values(values, indices);
  }
}

void Bone::animate(float time)
{
  Info_Print("Go to time "+std::to_string(time));
  // For recursive childrens
}

void Bone::print_bone(int level)
{
  std::string space = "";
  for (int i = 0; i < level; i++)
    space += "  ";
  // Print frame values
  // int f = 300;
  // std::cout << _name << ": " << std::endl;
  // for (int i = 0; i < _dofs.size(); i++) {
  //   std::cout <<  _dofs[i].name << ": " << _dofs[i]._values[f] << " | ";
  // }
  // std::cout << std::endl;

  // Print structure
  std::cout << space << _name << std::endl;
  for (int i = 0; i < _childrens.size(); i++) 
  {
    _childrens[i].print_bone(level+1);
  }
}

Bone::~Bone()
{
  _childrens.clear();
  _dofs.clear();
}

#endif // !BONE_CPP
