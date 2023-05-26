#pragma once

#include "cgp/graphics/camera/camera_controller/camera_controller_first_person_euler/camera_controller_first_person_euler.hpp"
#include "cgp/graphics/input_devices/input_devices.hpp"
#include "utils/controls/control_constants.hpp"
#include <map>

// TODO : gérer les clics et les mouvements de la souris
/**
Philosophie du contrôle du joueur:

  Z
Q   S : pour orienter la caméra
  W

ESPACE : pour avancer vers l'avant

Attention : comment gérer les keys qui sont pressed en continu, sans être relâchées ? Faire une map / un truc itérable qui contient les keys pressed ?

// TODO : Gérer plusieurs keys en même temps
// TODO : Définir les rotations de la caméra
// TODO : pour le déplacement avant, utiliser l'objet du joueur et sa vitesse, scale dans le référentiel OpenGL

// BUG : on va être obligés d'utiliser une caméra fps. Il faudra passer en arg la position de la caméra et son orientation pour que ça soit "cross camera"
*/

struct KeyEvent
{
    int action;
    int key;
};

// Keyboard control

class Controls
{
public:
    Controls()
    {
        // Initialise key states
        key_states[KEY_Z] = KEY_RELEASED;
        key_states[KEY_Q] = KEY_RELEASED;
        key_states[kEY_S] = KEY_RELEASED;
        key_states[KEY_W] = KEY_RELEASED;
        key_states[KEY_SPACE] = KEY_RELEASED;
    }

    void handleKeyEvent(cgp::input_devices *inputs)
    {
        // Update key states
        key_states[inputs->keyboard.last_action.key] = inputs->keyboard.last_action.action;
    }

    void updateCamera(cgp::camera_controller_first_person_euler &camera);

private:
    std::map<int, int> key_states;
};