Account = {
balance = 10,
names=0,
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
	return {1,2,3,4,5,6}
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


