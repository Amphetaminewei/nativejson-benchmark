#pragma once

#include"AmJson.h"
#include"Value.h"
#include<string>

namespace Json {
	class Parser final {
	private:
		//������Ҫ�������ַ����ͽ�����ϵ�json����
		Value& val;
		const char* cur;

		void parseWhitespace()noexcept;
		void parseValue();

		//����bool��null
		void parseNull(const char* literal);
		void parseBool(const char* literal, bool&&);

		void parseNumber();
		void parseString();
		void parseArray();
		void parseObject();

		void parseStringRaw(std::string&);
		void parseHex4(const char*& p, unsigned& u);
		void parseEncodeUtf8(std::string& s, unsigned u) const noexcept;
	public:
		//ͨ�����캯������ʵ�ʵĽ���������val��������������JSON����

		Parser(Value&, std::string&&);
	};
}


