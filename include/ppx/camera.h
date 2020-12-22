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

    void MoveAlongViewDirection(float distance);

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

// -------------------------------------------------------------------------------------------------
// ArcballCamera
// -------------------------------------------------------------------------------------------------

//! @class ArcballCamera
//!
//! Adapted from: https://github.com/Twinklebear/arcball-cpp
//!
class ArcballCamera
{
public:
    ArcballCamera();
    ArcballCamera(const float3& eye, const float3& center, const float3& up);
    ~ArcballCamera() {}

    void LookAt(const float3& eye, const float3& center, const float3& up);

    //! @fn void Rotate(const float2& prevPos, const float2& curPos)
    //!
    //! @param prevPos previous mouse position in normalized device coordinates
    //! @param curPos current mouse position in normalized device coordinates
    //!
    void Rotate(const float2& prevPos, const float2& curPos);

    //! @fn void Pan(const float2& delta)
    //! 
    //! @param delta mouse delta in normalized device coordinates
    //! 
    void Pan(const float2& delta);

    //! @fn void Zoom(float amount)
    //!
    void Zoom(float amount);

    //! @fn const float4x4& GetCameraMatrix() const
    //!
    const float4x4& GetCameraMatrix() const { return mCameraMatrix; }

    //! @fn const float4x4& GetInverseCameraMatrix() const
    //!
    const float4x4& GetInverseCameraMatrix() const { return mInverseCameraMatrix; }

    //! @fn float3 GetEyePosition() const
    //!
    const float3& GetEyePosition() const { return mEyePosition; }

    //! @fn float3 GetViewDirection() const
    //!
    const float3& GetViewDirection() const { return mViewDirection; }

    //! @fn float3 GetUpDirection() const
    //!
    const float3& GetUpDirection() const { return mUpDirection; }

private:
    void UpdateCamera();

private:
    float4x4 mCenterTranslationMatrix;
    float4x4 mTranslationMatrix;
    quat     mRotationQuat;
    float4x4 mCameraMatrix;
    float4x4 mInverseCameraMatrix;
    float3   mEyePosition;
    float3   mViewDirection;
    float3   mUpDirection;
};

} // namespace ppx

#endif // ppx_camera_h
