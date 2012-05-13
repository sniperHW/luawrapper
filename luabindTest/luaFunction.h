/*	
    Copyright (C) <2012>  <huangweilook@21cn.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/	
/*
* brief : 注册全局函数
* Author : huangwei
*/
#ifndef _LUAFUNCTION_H
#define _LUAFUNCTION_H
#include "luaWrapper.h"
#include "luaClass.h"
#include "utility.h"

template<typename T>
class objPush;


//取出栈顶的值，通知将其出栈
template<typename T>
T popvalue(lua_State *L);

//最大支持从lua中返回10个值
typedef LOKI_TYPELIST_9(lua_results<2>,lua_results<3>,lua_results<4>,lua_results<5>,lua_results<6>,lua_results<7>,lua_results<8>,lua_results<9>,lua_results<10>) retarrayType;


template<typename Ret>
class doLuaCall
{
public:
	static Ret doCall(lua_State *L,int nArgs,int errFunc,bool callByObject = false)
	{
		
		typedef LOKI_TYPELIST_1(void) voidType;
		return call(L,nArgs,errFunc,Int2Type<IndexOf<voidType,Ret>::value == 0>(),callByObject);
		
	}
private:
	//返回void
	static Ret call(lua_State *L,int nArgs,int errFunc,Int2Type<true>,bool callByObject)
	{
		if(lua_pcall(L, nArgs, 0, errFunc ) != 0)
		{
			const char *error = lua_tostring(L,-1);
			std::string err(error);
			lua_pop(L,1);
			throw err;
		}

		if(callByObject)
		{
			//将object从栈中弹出
			lua_pop(L,1);
		}

	}
	
	//有返回值
	static Ret call(lua_State *L,int nArgs,int errFunc,Int2Type<false>,bool callByObject)
	{

		Ret ret = _call(L,nArgs,IndexOf<retarrayType,Ret>::value+2,errFunc,Int2Type<IndexOf<retarrayType,Ret>::value == -1>(),callByObject);
		if(callByObject)
		{
			//将object从栈中弹出
			lua_pop(L,1);
		}

		return ret;
	}

	//只有一个返回值
	static Ret _call(lua_State *L,int nArgs,int nRets,int errFunc,Int2Type<true>,bool callByObject)
	{
		if(lua_pcall(L, nArgs, nRets, errFunc ) != 0)
		{
			const char *error = lua_tostring(L,-1);
			std::string err(error);
			lua_pop(L,1);
			throw err;
			return Ret();
		}
		return popvalue<Ret>(L);
	}


	static void GetRetFromStack(std::list<any> &out,lua_State *L)
	{
		if(lua_isnil(L,-1))
		{
			out.push_front(any());
			lua_pop(L,-1);
		}
		else if(lua_isuserdata(L,-1))
		{
			out.push_front(popvalue<const void*>(L));
		}		
		else if(lua_isnumber(L,-1))
		{
			out.push_front(popvalue<__int64>(L));
		}		
		else if(lua_isstring(L,-1))
		{
			out.push_front(popvalue<std::string>(L));
		}
		else if(lua_isboolean(L,-1))
		{
			out.push_front(popvalue<bool>(L));
		}
		else if(lua_istable(L,-1))//返回一个表，需要做特殊处理
		{
			out.push_front(popvalue<luatable>(L));
		}

		else
		{
			throw std::string("lua函数返回了不支持的类型");
		}
	}

	//多个返回值
	static Ret _call(lua_State *L,int nArgs,int nRets,int errFunc,Int2Type<false>,bool callByObject)
	{
		if(lua_pcall(L, nArgs, nRets, errFunc ) != 0)
		{
			const char *error = lua_tostring(L,-1);
			std::string err(error);
			lua_pop(L,1);
			throw err;
		}

		if(callByObject)
		{
			if (lua_gettop(L) - 1 != nRets)
			{
				throw std::string("返回值数量不正确");
			}
		}
		else
		{
			if (lua_gettop(L) != nRets)
			{
				throw std::string("返回值数量不正确");
			}
		}

		Ret rets;
		std::list<any> tmp;
		
		int c = nRets;
		while(c-- > 0)
			GetRetFromStack(tmp,L);

		std::list<any>::iterator it = tmp.begin();
		std::list<any>::iterator end = tmp.end();

		for( ; it != end; ++it)
		{
			rets._rets.push_back(*it);
		}

		return rets;
		
	}

};

#ifndef PUSH_FUNCTOR
#define PUSH_FUNCTOR\
	lua_pushstring(L,name);\
	lua_pushlightuserdata(L, _func);\
	lua_pushcclosure(L,lua_cfunction,1);\
	lua_settable(L, LUA_GLOBALSINDEX);
#endif


typedef int (*lua_fun)(lua_State *);
//用于注册全局函数
//无参
template<typename Ret>
class funbinder0
{
public:

	static void pushfuctor(lua_State *L,const char *name,Ret(*_func)())
	{
		PUSH_FUNCTOR;
	}

	static int lua_cfunction(lua_State *L)
	{
		return doCall<Ret>(L,(__func)lua_touserdata(L,lua_upvalueindex(1)));
	}

private:

	typedef typename Ret(*__func)();
	
	template<typename Result> 
	static int doCall(lua_State *L,__func func)
	{
		Result ret = func();
		objPush<Result>(L,ret);
		return 1;
	}

	template<> 
	static int doCall<void>(lua_State *L,__func func)
	{
		func();
		return 0;
	}

};

template<typename Ret>
void registerFun0(lua_State *L,const char *name,Ret(*_func)())
{
	funbinder0<Ret>::pushfuctor(L,name,_func);
}

//单参
template<typename Ret,typename Arg1>
class funbinder1
{
public:

	static void pushfuctor(lua_State *L,const char *name,Ret(*_func)(Arg1))
	{
		PUSH_FUNCTOR;
	}


	static int lua_cfunction(lua_State *L)
	{
		GetReplaceType<Arg1>::type tmp_arg1 = popvalue<GetReplaceType<Arg1>::type>(L);
		Arg1 arg1 = GetRawValue(tmp_arg1);
		return doCall<Ret>(L,arg1,(__func)lua_touserdata(L,lua_upvalueindex(1)));
	}

private:
	typedef typename Ret(*__func)(typename Arg1);

	template<typename Result> 
	static int doCall(lua_State *L,Arg1 arg1,__func func)
	{
		Result ret = func(arg1);
		objPush<Result>(L,ret);
		return 1;
	}

	template<> 
	static int doCall<void>(lua_State *L,Arg1 arg1,__func func)
	{
		func(arg1);
		return 0;
	}

};

template<typename Ret,typename Arg1>
void registerFun1(lua_State *L,const char *name,Ret(*_func)(Arg1))
{
	funbinder1<Ret,Arg1>::pushfuctor(L,name,_func);
}

//2参
template<typename Ret,typename Arg1,typename Arg2>
class funbinder2
{
public:

	static void pushfuctor(lua_State *L,const char *name,Ret(*_func)(Arg1,Arg2))
	{
		PUSH_FUNCTOR;
	}

	static int lua_cfunction(lua_State *L)
	{
		GetReplaceType<Arg2>::type tmp_arg2 = popvalue<GetReplaceType<Arg2>::type>(L);
		GetReplaceType<Arg1>::type tmp_arg1 = popvalue<GetReplaceType<Arg1>::type>(L);
		Arg1 arg1 = GetRawValue(tmp_arg1);
		Arg2 arg2 = GetRawValue(tmp_arg2);
		return doCall<Ret>(L,arg1,arg2,(__func)lua_touserdata(L,lua_upvalueindex(1)));
	}

private:
	typedef typename Ret(*__func)(typename Arg1,typename Arg2);

	template<typename Result> 
	static int doCall(lua_State *L,Arg1 arg1,Arg2 arg2,__func func)
	{
		Result ret = func(arg1,arg2);
		objPush<Result>(L,ret);
		return 1;
	}

	template<> 
	static int doCall<void>(lua_State *L,Arg1 arg1,Arg2 arg2,__func func)
	{
		func(arg1,arg2);
		return 0;
	}
};

template<typename Ret,typename Arg1,typename Arg2>
void registerFun2(lua_State *L,const char *name,Ret(*_func)(Arg1,Arg2))
{
	funbinder2<Ret,Arg1,Arg2>::pushfuctor(L,name,_func);
}

//3参
template<typename Ret,typename Arg1,typename Arg2,typename Arg3>
class funbinder3
{
public:

	static void pushfuctor(lua_State *L,const char *name,Ret(*_func)(Arg1,Arg2,Arg3))
	{
		PUSH_FUNCTOR;
	}

	static int lua_cfunction(lua_State *L)
	{
		GetReplaceType<Arg3>::type tmp_arg3 = popvalue<GetReplaceType<Arg3>::type>(L);
		GetReplaceType<Arg2>::type tmp_arg2 = popvalue<GetReplaceType<Arg2>::type>(L);
		GetReplaceType<Arg1>::type tmp_arg1 = popvalue<GetReplaceType<Arg1>::type>(L);
		Arg1 arg1 = GetRawValue(tmp_arg1);
		Arg2 arg2 = GetRawValue(tmp_arg2);
		Arg3 arg3 = GetRawValue(tmp_arg3);
		return doCall<Ret>(L,arg1,arg2,arg3,(__func)lua_touserdata(L,lua_upvalueindex(1)));
	}

private:
	typedef typename Ret(*__func)(typename Arg1,typename Arg2,typename Arg3);

	template<typename Result> 
	static int doCall(lua_State *L,Arg1 arg1,Arg2 arg2,Arg3 arg3,__func func)
	{
		Result ret = func(arg1,arg2,arg3);
		objPush<Result>(L,ret);
		return 1;
	}

	template<> 
	static int doCall<void>(lua_State *L,Arg1 arg1,Arg2 arg2,Arg3 arg3,__func func)
	{
		func(arg1,arg2,arg3);
		return 0;
	}

};


template<typename Ret,typename Arg1,typename Arg2,typename Arg3>
void registerFun3(lua_State *L,const char *name,Ret(*_func)(Arg1,Arg2,Arg3))
{
	funbinder3<Ret,Arg1,Arg2,Arg3>::pushfuctor(L,name,_func);
}

//4参
template<typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4>
class funbinder4
{
public:

	static void pushfuctor(lua_State *L,const char *name,Ret(*_func)(Arg1,Arg2,Arg3,Arg4))
	{
		PUSH_FUNCTOR;
	}

	static int lua_cfunction(lua_State *L)
	{
		GetReplaceType<Arg4>::type tmp_arg4 = popvalue<GetReplaceType<Arg4>::type>(L);
		GetReplaceType<Arg3>::type tmp_arg3 = popvalue<GetReplaceType<Arg3>::type>(L);
		GetReplaceType<Arg2>::type tmp_arg2 = popvalue<GetReplaceType<Arg2>::type>(L);
		GetReplaceType<Arg1>::type tmp_arg1 = popvalue<GetReplaceType<Arg1>::type>(L);
		Arg1 arg1 = GetRawValue(tmp_arg1);
		Arg2 arg2 = GetRawValue(tmp_arg2);
		Arg3 arg3 = GetRawValue(tmp_arg3);
		Arg4 arg4 = GetRawValue(tmp_arg4);
		return doCall<Ret>(L,arg1,arg2,arg3,arg4,(__func)lua_touserdata(L,lua_upvalueindex(1)));
	}

private:
	typedef typename Ret(*__func)(typename Arg1,typename Arg2,typename Arg3,typename Arg4);
	template<typename Result> 
	static int doCall(lua_State *L,Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4,__func func)
	{
		Result ret = func(arg1,arg2,arg3,arg4);
		objPush<Result>(L,ret);
		return 1;
	}

	template<> 
	static int doCall<void>(lua_State *L,Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4,__func func)
	{
		func(arg1,arg2,arg3,arg4);
		return 0;
	}
};

template<typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4>
void registerFun4(lua_State *L,const char *name,Ret(*_func)(Arg1,Arg2,Arg3,Arg4))
{
	funbinder4<Ret,Arg1,Arg2,Arg3,Arg4>::pushfuctor(L,name,_func);
}


template<typename Ret>
Ret call_luaFunction0(const char *funname,lua_State *L)
{
	lua_getglobal(L,funname);
	return doLuaCall<Ret>::doCall(L,0,0);
}

template<typename Ret,typename Arg1>
Ret call_luaFunction1(const char *funname,lua_State *L,Arg1 arg1)
{
	lua_getglobal(L,funname);
	objPush<Arg1>(L,arg1);
	return doLuaCall<Ret>::doCall(L,1,0);
}

template<typename Ret,typename Arg1,typename Arg2>
Ret call_luaFunction2(const char *funname,lua_State *L,Arg1 arg1,Arg2 arg2)
{
	lua_getglobal(L,funname);
	objPush<Arg1>(L,arg1);
	objPush<Arg2>(L,arg2);
	return doLuaCall<Ret>::doCall(L,2,0);
}

template<typename Ret,typename Arg1,typename Arg2,typename Arg3>
Ret call_luaFunction3(const char *funname,lua_State *L,Arg1 arg1,Arg2 arg2,Arg3 arg3)
{
	lua_getglobal(L,funname);
	objPush<Arg1>(L,arg1);
	objPush<Arg2>(L,arg2);
	objPush<Arg3>(L,arg3);
	return doLuaCall<Ret>::doCall(L,3,0);
}

template<typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4>
Ret call_luaFunction4(const char *funname,lua_State *L,Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4)
{
	lua_getglobal(L,funname);
	objPush<Arg1>(L,arg1);
	objPush<Arg2>(L,arg2);
	objPush<Arg3>(L,arg3);
	objPush<Arg4>(L,arg4);
	return doLuaCall<Ret>::doCall(L,4,0);
}

#include "PopValue.h"
#include "GetLuaParam.h"

#endif 