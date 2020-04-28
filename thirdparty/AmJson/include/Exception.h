#pragma once

#include <string>
#include <stdexcept>

namespace Json {
	//���ַ�ʽ����ֱ���ں��������˴��������ʱֱ���׳����󣬶�����Ҫ���ݺ������ؽ�����ж��Ƿ�ִ�гɹ�����ʡһ���жϵĿ���
	class Exception final : public std::logic_error {
	public:
		//ͨ��exception�Ĺ��캯��������Ӧ��logic_error�Ĺ��캯��
		Exception(const std::string& errorMsg) : logic_error(errorMsg) { }
	};
}