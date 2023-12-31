#include "simulation_handler.hpp"
#include "background/galaxy.hpp"
#include "celestial_bodies/asteroid_belt/asteroid_belt.hpp"
#include "celestial_bodies/overrides/star.hpp"
#include "celestial_bodies/planet/planet.hpp"
#include "utils/display/base_drawable.hpp"
#include "utils/noise/perlin.hpp"
#include "utils/physics/constants.hpp"
#include "utils/physics/object.hpp"
#include <iostream>
#include <iterator>
#include <memory>

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

void SimulationHandler::drawObjects(environment_structure const &environment, cgp::vec3 &position, cgp::rotation_transform &rotation, bool show_wireframe)
{
    for (auto &drawable : drawable_objects)
    {
        drawable->draw(environment, position, rotation, show_wireframe);
    }

    for (auto &belt : asteroid_belts)
    {
        belt.draw(environment, position, rotation, show_wireframe);
    }
}

void SimulationHandler::drawBillboards(environment_structure const &environment, cgp::vec3 &position, cgp::rotation_transform &rotation, bool show_wireframe)
{
    for (auto &drawable : billboard_drawable_objects)
    {
        drawable->drawBillboards(environment, position, rotation, show_wireframe);
    }
}

void SimulationHandler::simulateStep(float time_step)
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
        object->update(time_step * time_step_multiplier);
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

    for (auto &belt : asteroid_belts)
    {
        belt.initialize();
    }
}

void SimulationHandler::generateSolarSystem(SimulationHandler &handler)
{
    // Add galaxy first (background)
    Galaxy galaxy;
    handler.addObject(galaxy);

    // Add sun
    Star sun(SUN_MASS, SUN_RADIUS / 10, {0, 0, 0}, "assets/planets/sun.jpg", NO_PERLIN_NOISE);
    sun.setShouldRotate(false);
    sun.setShouldTranslate(false);
    sun.setShader("lava");
    sun.setPhysicsRadius(SUN_RADIUS / 10 * DISPLAY_SCALE); // For asteroid collisions
    handler.addObject(sun);

    Object *sun_ptr = handler.physical_objects.back();

    AsteroidBelt solar_asteroid_belt(BeltPresets::SUN);
    solar_asteroid_belt.addAttractor(sun_ptr);
    handler.addAsteroidBelt(solar_asteroid_belt);

    AsteroidBelt kuiper_belt(BeltPresets::KUIPER);
    kuiper_belt.addAttractor(sun_ptr);
    handler.addAsteroidBelt(kuiper_belt);

    // Add Earth
    Planet earth(EARTH_MASS, EARTH_RADIUS, {EARTH_SUN_DISTANCE, 0, 0}, "assets/planets/earth.jpg", NO_PERLIN_NOISE);
    earth.setLowPolyColor({32.0f / 255, 60.0f / 255, 74.0f / 255});
    earth.setVelocity({0, Object::computeOrbitalSpeed(SUN_MASS, EARTH_SUN_DISTANCE), 0});
    earth.setInitialRotationSpeed(EARTH_ROTATION_SPEED);
    earth.setRotationAxis(EARTH_ROTATION_AXIS);
    earth.setPhysicsRadius(EARTH_RADIUS * DISPLAY_SCALE); // For player collision
    handler.addObject(earth);

    // Add Mars
    Planet mars(MARS_MASS, MARS_RADIUS, {MARS_SUN_DISTANCE, 0, 0}, "assets/planets/mars.jpg", NO_PERLIN_NOISE);
    mars.setLowPolyColor({181.0 / 255, 99.0 / 255, 73.0 / 255});
    mars.setVelocity({0, Object::computeOrbitalSpeed(SUN_MASS, MARS_SUN_DISTANCE), 0});
    mars.setInitialRotationSpeed(MARS_ROTATION_SPEED);
    mars.setRotationAxis(MARS_ROTATION_AXIS);
    mars.setPhysicsRadius(MARS_RADIUS * DISPLAY_SCALE); // For player collisions
    handler.addObject(mars);

    // Add Saturn
    Planet saturn(SATURN_MASS, SATURN_RADIUS, {SATURN_SUN_DISTANCE, 0, 0}, "assets/planets/saturn.jpg", NO_PERLIN_NOISE);
    saturn.setLowPolyColor({207.0f / 255, 171.0f / 255, 134.0f / 255});
    saturn.setVelocity({0, Object::computeOrbitalSpeed(SUN_MASS, SATURN_SUN_DISTANCE), 0});
    saturn.setInitialRotationSpeed(SATURN_ROTATION_SPEED);
    saturn.setRotationAxis(SATURN_ROTATION_AXIS);
    saturn.setPhysicsRadius(SATURN_RADIUS * DISPLAY_SCALE); // For asteroid collisions
    handler.addObject(saturn);

    Object *saturn_ptr = handler.physical_objects.back();

    // Add asteroid belt around saturn
    AsteroidBelt saturnBelt(BeltPresets::SATURN);
    saturnBelt.addAttractor(saturn_ptr); // Add main attractor first
    handler.addAsteroidBelt(saturnBelt);

    // Add jupiter
    Planet jupiter(JUPITER_MASS, JUPITER_RADIUS, {JUPITER_SUN_DISTANCE, 0, 0}, "assets/planets/jupiter.jpg", NO_PERLIN_NOISE);
    jupiter.setLowPolyColor({161.0 / 255, 150.0 / 255, 132.0 / 255});
    jupiter.setVelocity({0, Object::computeOrbitalSpeed(SUN_MASS, JUPITER_SUN_DISTANCE), 0});
    jupiter.setInitialRotationSpeed(JUPITER_ROTATION_SPEED);
    jupiter.setRotationAxis(JUPITER_ROTATION_AXIS);
    jupiter.setPhysicsRadius(JUPITER_RADIUS * DISPLAY_SCALE); // For player collisions
    handler.addObject(jupiter);

    // Add Uranus
    Planet uranus(URANUS_MASS, URANUS_RADIUS, {URANUS_SUN_DISTANCE, 0, 0}, "assets/planets/uranus.jpg", NO_PERLIN_NOISE);
    uranus.setLowPolyColor({155.0 / 255, 202.0 / 255, 209.0 / 255});
    uranus.setVelocity({0, Object::computeOrbitalSpeed(SUN_MASS, URANUS_SUN_DISTANCE), 0});
    uranus.setInitialRotationSpeed(URANUS_ROTATION_SPEED);
    uranus.setRotationAxis(URANUS_ROTATION_AXIS);
    uranus.setPhysicsRadius(URANUS_RADIUS * DISPLAY_SCALE); // For player collisions
    handler.addObject(uranus);

    // Add Neptune
    Planet neptune(NEPTUNE_MASS, NEPTUNE_RADIUS, {NEPTUNE_SUN_DISTANCE, 0, 0}, "assets/planets/neptune.jpg", NO_PERLIN_NOISE);
    neptune.setLowPolyColor({54.0 / 255, 79.0 / 255, 167.0 / 255});
    neptune.setVelocity({0, Object::computeOrbitalSpeed(SUN_MASS, NEPTUNE_SUN_DISTANCE), 0});
    neptune.setInitialRotationSpeed(NEPTUNE_ROTATION_SPEED);
    neptune.setRotationAxis(NEPTUNE_ROTATION_AXIS);
    neptune.setPhysicsRadius(NEPTUNE_RADIUS * DISPLAY_SCALE); // For player collisions
    handler.addObject(neptune);
}

void SimulationHandler::addAsteroidBelt(AsteroidBelt asteroid_belt)
{
    asteroid_belts.push_back(asteroid_belt);
}

std::vector<Object *> SimulationHandler::getPhysicalObjects() const
{
    return physical_objects;
}
