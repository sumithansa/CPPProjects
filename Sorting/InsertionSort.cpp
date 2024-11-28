#include<iostream>
#include<vector>

void insertionSort(std::vector <int> A)
{
    int i=0, j, temp;
    bool isAlreadySorted = true;
    for (i;i<A.size()-1;++i)
    {
        j=i+1;
        while((j > 0) && (A[j] < A[j-1]))
        {
            isAlreadySorted =  false;
            temp = A[j];
            A[j] = A[j-1];
            --j;
            A[j] = temp;
        }
        if(isAlreadySorted)
        {
            std::cout<<"\nThis Array is already sorted\n";
            break;
        }
        
    }

    std::cout<<"\nINSERTION SORT\nPrinting Array:\n";
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
    insertionSort(A);
    insertionSort(B);
    insertionSort(C);
    return 0;
}