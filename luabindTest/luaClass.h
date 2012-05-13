#ifndef _LUACLASS_H
#define _LUACLASS_H
#include "luaWrapper.h"
#include "luaObject.h"
#include "any.h"

//extern std::string luaInvokeName;

static void kennyluainit(lua_State *L,luaWrapper *lw)
{			
	luaL_newmetatable(L, "kenny.lualib");
	lua_pop(L,1);
}

extern int newI64(lua_State *L);
static const struct luaL_Reg i64Lib[] = {
	{"new",newI64},
	{NULL,NULL},
};

class Integer64
{
public:
	Integer64(__int64 val):m_val(val){}

	static void Register2Lua(lua_State *L)
	{
		luaL_getmetatable(L, "kenny.lualib");
		lua_pushstring(L,"int64");
		lua_newtable(L);

		
		lua_pushstring(L, "__add");
		lua_pushcfunction(L, i64Add);
		lua_rawset(L, -3);

		lua_pushstring(L, "__sub");
		lua_pushcfunction(L, i64Sub);
		lua_rawset(L, -3);

		lua_pushstring(L, "__div");
		lua_pushcfunction(L, i64Div);
		lua_rawset(L, -3); 

		lua_pushstring(L, "__mul");
		lua_pushcfunction(L, i64Mul);
		lua_rawset(L, -3); 

		lua_pushstring(L, "__mod");
		lua_pushcfunction(L, i64Mod);
		lua_rawset(L, -3);

		lua_pushstring(L, "__eq");
		lua_pushcfunction(L, i64Eq);
		lua_rawset(L, -3);

		lua_pushstring(L, "__lt");
		lua_pushcfunction(L, i64Lt);
		lua_rawset(L, -3);

		lua_pushstring(L, "__le");
		lua_pushcfunction(L, i64Le);
		lua_rawset(L, -3);

		lua_pushstring(L, "__tostring");
		lua_pushcfunction(L, i64toString);
		lua_rawset(L, -3);
		
		lua_rawset(L,1);
		lua_pop(L,1);
		luaL_register(L,"i64",i64Lib);
		lua_pop(L,1);
		
	}
	

	static void setmetatable(lua_State *L)
	{
		luaL_getmetatable(L, "kenny.lualib");
		lua_pushstring(L,"int64");
		lua_gettable(L,-2);
		lua_setmetatable(L, -3);
		lua_pop(L,1);
	}

#ifndef I64_RELA
#define I64_RELA(OP)\
	Integer64 *i64self  = (Integer64 *)lua_touserdata(L,1);\
	Integer64 *i64other = (Integer64 *)lua_touserdata(L,2);\
	lua_pushboolean(L,i64self->m_val OP i64other->m_val);\
	return 1;
#endif

	static int i64Le(lua_State *L)
	{
		I64_RELA(<=);
	}

	static int i64Lt(lua_State *L)
	{
		I64_RELA(<);
	}

	static int i64Eq(lua_State *L)
	{
		I64_RELA(==);
	}

#ifndef I64_MATH
#define I64_MATH(OP)\
	Integer64 *i64self  = (Integer64 *)lua_touserdata(L,1);\
	Integer64 *i64other = (Integer64 *)lua_touserdata(L,2);\
	Integer64 tmp(0);\
	if(!i64other)\
	{\
		tmp.m_val = lua_tonumber(L,2);\
		i64other = &tmp;\
	}\
	if(!i64self)\
	{\
		long num = lua_tonumber(L,1);\
		size_t nbytes = sizeof(Integer64);\
		i64self = (Integer64*)lua_newuserdata(L, nbytes);\
		new(i64self)Integer64(num);\
		i64self->m_val OP= i64other->m_val;\
	}else\
	{\
		i64self->m_val OP= i64other->m_val;\
		lua_pushlightuserdata(L,i64self);\
	}\
	setmetatable(L);\
	return 1;
#endif

	static int i64Mod(lua_State *L)
	{
		I64_MATH(%);
	}

	static int i64Div(lua_State *L)
	{
		I64_MATH(/);
	}

	static int i64Mul(lua_State *L)
	{
		I64_MATH(*);
	}

	static int i64Add(lua_State *L)
	{
		I64_MATH(+);
	}

	static int i64Sub(lua_State *L)
	{
		I64_MATH(-);
	}

	static int i64toString(lua_State *L)
	{
		Integer64 *i64self  = (Integer64 *)lua_touserdata(L,1);
		luaL_argcheck(L, i64self  != NULL, 1, "userdata expected");	
		char temp[64];
		sprintf(temp, "%I64d", i64self->m_val);
		lua_pushstring(L, temp);
		return 1;
	}
private:
	__int64 m_val;
};



static int newI64(lua_State *L)
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

struct dummy
{
};

struct membfun : virtual public dummy
{
    virtual int dummyFunc(){}
    typedef void (membfun::*_func)(void);
};

template <typename T>
struct memberfield
{
    memberfield():gmv(NULL),smv(NULL),mc(NULL),mbfunc(0){}

    template<typename PARENT>
    memberfield(const memberfield<PARENT> &p):gmv((GMV)p.gmv),smv((SMV)p.smv),mc((MC)p.mc),mbfunc(p.mbfunc)
    {
    }

    typedef void (*GMV)(T *,lua_State*);//获得成员变量值
    typedef void (*SMV)(T *,lua_State*,void*);//设置成员变量值
    typedef int (*MC)(lua_State*);//调用成员函数
    GMV gmv;
    SMV smv;
    MC  mc;
    membfun::_func mbfunc;
};

template<typename T>
class objUserData;

template<typename T>
void RegisterClass(lua_State *L,const char *name);

template<typename T,typename type,type (T::*field)>
void registerField(const char *name);



//用于向lua注册一个类
template<typename T>
class luaClassWrapper
{
    friend objUserData<T>;
    
    public:

        static int luaopen_objlib(lua_State *L) {
            
            luaL_getmetatable(L, "kenny.lualib");
            
            lua_pushstring(L,luaRegisterClass<T>::GetClassName());

            luaL_newmetatable(L, luaRegisterClass<T>::GetClassName());
            lua_pushstring(L, "__index");
            lua_pushcfunction(L, &objUserData<T>::Index);
            lua_settable(L, -3);  

            lua_pushstring(L, "__call");
            lua_pushcfunction(L, &objUserData<T>::Call);
            lua_settable(L, -3);
            
            lua_pushstring(L, "__newindex");
            lua_pushcfunction(L, &objUserData<T>::NewIndex);  
            lua_rawset(L, -3);

            lua_rawset(L,1);
            lua_pop(L,1);
            return 1;
        }

        static std::map<std::string,memberfield<T> > &GetAllFields()
        {
            return fields;
        }

        static void InsertFields(const char *name,memberfield<T> &mf)
        {
            fields.insert(std::make_pair(std::string(name),mf));
        }

        //获取成员变量的值
        template<typename type,type (T::*field)>
        static void GetMemberValue(T *self,lua_State *L)
        {   
            GetMemberData<type,field>(self,L,Int2Type<!pointerTraits<type>::isPointer && IndexOf<internalType,pointerTraits<type>::PointeeType>::value == -1>());
        }

        template<typename type,type (T::*field)>
        static void SetMemberValue(memberfield<T> &mf,Int2Type<false>)
        {
            mf.smv = SetMemberData<type,field>;
        }

        template<typename type,type (T::*field)>
        static void SetMemberValue(memberfield<T> &mf,Int2Type<true>)
        {
         mf.smv = SetMemberPtr<type,field>;
        }
    private:
        
        template<typename type,type (T::*field)>
        static void GetMemberData(T *self,lua_State *L,Int2Type<true>)
        {
            objPush<type*>(L,&(self->*field));
        }

        template<typename type,type (T::*field)>
        static void GetMemberData(T *self,lua_State *L,Int2Type<false>)
        {
            objPush<type>(L,(self->*field));
        }
        
        //设置数值型成员变量值
        template<typename type,type (T::*field)>
        static void SetMemberData(T *self,lua_State *L,void *buf)
        {
            //从lua中传进来的数值都是double型的
            double *tmp = (double*)buf;
            (self->*field) = (type)(*tmp);
            objPush<type>(L,(self->*field));
        }

        //设置指针型成员变量的值
        template<typename type,type (T::*field)>
        static void SetMemberPtr(T *self,lua_State *L,void *buf)
        {
            (self->*field) = (*(type*)buf);
            objPush<type>(L,(self->*field));
        }
        
    private:
        static std::map<std::string,memberfield<T> > fields;
};


template<typename T>
std::map<std::string,memberfield<T> > luaClassWrapper<T>::fields;

extern int NewObj(lua_State *L,void *ptr,const char *classname);

template<typename T>
class objUserData
{
public:


    static objUserData<T> *checkobjuserdata (lua_State *L,int index) {
      /*const char *name = luaRegisterClass<T>::GetClassName();
      if(strcmp(name,"") == 0)
            return NULL;
      
      void *ud = luaL_checkudata(L, index,name);
      */
      void *ud = lua_touserdata(L,index);
      luaL_argcheck(L, ud != NULL, 1, "userdata expected");
      return (objUserData<T> *)ud;
    }

    static int NewObj(lua_State *L,T *ptr) 
    {
        return ::NewObj(L,ptr,luaRegisterClass<T>::GetClassName());
    }

    static int NewIndex(lua_State *L)
    {
       double valn;
       void *valu = NULL;
       objUserData<T> *self = checkobjuserdata(L,1);
       const char *name = luaL_checkstring(L, 2);
       bool isuserdata = false;
       if(lua_isnumber(L,3))
            valn = lua_tonumber(L,3);
       else
       {
            valu = lua_touserdata(L,3);
            isuserdata = true;
       }
       std::map<std::string,memberfield<T> >::iterator it = luaClassWrapper<T>::fields.find(std::string(name));
       if(it != luaClassWrapper<T>::fields.end())
       {
            if(isuserdata)
                it->second.smv(self->ptr,L,&valu);
            else
                it->second.smv(self->ptr,L,&valn);
       }
       return 0;

    }

    static int Index(lua_State *L)
    {
        objUserData<T> *obj = checkobjuserdata(L,1);
        const char *name = luaL_checkstring(L, 2);
        std::map<std::string,memberfield<T> >::iterator it = luaClassWrapper<T>::fields.find(std::string(name));
        if(it != luaClassWrapper<T>::fields.end())
        {
            if(it->second.mbfunc)
            {
                lua_pushlightuserdata(L,&it->second.mbfunc);
                lua_pushcclosure(L,it->second.mc,1);
            }
            else
                it->second.gmv(obj->ptr,L);
        }
        else
        {
            lua_pushnil(L);
        }
        return 1;

    }

    
    static int Call(lua_State *L)
    {
        return 0;
    }
    
public:
    T *ptr;

};

static int NewObj(lua_State *L,void *ptr,const char *classname) 
{
    size_t nbytes = sizeof(objUserData<void>);
    objUserData<void> *obj = (objUserData<void> *)lua_newuserdata(L, nbytes);
    obj->ptr = ptr;
    luaL_getmetatable(L, "kenny.lualib");
    lua_pushstring(L,classname);
    lua_gettable(L,-2);
    lua_setmetatable(L, -3);
      
    lua_pop(L,1);//pop mt kenny.lualib
    
    return 1;
}


typedef int (*lua_fun)(lua_State*);

#ifndef _GETFUNC
#define _GETFUNC\
        __func func;\
        void *t = lua_touserdata(L,lua_upvalueindex(1));\
        memcpy(&func,t,sizeof(func));
#endif


//用于注册成员函数
template<typename Ret,typename Cla>
class memberfunbinder0
{
public:

    static int lua_cfunction(lua_State *L)
    {    
        objUserData<Cla> *obj = objUserData<Cla>::checkobjuserdata(L,1);
        Cla *cla = obj->ptr;
        return doCall<Ret>(L,cla);
    }

private:

    typedef typename Ret(Cla::*__func)();

    
    template<typename Result> 
    static int doCall(lua_State *L,Cla *cla)
    {
        _GETFUNC;
        Result ret = (cla->*func)();
        objPush<Result>(L,ret);
        return 1;
    }

    template<> 
    static int doCall<void>(lua_State *L,Cla *cla)
    {
        _GETFUNC;
        (cla->*func)();
        return 0;
    }
    
};

template<typename Ret,typename Arg1,typename Cla>
class memberfunbinder1
{
public:

    static int lua_cfunction(lua_State *L)
    {
        objUserData<Cla> *obj = objUserData<Cla>::checkobjuserdata(L,1);
        Cla *cla = obj->ptr;
        GetReplaceType<Arg1>::type tmp_arg1 = popvalue<GetReplaceType<Arg1>::type>(L);
        Arg1 arg1 = GetRawValue(tmp_arg1);
        return doCall<Ret>(L,cla,arg1);
    }

private:

    typedef typename Ret(Cla::*__func)(Arg1);

    template<typename Result> 
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1)
    {
        _GETFUNC;
        Result ret = (cla->*func)(arg1);
        objPush<Result>(L,ret);
        return 1;
    }

    template<> 
    static int doCall<void>(lua_State *L,Cla *cla,Arg1 arg1)
    {
        _GETFUNC;
        (cla->*func)(arg1);
        return 0;
    }

};

template<typename Ret,typename Arg1,typename Arg2,typename Cla>
class memberfunbinder2
{
public:

    static int lua_cfunction(lua_State *L)
    {
        objUserData<Cla> *obj = objUserData<Cla>::checkobjuserdata(L,1);
        Cla *cla = obj->ptr;
        GetReplaceType<Arg2>::type tmp_arg2 = popvalue<GetReplaceType<Arg2>::type>(L);
        GetReplaceType<Arg1>::type tmp_arg1 = popvalue<GetReplaceType<Arg1>::type>(L);
        Arg1 arg1 = GetRawValue(tmp_arg1);
        Arg2 arg2 = GetRawValue(tmp_arg2);
        return doCall<Ret>(L,cla,arg1,arg2);
    }

private:

    typedef typename Ret(Cla::*__func)(Arg1,Arg2);

    template<typename Result> 
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2)
    {
        _GETFUNC;
        Result ret = (cla->*func)(arg1,arg2);
        objPush<Result>(L,ret);
        return 1;
    }

    template<> 
    static int doCall<void>(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2)
    {
        _GETFUNC;
        (cla->*func)(arg1,arg2);
        return 0;
    }
};

template<typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Cla>
class memberfunbinder3
{
public:

    static int lua_cfunction(lua_State *L)
    {
        objUserData<Cla> *obj = objUserData<Cla>::checkobjuserdata(L,1);
        Cla *cla = obj->ptr;
        GetReplaceType<Arg3>::type tmp_arg3 = popvalue<GetReplaceType<Arg3>::type>(L);
        GetReplaceType<Arg2>::type tmp_arg2 = popvalue<GetReplaceType<Arg2>::type>(L);
        GetReplaceType<Arg1>::type tmp_arg1 = popvalue<GetReplaceType<Arg1>::type>(L);
        Arg1 arg1 = GetRawValue(tmp_arg1);
        Arg2 arg2 = GetRawValue(tmp_arg2);
        Arg3 arg3 = GetRawValue(tmp_arg3);
        return doCall<Ret>(L,cla,arg1,arg2,arg3);
    }

private:

    typedef typename Ret(Cla::*__func)(Arg1,Arg2,Arg3);

    template<typename Result> 
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2,Arg3 arg3)
    {
        _GETFUNC;
        Result ret = (cla->*func)(arg1,arg2,arg3);
        objPush<Result>(L,ret);
        return 1;
    }

    template<> 
    static int doCall<void>(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2,Arg3 arg3)
    {
        _GETFUNC;
        (cla->*func)(arg1,arg2,arg3);
        return 0;
    }
};

template<typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename Cla>
class memberfunbinder4
{
public:

    static int lua_cfunction(lua_State *L)
    {
        objUserData<Cla> *obj = objUserData<Cla>::checkobjuserdata(L,1);
        Cla *cla = obj->ptr;
        GetReplaceType<Arg4>::type tmp_arg4 = popvalue<GetReplaceType<Arg4>::type>(L);
        GetReplaceType<Arg3>::type tmp_arg3 = popvalue<GetReplaceType<Arg3>::type>(L);
        GetReplaceType<Arg2>::type tmp_arg2 = popvalue<GetReplaceType<Arg2>::type>(L);
        GetReplaceType<Arg1>::type tmp_arg1 = popvalue<GetReplaceType<Arg1>::type>(L);
        Arg1 arg1 = GetRawValue(tmp_arg1);
        Arg2 arg2 = GetRawValue(tmp_arg2);
        Arg3 arg3 = GetRawValue(tmp_arg3);
        Arg4 arg4 = GetRawValue(tmp_arg4);
        return doCall<Ret>(L,cla,arg1,arg2,arg3,arg4);
    }

private:

    typedef typename Ret(Cla::*__func)(Arg1,Arg2,Arg3,Arg4);

    template<typename Result> 
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4)
    {
        _GETFUNC;
        Result ret = (cla->*func)(arg1,arg2,arg3,arg4);
        objPush<Result>(L,ret);
        return 1;
    }

    template<> 
    static int doCall<void>(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4)
    {
        _GETFUNC;
        (cla->*func)(arg1,arg2,arg3,arg4);
        return 0;
    }
};


//注册一个类
template<typename T>
void RegisterClass(lua_State *L,const char *name)
{
    luaRegisterClass<T>::SetClassName(name);
    luaClassWrapper<T>::luaopen_objlib(L);
    luaRegisterClass<T>::SetRegister();
}

template<typename Parent,typename T>
void DefParent()
{
     std::map<std::string,memberfield<Parent>>&parent_fields =  luaClassWrapper<Parent>::GetAllFields();
     std::map<std::string,memberfield<T>>&filds = luaClassWrapper<T>::GetAllFields();
     std::map<std::string,memberfield<Parent>>::iterator it = parent_fields.begin();
     std::map<std::string,memberfield<Parent>>::iterator end = parent_fields.end();
     for( ; it != end; ++it)
     {
         filds.insert(std::make_pair(it->first,it->second));
     }
}

//注册类的成员变量
template<typename T,typename type,type (T::*field)>
void registerField(const char *name)
{            
    memberfield<T> mf;
    mf.gmv = luaClassWrapper<T>::GetMemberValue<type,field>;
    luaClassWrapper<T>::SetMemberValue<type,field>(mf,Int2Type<pointerTraits<type>::isPointer>());
    luaClassWrapper<T>::InsertFields(name,mf);
}

//注册类成员函数的接口
template<typename Ret,typename T>
void registerMemberFunction0(const char *fun_name,Ret(T::*_func)())
{

    memberfield<T> mf;
    memcpy(&mf.mbfunc,&_func,sizeof(_func));
    lua_fun fun = memberfunbinder0<Ret,T>::lua_cfunction;
    mf.mc = fun;
    luaClassWrapper<T>::InsertFields(fun_name,mf);
}

template<typename Ret,typename Arg1,typename T>
void registerMemberFunction1(const char *fun_name,Ret(T::*_func)(Arg1))
{
    memberfield<T> mf;
    memcpy(&mf.mbfunc,&_func,sizeof(_func));
    lua_fun fun = memberfunbinder1<Ret,Arg1,T>::GetFunction(fun_name,_func);
    mf.mc = fun;
    luaClassWrapper<T>::InsertFields(fun_name,mf); 
}

template<typename Ret,typename Arg1,typename Arg2,typename T>
void registerMemberFunction2(const char *fun_name,Ret(T::*_func)(Arg1,Arg2))
{
    memberfield<T> mf;
    memcpy(&mf.mbfunc,&_func,sizeof(_func));
    lua_fun fun = memberfunbinder2<Ret,Arg1,Arg2,T>::GetFunction(fun_name,_func);
    mf.mc = fun;
    luaClassWrapper<T>::InsertFields(fun_name,mf); 
}

template<typename Ret,typename Arg1,typename Arg2,typename Arg3,typename T>
void registerMemberFunction3(const char *fun_name,Ret(T::*_func)(Arg1,Arg2,Arg3))
{
    memberfield<T> mf;
    memcpy(&mf.mbfunc,&_func,sizeof(_func));
    lua_fun fun = memberfunbinder3<Ret,Arg1,Arg2,Arg3,T>::GetFunction(fun_name,_func);
    mf.mc = fun;
    luaClassWrapper<T>::InsertFields(fun_name,mf); 
}

template<typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename T>
void registerMemberFunction4(const char *fun_name,Ret(T::*_func)(Arg1,Arg2,Arg3,Arg4))
{
    memberfield<T> mf;
    memcpy(&mf.mbfunc,&_func,sizeof(_func));
    lua_fun fun = memberfunbinder4<Ret,Arg1,Arg2,Arg3,Arg4,T>::GetFunction(fun_name,_func);
    mf.mc = fun;
    luaClassWrapper<T>::InsertFields(fun_name,mf); 
}

#include "ObjPush.h"

#endif