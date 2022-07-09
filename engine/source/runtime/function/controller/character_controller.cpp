#include "runtime/function/controller/character_controller.h"

#include "runtime/core/base/macro.h"

#include "runtime/function/framework/component/motor/motor_component.h"
#include "runtime/function/framework/world/world_manager.h"
#include "runtime/function/global/global_context.h"
#include "runtime/function/physics/physics_scene.h"

namespace Piccolo
{
    CharacterController::CharacterController(const Capsule& capsule) : m_capsule(capsule)
    {
        m_rigidbody_shape                                    = RigidBodyShape();
        m_rigidbody_shape.m_geometry                         = PICCOLO_REFLECTION_NEW(Capsule);
        *static_cast<Capsule*>(m_rigidbody_shape.m_geometry) = m_capsule;

        m_rigidbody_shape.m_type = RigidBodyShapeType::capsule;

        Quaternion orientation;
        orientation.fromAngleAxis(Radian(Degree(90.f)), Vector3::UNIT_X);

        m_rigidbody_shape.m_local_transform =
            Transform(Vector3(0, 0, capsule.m_half_height + capsule.m_radius), orientation, Vector3::UNIT_SCALE);
    }

    Vector3 CharacterController::move(const Vector3& current_position, const Vector3& displacement)
    {
        std::shared_ptr<PhysicsScene> physics_scene =
            g_runtime_global_context.m_world_manager->getCurrentActivePhysicsScene().lock();
        ASSERT(physics_scene);

        Vector3 final_position = current_position + displacement;

        Transform final_transform = Transform(final_position, Quaternion::IDENTITY, Vector3::UNIT_SCALE);

        if (physics_scene->isOverlap(m_rigidbody_shape, final_transform.getMatrix()))
        {
            final_position = current_position;
        }

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
    }
} // namespace Piccolo
