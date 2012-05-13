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
#ifndef _OBJPUSH_H
#define _OBJPUSH_H
//调用lua函数时参数压栈的抽象
template<typename T>
class objPush
{
public:
	objPush(lua_State *L,const T arg)
	{
		doPush(L,arg,Int2Type<pointerTraits<T>::isPointer>(),Int2Type<IndexOf<internalType,pointerTraits<T>::PointeeType>::value == -1>());
	}
private:
	 //处理指向对象的指针
	 void doPush(lua_State *L,T arg,Int2Type<true>,Int2Type<true>)
	 {
		if(!arg)
			lua_pushnil(L);
		else
		{
			if(!luaRegisterClass<pointerTraits<T>::PointeeType>::isRegister())
				lua_pushlightuserdata(L,(void*)arg);
			else
				objUserData<pointerTraits<T>::PointeeType>::NewObj(L,arg);
		}
	}

	//处理指向内部类型的指针
	void doPush(lua_State *L,T arg,Int2Type<true>,Int2Type<false>)
	{
		if(!arg)
			lua_pushnil(L);
		{
			typedef LOKI_TYPELIST_2(char,unsigned char) CharType;
			//指向内部类型的指中有可能是char *
			doPushUserData(L,arg,Int2Type<IndexOf<CharType,pointerTraits<T>::PointeeType>::value != -1>());
		}
	}

	//处理指向内部类型
	void doPush(lua_State *L,T arg,Int2Type<false>,Int2Type<false>)
	{
		lua_pushnumber(L,(lua_Number)arg);
	}
	
	//用于处理指向char *的指针
	void doPushUserData(lua_State *L,T arg, Int2Type<true>)
	{
		lua_pushstring(L,arg);
	}

	//处理其它指向内部类型的指针
	void doPushUserData(lua_State *L,T arg, Int2Type<false>)
	{
		lua_pushlightuserdata(L,(void*)arg);
	}

};

//对std::string的特化
template<>
class objPush<std::string>
{
public:
	objPush(lua_State *L,const std::string &arg)
	{
		lua_pushstring(L,arg.c_str());
	}
};

//针对luaObject的特化
template<>
class objPush<luaObject>
{
public:
	objPush(lua_State *L,const luaObject arg)
	{
		lua_rawgeti(L,LUA_REGISTRYINDEX,arg.getIndex());
	}
};


//针对luatable的特化
template<>
class objPush<luatable>
{
public:
	objPush(lua_State *L,const luatable &arg)
	{
		lua_newtable(L);
		for(int i = 0; i < (int)arg.size(); ++i)
		{
			//lua_pushnumber(L,i+1);
			
			if(arg[0].empty())
			{
				lua_pushnil(L);
			}
			else
			{	

				short t_type = arg[i].GetType();
				if(t_type == -1)
				{
					if(arg[i].IsPointerType())
					{
						//是否注册的用户类型
						if(arg[i].luaRegisterClassName == "")
							objPush<void*> obj(L,any_cast<void*>(arg[i]));
						else
						{
							NewObj(L,any_cast<void*>(arg[i]),arg[i].luaRegisterClassName.c_str());
						}
					}
					else
					{
						//出错
					}
				}
				else if(t_type >=0 && t_type <= 9)
				{
					switch(t_type)
					{
					case 0:
					case 1:
						lua_pushnumber(L,any_cast<unsigned char>(arg[i]));
						break;
					case 2:
					case 3:
						lua_pushnumber(L,any_cast<unsigned short>(arg[i]));
						break;
					case 4:
					case 5:
						lua_pushnumber(L,any_cast<unsigned int>(arg[i]));
						break;
					case 6:
					case 7:
						lua_pushnumber(L,any_cast<unsigned long>(arg[i]));
						break;
					case 8:
						lua_pushnumber(L,any_cast<float>(arg[i]));
						break;
					case 9:
						lua_pushnumber(L,any_cast<double>(arg[i]));
						break;
					};
				}
				else if(t_type == 10)
				{
					lua_pushstring(L,any_cast<std::string>(arg[i]).c_str());
				}
				else if(t_type == 11)
				{
					objPush<luaObject> obj(L,any_cast<luaObject>(arg[i]));
				}
				else if(t_type == 12)
				{
					objPush<luatable> obj(L,any_cast<luatable>(arg[i]));
				}
			}

			lua_rawseti(L,-2,i+1);
		}
	}
};


#endif