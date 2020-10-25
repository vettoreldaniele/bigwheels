#include "ppx/camera.h"

namespace ppx {

// -------------------------------------------------------------------------------------------------
// Camera
// -------------------------------------------------------------------------------------------------
Camera::Camera(float nearClip, float farClip)
    : mNearClip(nearClip),
      mFarClip(farClip)
{
}

void Camera::LookAt(const float3& eye, const float3& center, const float3& up)
{
    mEyePosition          = eye;
    mViewMatrix           = glm::lookAt(mEyePosition, center, up);
    mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
}

float3 Camera::WorldToViewPoint(const float3& worldPoint) const
{
    float3 viewPoint = float3(mViewMatrix * float4(worldPoint, 1.0f));
    return viewPoint;
}

float3 Camera::WorldToViewVector(const float3& worldVector) const
{
    float3 viewPoint = float3(mViewMatrix * float4(worldVector, 0.0f));
    return viewPoint;
}

// -------------------------------------------------------------------------------------------------
// PerspCamera
// -------------------------------------------------------------------------------------------------
PerspCamera::PerspCamera()
{
}

PerspCamera::PerspCamera(
    float fovDegrees,
    float aspect,
    float nearClip,
    float farClip)
    : Camera(nearClip, farClip)
{
    SetPerspective(
        fovDegrees,
        aspect,
        nearClip,
        farClip);
}

PerspCamera::PerspCamera(
    const float3& eye,
    const float3& center,
    const float3& up,
    float         fovDegrees,
    float         aspect,
    float         nearClip,
    float         farClip)
    : Camera(nearClip, farClip)
{
    LookAt(eye, center, up);
    SetPerspective(
        fovDegrees,
        aspect,
        nearClip,
        farClip);
}

PerspCamera::~PerspCamera()
{
}

void PerspCamera::SetPerspective(float fovDegrees, float aspect, float nearClip, float farClip)
{
    mFovDegrees = fovDegrees;
    mAspect     = aspect;
    mNearClip   = nearClip;
    mFarClip    = farClip;

    mProjectionMatrix = glm::perspective(
        glm::radians(mFovDegrees),
        mAspect,
        mNearClip,
        mFarClip);

    mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
}

// -------------------------------------------------------------------------------------------------
// OrthoCamera
// -------------------------------------------------------------------------------------------------
OrthoCamera::OrthoCamera()
{
}

OrthoCamera::OrthoCamera(
    float left,
    float right,
    float bottom,
    float top,
    float near_clip,
    float far_clip)
{
    SetOrthographic(
        left,
        right,
        bottom,
        top,
        near_clip,
        far_clip);
}

OrthoCamera::~OrthoCamera()
{
}

void OrthoCamera::SetOrthographic(
    float left,
    float right,
    float bottom,
    float top,
    float nearClip,
    float farClip)
{
    mLeft     = left;
    mRight    = right;
    mBottom   = bottom;
    mTop      = top;
    mNearClip = nearClip;
    mFarClip  = farClip;

    mProjectionMatrix = glm::ortho(
        mLeft,
        mRight,
        mBottom,
        mTop,
        mNearClip,
        mFarClip);
}

} // namespace ppx
