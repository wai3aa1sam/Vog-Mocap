#include "vogpch.h"
#include "ImGuiLibrary.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace vog {

	struct ImGuiSetting
	{
	};

	static ImGuiSetting s_data;

	void ImGuiLibrary::showImGuiSetting()
	{
		/*ImGui::Begin("ImGui Setting");


		ImGui::End();*/
	}

	void ImGuiLibrary::showDebugMetrics()
	{
		ImGuiContext& g = *GImGui;
		ImGuiIO& io = g.IO;
		ImGuiMetricsConfig* cfg = &g.DebugMetricsConfig;

		ImGui::Begin("Dear ImGui Metrics / Debugger");

		// Basic info
		//ImGui::Text("Dear ImGui %s", GetVersion());
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::Text("%d vertices, %d indices (%d triangles)", io.MetricsRenderVertices, io.MetricsRenderIndices, io.MetricsRenderIndices / 3);
		ImGui::Text("%d visible windows, %d active allocations", io.MetricsRenderWindows, io.MetricsActiveAllocations);

		ImGui::End();
	}

	//=========================================Impl======================================

	bool ImGuiLibrary::drawVec3fControl(const std::string& label_, Vector3f& values_, float resetValue_, float columnWidth_)
	{
		bool hasChangedValue = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label_.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth_);
		ImGui::Text(label_.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			values_.x = resetValue_;
			hasChangedValue |= true;
		}
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &values_.x, 0.1f, 0.0f, 0.0f, "%.2f"))
			hasChangedValue |= true;
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			values_.y = resetValue_;
			hasChangedValue |= true;
		}
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &values_.y, 0.1f, 0.0f, 0.0f, "%.2f"))
			hasChangedValue |= true;

		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
		{
			values_.z = resetValue_;
			hasChangedValue |= true;
		}
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &values_.z, 0.1f, 0.0f, 0.0f, "%.2f"))
			hasChangedValue |= true;
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return hasChangedValue;
	}

	bool ImGuiLibrary::drawVec3bCheckbox(const std::string& label_, Vector3b& value_, float columnWidth_)
	{
		bool hasChangedValue = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label_.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth_);
		ImGui::Text(label_.c_str());
		ImGui::NextColumn();

		//ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10.0f, 0 });

		ImGui::TextWrapped("X");

		ImGui::SameLine();
		if (ImGui::Checkbox("##X", &value_.x))
			hasChangedValue |= true;
		//ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::TextWrapped("Y");

		ImGui::SameLine();
		if (ImGui::Checkbox("##Y", &value_.y))
			hasChangedValue |= true;
		//ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::TextWrapped("Z");

		ImGui::SameLine();
		if (ImGui::Checkbox("##Z", &value_.z))
			hasChangedValue |= true;
		//ImGui::PopItemWidth();
		ImGui::SameLine();

		//ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return hasChangedValue;
	}

	static ImVec2 getRightAlignedSize(const std::string& label_) { return ImVec2(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowWidth() * 0.4f - ImGuiLibrary::s_padding, ImGui::GetWindowHeight()); }

	static ImVec2 getIndentationSize(float indentRatio_) { return ImVec2(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowWidth() * (1.0f - indentRatio_), ImGui::GetWindowHeight()); }

	bool ImGuiLibrary::drawDragFloat(const std::string& label_, float& value_, float speed_)
	{
		std::string id = toString_ImGuiID(label_);

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);

		ImGui::TextWrapped(label_.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(getRightAlignedSize(label_).x);
		bool hasChange = ImGui::DragFloat(id.c_str(), &value_, speed_);
		ImGui::PopItemWidth();
		return hasChange;
	}

	bool ImGuiLibrary::drawDragFloat2(const std::string& label_, Vector2f& value_, float speed_)
	{
		std::string id = toString_ImGuiID(label_);

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);

		ImGui::TextWrapped(label_.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(getRightAlignedSize(label_).x);
		bool hasChange = ImGui::DragFloat2(id.c_str(), glm::value_ptr(value_), speed_);
		ImGui::PopItemWidth();
		return hasChange;
	}

	bool ImGuiLibrary::drawDragFloat3(const std::string& label_, Vector3f& value_, float speed_)
	{
		std::string id = toString_ImGuiID(label_);

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);

		ImGui::TextWrapped(label_.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(getRightAlignedSize(label_).x);
		bool hasChange = ImGui::DragFloat3(id.c_str(), glm::value_ptr(value_), speed_);
		ImGui::PopItemWidth();
		return hasChange;
	}

	bool ImGuiLibrary::drawDragFloat4(const std::string& label_, Vector4f& value_, float speed_)
	{
		std::string id = toString_ImGuiID(label_);

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);

		ImGui::TextWrapped(label_.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(getRightAlignedSize(label_).x);
		bool hasChange = ImGui::DragFloat4(id.c_str(), glm::value_ptr(value_), speed_);
		ImGui::PopItemWidth();
		return hasChange;
	}

	bool ImGuiLibrary::drawInputInt(const std::string& label_, int& value_)
	{
		std::string id = toString_ImGuiID(label_);

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);

		ImGui::TextWrapped(label_.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(getRightAlignedSize(label_).x);
		bool hasChange = ImGui::InputInt(id.c_str(), &value_);
		ImGui::PopItemWidth();
		return hasChange;
	}

	bool ImGuiLibrary::drawInputFloat(const std::string& label_, float& value_)
	{
		std::string id = toString_ImGuiID(label_);

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);

		ImGui::TextWrapped(label_.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(getRightAlignedSize(label_).x);
		bool hasChange = ImGui::InputFloat(id.c_str(), &value_);
		ImGui::PopItemWidth();
		return hasChange;

	}

	bool ImGuiLibrary::drawCheckbox(const std::string& label_, bool& value_)
	{
		std::string id = toString_ImGuiID(label_);

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);
		ImGui::TextWrapped(label_.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(getRightAlignedSize(label_).x);
		bool hasChange = ImGui::Checkbox(id.c_str(), &value_);
		ImGui::PopItemWidth();
		return hasChange;
	}

	bool ImGuiLibrary::drawDragFloatArray(const std::string& label_, float* values_, uint32_t count_, float speed_)
	{
		return _drawDragFloatArray<float>(label_, values_, count_, drawDragFloat, speed_);
	}

	bool ImGuiLibrary::drawDragFloat2Array(const std::string& label_, Vector2f* values_, uint32_t count_, float speed_)
	{
		return _drawDragFloatArray<Vector2f>(label_, values_, count_, drawDragFloat2, speed_);
	}

	bool ImGuiLibrary::drawDragFloat3Array(const std::string& label_, Vector3f* values_, uint32_t count_, float speed_)
	{
		return _drawDragFloatArray<Vector3f>(label_, values_, count_, drawDragFloat3, speed_);
	}

	bool ImGuiLibrary::drawDragFloat4Array(const std::string& label_, Vector4f* values_, uint32_t count_, float speed_)
	{
		return _drawDragFloatArray<Vector4f>(label_, values_, count_, drawDragFloat4, speed_);
	}

	bool ImGuiLibrary::drawInputIntArray(const std::string& label_, int* values_, uint32_t count_)
	{
		return _drawDragArray<int>(label_, values_, count_, drawInputInt);
	}

	bool ImGuiLibrary::drawCheckboxArray(const std::string& label_, bool* values_, uint32_t count_)
	{
		return _drawDragArray<bool>(label_, values_, count_, drawCheckbox);
	}

	void ImGuiLibrary::drawText(const std::string& label_, float indentRatio_)
	{
		//ImGui::SetCursorPosX(getIndentationSize(indentRatio_).x);
		ImGui::TextWrapped(label_.c_str());
	}

	void ImGuiLibrary::drawTextWithValue(const std::string& label_, int value_)
	{
		ImGui::TextWrapped(label_.c_str());
		ImGui::SameLine(); ImGui::SetCursorPosX(getRightAlignedSize(label_).x);
		ImGui::Text(std::to_string(value_).c_str());
	}

	void ImGuiLibrary::drawTextWithValue(const std::string& label_, size_t value_)
	{
		ImGui::TextWrapped(label_.c_str());
		ImGui::SameLine(); ImGui::SetCursorPosX(getRightAlignedSize(label_).x);
		ImGui::Text(std::to_string(value_).c_str());
	}

	void ImGuiLibrary::drawTextWithValue(const std::string& label_, uint32_t value_)
	{
		ImGui::TextWrapped(label_.c_str());
		ImGui::SameLine(); ImGui::SetCursorPosX(getRightAlignedSize(label_).x);
		ImGui::Text(std::to_string(value_).c_str());
	}

	void ImGuiLibrary::drawTextWithValue(const std::string& label_, float value_)
	{
		ImGui::TextWrapped(label_.c_str());
		ImGui::SameLine(); 
		ImGui::SetCursorPosX(getRightAlignedSize(label_).x); ImGui::Text(std::to_string(value_).c_str());
	}

	void ImGuiLibrary::drawTextWithValue(const std::string& label_, const Vector2f& value_, float indentRatio_)
	{
		ImGui::TextWrapped(label_.c_str());
		ImGui::SetCursorPosX(getIndentationSize(indentRatio_).x);

		ImGui::Text("X");
		ImGui::SameLine(); ImGui::Text(std::to_string(value_.x).c_str());
		ImGui::SameLine(); ImGui::Text("Y");
		ImGui::SameLine(); ImGui::Text(std::to_string(value_.y).c_str());
	}

	void ImGuiLibrary::drawTextWithValue(const std::string& label_, const Vector3f& value_, float indentRatio_)
	{
		ImGui::TextWrapped(label_.c_str());
		ImGui::SetCursorPosX(getIndentationSize(indentRatio_).x);

		ImGui::Text("X"); 
		ImGui::SameLine(); ImGui::Text(std::to_string(value_.x).c_str()); 
		ImGui::SameLine(); ImGui::Text("Y"); 
		ImGui::SameLine(); ImGui::Text(std::to_string(value_.y).c_str()); 
		ImGui::SameLine(); ImGui::Text("Z"); 
		ImGui::SameLine(); ImGui::Text(std::to_string(value_.z).c_str());
	}

	void ImGuiLibrary::drawTextWithValue(const std::string& label_, const Vector4f& value_, float indentRatio_)
	{
		ImGui::TextWrapped(label_.c_str());
		ImGui::SetCursorPosX(getIndentationSize(indentRatio_).x);

		ImGui::Text("X");
		ImGui::SameLine(); ImGui::Text(std::to_string(value_.x).c_str());
		ImGui::SameLine(); ImGui::Text("Y");
		ImGui::SameLine(); ImGui::Text(std::to_string(value_.y).c_str());
		ImGui::SameLine(); ImGui::Text("Z");
		ImGui::SameLine(); ImGui::Text(std::to_string(value_.z).c_str());
		ImGui::SameLine(); ImGui::Text("W");
		ImGui::SameLine(); ImGui::Text(std::to_string(value_.w).c_str());
	}

	bool ImGuiLibrary::drawTextureIcon(const std::string& label_, uint32_t textureID_, Vector2f size_)
	{
		uint32_t padding = 10;
		ImGui::Text(label_.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - s_textureIconSize - padding);
		ImTextureID textureID = (ImTextureID)(size_t)textureID_;
		ImGui::ImageButton(textureID, { size_.x , size_.y }, { 0, 1 }, { 1, 0 });
		return true;
	}

	std::string ImGuiLibrary::toString_ImGuiID(const std::string& label_)
	{
		std::string buffer;
		buffer.reserve(label_.size() + 2);
		buffer.append("##");
		buffer.append(label_);
		return buffer;
	}

	std::string ImGuiLibrary::toString_ArrayName(const std::string& label_, size_t index_)
	{
		std::string buffer;
		std::string numberBuffer = std::to_string(index_);
		buffer.reserve(label_.size() + numberBuffer.size() + 1);
		buffer.append(numberBuffer);
		buffer.append("]", 2);
		return buffer;
	}

	void ImGuiLibrary::arrayName_changeIndex(std::string& arrayNameBuffer_, size_t index_)
	{
		VOG_CORE_ASSERT(!arrayNameBuffer_.empty(), "");
		VOG_CORE_ASSERT(arrayNameBuffer_.rfind('[') != std::string::npos, "");

		size_t sub_label_size = arrayNameBuffer_.rfind('[') + 1;									// xxxxxxx[0], we want "xxxxxxx[" only

		arrayNameBuffer_.erase(sub_label_size, arrayNameBuffer_.size() - sub_label_size);			// xxxxxxx[0], we want erase "0]" only 
		std::string numberBuffer = std::to_string(index_);
		arrayNameBuffer_.reserve(sub_label_size + numberBuffer.size() + 1);
		arrayNameBuffer_.append(numberBuffer);
		arrayNameBuffer_.append("]", 2);
	}

	void ImGuiLibrary::setIndentationPosition(float indentRatio_) { ImGui::SetCursorPosX(getIndentationSize(indentRatio_).x); }

	void ImGuiLibrary::_drawDragFloat(const std::string& label_, float& value_)
	{
		std::string id = toString_ImGuiID(label_);

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);

		ImGui::TextWrapped(label_.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(getRightAlignedSize(label_).x);
		ImGui::DragFloat(id.c_str(), &value_);
		ImGui::PopItemWidth();
	}
}
