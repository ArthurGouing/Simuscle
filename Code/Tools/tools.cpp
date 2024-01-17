#ifndef TOOL_CPP
#define TOOL_CPP

#include "tools.h"


// Printing tools
void Title_Print(std::string msg, bool ansi_color)
{
  if (ansi_color) std::cout << "\033[1;36m*** " << msg << " ***\033[0m"<< std::endl;
  else std::cout << msg << std::endl;
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

void Vec3_Print(std::string msg, glm::vec3 vec, bool ansi_color)
{
  /*
   Print the glm::vec3 vector with the follwing format.
    msg
    x
    y
    z
  */
  unsigned short precision = 7;
  std::cout.precision(precision-2);

  std::cout << "  " << msg << std::endl;
  for (int i = 0; i<3; i++) {
    std::cout << std::setw(precision);
    std::cout << vec[i] << std::endl;
  }
}

void Size_Print(std::string msg, Eigen::MatrixXf mat, bool ansi_color)
{
  std::cout << msg << "size->"<< mat.size()<<" ("<<mat.rows() <<", "<<mat.cols()<<")"<< std::endl;
}

void Vec3_Draw(glm::vec3 pos, glm::vec3 vector)
{
  unsigned int VAO, VBO;
  std::vector<float> vertices = {
       pos.x, pos.y, pos.z,
       pos.x + vector.x , pos.y + vector.y, pos.z + vector.z,
  };
  
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
 
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
 
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
 
  glBindBuffer(GL_ARRAY_BUFFER, 0); 
  glBindVertexArray(0); 
 
  glBindVertexArray(VAO);
  glDisable(GL_DEPTH_TEST);
  glDrawArrays(GL_LINES, 0, 2);
  glEnable(GL_DEPTH_TEST);
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

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            // case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            // case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}

#endif // !TOOL_CPP
