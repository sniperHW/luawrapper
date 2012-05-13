#include "stdafx.h"
#include "luaWrapper.h"

//std::string luaInvokeName;

bool luaWrapper::load()
{
    if (luaL_dofile(lState, "start.lua")) {
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

bool luaWrapper::init()
{
	lState = luaL_newstate();
	if(!lState)
		return false;
	luaL_openlibs(lState);
	if(!load())
		return false;
	regCFunction();
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




