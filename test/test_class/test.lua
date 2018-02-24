function test1(obj1,obj2)
	print(obj1,obj2)
	print(obj1.memb_a)
	obj1.memb_a = 101
	obj1:show()
end

function test2(b,c)
	print(b,c)
	b:show()
	c:show2()
	c:child_show()
	b:child_show() --this should failed
end

function  test3(obj)
	print("obj.memb_a",obj.memb_a)
	pass_by_value(obj)
	print("after call pass_by_value",obj.memb_a)
	pass_by_reference(obj)
	print("after call pass_by_reference",obj.memb_a)
end

function  test4(obj)
	return {obj,obj}
end

function test5()
	a = test_class_A()
	b = pass_by_reference(a)
	print(a,b,a.memb_a,b.memb_a)
end