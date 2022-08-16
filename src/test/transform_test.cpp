#include <gtest/gtest.h>
#include <glm/gtx/string_cast.hpp>
#include "ppx/transform.h"

using namespace ppx;

TEST(TransformTest, Identity)
{
    Transform transform;

    EXPECT_EQ(transform.GetTranslation(), float3(0, 0, 0));
    EXPECT_EQ(transform.GetScale(), float3(1, 1, 1));
    EXPECT_EQ(transform.GetRotation(), float3(0, 0, 0));
    EXPECT_EQ(transform.GetRotationOrder(), Transform::RotationOrder::XYZ);

    EXPECT_EQ(transform.GetTranslationMatrix(), glm::translate(float3(0, 0, 0)));
    EXPECT_EQ(transform.GetScaleMatrix(), glm::scale(float3(1, 1, 1)));
    EXPECT_EQ(transform.GetRotationMatrix(), glm::eulerAngleXYZ(0.0f, 0.0f, 0.0f));
}

TEST(TransformTest, Scale)
{
    Transform transform;
    transform.SetScale(float3(3, 5, 7));
    EXPECT_EQ(transform.GetScale(), float3(3, 5, 7));
    EXPECT_EQ(transform.GetScaleMatrix(), glm::scale(float3(3, 5, 7)));
}

TEST(TransformTest, Translate)
{
    Transform transform;
    transform.SetTranslation(float3(3, 5, 7));
    EXPECT_EQ(transform.GetTranslation(), float3(3, 5, 7));
    EXPECT_EQ(transform.GetTranslationMatrix(), glm::translate(float3(3, 5, 7)));
}

TEST(TransformTest, Rotate)
{
    Transform transform;
    transform.SetRotation(float3(3, 5, 7));
    EXPECT_EQ(transform.GetRotation(), float3(3, 5, 7));
    EXPECT_EQ(transform.GetRotationMatrix(), glm::eulerAngleXYZ(3.0f, 5.0f, 7.0f));
}

TEST(TransformTest, TranslateScaleRotate)
{
    Transform transform;
    transform.SetTranslation(float3(19, 23, 29));
    transform.SetScale(float3(11, 13, 17));
    transform.SetRotation(float3(3, 5, 7));
    EXPECT_EQ(transform.GetConcatenatedMatrix(), glm::translate(float3(19, 23, 29)) * glm::eulerAngleXYZ(3.0f, 5.0f, 7.0f) * glm::scale(float3(11, 13, 17)));
}
