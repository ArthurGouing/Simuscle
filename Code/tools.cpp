#ifndef TOOL_CPP
#define TOOL_CPP

#include "tools.h"


// Printing tools
void Title_Print(std::string msg, bool ansi_color)
{
  if (ansi_color) std::cout << "\033[1;36m*** " << msg << " ***\033[0m"<< std::endl;
  else std::cout << msg << std::endl;
}

void Info_Print(std::string msg)
{
  std::cout << "  " << msg << std::endl;
}

void Warn_Print(std::string msg, bool ansi_color)
{
  if (ansi_color) std::cout << "\033[0;33mWarning: " << msg << "\033[0m"<< std::endl;
  else std::cout << "Warning" << msg << std::endl;
}

void Err_Print(std::string msg, std::string file, bool ansi_color)
{
  if (ansi_color) std::cout << "\n\033[0;31mError in "<< file << ": \n" << msg << "\033[0m" << std::endl;
  else std::cout << "Error: " << msg << " in file " << file << std::endl;
}

void Mat4_Print(std::string msg, glm::mat4 mat, bool ansi_color)
{
  /*
  Print the glm::mat4 matrix with the follwing format.
   msg
   a  b  c  d 
   e  f  g  h
   i  j  k  l
   m  n  o  p
  */
  unsigned short precision = 7;
  std::cout.precision(precision-2);

  std::cout << "  " << msg << std::endl;
  for (int i = 0; i<4*4; i++) {
    if (i%4==0) {
      std::cout << "   ";
    }
    std::cout << std::setw(precision);
    std::cout << mat[i%4][i/4] << " ";
    if (i%4==3) {
      std::cout << std::endl;
    }
  }
}

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
// }
#endif // !TOOL_CPP
