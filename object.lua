    
Account = {
balance = 10,
gtest = 1,
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




