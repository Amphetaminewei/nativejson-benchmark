#pragma once

#include<variant>
#include<vector>
#include"AmJson.h"

namespace Json {
	class Value final {
	private:
		//variant可以在更换保存的内容时自动析构
		//void init(const Value&)noexcept;
		//void free()noexcept;

		friend bool operator==(const Value& lhs, const Value& rhs) noexcept;

		Json::type theType = Json::Null;
		//std::variant来代替union
		std::variant<double, std::string, std::vector<Value>, std::vector<std::pair<std::string, Value>>> value;

	public:
		void parse(const std::string& content);
		void stringify(std::string& content) const noexcept;

		int getType()const noexcept;
		void setType(type)noexcept;

		double getNumber()const noexcept;
		void setNumber(double)noexcept;

		const std::string& getString()const noexcept;
		void setString(const std::string&)noexcept;

		const Value& getArrayElement(size_t index)const noexcept;
		size_t getArraySize()const noexcept;
		void setArray(const std::vector<Value>&)noexcept;
		void pushbackArrayElement(const Value&)noexcept;
		void popbackArrayElement()noexcept;
		void insertArrayElement(const Value&, size_t)noexcept;
		void eraseArrayElement(size_t index, size_t count)noexcept;
		void clearArray()noexcept;

		void setObject(const std::vector<std::pair<std::string, Value>>&)noexcept;
		size_t getObjectSize()const noexcept;
		const std::string getObjectKey(size_t index)const noexcept;
		size_t getObjcetKeyLength(size_t index)const noexcept;
		const Value& getObjectValue(size_t index)const noexcept;
		void setObjectValue(const std::string& key, const Value& val)noexcept;
		long long findObjectIndex(const std::string& key)const noexcept;
		void removeObjectValue(size_t index)noexcept;
		void clearObject()noexcept;

		Value()noexcept;
		Value(const Value&)noexcept;
		Value& operator=(const Value& rhs)noexcept;
		~Value()noexcept;
	};

	bool operator==(const Value& lhs, const Value& rhs) noexcept;
	bool operator!=(const Value& lhs, const Value& rhs) noexcept;

}
