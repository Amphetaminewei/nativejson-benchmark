#include "../test.h"
#include "AmJson/include/AmJson.h"
#include "AmJson/include/Value.h"
#include "AmJson/include/Parser.h"
#include "AmJson/include/Generator.h"
#include "AmJson/include/Exception.h"

#include "AmJson/include/AmJson.cpp"
#include "AmJson/include/Value.cpp"
#include "AmJson/include/Parser.cpp"
#include "AmJson/include/Generator.cpp"

using namespace Json;


static void GenStat(Stat& stat, const AmJson& v) {
	switch (v.getType()) {
	case Array:
		for (size_t i=0;i<v.getArraySize();i++)
			//看来有必要设计一个迭代器了
			GenStat(stat, v.getArrayElement(i));
		stat.arrayCount++;
		stat.elementCount += v.getArraySize();
		break;

	case Object:
		for (size_t i = 0; i < v.getObjectSize(); i++) {
			GenStat(stat, v.getObjectValue(i));
			stat.stringLength += v.getObjectKeyLength(i);
		}
		stat.objectCount++;
		stat.memberCount += v.getObjectSize();
		stat.stringCount += v.getObjectSize();
		break;

	case String:
		stat.stringCount++;
		stat.stringLength += v.getString().size();
		break;

	case Number:
		stat.numberCount++;
		break;

	case True:
		stat.trueCount++;
		break;

	case False:
		stat.falseCount++;
		break;

	case Null:
		stat.nullCount++;
		break;
	}
}

class AmJsonParseResult : public ParseResultBase {
public:
	AmJson root;
};

class AmJsonStringResult : public StringResultBase {
public:
	std::string s;

	virtual const char* c_str()const {
		return s.c_str();
	}
};

class AmJsonTest :public TestBase {
public:
#if TEST_INFO
	virtual const char* GetName() const { return "AmJson (C++17)"; }
	virtual const char* GetFilename() const { return __FILE__; }
#endif // TEST_INFO

#if TEST_PARSE
	virtual ParseResultBase* Parse(const char* j, size_t length)const {
		(void)length;
		AmJsonParseResult* pr = new AmJsonParseResult;
		AmJson testParse;
		try {
			//parse会将字符串解析到当前对象，并不会返回一个新的对象
			testParse.parse(j);
			pr->root = testParse;
		}
		catch (...) {
			delete pr;
			return 0;
		}
		return pr;
	}
#endif //TEST_PARSE

#if TEST_STRINGIFY
	virtual StringResultBase* Stringify(const ParseResultBase* parseResult)const {
		const AmJsonParseResult* pr = static_cast<const AmJsonParseResult*>(parseResult);
		AmJsonStringResult* sr = new AmJsonStringResult;
		std::string result;
		pr->root.stringify(result);
		sr->s = result;
		return sr;
	}
#endif // TEST_STRINGIFY


#if TEST_STATISTICS
	virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
		const AmJsonParseResult* pr = static_cast<const AmJsonParseResult*>(parseResult);
		memset(stat, 0, sizeof(Stat));
		GenStat(*stat, pr->root);
		return true;
	}
#endif // TEST_STATISTICS

#if TEST_CONFORMANCE
	virtual bool ParseDouble(const char* j, double* d)const {
		AmJson root;
		root.parse(j);
		*d = root.getArrayElement(0).getNumber();
		return true;
	}
	virtual bool ParseString(const char* j, std::string& s) const {
		try {
			AmJson root;
			root.parse(j);
			s = root.getArrayElement(0).getString();
			return true;
		}catch(...){}
		return false;
	}
#endif // TEST_CONFORMANCE
};

REGISTER_TEST(AmJsonTest);