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
#ifndef _ANY_H
#define _ANY_H
#include <vector>
#include <iostream>
#include <stdint.h>
#include <map>
#include <stdlib.h>
#include "TypeList.h"
#include "Trait.h"
#include "luacommon.h"

typedef LOKI_TYPELIST_12(char,unsigned char,short,unsigned short,int,unsigned int,long,unsigned long,float,double,void,int64_t) internalType;

class luaObject;
class any;


typedef std::vector<any> luatable;

typedef LOKI_TYPELIST_14(char,unsigned char,short,unsigned short,int,unsigned int,long,unsigned long,float,double,std::string,luaObject,luatable,int64_t) SupportType;

//注册到lua中的用户数据类型信息
template<typename T>
class luaRegisterClass
{
public:
	static bool isRegister()
	{
		return m_isRegister;
	}

	static void SetRegister()
	{
		m_isRegister = true;
	}

	static void ClearRegister()
	{	
		m_isRegister = false;
	}

	static void SetClassName(const char *name)
	{
		classname = name;
	}

	static const char *GetClassName()
	{
		return classname.c_str();
	}

private:
	static bool m_isRegister;
	static std::string classname;
};

template<typename T>
bool luaRegisterClass<T>::m_isRegister = false;

template<typename T>
std::string luaRegisterClass<T>::classname;

class any;
class any_pusher
{
public:
	virtual void push(lua_State *L,any*) = 0; 
};
template <typename T>
any_pusher *create_any_pusher();

//一个精简版的boost::any
class any
{
public: // structors

   any(): content(0),counter(0){}

   template<typename ValueType>
   any(const ValueType & value)
      : content(new holder<ValueType>(value)),counter(new int(1)),luaRegisterClassName("")
   {
	   luaRegisterClassName = 	luaRegisterClass<typename pointerTraits<ValueType>::PointeeType>::GetClassName();
	   _any_pusher = create_any_pusher<ValueType>();
   }

   any(const any & other)
   {
	   if(other.counter && other.content)
	   {
		   content = other.content;
		   counter = other.counter;
		   ++(*counter);
		   luaRegisterClassName = other.luaRegisterClassName;
		   _any_pusher = other._any_pusher;
	   }
	   else
	   {
		   content = NULL;
		   counter = NULL;
		   _any_pusher = NULL;
	   }
   }

   	any & operator=(const any & rhs)
	{
		if(&rhs == this)
			return *this;
		else
		{      
			_destroy();
			if(rhs.counter && rhs.content)
			{
				content = rhs.content;
				counter = rhs.counter;
				_any_pusher = rhs._any_pusher;
				++(*counter);
			}
			else
			{
				content = 0;
				counter = 0;
			}
			return *this;
		}
	}
	
   ~any()
   {
		_destroy();
   }

private:
	void _destroy()
	{
		if(counter && content && --*counter <= 0)
		{
			delete counter;
			delete content;
			delete _any_pusher;
			//printf("destroy\n");
		}
	}

public: // queries
    bool empty() const
    {
       return !content;
    }
public:

    class placeholder
    {
      public: // structors
		virtual ~placeholder(){}
     };

     template<typename ValueType>
     class holder : public placeholder
     {
       public: // structors

         holder(const ValueType & value)
           : held(value){}
       public: // representation
			 ValueType held;

      };
   placeholder * content;
   int         * counter;
public: // representation (public so any_cast can be non-friend)
   any_pusher     * _any_pusher;
   std::string luaRegisterClassName;//保存的类型在lua中注册的名字

};

template<typename ValueType>
inline ValueType any_cast(const any & operand)
{
	return static_cast<any::holder<ValueType> *>(operand.content)->held;
}

template<>
inline std::string any_cast(const any & operand)
{
	any::holder<std::string> *tmp = static_cast<any::holder<std::string> *>(operand.content);
	return tmp->held;
}

//存放从lua中返回的一组值,volume表示返回值的数量
template<int volume>
class lua_results
{
public:

	lua_results():counter(new int(1)),rets(new std::vector<any>),_rets(*rets){}

	~lua_results()
	{
		if(counter)
		{
			if(--*counter <= 0)
			{
				delete counter;
				delete rets;
				//printf("retarray destroy\n");
			}
		}
	}

	lua_results(const lua_results & other):counter(other.counter),rets(other.rets),_rets(*rets)
	{
		if(counter)
			++(*counter);
	}
private:
	int * counter;
	std::vector<any> *rets;
public:
	std::vector<any> &_rets;
private:
	lua_results<volume> & operator=(const lua_results<volume> & rhs);
};

#endif
