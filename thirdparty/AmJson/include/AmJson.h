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


	//需要用到value的成员
	class Value;


	//json接口类，只提供接口，成员函数实现将转发到value类型成员的成员函数，真正的实现由value类提供
	class AmJson final{
	private:
		std::unique_ptr<Json::Value> theValue;

		friend bool operator==(const AmJson& lhs, const AmJson& rhs) noexcept;
		friend bool operator!=(const AmJson& lhs, const AmJson& rhs) noexcept;

	public:
		AmJson() noexcept;
		~AmJson() noexcept;
		//复制和移动构造函数
		AmJson(const AmJson&) noexcept;
		AmJson(AmJson&&) noexcept;

		//复制和移动赋值
		AmJson& operator=(const AmJson& rhs)noexcept;
		AmJson& operator=(AmJson&& rhs)noexcept;

		//解析函数
		void parse(std::string content, std::string& status) noexcept;
		void parse(std::string content);
		//生成
		void stringify(std::string& content) const noexcept;

		void swap(AmJson&) noexcept;

		//一系列对外接口,通过这些函数去调用value的相应函数来实现功能，降低了文件之间的编译依赖
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
	//提供非成员函数版本的== != 和交换
	bool operator==(const AmJson& lhs, const AmJson& rhs)noexcept;
	bool operator!=(const AmJson& lhs, const AmJson& rhs)noexcept;
	void swap(AmJson& lhs, AmJson& rhs)noexcept;
}
