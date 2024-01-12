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

Bone::Bone(std::string file_name):
  transformation(1.0), _nb_frames(0), _indice_offset(0)
{
  create_from_file(file_name);
}

Bone::Bone(std::ifstream& anim_file, std::string name, Bone *parent):
  transformation(1.0), _nb_frames(0), _indice_offset(0)
{
  parse_bone(anim_file, name, parent);
}


void Bone::create_from_file(std::string file_name)
{
  // Init
  std::ifstream anim_file(file_name);
  std::string   buff; 
  if (!anim_file.is_open()) {
    Err_Print("Cannot open "+file_name, "bone.cpp");
    return;// ou exit ??
  }
  // Read Tokens
  while (anim_file.is_open())
  {
    anim_file >> buff; 
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
        anim_file.close();
        break;
      default:
          Err_Print("Bad structure of BVH file (line 2: exepted HIERARCHY/MOTION)", "bone.cpp");
          return;
    }
  }
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
      // construct the posistion of the bone
      if (parent==nullptr)  {
        _pos = _offset;
      } else {
        _pos = parent->_pos + _offset;
      }

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
          order_count += 100*(i%3+1); // TODO: improve, will bug if nbuff != 3 or 6
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
  for (size_t dof = 0; dof < _dofs.size(); dof++)
  {
    float dof_value;
    anim_file >> dof_value;
    _dofs[dof]._values.push_back(dof_value);
  }

  // Recursive to childs
  for (size_t ichild = 0; ichild < _childrens.size(); ichild++)
  {
    _childrens[ichild].parse_frames(anim_file);
  }
}

void Bone::create_geometry(BonesInfo info, std::string project_path, int *indice_offset)
// Read bone_inf.json file to assigne the correct geometry to each bone
{
  for (size_t i = 0; i < info.list_info.size(); i++)
  {
    if (info.list_info[i].parent == _name) {

      std::string geometry_file = project_path+"Bones/"+info.list_info[i].name+".off";
      _mesh.create_from_file(geometry_file);
      _mesh.set_id_offset(*indice_offset);
      // for (int i = 0; i < _mesh.n_verts; i++){
      //   _mesh.vertex_list[i].id += *indice_offset;
      // }
      // _mesh.offset_id = *indice_offset;
      // Info_Print("id offset in mesh: "+std::to_string(_mesh.offset_id));
      *indice_offset += _mesh.n_verts;
      break;
    }
  }
  // Recursive child function
  for (size_t ichild = 0; ichild < _childrens.size(); ichild++) {
    _childrens[ichild].create_geometry(info, project_path, indice_offset);
  }
}

void Bone::link_geometry(MatcapRenderer<Geometry>* renderer)
{
  Info_Print("link bone geometry of "+_name);
  renderer->add_object(&_mesh);

  for (size_t ichild = 0; ichild < _childrens.size(); ichild++)
  {
    _childrens[ichild].link_geometry(renderer);
  }

}

void Bone::set_indices(std::vector<int>* indices)
{
  // indices->insert(indices->end(), _mesh.face_indices.begin(), _mesh.face_indices.end());
  // for (int ichild = 0; ichild < _childrens.size() ; ichild++) {
  //   _childrens[ichild].set_indices(indices);
  // }
}

void Bone::get_values_size(int* values_size)
{
  *values_size += _mesh.n_verts;
  for (size_t ichild = 0; ichild < _childrens.size(); ichild++) {
    _childrens[ichild].get_values_size(values_size);
  }
}

void Bone::compute_transform(int frame, mat4 parent_transform)
{
  mat4 localtransform = mat4(1.0);
  // if (frame >= _nb_frames)
  //   frame = _nb_frames-1;

  // translate - la posistion du bone
  vec3 current_trans = _pos;
  // current_trans = vec3(0., 0., 1.0);
  localtransform = translate(localtransform, current_trans);
  if (_dofs.size()==3) // Only rotation
  {
    // rotate the bone
    //
    localtransform = rotate(localtransform, radians(_dofs[0].get_value(frame)), vec3( 1.0, 0.0, 0.0)); //TODO: les stocker en float directement
    localtransform = rotate(localtransform, radians(_dofs[1].get_value(frame)), vec3( 0.0, 1.0, 0.0));
    localtransform = rotate(localtransform, radians(_dofs[2].get_value(frame)), vec3( 0.0, 0.0, 1.0));
  }
  else if (_dofs.size()==6) // Rotation and Translation
  {
    vec3 trans = vec3(_dofs[0].get_value(frame), _dofs[1].get_value(frame), _dofs[2].get_value(frame)) - _offset;
    localtransform = translate(localtransform, trans);
    localtransform = rotate(localtransform, radians(_dofs[3].get_value(frame)), vec3( 1.0, 0.0, 0.0));
    localtransform = rotate(localtransform, radians(_dofs[4].get_value(frame)), vec3( 0.0, 1.0, 0.0));
    localtransform = rotate(localtransform, radians(_dofs[5].get_value(frame)), vec3( 0.0, 0.0, 1.0));
  }
  else if (_dofs.size()==0)
  {
    return;
  }
  else 
  {
    Err_Print("For now, only 3 or 6 Channels or supported (not compatible bvh).\nThe following channels format are actually supported:\n CHANNELS 6 Xposition Yposition Zposition Xrotation Yrotation Zrotation\nCHANNELS 3 Xrotation Yrotation Zrotation", "bone.cpp");
  }
  // translate + la posistion du bone
  localtransform = translate(localtransform, -current_trans);
  
  //
  transformation = parent_transform * localtransform;
  _mesh.set_transform(transformation);
  // Recursive
  for (size_t ichild = 0; ichild < _childrens.size(); ichild++) {
    _childrens[ichild].compute_transform(frame, transformation);
  }
}

void Bone::update_values(std::vector<glm::vert_arr>* values, int frame, bool reset_pose)
  // Update recursively geometry to the VAO for every bones
{
  _mesh.set_transform(transformation);
  // for (int i = 0; i < _mesh.n_verts; i++) {
  //   // values->at(i + _indice_offset) = _mesh.vert_values[i];
  //   if (reset_pose) {
  //     values->at(i + _indice_offset) = _mesh.vert_values[i];
  //   } else {
  //     // TODO: cest transformation devrait être faite sur le GPU
  //     values->at(i + _indice_offset).pos = vec3(transformation * vec4(_mesh.vert_values[i].pos, 1));
  //     values->at(i + _indice_offset).normal = vec3(transformation * vec4(_mesh.vert_values[i].normal, 0));
  //   }
  // }
  // // Recursive
  // for (int ichild = 0; ichild < _childrens.size() ; ichild++) {
  //   _childrens[ichild].update_values(values, frame, reset_pose);
  // }
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
  for (size_t i = 0; i < _childrens.size(); i++) 
  {
    _childrens[i].print_bone(level+1);
  }
}

Bone* Bone::find_bone(std::string bone_name)
{
  if (_name == bone_name)
  {
   return this;
  }
  for (size_t ichild = 0; ichild < _childrens.size(); ichild++)
  {
    Bone* bone;
    bone = _childrens[ichild].find_bone(bone_name);
    if (bone != nullptr){
      return bone;}
  }
  return nullptr;
}

Bone::~Bone()
{
  _childrens.clear();
  _dofs.clear();
}

#endif // !BONE_CPP
