#include "luaWrapper.h"
#include <stdlib.h>
namespace luacpp{

std::map<void*,void*> ptrToUserData;
std::set<void*> userdataSet;;

bool luaWrapper::dofile(const char *filename)
{
    if (luaL_dofile(lState, filename)) {
        const char * error = lua_tostring(lState, -1);
		lua_pop(lState,1);
		printf("%s\n",error);
        return false;
    }
	return true;
}

static void kennyluainit(lua_State *L)
{			
	luaL_newmetatable(L, "kenny.lualib");
	lua_pop(L,1);
}

void luaWrapper::init()
{
	lState = luaL_newstate();
	luaL_openlibs(lState);
	kennyluainit(lState);
}

int pushObj(lua_State *L,const void *ptr,const char *classname)
{
	objUserData<void> *userdata = (objUserData<void>*)ptrToUserData[(void*)ptr];
	if(!userdata) {
	    size_t nbytes = sizeof(objUserData<void>);
	    userdata = (objUserData<void> *)lua_newuserdata(L, nbytes);
	    userdata->ptr = const_cast<void*>(ptr);	
	    userdata->construct_by_lua = false;
	    ptrToUserData[(void*)ptr] = userdata;
	    userdataSet.insert((void*)userdata);
	} else {
		lua_pushlightuserdata(L,userdata);
	} 

    luaL_getmetatable(L, "kenny.lualib");
    lua_pushstring(L,classname);
    lua_gettable(L,-2);
    lua_setmetatable(L, -3);
    lua_pop(L,1);//pop mt kenny.lualib
    return 1;

} 

void get_luatable(luatable &lt,lua_State *L)
{
	lt = popvalue<luatable>(L);
}
}