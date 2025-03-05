#include"customString.cpp"
//#include"gtest/gtest.h" // Assuming this to be the path of google tests

TEST(customStringTest, SizeTest) 
{
    customString c1("Hello");
    ASSERT_EQ(c1.length(), 5);
    
    customString c2;
    ASSERT_EQ(c2.length(), 0);
}

TEST(customStringTest, InitTest) 
{
    customString c1;
    c1.init("Hello");
    ASSERT_EQ(c1.length(), 5);
}

TEST(customStringTest, FreeTest) 
{
    customString c1;
    c1.init("Hello");
    c1.free();
    ASSERT_EQ(c1.length(), 0);
}

TEST(customStringTest, AssigmentTest) 
{
    customString c1, c2;
    c1.init("Hello");
    c2 = c1;
    ASSERT_EQ(c1.length(), c2.length());
    ASSERT_STREQ(c1.m_string, c2.m_string);
}

TEST(customStringTest, BeginIteratorTest) 
{
    customString c1;
    c1.init("Hello");
    char c = *(c1.begin());
    ASSERT_EQ(c, c1[0]);
}

TEST(customStringTest, EndIteratorTest) 
{
    customString c1;
    c1.init("Hello");
    char c = *(c1.end());
    ASSERT_EQ(c, '\0');
}

TEST(customStringTest, SubStringTest) 
{
    customString c1, c2;
    c1.init("Hello, World!");
    c2 = c1.substr(7, 5);
    ASSERT_EQ(c2.m_string, "World");
}

TEST(customStringTest, concatTest) 
{
    customString c1, c2, c3;
    c1.init("Hello");
    c2.init("world!");
    c3=concat_new(c1, c2);
    c1.concat(c2);
    ASSERT_EQ(c1.m_string, c3.m_string);
}

TEST(customStringTest, compareTest) 
{
    customString c1, c2, c3;
    c1.init("Hello");
    c2.init("world!");
    c3=c2;
    ASSERT_NE(c1.m_string, c2.m_string);
    ASSERT_EQ(c3.m_string, c2.m_string);
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}