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
	tb:func()
end

function test1(tb,i,ptr_2_b)
	t(tb)
	show("hello world")
	print(tb.valb)
	tb.valb = i64:new(10000003) 
	tb.vald = 1000.2
	print("data1:" .. tb._st.data1)
	print(tb._lt[1])
	print(tb._lt[2])
	tb._lt = {3,4}
	print(tb.str)
	print(tb._str)
	tb.str = "shshs"
	tb._str = "fuck you"
	tb.ptr = ptr_2_b
	print(i)
end

function test2()
	return {1,2,3,4,5}
end

function test3()
	account = Account:new()
	account.balance = 100
	account.name = "sniperHW"
	return account
end

function test4()
	return 1,2,3,4,5
end

function test5(lt)
	print(lt[1])
	print(lt[2])
	print(lt[3])
	print(lt[4])
	print(lt[5])
end

function test6(str)
	print(str)
end



