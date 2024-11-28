#include<iostream>
#include<vector>
void BubbleSort(std::vector<int> A)
{
    int i = 0, j = 0, temp = 0;
    bool isAlreadySorted = true;;
    for (i=0; i<A.size()-1; ++i)
    {
        for(j=0;j<A.size()-i-1;++j)
        {
            if(A[j] > A[j+1])
            {
                isAlreadySorted = false;
                temp = A[j];
                A[j] =A[j+1];
                A[j+1] = temp;
            }
        }
        if(isAlreadySorted)
        {
            std::cout<<"\nThis Array is already sorted\n";
            break;
        }
    }
    std::cout<<"\nINSERTION SORT\nPrinting Array\n";
    for(auto a : A)
    {
        std::cout<<a<<"\n";
    }

}

int main()
{
    std::vector<int> A={1,2,3,4,5,6,7,8};
    std::vector<int> B={8,7,6,5,4,3,2,1};    
    std::vector<int> C={8,7,1,2,3,4,6,5};
    BubbleSort(A);
    BubbleSort(B);
    BubbleSort(C);
    std::cout<<"\n";
    return 0;
}