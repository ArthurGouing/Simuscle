//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: line.cpp
// Parent: None
// Date: 21/12/23
// Content: Tool qui permet d'afficher une ligne sur l'écran
//**********************************************************************************//
#ifndef LINE_H
#define LINE_H

#include <iostream>
#include <vector>
#include <math.h>
 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
 
using glm::mat4;
using glm::vec3;
using glm::radians;
using glm::lookAt;
using std::vector;


// settings
class Line {
    int shaderProgram;
    unsigned int VBO, VAO;
    vector<float> vertices;
    vec3 lineColor;
public:
    Line(vec3 pos, vec3 vector); 
    int setColor(vec3 color) {
        lineColor = color;
        return 1;
    }

    int setVector(vec3 pos, vec3 vector)
    {
       vertices = {
            pos.x, pos.y, pos.z,
            pos.x + vector.x, pos.y + vector.y, pos.z + vector.z
       };

       glBindBuffer(GL_ARRAY_BUFFER, VBO);
       glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
 
       glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
       glEnableVertexAttribArray(0);
 
       glBindBuffer(GL_ARRAY_BUFFER, 0); 
       glBindVertexArray(0); 
    }
 
    int draw(glm::mat4 VPMat);
 
    ~Line() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    }
};

#endif // !LINE_H
