# C++ Reflection System

A modern C++ reflection library providing both static (compile-time) and dynamic (runtime) reflection capabilities.

## Features

- **Dual Reflection System**
  - **Static Reflection**: Zero-overhead compile-time reflection with full type safety
  - **Dynamic Reflection**: Runtime flexibility with type erasure via `Any` type

- **Comprehensive Support**
  - Member variables and functions
  - Containers (vector, map, set, etc.)
  - Enumerations
  - Arithmetic operations on reflected types

- **Modern C++17**
  - Template metaprogramming
  - Compile-time type lists
  - Index-based access with compile-time name lookup

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Static Reflection](#static-reflection)
3. [Dynamic Reflection](#dynamic-reflection)
4. [Any Type](#any-type)
5. [Utilities](#utilities)
6. [Complete Examples](#complete-examples)
7. [Performance Comparison](#performance-comparison)

---

## Quick Start

### Define a Class

```cpp
class Person {
public:
    std::string name;
    int age;
    std::vector<std::string> friends;

    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
    int getAge() const { return age; }
};
```

---

## Static Reflection

Static reflection provides **compile-time** type information with zero runtime overhead and full type safety.

### 1. Register Types

Use macros to register types for reflection:

```cpp
#include "static_refl/reflect_core.h"

BEGIN_REFLECT(Person)
BASE_CLASSES()  // If there are base classes: BASE_CLASSES(BaseClass1, BaseClass2)

functions(
    func(&Person::getName),
    func(&Person::setName),
    func(&Person::getAge)
)

variables(
    var(&Person::name),
    var(&Person::age)
)

containers(
    container(&Person::friends)
)

END_REFLECT()
```

### 2. Access Member Variables

**Method 1: Direct pointer access** (Recommended - zero overhead):

```cpp
using PersonType = my_reflect::static_refl::TypeData<Person>;

Person p{"Alice", 25};

// Get fields
constexpr auto person_vars = PersonType::variables;
auto name_field = std::get<0>(person_vars);  // name field
auto age_field = std::get<1>(person_vars);   // age field

// Read values
std::cout << p.*name_field.ptr_ << "\n";     // Alice
std::cout << p.*age_field.ptr_ << "\n";      // 25

// Modify values
p.*name_field.ptr_ = "Bob";
p.*age_field.ptr_ = 30;
```

**Method 2: Index-based access**:

```cpp
// Get by index (starting from 0)
const auto& name = PersonType::get<0>(p);    // get name
const auto& age = PersonType::get<1>(p);     // get age

// Set by index
PersonType::set<0>(p, std::string("Charlie"));
PersonType::set<1>(p, 35);

// Get reference and modify
auto& age_ref = PersonType::get<1>(p);
age_ref = 40;
```

**Method 3: Compile-time name lookup + index-based access**:

```cpp
// Compile-time variable index lookup
constexpr int name_idx = PersonType::find_variable_index("name");
constexpr int age_idx = PersonType::find_variable_index("age");

// Compile-time check if variable exists
if constexpr (name_idx >= 0) {
    const auto& name = PersonType::get<name_idx>(p);
    PersonType::set<name_idx>(p, std::string("David"));
}

// Non-existent variable returns -1
constexpr int notFound = PersonType::find_variable_index("nonExistent");  // -1
```

### 3. Call Member Functions

**Method 1: Direct call** (Recommended - zero overhead):

```cpp
constexpr auto person_funcs = PersonType::functions;
auto getName_field = std::get<0>(person_funcs);

// Call function
std::string name = (p.*getName_field.ptr_)();
```

**Method 2: Call by index**:

```cpp
// Call by index (starting from 0)
auto name = PersonType::invoke<0>(p);        // call getName
auto age = PersonType::invoke<2>(p);         // call getAge

PersonType::invoke<1>(p, "Charlie");         // call setName
```

**Method 3: Compile-time name lookup + call**:

```cpp
// Compile-time function index lookup
constexpr int getName_idx = PersonType::find_function_index("getName");
constexpr int setName_idx = PersonType::find_function_index("setName");

// Compile-time check if function exists
if constexpr (getName_idx >= 0) {
    auto result = PersonType::invoke<getName_idx>(p);
}

// Non-existent function returns -1
constexpr int notFound = PersonType::find_function_index("nonExistent");  // -1
```

### 4. Access Containers

```cpp
constexpr auto person_containers = PersonType::containers;
auto friends_field = std::get<0>(person_containers);

// Access container
(p.*friends_field.ptr_).push_back("David");
std::cout << (p.*friends_field.ptr_).size() << "\n";
```

### 5. Query Type Information

```cpp
using PersonType = my_reflect::static_refl::TypeData<Person>;

// Get type lists
using FunctionTypes = PersonType::function_types;   // function type list
using VariableTypes = PersonType::variable_types;   // variable type list
using ContainerTypes = PersonType::container_types; // container type list

// Get counts (compile-time constants)
constexpr size_t func_count = my_reflect::static_refl::size_v<FunctionTypes>;
constexpr size_t var_count = my_reflect::static_refl::size_v<VariableTypes>;

// Query field information
auto name_field = std::get<0>(PersonType::variables);
std::cout << name_field.name_ << "\n";           // field name
std::cout << name_field.is_variable() << "\n";   // is variable?
std::cout << name_field.is_member() << "\n";     // is member?
```

---

## Dynamic Reflection

Dynamic reflection provides **runtime** type information with type erasure and runtime queries.

### 1. Register Types

```cpp
#include "dynamic_refl/dynamic_reflect_core.h"

namespace dyn_ref = my_reflect::dynamic_refl;

// Register basic types (if needed)
dyn_ref::Register<std::string>().Register("std::string");
dyn_ref::Register<int>().Register("int");

// Register class
dyn_ref::Register<Person>()
    .Register("Person")
    .Add("getName", &Person::getName)
    .Add("setName", &Person::setName)
    .Add("getAge", &Person::getAge)
    .Add<decltype(&Person::name)>("name")
    .Add<decltype(&Person::age)>("age")
    .Add<decltype(&Person::friends)>("friends");
```

### 2. Runtime Type Query

```cpp
// Find type by name
const dyn_ref::Type* personType = dyn_ref::GetType("Person");

std::cout << personType->GetName() << "\n";      // Person
std::cout << (int)personType->GetKind() << "\n"; // type kind

// Cast to specific type
const dyn_ref::Class* personClass = personType->AsClass();
const dyn_ref::Enum* enumType = someType->AsEnum();
const dyn_ref::Arithmetic* arithType = someType->AsArithmetic();
```

### 3. Iterate Class Members

```cpp
const dyn_ref::Class* personClass = dyn_ref::GetType("Person")->AsClass();

// Iterate member functions
for (const auto& func : personClass->memberFunctions_) {
    std::cout << "Function: " << func.name_ << "\n";
}

// Iterate member variables
for (const auto& var : personClass->memberVariables_) {
    std::cout << "Variable: " << var.name_ << "\n";
}

// Iterate container members
for (const auto& container : personClass->memberContainers_) {
    std::cout << "Container: " << container.name_ << "\n";
}
```

### 4. Runtime Operations with Any

See next section [Any Type](#any-type).

---

## Any Type

`Any` is a type-erased wrapper that stores objects of any type while preserving reflection information.

### 1. Create Any Objects

```cpp
Person p{"Alice", 25};

// Four storage modes
auto any1 = dyn_ref::make_copy(p);   // Copy storage
auto any2 = dyn_ref::make_move(std::move(p)); // Move storage
auto any3 = dyn_ref::make_ref(p);    // Reference storage (non-owning)
auto any4 = dyn_ref::make_cref(p);   // Const reference storage
```

### 2. Extract Values from Any

```cpp
// any_cast returns pointer (nullptr on failure)
Person* ptr = dyn_ref::any_cast<Person>(any3);
if (ptr) {
    std::cout << ptr->name << "\n";
}

// any_get returns optional (nullopt on failure)
std::optional<int> value = dyn_ref::any_get<int>(int_any);
if (value) {
    std::cout << *value << "\n";
}
```

### 3. Operations on Any

#### Arithmetic Type Operations

```cpp
int x = 42;
auto x_any = dyn_ref::make_copy(x);

// Generic operations (Layer 1)
dyn_ref::any_set(x_any, 100);
auto result = dyn_ref::any_get<int>(x_any);  // 100

// Type-checked operations (Layer 2)
const dyn_ref::Arithmetic* arithType = x_any.typeInfo->AsArithmetic();
if (arithType) {
    arithType->SetValue(x_any, 200);
    auto value = arithType->GetValue<int>(x_any);
}
```

#### Enum Type Operations

```cpp
enum class Color { Red, Green, Blue };

// Register enum
dyn_ref::Register<Color>()
    .Register("Color")
    .Value("Red", Color::Red)
    .Value("Green", Color::Green)
    .Value("Blue", Color::Blue);

Color c = Color::Red;
auto c_any = dyn_ref::make_copy(c);

const dyn_ref::Enum* enumType = dyn_ref::GetType("Color")->AsEnum();

// Set by name
enumType->SetByName(c_any, "Green");

// Set by value
enumType->SetByValue(c_any, 2);  // Blue

// Get name
std::optional<std::string> name = enumType->GetName(c_any);  // "Blue"

// Get value
std::optional<long long> value = enumType->GetValue(c_any);  // 2
```

#### Container Operations

```cpp
std::vector<int> vec = {1, 2, 3};
auto vec_any = dyn_ref::make_ref(vec);

// Need MemberContainer info
auto containerInfo = dyn_ref::MemberContainer::Create<std::vector<int>>("vec");

// Get size
size_t sz = dyn_ref::container_ops::Size(containerInfo, vec_any);

// Add element
auto value_any = dyn_ref::make_copy(42);
dyn_ref::container_ops::Push(containerInfo, vec_any, value_any);

// Access element
auto elem = dyn_ref::container_ops::At(containerInfo, vec_any, 0);

// Clear
dyn_ref::container_ops::Clear(containerInfo, vec_any);
```

**Map Operations**:

```cpp
std::map<std::string, int> scores;
auto map_any = dyn_ref::make_ref(scores);
auto mapInfo = dyn_ref::MemberContainer::Create<std::map<std::string, int>>("scores");

// Insert key-value pair
auto key = dyn_ref::make_copy(std::string("Math"));
auto val = dyn_ref::make_copy(95);
dyn_ref::container_ops::InsertKV(mapInfo, map_any, key, val);

// Get value
auto result = dyn_ref::container_ops::GetValue(mapInfo, map_any, key);

// Check if key exists
bool exists = dyn_ref::container_ops::ContainsKey(mapInfo, map_any, key);
```

### 4. Call Functions via Any

**Method 1: Find via Class and call**:

```cpp
Person p{"Alice", 25};
auto p_any = dyn_ref::make_ref(p);

const dyn_ref::Class* personClass = dyn_ref::GetType("Person")->AsClass();

// Find function
const dyn_ref::MemberFunction* func = personClass->FindFunction("getName");

// Call function
std::vector<dyn_ref::Any> args;  // no arguments
auto result = func->Invoke(p_any, args);

// Call with arguments
const dyn_ref::MemberFunction* setNameFunc = personClass->FindFunction("setName");
std::string newName = "Bob";
std::vector<dyn_ref::Any> args2;
args2.push_back(dyn_ref::make_ref(newName));
setNameFunc->Invoke(p_any, args2);
```

**Method 2: Direct Any.invoke (Recommended)**:

```cpp
Person p{"Alice", 25};
auto p_any = dyn_ref::make_ref(p);

// Call by name
auto name = p_any.invoke("getName");
std::cout << *dyn_ref::any_cast<std::string>(name) << "\n";

// Call with arguments
std::string newName = "Charlie";
p_any.invoke("setName", newName);

// Call by index
auto age = p_any.invoke(2);  // call 2nd function

// Call with multiple arguments
p_any.invoke("speak", std::string("Hello"), 5);
```

---

## Utilities

### 1. type_list Operations

```cpp
using namespace my_reflect::static_refl;

using list1 = type_list<int, double, char>;

// Get size
constexpr size_t sz = size_v<list1>;  // 3

// Get elements
using first = front_t<list1>;   // int
using last = back_t<list1>;     // char
using second = get_t<list1, 1>; // double

// Add element
using list2 = push_t<std::string, list1>;  // type_list<std::string, int, double, char>

// Pop element
using list3 = pop_t<list1>;  // type_list<double, char>
```

### 2. Type Trait Queries

```cpp
// Field trait query
auto field = std::get<0>(PersonType::functions);
field.is_function();      // is function?
field.is_variable();      // is variable?
field.is_const();         // is const?
field.is_member();        // is member?
field.param_count();      // parameter count

// Container trait query
auto container_field = std::get<0>(PersonType::containers);
container_field.is_container();   // is container?
container_field.container_kind(); // container type: Set/Vector/Map
```

### 3. Function Trait Extraction

```cpp
// Function type traits
using GetNameFunc = decltype(&Person::getName);
using traits = my_reflect::static_refl::function_traits<GetNameFunc>;

using RetType = traits::return_type;          // const std::string&
using ParamTypes = traits::parameter_type;    // type_list<>
constexpr bool isConst = traits::is_const;    // true
constexpr bool isMember = traits::is_member;  // true
constexpr size_t paramCount = traits::parameter_count; // 0
```

### 4. Container Trait Extraction

```cpp
using FriendsType = std::vector<std::string>;
using container_traits = my_reflect::static_refl::container_traits<FriendsType>;

using value_type = container_traits::value_type;  // std::string
constexpr auto kind = container_traits::kind;     // ContainerKind::Vector

// Check if container
constexpr bool isContainer = my_reflect::static_refl::is_container_v<FriendsType>;
```

---

## Complete Examples

### Static Reflection Complete Example

```cpp
#include "static_refl/reflect_core.h"
#include <iostream>

class Person {
public:
    std::string name;
    int age;

    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
};

BEGIN_REFLECT(Person)
BASE_CLASSES()
functions(
    func(&Person::getName),
    func(&Person::setName)
)
variables(
    var(&Person::name),
    var(&Person::age)
)
END_REFLECT()

int main() {
    using PersonType = my_reflect::static_refl::TypeData<Person>;

    Person p{"Alice", 25};

    // Access variable (Method 1: direct pointer access)
    auto name_field = std::get<0>(PersonType::variables);
    std::cout << "Name: " << p.*name_field.ptr_ << "\n";

    // Access variable (Method 2: by index)
    const auto& name = PersonType::get<0>(p);
    std::cout << "get<0>: " << name << "\n";

    // Access variable (Method 3: compile-time lookup)
    constexpr int name_idx = PersonType::find_variable_index("name");
    if constexpr (name_idx >= 0) {
        PersonType::set<name_idx>(p, std::string("Bob"));
        std::cout << "After set: " << PersonType::get<name_idx>(p) << "\n";
    }

    // Call function (Method 1: direct)
    auto getName_field = std::get<0>(PersonType::functions);
    std::cout << "getName(): " << (p.*getName_field.ptr_)() << "\n";

    // Call function (Method 2: by index)
    std::cout << "invoke<0>: " << PersonType::invoke<0>(p) << "\n";

    // Call function (Method 3: compile-time lookup)
    constexpr int idx = PersonType::find_function_index("getName");
    if constexpr (idx >= 0) {
        std::cout << "By index: " << PersonType::invoke<idx>(p) << "\n";
    }

    return 0;
}
```

### Dynamic Reflection Complete Example

```cpp
#include "dynamic_refl/dynamic_reflect_core.h"
#include <iostream>

class Person {
public:
    std::string name;
    int age;

    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
};

int main() {
    namespace dyn_ref = my_reflect::dynamic_refl;

    // Register type
    dyn_ref::Register<Person>()
        .Register("Person")
        .Add("getName", &Person::getName)
        .Add("setName", &Person::setName);

    // Create object and Any
    Person p{"Alice", 25};
    auto p_any = dyn_ref::make_ref(p);

    // Call function via Any
    auto name = p_any.invoke("getName");
    std::cout << "getName(): "
              << *dyn_ref::any_cast<std::string>(name) << "\n";

    // Modify
    p_any.invoke("setName", std::string("Bob"));
    std::cout << "After setName: " << p.name << "\n";

    return 0;
}
```

---

## Performance Comparison

| Feature | Static Reflection | Dynamic Reflection |
|---------|------------------|-------------------|
| **Type Checking** | Compile-time | Runtime |
| **Function Call** | Zero-overhead (inline) | Virtual function overhead |
| **Name Lookup** | Compile-time | Runtime linear search |
| **Flexibility** | Requires concrete type | Full type erasure |
| **Use Cases** | Performance-critical paths | Plugin systems, serialization |

---

## FAQ

**Q: Can static reflection call functions by name?**

A: Yes, via `find_function_index` for compile-time index lookup, then `invoke<index>` to call. Pure runtime name-based calls are not supported due to different return types.

**Q: Can dynamic and static reflection be used together?**

A: Yes! They are independent systems that can be used simultaneously. Use static reflection for performance-critical code and dynamic reflection for flexible runtime operations.

**Q: How to choose between the four Any storage modes?**

A:
- `make_copy`: Need ownership
- `make_move`: Take ownership from rvalue
- `make_ref`: Temporary reference, object lifetime managed externally
- `make_cref`: Read-only reference

**Q: Why do container operations need MemberContainer parameter?**

A: Because container operations are type-erased, they need the function pointers stored in MemberContainer to perform actual operations.

---

## Requirements

- **C++ Standard**: C++17 or higher
  - Requires `if constexpr`
  - Requires `std::void_t`
  - Requires fold expressions
  - Requires `auto` template parameters

- **Compiler Support**:
  - GCC 7+
  - Clang 5+
  - MSVC 2019+

---

## Summary

- **Static Reflection**: Compile-time, zero-overhead, type-safe, suitable for performance-sensitive scenarios
- **Dynamic Reflection**: Runtime, flexible, supports type erasure, suitable for plugins, serialization, etc.
- **Any Type**: Type-erased container bridging static types and dynamic reflection
- Two reflection systems complement each other; choose based on actual needs

Happy reflecting! 🎉
