#pragma once
#include "Value.h"

namespace Json {

	class Generator final {
	public:
		Generator(const Value& val, std::string& result);
	private:
		void stringifyValue(const Value&);
		//由于生成对象的时候还需要单独生成关键字，所以把生成字符串单独拿出来
		void stringifyString(const std::string&);

		std::string& res;

	};

}