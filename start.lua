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

function test1()
	cshow("hello")
end

function test2(obj)
	print(obj.memb_a)
	obj.memb_a = 101
	obj:show()
end

function test3(lt)
	for i = 1,#lt do
		print(lt[i])
	end
end

function test4()
	local lt = c_return_luatable()
	for i = 1,#lt do
		print(lt[i])
	end	
end

function test5()
	return {"a","b","c","d","e","f"}
end

function test6()
	account = Account:new()
	account.balance = 100
	account.name = "sniperHW"
	return account
end

function test7(b,c)
	b:show()
	c:show2()
end

function test8(a,b)
	print(a + b * 2)
	print(a*20 - b * 2)
end

function test9(obj)
	print(obj.memb_a)
	obj.memb_a = 101
	obj:show()
end
function test10(obj)
	arg_c_object(obj)
end

function test11(obj)
	arg_c_object_ref(obj)
end

function test12(obj)
	local t1 = test_class_A()
	t1.memb_a = 100
	local t2 = test_class_A(t1)
	print(t2.memb_a)		
end

t_table = {1,2,3,4}

function test13()
	print(TEST_GLOBAL)
end


