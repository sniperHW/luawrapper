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

int main()
{
	luaWrapper lw;
	lw.init();
	lw.load("start.lua");
	lua_State *L = *(&lw);
	//≤‚ ‘◊¢≤·»Œ“‚C++∫Ø ˝
	register_function(L,"show",showmsg);
	
	//≤‚ ‘œÚlua◊¢≤·C++¿‡
	register_class<testb,void>(L,"testb");
	register_property("valb",&testb::valb);
	register_member_function("func",&testb::function);

	register_class<testc,testb>(L,"testc");
	register_member_function("funcc",&testc::functionc);

	testc tc;
	tc.valb = 100;
	testb tb;
	tb.valb = 1000;
	call_luaFunction1<void>("test1",L,&tc);
	printf("%d\n",tc.valb);
	printf("haha\n");
	getchar();
	return 0;
}