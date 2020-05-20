#include "Parser.h"
#include"Exception.h"
#include<assert.h>
#include<ctype.h>
#include<errno.h>
#include<math.h>
#include<stdlib.h>

#include<iostream>

namespace Json {
	//�����ж�c�ĵ�һ��ֵ�ǲ�����ch��ȣ���������������������������һ�������Ƿ�ƥ�䣬�����������ĺϷ���
	inline void expect(const char*& c, char ch)
	{
		assert(*c == ch);
		++c;
	}

	//�Գ�Ա��ʼ���������ý����ͺ����������Ľ���������parseValue
	Parser::Parser(Value& value, std::string&& content):val(value),cur(std::move(content.c_str())) {
		//����ǰ�ո�
		parseWhitespace();
		//����jsonֵ
		parseValue();
		//������ո�
		parseWhitespace();

		//���������֮��������ս�����׳��쳣
		if (*cur != '\0') {
			val.setValue(1);
			throw(Json::Exception("parse root not singular"));
		}
	}

	void Parser::parseWhitespace()noexcept {
		//����ǿո񡢻��з����س���tab������
		while (*cur == ' ' || *cur == '\n' || *cur == '\t' || *cur == '\r') {
			++cur;
		}
	}

	void Parser::parseValue() {
		//����JSON�����жϽ�Ϊ�򵥣�ֻ���ж��׸��ַ�����ȷ�����ͣ���������ֱ���ж����Ͳ����зַ�
		//�ַ���ĺ���ʹ�ö��Խ����ٴ�У��
		switch (*cur) {
		case 'n': parseNull("null");  return;
		case 't': parseBool("true", true);  return;
		case 'f': parseBool("false", false); return;
		case '\"': parseString(); return;
		case '[': parseArray();  return;
		case '{': parseObject(); return;
		default: parseNumber(); return;
		case '\0': throw(Exception("parse expect value"));
		}
	}


	void Parser::parseNull(const char* literal) {
		expect(cur, literal[0]);
		size_t i;
		for (i = 0; literal[i + 1]; ++i) {
			if (cur[i] != literal[i + 1]) {
				val.setValue(1);
				throw (Exception("parse invalid value"));
			}
		}
		cur += i;
		val.setValue(1);
	}

	void Parser::parseBool(const char* literal, bool&& bol) {
		expect(cur, literal[0]);
		size_t i;
		for (i = 0; literal[i + 1]; ++i) {
			if (cur[i] != literal[i + 1]) {
				val.setValue(1);
				throw (Exception("parse invalid value"));
			}
		}
		cur += i;
		val.setValue(std::move(bol));
	}

	//��������
	void Parser::parseNumber() {
		//json��������ָ������ǲ�����+����ʾ��������������ֻ��С��Ҳ����0.xx������¿���Ϊ0

		//��ֹ����curָ�룬���µļ�������ʹ��p�����
		const char* p = cur;
		//ע���������ʹ��˫���ţ������������������ַ���˫���������������ַ���
		if (*p == '-') {
			++p;
		}
		if (*p == '0') {
			++p;
		}
		else {
			//isdigit���ж��Ƿ�Ϊ0-9֮�������
			if (!isdigit(*p)) {
				val.setValue(1);
				throw(Exception("parse invalid value"));
			}
			//�������0��ͷ��ֱ�������ߣ�ֱ��С���������������
			while (isdigit(*++p));
		}
		if (*p == '.') {
			//����Ĵ����ǰ�����в�ͬ����Ҫ��С������������Ȼ���������Ƿ�������
			if (!isdigit(*++p)) throw (Exception("parse invalid value"));
			while (isdigit(*++p));
		}
		if (*p == 'e' || *p == 'E') {
			//��Ϊdouble������֧�ֿ�ѧ�����ģ�Ҳ����Ҫ�����⴦��������Ĵ�����
			//����eȻ��У������ǲ��Ƕ�������
			++p;
			//��������Կ�ѧ�����ļӼ���
			if (*p == '+' || *p == '-') { 
				++p; 
			}
			if (!isdigit(*p)) { 
				val.setValue(1);
				throw (Exception("parse invalid value")); 
			}
			while (isdigit(*++p));
		}
		//������������쳣���
		errno = 0;
		double v = strtod(cur, NULL);
		//��ΪJSON��û���������ִ�С��������Ҫ����Ƿ���double�����ɵķ�Χ��
		if (errno == ERANGE && (v == HUGE_VAL || v == -HUGE_VAL)) {
			val.setValue(1);
			throw (Exception("parse number too big"));
		}
		val.setValue(std::move(v));
		//���涼�������ˣ����Խ�cur��λ��p��λ�ã�����curֱ�Ӹ���p������Ϊ�����strtod��Ҫ
		cur = p;
	}
	
	//�����ַ���
	void Parser::parseString() {
		std::string s;
		//��Ϊ�ڴ���������͵�ʱ�򻹻��õ��ⲿ�ֹ��ܣ�Ϊ�˴��븴�õ������һ��������s���д���У��
		//��Щ����������������������������ģ����ܻ����
		parseStringRaw(s);
		val.setValue(std::move(s));
	}

	void Parser::parseStringRaw(std::string& tmp) {
		//������Ϊ���ڽ��������ʱ����Ե����ؼ��ֽ������������ڵ�����λ�ã������Ǳ���Ϊvalue����
		//��Ϊ�������˷�value��type��Ա�����һ���Ӹ���

		//У�鲢��������
		expect(cur, '\"');
		const char* p = cur;
		unsigned u = 0, u2 = 0;

		//֮ǰ�ڶ��Եĵط����Ѿ�������ǰ�����ˣ�������������Ǻ����ţ�ֱ�������ŵ�λ��ͣ��
		while (*p != '\"') {
			if (*p == '\0') {
				val.setValue(1);
				throw(Exception("parse miss quotation mark"));
			}
			if (*p == '\\' && ++p) {
				switch (*p++) {
				case '\"': tmp += '\"'; break;
				case '\\': tmp += '\\'; break;
				case '/': tmp += '/'; break;
				case 'b': tmp += '\b'; break;
				case 'f': tmp += '\f'; break;
				case 'n': tmp += '\n'; break;
				case 'r': tmp += '\r'; break;
				case 't': tmp += '\t'; break;
				case'u':
					//\uXXXX�Ľ���
					parseHex4(p, u);
					if (u >= 0xD800 && u <= 0xDBFF) {
						//�����ߴ�������Ҫ�ѵʹ�����Ҳ����������Ȼ��������
						if (*p++ != '\\') {
							val.setValue(1);
							throw(Exception("parse invalid unicode surrogate"));
						}
						if (*p++ != 'u') {
							val.setValue(1);
							throw(Exception("parse invalid unicode surrogate"));
						}
						parseHex4(p, u2);
						if (u2 < 0xDC00 || u2>0xDFFF) {
							val.setValue(1);
							throw(Exception("parse invalid unicode surrogate"));
						}
						u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
					}
					parseEncodeUtf8(tmp, u);
					break;
				default:throw (Exception("parse invalid string escape"));
				}
			}
			//%x00 �� %x1F��JSON�涨�Ĳ��Ϸ��ַ���Χ
			else if ((unsigned char)*p < 0x20) {
				val.setValue(1);
				throw (Exception("parse invalid string char"));
			}
			//���û��ת���ַ�Ҳû�к����ž�һֱ�����ߣ�ע��Ŷ����һֱ�ڸ�tmp�м�����
			//������Ҫ���ǽ�json�ַ��������ַ������͵����ݴ�ŵ�һ����ʱ�ַ����У�Ȼ����
			//����set��value��
			else tmp += *p++;
		}
		//�����ӽ�cur��λ���������λ��
		cur = ++p;
	}

	// JSON�ַ����е� \uXXXX ���� 16 ���Ʊ�ʾ��� U + 0000 �� U + FFFF
	//����������Ҫ������λ16�������֣��洢Ϊ���u
	void Parser::parseHex4(const char*& p, unsigned& u) {
		u = 0;
		for (int i = 0; i < 4; ++i) {
			char ch = *p++;
			//���￪ʼ����ֺܶ�λ���㣬���һ��Ǵ��Ⱥŵģ�ʵ�ʺ�+=ͬ��
			//������޷�����������Ϊ���Ī�ø���
			u <<= 4;
			if (isdigit(ch)){
				u |= ch - '0';
			}
			else if (ch >= 'A' && ch <= 'F') {
				u |= ch - ('A' - 10);
			}
			else if (ch > 'a' && ch <= 'f') {
				u |= ch - ('a' - 10);
			}
			else throw(Exception("parse invalid unicode hex"));
		}
	}

	//���������������utf8�����תΪutf8
	void Parser::parseEncodeUtf8(std::string& str,unsigned u)const noexcept{
		if (u <= 0x7F) {
			str += static_cast<char>(u & 0xFF);
		}
		else if (u <= 0x7FF) {
			str += static_cast<char> (0xC0 | ((u >> 6) & 0xFF));
			str += static_cast<char> (0x80 | (u & 0x3F));
		}
		else if (u <= 0xFFFF) {
			str += static_cast<char> (0xE0 | ((u >> 12) & 0xFF));
			str += static_cast<char> (0x80 | ((u >> 6) & 0x3F));
			str += static_cast<char> (0x80 | (u & 0x3F));
		}
		else {
			//����ֻ��Ҫ���������жϣ�����Ҫ�����ƶ�ָ�룬���ԾͲ���expect��
			//����0x10FFFF��utf-8�����Χ�ˣ���������������ǲ��Ϸ��ַ���
			assert(u <= 0x10FFFF);
			str += static_cast<char> (0xF0 | ((u >> 18) & 0xFF));
			str += static_cast<char> (0x80 | ((u >> 12) & 0x3F));
			str += static_cast<char> (0x80 | ((u >> 6) & 0x3F));
			str += static_cast<char> (0x80 | (u & 0x3F));
		}
	}

	//��������
	void Parser::parseArray() {
		//ע������[ �� {
		//����У��
		expect(cur, '[');
		parseWhitespace();
		//�������滺���������������ƿ��ʱ����Ҫ���ɶ�������
		std::vector<Value> tmp;
		if (*cur == ']') {
			//�������Ϊ��
			++cur;
			val.setValue(std::move(tmp));
			return;
		}
		for (;;) {

			//�����д�����ݽ���parseValue������,��Ϊ�����п϶����˺ܶ���������ֱ�ӽ�������ѭ����ֱ��������]Ϊֹ
			try {
				//����JSON�������ǿ���Ƕ������ģ���������ڽ��������е�����ʱ���еݹ�
				parseValue();
			}
			catch (Exception) {
				//�ڶ��������ͽ��������У����󶼾���������
				throw;
			}
			tmp.push_back(std::move(val));
			parseWhitespace();
			if (*cur == ',') {
				++cur;
				parseWhitespace();
			}
			else if (*cur == ']') {
				++cur;
				val.setValue(std::move(tmp));
				return;
			}
			else {
				val.setValue(1);
				throw(Exception("parse miss comma or square bracket"));
			}
		}
	}

	//��������
	void Parser::parseObject() {
		//�ʹ��������е����ƣ����Ƕ�����k-y��ʽ����Ҫ��������ؼ���
		//��JSON�Ľṹ�����ٰ���һ�������������ݰ�������������У����Ƕ���Ҳ����Ƕ�׶���
		expect(cur, '{');
		parseWhitespace();
		std::vector<std::pair<std::string, Value>> tmp;
		std::string key;
		if (*cur == '}') {
			++cur;
			val.setValue(std::move(tmp));
			return;
		}
		for (;;) {
			if (*cur != '\"') {
				val.setValue(1);
				throw(Exception("parse miss key"));
			}
			try {
				parseStringRaw(key);
			}
			catch (Exception) {
				val.setValue(1);
				throw(Exception("parse miss key"));
			}

			parseWhitespace();
			if (*cur++ != ':') {
				val.setValue(1);
				throw(Exception("parse miss colon"));
			}
			parseWhitespace();

			try {
				//������һ��ʹ��parseValue����ֵ
				parseValue();
			}
			catch (Exception) {
				throw;
			}

			//������make_pair����ľ�����ֵ
			tmp.push_back(std::move(make_pair(key, val)));
			key.clear();

			parseWhitespace();
			if (*cur == ',') {
				++cur;
				parseWhitespace();
			}
			else if (*cur == '}') {
				++cur;
				val.setValue(std::move(tmp));
				return;
			}
			else {
				val.setValue(1);
				throw(Exception("parse miss comma or curly bracket"));
			}
		}
	}
}