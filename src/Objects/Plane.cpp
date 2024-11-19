#include "Plane.h"

Plane::Plane(const Vec3f& point, const Vec3f& normal, int material_index) : Object(point, material_index)
{
    m_PointOnPlane = point;
    m_Normal = normal;
}

bool Plane::Hit(const Ray<float>& r, float tMin, float tMax, float& hitDistance) const
{
    const float denom = Dot(Normalize(m_Normal), Normalize(r.Direction));
    if (denom > 1e-6) {
        //Vec3f p0l0 = Normalize(m_PointOnPlane) - Normalize(r.Origin);
        const Vec3f p0l0 = m_PointOnPlane - r.Origin;
        hitDistance = Dot(p0l0, Normalize(m_Normal)) / denom;
        
        return (hitDistance >= 0);
    }

    return false;
}
