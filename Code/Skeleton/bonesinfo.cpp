
#ifndef BONESINFO_CPP
#define BONESINFO_CPP
#include "bonesinfo.h"

// Bones Info
BonesInfo::BonesInfo(std::string file_name)
{
  read_info(file_name);
}
void BonesInfo::read_info(std::string file_name)
{
  std::ifstream info(file_name);
  std::string buff;
  Info_Print("Read '"+file_name+"'");
  info >> buff;
  while(info.is_open())
  {
    Info_struct info_data; 
    info >> buff; // {
    info >> buff;
    if (buff == "\"name\":"){
      info >> buff;
      info_data.name = buff.substr(1, buff.size()-3);
    }
    else if (buff == "]") {
      return;
    }
    else{
      Info_Print("False token ("+buff+")");
      exit(0);
    }
    info >> buff;
    if (buff == "\"parent\":"){
      info >> buff;
      info_data.parent = buff.substr(1, buff.size()-2);
    }
    else{
      Info_Print("False token ("+buff+")");
      exit(0);
    }
    list_info.push_back(info_data);
    info >> buff;
    if (buff == "}") {
      return;
    }
  }
}

void BonesInfo::print()
{
  for (int i = 0; i < list_info.size(); i++)
  {
    Info_Print("Name   : "+list_info[i].name);
    Info_Print("Parent : "+list_info[i].parent);
    Info_Print("");
  }
}

// Destructor
BonesInfo::~BonesInfo()
{}
#endif // !BONESINFO_CPP
