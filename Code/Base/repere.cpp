#ifndef REPERE_CPP
#define REPERE_CPP

using namespace glm;


Repere::Repere() : _pos(0.f), _rot(1.0), _transformation(1.0f)

Repere::Repere(vec3 init_pos, mat3 init_rot): _pos(init_pos), _rot(init_rot), _transformation(1.0f)
{
  // build transformation matrix
  compute_transforms();
}
Repere::Repere(vec3 init_pos, float rot_x, float rot_y, float rot_z): _pos(init_pos), _rot(1.0f), _transformation(1.0f)
{
  // Build rotation matrix
  _rot = rotate(rot, rot_x, vec3(1.f, 0.f, 0.f));
  _rot = rotate(rot, rot_y, vec3(0.f, 1.f, 0.f));
  _rot = rotate(rot, rot_z, vec3(0.f, 0.f, 1.f));
  compute_transforms();
}


Repere operator=(Repere const& R1)
{
  _pos = R1._pos;
  _rot = R1._rot;
  compute_transforms();
}
Repere operator+(Repere const& R1);
{
  _pos += R1._pos;
  _rot = R1._rot * _rot;
  compute_transforms();
}
Repere operator-(Repere const& R1);
{
  _pos -= R1._pos;
  _rot = transpose(R1._rot) * _rot;
  compute_transforms();
}


void Repere::set_pos(vec3 new_pos)  {pos = new_pos; compute_transforms();}

void Repere::set_rot(mat3 new_rot)  {rot = new_rot; compute_transforms();}
void Repere::set_rot(float rot_x, float rot_y, float r_z)  // TODO je peux template car rotate est tanplaté sur rot_x, rot_y, rot_z
{
  rot = rot(1.0f);
  rot = rotate(rot, rot_x, vec3(1.f, 0.f, 0.f));
  rot = rotate(rot, rot_y, vec3(0.f, 1.f, 0.f));
  rot = rotate(rot, rot_z, vec3(0.f, 0.f, 1.f));
  compute_transforms();
}
void Repere::compute_transforms()
{
  _transformation = translate(_transformation, _pos) * mat3(_rot)
  _inv_transformation = mat4(transpose(_rot)) * translate(_inv_transformation, -_pos);
}


vec3 Repere::local_to_world(vec3 local_point)
{
  return _transformation * local_point;
}


vec3 Repere::world_to_local(vec3 world_point)
{
  return _inv_transformation * world_point;
}

Repere::~Repere() {}

#endif // !REPERE_CPP
