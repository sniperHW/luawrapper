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
	
	Integer64::Register2Lua(L);

	//测试注册任意C++函数
	registerFun1(L,"show",showmsg);
	
	//测试向lua注册C++类
	RegisterClass<testb>(L,"testb");
	registerField<testb,int,&testb::valb>("valb");
	registerMemberFunction0("func",&testb::function);

	RegisterClass<testc>(L,"testc");
	DefParent<testb,testc>();
	registerMemberFunction0("funcc",&testc::functionc);

	testc tc;
	tc.valb = 100;
	testb tb;
	tb.valb = 1000;
	call_luaFunction2<void>("test1",L,&tb,&tc);
	
/*
	printf("after call valb = %d\n",tb.valb);

	
	//测试调用返回table的lua函数
	luatable ret = call_luaFunction0<luatable>("test2",L);
	for(int i = 0; i < ret.size();++i)
		printf("%d",any_cast<int>(ret[i]));
	printf("\n");

	//测试返回lua对象
	luaObject account = call_luaFunction0<luaObject>("test3",L);
	account.CallMemberFunction0<void>("show");
	printf("%s\n",account.GetMemberValue<std::string>("name").c_str());
	account.SetMemberValue<std::string>("name","sniperhuangwei");
	//将改变后的对象返回到lua中
    call_luaFunction1<void>("test4",L,account);
*/
	getchar();


	return 0;
}

/*
template<class T> void* GetFunAddr( T fun ) 
{
	return *(void**)&fun; 
}
template<class T> T     GetFunAddr( void* pFun ) 
{
	return *(T*)&pFun; 
}

class CBase
{
protected:
	int m_nA;

public:
	CBase() { this->m_nA = 1; }
};

class CAA : public CBase
{
public:
	virtual void fun() 
	{
		this->m_nA = 100; 
	}
	int m_nA;
};

int main()
{
	CAA* pAA = new CAA();
	void* pAddr = GetFunAddr( &CAA::fun );
	typedef void(CAA::*FunType)();
	FunType fun2 = GetFunAddr<FunType>( pAddr );
	FunType fun = &CAA::fun;//= *(FunType*)&pAddr;//GetFunAddr<FunType>( pAddr );
	//(pAA->*(*(FunType*)&pAddr))();
	int s = sizeof(FunType);
	(pAA->*fun)();
	(pAA->*fun2)();
	return 0;
}
*/
