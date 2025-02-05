#include "Plane.h"

Plane::Plane(const glm::vec3& point, const glm::vec3& normal, int material_index) : Object(point, material_index)
{
    m_PointOnPlane = point;
    m_Normal = normal;
}

bool Plane::Hit(const Ray& r, float tMin, float tMax, float& hitDistance) const
{
    const float denom = glm::dot(glm::normalize(m_Normal), glm::normalize(r.Direction));
    if (denom > 1e-6) {
        //glm::vec3 p0l0 = Normalize(m_PointOnPlane) - Normalize(r.Origin);
        const glm::vec3 p0l0 = m_PointOnPlane - r.Origin;
        hitDistance = glm::dot(p0l0, glm::normalize(m_Normal)) / denom;
        
        return (hitDistance >= 0);
    }

    return false;
}
