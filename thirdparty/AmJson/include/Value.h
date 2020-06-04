#pragma once

#include<variant>
#include<vector>
#include"AmJson.h"
#include"Parser.h"
#include"Generator.h"

namespace Json {
	class Value final {
	private:
		//variant可以在更换保存的内容时自动析构
		//void init(const Value&)noexcept;
		//void free()noexcept;

		friend bool operator==(const Value& lhs, const Value& rhs) noexcept;

		//Json::Type theType = Json::Null;
		//std::variant来代替union
		std::variant<int, bool, double, std::string, std::vector<Value>, std::vector<std::pair<std::string, Value>>> value;

	public:
		template<typename T>
		void parse(T&& content);
		template<typename T>
		void stringify(T&& content)const noexcept;

		int getType()const noexcept;

		/*使用一个单独的万能引用版本setValue来完成对variant中值的更改
		在原来的版本中这项任务是由很多个分开的函数完成的，因为类型标志位的存在，
		每次进行set时都需要将标志位改为相应的值，
		但是如果使用模板函数，
		就无法确定set的具体类型是什么也就无法正确更改标志位的值，
		而如今取消了标志位也就可以仅仅依托一个模板函数来完成这项工作了*/
		template<typename T>
		void setValue(T&& val)noexcept;

		double getNumber()const noexcept;

		const std::string& getString()const noexcept;

		const Value& getArrayElement(const size_t& index)const noexcept;
		size_t getArraySize()const noexcept;
		void pushbackArrayElement(const Value&)noexcept;
		void popbackArrayElement()noexcept;
		void insertArrayElement(const Value&, const size_t&)noexcept;
		void eraseArrayElement(const size_t& index, const size_t& count)noexcept;
		void clearArray()noexcept;

		size_t getObjectSize()const noexcept;
		const std::string getObjectKey(const size_t& index)const noexcept;
		size_t getObjcetKeyLength(const size_t& index)const noexcept;
		const Value& getObjectValue(const size_t& index)const noexcept;
		void setObjectValue(const std::string& key, const Value& val)noexcept;
		long long findObjectIndex(const std::string& key)const noexcept;
		void removeObjectValue(const size_t& index)noexcept;
		void clearObject()noexcept;

		void setNull()noexcept;

		Value()noexcept;
		Value(const Value&)noexcept;
		Value& operator=(const Value& rhs)noexcept;
		~Value()noexcept;

	};

	bool operator==(const Value& lhs, const Value& rhs) noexcept;
	bool operator!=(const Value& lhs, const Value& rhs) noexcept;

	//依托万能引用的模板函数，必须将实现写在头文件中
	//而且这个函数基于万能引用实现，因此只要是最后一次使用参数，都可以直接以std::move传入右值，从而移动赋值给value
	template<typename T>
	void Value::setValue(T&& val)noexcept {
		value = std::forward<T>(val);
	}

	template<typename T>
	void Value::parse(T&& content) {
		Parser(*this, std::forward<T>(content));
	}
	template<typename T>
	void Value::stringify(T&& content)const noexcept {
		Generator(*this, std::forward<T>(content));
	}
}
