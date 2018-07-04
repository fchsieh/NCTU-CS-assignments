#include <iostream>
#include <cstdlib>
#include "Link_List.h"
using namespace std;
//#define DEBUG

template <typename T>
ostream &operator<<(ostream &out, const Link_List<T> &a)
{
    Int_Node<T> *node = a.head;
    if (a.size == 0)
        out << "List is empty.";
    while (node != NULL)
    {
        out << node->value << " ";
        node = node->next;
    }
    return out;
}

template <typename T>
istream &operator>>(istream &in, Link_List<T> &a)
{
    T value;
    in >> value;
    Int_Node<T> *node = a.head;
    for (int i = 0; i < a.size; i++)
        node = node->next;
    a.insert_node(value);
    return in;
}

template <typename T>
Link_List<T>::Link_List()
{
    size = 0;
    head = NULL;
    tail = NULL;
}

template <typename T>
Link_List<T>::Link_List(const Link_List<T> &listToCopy)
    : size(listToCopy.size)
{
    if (listToCopy.head == NULL)
        head = NULL;
    else
    {
        head = new Int_Node<T>;
        Int_Node<T> *current = head;
        Int_Node<T> *insert = listToCopy.head;
#ifdef DEBUG
        cout << "Insert value: " << insert->value << "\n";
#endif
        head->value = insert->value;
        head->next = NULL;
        head->pre = NULL;
        tail = head;
        int tmp_size = size - 1;
        while (tmp_size--)
        {
            insert = insert->next;
            Int_Node<T> *new_node = new Int_Node<T>;
            new_node->value = insert->value;
            new_node->next = NULL;
            new_node->pre = tail;
            tail->next = new_node;
            tail = new_node;
        }
    }
}

template <typename T>
Link_List<T>::~Link_List()
{
    reset();
}

template <typename T>
int Link_List<T>::getSize() const
{
    return size;
}

template <typename T>
void Link_List<T>::reset()
{
    while (size--)
        delete_node();
    head = NULL;
    tail = NULL;
    size = 0;
}

template <typename T>
const Link_List<T> &Link_List<T>::operator=(const Link_List<T> &listToCopy)
{
    reset();
    size = listToCopy.size;
    head = new Int_Node<T>;
    tail = new Int_Node<T>;
    head->value = listToCopy.head->value;
    head->next = NULL;
    head->pre = NULL;
    Int_Node<T> *current = head;
    Int_Node<T> *r_current = listToCopy.head;
    for (int i = 0; i < listToCopy.size - 1; i++)
    {
        current->next = new Int_Node<T>;
        current->next->pre = current;
        current = current->next;
        r_current = r_current->next;
        current->value = r_current->value;
    }
    tail = current;
    tail->next = NULL;
    return *this;
}

template <typename T>
bool Link_List<T>::operator==(const Link_List<T> &right) const
{
    if (size != right.size)
        return false;
    else
    {
        int tmp_size = size;
        Int_Node<T> *current = head;
        Int_Node<T> *r_current = right.head;
        while (tmp_size--)
        {
            if (current->value != r_current->value)
                return false;
            current = current->next;
            r_current = r_current->next;
        }
        return true;
    }
}

template <typename T>
T &Link_List<T>::operator[](int index)
{
    // index starts from 0
    if (index < 0 || index >= size)
    {
        cerr << "ERROR: index is illegal, exit program\n";
        exit(1);
    }
    Int_Node<T> *pos = head;
    while (index--)
        pos = pos->next;
    return pos->value;
}

template <typename T>
T Link_List<T>::operator[](int index) const
{
    // index starts from 0
    if (index < 0 || index >= size)
    {
        cerr << "ERROR: index is illegal, exit program\n";
        exit(1);
    }
    Int_Node<T> *pos = head;
    while (index--)
        pos = pos->next;
    return pos->value;
}

template <typename T>
bool Link_List<T>::insert_node(T value)
{
    Int_Node<T> *new_node = new Int_Node<T>;
    new_node->value = value;
    new_node->next = NULL;
    new_node->pre = NULL;
    // list is empty
    if (size == 0)
    {
        head = new_node;
        tail = head;
        size++;
        return true;
    }
    // insert at last
    else
    {
        Int_Node<T> *current = head;
        for (int i = 0; i < size - 1; i++)
            current = current->next;
#ifdef DEBUG
        cout << "current at: " << current->value << "\n";
#endif
        current->next = new_node;
        new_node->pre = current;
        tail = new_node;
        size++;
        return true;
    }
    // other situation, should not be here
    return false;
}

template <typename T>
bool Link_List<T>::insert_node(int index, T value)
{
    if (index > size)
        return false;
    // list is empty, insert at last
    // or insert at last
    if (size == 0 || index == size)
    {
        insert_node(value);
        return true;
    }
    else if (size > 0)
    {
        Int_Node<T> *new_node = new Int_Node<T>;
        new_node->value = value;
        new_node->pre = NULL;
        new_node->next = NULL;
        // insert at first
        if (index == 0)
        {
            new_node->next = head;
            head->pre = new_node;
            head = new_node;
            size++;
            return true;
        }
        // insert at middle
        else
        {
            Int_Node<T> *current = head;
            for (int i = 1; i < index; i++)
                current = current->next;
            current->next->pre = new_node;
            new_node->next = current->next;
            new_node->pre = current;
            current->next = new_node;
            size++;
            return true;
        }
    }
    // should not be in here
    else
        return false;
}

template <typename T>
bool Link_List<T>::delete_node()
{
    if (size <= 0)
    {
        return false;
    }
    else if (size == 1)
    {
        delete head;
        delete tail;
        head = NULL;
        tail = NULL;
    }
    else
    {
        Int_Node<T> *current = head;
        // get last node
        for (int i = 0; i < size - 1; i++)
            current = current->next;
        tail = current->pre;
        current->pre->next = NULL;
    }
    size--;
    return true;
}

template <typename T>
bool Link_List<T>::delete_node(int index)
{
    if (index >= size || index < 0 || size == 0)
        return false;
    // delete last node
    if (index == size - 1)
    {
        return delete_node();
    }
    else
    {
        // delete first node, size > 1
        if (index == 0)
        {
            head = head->next;
            head->pre = NULL;
        }
        // delete middle node
        else
        {
            Int_Node<T> *current = head;
            for (int i = 0; i < index; i++)
                current = current->next;
            current->pre->next = current->next;
            current->next->pre = current->pre;
            delete current;
        }
    }
    size--;
    return true;
}

// dealing with template problem...
template class Link_List<int>;
template class Link_List<char>;
template class Link_List<float>;
template ostream &operator<<(ostream &out, const Link_List<int> &a);
template ostream &operator<<(ostream &out, const Link_List<float> &a);
template ostream &operator<<(ostream &out, const Link_List<char> &a);
template istream &operator>>(istream &in, Link_List<int> &a);
template istream &operator>>(istream &in, Link_List<float> &a);
template istream &operator>>(istream &in, Link_List<char> &a);
