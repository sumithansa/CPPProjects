//You are given two non-empty linked lists representing two non-negative integers. 
//The digits are stored in reverse order, and each of their nodes contains a single digit.
//Add the two numbers and return the sum as a linked list.
//You may assume the two numbers do not contain any leading zero, except the number 0 itself.

#include<iostream>
#include<math.h>
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

int getNumber(ListNode* l)
{
    int i=0, num=0, number=0;
    while (l)
    {
        num = l->val;
        num = num * (int)(pow(10, i)+ 0.5);
        i++;
        number += num;
        l = l->next;
    }
    return number;
}

void generateNode(int number, ListNode* l)
{
    while(l)
    {
        if(number > 0)
        {
            l->val = number%10;
            number = number - l->val;
            if((number / 10)>0)
            {
                number = number / 10;
                l->next = new ListNode();
            }
            else
            {
                l->next = nullptr;
            }
            
            l = l->next;            
        }
    }
}

ListNode* addTwoNumbers(ListNode* l1, ListNode* l2)
{
    int final_number = getNumber(l1) + getNumber(l2);
    ListNode* finalNode = new ListNode();
    generateNode(final_number, finalNode);
    return finalNode;
}

int main()
{
ListNode* l1; ListNode* l2;
ListNode* temp = new ListNode(9);
l1 = temp;
temp->next = new ListNode(9);
temp = temp->next;
temp->next = new ListNode(9);
temp = temp->next;
temp->next = new ListNode(9);
temp = temp->next;
temp->next = nullptr;
/*while(l1)
{
    std::cout<<l1->val<<"\t";
    l1 = l1->next;
}*/

ListNode* temp1 = new ListNode(9);
l2 = temp1;
temp1->next = new ListNode(9);
temp1 = temp1->next;
temp1->next = new ListNode(9);
temp1 = temp1->next;
temp1->next = new ListNode(9);
temp1 = temp1->next;
temp1->next = new ListNode(9);
temp1 = temp1->next;
temp1->next = new ListNode(9);
temp1 = temp1->next;
temp1->next = new ListNode(9);
temp1 = temp1->next;
temp1->next = nullptr;

std::cout<<std::endl;
/*while(l2)
{
    std::cout<<l2->val<<"\t";
    l2 = l2->next;
}*/

ListNode* finalNode = addTwoNumbers(l1, l2);

std::cout<<"\n OutPut: "<<std::endl;
while(finalNode)
{
    std::cout<<finalNode->val<<"\t";
    finalNode = finalNode->next;
}
char ch;
std::cin>>ch;
return 0;
}