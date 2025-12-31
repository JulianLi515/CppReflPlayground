#include <string>
#include <iostream>
#include "../include/static_refl/reflect_core.h"
#include "../include/static_refl/reflect_utils.h"
#include "../include/static_refl/type_list.h"
#include "../include/dynamic_refl/dynamic_reflect_core.h"


static int g_value = 3;

static bool foo(int) {
	return false;
}

class Bar {
public:
	int f1;
	void f2(int a, double b){}
	static void f3(int a) {}
};


class Person {
public:
	Person(const std::string& n, int a) :age(a), name(n) {}
	const std::string& getName() const { return name; }
	void setName(const std::string& n) { name = n; }
	int getAge() const { return age; }
	Person* GetThis() { return this; }
	void speak (const std::string&, int duration) const{}
	int age;
	std::string name;
};

class Student : public Person {
public:
	Student(const std::string& n, int a, long id) : Person(n, a), studentID(id) {}
	void setID(long id) { studentID = id;}
	long getID() const { return studentID; }
	long studentID;
};

class Gay : public Person
{
public:
	Gay(const std::string& n, int a, bool b) : Person(n, a), is0(b) {}
	void sing() {};
	bool is0;
};

BEGIN_REFLECT(Person)
BASE_CLASSES()
functions(
	func(&Person::getName),
	func(&Person::setName),
	func(&Person::getAge),
	func(&Person::speak)
)
variables(
	var(&Person::name),
	var(&Person::age)
)
END_REFLECT()

BEGIN_REFLECT(Student)
BASE_CLASSES(Person)
functions(
	func(&Student::getID),
	func(&Student::setID)
)
variables(
	var(&Student::studentID)
)
END_REFLECT()

BEGIN_REFLECT(Gay)
BASE_CLASSES(Person)
functions(
	func(&Gay::sing)
)
variables(
	var(&Gay::is0)
)
END_REFLECT()


enum class Color { red, green, blue };

int main() {
	namespace dyn_ref = my_reflect::dynamic_refl;
	namespace sta_ref = my_reflect::static_refl;

	dyn_ref::Register<Person>()
		.Register("Person")
		.Add<decltype(&Person::getName)>("getName")
		.Add<decltype(&Person::setName)>("setName")
		.Add<decltype(&Person::getAge)>("getAge")
		.Add<decltype(&Person::speak)>("speak")
		.Add<decltype(&Person::GetThis)>("GetThis")
		.Add<decltype(&Person::age)>("age")
		.Add<decltype(&Person::name)>("name");
	const my_reflect::dynamic_refl::Type* typeInfo = dyn_ref::GetType("Person");

	std::cout << typeInfo->GetName() << '\n';


	// my_reflect::dynamic_refl::Register<Color>()
	// 			.Register("Color")
	// 			.Add("red", Color::red)
	// 			.Add("green", Color::green)
	// 			.Add("blue", Color::blue);
	// const my_reflect::dynamic_refl::Type* typeInfo = my_reflect::dynamic_refl::GetType("Color");
	// const my_reflect::dynamic_refl::Type* typeInfo2 = my_reflect::dynamic_refl::GetType<int>();
	// const my_reflect::dynamic_refl::Enum* enumInfo = typeInfo->AsEnum();
	// for (const auto& item: enumInfo->GetItems()) {
	// 	std::cout << item.name_ << '\n';
	// }
	// std::cout << typeInfo->GetName() << '\n';
	// // can be used with instance
	// Student stu("Bob", 22, 654321L);
	// my_reflect::static_refl::utils::print_all(stu);
	//
	// // can be used without constructing instance (compile time only)
	// using typeData = my_reflect::static_refl::TypeData<Student>;
	// using function_types = typeData::function_types;




	// constexpr auto sss = my_reflect::static_refl::TypeData<Gay>::variables;
	// typeInfo.variables
	// my_reflect::static_refl::utils::print_all(stu);
	// auto s = &Student::getName;
	// typeInfo.variables;
	// using o = decltype(studentTypeInfo)::base_types;
	// auto f =  std::get<0>(studentTypeInfo.functions);
	// using e = decltype(f);
	// using n = e::class_type;// Should be person
	//
	// std::cout << std::get<3>(typeInfo.functions).param_cout() << '\n';
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