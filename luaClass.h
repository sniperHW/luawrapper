#ifndef _LUACLASS_H
#define _LUACLASS_H
//#include "luaWrapper.h"
#include "luacommon.h"
#include "luaObject.h"
#include "any.h"
#include <string.h>

template<typename T>
void push_obj(lua_State *L,const T obj);

//取出栈顶的值，通知将其出栈
template<typename T>
T popvalue(lua_State *L);
class Integer64
{
public:
	Integer64(int64_t val):m_val(val)
	{
		m_flag = 0XFEDC1234;
	}

	int64_t GetValue() const
	{
		return m_val;
	}

	int GetFlag() const{return m_flag;}
	static void Register2Lua(lua_State *L);
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
		i64self->m_val =  i64self->m_val OP i64other->m_val;\
	}else\
	{\
		i64self->m_val = i64self->m_val OP i64other->m_val;\
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

	static int i64Destroy(lua_State *L)
	{
		Integer64 *i64self  = (Integer64 *)lua_touserdata(L,1);
		luaL_argcheck(L, i64self  != NULL, 1, "userdata expected");	
		printf("i64Destroy\n");
		return 0;
	}

private:
	int64_t m_val;
	int     m_flag;
};

static void pushI64(lua_State *L,int64_t value)
{
	size_t nbytes = sizeof(Integer64);
	void *buf = lua_newuserdata(L, nbytes);
	new(buf)Integer64(value);
	Integer64::setmetatable(L);
}

template <typename T>
struct memberfield
{
    memberfield():gmv(NULL),smv(NULL),mc(NULL),mbfunc(0){}

    template<typename PARENT>
    memberfield(const memberfield<PARENT> &p):gmv((GMV)p.gmv),smv((SMV)p.smv),mc((MC)p.mc),
		mbfunc(p.mbfunc),field(p.field)
    {
    }

    typedef void (*GMV)(T *,lua_State*,void *(T::*));//获得成员变量值
    typedef void (*SMV)(T *,lua_State*,void*,void *(T::*));//设置成员变量值
    typedef int (*MC)(lua_State*);//调用成员函数
    GMV gmv;
    SMV smv;
    MC  mc;
    void (T::*mbfunc)(void);
	void *(T::*field);
};

template<typename T>
class objUserData;

class array_holder
{
public:
	template<typename T>
	array_holder(T _ptr):ptr(_ptr)
	{
		type_index = IndexOf<internalType,typename pointerTraits<T>::PointeeType>::value;
	}
	void *ptr;
	int type_index;
};

///////////////////////////////////////

template<typename T,typename type>
static void GetPropertyData(T *self,lua_State *L,Int2Type<true>,void*(T::*field))
{
	push_obj<type*>(L,&(self->*field));
}

template<typename T,typename type>
static void GetPropertyData(T *self,lua_State *L,Int2Type<false>,void*(T::*field))
{
	push_obj<type>(L,(type)(self->*field));
}

//设置数值型成员变量值
template<typename T,typename type>
static void SetPropertyData(T *self,lua_State *L,void *buf,void*(T::*field))
{
	double *tmp = (double*)buf;
	*(type*)(&(self->*field)) = (type)*tmp;
	push_obj<type>(L,(type)(self->*field));
}

//设置指针型成员变量的值
template<typename T,typename type>
static void SetPropertyPtr(T *self,lua_State *L,void *buf,void*(T::*field))
{
	*(type*)(&(self->*field)) = (*(type*)buf);
	push_obj<type>(L,(type)(self->*field));
}

//获取成员变量的值
template<typename T,typename type>
static void GetProperty(T *self,lua_State *L,void*(T::*field) )
{   
	GetPropertyData<T,type>(self,L,Int2Type<!pointerTraits<type>::isPointer && IndexOf<internalType,typename pointerTraits<type>::PointeeType>::value == -1>(),field);
}

template<typename T,typename type>
static void SetProperty(memberfield<T> &mf,Int2Type<false>)
{
	mf.smv = SetPropertyData<T,type>;
}

template<typename T,typename type>
static void SetProperty(memberfield<T> &mf,Int2Type<true>)
{
	mf.smv = SetPropertyPtr<T,type>;
}
////////////////////////////////////
//用于向lua注册一个类
template<typename T>
class luaClassWrapper
{
    friend class objUserData<T>;
    
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
       typename std::map<std::string,memberfield<T> >::iterator it = luaClassWrapper<T>::fields.find(std::string(name));
       if(it != luaClassWrapper<T>::fields.end())
       {
            if(isuserdata)
                it->second.smv(self->ptr,L,&valu,it->second.field);
            else
                it->second.smv(self->ptr,L,&valn,it->second.field);
       }
       return 0;

    }

    static int Index(lua_State *L)
    {
        objUserData<T> *obj = checkobjuserdata(L,1);
        const char *name = luaL_checkstring(L, 2);
        typename std::map<std::string,memberfield<T> >::iterator it = luaClassWrapper<T>::fields.find(std::string(name));
        if(it != luaClassWrapper<T>::fields.end())
        {
            if(it->second.mbfunc)
            {
                lua_pushlightuserdata(L,&it->second.mbfunc);
                lua_pushcclosure(L,it->second.mc,1);
            }
            else
                it->second.gmv(obj->ptr,L,it->second.field);
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
	int m_flag;

};

typedef int (*lua_fun)(lua_State*);

#ifndef _GETFUNC
#define _GETFUNC\
        __func func;\
        void *t = lua_touserdata(L,lua_upvalueindex(1));\
        memcpy(&func,t,sizeof(func));
#endif

template<typename RUNC_TYPE>
class memberfunbinder{};

//用于注册成员函数
template<typename Ret,typename Cla>
class memberfunbinder<Ret(Cla::*)()>
{
public:
	typedef Cla CLASS_TYPE;
    static int lua_cfunction(lua_State *L)
    {    
        objUserData<Cla> *obj = objUserData<Cla>::checkobjuserdata(L,1);
        Cla *cla = obj->ptr;
        return doCall<Ret>(L,cla,Int2Type<isVoid<Ret>::is_Void>());
    }

private:

    typedef Ret(Cla::*__func)();
	

    
    template<typename Result> 
    static int doCall(lua_State *L,Cla *cla,Int2Type<false>)
    {
        _GETFUNC;
        Result ret = (cla->*func)();
        push_obj<Result>(L,ret);
        return 1;
    }

     template<typename Result>
    static int doCall(lua_State *L,Cla *cla,Int2Type<true>)
    {
        _GETFUNC;
        (cla->*func)();
        return 0;
    }
    
};

template<typename Ret,typename Arg1,typename Cla>
class memberfunbinder<Ret(Cla::*)(Arg1)>
{
public:
	typedef Cla CLASS_TYPE;
    static int lua_cfunction(lua_State *L)
    {
        objUserData<Cla> *obj = objUserData<Cla>::checkobjuserdata(L,1);
        Cla *cla = obj->ptr;
        typename GetReplaceType<Arg1>::type tmp_arg1 = popvalue<typename GetReplaceType<Arg1>::type>(L);
        Arg1 arg1 = GetRawValue(tmp_arg1);
        return doCall<Ret>(L,cla,arg1,Int2Type<isVoid<Ret>::is_Void>());
    }

private:

    typedef Ret(Cla::*__func)(Arg1);

    template<typename Result> 
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Int2Type<false>)
    {
        _GETFUNC;
        Result ret = (cla->*func)(arg1);
        push_obj<Result>(L,ret);
        return 1;
    }

    template<typename Result> 
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Int2Type<true>)
    {
        _GETFUNC;
        (cla->*func)(arg1);
        return 0;
    }

};

template<typename Ret,typename Arg1,typename Arg2,typename Cla>
class memberfunbinder<Ret(Cla::*)(Arg1,Arg2)>
{
public:
	typedef Cla CLASS_TYPE;
    static int lua_cfunction(lua_State *L)
    {
        objUserData<Cla> *obj = objUserData<Cla>::checkobjuserdata(L,1);
        Cla *cla = obj->ptr;
        typename GetReplaceType<Arg2>::type tmp_arg2 = popvalue<typename GetReplaceType<Arg2>::type>(L);
        typename GetReplaceType<Arg1>::type tmp_arg1 = popvalue<typename GetReplaceType<Arg1>::type>(L);
        Arg1 arg1 = GetRawValue(tmp_arg1);
        Arg2 arg2 = GetRawValue(tmp_arg2);
        return doCall<Ret>(L,cla,arg1,arg2,Int2Type<isVoid<Ret>::is_Void>());
    }

private:

    typedef Ret(Cla::*__func)(Arg1,Arg2);

    template<typename Result> 
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2,Int2Type<false>)
    {
        _GETFUNC;
        Result ret = (cla->*func)(arg1,arg2);
        push_obj<Result>(L,ret);
        return 1;
    }

    template<typename Result> 
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2,Int2Type<true>)
    {
        _GETFUNC;
        (cla->*func)(arg1,arg2);
        return 0;
    }
};

template<typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Cla>
class memberfunbinder<Ret(Cla::*)(Arg1,Arg2,Arg3)>
{
public:
	typedef Cla CLASS_TYPE;
    static int lua_cfunction(lua_State *L)
    {
        objUserData<Cla> *obj = objUserData<Cla>::checkobjuserdata(L,1);
        Cla *cla = obj->ptr;
        typename GetReplaceType<Arg3>::type tmp_arg3 = popvalue<typename GetReplaceType<Arg3>::type>(L);
        typename GetReplaceType<Arg2>::type tmp_arg2 = popvalue<typename GetReplaceType<Arg2>::type>(L);
        typename GetReplaceType<Arg1>::type tmp_arg1 = popvalue<typename GetReplaceType<Arg1>::type>(L);
        Arg1 arg1 = GetRawValue(tmp_arg1);
        Arg2 arg2 = GetRawValue(tmp_arg2);
        Arg3 arg3 = GetRawValue(tmp_arg3);
        return doCall<Ret>(L,cla,arg1,arg2,arg3,Int2Type<isVoid<Ret>::is_Void>());
    }

private:

    typedef Ret(Cla::*__func)(Arg1,Arg2,Arg3);

    template<typename Result> 
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2,Arg3 arg3,Int2Type<false>)
    {
        _GETFUNC;
        Result ret = (cla->*func)(arg1,arg2,arg3);
        push_obj<Result>(L,ret);
        return 1;
    }

    template<typename Result> 
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2,Arg3 arg3,Int2Type<true>)
    {
        _GETFUNC;
        (cla->*func)(arg1,arg2,arg3);
        return 0;
    }
};

template<typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename Cla>
class memberfunbinder<Ret(Cla::*)(Arg1,Arg2,Arg3,Arg4)>
{
public:

    static int lua_cfunction(lua_State *L)
    {
        objUserData<Cla> *obj = objUserData<Cla>::checkobjuserdata(L,1);
        Cla *cla = obj->ptr;
        typename GetReplaceType<Arg4>::type tmp_arg4 = popvalue<typename GetReplaceType<Arg4>::type>(L);
        typename GetReplaceType<Arg3>::type tmp_arg3 = popvalue<typename GetReplaceType<Arg3>::type>(L);
        typename GetReplaceType<Arg2>::type tmp_arg2 = popvalue<typename GetReplaceType<Arg2>::type>(L);
        typename GetReplaceType<Arg1>::type tmp_arg1 = popvalue<typename GetReplaceType<Arg1>::type>(L);
        Arg1 arg1 = GetRawValue(tmp_arg1);
        Arg2 arg2 = GetRawValue(tmp_arg2);
        Arg3 arg3 = GetRawValue(tmp_arg3);
        Arg4 arg4 = GetRawValue(tmp_arg4);
        return doCall<Ret>(L,cla,arg1,arg2,arg3,arg4,Int2Type<isVoid<Ret>::is_Void>());
    }

private:

    typedef Ret(Cla::*__func)(Arg1,Arg2,Arg3,Arg4);

    template<typename Result> 
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4,Int2Type<false>)
    {
        _GETFUNC;
        Result ret = (cla->*func)(arg1,arg2,arg3,arg4);
        push_obj<Result>(L,ret);
        return 1;
    }

	template<typename Result>
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4,Int2Type<true>)
    {
        _GETFUNC;
        (cla->*func)(arg1,arg2,arg3,arg4);
        return 0;
    }
};

template<typename Parent,typename T>
void DefParent(Int2Type<false>)
{
     std::map<std::string,memberfield<Parent> > &parent_fields =  luaClassWrapper<Parent>::GetAllFields();
     std::map<std::string,memberfield<T> > &filds = luaClassWrapper<T>::GetAllFields();
     typename std::map<std::string,memberfield<Parent> >::iterator it = parent_fields.begin();
     typename std::map<std::string,memberfield<Parent> >::iterator end = parent_fields.end();
     for( ; it != end; ++it)
     {
         filds.insert(std::make_pair(it->first,it->second));
     }
}

template<typename Parent,typename T>
void DefParent(Int2Type<true>){}

//注册一个类,最多支持继承自4个基类
template<typename T,typename Parent1 = void,typename Parent2 = void,typename Parent3 = void,typename Parent4 = void>
void register_class(lua_State *L,const char *name)
{
    luaRegisterClass<T>::SetClassName(name);
    luaClassWrapper<T>::luaopen_objlib(L);
    luaRegisterClass<T>::SetRegister();
	DefParent<Parent1,T>(Int2Type<isVoid<Parent1>::is_Void>());
	DefParent<Parent2,T>(Int2Type<isVoid<Parent2>::is_Void>());
	DefParent<Parent3,T>(Int2Type<isVoid<Parent3>::is_Void>());
	DefParent<Parent4,T>(Int2Type<isVoid<Parent4>::is_Void>());
}

//注册类的成员变量
template<typename T,typename property_type>
void register_property(const char *name,property_type (T::*field))
{            
    memberfield<T> mf;
    mf.gmv = GetProperty<T,property_type>;
	mf.field = (void*(T::*))field;
    SetProperty<T,property_type>(mf,Int2Type<pointerTraits<property_type>::isPointer>());
    luaClassWrapper<T>::InsertFields(name,mf);
}

//注册类成员函数的接口
template<typename FUNTOR>
void register_member_function(const char *fun_name,FUNTOR _func)//Ret(T::*_func)())
{
    memberfield<typename memberfunbinder<FUNTOR>::CLASS_TYPE> mf;
    memcpy(&mf.mbfunc,&_func,sizeof(_func));
    lua_fun fun = memberfunbinder<FUNTOR>::lua_cfunction;	
    mf.mc = fun;
    luaClassWrapper<typename memberfunbinder<FUNTOR>::CLASS_TYPE>::InsertFields(fun_name,mf);
}

#endif