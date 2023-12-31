#pragma once

#include "cgp/geometry/shape/mesh/structure/mesh.hpp"
#include "drawable.hpp"
#include "environment.hpp"

const double LOW_POLY_DISTANCE_RATIO = 300;
const int LOW_POLY_RESOLUTION = 10;

/**
 * Low poly abstract drawable object. If the distance if too big,
 * we use this low poly object instead of the real one.
 */
class LowPolyDrawable : public Drawable
{
public:
    LowPolyDrawable(double low_poly_radius);

    virtual void initialize() override;

    virtual void draw(environment_structure const &environment, cgp::vec3 &position, cgp::rotation_transform &rotation, bool show_wireframe = true) override;

    // Draw the real object
    virtual void draw_real(environment_structure const &environment, cgp::vec3 &position, cgp::rotation_transform &rotation, bool show_wireframe = true) = 0;

    // Draw the low poly object
    void draw_low_poly(environment_structure const &environment, cgp::vec3 &position, cgp::rotation_transform &rotation, bool show_wireframe = true);

    // Set low poly color
    virtual void setLowPolyColor(cgp::vec3 color);

    // Setters
    virtual void setPosition(cgp::vec3 position) override;

    // Getters
    cgp::vec3 getPosition() const override;
    bool shouldDrawLowPoly(const cgp::vec3 &position) const;

private:
    // Private : the low poly members do not need to be accessed from children classes
    // Low poly meshes, to display if far enough
    double low_poly_radius;
    cgp::mesh low_poly_mesh;
    cgp::mesh_drawable low_poly_drawable;
    cgp::vec3 low_poly_color;
};