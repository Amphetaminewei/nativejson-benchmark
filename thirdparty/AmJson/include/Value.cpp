#include<assert.h>
#include<variant>

#include"Parser.h"
#include"Value.h"
#include"Generator.h"

namespace Json {
	Value::Value()noexcept { }
	Value::Value(const Value& rhs)noexcept {
		theType = rhs.theType;
		value = rhs.value;
	}
	Value::~Value()noexcept {}

	//���͵�get��set
	int Value::getType()const noexcept {
		return theType;
	}
	void Value::setType(type t)noexcept {
		theType = t;
	}

	double Value::getNumber()const noexcept {
		//��ʵ��std::variant���ĵ���˵get�������Ͳ����ڵ�ʱ�򷵻ش�����ֹ��������񣬵���ûд����ϸ
		//�������ﻹ���ȼ��϶�����,������ܿ��ǵĻ����ܿ���ȥ��
		assert(theType == Json::Number);
		return std::get<double>(value);
	}
	void Value::setNumber(double num)noexcept {
		theType = Json::Number;
		value = num;
	}

	const std::string& Value::getString()const noexcept {
		assert(theType == Json::String);
		return std::get<std::string>(value);
	}
	void Value::setString(const std::string& str)noexcept {
		theType = Json::String;
		value = str;
	}

	const Value& Value::getArrayElement(size_t index)const noexcept {
		assert(theType == Json::Array);
		return std::get<std::vector<Value>>(value)[index];
	}
	size_t Value::getArraySize()const noexcept {
		assert(theType == Json::Array);
		return std::get<std::vector<Value>>(value).size();
	}
	void Value::setArray(const std::vector<Value>& arr)noexcept {
		//std::variant����ֱ�Ӹ�ֵ���ڵȺ�ǰ�����Ͳ�һ��ʱ���Զ�����֮ǰ���͵Ķ��󲢸���Ϊ�����͵Ķ���
		if (theType == Json::Array) {
			value = arr;
		}
		else {
			theType = Json::Array;
			value = arr;
		}
	}
	void Value::pushbackArrayElement(const Value& val)noexcept {
		assert(theType == Json::Array);
		std::get<std::vector<Value>>(value).push_back(val);
	}
	void Value::popbackArrayElement()noexcept {
		assert(theType == Json::Array);
		std::get<std::vector<Value>>(value).pop_back();
	}
	void Value::eraseArrayElement(size_t index, size_t count)noexcept {
		assert(theType == Json::Array);
		std::get<std::vector<Value>>(value).erase(std::get<std::vector<Value>>(value).begin() + index, std::get<std::vector<Value>>(value).begin() + index + count);
	}
	void Value::insertArrayElement(const Value& val, size_t index)noexcept {
		assert(theType == Json::Array);
		std::get<std::vector<Value>>(value).insert(std::get<std::vector<Value>>(value).begin() + index, val);
	}
	void Value::clearArray()noexcept {
		assert(theType == Json::Array);
		std::get<std::vector<Value>>(value).clear();
	}


	void Value::setObject(const std::vector<std::pair<std::string, Json::Value>>& obj)noexcept {
		if (theType == Json::Object) {
			value = obj;
		}
		else {
			theType = Json::Object;
			value = obj;
		}
	}
	size_t Value::getObjectSize()const noexcept {
		assert(theType == Json::Object);
		return std::get<std::vector<std::pair<std::string, Value>>>(value).size();
	}
	const std::string Value::getObjectKey(size_t index)const noexcept {
		assert(theType == Json::Object);
		return std::get<std::vector<std::pair<std::string, Value>>>(value)[index].first;
	}
	size_t Value::getObjcetKeyLength(size_t index)const noexcept {
		assert(theType == Json::Object);
		return std::get<std::vector<std::pair<std::string, Value>>>(value)[index].first.size();
	}
	const Value& Value::getObjectValue(size_t index)const noexcept {
		assert(theType == Json::Object);
		return std::get<std::vector<std::pair<std::string, Value>>>(value)[index].second;
	}
	long long Value::findObjectIndex(const std::string& key)const noexcept {
		assert(theType == Json::Object);
		for (int i = 0; i < std::get<std::vector<std::pair<std::string, Value>>>(value).size(); ++i) {
			if (std::get<std::vector<std::pair<std::string, Value>>>(value)[i].first == key)
				return i;
		}
		return -1;
	}
	void Value::setObjectValue(const std::string& key, const Value& val)noexcept {
		assert(theType == Json::Object);
		auto index = findObjectIndex(key);
		if (index >= 0) {
			//����ؼ��ִ���
			std::get<std::vector<std::pair<std::string, Value>>>(value)[index].second = val; 
		}
		else {
			//����ؼ��ֲ�����
			std::get<std::vector<std::pair<std::string, Value>>>(value).push_back(make_pair(key, val));
		}
	}
	void Value::removeObjectValue(size_t index)noexcept {
		assert(theType == Json::Object);
		//������������ǿ�д���ˣ���Ӧ�ø������������
		std::get<std::vector<std::pair<std::string, Value>>>(value).erase(std::get<std::vector<std::pair<std::string, Value>>>(value).begin() + index, std::get<std::vector<std::pair<std::string, Value>>>(value).begin() + index
			+ 1);
	}
	void Value::clearObject() noexcept {
		assert(theType == Json::Object);
		std::get<std::vector<std::pair<std::string, Value>>>(value).clear();
	}



	void Value::parse(const std::string& content) {
		Parser(*this, content);
	}

	void Value::stringify(std::string& content)const noexcept {
		Generator(*this, content);
	}

	Value& Value::operator=(const Value& rhs)noexcept {
		theType = rhs.theType;
		//variant�ڸ�ֵʱ�����Զ�����ԭ���������Ȼ�󻻳��µ�
		value = rhs.value;
		return *this;
	}
	bool operator==(const Value& lhs, const Value& rhs) noexcept {
		if (lhs.theType != rhs.theType) {
			return false;
		}
		else if (lhs.theType == Json::Object) {
			//����object��������һ�¶�˳��һ�µ����⣬���Ҫ��������
			if (lhs.getObjectSize() != rhs.getObjectSize()) {
				return false;
			}
			for (size_t i = 0; i < lhs.getObjectSize(); ++i) {
				auto index = rhs.findObjectIndex(lhs.getObjectKey(i));
				if (index < 0 || lhs.getObjectValue(i) != rhs.getObjectValue(index)) {
					return false;
				}
			}
			return true;
		}
		else {
			return lhs.value == rhs.value;
		}
	}
	bool operator!=(const Value& lhs, const Value& rhs) noexcept {
		return !(lhs == rhs);
	}
}