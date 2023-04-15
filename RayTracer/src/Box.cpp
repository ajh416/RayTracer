#include "Box.h"

bool Box::Hit(const Ray<float>& r, float tMin, float tMax, float& hitDistance) const
{
    float t1 = (m_Box.pMin.x - r.Origin.x) / r.Direction.x;
    float t2 = (m_Box.pMax.x - r.Origin.x) / r.Direction.x;
    float tmin = std::min(t1, t2);
    float tmax = std::max(t1, t2);

    float ty1 = (m_Box.pMin.y - r.Origin.y) / r.Direction.y;
    float ty2 = (m_Box.pMax.y - r.Origin.y) / r.Direction.y;
    tmin = std::max(tmin, std::min(ty1, ty2));
    tmax = std::min(tmax, std::max(ty1, ty2));

    float tz1 = (m_Box.pMin.z - r.Origin.z) / r.Direction.z;
    float tz2 = (m_Box.pMax.z - r.Origin.z) / r.Direction.z;
    tmin = std::max(tmin, std::min(tz1, tz2));
    tmax = std::min(tmax, std::max(tz1, tz2));

    hitDistance = tmin;
    return tmax >= std::max(0.0f, tmin);
}
