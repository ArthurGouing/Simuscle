//**********************************************************************************//
// Simuscle   
// File: tools.cpp
// Parent: all
// Date: 12/09/23
// Content: Tools used by the whole project for printing, ...
//**********************************************************************************//
#ifndef TOOL_H
#define TOOL_H

// Library
#include <iostream>
#include <iomanip>
#include "imgui.h"
#include "Eigen_src/Sparse"
#include "Eigen_src/Dense"
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Printing tools
void Title_Print(std::string msg, bool ansi_color=true);
// void Info_Print(std::string msg);
void Warn_Print(std::string msg, bool ansi_color=true);
void Err_Print(std::string msg, std::string file, bool ansi_color=true);
void Mat4_Print(std::string msg, glm::mat4 mat, bool ansi_color=true);
void Vec3_Print(std::string msg, glm::vec3 vec, bool ansi_color=true);
void Vec3_Draw(glm::vec3 pos, glm::vec3 vector);
void Size_Print(std::string msg, Eigen::MatrixXf mat, bool ansi_color=true);
GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

template <typename T>
void Info_Print(T msg)
{
  std::cout << "  " << msg << std::endl;
};

template <typename T>
void Value_Print(std::string name, T value)
{
  std::cout << "  " << name << ": " << value << std::endl;
};
// ImGui tools
// static void HelpMarker(const char* desc)
// {
//     ImGui::TextDisabled("(?)");
//     if (ImGui::BeginItemTooltip())
//     {
//         ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
//         ImGui::TextUnformatted(desc);
//         ImGui::PopTextWrapPos();
//         ImGui::EndTooltip();
//     }
// };

#endif // !TOOL_H
