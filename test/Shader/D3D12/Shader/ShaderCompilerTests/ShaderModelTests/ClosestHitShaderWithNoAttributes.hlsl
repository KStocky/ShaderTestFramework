struct RayPayload
{
    float Dist;
};

[shader("closesthit")]
void Hit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    payload.Dist = RayTCurrent();
}