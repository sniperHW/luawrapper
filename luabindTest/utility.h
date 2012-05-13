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
template<typename T>
struct GetReplaceType
{
	typedef T type;
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

template<>
struct GetRawType<std::string>
{
	typedef const char * type;
};

//template<typename T>
//inline typename GetRawType<T>::type GetRawValue(T &in);

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


template<typename T>
class realGet
{
public:

	//ispointer
	static T* Get(lua_State *L,int index,Int2Type<true>)
	{
		return GetPointer(L,index);
	}
	
	static T Get(lua_State *L,int index,Int2Type<false>)
	{
		return T();
	}

private:

	static T* GetPointer(lua_State *L,int index)
	{
		objUserData<T> *obj = objUserData<T>::checkobjuserdata(L,index);
		if(obj)
			return obj->ptr;
		else 
			return (T*)lua_touserdata(L,index);
	}
};


#endif