#ifndef ppx_camera_h
#define ppx_camera_h

#include "ppx/000_math_config.h"

namespace ppx {

// -------------------------------------------------------------------------------------------------
// Camera
// -------------------------------------------------------------------------------------------------
class Camera
{
public:
    Camera() {}
    Camera(float nearClip, float farClip);
    ~Camera() {}

    void LookAt(const float3& eye, const float3& center, const float3& up = float3(0, 1, 0));

    const float3& GetEyePosition() const { return mEyePosition; }

    const float4x4& GetViewMatrix() const { return mViewMatrix; }
    const float4x4& GetProjectionMatrix() const { return mProjectionMatrix; }
    const float4x4& GetViewProjectionMatrix() const { return mViewProjectionMatrix; }

    float3 WorldToViewPoint(const float3& worldPoint) const;
    float3 WorldToViewVector(const float3& worldVector) const;

protected:
    float            mAspect               = 0;
    float            mNearClip             = 0.1f;
    float            mFarClip              = 10000.0f;
    float3           mEyePosition          = float3(0, 0, 0);
    float3           mLookAt               = float3(0, 0, -1);
    float3           mViewDirection        = float3(0, 0, -1);
    float3           mWorldUp              = float3(0, 1, 0);
    mutable float4x4 mViewMatrix           = float4x4(1);
    mutable float4x4 mProjectionMatrix     = float4x4(1);
    mutable float4x4 mViewProjectionMatrix = float4x4(1);
};

// -------------------------------------------------------------------------------------------------
// PerspCamera
// -------------------------------------------------------------------------------------------------
class PerspCamera : public Camera
{
public:
    PerspCamera();
    PerspCamera(
        float fovDegrees,
        float aspect,
        float nearClip = 0.1f,
        float farClip  = 10000.0f);
    PerspCamera(
        const float3& eye,
        const float3& center,
        const float3& up,
        float         fovDegrees,
        float         aspect,
        float         nearClip = 0.1f,
        float         farClip  = 10000.0f);
    ~PerspCamera();

    void SetPerspective(float fovDegrees, float aspect, float nearClip = 0.1f, float farClip = 10000.0f);

private:
    bool  mPixelAligned = false;
    float mFovDegrees   = 60.0f;
    float mAspect       = 1.0f;
};

// -------------------------------------------------------------------------------------------------
// OrthoCamera
// -------------------------------------------------------------------------------------------------
class OrthoCamera : public Camera
{
public:
    OrthoCamera();
    OrthoCamera(
        float left,
        float right,
        float bottom,
        float top,
        float nearClip,
        float farClip);
    ~OrthoCamera();

    void SetOrthographic(
        float left,
        float right,
        float bottom,
        float top,
        float nearClip,
        float farClip);

private:
    float mLeft   = -1.0f;
    float mRight  = 1.0f;
    float mBottom = -1.0f;
    float mTop    = 1.0f;
};

} // namespace ppx

#endif // ppx_camera_h