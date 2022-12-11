#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Vog/ImGui/ImGuiLibrary.h"

#include <SerialPort.h>

namespace vog {

	struct SerialData
	{
		Quaternion orienatation = {1.0f, 0.0f, 0.0f, 0.0f };
		Vector3f accerlation = { 0.0f, 0.0f, 0.0f };
	};

	enum IMUJointType
	{
		Right_Elbow		= 22,
		Right_Arm		= 24,
		Chest			= 26,
		Head			= 28,
		Left_Elbow		= 30,
		Left_Arm		= 32,
	};
	
	class ArduinoSerial	: public NonCopyable
	{
	public:
		static constexpr char* s_arduino_port_name = "\\\\.\\COM4";
		static constexpr int s_max_data_length = 255;
		static constexpr int s_max_imu_count = 7;
		static constexpr int s_start_pin_num = 22;

		int pinToIndex(int pin_)
		{
			int index = (pin_ - s_start_pin_num) / 2;
			if (index >= 0 && index < s_max_imu_count)
				return index;
			//VOG_CORE_ASSERT(index >= 0 && index < s_max_imu_count, "");
			return 0;
		}

	public:
		ArduinoSerial() = default;
		~ArduinoSerial();
		void init();
		void destroy();
		void update(void);
		std::array<SerialData, s_max_imu_count>& getResultData();

		void onImGuiRender();

		static ArduinoSerial& get() { return *s_pInstance; }

		Vector3f getFinalRotation(IMUJointType type_) 
		{
			int index = pinToIndex(type_);
			return MyMath::toEulerAngles(m_ret_data[index].orienatation) - MyMath::toEulerAngles(m_setZero_data[index].orienatation);
		}

	private:
		int receiveData(void);

	private:
		static ArduinoSerial* s_pInstance;

		SerialPort* m_pSerialPort = nullptr;
		std::string m_incomingData;
		int m_active_imu_count = 0;
		std::array<SerialData, s_max_imu_count> m_ret_data;
		std::array<SerialData, s_max_imu_count> m_setZero_data;

		bool m_isSetZero = true;

		bool m_isFirstReady = false;
	};

	inline std::array<SerialData, ArduinoSerial::s_max_imu_count>& ArduinoSerial::getResultData() { return m_ret_data; }

	inline void ArduinoSerial::onImGuiRender()
	{
		ImGuiLibrary::drawCheckbox("IMU set zero", m_isSetZero);
	}

}