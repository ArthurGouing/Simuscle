#ifndef BONE_CPP
#define BONE_CPP

#include "bone.h"

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
  Info_Print("Before ???");
  while (anim_file.is_open())
  {
    Info_Print("Enter while loop");
    anim_file >> buff; 
    Info_Print(buff);
    switch (find_tok(buff)) {
      case Hierarchy:
        anim_file >> buff;
        Info_Print(buff);
        if(buff != "ROOT") {
          Err_Print("Bad structure of BVH file (line 2: exepted ROOT)", "bone.cpp");
          return;
        }
        anim_file >> buff;
        Info_Print(buff);
        Info_Print("Parse the bone");
        parse_bone(anim_file, buff, nullptr);
        break;
      case Motion:
        parse_frames(anim_file);
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
  Info_Print("Parsing joint "+_name);
  bool parse_end = false;
  while (!parse_end)
  {
    anim_file >> buff;
    Info_Print("Token = "+buff);
    switch (find_tok(buff)) {
      case Offset:
        // store lenth and of the bone
        break;
      case Channels:
        // store dofs
        break;
      case Joint:
        // add a children
        break;
      case End:
        // Last Bone of the chains
        break;
      case Return:
        Info_Print("End the parsing of "+_name);
        parse_end = true;
        return;
      default:
        Err_Print("Error: the token '"+buff+"' is unknow. Reading bvh file cancelled", "bone.cpp");
        return;
    }
  }
}

void Bone::parse_frames(std::ifstream& anim_file)
{
  Info_Print("parse_frames, TODO");
  return;
}

void Bone::animate(float time)
{
  Info_Print("Go to time "+std::to_string(time));
  // For recursive childrens
}

Bone::~Bone()
{
  _childrens.clear();
  // dof.clear();
}

#endif // !BONE_CPP
