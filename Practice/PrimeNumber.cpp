#include<iostream>
#include<vector>

bool isPrime(int number)
{
    for (auto i = 2; i*i < number + 1; i++) // iterate only to square root of the number
    {
        if(number % i == 0)
            return false;
    }
    return true;
}

int main()
{
    int numbers[] = {4, 7, 13, 42, 91, 17};
    std::vector<int> primes;
    for(auto& num: numbers)
    {
        if(isPrime((num)))
            primes.emplace_back(num);
    }

    for(auto& elem: primes)
    {
        std::cout<<elem<<"\t";
    }
    std::cout<<std::endl;
    return 0;
}