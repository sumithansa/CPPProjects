// Idiomatic operator overloading for a value type.
//
// The earlier version implemented operator+ by modifying *this, so `a + b`
// silently changed `a`. The canonical pattern is: implement the compound
// operator (+=) as a member, then define the binary operator (+) as a
// non-member in terms of it, taking the left operand by value.
#include <iostream>

class Vector2 {
public:
    constexpr Vector2(float x, float y) : x_(x), y_(y) {}

    constexpr Vector2& operator+=(const Vector2& rhs) {
        x_ += rhs.x_;
        y_ += rhs.y_;
        return *this;
    }

    constexpr Vector2& operator*=(float scale) {
        x_ *= scale;
        y_ *= scale;
        return *this;
    }

    // Hidden friends: found only by argument-dependent lookup, and symmetric
    // in their operands so implicit conversions apply to both sides.
    friend constexpr Vector2 operator+(Vector2 lhs, const Vector2& rhs) { return lhs += rhs; }
    friend constexpr Vector2 operator*(Vector2 lhs, float scale) { return lhs *= scale; }
    friend constexpr Vector2 operator*(float scale, Vector2 rhs) { return rhs *= scale; }
    friend constexpr bool operator==(const Vector2&, const Vector2&) = default;

    friend std::ostream& operator<<(std::ostream& os, const Vector2& v) {
        return os << '(' << v.x_ << ", " << v.y_ << ')';
    }

private:
    float x_;
    float y_;
};

int main() {
    constexpr Vector2 position(4.0F, 4.0F);
    constexpr Vector2 speed(0.5F, 1.5F);

    constexpr Vector2 next = position + speed * 2.0F;
    static_assert(next == Vector2(5.0F, 7.0F)); // evaluated at compile time

    std::cout << "position " << position << " + 2 * speed " << speed << " = " << next << '\n';
    return 0;
}
