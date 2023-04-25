#pragma once

#include "cgp/cgp.hpp"
#include "environment.hpp"

using cgp::mesh;
using cgp::mesh_drawable;
using cgp::numarray;
using cgp::vec3;

class Navion
{
public:
    void initialize();
    void draw(environment_structure const &environment);
    void set_position(vec3 const &position);
    void set_direction(vec3 const &direction);
    void set_angle_aile(float const angle);
   

protected:
    cgp::hierarchy_mesh_drawable hierarchie;
private:
    float nangle_aile;
    mesh create_cocpit_coque(float const& radius, float const& length);
    
};

// void initialize_navion();
// void bouge_navion();
