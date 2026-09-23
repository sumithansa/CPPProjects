// Counts the copies std::vector performs with and without reserve() + emplace_back().
// benchmarks/bench_vector.cpp measures the same effect in nanoseconds.
#include <iostream>
#include <vector>

struct Point {
    static inline int copies = 0;
    static inline int moves = 0;
    int x, y;

    Point(int px, int py) : x(px), y(py) {}
    Point(const Point& o) : x(o.x), y(o.y) { ++copies; }
    Point(Point&& o) noexcept : x(o.x), y(o.y) { ++moves; }
    Point& operator=(const Point&) = default;
    Point& operator=(Point&&) = default;
    ~Point() = default;

    static void reset() { copies = moves = 0; }
};

int main() {
    constexpr int kCount = 1000;

    Point::reset();
    std::vector<Point> naive;
    for (int i = 0; i < kCount; ++i) {
        naive.push_back(Point(i, i)); // NOLINT(modernize-use-emplace): the point of the demo
    }
    std::cout << "push_back, no reserve : " << Point::copies << " copies, " << Point::moves
              << " moves\n";

    Point::reset();
    std::vector<Point> tuned;
    tuned.reserve(kCount); // one allocation, no reallocation moves
    for (int i = 0; i < kCount; ++i) {
        tuned.emplace_back(i, i); // constructed in place from the arguments
    }
    std::cout << "reserve + emplace_back: " << Point::copies << " copies, " << Point::moves
              << " moves\n";
    return 0;
}
