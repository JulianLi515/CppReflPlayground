#include <iostream>
#include <string_view>
#include <tuple>
#include "../include/static_refl/reflect_core.h"

struct Field {
    std::string_view name_;

    constexpr Field(std::string_view name) : name_(name) {
        // This code executes at compile-time!
    }
};

// Compile-time created objects
static constexpr auto fields = std::make_tuple(
    Field{"age"},
    Field{"name"},
    Field{"address"}
);

// Runtime created objects
static auto runtime_fields = std::make_tuple(
    Field{"runtime_age"},
    Field{"runtime_name"}
);

// ============================================
// Real reflection system example
// ============================================

class Person {
public:
    std::string name;
    int age;
    double salary;

    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
    int getAge() const { return age; }
    void setAge(int a) { age = a; }
};

// Register Person for reflection
BEGIN_REFLECT(Person)
BASE_CLASSES()
functions(
    func(&Person::getName),
    func(&Person::setName),
    func(&Person::getAge),
    func(&Person::setAge)
)
variables(
    var(&Person::name),
    var(&Person::age),
    var(&Person::salary)
)
END_REFLECT()

int main() {
    std::cout << "=== Memory Address Analysis ===\n\n";

    // Print compile-time object addresses
    std::cout << "Compile-time objects (constexpr):\n";
    std::cout << "  fields object address: " << &fields << "\n";
    std::cout << "  first element name_ points to: " << (void*)std::get<0>(fields).name_.data() << "\n";
    std::cout << "  first element name_ content: " << std::get<0>(fields).name_ << "\n";
    std::cout << "\n";

    // Print runtime object addresses
    std::cout << "Runtime objects:\n";
    std::cout << "  runtime_fields object address: " << &runtime_fields << "\n";
    std::cout << "  first element name_ points to: " << (void*)std::get<0>(runtime_fields).name_.data() << "\n";
    std::cout << "  first element name_ content: " << std::get<0>(runtime_fields).name_ << "\n";
    std::cout << "\n";

    // Print string literal addresses
    const char* literal1 = "age";
    const char* literal2 = "name";
    std::cout << "String literals:\n";
    std::cout << "  \"age\" address: " << (void*)literal1 << "\n";
    std::cout << "  \"name\" address: " << (void*)literal2 << "\n";
    std::cout << "\n";

    // Print stack object
    Field stack_field{"stack_name"};
    std::cout << "Stack objects:\n";
    std::cout << "  stack_field address: " << &stack_field << "\n";
    std::cout << "  stack_field.name_ points to: " << (void*)stack_field.name_.data() << "\n";
    std::cout << "\n";

    // Analyze memory regions
    std::cout << "=== Memory Region Analysis ===\n";
    std::cout << "Note: Lower addresses are typically in low-address segments (e.g. .rodata)\n";
    std::cout << "      Higher addresses are typically in high-address segments (e.g. stack)\n";
    std::cout << "\n";

    // ============================================
    // Reflection System Example
    // ============================================
    std::cout << "=== Reflection System - Compile-Time Name Lookup ===\n\n";

    using PersonType = my_reflect::static_refl::TypeData<Person>;

    Person person{"Alice", 30, 75000.0};

    // Compile-time variable index lookup
    std::cout << "Variable Lookup:\n";
    constexpr int name_idx = PersonType::find_variable_index("name");
    constexpr int age_idx = PersonType::find_variable_index("age");
    constexpr int salary_idx = PersonType::find_variable_index("salary");
    constexpr int not_exist_idx = PersonType::find_variable_index("notExist");

    std::cout << "  find_variable_index(\"name\") = " << name_idx << "\n";
    std::cout << "  find_variable_index(\"age\") = " << age_idx << "\n";
    std::cout << "  find_variable_index(\"salary\") = " << salary_idx << "\n";
    std::cout << "  find_variable_index(\"notExist\") = " << not_exist_idx << " (not found)\n";
    std::cout << "\n";

    // Access variables by compile-time index
    std::cout << "Access variables by index:\n";
    if constexpr (name_idx >= 0) {
        std::cout << "  PersonType::get<" << name_idx << ">(person) = \""
                  << PersonType::get<name_idx>(person) << "\"\n";
    }
    if constexpr (age_idx >= 0) {
        std::cout << "  PersonType::get<" << age_idx << ">(person) = "
                  << PersonType::get<age_idx>(person) << "\n";
    }
    if constexpr (salary_idx >= 0) {
        std::cout << "  PersonType::get<" << salary_idx << ">(person) = "
                  << PersonType::get<salary_idx>(person) << "\n";
    }
    std::cout << "\n";

    // Modify variables
    std::cout << "Modify variables by index:\n";
    if constexpr (name_idx >= 0) {
        PersonType::set<name_idx>(person, std::string("Bob"));
        std::cout << "  After set<" << name_idx << ">(person, \"Bob\"): name = \""
                  << person.name << "\"\n";
    }
    if constexpr (age_idx >= 0) {
        PersonType::set<age_idx>(person, 35);
        std::cout << "  After set<" << age_idx << ">(person, 35): age = "
                  << person.age << "\n";
    }
    std::cout << "\n";

    // Compile-time function index lookup
    std::cout << "Function Lookup:\n";
    constexpr int getName_idx = PersonType::find_function_index("getName");
    constexpr int setName_idx = PersonType::find_function_index("setName");
    constexpr int getAge_idx = PersonType::find_function_index("getAge");
    constexpr int setAge_idx = PersonType::find_function_index("setAge");

    std::cout << "  find_function_index(\"getName\") = " << getName_idx << "\n";
    std::cout << "  find_function_index(\"setName\") = " << setName_idx << "\n";
    std::cout << "  find_function_index(\"getAge\") = " << getAge_idx << "\n";
    std::cout << "  find_function_index(\"setAge\") = " << setAge_idx << "\n";
    std::cout << "\n";

    // Call functions by compile-time index
    std::cout << "Call functions by index:\n";
    if constexpr (getName_idx >= 0) {
        auto result = PersonType::invoke<getName_idx>(person);
        std::cout << "  PersonType::invoke<" << getName_idx << ">(person) = \""
                  << result << "\"\n";
    }
    if constexpr (getAge_idx >= 0) {
        auto result = PersonType::invoke<getAge_idx>(person);
        std::cout << "  PersonType::invoke<" << getAge_idx << ">(person) = "
                  << result << "\n";
    }
    std::cout << "\n";

    // Show where the reflection data is stored
    std::cout << "Reflection metadata addresses:\n";
    std::cout << "  PersonType::variables tuple address: " << &PersonType::variables << "\n";
    std::cout << "  PersonType::functions tuple address: " << &PersonType::functions << "\n";
    std::cout << "  First variable name_ points to: "
              << (void*)std::get<0>(PersonType::variables).name_.data() << "\n";
    std::cout << "  First function name_ points to: "
              << (void*)std::get<0>(PersonType::functions).name_.data() << "\n";
    std::cout << "\n";

    std::cout << "Note: All lookup happens at COMPILE-TIME!\n";
    std::cout << "      The indices (0, 1, 2) are compile-time constants.\n";
    std::cout << "      The reflection metadata is stored in .rdata segment.\n";

    return 0;
}
