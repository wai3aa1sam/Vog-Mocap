#pragma once

#include "Vog/Core/Core.h"
#include "Vog/Core/Log.h"

#include "spdlog/fmt/ostr.h" // must be included

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx\euler_angles.hpp>
#include <glm/gtx\rotate_vector.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <sstream>
#include <array>


/// <summary>
/// This Class currently just wrap glm,
/// later will implment with our own math class (using simd)
/// all function should be inline
/// </summary>
/// 


//class aiVector2D;
//class aiVector3D;
//class aiMatrix4x4;
//class aiQuaternion;

namespace vog {

	using Vector2f = glm::vec2;
	using Vector3f = glm::vec3;
	using Vector4f = glm::vec4;

	using Matrix2f = glm::mat2;
	using Matrix3f = glm::mat3;
	using Matrix4f = glm::mat4;

	using Quaternion = glm::quat;

	using Vector3b = glm::bvec3;

	class MyMath
	{
	public:
		static constexpr float epsilon = 0.0001f;

		static float pi() { return glm::pi<float>(); }
		static Vector3f up() { return { 0.0f, 1.0f, 0.0f }; }

		static bool isEqualTo(float lhs_, float rhs_, float epsilon_ = epsilon);

		static float modf(float x_, float y_) { return glm::modf(x_, y_); }
		static float absf(float value_) { return glm::abs(value_); }
		template<typename T> static T abs(const T& value_) { return glm::abs(value_); }

		template<typename T> static T min(T rhs_, T lhs_) { return glm::min<T>(rhs_, lhs_); }
		template<typename T> static T max(T rhs_, T lhs_) { return glm::max<T>(rhs_, lhs_); }

		static Vector3f getMin(const Vector3f& pt1_, const Vector3f& pt2_);
		static Vector3f getMax(const Vector3f& pt1_, const Vector3f& pt2_);

		template<typename T> static T degrees(const T& rad_) { return glm::degrees(rad_); }
		template<typename T> static T radians(const T& deg_) { return glm::radians(deg_); }

		static float cos(float rad_) { return glm::cos(rad_); };
		static float sin(float rad_) { return glm::sin(rad_); };

		static float acos(float rad_) { return glm::acos(rad_); };
		static float asin(float rad_) { return glm::asin(rad_); };

		template<typename T> static T sqrt(const T& value_) { return glm::sqrt(value_); }

		template<typename T> static float magnitude(const T& value_) { return glm::length(value_);  }
		template<typename T> static float magnitude_square(const T& value_) { return glm::length2(value_); }

		template<typename T> static float distance(const T& value0_, const T& value1_) { return glm::distance(value0_, value1_); }
		template<typename T> static float distance_square(const T& value0_, const T& value1_) { return glm::distance2(value0_, value1_); }

		//static Matrix4f inverse(const Matrix4f& mat4_) { return glm::inverse(mat4_); }
		//static Quaternion inverse(const Quaternion& quat_) { return glm::inverse(quat_); }
		template<typename T> static T inverse(const T& value_) { return glm::inverse(value_); }
		
		static Quaternion conjugate(const Quaternion& quat_) { return glm::conjugate(quat_); }

		static Vector3f normalize(const Vector3f& vec3_) { return glm::normalize(vec3_); }

		//template<typename T>
		static Quaternion normalize(const Quaternion& value_) { return glm::normalize(value_); }

		static Vector3f cross(const Vector3f& lhs_, const Vector3f& rhs_) { return glm::cross(lhs_, rhs_); }
		static float dot(const Vector3f& lhs_, const Vector3f& rhs_) { return glm::dot(lhs_, rhs_); }

		static void decompose(const Matrix4f& modelMatrix_, Vector3f& scale_, Quaternion& rotation_, Vector3f& translation_, Vector3f& skew_, Vector4f& perspective_)
		{ 
			glm::decompose(modelMatrix_, scale_, rotation_, translation_, skew_, perspective_);
		}

		static void decompose(const Matrix4f& modelMatrix_, Vector3f& scale_, Quaternion& rotation_, Vector3f& translation_)
		{
			Vector3f skew;
			Vector4f perspective;
			glm::decompose(modelMatrix_, scale_, rotation_, translation_, skew, perspective);
		}

		static void decompose(const Matrix4f& modelMatrix_, Vector3f& scale_, Vector3f& rotation_, Vector3f& translation_)
		{
			Vector3f skew;
			Vector4f perspective;
			Quaternion rot;
			glm::decompose(modelMatrix_, scale_, rot, translation_, skew, perspective);
			rotation_ = toEulerAngles(rot);
		}

		static Matrix4f perspective(float fovy_rad_, float aspect_, float zNear_, float zFar_) { return glm::perspective(fovy_rad_, aspect_, zNear_, zFar_); }
		static Matrix4f orthographic(float left_, float right_, float bottom_, float top_, float near_ = 1.0f, float far_ = 7.5f) { return glm::ortho(left_, right_, bottom_, top_, near_, far_); }

		static Matrix4f lookAt(const Vector3f& eyePosition_, const Vector3f& target_, const Vector3f& up_) { return glm::lookAt(eyePosition_, target_, up_); }

		static Matrix4f translate(const Vector3f& position_) { return glm::translate(position_); }
		static Matrix4f rotate(float rad_, const Vector3f& axis_) { return glm::rotate(glm::mat4(1.0f), rad_, axis_); }
		static Matrix4f scale(const Vector3f& scale_) { return glm::scale(glm::mat4(1.0f), scale_); }
		static Matrix4f trs(const Vector3f& position_, float rad_, const Vector3f& axis_, const Vector3f& scale_) { return translate(position_) * rotate(rad_, axis_) * scale(scale_); }

		static Vector3f lerp(const Vector3f& start_, const Vector3f& end_, float ratio_) { return glm::mix(start_, end_, ratio_); }
		static Vector3f lerp(const Vector3f& start_, const Vector3f& end_, const Vector3f& ratio_) { return glm::mix(start_, end_, ratio_); }

		static Quaternion slerp(const Quaternion& start_, const Quaternion& end_, float ratio_) { return glm::slerp(start_, end_, ratio_); }

		static Matrix3f toMatrix3f(const Quaternion& quat_) { return glm::toMat3(quat_); }
		static Matrix4f toMatrix4f(const Quaternion& quat_) { return glm::toMat4(quat_); }

		static Quaternion diff(const Quaternion& to_, const Quaternion& from_) { Vector3f diff = glm::eulerAngles(to_) - glm::eulerAngles(from_);  return normalize(Quaternion(diff)); }

		static Vector3f toEulerAngles(const Quaternion& quat_) { return glm::eulerAngles(quat_); }
		static Quaternion toQuaternion(const Vector3f& vec3_) { return glm::quat(vec3_); }

		static Matrix3f eulerToMatrix3f(const Vector3f& rotation_) { return glm::toMat3(glm::quat(rotation_)); }
		static Matrix4f eulerToMatrix4f(const Vector3f& rotation_) { return glm::toMat4(glm::quat(rotation_)); }

		
		static Matrix3f transpose(const Matrix3f& matrix_) { return glm::transpose(matrix_); }
		static Matrix4f transpose(const Matrix4f& matrix_) { return glm::transpose(matrix_); }
	};

	//static Vector2f toVec2fFrom(const aiVector2D& aiVec2f_);
	//static Vector3f toVec3fFrom(const aiVector3D& aiVec3f_);
	//static Matrix4f toMat4fFrom(const aiMatrix4x4& aiMat4_);
	//static Quaternion toQuatFrom(const aiQuaternion& aiQuat_);
	

#pragma region log_glm
#ifdef VOG_ENABLE_PRINT_MATH
	inline void printGLM(const Matrix2f& mat2_)		{ VOG_LOG_INFO(glm::to_string(mat2_)); }
	inline void printGLM(const Matrix3f& mat3_)		{ VOG_LOG_INFO(glm::to_string(mat3_)); }
	inline void printGLM(const Matrix4f& mat4_)		{ VOG_LOG_INFO(glm::to_string(mat4_)); }
	inline void printGLM(const Vector2f& vec2f_)	{ VOG_LOG_INFO(glm::to_string(vec2f_)); }
	inline void printGLM(const Vector3f& vec3f_)	{ VOG_LOG_INFO(glm::to_string(vec3f_)); }
	inline void printGLM(const Vector4f& vec4f_)	{ VOG_LOG_INFO(glm::to_string(vec4f_)); }
	inline void printGLM(const Quaternion& quat_)	{ VOG_LOG_INFO(glm::to_string(quat_)); }

	inline void printGLM_Core(const Matrix2f& mat2_)	{ VOG_CORE_LOG_INFO(glm::to_string(mat2_)); }
	inline void printGLM_Core(const Matrix3f& mat3_)	{ VOG_CORE_LOG_INFO(glm::to_string(mat3_)); }
	inline void printGLM_Core(const Matrix4f& mat4_)	{ VOG_CORE_LOG_INFO(glm::to_string(mat4_)); }
	inline void printGLM_Core(const Vector2f& vec2f_)	{ VOG_CORE_LOG_INFO(glm::to_string(vec2f_)); }
	inline void printGLM_Core(const Vector3f& vec3f_)	{ VOG_CORE_LOG_INFO(glm::to_string(vec3f_)); }
	inline void printGLM_Core(const Vector4f& vec4f_)	{ VOG_CORE_LOG_INFO(glm::to_string(vec4f_)); }
	inline void printGLM_Core(const Quaternion& quat_)	{ VOG_CORE_LOG_INFO(glm::to_string(quat_)); }

	#define VOG_CORE_LOG_GLM(glm)		::vog::printGLM_Core(glm)
	#define VOG_LOG_GLM(glm)			::vog::printGLM(glm)
#else // VOG_DEBUG

	inline void printGLM(const Matrix2f& mat2_)		{}
	inline void printGLM(const Matrix3f& mat3_)		{}
	inline void printGLM(const Matrix4f& mat4_)		{}
	inline void printGLM(const Vector2f& vec2f_)	{}
	inline void printGLM(const Vector3f& vec3f_)	{}
	inline void printGLM(const Vector4f& vec4f_)	{}
	inline void printGLM(const Quaternion& quat_)	{}

	inline void printGLM_Core(const Matrix2f& mat2_) {}
	inline void printGLM_Core(const Matrix3f& mat3_) {}
	inline void printGLM_Core(const Matrix4f& mat4_) {}
	inline void printGLM_Core(const Vector2f& vec2f_) {}
	inline void printGLM_Core(const Vector3f& vec3f_) {}
	inline void printGLM_Core(const Vector4f& vec4f_) {}
	inline void printGLM_Core(const Quaternion& quat_) {}
	
	#define VOG_CORE_LOG_GLM(glm)	
	#define VOG_LOG_GLM(glm)		
#endif
	//void printGLM(const ::vog::Vector2f& vec2f_)
	//{
	//	VOG_LOG_INFO("<{0}, {1}>", vec2f_.x, vec2f_.y);
	//}

	//void printGLM(const ::vog::Vector3f& vec3f_)
	//{
	//	VOG_LOG_INFO("<{0}, {1}, {2}>", vec3f_.x, vec3f_.y, vec3f_.z);
	//}

	//void printGLM(const Vector4f& vec4f_)
	//{
	//	VOG_LOG_INFO("<{0}, {1}, {2}, {3}>", vec4f_.x, vec4f_.y, vec4f_.z, vec4f_.w);
	//}
#pragma endregion



	//inline std::ostream& operator << (std::ostream& os_, const Vector2f& vec2f_) {
	//	os_ << "<" << vec2f_.x << ", " << vec2f_.y << ">";
	//	return os_;
	//}

	//inline std::ostream& operator << (std::ostream& os_, const Vector3f& vec3f_) {
	//	os_ << "<" << vec3f_.x << ", " << vec3f_.y << ", " << vec3f_.z << ">";
	//	return os_;
	//}

	//inline std::ostream& operator << (std::ostream& os_, const Vector4f& vec4f_) {
	//	os_ << "<" << vec4f_.x << ", " << vec4f_.y << ", " << vec4f_.z << ", " << vec4f_.w << ">";
	//	return os_;
	//}
 
	//inline std::ostream& operator << (std::ostream& os_, const Quaterion& quat_) {
	//	os_ << "<" << quat_.x << ", " << quat_.y << ", " << quat_.z << ", " << quat_.w << ">";
	//	return os_;
	//}

//
//#pragma region vec2f
//	class VOG_API Vector2f
//	{
//	public:
//		union
//		{
//			//float x, y;
//			glm::vec2 vec2f;
//		};
//
//		Vector2f(); 
//		Vector2f(float x_, float y_);
//		Vector2f(const Vector2f& vec2f_);
//		~Vector2f() = default;
//
//		void set(float x_, float y_);
//		const float* data();
//
//		Vector2f operator+(const Vector2f& rhs_) const;
//		Vector2f operator-(const Vector2f& rhs_) const;
//		Vector2f operator*(const Vector2f& rhs_) const;
//		Vector2f operator/(const Vector2f& rhs_) const;
//
//		//Vector2f operator-() const;
//	};
//
//	inline std::ostream& operator << (std::ostream& os_, const Vector2f& vec2f_) {
//		os_ << "<" << vec2f_.vec2f.x << ", " << vec2f_.vec2f.y << ">";
//		return os_;
//	}
//#pragma endregion
//
//#pragma region vec3f
//	class VOG_API Vector3f
//	{
//	public:
//		union
//		{
//			//float x, y, z;
//			glm::vec3 vec3f;
//		};
//
//		Vector3f();
//		Vector3f(float x_, float y_, float z_);
//		Vector3f(const Vector3f& vec3f_);
//		~Vector3f() = default;
//
//		void set(float x_, float y_, float z_);
//		const float* data();
//
//		float magnitude() const;
//		float length() const;
//		Vector3f normalize() const;
//
//		float dot(const Vector3f& rhs_) const;
//		Vector3f cross(const Vector3f& rhs_) const;
//
//		Vector3f min(const Vector3f& rhs_) const;
//		Vector3f max(const Vector3f& rhs_) const;
//
//		Vector3f operator+(const Vector3f& rhs_) const;
//		Vector3f operator-(const Vector3f& rhs_) const;
//		Vector3f operator*(const Vector3f& rhs_) const;
//		Vector3f operator/(const Vector3f& rhs_) const;
//	};
//
//	inline std::ostream& operator << (std::ostream& os_, const Vector4f& vec4f_) {
//		os_ << "<" << vec4f_.vec4f.x << ", " << vec4f_.vec4f.y << ", " << vec4f_.vec4f.z << ", " << vec4f_.vec4f.w << ">";
//		return os_;
//	}
//#pragma endregion
//
//#pragma region vec4f
//	class VOG_API Vector4f
//	{
//	public:
//		union
//		{
//			float x, y, z, w;
//			glm::vec4 vec4f;
//		};
//
//		Vector4f();
//		Vector4f(float x_, float y_);
//		Vector4f(const Vector2f& vec2f_);
//		~Vector4f() = default;
//
//		void set(float x_, float y_);
//		const float* data();
//
//		Vector4f operator+(const Vector4f& rhs_) const;
//		Vector4f operator-(const Vector4f& rhs_) const;
//		Vector4f operator*(const Vector4f& rhs_) const;
//		Vector4f operator/(const Vector4f& rhs_) const;
//	};
//
//	inline std::ostream& operator << (std::ostream& os_, const Vector4f& vec4f_) {
//		os_ << "<" << vec4f_.vec4f.x << ", " << vec4f_.vec4f.y << ", " << vec4f_.vec4f.z << ", " << vec4f_.vec4f.w << ">";
//		return os_;
//	}
//#pragma endregion

}