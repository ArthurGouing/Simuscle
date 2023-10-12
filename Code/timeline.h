//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: timeline.cpp
// Parent: app, rig and other simulation classes
// Date: 02/10/23
// Content: File which track the time on frame (and inter-frame? for simulation dt ?)
//**********************************************************************************//
#ifndef TIMELINE_H
#define TIMELINE_H

#include "imgui.h"

class Timeline
{
  public:
    Timeline();
    Timeline(float fps, int first_frame, int last_frame);
    ~Timeline();

    void time_step();
    void goto_frame(int frame);

    bool is_paused;

    // User interface 
    void UI_pannel();

  private:
    // Variable
    int   _frame;
    float _time;
    float _dtframe, _fps;

    int _first_frame, _last_frame;

};
#endif // !TIMELINE_H
