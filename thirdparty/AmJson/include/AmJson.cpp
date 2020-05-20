#include"AmJson.h"
#include"Value.h"
#include"Exception.h"

/*
	json接口类，在程序中创建此类的对象即可使用它解析JSON字符串，解析方式为
	AmJson j;
	j.parse(str);
	其中str为要解析的JSON字符串，解析后parse()并不返回任何东西，而是将v构建为一个树形结构保存JSON
	而相对的可以使用这个类提供的接口来检索获取其中保存的对象，由于是树形结构，这个类默认保存的是根节点
	但是目前还没提供迭代器咳咳咳
	 AmJson::stringify(std::string& content)为反序列化接口，注意这个函数同样不返回任何值，他只会将content
	 构建为反序列化的结果，因此不要传入一个临时对象或者右值对象，那样你将永远无法获取结果呐
	 但是越想越不爽，为了反序列化需要单独创建一个string对象，这种事怎么看怎么应该是返回值做的事呢
*/

namespace Json {
	AmJson::AmJson()noexcept:theValue(new Json::Value){}
	AmJson::AmJson(const AmJson& rhs)noexcept {
		theValue.reset(new Json::Value(*(rhs.theValue)));
	}
	AmJson::AmJson(AmJson&& rhs)noexcept {
		theValue.reset(rhs.theValue.release());
	}
	AmJson::~AmJson()noexcept{}

	void AmJson::parse(std::string content, std::string& status)noexcept {
		//可以传递解析状态的解析函数，用来测试呦
		try {
			theValue->parse(std::move(content));
			status = "parse ok";
		}
		catch (const Json::Exception& msg) {
			status = msg.what();
		}
		catch (...) {

		}
	}
	void AmJson::parse(std::string content) {
		theValue->parse(std::move(content));
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
		theValue->setValue(1);
	}

	void AmJson::setBool(bool b)noexcept {
		theValue->setValue(b);
	}


	double AmJson::getNumber()const noexcept {
		//因为在Value的getNumber中有了断言，这里就不再做类型检查了
		return theValue->getNumber();
	}
	void AmJson::setNumber(double d)noexcept {
		theValue->setValue(d);
	}


	const std::string AmJson::getString()const noexcept {
		return theValue->getString();
	}
	void AmJson::setString(const std::string& str)noexcept {
		theValue->setValue(str);
	}


	AmJson AmJson::getArrayElement(const size_t& index)const noexcept {
		AmJson rst;
		rst.theValue.reset(new Json::Value(theValue->getArrayElement(index)));
		return rst;
	}
	size_t AmJson::getArraySize()const noexcept {
		return theValue->getArraySize();
	}
	void AmJson::setArray()noexcept {
		theValue->setValue(std::vector<Json::Value>{});
	}
	void AmJson::pushbackArrayElement(const AmJson& json)noexcept {
		theValue->pushbackArrayElement(*json.theValue);
	}
	void AmJson::popbackArrayElement()noexcept {
		theValue->popbackArrayElement();
	}
	void AmJson::eraseArrayElement(const size_t& index, const size_t& count)noexcept {
		//这里会发生两次复制！
		theValue->eraseArrayElement(index, count);
	}
	void AmJson::insertArrayElement(const AmJson& js, const size_t& index)noexcept {
		theValue->insertArrayElement(*js.theValue, index);
	}
	void AmJson::clearArray()noexcept {
		theValue->clearArray();
	}

	void AmJson::setObject()noexcept {
		//填入一个空的对象
		theValue->setValue(std::vector<std::pair<std::string, Json::Value>>{});
	}
	size_t AmJson::getObjectSize()const noexcept {
		return theValue->getObjectSize();
	}
	const std::string AmJson::getObjectKey(const size_t& index)const noexcept {
		return theValue->getObjectKey(index);
	}
	size_t AmJson::getObjectKeyLength(const size_t& index)const noexcept {
		return theValue->getObjcetKeyLength(index);
	}
	AmJson AmJson::getObjectValue(const size_t& index)const noexcept {
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
	void AmJson::removeObjectValue(const size_t& index)noexcept {
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