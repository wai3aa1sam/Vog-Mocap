#include "vogpch.h"
#include "MyMath.h"

#include <glm/gtc/type_ptr.hpp>

#include <assimp/matrix4x4.h>
#include <assimp/vector2.h>
#include <assimp/quaternion.h>

namespace vog {

#pragma region vec2f
   /* Vector2f::Vector2f() : vec2f(0.0f, 0.0f) {}

    Vector2f::Vector2f(float x_, float y_) : vec2f(x_, y_) {}

    Vector2f::Vector2f(const Vector2f& vec2_) : vec2f(vec2_.vec2f.x, vec2_.vec2f.y) {}

    void Vector2f::set(float x_, float y_)
    {
        vec2f = { x_, y_ };
    }

    const float* Vector2f::data()
    {
        return glm::value_ptr(vec2f);
    }

    Vector2f Vector2f::operator+(const Vector2f& rhs_) const { auto res = vec2f + rhs_.vec2f; return Vector2f{ res.x, res.y }; }

    Vector2f Vector2f::operator-(const Vector2f& rhs_) const { auto res = vec2f - rhs_.vec2f; return Vector2f{ res.x, res.y }; }

    Vector2f Vector2f::operator*(const Vector2f& rhs_) const { auto res = vec2f * rhs_.vec2f; return Vector2f{ res.x, res.y }; }

    Vector2f Vector2f::operator/(const Vector2f& rhs_) const { auto res = vec2f / rhs_.vec2f; return Vector2f{ res.x, res.y }; }*/
#pragma endregion

#pragma region vec3f
  /*  Vector3f::Vector3f() : vec3f(0.0f, 0.0f, 0.0f) {}

    Vector3f::Vector3f(float x_, float y_, float z_) : vec3f(x_, y_, z_) {}

    Vector3f::Vector3f(const Vector3f& vec3f_) : vec3f(vec3f_.vec3f.x, vec3f_.vec3f.y, vec3f_.vec3f.z) {}

    void Vector3f::set(float x_, float y_, float z_)
    {
    }

    const float* Vector3f::data()
    {
        return nullptr;
    }

    float Vector3f::magnitude() const
    {
        return 0.0f;
    }

    float Vector3f::length() const
    {
        return 0.0f;
    }

    Vector3f Vector3f::normalize() const
    {
        return Vector3f();
    }

    float Vector3f::dot(const Vector3f& r) const
    {
        return 0.0f;
    }

    Vector3f Vector3f::cross(const Vector3f& r) const
    {
        return Vector3f();
    }

    Vector3f Vector3f::min(const Vector3f& r) const
    {
        return Vector3f();
    }

    Vector3f Vector3f::max(const Vector3f& r) const
    {
        return Vector3f();
    }

    Vector3f Vector3f::operator+(const Vector3f& rhs_) const
    {
        return Vector3f();
    }

    Vector3f Vector3f::operator-(const Vector3f& rhs_) const
    {
        return Vector3f();
    }

    Vector3f Vector3f::operator*(const Vector3f& rhs_) const
    {
        return Vector3f();
    }

    Vector3f Vector3f::operator/(const Vector3f& rhs_) const
    {
        return Vector3f();
    }*/
#pragma endregion


#pragma region assimp_stuff
    //Vector2f toVec2fFrom(const aiVector2D& aiVec2f_) { return Vector2f{ aiVec2f_.x, aiVec2f_.y }; }

    //Vector3f toVec3fFrom(const aiVector3D& aiVec3f_) { return Vector3f{ aiVec3f_.x, aiVec3f_.y, aiVec3f_.z }; }

    //Matrix4f toMat4fFrom(const aiMatrix4x4& aiMat4_)
    //{
    //    Matrix4f to;
    //    to[0][0] = aiMat4_.a1; to[0][1] = aiMat4_.b1;  to[0][2] = aiMat4_.c1; to[0][3] = aiMat4_.d1;
    //    to[1][0] = aiMat4_.a2; to[1][1] = aiMat4_.b2;  to[1][2] = aiMat4_.c2; to[1][3] = aiMat4_.d2;
    //    to[2][0] = aiMat4_.a3; to[2][1] = aiMat4_.b3;  to[2][2] = aiMat4_.c3; to[2][3] = aiMat4_.d3;
    //    to[3][0] = aiMat4_.a4; to[3][1] = aiMat4_.b4;  to[3][2] = aiMat4_.c4; to[3][3] = aiMat4_.d4;
    //    return to;
    //}
    //Quaternion toQuatFrom(const aiQuaternion& aiQuat_) { return { aiQuat_.w, aiQuat_.x, aiQuat_.y, aiQuat_.z }; }
#pragma endregion

bool MyMath::isEqualTo(float lhs_, float rhs_, float epsilon_)
{
    return absf(lhs_ - rhs_) < epsilon_;
}

Vector3f MyMath::getMin(const Vector3f& pt1_, const Vector3f& pt2_)
    {
        Vector3f ret = pt1_;
        ret.x = (pt1_.x > pt2_.x) ? pt2_.x : pt1_.x;
        ret.y = (pt1_.y > pt2_.y) ? pt2_.y : pt1_.y;
        ret.z = (pt1_.z > pt2_.z) ? pt2_.z : pt1_.z;
        return ret;
    }
    Vector3f MyMath::getMax(const Vector3f& pt1_, const Vector3f& pt2_)
    {
        Vector3f ret = pt1_;
        ret.x = (pt1_.x < pt2_.x) ? pt2_.x : pt1_.x;
        ret.y = (pt1_.y < pt2_.y) ? pt2_.y : pt1_.y;
        ret.z = (pt1_.z < pt2_.z) ? pt2_.z : pt1_.z;
        return ret;
    }
}