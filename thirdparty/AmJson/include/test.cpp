#include <iostream>
#include <string.h>
#include "AmJson.h"

static int mainRet = 0;
static int testCount = 0;
static int testPass = 0;
static std::string status;

#define EXPECT_EQ_BASE(expect, actual) \
	do {\
		++testCount;\
		if (expect == actual){\
			++testPass;\
		}\
		else {\
			std::cerr << __FILE__ << ":" << __LINE__ << ": expect: " << expect << " actual: " << actual << std::endl;\
			mainRet = 1;\
		}\
	} while(0)


//以下为解析测试

#define TEST_LITERAL(expect, content)\
	do {\
		Json::AmJson l;\
		l.setBool(false);\
		l.parse(content, status);\
		EXPECT_EQ_BASE("parse ok", status);\
		EXPECT_EQ_BASE(expect, l.getType());\
	} while (0)

static void testParseLiteral()
{
	//可以在这添加测试用例
	TEST_LITERAL(Json::Null, "null");
	TEST_LITERAL(Json::True, "true");
	TEST_LITERAL(Json::False, "false");
}



#define TEST_NUMBER(expect, content)\
	do {\
		Json::AmJson n;\
		n.parse(content, status);\
		EXPECT_EQ_BASE("parse ok", status);\
		EXPECT_EQ_BASE(Json::Number, n.getType());\
		EXPECT_EQ_BASE(expect, n.getNumber());\
	} while(0)

static void testParseNumber() {
	TEST_NUMBER(0.0, "0");
	TEST_NUMBER(0.0, "-0");
	TEST_NUMBER(0.0, "-0.0");
	TEST_NUMBER(1.0, "1");
	TEST_NUMBER(-1.0, "-1");
	TEST_NUMBER(1.5, "1.5");
	TEST_NUMBER(-1.5, "-1.5");
	TEST_NUMBER(3.1416, "3.1416");
	//科学计数测试
	TEST_NUMBER(1E10, "1E10");
	TEST_NUMBER(1e10, "1e10");
	TEST_NUMBER(1E+10, "1E+10");
	TEST_NUMBER(1E-10, "1E-10");
	TEST_NUMBER(-1E10, "-1E10");
	TEST_NUMBER(-1e10, "-1e10");
	TEST_NUMBER(-1E+10, "-1E+10");
	TEST_NUMBER(-1E-10, "-1E-10");
	TEST_NUMBER(1.234E+10, "1.234E+10");
	TEST_NUMBER(1.234E-10, "1.234E-10");
	//前后不等，解析成功但是因为1e-10000是一个非常小的值，最后的解析结果会是0，这是double的问题
	TEST_NUMBER(0.0, "1e-10000");

	//测试超过double范围的情况，会在第三次调用EXPECT_EQ_BASE的时候被断言截住，因为解析失败的情况下，类型会是Json::Null
	//而在第三次调用EXPECT_EQ_BASE时会调用Json::getNumber,会触发Value::getNumber的断言
//	TEST_NUMBER(1.7976931348623158e+308, "2.7976931348623158e+308");
}

//除了类型，解析结果之外，还进行了
#define TEST_STRING(expect, content)\
	do {\
		Json::AmJson s;\
		s.parse(content, status);\
		EXPECT_EQ_BASE("parse ok", status);\
		EXPECT_EQ_BASE(Json::String, s.getType());\
		EXPECT_EQ_BASE(0, memcmp(expect, s.getString().c_str(), s.getString().size()));\
	} while(0)

#define EXPECT_EQ_STRING(expect, actual) EXPECT_EQ_BASE(0, memcmp(expect, actual.c_str(), actual.size()));

static void testParseString() {
	TEST_STRING("", "\"\"");
	TEST_STRING("Hello", "\"Hello\"");

	TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
	TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");

	TEST_STRING("Hello\0World", "\"Hello\\u0000World\"");
	TEST_STRING("\x24", "\"\\u0024\"");         //U+0024 
	TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     //U+00A2
	TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); // U+20AC
	TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  //U+1D11E 
	TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  //G clef sign U+1D11E
}

static void testParseArray()
{
	Json::AmJson a;

	a.parse("[ ]", status);
	EXPECT_EQ_BASE("parse ok", status);
	EXPECT_EQ_BASE(Json::Array, a.getType());
	EXPECT_EQ_BASE(0, a.getArraySize());

	a.parse("[ null , false , true , 123 , \"abc\" ]", status);
	EXPECT_EQ_BASE("parse ok", status);
	EXPECT_EQ_BASE(Json::Array, a.getType());
	//检查长度
	EXPECT_EQ_BASE(5, a.getArraySize());
	//检查数组中内容的类型
	EXPECT_EQ_BASE(Json::Null, a.getArrayElement(0).getType());
	EXPECT_EQ_BASE(Json::False, a.getArrayElement(1).getType());
	EXPECT_EQ_BASE(Json::True, a.getArrayElement(2).getType());
	EXPECT_EQ_BASE(Json::Number, a.getArrayElement(3).getType());
	EXPECT_EQ_BASE(Json::String, a.getArrayElement(4).getType());
	//检查内容是否一致
	EXPECT_EQ_BASE(123.0, a.getArrayElement(3).getNumber());
	EXPECT_EQ_STRING("abc", a.getArrayElement(4).getString());

	//对于嵌套数组的检查
	a.parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]", status);
	EXPECT_EQ_BASE("parse ok", status);
	EXPECT_EQ_BASE(Json::Array, a.getType());
	EXPECT_EQ_BASE(4, a.getArraySize());
	for (int i = 0; i < 4; ++i) {
		Json::AmJson item = a.getArrayElement(i);
		EXPECT_EQ_BASE(Json::Array, item.getType());
		EXPECT_EQ_BASE(i, item.getArraySize());
		for (int j = 0; j < i; ++j) {
			Json::AmJson e = item.getArrayElement(j);
			EXPECT_EQ_BASE(Json::Number, e.getType());
			EXPECT_EQ_BASE((double)j, e.getNumber());

		}
	}
}

static void testParseObject()
{
	Json::AmJson v;
	
	v.parse(" { } ", status);
	EXPECT_EQ_BASE("parse ok", status);
	EXPECT_EQ_BASE(Json::Object, v.getType());
	EXPECT_EQ_BASE(0, v.getObjectSize());
	v.parse(" { "
		"\"n\" : null , "
		"\"f\" : false , "
		"\"t\" : true , "
		"\"i\" : 123 , "
		"\"s\" : \"abc\", "
		"\"a\" : [ 1, 2, 3 ],"
		"\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
		" } ", status);

	EXPECT_EQ_BASE("parse ok", status);
	EXPECT_EQ_BASE(7, v.getObjectSize());
	EXPECT_EQ_BASE("n", v.getObjectKey(0));
	EXPECT_EQ_BASE(Json::Null, v.getObjectValue(0).getType());
	EXPECT_EQ_BASE("f", v.getObjectKey(1));
	EXPECT_EQ_BASE(Json::False, v.getObjectValue(1).getType());
	EXPECT_EQ_BASE("t", v.getObjectKey(2));
	EXPECT_EQ_BASE(Json::True, v.getObjectValue(2).getType());
	EXPECT_EQ_BASE("i", v.getObjectKey(3));
	EXPECT_EQ_BASE(Json::Number, v.getObjectValue(3).getType());
	EXPECT_EQ_BASE(123.0, v.getObjectValue(3).getNumber());
	EXPECT_EQ_BASE("s", v.getObjectKey(4));
	EXPECT_EQ_BASE(Json::String, v.getObjectValue(4).getType());
	EXPECT_EQ_BASE("abc", v.getObjectValue(4).getString());
	EXPECT_EQ_BASE("a", v.getObjectKey(5));
	EXPECT_EQ_BASE(Json::Array, v.getObjectValue(5).getType());
	EXPECT_EQ_BASE(3, v.getObjectValue(5).getArraySize());
	for (int i = 0; i < 3; ++i) {
		Json::AmJson e = v.getObjectValue(5).getArrayElement(i);
		EXPECT_EQ_BASE(Json::Number, e.getType());
		EXPECT_EQ_BASE(i + 1.0, e.getNumber());
	}
	EXPECT_EQ_BASE("o", v.getObjectKey(6));
	{
		Json::AmJson o = v.getObjectValue(6);
		EXPECT_EQ_BASE(Json::Object, o.getType());
		for (int i = 0; i < 3; ++i) {
			Json::AmJson ov = o.getObjectValue(i);
			EXPECT_EQ_BASE('1' + i, (o.getObjectKey(i))[0]);
			EXPECT_EQ_BASE(1, o.getObjectKeyLength(i));
			EXPECT_EQ_BASE(Json::Number, ov.getType());
			EXPECT_EQ_BASE(i + 1.0, ov.getNumber());
		}
	}

}


#define TEST_ERROR(error, content) \
	do {\
		Json::AmJson err;\
		err.parse(content, status);\
		EXPECT_EQ_BASE(error, status);\
		EXPECT_EQ_BASE((Json::Null), err.getType());\
	} while(0)
static void testParseExpectValue() {
	TEST_ERROR("parse expect value", "");
	TEST_ERROR("parse expect value", " ");
}
static void testParseInvalidValue() {
	TEST_ERROR("parse invalid value", "nul");
	TEST_ERROR("parse invalid value", "!?");

	TEST_ERROR("parse invalid value", "+0");
	TEST_ERROR("parse invalid value", "+1");
	TEST_ERROR("parse invalid value", ".123");
	TEST_ERROR("parse invalid value", "1.");
	TEST_ERROR("parse invalid value", "INF");
	TEST_ERROR("parse invalid value", "inf");
	TEST_ERROR("parse invalid value", "NAN");
	TEST_ERROR("parse invalid value", "nan");

	TEST_ERROR("parse invalid value", "[1,]");
	TEST_ERROR("parse invalid value", "[\"a\", nul]");
}

static void testParseRootNotSingular() {
	TEST_ERROR("parse root not singular", "null x");
	TEST_ERROR("parse root not singular", "truead");
	TEST_ERROR("parse root not singular", "\"dsad\"d");

	TEST_ERROR("parse root not singular", "0123");
	TEST_ERROR("parse root not singular", "0x0");
	TEST_ERROR("parse root not singular", "0x123");

}

static void testParseNumberTooBig() {
	TEST_ERROR("parse number too big", "1e309");
	TEST_ERROR("parse number too big", "-1e309");
}

static void testParseMissingQuotationMark() {
	TEST_ERROR("parse miss quotation mark", "\"");
	TEST_ERROR("parse miss quotation mark", "\"abc");
}

static void testParseInvalidStringEscape() {
	TEST_ERROR("parse invalid string escape", "\"\\v\"");
	TEST_ERROR("parse invalid string escape", "\"\\'\"");
	TEST_ERROR("parse invalid string escape", "\"\\0\"");
	TEST_ERROR("parse invalid string escape", "\"\\x12\"");
}

static void testParseInvalidStringChar() {
	TEST_ERROR("parse invalid string char", "\"\x01\"");
	TEST_ERROR("parse invalid string char", "\"\x1F\"");
}

static void testParseInvalidUnicodeHex() {
	TEST_ERROR("parse invalid unicode hex", "\"\\u\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u0\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u01\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u012\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u/000\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\uG000\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u0/00\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u0G00\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u0/00\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u00G0\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u000/\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u000G\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u 123\"");
}

static void testParseInvalidUnicodeSurrogate() {
	TEST_ERROR("parse invalid unicode surrogate", "\"\\uD800\"");
	TEST_ERROR("parse invalid unicode surrogate", "\"\\uDBFF\"");
	TEST_ERROR("parse invalid unicode surrogate", "\"\\uD800\\\\\"");
	TEST_ERROR("parse invalid unicode surrogate", "\"\\uD800\\uDBFF\"");
	TEST_ERROR("parse invalid unicode surrogate", "\"\\uD800\\uE000\"");
}

static void testParseMissCommaOrSquareBracket() {
	TEST_ERROR("parse miss comma or square bracket", "[1");
	TEST_ERROR("parse miss comma or square bracket", "[1}");
	TEST_ERROR("parse miss comma or square bracket", "[1 2");
	TEST_ERROR("parse miss comma or square bracket", "[[]");
}

static void testParseMissKey() {
	TEST_ERROR("parse miss key", "{:1,");
	TEST_ERROR("parse miss key", "{1:1,");
	TEST_ERROR("parse miss key", "{true:1,");
	TEST_ERROR("parse miss key", "{false:1,");
	TEST_ERROR("parse miss key", "{null:1,");
	TEST_ERROR("parse miss key", "{[]:1,");
	TEST_ERROR("parse miss key", "{{}:1,");
	TEST_ERROR("parse miss key", "{\"a\":1,");
}

static void testParseMissColon() {
	TEST_ERROR("parse miss colon", "{\"a\"}");
	TEST_ERROR("parse miss colon", "{\"a\",\"b\"}");
}

static void testParseMissCommaOrCurlyBracket()
{
	TEST_ERROR("parse miss comma or curly bracket", "{\"a\":1");
	TEST_ERROR("parse miss comma or curly bracket", "{\"a\":1]");
	TEST_ERROR("parse miss comma or curly bracket", "{\"a\":1 \"b\"");
	TEST_ERROR("parse miss comma or curly bracket", "{\"a\":{}");
}

static void testParse() {
	//不同类型解析测试
	testParseLiteral();
	testParseNumber();
	testParseString();
	testParseArray();
	testParseObject();

	//解析错误测试
	testParseExpectValue();
	testParseInvalidValue();
	testParseRootNotSingular();
	testParseNumberTooBig();
	testParseMissingQuotationMark();
	testParseInvalidStringEscape();
	testParseInvalidStringChar();
	testParseInvalidUnicodeHex();
	testParseInvalidUnicodeSurrogate();
	testParseMissCommaOrSquareBracket();
	testParseMissKey();
	testParseMissColon();
	testParseMissCommaOrCurlyBracket();
}


//以下是生成测试

//先解析在生成，对比生成之后的结果和最初输入是否一致
#define TEST_ROUNDTRIP(content)\
	do {\
		Json::AmJson v;\
		v.parse(content, status);\
		EXPECT_EQ_BASE("parse ok", status);\
		v.stringify(status);\
		EXPECT_EQ_BASE(content, status);\
	} while(0)

static void testStringifyNumber() {
	TEST_ROUNDTRIP("0");
	TEST_ROUNDTRIP("-0");
	TEST_ROUNDTRIP("1");
	TEST_ROUNDTRIP("-1");
	TEST_ROUNDTRIP("1.5");
	TEST_ROUNDTRIP("-1.5");
	TEST_ROUNDTRIP("3.25");
	TEST_ROUNDTRIP("1e+20");
	TEST_ROUNDTRIP("1.234e+20");
	TEST_ROUNDTRIP("1.234e-20");

	TEST_ROUNDTRIP("1.0000000000000002");
	TEST_ROUNDTRIP("4.9406564584124654e-324");
	TEST_ROUNDTRIP("-4.9406564584124654e-324");
	TEST_ROUNDTRIP("2.2250738585072009e-308");
	TEST_ROUNDTRIP("-2.2250738585072009e-308");
	TEST_ROUNDTRIP("2.2250738585072014e-308");
	TEST_ROUNDTRIP("-2.2250738585072014e-308");
	TEST_ROUNDTRIP("1.7976931348623157e+308");
	TEST_ROUNDTRIP("-1.7976931348623157e+308");
}

static void testStringifyString() {
	TEST_ROUNDTRIP("\"\"");
	TEST_ROUNDTRIP("\"Hello\"");
	TEST_ROUNDTRIP("\"Hello\\nWorld\"");
	TEST_ROUNDTRIP("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
	TEST_ROUNDTRIP("\"Hello\\u0000World\"");
}

static void testStringifyArray() {
	TEST_ROUNDTRIP("[]");
	TEST_ROUNDTRIP("[null,false,true,123,\"abc\",[1,2,3]]");
}

static void testStringifyObject() {
	TEST_ROUNDTRIP("{}");
	TEST_ROUNDTRIP("{\"n\":null,\"f\":false,\"t\":true,\"i\":123,\"s\":\"abc\",\"a\":[1,2,3],\"o\":{\"1\":1,\"2\":2,\"3\":3}}");
}


static void testStringify() {
	TEST_ROUNDTRIP("null");
	TEST_ROUNDTRIP("false");
	TEST_ROUNDTRIP("true");
	testStringifyNumber();
	testStringifyString();
	testStringifyArray();
	testStringifyObject();
}


//检查json1, json2是否相等，数组是有序的所以[1,2,3]!=[3,2,1],而对象是无序的，因此

#define TEST_EQUAL(json1, json2, equality)\
	do {\
		Json::AmJson v1, v2;\
		v1.parse(json1, status);\
		EXPECT_EQ_BASE("parse ok", status);\
		v2.parse(json2, status);\
		EXPECT_EQ_BASE("parse ok", status);\
		EXPECT_EQ_BASE(equality, int(v1 == v2));\
	} while(0)

static void testEqual() {
	TEST_EQUAL("true", "true", 1);
	TEST_EQUAL("true", "false", 0);
	TEST_EQUAL("false", "false", 1);
	TEST_EQUAL("null", "null", 1);
	TEST_EQUAL("null", "0", 0);
	TEST_EQUAL("123", "123", 1);
	TEST_EQUAL("123", "456", 0);
	TEST_EQUAL("\"abc\"", "\"abc\"", 1);
	TEST_EQUAL("\"abc\"", "\"abcd\"", 0);
	TEST_EQUAL("[]", "[]", 1);
	TEST_EQUAL("[]", "null", 0);
	TEST_EQUAL("[1,2,3]", "[1,2,3]", 1);
	TEST_EQUAL("[1,2,3]", "[1,2,3,4]", 0);
	TEST_EQUAL("[[]]", "[[]]", 1);
	TEST_EQUAL("{}", "{}", 1);
	TEST_EQUAL("{}", "null", 0);
	TEST_EQUAL("{}", "[]", 0);
	TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2}", 1);
	TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"b\":2,\"a\":1}", 1);
	TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":3}", 0);
	TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2,\"c\":3}", 0);
	TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":{}}}}", 1);
	TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":[]}}}", 0);
}

//测试 operatpr= 的复制和移动实现
static void testCopy() {
	Json::AmJson v1, v2;
	v1.parse("{\"t\":true,\"f\":false,\"n\":null,\"d\":1.5,\"a\":[1,2,3]}");
	v2 = v1;
	EXPECT_EQ_BASE(1, int(v2 == v1));
}


static void testMove() {
	Json::AmJson v1, v2, v3;
	v1.parse("{\"t\":true,\"f\":false,\"n\":null,\"d\":1.5,\"a\":[1,2,3]}");
	v2 = v1;
	v3 = std::move(v2);
	EXPECT_EQ_BASE(Json::Null, v2.getType());
	EXPECT_EQ_BASE(1, int(v3 == v1));
}


static void testSwap() {
	Json::AmJson v1, v2;
	v1.setString("Hello");
	v2.setString("World!");
	Json::swap(v1, v2);
	EXPECT_EQ_BASE("World!", v1.getString());
	EXPECT_EQ_BASE("Hello", v2.getString());
}


//针对set的测试
static void testAccessNull() {
	Json::AmJson v;
	v.setString("a");
	v.setNull();
	EXPECT_EQ_BASE(Json::Null, v.getType());
}
static void testAccessBoolean() {
	Json::AmJson v;
	v.setString("a");
	v.setBool(false);
	EXPECT_EQ_BASE(Json::False, v.getType());
}
static void testAccessNumber() {
	Json::AmJson v;
	v.setString("a");
	v.setNumber(1234.5);
	EXPECT_EQ_BASE(1234.5, v.getNumber());
}
static void testAccessString() {
	Json::AmJson v;
	v.setString("");
	EXPECT_EQ_STRING("", v.getString());
	v.setString("Hello");
	EXPECT_EQ_STRING("Hello", v.getString());
}
static void testAccessArray() {
	Json::AmJson a, e;

	for (size_t j = 0; j < 5; j += 5) {
		a.setArray();
		EXPECT_EQ_BASE(0, a.getArraySize());
		for (int i = 0; i < 10; ++i) {
			e.setNumber(i);
			a.pushbackArrayElement(e);
		}

		EXPECT_EQ_BASE(10, a.getArraySize());
		for (int i = 0; i < 10; ++i)
			EXPECT_EQ_BASE(static_cast<double>(i), a.getArrayElement(i).getNumber());
	}

	a.popbackArrayElement();
	EXPECT_EQ_BASE(9, a.getArraySize());
	for (int i = 0; i < 9; ++i)
		EXPECT_EQ_BASE(static_cast<double>(i), a.getArrayElement(i).getNumber());

	a.eraseArrayElement(4, 0);
	EXPECT_EQ_BASE(9, a.getArraySize());
	for (int i = 0; i < 9; ++i)
		EXPECT_EQ_BASE(static_cast<double>(i), a.getArrayElement(i).getNumber());

	a.eraseArrayElement(8, 1);
	EXPECT_EQ_BASE(8, a.getArraySize());
	for (int i = 0; i < 8; ++i)
		EXPECT_EQ_BASE(static_cast<double>(i), a.getArrayElement(i).getNumber());

	a.eraseArrayElement(0, 2);
	EXPECT_EQ_BASE(6, a.getArraySize());
	for (int i = 0; i < 6; ++i)
		EXPECT_EQ_BASE(static_cast<double>(i) + 2, a.getArrayElement(i).getNumber());

	for (int i = 0; i < 2; ++i) {
		e.setNumber(i);
		a.insertArrayElement(e, i);
	}

	EXPECT_EQ_BASE(8, a.getArraySize());
	for (int i = 0; i < 8; ++i) {
		EXPECT_EQ_BASE(static_cast<double>(i), a.getArrayElement(i).getNumber());
	}

	e.setString("Hello");
	a.pushbackArrayElement(e);

	a.clearArray();
	EXPECT_EQ_BASE(0, a.getArraySize());
}
static void testAccessObject() {
	Json::AmJson o, v;

	for (int j = 0; j <= 5; j += 5) {
		o.setObject();
		EXPECT_EQ_BASE(0, o.getObjectSize());
		for (int i = 0; i < 10; ++i) {
			std::string key = "a";
			key[0] += i;
			v.setNumber(i);
			o.setObjectValue(key, v);
		}
		EXPECT_EQ_BASE(10, o.getObjectSize());
		for (int i = 0; i < 10; ++i) {
			std::string key = "a";
			key[0] += i;
			auto index = o.findObjectIndex(key);
			EXPECT_EQ_BASE(1, static_cast<int>(index >= 0));
			v = o.getObjectValue(index);
			EXPECT_EQ_BASE(static_cast<double>(i), v.getNumber());
		}
	}

	auto index = o.findObjectIndex("j");
	EXPECT_EQ_BASE(1, static_cast<int>(index >= 0));
	o.removeObjectValue(index);
	index = o.findObjectIndex("j");
	EXPECT_EQ_BASE(1, static_cast<int>(index < 0));
	EXPECT_EQ_BASE(9, o.getObjectSize());

	index = o.findObjectIndex("a");
	EXPECT_EQ_BASE(1, static_cast<int>(index >= 0));
	o.removeObjectValue(index);
	index = o.findObjectIndex("a");
	EXPECT_EQ_BASE(1, static_cast<int>(index < 0));
	EXPECT_EQ_BASE(8, o.getObjectSize());

	for (int i = 0; i < 8; i++) {
		std::string key = "a";
		key[0] += i + 1;
		EXPECT_EQ_BASE((double)i + 1, o.getObjectValue(o.findObjectIndex(key)).getNumber());
	}

	v.setString("Hello");
	o.setObjectValue("World", v);

	index = o.findObjectIndex("World");
	EXPECT_EQ_BASE(1, static_cast<int>(index >= 0));
	v = o.getObjectValue(index);
	EXPECT_EQ_STRING("Hello", v.getString());

	o.clearObject();
	EXPECT_EQ_BASE(0, o.getObjectSize());

}

static void testAccess() {
	testAccessNull();
	testAccessBoolean();
	testAccessNumber();
	testAccessString();
	testAccessArray();
	testAccessObject();
}
int main() {
	testParse();
	testStringify();
	testEqual();
	testCopy();
	testMove();
	testSwap();
	testAccess();
	printf("%d/%d (%3.2f%%) passed\n", testPass, testCount, testPass * 100.0 / testCount);
	return mainRet;
}