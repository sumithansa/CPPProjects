// Why single-argument constructors should usually be `explicit`.
#include <iostream>
#include <string>
#include <utility>

class Implicit {
public:
    Implicit(int age) : age_(age) {} // NOLINT(google-explicit-constructor): deliberately implicit
    int age() const { return age_; }

private:
    int age_;
};

class Explicit {
public:
    explicit Explicit(int age) : age_(age) {}
    explicit Explicit(std::string name) : name_(std::move(name)) {}
    int age() const { return age_; }
    const std::string& name() const { return name_; }

private:
    int age_ = -1;
    std::string name_ = "unknown";
};

void greet_implicit(const Implicit& person) {
    std::cout << "age " << person.age() << '\n';
}
void greet_explicit(const Explicit& person) {
    std::cout << "age " << person.age() << '\n';
}

int main() {
    greet_implicit(22); // compiles: 22 silently becomes an Implicit - easy to do by accident
    // greet_explicit(22);           // error: no implicit conversion from int
    greet_explicit(Explicit(22)); // the conversion is visible at the call site

    const Explicit named(std::string("Saumit Kumar"));
    std::cout << "name " << named.name() << '\n';
    return 0;
}
