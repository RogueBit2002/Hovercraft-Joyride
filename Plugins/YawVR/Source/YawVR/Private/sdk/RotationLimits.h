#pragma once

class TiltLimits
{
public:
    float yaw, pitchForward, pitchBackward, roll;

    TiltLimits(float pitchForward, float pitchBackward, float roll);
};