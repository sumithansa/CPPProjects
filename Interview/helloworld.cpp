/*
    Stack:
        1. Push
        2. Pop
        3. Top
        4. Size
*/

#include<iostream>
struct Node
{
    int key;
    Node* next;
};

class stack
{
    Node *head;
    int size;
public:
    stack(int val)
    {
        size = 1;
        head = new Node;
        head->key = val;
        head->next = nullptr;
    }
    void push(int val)
    {
        size++;
        Node* curr = head;
        while(curr && curr->next)
        {
            curr = curr->next;
        }
        curr = new Node;
        curr->key = val;
        curr->next = nullptr;
    }
    int top() const
    {
        if(!head) // no head - case
            return 0;

        Node *curr = head;
        while(curr && curr->next)
        {
            curr = curr->next;
        }        
        return curr->key;
    }
    void pop()
    {
        Node* curr = head;
        if(!curr)
            return;
        while(curr->next)
        {
            curr = curr->next;
        }
        std::cout<<curr->key; // display value to pop
        --size;
        delete curr;
    }
    int Size() const
    {
        return size;
    }

    void clear()
    {
        Node* curr = head;
        Node* temp;
        while(curr)
        {
            temp = curr;
            if(curr->next)curr = curr->next;
            delete temp;
        }
        head = nullptr;
        size = 0;
    }

};


int sum(const int& a, const int& b)
{
    return a+b;
}

int sum(int a, int b, int c){}

int increment(int &a)
{
    return a++;
}

int main()
{

}