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
	register_class<testb>(L,"testb");
	register_property("valb",&testb::valb);
	register_member_function("func",&testb::function);

	register_class<testc,testb>(L,"testc");
	register_member_function("funcc",&testc::functionc);

	testc tc;
	tc.valb = 100;
	testb tb;
	tb.valb = 1000;
	call_luaFunction<void>("test1",L,&tc,(int64_t)1000000000000000000);
	printf("%d\n",tc.valb);
	
	luatable ret = call_luaFunction<luatable>("test2",L);
	int i=0;
	printf("{");
	for(;i<ret.size();++i)
	{
		printf("%d,",(int)any_cast<int64_t>(ret[i]));
	}
	printf("}\n");
	
	lua_results<5> ret1 = call_luaFunction<lua_results<5>>("test4",L);
	printf("{");
	for(i=0;i<ret1._rets.size();++i)
	{
		printf("%d,",(int)any_cast<int64_t>(ret1._rets[i]));
	}
	printf("}\n");	
	
	luaObject ac = call_luaFunction<luaObject>("test3",L);
	ac.CallMemberFunction<void>("show");
	printf("balance:%d\n",ac.GetMemberValue<int>("balance"));
	ac.SetMemberValue<int>("balance",1000);
	printf("balance:%d\n",ac.GetMemberValue<int>("balance"));
	
	getchar();
	return 0;
}