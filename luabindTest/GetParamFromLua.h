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
#ifndef _GETPARAMFROMLUA_H
#define _GETPARAMFROMLUA_H
#include "any.h"
#include "luaFunction.h"
//c<->lua接口函数中获取lua传进来的参数

template <typename Param1>
bool GetParamFromLua1(lua_State *L,Param1 &param1)
{
	if(lua_gettop(L) < 1)
		return false;
	param1 = getLuaParam<Param1>(L,1);
	return true;
}

template <typename Param1,typename Param2>
bool GetParamFromLua2(lua_State *L,Param1 &param1,Param2 &param2)
{
	if(lua_gettop(L) < 2)
		return false;
	param1 = getLuaParam<Param1>(L,1);
	param2 = getLuaParam<Param2>(L,2);
	return true;
}

template <typename Param1,typename Param2,typename Param3>
bool GetParamFromLua3(lua_State *L,Param1 &param1,Param2 &param2,Param3 &param3)
{
	if(lua_gettop(L) < 3)
		return false;
	param1 = getLuaParam<Param1>(L,1);
	param2 = getLuaParam<Param2>(L,2);
	param3 = getLuaParam<Param3>(L,3);
	return true;
}

template <typename Param1,typename Param2,typename Param3,typename Param4>
bool GetParamFromLua4(lua_State *L,Param1 &param1,Param2 &param2,Param3 &param3,Param4 &param4)
{
	if(lua_gettop(L) < 4)
		return false;
	param1 = getLuaParam<Param1>(L,1);
	param2 = getLuaParam<Param2>(L,2);
	param3 = getLuaParam<Param3>(L,3);
	param4 = getLuaParam<Param4>(L,4);
	return true;
}

template <typename Param1,typename Param2,typename Param3,typename Param4,typename Param5>
bool GetParamFromLua5(lua_State *L,Param1 &param1,Param2 &param2,Param3 &param3,Param4 &param4,Param5 &param5)
{
	if(lua_gettop(L) < 5)
		return false;
	param1 = getLuaParam<Param1>(L,1);
	param2 = getLuaParam<Param2>(L,2);
	param3 = getLuaParam<Param3>(L,3);
	param4 = getLuaParam<Param4>(L,4);
	param5 = getLuaParam<Param5>(L,5);
	return true;
}

template <typename Param1,typename Param2,typename Param3,typename Param4,typename Param5,typename Param6>
bool GetParamFromLua6(lua_State *L,Param1 &param1,Param2 &param2,Param3 &param3,Param4 &param4,Param5 &param5,Param6 &param6)
{
	if(lua_gettop(L) < 6)
		return false;
	param1 = getLuaParam<Param1>(L,1);
	param2 = getLuaParam<Param2>(L,2);
	param3 = getLuaParam<Param3>(L,3);
	param4 = getLuaParam<Param4>(L,4);
	param5 = getLuaParam<Param5>(L,5);
	param6 = getLuaParam<Param6>(L,6);
	return true;
}

template <typename Param1,typename Param2,typename Param3,typename Param4,typename Param5,typename Param6,typename Param7>
bool GetParamFromLua7(lua_State *L,Param1 &param1,Param2 &param2,Param3 &param3,Param4 &param4,Param5 &param5,Param6 &param6,Param7 &param7)
{
	if(lua_gettop(L) < 7)
		return false;
	param1 = getLuaParam<Param1>(L,1);
	param2 = getLuaParam<Param2>(L,2);
	param3 = getLuaParam<Param3>(L,3);
	param4 = getLuaParam<Param4>(L,4);
	param5 = getLuaParam<Param5>(L,5);
	param6 = getLuaParam<Param6>(L,6);
	param7 = getLuaParam<Param7>(L,7);
	return true;
}

template <typename Param1,typename Param2,typename Param3,typename Param4,typename Param5,typename Param6,typename Param7,typename Param8>
bool GetParamFromLua8(lua_State *L,Param1 &param1,Param2 &param2,Param3 &param3,Param4 &param4,Param5 &param5,Param6 &param6,Param7 &param7,Param8 &param8)
{
	if(lua_gettop(L) < 8)
		return false;
	param1 = getLuaParam<Param1>(L,1);
	param2 = getLuaParam<Param2>(L,2);
	param3 = getLuaParam<Param3>(L,3);
	param4 = getLuaParam<Param4>(L,4);
	param5 = getLuaParam<Param5>(L,5);
	param6 = getLuaParam<Param6>(L,6);
	param7 = getLuaParam<Param7>(L,7);
	param8 = getLuaParam<Param8>(L,8);
	return true;
}

#endif