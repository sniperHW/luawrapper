C++对lua访问接口的封装,提供访问lua函数，向lua暴露普通C函数，向lua注册C++类型，对lua对象和表的封装等等.


---向lua注册函数
static int showmsg(const char *msg)
{
	printf("c-showmsg:%s\n",msg);
	return 0;
}
lWrapper::register_function(L,"cshow",showmsg);

---调用lua函数
lWrapper::call<void>("test1",L);

--注册c++类型
class test_class_A
{
	public:
		test_class_A():memb_a(0)
		{
			printf("test_class_A void\n");
		}
		test_class_A(const test_class_A &other):memb_a(other.memb_a)
		{
			printf("test_class_A con %d \n",memb_a);
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
};

lWrapper::register_class<test_class_A>(L,"test_class_A")
	.constructor<void>()//无参构造
	.constructor<const test_class_A&>()//一个参数构造
	.property("memb_a",&test_class_A::memb_a)
	.function("show",&test_class_A::show);
	
	