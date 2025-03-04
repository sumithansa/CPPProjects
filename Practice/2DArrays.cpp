#include<iostream>

int main()
{
    // 1D array
    int *ptr;
    int arr_1[4] = {20, 30, 40, 50};
    ptr = arr_1;    
    std::cout<<*ptr++<<std::endl; // Prints 20; post increment is performed after * is evaluted 
    std::cout<<*(ptr++)<<std::endl; // Prints 30; same as above 
    std::cout<<*++ptr<<std::endl<<std::endl; // Prints 50; first pre increment sets to next pointer then derefrencing it.

    // 2D arrays
    int** a2d = new int*[4]; // setting rows - most imprtant thing

    for (int i =0; i<4; i++)
    {
        a2d[i] = new int[3]; // setting columns
    }

    for(int i =0; i <3; i++)
    {
        for(int j =0; j <4; j++)
        {
            a2d[i][j] = i*j;    // assignng values to each one
        }
    }

    for(int i =0; i<3; i++)
    {
        for(int j =0; j<4; j++)
        {
            std::cout<<a2d[i][j]<<std::endl; // printing values of 2D array
        }
    }
    return 0;
}