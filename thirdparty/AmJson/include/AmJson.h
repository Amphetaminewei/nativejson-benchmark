#pragma once
#include<memory>
#include<string>


namespace Json {
	enum Type : int {
		Null,
		True,
		False,
		Number,
		String,
		Array,
		Object
	};


	//��Ҫ�õ�value�ĳ�Ա
	class Value;


	//json�ӿ��ֻ࣬�ṩ�ӿڣ���Ա����ʵ�ֽ�ת����value���ͳ�Ա�ĳ�Ա������������ʵ����value���ṩ
	class AmJson final{
	private:
		std::unique_ptr<Json::Value> theValue;

		friend bool operator==(const AmJson& lhs, const AmJson& rhs) noexcept;
		friend bool operator!=(const AmJson& lhs, const AmJson& rhs) noexcept;

	public:
		AmJson() noexcept;
		~AmJson() noexcept;
		//���ƺ��ƶ����캯��
		AmJson(const AmJson&) noexcept;
		AmJson(AmJson&&) noexcept;

		//���ƺ��ƶ���ֵ
		AmJson& operator=(const AmJson& rhs)noexcept;
		AmJson& operator=(AmJson&& rhs)noexcept;

		//��������
		void parse(std::string content, std::string& status) noexcept;
		void parse(std::string content);
		//����
		void stringify(std::string& content) const noexcept;

		void swap(AmJson&) noexcept;

		//һϵ�ж���ӿ�,ͨ����Щ����ȥ����value����Ӧ������ʵ�ֹ��ܣ��������ļ�֮��ı�������
		int getType()const noexcept;
		void setNull()noexcept;
		void setBool(bool)noexcept;

		double getNumber()const noexcept;
		void setNumber(double) noexcept;

		const std::string getString()const noexcept;
		void setString(const std::string&)noexcept;

		AmJson getArrayElement(const size_t& index)const noexcept;
		size_t getArraySize()const noexcept;
		void setArray()noexcept;
		void pushbackArrayElement(const AmJson&)noexcept;
		void popbackArrayElement()noexcept;
		void insertArrayElement(const AmJson&, const size_t&)noexcept;
		void eraseArrayElement(const size_t& index, const size_t& count)noexcept;
		void clearArray()noexcept;

		void setObject()noexcept;
		size_t getObjectSize()const noexcept;
		const std::string getObjectKey(const size_t& index)const noexcept;
		size_t getObjectKeyLength(const size_t& index)const noexcept;
		AmJson getObjectValue(const size_t& index)const noexcept;
		void setObjectValue(const std::string& key, const AmJson& value)noexcept;
		long long findObjectIndex(const std::string& key)const noexcept;
		void removeObjectValue(const size_t& index)noexcept;
		void clearObject()noexcept;
	};
	//�ṩ�ǳ�Ա�����汾��== != �ͽ���
	bool operator==(const AmJson& lhs, const AmJson& rhs)noexcept;
	bool operator!=(const AmJson& lhs, const AmJson& rhs)noexcept;
	void swap(AmJson& lhs, AmJson& rhs)noexcept;
}
