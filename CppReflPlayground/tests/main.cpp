#include <cassert>
#include <string>
#include <iostream>
#include "../include/static_refl/reflect_core.h"
#include "../include/static_refl/reflect_utils.h"
#include "../include/static_refl/type_list.h"
#include "../include/dynamic_refl/dynamic_reflect_core.h"
#include "../include/dynamic_refl/Any.h"
#include "../include/dynamic_refl/Arithmetic.h"
#include "../include/dynamic_refl/MemberContainer.h"
#include "../include/dynamic_refl/container_operations.h"


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
	Person(const std::string& n, int a) :age(a), name(n) {
		std::cout << "[Person] Constructor: " << name << ", age " << age << "\n";
	}

	Person(const Person& other) : age(other.age), name(other.name),
		friends(other.friends), luckyNumbers(other.luckyNumbers), scores(other.scores) {
		std::cout << "[Person] Copy Constructor: " << name << "\n";
	}

	Person(Person&& other) noexcept : age(other.age), name(std::move(other.name)),
		friends(std::move(other.friends)), luckyNumbers(std::move(other.luckyNumbers)),
		scores(std::move(other.scores)) {
		std::cout << "[Person] Move Constructor: " << name << "\n";
	}

	~Person() {
		std::cout << "[Person] Destructor: " << name << "\n";
	}

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
	std::cout << "  scores size: " << (person.*scores_field.ptr_).size() << "\n";

	// Test calling functions via reflection
	std::cout << "\nCalling functions via reflection:\n";
	std::cout << "  getName() via ptr: " << (person.*getName_field.ptr_)() << "\n";
	std::cout << "  getAge() via ptr: " << (person.*std::get<2>(person_funcs).ptr_)() << "\n";

	// Call setName
	(person.*setName_field.ptr_)("Reflected Alice");
	std::cout << "  After setName via ptr: " << person.getName() << "\n";

	// Call speak
	std::string message = "Hello from reflection";
	(person.*speak_field.ptr_)(message, 5);
	std::cout << "  speak() called via ptr successfully\n\n";

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

	// Test 11: Static reflection find function index
	std::cout << "Test 11: Static Reflection Find Function Index\n";
	std::cout << "-----------------------------------------------\n";

	Person person2("InvokeTest", 35);

	// Find function indices (constexpr - happens at compile time!)
	constexpr int getName_idx = PersonType::find_function_index("getName");
	constexpr int setName_idx = PersonType::find_function_index("setName");
	constexpr int getAge_idx = PersonType::find_function_index("getAge");
	constexpr int speak_idx = PersonType::find_function_index("speak");
	constexpr int notFound_idx = PersonType::find_function_index("nonExistent");

	std::cout << "Function indices (found at compile time):\n";
	std::cout << "  getName: " << getName_idx << "\n";
	std::cout << "  setName: " << setName_idx << "\n";
	std::cout << "  getAge: " << getAge_idx << "\n";
	std::cout << "  speak: " << speak_idx << "\n";
	std::cout << "  nonExistent: " << notFound_idx << " (not found)\n\n";

	// Test 12: Static reflection invoke by index
	std::cout << "Test 12: Static Reflection Invoke by Index\n";
	std::cout << "-------------------------------------------\n";

	try {
		// Direct index invocation
		auto name_by_idx = PersonType::invoke<0>(person2);
		std::cout << "PersonType::invoke<0>(person2): " << name_by_idx << "\n";

		auto age_by_idx = PersonType::invoke<2>(person2);
		std::cout << "PersonType::invoke<2>(person2): " << age_by_idx << "\n";

		PersonType::invoke<1>(person2, std::string("IndexedName"));
		std::cout << "After PersonType::invoke<1>(person2, \"IndexedName\"): " << person2.getName() << "\n";

		// Using constexpr index (compile-time name lookup!)
		if constexpr (getName_idx >= 0) {
			auto result = PersonType::invoke<getName_idx>(person2);
			std::cout << "PersonType::invoke<getName_idx>(person2): " << result << "\n";
		}
	} catch (const std::exception& e) {
		std::cout << "ERROR: " << e.what() << "\n";
	}
	std::cout << "\n";

	// Test 13: Compile-time function existence check
	std::cout << "Test 13: Compile-Time Function Existence Check\n";
	std::cout << "-----------------------------------------------\n";

	if constexpr (PersonType::find_function_index("getName") >= 0) {
		std::cout << "getName exists (checked at compile time!)\n";
	}

	if constexpr (PersonType::find_function_index("nonExistent") < 0) {
		std::cout << "nonExistent does not exist (checked at compile time!)\n";
	}
	std::cout << "\n";

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
		.Add("getName", &Person::getName)
		.Add("setName", &Person::setName)
		.Add("getAge", &Person::getAge)
		.Add("speak", &Person::speak)
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




void test_any() {
	namespace dyn_ref = my_reflect::dynamic_refl;

	std::cout << "\n\n========== Any Type Test Suite ==========\n\n";

	// Register Person type for Any
	dyn_ref::Register<Person>()
		.Register("Person");

	// Test 1: make_copy
	std::cout << "Test 1: make_copy - Should call Copy Constructor\n";
	std::cout << "------------------------------------------------\n";
	{
		Person p1("Alice", 25);
		std::cout << "Creating Any with make_copy...\n";
		auto any1 = dyn_ref::make_copy(p1);
		std::cout << "Any created. Storage type: " << (int)any1.storage() << " (1=Copy)\n";
		std::cout << "Exiting scope, should destroy Any and its contained Person...\n";
	}
	std::cout << "Scope exited.\n\n";

	// Test 2: make_move
	std::cout << "Test 2: make_move - Should call Move Constructor\n";
	std::cout << "-------------------------------------------------\n";
	{
		Person p2("Bob", 30);
		std::cout << "Creating Any with make_move...\n";
		auto any2 = dyn_ref::make_move(std::move(p2));
		std::cout << "Any created. Storage type: " << (int)any2.storage() << " (2=Move)\n";
		std::cout << "Exiting scope, should destroy Any and its contained Person...\n";
	}
	std::cout << "Scope exited.\n\n";

	// Test 3: make_ref
	std::cout << "Test 3: make_ref - Should NOT call Copy/Move Constructor\n";
	std::cout << "---------------------------------------------------------\n";
	{
		Person p3("Charlie", 35);
		std::cout << "Creating Any with make_ref...\n";
		auto any3 = dyn_ref::make_ref(p3);
		std::cout << "Any created. Storage type: " << (int)any3.storage() << " (3=Ref)\n";

		Person* ptr = dyn_ref::any_cast<Person>(any3);
		if (ptr) {
			std::cout << "any_cast successful: " << ptr->getName() << ", age " << ptr->getAge() << "\n";
		}

		std::cout << "Exiting scope, should destroy Any but NOT the referenced Person...\n";
	}
	std::cout << "Scope exited.\n\n";

	// Test 4: make_cref
	std::cout << "Test 4: make_cref - Const Reference\n";
	std::cout << "------------------------------------\n";
	{
		const Person p4("Diana", 28);
		std::cout << "Creating Any with make_cref...\n";
		auto any4 = dyn_ref::make_cref(p4);
		std::cout << "Any created. Storage type: " << (int)any4.storage() << " (4=ConstRef)\n";

		const Person* ptr = dyn_ref::any_cast<Person>(any4);
		if (ptr) {
			std::cout << "any_cast successful: " << ptr->getName() << ", age " << ptr->getAge() << "\n";
		}

		std::cout << "Exiting scope, should destroy Any but NOT the const referenced Person...\n";
	}
	std::cout << "Scope exited.\n\n";

	// Test 5: Any copy constructor
	std::cout << "Test 5: Any Copy Constructor\n";
	std::cout << "----------------------------\n";
	{
		Person p5("Eve", 22);
		auto any5 = dyn_ref::make_copy(p5);
		std::cout << "Creating any6 by copying any5...\n";
		auto any6 = any5;
		std::cout << "Both Any objects created.\n";
		std::cout << "Exiting scope, should destroy both Any objects and their Persons...\n";
	}
	std::cout << "Scope exited.\n\n";

	// Test 6: Any move constructor
	std::cout << "Test 6: Any Move Constructor\n";
	std::cout << "----------------------------\n";
	{
		Person p6("Frank", 40);
		auto any7 = dyn_ref::make_copy(p6);
		std::cout << "Moving any7 to any8...\n";
		auto any8 = std::move(any7);
		std::cout << "any7 is now empty: " << any7.empty() << " (1=true)\n";
		std::cout << "any8 storage type: " << (int)any8.storage() << "\n";
		std::cout << "Exiting scope, should destroy only any8's Person (any7 is empty)...\n";
	}
	std::cout << "Scope exited.\n\n";

	// Test 7: Any copy assignment
	std::cout << "Test 7: Any Copy Assignment\n";
	std::cout << "---------------------------\n";
	{
		Person p7("Grace", 27);
		Person p8("Henry", 33);
		auto any9 = dyn_ref::make_copy(p7);
		auto any10 = dyn_ref::make_copy(p8);
		std::cout << "Assigning any10 = any9...\n";
		any10 = any9;
		std::cout << "Assignment complete. Both should have Grace.\n";
		std::cout << "Exiting scope, should destroy both Any objects and two Grace copies plus one Henry...\n";
	}
	std::cout << "Scope exited.\n\n";

	// Test 8: Any move assignment
	std::cout << "Test 8: Any Move Assignment\n";
	std::cout << "---------------------------\n";
	{
		Person p9("Ivy", 29);
		Person p10("Jack", 31);
		auto any11 = dyn_ref::make_copy(p9);
		auto any12 = dyn_ref::make_copy(p10);
		std::cout << "Assigning any12 = std::move(any11)...\n";
		any12 = std::move(any11);
		std::cout << "any11 is now empty: " << any11.empty() << "\n";
		std::cout << "Exiting scope, should destroy Jack first, then Ivy...\n";
	}
	std::cout << "Scope exited.\n\n";

	std::cout << "========== All Any Tests Completed ==========\n";
}

void test_any_operations() {
	namespace dyn_ref = my_reflect::dynamic_refl;

	std::cout << "\n\n========== Any Operations Test Suite ==========\n\n";

	// Test 1: Arithmetic operations with any_set and any_get
	std::cout << "Test 1: Arithmetic - any_set and any_get\n";
	std::cout << "-----------------------------------------\n";
	{
		auto int_any = dyn_ref::make_copy(42);
		std::cout << "Created int Any with value: " << dyn_ref::any_get<int>(int_any).value_or(-1) << "\n";

		// Set new value
		bool success = dyn_ref::any_set(int_any, 100);
		std::cout << "any_set(int_any, 100): " << (success ? "success" : "failed") << "\n";
		std::cout << "New value: " << dyn_ref::any_get<int>(int_any).value_or(-1) << "\n";

		// Try to set wrong type
		success = dyn_ref::any_set(int_any, 3.14);
		std::cout << "any_set(int_any, 3.14): " << (success ? "success" : "failed") << " (should fail)\n";
		std::cout << "Value unchanged: " << dyn_ref::any_get<int>(int_any).value_or(-1) << "\n";
	}
	std::cout << "\n";

	// Test 2: Arithmetic::SetValue and GetValue with type checking
	std::cout << "Test 2: Arithmetic::SetValue and GetValue\n";
	std::cout << "------------------------------------------\n";
	{
		auto double_any = dyn_ref::make_copy(3.14);
		std::cout << "Created double Any with value: " << dyn_ref::Arithmetic::GetValue<double>(double_any).value_or(0.0) << "\n";

		// Set new value
		bool success = dyn_ref::Arithmetic::SetValue(double_any, 2.71);
		std::cout << "Arithmetic::SetValue(double_any, 2.71): " << (success ? "success" : "failed") << "\n";
		std::cout << "New value: " << dyn_ref::Arithmetic::GetValue<double>(double_any).value_or(0.0) << "\n";

		// Try with wrong type - should throw
		try {
			auto person_any = dyn_ref::make_copy(Person("Test", 25));
			dyn_ref::Arithmetic::SetValue(person_any, 42);
			std::cout << "ERROR: Should have thrown exception!\n";
		} catch (const std::bad_cast&) {
			std::cout << "Correctly threw std::bad_cast for non-arithmetic type\n";
		}
	}
	std::cout << "\n";

	// Test 3: Const reference protection
	std::cout << "Test 3: Const Reference Protection\n";
	std::cout << "-----------------------------------\n";
	{
		int value = 42;
		auto const_any = dyn_ref::make_cref(value);
		std::cout << "Created const ref Any with value: " << dyn_ref::any_get<int>(const_any).value_or(-1) << "\n";

		// Try to modify - should throw
		try {
			dyn_ref::any_set(const_any, 100);
			std::cout << "ERROR: Should have thrown exception!\n";
		} catch (const std::runtime_error& e) {
			std::cout << "Correctly threw exception: " << e.what() << "\n";
		}

		std::cout << "Original value unchanged: " << value << "\n";
	}
	std::cout << "\n";

	// Test 4: Enum operations - SetByName and GetName
	std::cout << "Test 4: Enum::SetByName and GetName\n";
	std::cout << "------------------------------------\n";
	{
		Color color = Color::red;
		auto enum_any = dyn_ref::make_copy(color);

		const auto* enumType = dyn_ref::GetType("Color")->AsEnum();
		if (enumType) {
			auto name = enumType->GetName(enum_any);
			std::cout << "Initial color name: " << name.value_or("unknown") << "\n";

			// Set by name
			bool success = enumType->SetByName(enum_any, "green");
			std::cout << "SetByName(enum_any, \"green\"): " << (success ? "success" : "failed") << "\n";

			name = enumType->GetName(enum_any);
			std::cout << "New color name: " << name.value_or("unknown") << "\n";

			// Try invalid name
			success = enumType->SetByName(enum_any, "invalid");
			std::cout << "SetByName(enum_any, \"invalid\"): " << (success ? "success" : "failed") << " (should fail)\n";
		}
	}
	std::cout << "\n";

	// Test 5: Enum operations - SetByValue and GetValue
	std::cout << "Test 5: Enum::SetByValue and GetValue\n";
	std::cout << "--------------------------------------\n";
	{
		Color color = Color::blue;
		auto enum_any = dyn_ref::make_copy(color);

		const auto* enumType = dyn_ref::GetType("Color")->AsEnum();
		if (enumType) {
			auto value = enumType->GetValue(enum_any);
			std::cout << "Initial color value: " << value.value_or(-1) << "\n";

			// Set by value
			bool success = enumType->SetByValue(enum_any, 0); // red
			std::cout << "SetByValue(enum_any, 0): " << (success ? "success" : "failed") << "\n";

			auto name = enumType->GetName(enum_any);
			std::cout << "New color name: " << name.value_or("unknown") << "\n";

			// Try invalid value
			success = enumType->SetByValue(enum_any, 999);
			std::cout << "SetByValue(enum_any, 999): " << (success ? "success" : "failed") << " (should fail)\n";
		}
	}
	std::cout << "\n";

	// Test 6: Reference semantics with any_set
	std::cout << "Test 6: Reference Semantics with any_set\n";
	std::cout << "-----------------------------------------\n";
	{
		int original = 50;
		auto ref_any = dyn_ref::make_ref(original);

		std::cout << "Original value: " << original << "\n";
		std::cout << "Ref Any value: " << dyn_ref::any_get<int>(ref_any).value_or(-1) << "\n";

		// Modify through Any
		dyn_ref::any_set(ref_any, 200);
		std::cout << "After any_set(ref_any, 200):\n";
		std::cout << "  Original value: " << original << " (should be 200)\n";
		std::cout << "  Ref Any value: " << dyn_ref::any_get<int>(ref_any).value_or(-1) << "\n";
	}
	std::cout << "\n";

	std::cout << "========== All Any Operations Tests Completed ==========\n";
}

void test_container_operations() {
	namespace dyn_ref = my_reflect::dynamic_refl;

	std::cout << "\n\n========== Container Operations Test Suite ==========\n\n";

	// Test 1: Vector operations
	std::cout << "Test 1: Vector Operations\n";
	std::cout << "-------------------------\n";
	{
		std::vector<int> vec = {1, 2, 3};
		auto vec_any = dyn_ref::make_ref(vec);

		// Create MemberContainer info with operations
		auto containerInfo = dyn_ref::MemberContainer::Create<std::vector<int>>("test_vec");

		std::cout << "Initial vector size: " << dyn_ref::container_ops::Size(containerInfo, vec_any) << "\n";

		// Push new element
		auto value_any = dyn_ref::make_copy(42);
		bool success = dyn_ref::container_ops::Push(containerInfo, vec_any, value_any);
		std::cout << "Push 42: " << (success ? "success" : "failed") << "\n";
		std::cout << "New size: " << dyn_ref::container_ops::Size(containerInfo, vec_any) << "\n";

		// Access by index
		auto elem_any = dyn_ref::container_ops::At(containerInfo, vec_any, 3);
		auto* elem_ptr = dyn_ref::any_cast<int>(elem_any);
		std::cout << "Element at index 3: " << (elem_ptr ? *elem_ptr : -1) << "\n";

		// Clear
		dyn_ref::container_ops::Clear(containerInfo, vec_any);
		std::cout << "After clear, size: " << dyn_ref::container_ops::Size(containerInfo, vec_any) << "\n";
	}
	std::cout << "\n";

	// Test 2: Set operations
	std::cout << "Test 2: Set Operations\n";
	std::cout << "----------------------\n";
	{
		std::set<std::string> set_obj = {"apple", "banana"};
		auto set_any = dyn_ref::make_ref(set_obj);

		auto containerInfo = dyn_ref::MemberContainer::Create<std::set<std::string>>("test_set");

		std::cout << "Initial set size: " << dyn_ref::container_ops::Size(containerInfo, set_any) << "\n";

		// Insert new element
		auto value_any = dyn_ref::make_copy(std::string("cherry"));
		bool success = dyn_ref::container_ops::Push(containerInfo, set_any, value_any);
		std::cout << "Insert 'cherry': " << (success ? "success" : "failed") << "\n";
		std::cout << "New size: " << dyn_ref::container_ops::Size(containerInfo, set_any) << "\n";

		// Clear
		dyn_ref::container_ops::Clear(containerInfo, set_any);
		std::cout << "After clear, size: " << dyn_ref::container_ops::Size(containerInfo, set_any) << "\n";
	}
	std::cout << "\n";

	// Test 3: Map operations
	std::cout << "Test 3: Map Operations\n";
	std::cout << "----------------------\n";
	{
		std::map<std::string, int> map_obj = {{"one", 1}, {"two", 2}};
		auto map_any = dyn_ref::make_ref(map_obj);

		auto containerInfo = dyn_ref::MemberContainer::Create<std::map<std::string, int>>("test_map");

		std::cout << "Initial map size: " << dyn_ref::container_ops::Size(containerInfo, map_any) << "\n";

		// Insert key-value pair
		auto key_any = dyn_ref::make_copy(std::string("three"));
		auto value_any = dyn_ref::make_copy(3);
		bool success = dyn_ref::container_ops::InsertKV(containerInfo, map_any, key_any, value_any);
		std::cout << "Insert ('three', 3): " << (success ? "success" : "failed") << "\n";
		std::cout << "New size: " << dyn_ref::container_ops::Size(containerInfo, map_any) << "\n";

		// Check if key exists
		bool exists = dyn_ref::container_ops::ContainsKey(containerInfo, map_any, key_any);
		std::cout << "Contains key 'three': " << (exists ? "yes" : "no") << "\n";

		// Get value by key
		try {
			auto result_any = dyn_ref::container_ops::GetValue(containerInfo, map_any, key_any);
			auto* result_ptr = dyn_ref::any_cast<int>(result_any);
			std::cout << "Value for key 'three': " << (result_ptr ? *result_ptr : -1) << "\n";
		} catch (const std::exception& e) {
			std::cout << "GetValue failed: " << e.what() << "\n";
		}

		// Clear
		dyn_ref::container_ops::Clear(containerInfo, map_any);
		std::cout << "After clear, size: " << dyn_ref::container_ops::Size(containerInfo, map_any) << "\n";
	}
	std::cout << "\n";

	// Test 4: Working with Person's containers via reflection
	std::cout << "Test 4: Person Container Members\n";
	std::cout << "---------------------------------\n";
	{
		Person person("Alice", 30);
		person.friends = {"Bob", "Charlie"};

		const auto* personClass = dyn_ref::GetType("Person")->AsClass();
		if (personClass) {
			// Find the 'friends' container
			for (const auto& container : personClass->memberContainers_) {
				if (container.name_ == "friends") {
					std::cout << "Found 'friends' container\n";

					// Create Any wrapper for the friends vector
					auto friends_any = dyn_ref::make_ref(person.friends);

					std::cout << "Current friends count: "
					          << dyn_ref::container_ops::Size(container, friends_any) << "\n";

					// Add a new friend
					auto new_friend = dyn_ref::make_copy(std::string("Diana"));
					bool success = dyn_ref::container_ops::Push(container, friends_any, new_friend);
					std::cout << "Added Diana: " << (success ? "success" : "failed") << "\n";
					std::cout << "New friends count: "
					          << dyn_ref::container_ops::Size(container, friends_any) << "\n";

					// Print all friends
					std::cout << "Friends: ";
					for (const auto& f : person.friends) {
						std::cout << f << " ";
					}
					std::cout << "\n";

					break;
				}
			}
		}
	}
	std::cout << "\n";

	std::cout << "========== All Container Operations Tests Completed ==========\n";
}

void test_function_invoke() {
	namespace dyn_ref = my_reflect::dynamic_refl;
	std::cout << "\n========== Testing Member Function Invocation ==========\n\n";

	// Test 1: Invoke const member function (getName)
	std::cout << "Test 1: Invoke const member function getName()\n";
	std::cout << "------------------------------------------------\n";

	Person p1("Alice", 25);
	auto p1_any = dyn_ref::make_ref(p1);

	const auto* personClass = dyn_ref::GetType("Person")->AsClass();
	if (!personClass) {
		std::cout << "ERROR: Failed to get Person class\n";
		return;
	}

	// Find getName function
	const dyn_ref::MemberFunction* getName_func = nullptr;
	for (const auto& func : personClass->memberFunctions_) {
		if (func.name_ == "getName") {
			getName_func = &func;
			break;
		}
	}

	if (getName_func) {
		std::vector<dyn_ref::Any> args; // No arguments
		auto result = getName_func->Invoke(p1_any, args);

		if (const auto* name = dyn_ref::any_cast<std::string>(result)) {
			std::cout << "getName() returned: " << *name << "\n";
		}
	}
	std::cout << "\n";

	// Test 2: Invoke non-const member function (setName)
	std::cout << "Test 2: Invoke non-const member function setName()\n";
	std::cout << "---------------------------------------------------\n";

	const dyn_ref::MemberFunction* setName_func = nullptr;
	for (const auto& func : personClass->memberFunctions_) {
		if (func.name_ == "setName") {
			setName_func = &func;
			break;
		}
	}

	if (setName_func) {
		std::string newName = "Bob";
		std::vector<dyn_ref::Any> args;
		args.push_back(dyn_ref::make_ref(newName));

		setName_func->Invoke(p1_any, args);
		std::cout << "After setName(\"Bob\"), name is now: " << p1.getName() << "\n";
	}
	std::cout << "\n";

	// Test 3: Invoke function with return value (getAge)
	std::cout << "Test 3: Invoke function with return value getAge()\n";
	std::cout << "---------------------------------------------------\n";

	const dyn_ref::MemberFunction* getAge_func = nullptr;
	for (const auto& func : personClass->memberFunctions_) {
		if (func.name_ == "getAge") {
			getAge_func = &func;
			break;
		}
	}

	if (getAge_func) {
		std::vector<dyn_ref::Any> args;
		auto result = getAge_func->Invoke(p1_any, args);

		if (const auto* age = dyn_ref::any_cast<int>(result)) {
			std::cout << "getAge() returned: " << *age << "\n";
		}
	}
	std::cout << "\n";

	// Test 4: Invoke function with multiple parameters (speak)
	std::cout << "Test 4: Invoke function with multiple parameters speak()\n";
	std::cout << "---------------------------------------------------------\n";

	const dyn_ref::MemberFunction* speak_func = nullptr;
	for (const auto& func : personClass->memberFunctions_) {
		if (func.name_ == "speak") {
			speak_func = &func;
			break;
		}
	}

	if (speak_func) {
		std::string message = "Hello";
		int duration = 5;
		std::vector<dyn_ref::Any> args;
		args.push_back(dyn_ref::make_ref(message));
		args.push_back(dyn_ref::make_copy(duration));

		speak_func->Invoke(p1_any, args);
		std::cout << "speak(\"Hello\", 5) called successfully\n";
	}
	std::cout << "\n";

	std::cout << "========== All Function Invocation Tests Completed ==========\n";
}

void test_any_invoke() {
	namespace dyn_ref = my_reflect::dynamic_refl;
	std::cout << "\n========== Testing Any::invoke Method ==========\n\n";

	Person p1("Charlie", 30);
	auto p_any = dyn_ref::make_ref(p1);

	// Test 1: Invoke by name - no arguments
	std::cout << "Test 1: Invoke getName() by name\n";
	std::cout << "---------------------------------\n";

	try {
		auto result = p_any.invoke("getName");
		if (const auto* name = dyn_ref::any_cast<std::string>(result)) {
			std::cout << "p.invoke(\"getName\") returned: " << *name << "\n";
		}
	} catch (const std::exception& e) {
		std::cout << "ERROR: " << e.what() << "\n";
	}
	std::cout << "\n";

	// Test 2: Invoke by name - with argument
	std::cout << "Test 2: Invoke setName() by name with argument\n";
	std::cout << "-----------------------------------------------\n";

	try {
		std::string newName = "Diana";
		p_any.invoke("setName", newName);
		std::cout << "p.invoke(\"setName\", \"Diana\") called\n";
		std::cout << "New name: " << p1.getName() << "\n";
	} catch (const std::exception& e) {
		std::cout << "ERROR: " << e.what() << "\n";
	}
	std::cout << "\n";

	// Test 3: Invoke by index
	std::cout << "Test 3: Invoke getAge() by index\n";
	std::cout << "---------------------------------\n";

	try {
		// Need to know the index - getAge is at index 2
		// (0: getName, 1: setName, 2: getAge, 3: speak)
		auto result = p_any.invoke(2);
		if (const auto* age = dyn_ref::any_cast<int>(result)) {
			std::cout << "p.invoke(2) returned age: " << *age << "\n";
		}
	} catch (const std::exception& e) {
		std::cout << "ERROR: " << e.what() << "\n";
	}
	std::cout << "\n";

	// Test 4: Invoke with multiple arguments
	std::cout << "Test 4: Invoke speak() with multiple arguments\n";
	std::cout << "-----------------------------------------------\n";

	try {
		std::string msg = "Goodbye";
		int duration = 10;
		p_any.invoke("speak", msg, duration);
		std::cout << "p.invoke(\"speak\", \"Goodbye\", 10) called successfully\n";
	} catch (const std::exception& e) {
		std::cout << "ERROR: " << e.what() << "\n";
	}
	std::cout << "\n";

	// Test 5: Invoke with rvalue arguments (should use make_copy)
	std::cout << "Test 5: Invoke with rvalue arguments\n";
	std::cout << "-------------------------------------\n";

	try {
		p_any.invoke("speak", std::string("Temporary"), 3);
		std::cout << "p.invoke(\"speak\", std::string(\"Temporary\"), 3) called successfully\n";
	} catch (const std::exception& e) {
		std::cout << "ERROR: " << e.what() << "\n";
	}
	std::cout << "\n";

	// Test 6: Error handling - function not found
	std::cout << "Test 6: Error handling - function not found\n";
	std::cout << "--------------------------------------------\n";

	try {
		p_any.invoke("nonExistentFunction");
		std::cout << "ERROR: Should have thrown exception\n";
	} catch (const std::exception& e) {
		std::cout << "Expected error: " << e.what() << "\n";
	}
	std::cout << "\n";

	// Test 7: Error handling - invoke on non-class type
	std::cout << "Test 7: Error handling - invoke on non-class type\n";
	std::cout << "--------------------------------------------------\n";

	try {
		int value = 42;
		auto int_any = dyn_ref::make_copy(value);
		int_any.invoke("someMethod");
		std::cout << "ERROR: Should have thrown exception\n";
	} catch (const std::exception& e) {
		std::cout << "Expected error: " << e.what() << "\n";
	}
	std::cout << "\n";

	std::cout << "========== All Any::invoke Tests Completed ==========\n";
}

void test_static_variable_access() {
	std::cout << "\n========== Static Reflection Variable Access by Index Tests ==========\n\n";

	using PersonType = my_reflect::static_refl::TypeData<Person>;

	Person person("Alice", 25);

	// Test 1: Find variable index by name
	std::cout << "Test 1: Find variable index by name\n";
	std::cout << "------------------------------------\n";

	constexpr int name_idx = PersonType::find_variable_index("name");
	constexpr int age_idx = PersonType::find_variable_index("age");
	constexpr int not_exist_idx = PersonType::find_variable_index("notExist");

	std::cout << "Index of 'name': " << name_idx << "\n";
	std::cout << "Index of 'age': " << age_idx << "\n";
	std::cout << "Index of 'notExist': " << not_exist_idx << " (expected -1)\n";
	std::cout << "\n";

	// Test 2: Get variable by index
	std::cout << "Test 2: Get variable by index\n";
	std::cout << "------------------------------\n";

	if constexpr (name_idx >= 0) {
		const auto& name = PersonType::get<name_idx>(person);
		std::cout << "PersonType::get<" << name_idx << ">(person) = \"" << name << "\"\n";
	}

	if constexpr (age_idx >= 0) {
		const auto& age = PersonType::get<age_idx>(person);
		std::cout << "PersonType::get<" << age_idx << ">(person) = " << age << "\n";
	}
	std::cout << "\n";

	// Test 3: Set variable by index
	std::cout << "Test 3: Set variable by index\n";
	std::cout << "------------------------------\n";

	std::cout << "Before set: name = \"" << person.name << "\", age = " << person.age << "\n";

	if constexpr (name_idx >= 0) {
		PersonType::set<name_idx>(person, std::string("Bob"));
	}

	if constexpr (age_idx >= 0) {
		PersonType::set<age_idx>(person, 30);
	}

	std::cout << "After set:  name = \"" << person.name << "\", age = " << person.age << "\n";
	std::cout << "\n";

	// Test 4: Get from const instance
	std::cout << "Test 4: Get from const instance\n";
	std::cout << "--------------------------------\n";

	const Person constPerson("Charlie", 35);

	if constexpr (name_idx >= 0) {
		const auto& name = PersonType::get<name_idx>(constPerson);
		std::cout << "PersonType::get<" << name_idx << ">(constPerson) = \"" << name << "\"\n";
	}

	if constexpr (age_idx >= 0) {
		const auto& age = PersonType::get<age_idx>(constPerson);
		std::cout << "PersonType::get<" << age_idx << ">(constPerson) = " << age << "\n";
	}
	std::cout << "\n";

	// Test 5: Modify via reference returned from get
	std::cout << "Test 5: Modify via reference returned from get\n";
	std::cout << "-----------------------------------------------\n";

	std::cout << "Before modify: age = " << person.age << "\n";

	if constexpr (age_idx >= 0) {
		auto& age_ref = PersonType::get<age_idx>(person);
		age_ref = 40;
	}

	std::cout << "After modify:  age = " << person.age << "\n";
	std::cout << "\n";

	std::cout << "========== All Static Variable Access Tests Completed ==========\n";
}

int main() {
	test_static_reflection();
	test_dynamic_reflection();
	test_any();
	test_any_operations();
	test_function_invoke();
	test_any_invoke();
	test_container_operations();
	test_static_variable_access();
	return 0;
}
