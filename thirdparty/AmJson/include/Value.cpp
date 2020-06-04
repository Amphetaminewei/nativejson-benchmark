#include<assert.h>
#include<variant>

#include"Value.h"


namespace Json {
	Value::Value()noexcept { }
	Value::Value(const Value& rhs)noexcept {
		value = rhs.value;
	}
	Value::~Value()noexcept {}

	void Value::setNull()noexcept {
		value = 1;
	}

	//类型的get、set
	int Value::getType()const noexcept {
		switch (value.index()) {
		case 0: return Json::Null;
		case 1: {
			if (std::get<bool>(value)) {
				return Json::True;
			}
			else {
				return Json::False;
			}
		}
		case 2: return Json::Number;
		case 3: return Json::String;
		case 4: return Json::Array;
		case 5: return Json::Object;
		}
	}

	double Value::getNumber()const noexcept {
		return std::get<double>(value);
	}

	const std::string& Value::getString()const noexcept {
		return std::get<std::string>(value);
	}


	const Value& Value::getArrayElement(const size_t& index)const noexcept {
		return std::get<std::vector<Value>>(value)[index];
	}
	size_t Value::getArraySize()const noexcept {
		return std::get<std::vector<Value>>(value).size();
	}
	void Value::pushbackArrayElement(const Value& val)noexcept {
		std::get<std::vector<Value>>(value).push_back(val);
	}
	void Value::popbackArrayElement()noexcept {
		std::get<std::vector<Value>>(value).pop_back();
	}
	void Value::eraseArrayElement(const size_t& index, const size_t& count)noexcept {
		std::get<std::vector<Value>>(value).erase(std::get<std::vector<Value>>(value).begin() + index, std::get<std::vector<Value>>(value).begin() + index + count);
	}
	void Value::insertArrayElement(const Value& val, const size_t& index)noexcept {
		std::get<std::vector<Value>>(value).insert(std::get<std::vector<Value>>(value).begin() + index, val);
	}
	void Value::clearArray()noexcept {
		std::get<std::vector<Value>>(value).clear();
	}


	size_t Value::getObjectSize()const noexcept {
		return std::get<std::vector<std::pair<std::string, Value>>>(value).size();
	}
	const std::string Value::getObjectKey(const size_t& index)const noexcept {
		return std::get<std::vector<std::pair<std::string, Value>>>(value)[index].first;
	}
	size_t Value::getObjcetKeyLength(const size_t& index)const noexcept {
		return std::get<std::vector<std::pair<std::string, Value>>>(value)[index].first.size();
	}
	const Value& Value::getObjectValue(const size_t& index)const noexcept {
		return std::get<std::vector<std::pair<std::string, Value>>>(value)[index].second;
	}
	long long Value::findObjectIndex(const std::string& key)const noexcept {
		for (int i = 0; i < std::get<std::vector<std::pair<std::string, Value>>>(value).size(); ++i) {
			if (std::get<std::vector<std::pair<std::string, Value>>>(value)[i].first == key)
				return i;
		}
		return -1;
	}
	void Value::setObjectValue(const std::string& key, const Value& val)noexcept {
		auto index = findObjectIndex(key);
		if (index >= 0) {
			//如果关键字存在
			std::get<std::vector<std::pair<std::string, Value>>>(value)[index].second = val; 
		}
		else {
			//如果关键字不存在
			std::get<std::vector<std::pair<std::string, Value>>>(value).push_back(make_pair(key, val));
		}
	}
	void Value::removeObjectValue(const size_t& index)noexcept {
		//讲道理如果不是快写完了，真应该给他起个别名的
		std::get<std::vector<std::pair<std::string, Value>>>(value).erase(std::get<std::vector<std::pair<std::string, Value>>>(value).begin() + index, std::get<std::vector<std::pair<std::string, Value>>>(value).begin() + index
			+ 1);
	}
	void Value::clearObject() noexcept {
		std::get<std::vector<std::pair<std::string, Value>>>(value).clear();
	}



	Value& Value::operator=(const Value& rhs)noexcept {
		//variant在赋值时可以自动析构原本的内容物，然后换成新的
		value = rhs.value;
		return *this;
	}
	bool operator==(const Value& lhs, const Value& rhs) noexcept {
		if (lhs.getType() != rhs.getType()) {
			return false;
		}
		else if (lhs.getType() == Json::Object) {
			//由于object会有内容一致而顺序不一致的问题，因此要单独处理
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