#include"AmJson.h"
#include"Value.h"
#include"Exception.h"

namespace Json {
	AmJson::AmJson()noexcept:theValue(new Json::Value){}
	AmJson::AmJson(const AmJson& rhs)noexcept {
		theValue.reset(new Json::Value(*(rhs.theValue)));
	}
	AmJson::AmJson(AmJson&& rhs)noexcept {
		theValue.reset(rhs.theValue.release());
	}
	AmJson::~AmJson()noexcept{}

	void AmJson::parse(const std::string& content, std::string& status)noexcept {
		//可以传递解析状态的解析函数，用来测试呦
		try {
			theValue->parse(content);
			status = "parse ok";
		}
		catch (const Json::Exception& msg) {
			status = msg.what();
		}
		catch (...) {

		}
	}
	void AmJson::parse(const std::string& content) {
		theValue->parse(content);
	}


	void AmJson::stringify(std::string& content)const noexcept {
		theValue->stringify(content);
	}

	void AmJson::swap(AmJson& rhs)noexcept {
		std::swap(theValue, rhs.theValue);
	}

	int AmJson::getType()const noexcept {
		if (theValue == nullptr) {
			return Json::Null;
		}
		return theValue->getType();
	}

	void AmJson::setNull()noexcept {
		theValue->setType(Json::Null);
	}

	void AmJson::setBool(bool b)noexcept {
		if (b) {
			theValue->setType(Json::True);
		}
		else {
			theValue->setType(Json::False);
		}
	}


	double AmJson::getNumber()const noexcept {
		//因为在Value的getNumber中有了断言，这里就不再做类型检查了
		return theValue->getNumber();
	}
	void AmJson::setNumber(double d)noexcept {
		theValue->setNumber(d);
	}


	const std::string AmJson::getString()const noexcept {
		return theValue->getString();
	}
	void AmJson::setString(const std::string& str)noexcept {
		theValue->setString(str);
	}


	AmJson AmJson::getArrayElement(size_t index)const noexcept {
		AmJson rst;
		rst.theValue.reset(new Json::Value(theValue->getArrayElement(index)));
		return rst;
	}
	size_t AmJson::getArraySize()const noexcept {
		return theValue->getArraySize();
	}
	void AmJson::setArray()noexcept {
		theValue->setArray(std::vector<Json::Value>{});
	}
	void AmJson::pushbackArrayElement(const AmJson& json)noexcept {
		theValue->pushbackArrayElement(*json.theValue);
	}
	void AmJson::popbackArrayElement()noexcept {
		theValue->popbackArrayElement();
	}
	void AmJson::eraseArrayElement(size_t index, size_t count)noexcept {
		//这里会发生两次复制！
		theValue->eraseArrayElement(index, count);
	}
	void AmJson::insertArrayElement(const AmJson& js, size_t index)noexcept {
		theValue->insertArrayElement(*js.theValue, index);
	}
	void AmJson::clearArray()noexcept {
		theValue->clearArray();
	}

	void AmJson::setObject()noexcept {
		//填入一个空的对象
		theValue->setObject(std::vector<std::pair<std::string, Json::Value>>{});
	}
	size_t AmJson::getObjectSize()const noexcept {
		return theValue->getObjectSize();
	}
	const std::string AmJson::getObjectKey(size_t index)const noexcept {
		return theValue->getObjectKey(index);
	}
	size_t AmJson::getObjectKeyLength(size_t index)const noexcept {
		return theValue->getObjcetKeyLength(index);
	}
	AmJson AmJson::getObjectValue(size_t index)const noexcept {
		AmJson rst;
		rst.theValue.reset(new Json::Value(theValue->getObjectValue(index)));
		return rst;
	}
	void AmJson::setObjectValue(const std::string& key, const AmJson& value)noexcept {
		theValue->setObjectValue(key, *value.theValue);
	}
	long long AmJson::findObjectIndex(const std::string& key)const noexcept {
		return theValue->findObjectIndex(key);
	}
	void AmJson::removeObjectValue(size_t index)noexcept {
		//同样是两次复制
		theValue->removeObjectValue(index);
	}
	void AmJson::clearObject()noexcept {
		theValue->clearObject();
	}


	AmJson& AmJson::operator=(const AmJson& rhs)noexcept {
		theValue.reset(new Json::Value(*(rhs.theValue)));
		return *this;
	}
	AmJson& AmJson::operator=(AmJson&& rhs)noexcept {
		theValue.reset(rhs.theValue.release());
		return *this;
	}

	bool operator==(const AmJson& lhs, const AmJson& rhs)noexcept {
		return *lhs.theValue == *rhs.theValue;
	}
	bool operator!=(const AmJson& lhs, const AmJson& rhs)noexcept {
		return *lhs.theValue != *rhs.theValue;
	}
	void swap(AmJson& lhs, AmJson& rhs)noexcept {
		lhs.swap(rhs);
	}
}