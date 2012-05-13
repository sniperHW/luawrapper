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
#ifndef _POPVALUE_H
#define _POPVALUE_H

//从lua栈中弹出栈顶元素
template<typename T>
inline T popvalue(lua_State *L)
{
	return realGet<pointerTraits<T>::PointeeType>::Get(L,-1,Int2Type<pointerTraits<T>::isPointer>());
}


template<>
inline luaObject popvalue(lua_State *L)
{
	int r = luaL_ref(L,LUA_REGISTRYINDEX);
	luaObject obj(L,r);
	return obj;
}

template<>
inline double popvalue(lua_State *L)
{
	double ret = lua_tonumber(L,-1);
	lua_pop(L,1);
	return ret;
}

template<>
inline int popvalue(lua_State *L)
{
	int ret = (int)lua_tonumber(L,-1);
	lua_pop(L,1);
	return ret;
}

template<>
inline std::string popvalue(lua_State *L)
{
	const char *str = lua_tostring(L,-1);
	std::string ret(str);
	lua_pop(L,1);
	return ret;
}

template<>
inline const void *popvalue(lua_State *L)
{
	const void *ret;
	if(!lua_isuserdata(L,-1))
	{
		//lightuserdata
		ret = lua_touserdata(L,-1);
		lua_pop(L,1);
	}
	else
	{
		//userdata
		ret = lua_touserdata(L,-1);
		ret = ((objUserData<void>*)ret)->ptr;
		lua_pop(L,1);
	}
	return ret;
}

template<>
inline bool popvalue(lua_State *L)
{
	bool ret = (bool)lua_toboolean(L,-1);
	lua_pop(L,1);
	return ret;
}


template<>
inline luatable popvalue(lua_State *L)
{
	luatable ret;
	int len = lua_objlen(L, -1);
	for( int i = 1; i <= len; ++i)
	{
		lua_rawgeti(L,-1,i);
		
		if(lua_isnil(L,-1))
		{
			ret.push_back(any());
			lua_pop(L,1);
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
			throw std::string("lua函数返回了不支持的类型");
		}
	}

	lua_pop(L,1);
	return ret;
}


#endif