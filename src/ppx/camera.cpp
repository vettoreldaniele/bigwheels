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
    mLookAt               = center;
    mWorldUp              = up;
    mViewDirection        = glm::normalize(mEyePosition - mLookAt);
    mViewMatrix           = glm::lookAt(mEyePosition, mLookAt, mWorldUp);
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

void Camera::MoveAlongViewDirection(float distance)
{
    float3 eyePosition = mEyePosition + (distance * -mViewDirection);
    LookAt(eyePosition, mLookAt, mWorldUp);
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

// -------------------------------------------------------------------------------------------------
// ArcballCamera
// -------------------------------------------------------------------------------------------------
ArcballCamera::ArcballCamera()
{
    LookAt(float3(0, 0, 1), float3(0, 0, 0), float3(0, 1, 0));
}

ArcballCamera::ArcballCamera(const float3& eye, const float3& center, const float3& up)
{
    LookAt(eye, center, up);
}

void ArcballCamera::UpdateCamera()
{
    mCameraMatrix        = mTranslationMatrix * glm::mat4_cast(mRotationQuat) * mCenterTranslationMatrix;
    mInverseCameraMatrix = glm::inverse(mCameraMatrix);
}

void ArcballCamera::LookAt(const float3& eye, const float3& center, const float3& up)
{
    mEyePosition   = eye;
    mViewDirection = center - eye;
    mUpDirection   = up;
    float3 zAxis   = glm::normalize(mViewDirection);
    float3 xAxis   = glm::normalize(glm::cross(zAxis, glm::normalize(up)));
    float3 yAxis   = glm::normalize(glm::cross(xAxis, zAxis));
    xAxis          = glm::normalize(glm::cross(zAxis, yAxis));

    mCenterTranslationMatrix = glm::inverse(glm::translate(center));
    mTranslationMatrix       = glm::translate(float3(0.0f, 0.0f, -glm::length(mViewDirection)));
    mRotationQuat            = glm::normalize(glm::quat_cast(glm::transpose(glm::mat3(xAxis, yAxis, -zAxis))));

    UpdateCamera();
}

static quat ScreenToArcball(const float2& p)
{
    float dist = glm::dot(p, p);

    // If we're on/in the sphere return the point on it
    if (dist <= 1.0f) {
        return glm::quat(0.0f, p.x, p.y, glm::sqrt(1.0f - dist));
    }

    // Otherwise we project the point onto the sphere
    const glm::vec2 proj = glm::normalize(p);
    return glm::quat(0.0f, proj.x, proj.y, 0.0f);
}

void ArcballCamera::Rotate(const float2& prevPos, const float2& curPos)
{
    const float2 kNormalizeDeviceCoordinatesMin = float2(-1, -1);
    const float2 kNormalizeDeviceCoordinatesMax = float2(1, 1);

    // Clamp mouse positions to stay in NDC
    float2 clampedCurPos  = glm::clamp(curPos, kNormalizeDeviceCoordinatesMin, kNormalizeDeviceCoordinatesMax);
    float2 clampedPrevPos = glm::clamp(prevPos, kNormalizeDeviceCoordinatesMin, kNormalizeDeviceCoordinatesMax);

    quat mouseCurBall  = ScreenToArcball(clampedCurPos);
    quat mousePrevBall = ScreenToArcball(clampedPrevPos);

    mRotationQuat = mouseCurBall * mousePrevBall * mRotationQuat;

    UpdateCamera();
}

void ArcballCamera::Pan(const float2& delta)
{
    float  zoomAmount = glm::abs(mTranslationMatrix[3][2]);
    float4 motion     = float4(delta.x * zoomAmount, delta.y * zoomAmount, 0.0f, 0.0f);

    // Find the panning amount in the world space
    motion = mInverseCameraMatrix * motion;

    mCenterTranslationMatrix = glm::translate(float3(motion)) * mCenterTranslationMatrix;

    UpdateCamera();
}

void ArcballCamera::Zoom(float amount)
{
    float3 motion = float3(0.0f, 0.0f, amount);

    mTranslationMatrix = glm::translate(motion) * mTranslationMatrix;

    UpdateCamera();
}

} // namespace ppx
