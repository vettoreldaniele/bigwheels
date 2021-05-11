#ifndef ppx_camera_h
#define ppx_camera_h

#include "ppx/000_math_config.h"

#define PPX_CAMERA_DEFAULT_NEAR_CLIP      0.1f
#define PPX_CAMERA_DEFAULT_FAR_CLIP       10000.0f
#define PPX_CAMERA_DEFAULT_EYE_POSITION   float3(0, 0, 1)
#define PPX_CAMERA_DEFAULT_LOOK_AT        float3(0, 0, 0)
#define PPX_CAMERA_DEFAULT_WORLD_UP       float3(0, 1, 0)
#define PPX_CAMERA_DEFAULT_VIEW_DIRECTION float3(0, 0, -1)

namespace ppx {

// -------------------------------------------------------------------------------------------------
// Camera
// -------------------------------------------------------------------------------------------------
class Camera
{
public:
    Camera();

    Camera(float nearClip, float farClip);

    virtual ~Camera() {}

    virtual void LookAt(const float3& eye, const float3& target, const float3& up = PPX_CAMERA_DEFAULT_WORLD_UP);

    const float3& GetEyePosition() const { return mEyePosition; }

    const float4x4& GetViewMatrix() const { return mViewMatrix; }
    const float4x4& GetProjectionMatrix() const { return mProjectionMatrix; }
    const float4x4& GetViewProjectionMatrix() const { return mViewProjectionMatrix; }

    float3 WorldToViewPoint(const float3& worldPoint) const;
    float3 WorldToViewVector(const float3& worldVector) const;

    void MoveAlongViewDirection(float distance);

protected:
    float            mAspect               = 0;
    float            mNearClip             = PPX_CAMERA_DEFAULT_NEAR_CLIP;
    float            mFarClip              = PPX_CAMERA_DEFAULT_FAR_CLIP;
    float3           mEyePosition          = PPX_CAMERA_DEFAULT_EYE_POSITION;
    float3           mTarget               = PPX_CAMERA_DEFAULT_LOOK_AT;
    float3           mViewDirection        = PPX_CAMERA_DEFAULT_VIEW_DIRECTION;
    float3           mWorldUp              = PPX_CAMERA_DEFAULT_WORLD_UP;
    mutable float4x4 mViewMatrix           = float4x4(1);
    mutable float4x4 mProjectionMatrix     = float4x4(1);
    mutable float4x4 mViewProjectionMatrix = float4x4(1);
    mutable float4x4 mInverseViewMatrix    = float4x4(1);
};

// -------------------------------------------------------------------------------------------------
// PerspCamera
// -------------------------------------------------------------------------------------------------
class PerspCamera
    : public Camera
{
public:
    PerspCamera();

    PerspCamera(
        float horizFovDegrees,
        float aspect,
        float nearClip = PPX_CAMERA_DEFAULT_NEAR_CLIP,
        float farClip  = PPX_CAMERA_DEFAULT_FAR_CLIP);

    PerspCamera(
        const float3& eye,
        const float3& target,
        const float3& up,
        float         horizFovDegrees,
        float         aspect,
        float         nearClip = PPX_CAMERA_DEFAULT_NEAR_CLIP,
        float         farClip  = PPX_CAMERA_DEFAULT_FAR_CLIP);

    virtual ~PerspCamera();

    void SetPerspective(
        float horizFovDegrees,
        float aspect,
        float nearClip = PPX_CAMERA_DEFAULT_NEAR_CLIP,
        float farClip  = PPX_CAMERA_DEFAULT_FAR_CLIP);

    void FitToBoundingBox(const float3& bboxMinWorldSpace, const float3& bbxoMaxWorldSpace);

private:
    bool  mPixelAligned    = false;
    float mHorizFovDegrees = 60.0f;
    float mVertFovDegrees  = 36.98f;
    float mAspect          = 1.0f;
};

// -------------------------------------------------------------------------------------------------
// OrthoCamera
// -------------------------------------------------------------------------------------------------
class OrthoCamera
    : public Camera
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

    virtual ~OrthoCamera();

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
    : public PerspCamera
{
public:
    ArcballCamera();

    ArcballCamera(
        float horizFovDegrees,
        float aspect,
        float nearClip = PPX_CAMERA_DEFAULT_NEAR_CLIP,
        float farClip  = PPX_CAMERA_DEFAULT_FAR_CLIP);

    ArcballCamera(
        const float3& eye,
        const float3& target,
        const float3& up,
        float         horizFovDegrees,
        float         aspect,
        float         nearClip = PPX_CAMERA_DEFAULT_NEAR_CLIP,
        float         farClip  = PPX_CAMERA_DEFAULT_FAR_CLIP);

    virtual ~ArcballCamera() {}

    void LookAt(const float3& eye, const float3& target, const float3& up = PPX_CAMERA_DEFAULT_WORLD_UP) override;

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

    ////! @fn const float4x4& GetCameraMatrix() const
    ////!
    //const float4x4& GetCameraMatrix() const { return mCameraMatrix; }

    ////! @fn const float4x4& GetInverseCameraMatrix() const
    ////!
    //const float4x4& GetInverseCameraMatrix() const { return mInverseCameraMatrix; }

private:
    void UpdateCamera();

private:
    float4x4 mCenterTranslationMatrix;
    float4x4 mTranslationMatrix;
    quat     mRotationQuat;
    //float4x4 mCameraMatrix;
    //float4x4 mInverseCameraMatrix;
};

} // namespace ppx

#endif // ppx_camera_h
