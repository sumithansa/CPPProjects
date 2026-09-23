#include <sk/factory.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {

class Car {
public:
    explicit Car(std::string colour) : colour_(std::move(colour)) {}
    virtual ~Car() = default;
    virtual std::string model() const = 0;
    const std::string& colour() const { return colour_; }

private:
    std::string colour_;
};

class Safari final : public Car {
public:
    using Car::Car;
    std::string model() const override { return "Safari"; }
};

class Harrier final : public Car {
public:
    using Car::Car;
    std::string model() const override { return "Harrier"; }
};

using CarFactory = sk::Factory<Car, std::string, std::string>;

TEST(FactoryTest, CreatesRegisteredProducts) {
    CarFactory factory;
    ASSERT_TRUE(factory.register_type<Safari>("safari"));
    ASSERT_TRUE(factory.register_type<Harrier>("harrier"));

    auto car = factory.create("harrier", "red");
    ASSERT_NE(car, nullptr);
    EXPECT_EQ(car->model(), "Harrier");
    EXPECT_EQ(car->colour(), "red");
}

TEST(FactoryTest, UnknownKeyReturnsNull) {
    CarFactory factory;
    EXPECT_EQ(factory.create("nexon", "blue"), nullptr);
    EXPECT_FALSE(factory.contains("nexon"));
}

TEST(FactoryTest, DuplicateRegistrationIsRejected) {
    CarFactory factory;
    EXPECT_TRUE(factory.register_type<Safari>("suv"));
    EXPECT_FALSE(factory.register_type<Harrier>("suv"));
    EXPECT_EQ(factory.create("suv", "white")->model(), "Safari");
}

TEST(FactoryTest, AcceptsArbitraryCreatorFunctions) {
    CarFactory factory;
    factory.register_creator("always-black", [](const std::string&) -> std::unique_ptr<Car> {
        return std::make_unique<Safari>("black");
    });
    EXPECT_EQ(factory.create("always-black", "pink")->colour(), "black");
    EXPECT_EQ(factory.keys(), std::vector<std::string>{"always-black"});
}

} // namespace
