###C++对lua访问接口的封装,提供访问lua函数，向lua暴露普通C函数，向lua注册C++类型，对lua对象和表的封装等等.


####[向lua注册函数]

	static int showmsg(const char *msg)
	{
		printf("c-showmsg:%s\n",msg);
		return 0;
	}
	
	luacpp::reg_cfun(L,"cshow",showmsg);

####[调用lua函数]

	luacpp::call<void>(L,"test1");

####[注册c++类型]

	--C++ code

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
	
	luacpp::reg_cclass<test_class_A>::_reg(L,"test_class_A")
		.constructor<void>()//无参构造
		.constructor<const test_class_A&>()//一个参数构造
		.property("memb_a",&test_class_A::memb_a)
		.function("show",&test_class_A::show);

	--lua code

	function test12(obj)
		local t1 = test_class_A()
		t1.memb_a = 100
		local t2 = test_class_A(t1)
		print(t2.memb_a)
		t2:show()		
	end


	
####[获取lua全局变量]

	luatable lt = luacpp::Get<luatable>(L,"t_table");

####[设置lua全局变量]

	luacpp::Set(L,"TEST_GLOBAL","this is test global");


####[访问lua中的Object]

	--lua code
	Account = {
		balance = 10,
		names=0,
		[1] = 1,
		[2.2] = 2.2,
	}
	    
	function Account:withdraw (v)
	      self.balance = self.balance - v
	end
	    
	function Account:new (o)
	  o = o or {}   
	  setmetatable(o, self)
	  self.__index = self
	  return o
	end
	
	function Account:show()
		print("this is account show")
	end
	
	function Account:getBalance()
		return self.balance
	end
	
	function Account:setBalance(val)
		self.balance = val
	end
	
	function GetAccount()
		return Account:new()
	end


	--C++ code
	luaObject lo = luacpp::call<luaObject>(L,"GetAccount");
	lo.call<void>("show");
	printf("balance:%d\n",lo.Get<int>("balance"));
	lo.Set("balance",1000);
	printf("balance:%d\n",lo.Get<int>("balance"));
	printf("1:%d\n",lo.Get<int>(1));
	printf("2:%f\n",lo.Get<double>(2.2));


###[访问数组形式的lua表]

####[从lua返回]

	--lua code 
	function get_lua_array()
		return {"a","b","c","d","e","f"}
	end


	--C++ code

	luatable lt = luacpp::call<luatable>(L,"get_lua_array");
	for(int i = 0 ;i < (int)lt.size(); ++i)
		printf("%s\n",any_cast<std::string>(lt[i]).c_str());
		

####[传递到lua]
	--C++ code

	luatable lt;
	lt.push_back(1);
	lt.push_back(2);
	lt.push_back(3);
	luacpp::call<void>(L,"pass_luatable",lt);


	--lua code 
	function pass_luatable(lt)
		print(lt[1])
		print(lt[2])
		print(lt[3])
	end
