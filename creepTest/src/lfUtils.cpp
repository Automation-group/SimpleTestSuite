#include "lfUtils.h"
#include <QStringList>
#include <QRegExp>

namespace lfUtils
{
	namespace
	{
		BaudRateType baudFromString(const QString& str)
		{
			switch(str.toInt())
			{
			case 50: return BAUD50;
			case 75: return BAUD75;
			case 110: return BAUD110;
			case 134: return BAUD134;
			case 150: return BAUD150;
			case 200: return BAUD200;
			case 300: return BAUD300;
			case 600: return BAUD600;
			case 1200: return BAUD1200;
			case 1800: return BAUD1800;
			case 2400: return BAUD2400;
			case 4800: return BAUD4800;
			case 9600: return BAUD9600;
			case 14400: return BAUD14400;
			case 19200: return BAUD19200;
			case 38400: return BAUD38400;
			case 56000: return BAUD56000;
			case 57600: return BAUD57600;
			case 76800: return BAUD76800;
			case 115200: return BAUD115200;
			case 128000: return BAUD128000;
			case 256000: return BAUD256000;
			}
			return BAUD19200;
		}


		DataBitsType dataBitsFromString(const QString& str)
		{
			switch (str.toInt())
			{
			case 5: return DATA_5;
			case 6: return DATA_6;
			case 7: return DATA_7;
			}
			return DATA_8;
		}

		FlowType flowControlFromString(const QString& str)
		{
			return FLOW_HARDWARE;
		}

		ParityType 	parityFromString(const QString& str)
		{

			if (str == "O") return PAR_ODD;
			if (str == "E") return PAR_EVEN;
			if (str == "M") return PAR_MARK;
			if (str == "S") return PAR_SPACE;

			return PAR_NONE;
		}

		StopBitsType stopBitsFromString(const QString& str)
		{
			if (str == "2") return STOP_2;
			if (str == "1_5") return STOP_1_5;
			return STOP_1;
		}
	}

	PortSettings createPortSettings(const QString& confString)
	{
		PortSettings result;

		{
			QRegExp regexp("baud:(\\S*)");
			if (regexp.indexIn(confString) != -1)
				result.BaudRate = baudFromString(regexp.cap(1));
		}

		{
			QRegExp regexp("parity:(\\S*)");
			if (regexp.indexIn(confString) != -1)
				result.Parity = parityFromString(regexp.cap(1));
		}

		{
			QRegExp regexp("data:(\\S*)");
			if (regexp.indexIn(confString) != -1)
				result.DataBits = dataBitsFromString(regexp.cap(1));
		}

		{
			QRegExp regexp("stop:(\\S*)");
			if (regexp.indexIn(confString) != -1)
				result.StopBits = stopBitsFromString(regexp.cap(1));
		}

		result.FlowControl = FLOW_HARDWARE;
		return result;
	}
}
