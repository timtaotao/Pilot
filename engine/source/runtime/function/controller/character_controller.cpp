#include "runtime/function/controller/character_controller.h"

#include "runtime/function/framework/component/motor/motor_component.h"
#include "runtime/function/physics/physics_system.h"

namespace Pilot
{
    Controller::~Controller() = default;

    CharacterController::~CharacterController() {}

    Vector3 CharacterController::move(const Vector3& current_position, const Vector3& displacement)
    {
        Vector3 desired_position = current_position + displacement;
        if (PhysicsSystem::getInstance().overlapByCapsule(desired_position, m_capsule))
        {
            desired_position = current_position;
        }

<<<<<<< Updated upstream
        return desired_position;
=======
        hits.clear();

        // side pass
        if (physics_scene->sweep(
            m_rigidbody_shape,
            world_transform.getMatrix(),
            horizontal_direction,
            horizontal_direction.length() * 0.1f,
            hits))
        {
            Vector3 hit_tangent_1 = Vector3(hits[0].hit_normal.y, -hits[0].hit_normal.x, 0.f);
            Vector3 hit_tangent_2 = Vector3(-hits[0].hit_normal.y, hits[0].hit_normal.x, 0.f);
            Vector3 hit_tangent   = hit_tangent_1;

            if (hit_tangent_1.angleBetween(horizontal_direction) > hit_tangent_2.angleBetween(horizontal_direction))
            {
                hit_tangent = hit_tangent_2;
            }
            final_position += hits[0].hit_distance * hit_tangent * 0.1f; 
        }
        else
        {
            final_position += horizontal_displacement;
        }

        hits.clear();

        return final_position;
>>>>>>> Stashed changes
    }
} // namespace Pilot
