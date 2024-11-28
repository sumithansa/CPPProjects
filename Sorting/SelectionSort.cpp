#include<iostream>
#include<vector>

void selectionSort(std::vector<int>A)
{

    int i=0,j=0,k=0, temp;
    bool isAlreadySorted = true;
    for(i; i < A.size(); ++i)
    {
        j = i+1;
        k = i;
        while(j < A.size())
        {
            if(A[j] < A[k])
            { 
                isAlreadySorted = false;
                k = j;                
            }
            ++j;
        }
        temp = A[i];
        A[i] = A[k];
        A[k] = temp;
        if(isAlreadySorted)
        {
            std::cout<<"\nThis Array is already sorted\n";
            break;
        }
    }

    std::cout<<"\nSELECTION SORT\nPrinting Array:\n";
    for(auto a : A)
    {
        std::cout<<a<<"\n";
    }
}

int main()
{
    std::vector<int> A ={1,2,3,4,5,6};
    std::vector<int> B ={5,4,3,2,1,0};
    std::vector<int> C ={5,4,9,2,7,0};
    selectionSort(A);
    selectionSort(B);
    selectionSort(C);

    return 0;
}