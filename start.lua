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

function t(tb)
	--tb:func()
end

function test1(tb,tc)
	--show("hello world")
	--local mt = getmetatable(tb);
	--mt.tt = "a"
	--tb.tt
	--mt.func()
	--print(mt.func())
	--printf(mt.func);
	--print("tb.valb = " .. tb.valb)
	--tb.valb = 200
	tc:func()
	tb:func()
	tc:funcc()
	--tb()
	local i = i64.new(100)
	print(i)
	--local mt = getmetatable(i)
	--print(mt)
	--local mt2 = getmetatable(tb2)
	--print(mt2)
	print(tb.valb)
	--local i2 = i64.new()
	--print(i)
	--local i3 = i + i2
	
	--print(i)
	--print("" .. i)
end

function test2()
	return {1,2,3,4,5}
end

function test3()
	account = Account:new();
	account.balance = 100
	account.name = "sniperHW"
	return account
end

function test4(account)
	print(account.name)
end



