#include "luaWrapper.h"
#include <stdio.h>

static int userdata(lua_State *L)
{
	int *a = (int*)lua_touserdata(L,-1);
	printf("a = %d\n",*a);
	return 0;
}
static int showmsg(const char *msg)
{
	printf("%s\n",msg);
	return 0;
}

class testa
{
public:
	int vala;
	int valb[10];
};

class testd
{
public:
	virtual void testdf(){}
	int vald;
};

class testb : virtual public testa,virtual public testd
{
public:
	
	virtual void function()
	{
		printf("this is function\n");
	}

	int64_t valb;
	double  vald;
};

struct st
{
	int data1;
	int data2;
};

class testc : public testb
{
public:
	void function()
	{
		printf("this is testc\n");
	}

	void functionc()
	{
		printf("functionc\n");
	}
	
	struct st    _st;
	luatable     _lt;
	std::string  str;
	const char * _str;
	int  * ptr;
};



int main()
{
	luaWrapper lw;
	lw.init();
	lw.load("start.lua");
	lua_State *L = *(&lw);
	//≤‚ ‘◊¢≤·»Œ“‚C++∫Ø ˝
	register_function(L,"show",showmsg);
	
	//≤‚ ‘œÚlua◊¢≤·C++¿‡
	register_class<testb>(L,"testb");
	register_property("valb",&testb::valb);
	register_property("vald",&testb::vald);
	register_member_function("func",&testb::function);
	
	register_class<st>(L,"st");
	register_property("data1",&st::data1);
	register_property("data1",&st::data1);	

	register_class<testc,testb>(L,"testc");
	register_property("_st",&testc::_st);
	register_property("_lt",&testc::_lt);
	register_property("str",&testc::str);
	register_property("_str",&testc::_str);
	register_property("ptr",&testc::ptr);
	register_member_function("funcc",&testc::functionc);
	
	int a;
	int b;
	
	st _st = {10,11};
	testc tc;
	tc.valb = 1000000000000000002;
	tc.vald = 10.0f;
	tc._st = _st;
	tc._lt.push_back(15);
	tc._lt.push_back(16);
	tc.str = "hahaha";
	tc._str = "fuck";
	tc.ptr = &a;
	testb tb;
	tb.valb = 1000;
	call_luaFunction<void>("test1",L,&tc,(int64_t)1000000000000000001,&b);
	printf("%lld\n",tc.valb);
	printf("%f\n",tc.vald);
	printf("%s\n",tc.str.c_str());
	printf("%s\n",tc._str);
	printf("%d,%d\n",any_cast<int>(tc._lt[0]),any_cast<int>(tc._lt[1]));
	printf("%d\n",tc.ptr == &b);
	
	luatable ret = call_luaFunction<luatable>("test2",L);
	int i=0;
	printf("{");
	for(;i<ret.size();++i)
	{
		printf("%d,",any_cast<int>(ret[i]));
	}
	printf("}\n");
		
	luaObject ac = call_luaFunction<luaObject>("test3",L);
	ac.CallMemberFunction<void>("show");
	printf("balance:%d\n",ac.GetMemberValue<int>("balance"));
	ac.SetMemberValue<int>("balance",1000);
	printf("balance:%d\n",ac.GetMemberValue<int>("balance"));
	luatable lt_in;
	for(int i = 0; i < 5;++i)
		lt_in.push_back(i);
	call_luaFunction<void>("test5",L,lt_in);
	
	call_luaFunction<void>("test6",L,"this is string");	
	
	lt_in.clear();
	lt_in.push_back((const char*)"hello1");
	lt_in.push_back((const char*)"hello2");
	lt_in.push_back((const char*)"hello3");
	lt_in.push_back((const char*)"hello4");
	lt_in.push_back((const char*)"hello5");
	call_luaFunction<void>("test5",L,lt_in);
	getchar();
	return 0;
}
