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
#include <glm/glm.hpp>

// Printing tools
void Title_Print(std::string msg, bool ansi_color=true);
void Info_Print(std::string msg);
void Warn_Print(std::string msg, bool ansi_color=true);
void Err_Print(std::string msg, std::string file, bool ansi_color=true);
void Mat4_Print(std::string msg, glm::mat4 mat, bool ansi_color=true);

// ImGui tools
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
};

#endif // !TOOL_H
