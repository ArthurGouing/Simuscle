#ifndef TIMELINE_CPP
#define TIMELINE_CPP

//******** LIBRARY ******** 
#include "timeline.h"
#include <string>

Timeline::Timeline() :
  is_paused(true), is_loop(false),
  _frame(0), _time(0.0f),
  _dtframe(1.0f/24.0f), _fps(24.0f),
  _first_frame(0), _last_frame(120)
{
}

Timeline::Timeline(float fps, int first_frame, int last_frame):
  _frame(first_frame), _fps(fps),
  _first_frame(first_frame), _last_frame(last_frame)
{
  _dtframe   = 1.0f/fps;
  _time = _frame * _dtframe; 
  // TODO: au lieux de stocker une variable time, qu'il faut update à chaque fois
  // que la frame est modifié, il serait mieux de faire une fonction get time,
  // qui calcule time en fonction de frame quand on en a besoin ?
}

Timeline::~Timeline()
{
  //TODO: destructeur (a priori ok car pas de array)
}

void Timeline::time_step()
{
  if (is_paused) 
  {
    return;
  }

  if (_frame==_last_frame) {
    if(is_loop){
      _frame = _first_frame;
      _time = _frame * _dtframe;
    } else {
      return;
    }
  } else {
  _frame++;
  _time = _frame * _dtframe;
  }
}

void Timeline::goto_frame(int frame)
{
  _frame = frame;
  _time  = frame * _dtframe;
}

void Timeline::play_button()
{
  Info_Print("is_paused: " + std::to_string(is_paused));
  if (is_paused) {
    is_paused = false;
  } else {
    is_paused = true;
  }
}

void Timeline::UI_pannel()
{
  ImGui::Begin("Timeline");
  if (ImGui::Button("Play/Pause"))
    play_button();
  if (ImGui::Button("Loop")){
    if (is_loop){
      is_loop=false;
    } else {
      is_loop=true;
    }
  }
  ImGui::InputInt("Frame", &_frame); //, 1, _first_frame, _last_frame);
  // ImGui::SeparatorText("frame n°X");
  // int n_frame = _last_frame - _first_frame; 
  // float arr[n_frame] = { };
  // arr[_frame] = 10;
  // ImGui::PushItemWidth(-FLT_MIN);
  // ImGui::PlotHistogram("Current Frame", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 150.0f)); 


  ImGui::End();
}

#endif // !TIMELINE_CPP
