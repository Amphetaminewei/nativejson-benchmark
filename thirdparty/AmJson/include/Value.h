#pragma once

#include<variant>
#include<vector>
#include"AmJson.h"
#include"Parser.h"
#include"Generator.h"

namespace Json {
	class Value final {
	private:
		//variant�����ڸ������������ʱ�Զ�����
		//void init(const Value&)noexcept;
		//void free()noexcept;

		friend bool operator==(const Value& lhs, const Value& rhs) noexcept;

		//Json::Type theType = Json::Null;
		//std::variant������union
		std::variant<int, bool, double, std::string, std::vector<Value>, std::vector<std::pair<std::string, Value>>> value;

	public:
		template<typename T>
		void parse(T&& content);
		template<typename T>
		void stringify(T&& content)const noexcept;

		int getType()const noexcept;

		/*ʹ��һ���������������ð汾setValue����ɶ�variant��ֵ�ĸ���
		��ԭ���İ汾�������������ɺܶ���ֿ��ĺ�����ɵģ���Ϊ���ͱ�־λ�Ĵ��ڣ�
		ÿ�ν���setʱ����Ҫ����־λ��Ϊ��Ӧ��ֵ��
		�������ʹ��ģ�庯����
		���޷�ȷ��set�ľ���������ʲôҲ���޷���ȷ���ı�־λ��ֵ��
		�����ȡ���˱�־λҲ�Ϳ��Խ�������һ��ģ�庯��������������*/
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

	//�����������õ�ģ�庯�������뽫ʵ��д��ͷ�ļ���
	//�����������������������ʵ�֣����ֻҪ�����һ��ʹ�ò�����������ֱ����std::move������ֵ���Ӷ��ƶ���ֵ��value
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
