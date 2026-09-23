// RAII: tie a resource's lifetime to a scope so cleanup runs even on early return or exception.
#include <sk/unique_ptr.hpp>

#include <cstdio>
#include <iostream>
#include <stdexcept>

struct Entity {
    Entity() { std::cout << "  Entity created\n"; }
    ~Entity() { std::cout << "  Entity destroyed\n"; }
};

// A hand-written scope guard. It must not be copyable: two copies would
// both delete the same pointer (a double free). That is exactly the class
// of bug sk::UniquePtr prevents at compile time.
class ScopedPointer {
public:
    explicit ScopedPointer(Entity* entity) : entity_(entity) {}
    ScopedPointer(const ScopedPointer&) = delete;
    ScopedPointer& operator=(const ScopedPointer&) = delete;
    ~ScopedPointer() { delete entity_; }

private:
    Entity* entity_;
};

// A C resource wrapped with a custom deleter: fclose runs however we leave the scope.
struct FileCloser {
    void operator()(std::FILE* file) const noexcept { std::fclose(file); }
};
using File = sk::UniquePtr<std::FILE, FileCloser>;

void may_throw(bool fail) {
    const sk::UniquePtr<Entity> guard(new Entity());
    if (fail) {
        throw std::runtime_error("failure after acquiring the resource");
    }
}

int main() {
    std::cout << "Scope exit:\n";
    {
        const ScopedPointer scoped(new Entity());
    }

    std::cout << "Exception unwinding:\n";
    try {
        may_throw(true);
    } catch (const std::exception& e) {
        std::cout << "  caught: " << e.what() << '\n';
    }

    std::cout << "Custom deleter for a C handle:\n";
    if (File file(std::tmpfile()); file) {
        std::fputs("raii", file.get());
        std::cout << "  temporary file written; closed automatically\n";
    }
    return 0;
}
