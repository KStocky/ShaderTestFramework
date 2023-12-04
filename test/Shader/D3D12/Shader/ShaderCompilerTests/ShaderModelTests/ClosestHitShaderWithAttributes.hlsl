struct [raypayload] RayPayload
{
    float Dist : read(caller) : write(closesthit);
};

[shader("closesthit")]
void Hit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    payload.Dist = RayTCurrent();
}