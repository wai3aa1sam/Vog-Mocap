#include "ArduinoSerial.h"

namespace vog {

//#define VOG_ENABLE_SERIAL_LOG

	ArduinoSerial* ArduinoSerial::s_pInstance = nullptr;

	ArduinoSerial::~ArduinoSerial()
	{
		destroy();
	}

	void ArduinoSerial::init()
	{
		if (!m_pSerialPort)
		{
			m_pSerialPort = new SerialPort(s_arduino_port_name);
			m_incomingData.resize(s_max_data_length);

			s_pInstance = this;
		}

		if (m_pSerialPort->isConnected()) 
		{
			VOG_LOG_INFO("Connection established at port: {0}", s_arduino_port_name);
		}
	}

	void ArduinoSerial::destroy()
	{
		delete m_pSerialPort;
		m_pSerialPort = nullptr;

		m_isFirstReady = false;

		s_pInstance = nullptr;
	}

	void ArduinoSerial::update(void)
	{
		VOG_ASSERT(m_pSerialPort, "");

		if (m_pSerialPort && m_pSerialPort->isConnected())
			receiveData();
	}
	int ArduinoSerial::receiveData(void)
	{
		VOG_ASSERT(m_incomingData.size() == s_max_data_length, "");
		int readResult = m_pSerialPort->readSerialPort(m_incomingData.c_str(), s_max_data_length);

		//VOG_LOG_INFO("{0}", m_incomingData);

		std::string buf;

		if (size_t pos = my_getToken(buf, m_incomingData, 0, "==") != std::string::npos)
		{
#ifdef VOG_ENABLE_SERIAL_LOG
			VOG_LOG_INFO("found!");
			VOG_LOG_INFO("=====Start");
#endif // VOG_ENABLE_SERIAL_LOG

			if (!m_isFirstReady)
			{
				VOG_LOG_INFO("IMU Ready To send!");
				m_isFirstReady = true;
			}
			int pin = 0;

			pos = my_getToken(buf, m_incomingData, pos + 1, "=:");

			try
			{
				if (pos != std::string::npos && buf.size())
					pin = std::stoi(buf);
			}
			catch (const std::exception&)
			{
				pin = s_start_pin_num;
			}

#ifdef VOG_ENABLE_SERIAL_LOG
			VOG_LOG_INFO("pin: {0}", buf);
			VOG_LOG_INFO("index: {0}", pinToIndex(pin));
			VOG_LOG_INFO("{0}", m_incomingData);
#endif // VOG_ENABLE_SERIAL_LOG

			pos = my_getToken(buf, m_incomingData, pos, ":");

			SerialData& data = m_ret_data[pinToIndex(pin)];

			for (int i = 0; i < 4; i++)
			{
				pos = my_getToken(buf, m_incomingData, pos, ",");
				if (pos != std::string::npos && buf.size())
					data.orienatation[i] = std::stof(buf);
			}

			for (int i = 0; i < 2; i++)
			{
				pos = my_getToken(buf, m_incomingData, pos, ",");
				if (pos != std::string::npos && buf.size())
					data.accerlation[i] = std::stof(buf);
			}
			pos = my_getToken(buf, m_incomingData, pos, "\r\n");
			if (pos != std::string::npos && buf.size())
				data.accerlation[2] = std::stof(buf);

			if (m_isSetZero)
			{
				m_setZero_data[pinToIndex(pin)] = data;
				static int reset = 0;
				if (reset % s_max_imu_count == s_max_imu_count - 1)
				{
					m_isSetZero = false;
					VOG_LOG_INFO("IMU Reset Zero End");
				}
				reset++;
			}

#ifdef VOG_ENABLE_SERIAL_LOG
			VOG_LOG_GLM(data.orienatation);
			VOG_LOG_GLM(data.accerlation);
			VOG_LOG_INFO("=====End");
#endif // VOG_ENABLE_SERIAL_LOG

		}
		//Sleep(10);
		return readResult;
	}
}