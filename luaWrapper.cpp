#include "luaWrapper.h"
#include <stdlib.h>
//std::string luaInvokeName;

bool luaWrapper::load(const char *filename)
{
    if (luaL_dofile(lState, filename)) {
        const char * error = lua_tostring(lState, -1);
		lua_pop(lState,1);
		printf("%s\n",error);
        return false;
    }
	return true;
}

void luaWrapper::regCFunction()
{
}

static void kennyluainit(lua_State *L)
{			
	luaL_newmetatable(L, "kenny.lualib");
	lua_pop(L,1);
}

bool luaWrapper::init()
{
	lState = luaL_newstate();
	if(!lState)
		return false;
	luaL_openlibs(lState);
	regCFunction();
	kennyluainit(lState);
	Integer64::Register2Lua(lState);
	return true;
}

extern "C"
{
	void *l_alloc (void *ud, void *ptr, size_t osize, size_t nsize)
	{
		(void)ud;
		(void)osize;
		if(!ptr && nsize==0)return NULL;
		if (nsize == 0)
		{
			free(ptr);
			return NULL;
		}else if(ptr ==0)
		{
			return malloc(nsize);
		}
		return realloc(ptr, nsize);
	}
};

int NewObj(lua_State *L,void *ptr,const char *classname) 
{
    size_t nbytes = sizeof(objUserData<void>);
    objUserData<void> *obj = (objUserData<void> *)lua_newuserdata(L, nbytes);
    obj->ptr = ptr;
	obj->m_flag = 0x1234AFEC;
    luaL_getmetatable(L, "kenny.lualib");
    lua_pushstring(L,classname);
    lua_gettable(L,-2);
    lua_setmetatable(L, -3);
      
    lua_pop(L,1);//pop mt kenny.lualib
    
    return 1;
}

int newI64(lua_State *L)
{
	Integer64 *tmp = (Integer64*)lua_touserdata(L,1);
	if(tmp)
		lua_pushlightuserdata(L,tmp);
	else
	{
		long initval = lua_tonumber(L,1);
		size_t nbytes = sizeof(Integer64);
		void *buf = lua_newuserdata(L, nbytes);
		new(buf)Integer64(initval);
	}
	Integer64::setmetatable(L);
	return 1;
}

template <typename T>
static T* _GetPointer(lua_State *L,int index)
{
	objUserData<T> *obj = objUserData<T>::checkobjuserdata(L,index);
	if(obj)
		return obj->ptr;
	else 
		return (T*)lua_touserdata(L,index);
	return NULL;
}





