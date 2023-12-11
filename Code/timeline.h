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
#include "Tools/tools.h"

class Timeline
{
  public:
    Timeline();
    Timeline(float fps, int first_frame, int last_frame);
    ~Timeline();

    void time_step();
    void goto_frame(int frame);
    int get_frame(){return _frame;};

    // Play parameter
    bool is_paused;
    void play_button();

    // Loop parameter
    bool is_loop;
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
