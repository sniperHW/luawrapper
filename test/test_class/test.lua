
function test1(obj1,obj2)
	print("test1")
	print(obj1,obj2)
	print(obj1.memb_a)
	obj1.memb_a = 101
	obj1:show()
	obj1 = nil
	obj2 = nil
	collectgarbage("collect")
end

function test2(b,c)
	print("test2")
	print(b,c)
	b:show()
	c:show2()
	c:child_show()
	b:child_show() --this should failed
end

function  test3(obj)
	print("test3")
	print("obj.memb_a",obj.memb_a)
	pass_by_value(obj)
	print("after call pass_by_value",obj.memb_a)
	pass_by_reference(obj)
	print("after call pass_by_reference",obj.memb_a)
end

function  test4(obj)
	print("test4")
	return {obj,obj}
end

function test5()
	print("test5")
	local a = test_class_A(999)
	print(a,a.memb_a)
	local b = pass_by_reference(a)
	print(a,b)
	print(a,b,a.memb_a,b.memb_a)
end

--print(test1)