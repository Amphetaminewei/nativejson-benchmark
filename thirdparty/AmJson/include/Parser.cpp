#include "Parser.h"
#include"Exception.h"
#include<assert.h>
#include<ctype.h>
#include<errno.h>
#include<math.h>
#include<stdlib.h>

#include<iostream>

namespace Json {
	//用来判断c的第一个值是不是与ch相等，相等则跳过，用来跳过并检验第一个符号是否匹配，用来检查参数的合法性
	inline void expect(const char*& c, char ch)
	{
		assert(*c == ch);
		++c;
	}

	//对成员初始化，并调用解析和函数，真正的解析处理在parseValue
	Parser::Parser(Value& value, std::string&& content):val(value),cur(std::move(content.c_str())) {
		//跳过前空格
		parseWhitespace();
		//处理json值
		parseValue();
		//跳过后空格
		parseWhitespace();

		//如果处理完之后最后不是终结符则抛出异常
		if (*cur != '\0') {
			val.setNull();
			throw(Json::Exception("parse root not singular"));
		}
	}

	void Parser::parseWhitespace()noexcept {
		//如果是空格、换行符、回车、tab则跳过
		while (*cur == ' ' || *cur == '\n' || *cur == '\t' || *cur == '\r') {
			++cur;
		}
	}

	void Parser::parseValue() {
		//由于JSON类型判断较为简单，只需判断首个字符即可确定类型，所以这里直接判断类型并进行分发
		//分发后的函数使用断言进行再次校验
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
				val.setNull();
				throw (Exception("parse invalid value"));
			}
		}
		cur += i;
		val.setNull();
	}

	void Parser::parseBool(const char* literal, bool&& bol) {
		expect(cur, literal[0]);
		size_t i;
		for (i = 0; literal[i + 1]; ++i) {
			if (cur[i] != literal[i + 1]) {
				val.setNull();
				throw (Exception("parse invalid value"));
			}
		}
		cur += i;
		val.setValue(std::move(bol));
	}

	//解析数字
	void Parser::parseNumber() {
		//json中允许出现负数但是不能有+来显示声明整数，首字只在小数也就是0.xx的情况下可以为0

		//防止更改cur指针，以下的检查操作都使用p来检查
		const char* p = cur;
		//注意这里如果使用双引号，单引号括起来的是字符，双引号括起来的是字符串
		if (*p == '-') {
			++p;
		}
		if (*p == '0') {
			++p;
		}
		else {
			//isdigit来判断是否为0-9之间的数字
			if (!isdigit(*p)) {
				val.setNull();
				throw(Exception("parse invalid value"));
			}
			//如果不是0开头就直接往后走，直到小数点或者整数结束
			while (isdigit(*++p));
		}
		if (*p == '.') {
			//这里的处理和前面略有不同，需要将小数点先跳过，然后继续检查是否是数字
			if (!isdigit(*++p)) throw (Exception("parse invalid value"));
			while (isdigit(*++p));
		}
		if (*p == 'e' || *p == 'E') {
			//因为double本身是支持科学计数的，也不需要做额外处理，和上面的处理差不多
			//跳过e然后校验后面是不是都是数字
			++p;
			//这里是针对科学计数的加减号
			if (*p == '+' || *p == '-') { 
				++p; 
			}
			if (!isdigit(*p)) { 
				val.setNull();
				throw (Exception("parse invalid value")); 
			}
			while (isdigit(*++p));
		}
		//用来做后面的异常检查
		errno = 0;
		double v = strtod(cur, NULL);
		//因为JSON中没有限制数字大小，这里需要检查是否在double可容纳的范围内
		if (errno == ERANGE && (v == HUGE_VAL || v == -HUGE_VAL)) {
			val.setNull();
			throw (Exception("parse number too big"));
		}
		val.setValue(std::move(v));
		//上面都解析完了，所以将cur定位到p的位置，不让cur直接跟着p走是因为上面的strtod需要
		cur = p;
	}
	
	//解析字符串
	void Parser::parseString() {
		std::string s;
		//因为在处理对象类型的时候还会用到这部分功能，为了代码复用单独设计一个函数对s进行处理并校验
		//有些设计是用其他定制容器做缓冲区的，性能会更好
		parseStringRaw(s);
		val.setValue(std::move(s));
	}

	void Parser::parseStringRaw(std::string& tmp) {
		//这里是为了在解析对象的时候可以单独关键字解析出来，存在单独的位置，而不是保存为value类型

		//校验并跳过引号
		expect(cur, '\"');
		const char* p = cur;
		unsigned u = 0, u2 = 0;

		//之前在断言的地方就已经跳过了前引号了，所以这里检查的是后引号，直到后引号的位置停下
		while (*p != '\"') {
			if (*p == '\0') {
				val.setNull();
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
					//\uXXXX的解析
					parseHex4(p, u);
					if (u >= 0xD800 && u <= 0xDBFF) {
						//遇到高代理项需要把低代理项也解析进来，然后计算码点
						if (*p++ != '\\') {
							val.setNull();
							throw(Exception("parse invalid unicode surrogate"));
						}
						if (*p++ != 'u') {
							val.setNull();
							throw(Exception("parse invalid unicode surrogate"));
						}
						parseHex4(p, u2);
						if (u2 < 0xDC00 || u2>0xDFFF) {
							val.setNull();
							throw(Exception("parse invalid unicode surrogate"));
						}
						u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
					}
					parseEncodeUtf8(tmp, u);
					break;
				default:throw (Exception("parse invalid string escape"));
				}
			}
			//%x00 至 %x1F是JSON规定的不合法字符范围
			else if ((unsigned char)*p < 0x20) {
				val.setNull();
				throw (Exception("parse invalid string char"));
			}
			//如果没有转义字符也没有后引号就一直往后走，注意哦这里一直在给tmp中加内容
			//我们想要的是将json字符串中是字符串类型的内容存放到一个临时字符串中，然后再
			//将其set到value中
			else tmp += *p++;
		}
		//老样子将cur定位到解析后的位置
		cur = ++p;
	}

	// JSON字符串中的 \uXXXX 是以 16 进制表示码点 U + 0000 至 U + FFFF
	//所以这里需要解析四位16进制数字，存储为码点u
	void Parser::parseHex4(const char*& p, unsigned& u) {
		u = 0;
		for (int i = 0; i < 4; ++i) {
			char ch = *p++;
			//这里开始会出现很多位运算，而且还是带等号的，实际和+=同理
			//这里的无符号类型是因为码点莫得负数
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

	//将处理过的码点根据utf8编码表转为utf8
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
			//这里只需要进行以下判断，不需要往后移动指针，所以就不用expect了
			//而且0x10FFFF是utf-8的最大范围了，所以如果超过就是不合法字符了
			assert(u <= 0x10FFFF);
			str += static_cast<char> (0xF0 | ((u >> 18) & 0xFF));
			str += static_cast<char> (0x80 | ((u >> 12) & 0x3F));
			str += static_cast<char> (0x80 | ((u >> 6) & 0x3F));
			str += static_cast<char> (0x80 | (u & 0x3F));
		}
	}

	//解析数组
	void Parser::parseArray() {
		//注意区分[ 和 {
		//断言校验
		expect(cur, '[');
		parseWhitespace();
		//用来储存缓存的容器，在性能瓶颈时可能要换成定制容器
		std::vector<Value> tmp;
		if (*cur == ']') {
			//如果数组为空
			++cur;
			val.setValue(std::move(tmp));
			return;
		}
		for (;;) {

			//数组中存的内容交给parseValue来处理,因为数组中肯定存了很多内容所以直接进行无线循环，直到解析到]为止
			try {
				//由于JSON的数组是可以嵌套数组的，因此这里在解析数组中的数组时会有递归
				parseValue();
			}
			catch (Exception) {
				//在对其他类型解析过程中，错误都经过处理了
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
				val.setNull();
				throw(Exception("parse miss comma or square bracket"));
			}
		}
	}

	//解析对象
	void Parser::parseObject() {
		//和处理数组有点类似，但是对象都是k-y形式，需要单独处理关键字
		//在JSON的结构中至少包含一个对象，其余内容包含在这个对象中，但是对象也可以嵌套对象
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
				val.setNull();
				throw(Exception("parse miss key"));
			}
			try {
				parseStringRaw(key);
			}
			catch (Exception) {
				val.setNull();
				throw(Exception("parse miss key"));
			}

			parseWhitespace();
			if (*cur++ != ':') {
				val.setNull();
				throw(Exception("parse miss colon"));
			}
			parseWhitespace();

			try {
				//和数组一样使用parseValue处理值
				parseValue();
			}
			catch (Exception) {
				throw;
			}

			//在这里make_pair构造的就是右值
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
				val.setNull();
				throw(Exception("parse miss comma or curly bracket"));
			}
		}
	}
}