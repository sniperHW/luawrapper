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
#ifndef _LUAOBJECT_H
#define _LUAOBJECT_H
#include "luaWrapper.h"


#define PREPARE(L,index){\
		lua_rawgeti(L,LUA_REGISTRYINDEX,index);\
		lua_pushstring(L,name);\
		lua_gettable(L,-2);\
		lua_rawgeti(L,LUA_REGISTRYINDEX,index);\
	}

//代表lua中的一个对象(其实就是一个表)
class luaObject
{
public:
	luaObject(lua_State *lState,int index)
		:m_iKeyIndex(index),m_plState(lState)
		{
			counter = new int(1);
		}

	luaObject()
	:m_iKeyIndex(0),m_plState(0),counter(NULL)
	{
	}

	luaObject &operator = (const luaObject & other)
	{
		if(this != &other)
		{
			if(!counter)
			{
				counter = other.counter;
				++(*counter);
				this->m_iKeyIndex = other.m_iKeyIndex;
				this->m_plState = other.m_plState;
			}
			else
			{
				if(--(*counter) <= 0)
				{
					//原来指向的对象现在已经没有其它对象在引用了,可以从lua中释放掉了
					luaL_unref(this->m_plState,LUA_REGISTRYINDEX,this->m_iKeyIndex);
					delete counter;
				}

				counter = other.counter;
				++(*counter);
				this->m_iKeyIndex = other.m_iKeyIndex;
				this->m_plState = other.m_plState;
			}
		}
		return *this;
	}

	luaObject(const luaObject &other)
		:m_iKeyIndex(other.m_iKeyIndex),m_plState(other.m_plState),counter(other.counter)
	{
		++(*counter);
	}

	~luaObject()
	{
		if(counter)
		{
			if(!(--(*counter)))
			{
				luaL_unref(m_plState,LUA_REGISTRYINDEX,m_iKeyIndex);
				delete counter;
			}
		}
	}

	//获得类的成员变量
	template<typename T>
	T GetMemberValue(const char *name)
	{
		lua_rawgeti(m_plState,LUA_REGISTRYINDEX,m_iKeyIndex);
		lua_pushstring(m_plState,name);
		lua_gettable(m_plState,-2);
		T ret = popvalue<T>(m_plState);
		lua_pop(m_plState,1);//pop the table
		return ret;
	}

	//设置成员变量 
	template<typename T>
	void SetMemberValue(const char *name,T value)
	{
		lua_rawgeti(m_plState,LUA_REGISTRYINDEX,m_iKeyIndex);
		lua_pushstring(m_plState,name);
		objPush<T>(m_plState,value);
		lua_settable(m_plState,-3);
	}

	template<typename Ret>
	Ret CallMemberFunction0(const char *name)
	{
		PREPARE(m_plState,m_iKeyIndex);
		return doLuaCall<Ret>::doCall(m_plState,1,0,true);
	}

	template<typename Ret,typename Arg1>
	Ret CallMemberFunction1(const char *name,Arg1 arg1)
	{
		PREPARE(m_plState,m_iKeyIndex);
		objPush<Arg1>(m_plState,arg1);
		return doLuaCall<Ret>::doCall(m_plState,2,0,true);
	}

	template<typename Ret,typename Arg1,typename Arg2>
	Ret CallMemberFunction2(const char *name,Arg1 arg1,Arg2 arg2)
	{
		PREPARE(m_plState,m_iKeyIndex);
		objPush<Arg1>(m_plState,arg1);
		objPush<Arg2>(m_plState,arg2);
		return doLuaCall<Ret>::doCall(m_plState,3,0,true);
	}

	template<typename Ret,typename Arg1,typename Arg2,typename Arg3>
	Ret CallMemberFunction3(const char *name,Arg1 arg1,Arg2 arg2,Arg3 arg3)
	{
		PREPARE(m_plState,m_iKeyIndex);
		objPush<Arg1>(m_plState,arg1);
		objPush<Arg2>(m_plState,arg2);
		objPush<Arg3>(m_plState,arg3);
		return doLuaCall<Ret>::doCall(m_plState,4,0,true);
	}

	template<typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4>
	Ret CallMemberFunction3(const char *name,Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4)
	{
		PREPARE(m_plState,m_iKeyIndex);
		objPush<Arg1>(m_plState,arg1);
		objPush<Arg2>(m_plState,arg2);
		objPush<Arg3>(m_plState,arg3);
		objPush<Arg4>(m_plState,arg4);
		return doLuaCall<Ret>::doCall(m_plState,5,0,true);
	}

	lua_State *getState() const 
	{
		return m_plState;
	}

	int getIndex() const 
	{
		return m_iKeyIndex;
	}

private:
	lua_State *m_plState;
	int m_iKeyIndex;
	int *counter;
};
#endif