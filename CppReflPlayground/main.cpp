#include <string>
#include <iostream>
#include "reflect_core.h"

int g_value = 3;

bool Foo(int) {
	return false;
}

class Bar {
public:
	int f1;
	void f2(int a, double b){}
	static void f3(int a) {};
};


class Person {
public:
	Person(std::string n, int a) :name(n), age(a) {}
	const std::string& getName() const { return name; }
	void setName(const std::string& n) { name = n; }
	int getAge() const { return age; }
	int age;
	std::string name;
};

BEGIN_REFLECT(Person)
functions(
	func(&Person::getName),
	func(&Person::setName),
	func(&Person::getAge)
)
fields(
	var(&Person::name),
	var(&Person::age)
)
END_REFLECT()



int main() {
	auto typeInfo = my_reflect::type_data<Person>();
	std::cout << std::get<1>(typeInfo.functions)._name << std::endl;
	/*auto FunPtr = &Foo;
	auto ClassFunPtr = &Bar::f2;
	using func_info = function_traits<decltype(FunPtr) > ;
	using func_RetType = func_info::return_type;
	using func_ParamType = func_info::parameter_type;
	constexpr bool isClassFunc_func = func_info::is_member;
	using classFunc_info = function_traits<decltype(ClassFunPtr)>;
	using classFunc_RetType = classFunc_info::return_type;
	using classFunc_ParamType = classFunc_info::parameter_type;
	using classFunc_ClassType = classFunc_info::class_type;
	constexpr bool isClassFunc_classFunc = classFunc_info::is_member;

	auto VarPtr = &g_value;
	auto ClassVarPtr = &Bar::f1;
	using var_info = variable_traits<decltype(VarPtr)>;
	using var_type = var_info::type;*/

	/*auto FuncPtr = &Person::getName;
	constexpr bool isFuncPtr_const = function_traits<decltype(FuncPtr)>::is_const;

	auto FuncPtr2 = &Person::setName;
	auto fieldInfo = field_traits{ FuncPtr2};
	constexpr bool isFunc1_const = fieldInfo.is_const();
	std::cout << "isFunc1_member: " << isFunc1_const << std::endl;
	using field_type = decltype(fieldInfo);
	using field_RetType = field_type::return_type;
	using field_ParamType = field_type::parameter_type;*/
}