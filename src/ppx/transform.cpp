#include "ppx/transform.h"

namespace ppx {

Transform::Transform()
{
}

Transform::Transform(const float3& translation)
{
    SetTranslation(translation);
}

Transform::~Transform()
{
}

void Transform::SetTranslation(const float3& value)
{
    mTranslation        = value;
    mDirty.translation  = true;
    mDirty.concatenated = true;
}

void Transform::SetRotation(const float3& value)
{
    mRotation           = value;
    mDirty.rotation     = true;
    mDirty.concatenated = true;
}

void Transform::SetScale(const float3& value)
{
    mScale       = value;
    mDirty.scale = true;
}

void Transform::SetRotationOrder(RotationOrder value)
{
    mRotationOrder      = value;
    mDirty.rotation     = true;
    mDirty.concatenated = true;
}

const float4x4& Transform::GetTranslationMatrix() const
{
    if (mDirty.translation) {
        mTranslationMatrix  = glm::translate(mTranslation);
        mDirty.translation  = false;
        mDirty.concatenated = true;
    }
    return mTranslationMatrix;
}

const float4x4& Transform::GetRotationMatrix() const
{
    if (mDirty.rotation) {
        float4x4 xm = glm::rotate(mRotation.x, float3(1, 0, 0));
        float4x4 ym = glm::rotate(mRotation.y, float3(0, 1, 0));
        float4x4 zm = glm::rotate(mRotation.z, float3(0, 0, 1));
        switch (mRotationOrder) {
            case RotationOrder::XYZ: mRotationMatrix = xm * ym * zm; break;
            case RotationOrder::XZY: mRotationMatrix = xm * zm * ym; break;
            case RotationOrder::YZX: mRotationMatrix = ym * zm * xm; break;
            case RotationOrder::YXZ: mRotationMatrix = ym * xm * zm; break;
            case RotationOrder::ZXY: mRotationMatrix = zm * xm * ym; break;
            case RotationOrder::ZYX: mRotationMatrix = zm * ym * xm; break;
        }
        mDirty.rotation     = false;
        mDirty.concatenated = true;
    }
    return mRotationMatrix;
}

const float4x4& Transform::GetScaleMatrix() const
{
    if (mDirty.scale) {
        mScaleMatrix        = glm::scale(mScale);
        mDirty.scale        = false;
        mDirty.concatenated = true;
    }
    return mScaleMatrix;
}

const float4x4& Transform::GetConcatenatedMatrix() const
{
    if (mDirty.concatenated) {
        const float4x4& T   = GetTranslationMatrix();
        const float4x4& R   = GetRotationMatrix();
        const float4x4& S   = GetScaleMatrix();
        mConcatenatedMatrix = T * R * S;
        mDirty.concatenated = false;
    }
    return mConcatenatedMatrix;
}

} // namespace ppx
