#include "luaWrapper.h"
#include <stdio.h>

typedef luacpp::luatable luatable;
typedef luacpp::luaObject luaObject;
#define any_cast luacpp::any_cast

class test_class_A
{
	public:
		test_class_A():memb_a(0)
		{
			printf("test_class_A void\n");
		}
		test_class_A(int v):memb_a(v)
		{
			printf("test_class_A int\n");
		}
		test_class_A(const test_class_A &other):memb_a(other.memb_a)
		{
			printf("test_class_A copy constructor %d \n",memb_a);
		}
		~test_class_A()
		{
			printf("~test_class_A\n");
		}
		int  memb_a;
		void show()
		{
			printf("test_class_A::show\n");
		}

		static void func_static()
		{
			printf("func_static\n");
		} 	
};


class base
{
	public:
		virtual void show()
		{
			printf("this base show\n");
		}

		void show2()
		{
			printf("this is not virtual function\n");
		}
};

class child:public base
{
public:
	void show()
	{
		printf("this child show\n");
	}

	void child_show()
	{
		printf("child_show\n");
	}
};

void test_call_virtual_function(lua_State *L)
{
	printf("\n----- lua call C++ virtual function -----\n");
	luacpp::reg_cclass<base>::_reg(L,"base")
		.method("show",&base::show)
		.method("show2",&base::show2);
	luacpp::reg_cclass<child,base>::_reg(L,"child")
		.method("child_show",&base::show2);
	child c;
	base *b = &c;
	try{
		luacpp::call<void>(L,"test7",b,&c);
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
}


void pass_by_value(test_class_A a)
{
	a.memb_a = 102;
}

test_class_A* pass_by_reference(test_class_A &a)
{
	a.memb_a = 102;
	return &a;
}

int main()
{
	luacpp::luaWrapper L;
	L.dofile("test/test_class/test.lua");

	luacpp::reg_cclass<test_class_A>::_reg(L,"test_class_A")
		.constructor<void>()//无参构造
		.constructor<const test_class_A&>()//一个参数构造
		//.constructor<int>()				   //一个参数构造
		.property("memb_a",&test_class_A::memb_a)
		.method("show",&test_class_A::show)
		.method("static_func",&test_class_A::func_static);


	test_class_A a;
	a.memb_a = 100;
	try{
		luacpp::call<void>(L,"test1",&a,&a);
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
	printf("after call test1,a.memb_a:%d\n",a.memb_a);


	luacpp::reg_cclass<base>::_reg(L,"base")
		.method("show",&base::show)
		.method("show2",&base::show2);
	luacpp::reg_cclass<child,base>::_reg(L,"child")
		.method("child_show",&child::child_show);
	child c;
	base *b = &c;
	try{
		luacpp::call<void>(L,"test2",b,&c);
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}

	luacpp::reg_cfun(L,"pass_by_value",pass_by_value);
	luacpp::reg_cfun(L,"pass_by_reference",pass_by_reference);
	try{
		luacpp::call<void>(L,"test3",&a);
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}


	try{
		luatable lt = luacpp::call<luatable>(L,"test4",&a);
		for(int i = 0; i < (int)lt.size();++i)
		{
			test_class_A *ptr_a = any_cast<test_class_A*>(lt[i]);
			printf("%p,%d\n",ptr_a,ptr_a->memb_a);
		}
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
	


	try{
		luacpp::call<void>(L,"test5");
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}


	return 0;
}