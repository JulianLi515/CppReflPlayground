#include <cassert>
#include <string>
#include <iostream>
#include "../include/static_refl/reflect_core.h"
#include "../include/static_refl/reflect_utils.h"
#include "../include/static_refl/type_list.h"
#include "../include/dynamic_refl/dynamic_reflect_core.h"
#include "../include/dynamic_refl/Any.h"


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
	std::vector<std::string> friends;
	std::set<int> luckyNumbers;
	std::map<std::string, int> scores;

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
containers(
	container(&Person::friends),
	container(&Person::luckyNumbers),
	container(&Person::scores)
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

void test_static_reflection() {
	namespace sta_ref = my_reflect::static_refl;

	std::cout << "========== Static Reflection Test Suite ==========\n\n";

	// Test 1: TypeData extraction
	std::cout << "Test 1: TypeData Extraction\n";
	std::cout << "----------------------------\n";

	using PersonType = sta_ref::TypeData<Person>;
	using StudentType = sta_ref::TypeData<Student>;

	std::cout << "Person has functions, variables, and containers defined\n";
	std::cout << "Student has base class Person and its own fields\n\n";

	// Test 2: Type lists
	std::cout << "Test 2: Type Lists Manipulation\n";
	std::cout << "--------------------------------\n";

	using PersonFunctionTypes = PersonType::function_types;
	using PersonVariableTypes = PersonType::variable_types;
	using PersonContainerTypes = PersonType::container_types;
	using StudentBaseTypes = StudentType::base_types;

	std::cout << "Person function count: " << sta_ref::size_v<PersonFunctionTypes> << "\n";
	std::cout << "Person variable count: " << sta_ref::size_v<PersonVariableTypes> << "\n";
	std::cout << "Person container count: " << sta_ref::size_v<PersonContainerTypes> << "\n";
	std::cout << "Student base class count: " << sta_ref::size_v<StudentBaseTypes> << "\n\n";

	// Test 3: Field traits - Functions
	std::cout << "Test 3: Field Traits - Functions\n";
	std::cout << "---------------------------------\n";

	constexpr auto person_funcs = PersonType::functions;
	auto getName_field = std::get<0>(person_funcs);
	auto setName_field = std::get<1>(person_funcs);
	auto speak_field = std::get<3>(person_funcs);

	std::cout << "getName() - Name: " << getName_field.name_ << "\n";
	std::cout << "  is_function: " << getName_field.is_function() << "\n";
	std::cout << "  is_variable: " << getName_field.is_variable() << "\n";
	std::cout << "  is_const: " << getName_field.is_const() << "\n";
	std::cout << "  is_member: " << getName_field.is_member() << "\n";
	std::cout << "  param_count: " << getName_field.param_count() << "\n";

	std::cout << "\nsetName() - Name: " << setName_field.name_ << "\n";
	std::cout << "  is_const: " << setName_field.is_const() << "\n";
	std::cout << "  param_count: " << setName_field.param_count() << "\n";

	std::cout << "\nspeak() - Name: " << speak_field.name_ << "\n";
	std::cout << "  is_const: " << speak_field.is_const() << "\n";
	std::cout << "  param_count: " << speak_field.param_count() << "\n\n";

	// Test 4: Field traits - Variables
	std::cout << "Test 4: Field Traits - Variables\n";
	std::cout << "---------------------------------\n";

	constexpr auto person_vars = PersonType::variables;
	auto name_field = std::get<0>(person_vars);
	auto age_field = std::get<1>(person_vars);

	std::cout << "name - Name: " << name_field.name_ << "\n";
	std::cout << "  is_function: " << name_field.is_function() << "\n";
	std::cout << "  is_variable: " << name_field.is_variable() << "\n";
	std::cout << "  is_plain_variable: " << name_field.is_plain_variable() << "\n";
	std::cout << "  is_container: " << name_field.is_container() << "\n";
	std::cout << "  is_member: " << name_field.is_member() << "\n";

	std::cout << "\nage - Name: " << age_field.name_ << "\n";
	std::cout << "  is_variable: " << age_field.is_variable() << "\n";
	std::cout << "  is_plain_variable: " << age_field.is_plain_variable() << "\n\n";

	// Test 5: Field traits - Containers
	std::cout << "Test 5: Field Traits - Containers\n";
	std::cout << "----------------------------------\n";

	constexpr auto person_containers = PersonType::containers;
	auto friends_field = std::get<0>(person_containers);
	auto luckyNumbers_field = std::get<1>(person_containers);
	auto scores_field = std::get<2>(person_containers);

	std::cout << "friends - Name: " << friends_field.name_ << "\n";
	std::cout << "  is_function: " << friends_field.is_function() << "\n";
	std::cout << "  is_variable: " << friends_field.is_variable() << "\n";
	std::cout << "  is_plain_variable: " << friends_field.is_plain_variable() << "\n";
	std::cout << "  is_container: " << friends_field.is_container() << "\n";
	std::cout << "  container_kind: " << (int)friends_field.container_kind() << " (0=Set, 1=Vector, 2=Map)\n";

	std::cout << "\nluckyNumbers - Name: " << luckyNumbers_field.name_ << "\n";
	std::cout << "  is_container: " << luckyNumbers_field.is_container() << "\n";
	std::cout << "  container_kind: " << (int)luckyNumbers_field.container_kind() << " (0=Set, 1=Vector, 2=Map)\n";

	std::cout << "\nscores - Name: " << scores_field.name_ << "\n";
	std::cout << "  is_container: " << scores_field.is_container() << "\n";
	std::cout << "  container_kind: " << (int)scores_field.container_kind() << " (0=Set, 1=Vector, 2=Map)\n\n";

	// Test 6: Runtime access with instance
	std::cout << "Test 6: Runtime Access with Instance\n";
	std::cout << "-------------------------------------\n";

	Person person("Alice", 30);
	person.friends = {"Bob", "Charlie", "Diana"};
	person.luckyNumbers = {7, 13, 42};
	person.scores = {{"Math", 95}, {"English", 88}, {"Physics", 92}};

	std::cout << "Created Person: " << person.getName() << ", age " << person.getAge() << "\n";
	std::cout << "Accessing fields via reflection:\n";
	std::cout << "  name (via member ptr): " << person.*name_field.ptr_ << "\n";
	std::cout << "  age (via member ptr): " << person.*age_field.ptr_ << "\n";
	std::cout << "  friends size: " << (person.*friends_field.ptr_).size() << "\n";
	std::cout << "  luckyNumbers size: " << (person.*luckyNumbers_field.ptr_).size() << "\n";
	std::cout << "  scores size: " << (person.*scores_field.ptr_).size() << "\n\n";

	// Test 7: Student inheritance
	std::cout << "Test 7: Inheritance - Student Type\n";
	std::cout << "-----------------------------------\n";

	Student student("Bob", 22, 123456L);

	constexpr auto student_funcs = StudentType::functions;
	constexpr auto student_vars = StudentType::variables;
	auto getID_field = std::get<0>(student_funcs);
	auto studentID_field = std::get<0>(student_vars);

	std::cout << "Student has base class Person: " << std::is_base_of_v<Person, Student> << "\n";
	std::cout << "Student function count: " << sta_ref::size_v<typename StudentType::function_types> << "\n";
	std::cout << "Student variable count: " << sta_ref::size_v<typename StudentType::variable_types> << "\n";
	std::cout << "Student getID() name: " << getID_field.name_ << "\n";
	std::cout << "Student studentID name: " << studentID_field.name_ << "\n";
	std::cout << "Student ID via reflection: " << student.*studentID_field.ptr_ << "\n\n";

	// Test 8: Type list operations
	std::cout << "Test 8: Type List Operations\n";
	std::cout << "-----------------------------\n";

	using list1 = sta_ref::type_list<int, double, char>;
	using list2 = sta_ref::push_t<std::string, list1>;
	using first_type = sta_ref::front_t<list1>;
	using last_type = sta_ref::back_t<list1>;

	std::cout << "list1 size: " << sta_ref::size_v<list1> << "\n";
	std::cout << "list2 (pushed std::string) size: " << sta_ref::size_v<list2> << "\n";
	std::cout << "list1 first type is int: " << std::is_same_v<first_type, int> << "\n";
	std::cout << "list1 last type is char: " << std::is_same_v<last_type, char> << "\n\n";

	// Test 9: Container type extraction
	std::cout << "Test 9: Container Type Extraction\n";
	std::cout << "----------------------------------\n";

	using friends_value_type = decltype(friends_field)::value_type;
	using luckyNumbers_value_type = decltype(luckyNumbers_field)::value_type;
	using scores_key_type = decltype(scores_field)::key_type;
	using scores_value_type = decltype(scores_field)::value_type;

	std::cout << "friends value_type is std::string: " << std::is_same_v<friends_value_type, std::string> << "\n";
	std::cout << "luckyNumbers value_type is int: " << std::is_same_v<luckyNumbers_value_type, int> << "\n";
	std::cout << "scores key_type is std::string: " << std::is_same_v<scores_key_type, std::string> << "\n";
	std::cout << "scores value_type is int: " << std::is_same_v<scores_value_type, int> << "\n\n";

	// Test 10: Function type extraction
	std::cout << "Test 10: Function Type Extraction\n";
	std::cout << "----------------------------------\n";

	using getName_return = decltype(getName_field)::return_type;
	using setName_param = decltype(setName_field)::parameter_type;
	using speak_param = decltype(speak_field)::parameter_type;

	std::cout << "getName return type is const std::string&: " << std::is_same_v<getName_return, const std::string&> << "\n";
	std::cout << "setName param is type_list of (const std::string&): " << (sta_ref::size_v<setName_param> == 1) << "\n";
	std::cout << "speak param count: " << sta_ref::size_v<speak_param> << "\n\n";

	std::cout << "========== All Tests Completed ==========\n";
}

void test_dynamic_reflection() {
	namespace dyn_ref = my_reflect::dynamic_refl;

	std::cout << "\n\n========== Dynamic Reflection Test Suite ==========\n\n";

	// Register std::string first to avoid "Unknown_Class"
	dyn_ref::Register<std::string>()
		.Register("std::string");

	std::cout << "Registered std::string type\n\n";

	// Test 1: Register enum type
	std::cout << "Test 1: Enum Registration\n";
	std::cout << "-------------------------\n";

	dyn_ref::Register<Color>()
		.Register("Color")
		.Add("red", Color::red)
		.Add("green", Color::green)
		.Add("blue", Color::blue);

	const dyn_ref::Type* colorType = dyn_ref::GetType("Color");
	std::cout << "Registered enum type: " << colorType->GetName() << "\n";
	std::cout << "Type kind: " << (int)colorType->GetKind() << " (1=Enum)\n";

	const dyn_ref::Enum* colorEnum = colorType->AsEnum();
	std::cout << "Enum values:\n";
	for (const auto& item : colorEnum->GetItems()) {
		std::cout << "  " << item.name_ << " = " << item.value_ << "\n";
	}
	std::cout << "\n";

	// Test 2: Register Person class
	std::cout << "Test 2: Class Registration - Person\n";
	std::cout << "------------------------------------\n";

	dyn_ref::Register<Person>()
		.Register("Person")
		.Add<decltype(&Person::getName)>("getName")
		.Add<decltype(&Person::setName)>("setName")
		.Add<decltype(&Person::getAge)>("getAge")
		.Add<decltype(&Person::speak)>("speak")
		.Add<decltype(&Person::name)>("name")
		.Add<decltype(&Person::age)>("age")
		.Add<decltype(&Person::friends)>("friends")
		.Add<decltype(&Person::luckyNumbers)>("luckyNumbers")
		.Add<decltype(&Person::scores)>("scores");

	const dyn_ref::Type* personType = dyn_ref::GetType("Person");
	std::cout << "Registered class: " << personType->GetName() << "\n";
	std::cout << "Type kind: " << (int)personType->GetKind() << " (2=Class)\n";

	const dyn_ref::Class* personClass = static_cast<const dyn_ref::Class*>(personType);
	std::cout << "Member functions count: " << personClass->memberFunctions_.size() << "\n";
	std::cout << "Member variables count: " << personClass->memberVariables_.size() << "\n";
	std::cout << "Member containers count: " << personClass->memberContainers_.size() << "\n";
	std::cout << "\n";

	// Test 3: Inspect member functions
	std::cout << "Test 3: Member Functions Inspection\n";
	std::cout << "------------------------------------\n";

	for (const auto& func : personClass->memberFunctions_) {
		std::cout << "Function: " << func.name_ << "\n";
		std::cout << "  Return type: " << func.retType_->GetName() << "\n";
		std::cout << "  Parameters count: " << func.argTypes_.size() << "\n";
		if (!func.argTypes_.empty()) {
			std::cout << "  Parameter types: ";
			for (const auto* paramType : func.argTypes_) {
				std::cout << paramType->GetName() << " ";
			}
			std::cout << "\n";
		}
	}
	std::cout << "\n";

	// Test 4: Inspect member variables
	std::cout << "Test 4: Member Variables Inspection\n";
	std::cout << "------------------------------------\n";

	for (const auto& var : personClass->memberVariables_) {
		std::cout << "Variable: " << var.name_ << "\n";
		std::cout << "  Type: " << var.type_->GetName() << "\n";
	}
	std::cout << "\n";

	// Test 5: Inspect member containers
	std::cout << "Test 5: Member Containers Inspection\n";
	std::cout << "-------------------------------------\n";

	for (const auto& container : personClass->memberContainers_) {
		std::cout << "Container: " << container.name_ << "\n";
		std::cout << "  Kind: " << (int)container.kind_ << " (0=Set, 1=Vector, 2=Map)\n";
		std::cout << "  Value type: " << container.valueType_->GetName() << "\n";
		if (container.keyType_ != nullptr) {
			std::cout << "  Key type: " << container.keyType_->GetName() << "\n";
		}
	}
	std::cout << "\n";

	// Test 6: Register Student with base class
	std::cout << "Test 6: Class Registration - Student with Base Class\n";
	std::cout << "-----------------------------------------------------\n";

	dyn_ref::Register<Student>()
		.Register("Student")
		.AddBaseClass<Person>()
		.Add<decltype(&Student::getID)>("getID")
		.Add<decltype(&Student::setID)>("setID")
		.Add<decltype(&Student::studentID)>("studentID");

	const dyn_ref::Type* studentType = dyn_ref::GetType("Student");
	const dyn_ref::Class* studentClass = static_cast<const dyn_ref::Class*>(studentType);

	std::cout << "Registered class: " << studentClass->GetName() << "\n";
	std::cout << "Base classes count: " << studentClass->baseClasses_.size() << "\n";
	if (!studentClass->baseClasses_.empty()) {
		std::cout << "Base class: " << studentClass->baseClasses_[0]->GetName() << "\n";
	}
	std::cout << "Member functions count: " << studentClass->memberFunctions_.size() << "\n";
	std::cout << "Member variables count: " << studentClass->memberVariables_.size() << "\n";
	std::cout << "\n";

	// Test 7: Query type by name
	std::cout << "Test 7: Type Lookup by Name\n";
	std::cout << "----------------------------\n";

	dyn_ref::Type* foundPerson = dyn_ref::GetType("Person");
	dyn_ref::Type* foundStudent = dyn_ref::GetType("Student");
	dyn_ref::Type* foundColor = dyn_ref::GetType("Color");

	std::cout << "Found 'Person': " << (foundPerson != nullptr ? foundPerson->GetName() : "Not found") << "\n";
	std::cout << "Found 'Student': " << (foundStudent != nullptr ? foundStudent->GetName() : "Not found") << "\n";
	std::cout << "Found 'Color': " << (foundColor != nullptr ? foundColor->GetName() : "Not found") << "\n";
	std::cout << "\n";

	// Test 8: Get all registered types
	std::cout << "Test 8: All Registered Types\n";
	std::cout << "-----------------------------\n";

	const auto& allTypes = dyn_ref::GetAllTypes();
	std::cout << "Total registered types: " << allTypes.size() << "\n";
	std::cout << "Type names:\n";
	for (const auto& [name, type] : allTypes) {
		std::cout << "  " << name << " (Kind: " << (int)type->GetKind() << ")\n";
	}
	std::cout << "\n";

	// Test 9: Pointer type
	std::cout << "Test 9: Pointer Type\n";
	std::cout << "--------------------\n";

	const dyn_ref::Type* intPtrType = dyn_ref::GetType<int*>();
	std::cout << "int* type name: " << intPtrType->GetName() << "\n";
	std::cout << "Type kind: " << (int)intPtrType->GetKind() << " (3=Pointer)\n";

	const dyn_ref::Pointer* ptrInfo = static_cast<const dyn_ref::Pointer*>(intPtrType);
	std::cout << "Pointed type: " << ptrInfo->pointedType_->GetName() << "\n";
	std::cout << "\n";

	// Test 10: Arithmetic types
	std::cout << "Test 10: Arithmetic Types\n";
	std::cout << "-------------------------\n";

	const dyn_ref::Type* intType = dyn_ref::GetType<int>();
	const dyn_ref::Type* doubleType = dyn_ref::GetType<double>();
	const dyn_ref::Type* boolType = dyn_ref::GetType<bool>();

	std::cout << "int type: " << intType->GetName() << " (Kind: " << (int)intType->GetKind() << ")\n";
	std::cout << "double type: " << doubleType->GetName() << " (Kind: " << (int)doubleType->GetKind() << ")\n";
	std::cout << "bool type: " << boolType->GetName() << " (Kind: " << (int)boolType->GetKind() << ")\n";
	std::cout << "\n";

	std::cout << "========== All Dynamic Tests Completed ==========\n";
}




int main() {
	test_static_reflection();
	test_dynamic_reflection();
	return 0;
}
