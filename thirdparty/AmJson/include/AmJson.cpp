#include"AmJson.h"
#include"Value.h"
#include"Exception.h"

/*
	json�ӿ��࣬�ڳ����д�������Ķ��󼴿�ʹ��������JSON�ַ�����������ʽΪ
	AmJson j;
	j.parse(str);
	����strΪҪ������JSON�ַ�����������parse()���������κζ��������ǽ�v����Ϊһ�����νṹ����JSON
	����ԵĿ���ʹ��������ṩ�Ľӿ���������ȡ���б���Ķ������������νṹ�������Ĭ�ϱ�����Ǹ��ڵ�
	����Ŀǰ��û�ṩ�������ȿȿ�
	 AmJson::stringify(std::string& content)Ϊ�����л��ӿڣ�ע���������ͬ���������κ�ֵ����ֻ�Ὣcontent
	 ����Ϊ�����л��Ľ������˲�Ҫ����һ����ʱ���������ֵ���������㽫��Զ�޷���ȡ�����
	 ����Խ��Խ��ˬ��Ϊ�˷����л���Ҫ��������һ��string������������ô����ôӦ���Ƿ���ֵ��������
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
		//���Դ��ݽ���״̬�Ľ�������������������
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
		//��Ϊ��Value��getNumber�����˶��ԣ�����Ͳ��������ͼ����
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
		//����ᷢ�����θ��ƣ�
		theValue->eraseArrayElement(index, count);
	}
	void AmJson::insertArrayElement(const AmJson& js, const size_t& index)noexcept {
		theValue->insertArrayElement(*js.theValue, index);
	}
	void AmJson::clearArray()noexcept {
		theValue->clearArray();
	}

	void AmJson::setObject()noexcept {
		//����һ���յĶ���
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
		//ͬ�������θ���
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