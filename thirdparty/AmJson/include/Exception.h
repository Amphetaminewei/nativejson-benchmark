#pragma once

#include <string>
#include <stdexcept>

namespace Json {
	//这种方式可以直接在函数传入了错误的数据时直接抛出错误，而不需要根据函数返回结果来判断是否执行成功，节省一次判断的开销
	class Exception final : public std::logic_error {
	public:
		//通过exception的构造函数调用相应的logic_error的构造函数
		Exception(const std::string& errorMsg) : logic_error(errorMsg) { }
	};
}