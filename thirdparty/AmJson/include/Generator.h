#pragma once
#include "Value.h"

namespace Json {

	class Generator final {
	public:
		Generator(const Value& val, std::string& result);
	private:
		void stringifyValue(const Value&);
		//�������ɶ����ʱ����Ҫ�������ɹؼ��֣����԰������ַ��������ó���
		void stringifyString(const std::string&);

		std::string& res;

	};

}