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
#ifndef _UTILITY
#define _UTILITY
#include "TypeList.h"
#include "Trait.h"

template<typename T>
struct GetReplaceType
{
	typedef T type;
	//typedef typename refTraits<T>::RefType type;
};

template<>
struct GetReplaceType<const char *>
{
	typedef std::string type;
};

template<>
struct GetReplaceType<char *>
{
	typedef std::string type;
};

template<typename T>
struct GetRawType
{
	typedef T type;
};

template<typename T>
struct GetRawType<T&>
{
	typedef T &type;
};

template<>
struct GetRawType<std::string>
{
	typedef const char * type;
};

template<typename T>
inline typename GetRawType<T>::type GetRawValue(T &in)
{
	return in;
}

template<>
inline GetRawType<std::string>::type GetRawValue(std::string &in)
{
	return in.c_str();
}

static inline const char *stack_value_tostr(lua_State *L,int index) {
    static char buff[1024];
    int type = lua_type(L,index);
    switch(type) {
        case LUA_TNIL:{
            snprintf(buff,1023,"nil");
        }break;
        case LUA_TUSERDATA:{
            snprintf(buff,1023,"userdata");
        }break;
        case LUA_TLIGHTUSERDATA:{
            snprintf(buff,1023,"lightuserdata");
        }break;
        case LUA_TNUMBER:{
            lua_Number v = lua_tonumber(L,index);
            if(v != (lua_Integer)v){
                snprintf(buff,1023,"%f",v);
            } else {
                snprintf(buff,1023,"%lld",lua_tointeger(L,index));
            }
        }break;
        case LUA_TBOOLEAN:{
            lua_Integer b = lua_tointeger(L,index);
            snprintf(buff,1023,"%s",b == 1 ? "true":"false");
        }break;
        case LUA_TTABLE:{
            snprintf(buff,1023,"table");
        }break;
        case LUA_TTHREAD:{
            snprintf(buff,1023,"thread");
        }break;
        case LUA_TFUNCTION:{
            snprintf(buff,1023,"function");
        }break;
        case LUA_TSTRING:{
            snprintf(buff,1023,"%s",lua_tostring(L,index));
        }
        break;
        default:{
            snprintf(buff,1023,"unknow");
        }
    }

    buff[1023] = 0;
    return buff;
}

static inline void show_stack(lua_State *L,const char *msg = NULL) {
    int top = lua_gettop(L);
    if(msg) {
        printf("top:%d,%s\n",top,msg);
    }else {
        printf("top:%d\n",top);        
    }
    int i;
    for(i = top;i > 0; --i) {
        if(i == top) {
            printf("(stack index:%d) %s <- top\n",i,stack_value_tostr(L,i));
        } else {
            printf("(stack index:%d) %s\n",i,stack_value_tostr(L,i));           
        }
    }
}

#endif
