// luabindTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "luaWrapper.h"
#include "luaFunction.h"
#include "luaObject.h"
#include "luaClass.h"
#include <iostream>
#include "any.h"
#include "GetParamFromLua.h"
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

	int valb;
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
};

int _tmain(int argc, _TCHAR* argv[])
{
	luaWrapper lw;
	lw.init();
	lua_State *L = *(&lw);
	kennyluainit(L,&lw);
;
	//测试注册任意C++函数
	register_function(L,"show",showmsg);
	
	//测试向lua注册C++类
	register_class<testb>(L,"testb");
	register_property("valb",&testb::valb);
	register_member_function("func",&testb::function);

	register_class<testc>(L,"testc");
	DefParent<testb,testc>();
	register_member_function("funcc",&testc::functionc);

	testc tc;
	tc.valb = 100;
	testb tb;
	tb.valb = 1000;
	testa tt;
	call_luaFunction1<void>("test1",L,&tb);
	printf("%d\n",tb.valb);
	printf("haha\n");
	getchar();
	return 0;
}