#pragma once

#include "cgp/geometry/vec/vec3/vec3.hpp"
#include "environment.hpp"
#include "utils/display/base_drawable.hpp"

/**
 * Abstract base drawable class for billboards
 */
class BillboardDrawable : public virtual BaseDrawable

{
public:
    // Initialize member meshs
    virtual void initialize(){};

    virtual void setPosition(cgp::vec3 position) = 0;

    // Draw function
    virtual void drawBillboards(environment_structure const &environment, cgp::vec3 &position, cgp::rotation_transform &rotation, bool show_wireframe = true) = 0;
};