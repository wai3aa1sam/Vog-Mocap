#pragma once
#include "Vog/Core/Core.h"
#include "Vog/Math/MyMath.h"

#include <string>

// drawVec3Control References: https://github.com/TheCherno/Hazel/blob/master/Hazelnut/src/Panels/SceneHierarchyPanel.cpp

namespace vog {

	class VOG_API ImGuiLibrary
	{
	public:
		static constexpr int s_textureIconSize = 56;
		static constexpr int s_padding = 4;

		// show imgui stuff
		static void showImGuiSetting();
		static void showDebugMetrics();

		static bool drawVec3fControl(const std::string& label_, Vector3f& values_, float resetValue_ = 0.0f, float columnWidth_ = 100.0f);
		static bool drawVec3bCheckbox(const std::string& label_, Vector3b& value_, float columnWidth_ = 100.0f);

		static bool drawDragFloat(const std::string& label_, float& value_, float speed_ = 0.1f);
		static bool drawDragFloat2(const std::string& label_, Vector2f& value_, float speed_ = 0.1f);
		static bool drawDragFloat3(const std::string& label_, Vector3f& value_, float speed_ = 0.1f);
		static bool drawDragFloat4(const std::string& label_, Vector4f& value_, float speed_ = 0.1f);
		static bool drawInputInt(const std::string& label_, int& value_);
		static bool drawInputFloat(const std::string& label_, float& value_);
		static bool drawCheckbox(const std::string& label_, bool& value_);

		static bool drawDragFloatArray(const std::string& label_, float* values_, uint32_t count_, float speed_ = 0.1f);
		static bool drawDragFloat2Array(const std::string& label_, Vector2f* values_, uint32_t count_, float speed_ = 0.1f);
		static bool drawDragFloat3Array(const std::string& label_, Vector3f* values_, uint32_t count_, float speed_ = 0.1f);
		static bool drawDragFloat4Array(const std::string& label_, Vector4f* values_, uint32_t count_, float speed_ = 0.1f);
		static bool drawInputIntArray(const std::string& label_, int* values_, uint32_t count_);
		static bool drawCheckboxArray(const std::string& label_, bool* values_, uint32_t count_);

		static void drawText(const std::string& label_, float indentRatio_ = 0.0f);
		
		static void drawTextWithValue(const std::string& label_, int value_);
		static void drawTextWithValue(const std::string& label_, size_t value_);
		static void drawTextWithValue(const std::string& label_, uint32_t value_);
		static void drawTextWithValue(const std::string& label_, float value_);
		static void drawTextWithValue(const std::string& label_, const Vector2f& value_, float indentRatio_ = 0.15f);
		static void drawTextWithValue(const std::string& label_, const Vector3f& value_, float indentRatio_ = 0.15f);
		static void drawTextWithValue(const std::string& label_, const Vector4f& value_, float indentRatio_ = 0.15f);

		static bool drawTextureIcon(const std::string& label_, uint32_t textureID_, Vector2f size_ = { s_textureIconSize, s_textureIconSize });
		

		static std::string toString_ImGuiID(const std::string& label_);
		static std::string toString_ArrayName(const std::string& label_, size_t index_);		// label_name should be $type xxxx instead of $type xxxxx[]
		static void arrayName_changeIndex(std::string& arrayNameBuffer_, size_t index_);

		static void setIndentationPosition(float indentRatio_);

	private:
		template<typename T, typename Func>
		static bool _drawDragFloatArray(const std::string& label_, T* values_, uint32_t count_, Func func_, float speed_ = 0.1f);
		template<typename T, typename Func>
		static bool _drawDragArray(const std::string& label_, T* values_, uint32_t count_, Func func_);

		static void _drawDragFloat(const std::string& label_, float& value_);
	};
	template<typename T, typename Func>
	inline bool ImGuiLibrary::_drawDragFloatArray(const std::string& label_, T* values_, uint32_t count_, Func func_, float speed_)
	{
		VOG_CORE_ASSERT(count_ > 0, "");
		VOG_CORE_ASSERT(values_, "");

		bool hasChange = false;
		if (count_ > 1)
		{
			size_t padding = 10;
			std::string buffer;
			buffer.reserve(label_.size() + padding);
			buffer = label_;							// copy

			for (size_t i = 0; i < count_; i++, values_++)
			{
				VOG_CORE_ASSERT(values_, "");
				hasChange |= func_(buffer.c_str(), *values_, speed_);
				arrayName_changeIndex(buffer, i + 1);
			}
		}
		else
			hasChange |= func_(label_, *values_, speed_);

		return hasChange;
	}
	template<typename T, typename Func>
	inline bool ImGuiLibrary::_drawDragArray(const std::string& label_, T* values_, uint32_t count_, Func func_)
	{
		VOG_CORE_ASSERT(count_ > 0, "");
		VOG_CORE_ASSERT(values_, "");

		bool hasChange = false;
		if (count_ > 1)
		{
			size_t padding = 10;
			std::string buffer;
			buffer.reserve(label_.size() + padding);
			buffer = label_;							// copy

			for (size_t i = 0; i < count_; i++, values_++)
			{
				VOG_CORE_ASSERT(values_, "");
				hasChange |= func_(buffer.c_str(), *values_);
				arrayName_changeIndex(buffer, i + 1);
			}
		}
		else
			hasChange |= func_(label_, *values_);

		return hasChange;
	}
}