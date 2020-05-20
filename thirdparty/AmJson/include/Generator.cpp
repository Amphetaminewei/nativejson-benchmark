#include "Generator.h"


namespace Json {
	Generator::Generator(const Value& val, std::string& result) : res(result){
		//注意这里传入的是引用，给res初始化用的也是引用，意味着res的改变也会随之更改result
		res.clear();
		stringifyValue(val);
	}

	void Generator::stringifyValue(const Value& val) {
		switch (val.getType()){
			case Json::Null: res += "null";   break;
			case Json::True: res += "true";   break;
			case Json::False: res += "false"; break;
			case Json::Number: {
				char buffer[32] = { 0 };
				sprintf_s(buffer, "%.17g", val.getNumber());
				res += buffer;
			} break;
			case Json::String: stringifyString(val.getString()); break;
			case Json::Array: {
				res += '[';
				for (int i = 0; i < val.getArraySize(); ++i) {
					if (i > 0) {
						res += ',';
					}
					stringifyValue(val.getArrayElement(i));
				}
				res += ']';
			} break;
			case Json::Object: {
				res += '{';
				for (int i = 0; i < val.getObjectSize(); ++i) {
					if (i > 0) {
						res += ',';
					}
					stringifyString(val.getObjectKey(i));
					res += ':';
					stringifyValue(val.getObjectValue(i));
				}
				res += '}';
			}break;
		}
	}

	void Generator::stringifyString(const std::string& str){
		res += '\"';
		for (auto it = str.begin(); it < str.end(); ++it) {
			unsigned char ch = *it;
			switch (ch) {
				case '\"': res += "\\\""; break;
				case '\\': res += "\\\\"; break;
				case '\b': res += "\\b";  break;
				case '\f': res += "\\f";  break;
				case '\n': res += "\\n";  break;
				case '\r': res += "\\r";  break;
				case '\t': res += "\\t";  break;
				default:
					if (ch < 0x20) {
						char buffer[7] = { 0 };
						sprintf_s(buffer, "\\u%04X", ch);
						res += buffer;
					}
					else
						res += *it;
			}
		}
		res += '\"';
	}
}