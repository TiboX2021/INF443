#include "simulation_handler.hpp"
#include "background/galaxy.hpp"
#include "celestial_bodies/planet/planet.hpp"
#include "utils/display/base_drawable.hpp"
#include "utils/noise/perlin.hpp"
#include "utils/physics/constants.hpp"
#include <iostream>
#include <iterator>

template <typename TExtendsBaseDrawable>
void SimulationHandler::addObject(TExtendsBaseDrawable drawable)
{
    // Create unique_ptr
    std::unique_ptr<BaseDrawable> drawable_unique_ptr = std::make_unique<TExtendsBaseDrawable>(drawable);

    // Move the unique_ptr (its adress changes !)
    drawables.push_back(std::move(drawable_unique_ptr));

    // Get the memory adress from the vector pointer (while still keeping polymorphism !)
    BaseDrawable *ptr = drawables.back().get();

    // Add reference to the object in the corresponding vectors
    if (dynamic_cast<Drawable *>(ptr))
    {
        drawable_objects.push_back(dynamic_cast<Drawable *>(ptr));
    }
    if (dynamic_cast<BillboardDrawable *>(ptr))
    {
        billboard_drawable_objects.push_back(dynamic_cast<BillboardDrawable *>(ptr));
    }
    if (dynamic_cast<Object *>(ptr))
    {
        physical_objects.push_back(dynamic_cast<Object *>(ptr));
    }
}

void SimulationHandler::drawObjects(environment_structure const &environment, camera_controller_orbit_euler const &camera, bool show_wireframe)
{
    for (auto &drawable : drawable_objects)
    {
        drawable->draw(environment, camera, show_wireframe);
    }
}

void SimulationHandler::drawBillboards(environment_structure const &environment, camera_controller_orbit_euler const &camera, bool show_wireframe)
{
    for (auto &drawable : billboard_drawable_objects)
    {
        drawable->drawBillboards(environment, camera, show_wireframe);
    }
}

void SimulationHandler::simulateStep()
{
    // Clear forces
    for (auto &object : physical_objects)
    {
        object->resetForces();
    }

    // Iterate through all pairs of objects to compute forces
    for (auto it = physical_objects.begin(); it != physical_objects.end(); ++it)
    {
        for (auto it2 = it + 1; it2 != physical_objects.end(); ++it2)
        {
            // Compute forces between both objects
            (*it)->computeGravitationnalForce(*it2);
            (*it2)->computeGravitationnalForce(*it);
        }
    }

    // Update objects
    for (auto &object : physical_objects)
    {
        object->update(time_step);
        object->updateModels();
    }
}

void SimulationHandler::initialize()
{
    galaxy.initialize();
    for (auto &drawable : drawables)
    {
        drawable->initialize();
    }
}

void SimulationHandler::generateSolarSystem(SimulationHandler &handler)
{
    // Add galaxy first (background)
    Galaxy galaxy;
    handler.addObject(galaxy);

    // Add sun
    Planet sun(SUN_MASS, SUN_RADIUS, {0, 0, 0}, "assets/planets/sun.jpg", NO_PERLIN_NOISE);
    sun.setShouldRotate(false);
    sun.setShouldTranslate(false);
    handler.addObject(sun);

    // Add Earth
    Planet earth(EARTH_MASS, EARTH_RADIUS, {EARTH_SUN_DISTANCE, 0, 0}, "assets/planets/earth.jpg", NO_PERLIN_NOISE);
    earth.setLowPolyColor({32.0f / 255, 60.0f / 255, 74.0f / 255});
    earth.setInitialVelocity({0, Object::computeOrbitalSpeed(SUN_MASS, EARTH_SUN_DISTANCE), 0});
    earth.setInitialRotationSpeed(EARTH_ROTATION_SPEED);
    earth.setRotationAxis(EARTH_ROTATION_AXIS);
    handler.addObject(earth);

    // TODO : vérifier si ça tourne bien dans le bon sens
    // TODO mars
    // TODO jupiter

    // Add Saturn
    Planet saturn(SATURN_MASS, SATURN_RADIUS, {SATURN_SUN_DISTANCE, 0, 0}, "assets/planets/saturn.jpg", NO_PERLIN_NOISE);
    saturn.setLowPolyColor({207.0f / 255, 171.0f / 255, 134.0f / 255});
    saturn.setInitialVelocity({0, Object::computeOrbitalSpeed(SUN_MASS, SATURN_SUN_DISTANCE), 0});
    saturn.setInitialRotationSpeed(SATURN_ROTATION_SPEED);
    saturn.setRotationAxis(SATURN_ROTATION_AXIS);
    handler.addObject(saturn);

    // Add jupiter
    Planet jupiter;
    // TODO;
    handler.addObject(jupiter);
}