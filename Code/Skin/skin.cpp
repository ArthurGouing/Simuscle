#ifndef SKIN_CPP
#define SKIN_CPP

#include "skin.h"

Skin::Skin(std::string project, MatcapRenderer<Geometry>* skin_renderer, Skeleton* skel, MuscleSystem* musc):
  name("Skin"), _skel(skel), _musc(musc), _renderer(skin_renderer)
{
  Title_Print("Init Skin");

  /******** Load Skin Gemetry ********/
  Info_Print("Loading skin geometrie: ");
  std::string geometry_file = project+"Skin/skin.off";
  _mesh.create_from_file(geometry_file);

  // Link geometry to the renderer
  _renderer->add_object(&_mesh);

  /******** Read weight paint ********/
  std::string weight_file = project+"Skin/weight.off";
  // read_weights(weight_file);

  /******** Simulation mask ********/
  std::string mask_file = project+"Skin/mask.off";
  // read_mask(mask_file);
  
  /******** Init skin Solver ********/
  // _solver.Init();
}

void Skin::read_weights(std::string fileName)
{
  bool hard_mode = false; // Pour débug

	std::cout << "Reading weight file"<< fileName <<" ..." << std::endl;
  std::fstream file(fileName);
  std::string line;
	std::getline(file, line);
  int i = 0;
  while (std::getline(file, line)) {
    float weight;
	  float weight_max=0.;
	  int k_max=0;
	  int k=0;
	  std::vector<float> weight_line;
    std::stringstream ss(line);
    //weight_model.push_back(std::vector<float>());
	  ss >> weight;
    while (ss >> weight) {
	    if ((hard_mode)&(weight > weight_max)) { 
		    weight_max = weight;
		    k_max = k;
		  }
		  else if (!hard_mode){
        weight_line.push_back(weight);
		  }
		k++;
    }
	  //std::cout << k_max <<" : "<<weight_max<<std::endl;
	  if (hard_mode) {
	  	weight_line.resize(32);
	  	weight_line[k_max] = 1;
	  }
	  weights.push_back(weight_line);
	  i++;
  }
	std::cout << "fin read"<< std::endl;
}

void Skin::solve(int frame)
{
  // return a deformation to apply
}

Skin::~Skin()
{

}


#endif // !SKIN_CPP
