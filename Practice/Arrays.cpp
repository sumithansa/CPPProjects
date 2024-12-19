#include<iostream>

int arraySum(int arr[], int size)
{
    int sum = 0;
    for (int i =0; i<size; i++)
    {
        sum += arr[i];
    }
    return sum;
}
int main()
{
    int arr[] = {1,2,3,-4,-5,6,7};
    for(int num:arr)
    {
        std::cout<<num<<std::endl;
    }
    std::cout<<"Sum is: "<<arraySum(arr, 7)<<std::endl;
}