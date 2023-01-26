#include "Box.h"

bool Box::Hit(const Ray<Float>& r, Float tMin, Float tMax, Float& hitDistance) const
{
    double t1 = (m_Box.pMin.x - r.Origin.x) / r.Direction.x;
    double t2 = (m_Box.pMax.x - r.Origin.x) / r.Direction.x;
    double tmin = std::min(t1, t2);
    double tmax = std::max(t1, t2);

    double ty1 = (m_Box.pMin.y - r.Origin.y) / r.Direction.y;
    double ty2 = (m_Box.pMax.y - r.Origin.y) / r.Direction.y;
    tmin = std::max(tmin, std::min(ty1, ty2));
    tmax = std::min(tmax, std::max(ty1, ty2));

    double tz1 = (m_Box.pMin.z - r.Origin.z) / r.Direction.z;
    double tz2 = (m_Box.pMax.z - r.Origin.z) / r.Direction.z;
    tmin = std::max(tmin, std::min(tz1, tz2));
    tmax = std::min(tmax, std::max(tz1, tz2));

    hitDistance = tmin;
    return tmax >= std::max(0.0, tmin);
}
