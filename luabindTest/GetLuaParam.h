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
#ifndef _GETLUAPARAM_H
#define _GETLUAPARAM_H

//在c接口函数中获取lua传进来的参数
template<typename T>
inline T getLuaParam(lua_State *L,int index)
{
	return realGet<pointerTraits<T>::PointeeType>::Get(L,index,Int2Type<pointerTraits<T>::isPointer>());
}

//lua调用c接口函数时，c函数从lua栈中获取参数
template<>
inline long getLuaParam(lua_State *L,int index)
{
	return (long)lua_tonumber(L,index);
}

template<>
inline double getLuaParam(lua_State *L,int index)
{
	return lua_tonumber(L,index);
}

template<>
inline std::string getLuaParam(lua_State *L,int index)
{
	const char *str = lua_tostring(L,index);
	std::string ret(str);
	return ret;
}

template<>
inline char *getLuaParam(lua_State *L,int index)
{
	return const_cast<char*>(lua_tostring(L,index));
}

template<>
inline const char *getLuaParam(lua_State *L,int index)
{
	return lua_tostring(L,index);
}

template<>
inline const void *getLuaParam(lua_State *L,int index)
{
	const void *ret;
	if(!lua_isuserdata(L,index))
	{
		//lightuserdata
		ret = lua_touserdata(L,index);
	}
	else
	{
		//userdata
		ret = lua_touserdata(L,index);
		ret = ((objUserData<void>*)ret)->ptr;
	}
	return ret;
}

template<>
inline bool getLuaParam(lua_State *L,int index)
{
	return (bool)lua_toboolean(L,index);
}

template<>
inline luaObject getLuaParam(lua_State *L,int index)
{
	int r = luaL_ref(L,LUA_REGISTRYINDEX);
	luaObject obj(L,r);
	return obj;
}


template<>
inline luatable getLuaParam(lua_State *L,int index)
{
	luatable ret;
	int len = lua_objlen(L, index);
	for( int i = 1; i <= len; ++i)
	{
		lua_rawgeti(L,index,i);
		
		if(lua_isnil(L,-1))
		{
			ret.push_back(any());
		}
		else if(lua_isuserdata(L,-1))
		{
			ret.push_back(popvalue<const void*>(L));
		}
		else if(lua_isnumber(L,-1))
		{
			ret.push_back(popvalue<int>(L));
		}		
		else if(lua_isstring(L,-1))
		{
			ret.push_back(popvalue<std::string>(L));
		}
		else if(lua_isboolean(L,-1))
		{
			ret.push_back(popvalue<bool>(L));
		}
		else if(lua_istable(L,-1))
		{
			//获取元表，如果没有原表就是简单的table，否则是object
			if(0 == lua_getmetatable(L,-1))
			{
				ret.push_back(popvalue<luatable>(L));
			}
			else
			{
				lua_pop(L,1);
				ret.push_back(popvalue<luaObject>(L));
			}
			
		}
		else
		{
			throw std::string("lua函数提供了不支持的参数类型");
		}
	}

	//lua_pop(L,1);
	return ret;
}

#endif