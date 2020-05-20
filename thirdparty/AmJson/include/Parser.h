#pragma once

#include"AmJson.h"
#include"Value.h"
#include<string>

namespace Json {
	class Parser final {
	private:
		//缓存需要解析的字符串和解析完毕的json对象
		Value& val;
		const char* cur;

		void parseWhitespace()noexcept;
		void parseValue();

		//处理bool和null
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
		//通过构造函数进行实际的解析操作，val用来缓存解析后的JSON对象

		Parser(Value&, std::string&&);
	};
}


