#ifndef ppx_transform_h
#define ppx_transform_h

#include "ppx/000_math_config.h"

namespace ppx {

class Transform
{
public:
    enum RotationOrder
    {
        XYZ,
        XZY,
        YZX,
        YXZ,
        ZXY,
        ZYX,
    };

    Transform();
    Transform(const float3& translation);
    virtual ~Transform();

    const float3& GetTranslation() const { return mTranslation; }
    const float3& GetRotation() const { return mRotation; }
    const float3& GetScale() const { return mScale; }
    RotationOrder GetRotationOrder() const { return mRotationOrder; }

    void SetTranslation(const float3& value);
    void SetTranslation(float x, float y, float z) { SetTranslation(float3(x, y, z)); }
    void SetRotation(const float3& value);
    void SetRotation(float x, float y, float z) { SetRotation(float3(x, y, z)); }
    void SetScale(const float3& value);
    void SetScale(float x, float y, float z) { SetScale(float3(x, y, z)); }
    void SetRotationOrder(RotationOrder value);

    const float4x4& GetTranslationMatrix() const;
    const float4x4& GetRotationMatrix() const;
    const float4x4& GetScaleMatrix() const;
    const float4x4& GetConcatenatedMatrix() const;

protected:
    mutable struct
    {
        union
        {
            struct
            {
                bool translation  : 1;
                bool rotation     : 1;
                bool scale        : 1;
                bool concatenated : 1;
            };
            uint32_t mask = 0xF;
        };
    } mDirty;

    float3           mTranslation   = float3(0, 0, 0);
    float3           mRotation      = float3(0, 0, 0);
    float3           mScale         = float3(1, 1, 1);
    RotationOrder    mRotationOrder = RotationOrder::XYZ;
    mutable float4x4 mTranslationMatrix;
    mutable float4x4 mRotationMatrix;
    mutable float4x4 mScaleMatrix;
    mutable float4x4 mConcatenatedMatrix;
};

} // namespace ppx

#endif // ppx_transform_h
