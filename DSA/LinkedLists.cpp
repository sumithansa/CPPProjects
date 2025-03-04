/*
Looks like a set of unordered map
LinkedList Data structure- 
1. Node class - value, next pointer
2. LinkedList - Node* head, Node* tail, length
              - Append(val), Prepend(val), insert(index, val)
              - Delete_Last(), Delete_First(), Delete_Node(index)
              - Get(), Set(), Reverse(), print_linked_list()

3. Big O: 
              - O(1): append(val), prepend(val) & delete_first()
              - O(n): insert(index, val), delete_last() & delete_node(index), print_linked_list()

*/
#include<iostream>
struct Node
{
    int value;
    Node* next;
    Node(int val): value(val), next(nullptr){}
};

class LinkedList
{
    Node* head, *tail;
    int length;
    public:
        LinkedList():head(nullptr), tail(nullptr), length(0){}  // Default cnstructor
        LinkedList(int value)                                   // Parametrized constructor
        {
            std::cout<<"New node constructed: ";
            head = new Node(value);
            tail = head;
            length = 1;
        }

        ~LinkedList()
        {
            Node* temp;
            std::cout<<"Destructed Node: ";
            while(head)
            {
                std::cout<<"\t["<<head->value<<"]";
                temp = head;
                head = head->next;
                delete temp;
            }
        }
        /*
        append(int val): adds new node at the end of the LinkedList, if LinkedList is empty construct new node and return 
        */
        void append(int val)
        {            
            Node* new_node = new Node(val);
            if(!head)
            {
                head = tail = new_node;
                length = 1;
                return;
            }
            tail->next = new_node;
            tail = new_node;
            ++length;
        }

        /*
        Using 3 pointers to reverse pointers, 
                    if no curr->next, make curr the head node
                    if prev == head, make it the tail node
        */
        void reverse()
        {
            std::cout<<"Called reverse(): ";
            if(!head)
                return;
            
            Node *prev, *curr, *next;
            prev = head;
            curr = prev->next;
            while(curr)
            {
                next = curr->next;
                if(!curr->next)
                    head = curr;
                curr->next = prev;
                if(prev == head)
                {
                    prev->next = nullptr;
                    tail = prev;
                }
                prev = curr;
                curr = next;
            }
        }

        int get_length() const
        {
            return length;
        }

        void print_linked_list()
        {
            Node* temp = head;
            while(temp)
            {
                std::cout<<"\t["<<temp->value<<"]";
                temp = temp->next;
            }
            std::cout<<std::endl;
        }

        Node* get_middle_node()
        {
            if(!head)
                return nullptr;
            if(head == tail)
                return head;
            
            Node* fast, *slow;
            fast = slow = head;
            while((fast != tail) && (fast))
            {
                slow = slow->next;
                fast = fast->next->next;
            }

            return slow;
        }

};

// client of LinkedList
int main()
{
    LinkedList ll(10);
    ll.append(2);
    ll.append(4);
    ll.append(3);
    ll.append(13);
    ll.append(17);
    ll.append(19);
    ll.append(11);
    ll.append(25);
    ll.append(23);
    ll.print_linked_list();
    ll.reverse();
    ll.print_linked_list();

    std::cout<<"Middle Node: "<<ll.get_middle_node()->value<<std::endl;

    //int* on[10];
}