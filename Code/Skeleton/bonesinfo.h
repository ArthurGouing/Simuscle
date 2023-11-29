//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: bonesinfo.cpp
// Parent: Bone and Skeleton
// Date: 23/11/23
// Content: Read the bone file 
// //**********************************************************************************//
#ifndef BONESINFO_H
#define BONESINFO_H

//******** LIBRARY ******** 
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Tools/tools.h"

class BonesInfo
{
  public:
    BonesInfo(std::string file_name);
    ~BonesInfo();
    void read_info(std::string file_name);
    void print();
    struct Info_struct
    {
      std::string name;
      std::string parent;
    };
    std::vector<Info_struct> list_info;
};

#endif // !BONESINFO
