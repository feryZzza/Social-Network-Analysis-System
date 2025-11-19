#ifndef LIN_LIST_H
#define LIN_LIST_H

#include <iostream>
#include <ostream>
#include <stdexcept>

//线性表抽象类
template <class T>
class LinearList {
public:
    LinearList() {};
    virtual ~LinearList() {}; // 虚析构函数，确保子类析构被调用
    virtual bool empty() = 0; 
    virtual bool full() = 0; 
    virtual int size() = 0; 
    virtual bool insert(int index, const T& x) = 0; 
    virtual bool add(const T& x) = 0; 
    virtual bool remove(int index) = 0; 
    virtual bool getx(int index, T& x) = 0; 
    virtual bool setx(int index, const T& x) = 0; 
};

//顺序表子类
template <class T>
class SeqList : public LinearList<T> {
public:
    SeqList(int size, int length_now = 0) : maxSize(size), length(length_now) {
        data = new T[maxSize];
    }
    
    ~SeqList() {
        delete[] data;
    }

    int search(const T& x) {
        for (int i = 0; i < length; i++) {
            if (data[i] == x) return i;
        }
        return -1; 
    }

    int search(const T* x) {
        for (int i = 0; i < length; i++) {
            if (data[i] == *x) return i;
        }
        return -1; 
    }


    SeqList(const SeqList<T>& other) : maxSize(other.maxSize), length(other.length) {
        data = new T[maxSize];
        for (int i = 0; i < length; i++) {
            data[i] = other.data[i];
        }
    }

    SeqList<T>& operator=(const SeqList<T>& other) {
        if (this == &other) return *this;
        delete[] data; // 释放旧内存
        
        maxSize = other.maxSize;
        length = other.length;
        data = new T[maxSize];
        for (int i = 0; i < length; i++) {
            data[i] = other.data[i];
        }
        return *this;
    }

    bool empty() override { return length == 0; }
    bool full() override { return length >= maxSize; }
    int size() override { return length; }

    bool insert(int index, const T& x) override {
        if(!index_safe(index) && index != 0) return false;
        if(full()) return false;
        for (int i = length; i > index; i--) {
            data[i] = data[i - 1];
        }
        data[index] = x;
        length++;
        return true;
    }

    bool add(const T& x) override {
        if(full()) return false;
        data[length] = x;
        length++;
        return true;
    }

    bool remove(int index) override {
        if(!index_safe(index)) return false;
        for(int i = index; i < length-1; i++) {
            data[i] = data[i+1];
        }
        length--;
        return true;
    }

    bool getx(int index, T& x) override {
        if(!index_safe(index)) return false;
        x = data[index];
        return true;
    }

    bool setx(int index, const T& x) override {
        if(!index_safe(index)) return false;
        data[index] = x;
        return true;
    }
    
    bool index_safe(int index) { return index >= 0 && index < length; }
    
    T& operator[](int index) {
        if (!index_safe(index)) {
            throw std::out_of_range("Index不在范围内");
        }
        return data[index];
    }

    friend std::ostream& operator<< (std::ostream& os, SeqList<T>& list) {
        os << "[";
        for (int i = 0; i < list.length; i++) {
            os << list.data[i];
            if (i != list.length - 1) os << " ";
        }
        os << "]";
        return os;
    }

private:
    T* data; 
    int maxSize; 
    int length; 
};

// 链式表节点类
template <class T>
class ListNode{
public:
    ListNode(): next(nullptr) {}
    T data; 
    ListNode* next; 
};

template <class T>
class LinkList : public LinearList<T> {

public:
    LinkList() : head(nullptr), length(0), tail(nullptr) {} 
    
    ~LinkList() { 
        clear();
    }

    // [已有] 拷贝构造函数
    LinkList(const LinkList& other) : head(nullptr), length(0), tail(nullptr) {
        ListNode<T>* current = other.head;
        while(current != nullptr) {
            this->add(current->data); 
            current = current->next;
        }
    }

    // [已有] 赋值运算符
    LinkList& operator=(const LinkList& other) { 
        if (this != &other) {
            clear();
            ListNode<T>* current = other.head;
            while(current != nullptr) {
                this->add(current->data);
                current = current->next;
            }
        }
        return *this;
    }

    // 辅助清除函数
    void clear() {
        ListNode<T>* current = head;
        while(current != nullptr) {
            ListNode<T>* temp = current;
            current = current->next;
            delete temp;
        }
        head = nullptr;
        tail = nullptr;
        length = 0;
    }

    bool insert(int index, const T& x) override {
        if(!index_safe(index) && index != 0) return false;
        ListNode<T>* newListNode = new ListNode<T>();
        newListNode->data = x;
        if(index == 0) {
            newListNode->next = head;
            head = newListNode;
            if(length == 0) tail = newListNode; 
        }else if(index < length){
            ListNode<T>* current = head;
            for(int i = 0; i < index - 1; i++) {
                current = current->next;
            }
            newListNode->next = current->next;
            current->next = newListNode;
        }else if(index == length){
            tail->next = newListNode;
            newListNode->next = nullptr;
            tail = newListNode;
        }
        length++;
        return true;
    }

    bool add(const T& x) override {
        ListNode<T>* newListNode = new ListNode<T>();
        newListNode->data = x;
        newListNode->next = nullptr;
        if(tail == nullptr) {
            head = newListNode;
            tail = newListNode;
        }else{
            tail->next = newListNode;
            tail = newListNode;
        }
        length++;
        return true;
    }

    bool remove(int index) override {
        if(!index_safe(index)) return false;
        ListNode<T>* toDelete;
        if(index == 0) {
            toDelete = head;
            head = head->next;
            if(head == nullptr) tail = nullptr; 
        }else{
            ListNode<T>* current = head;
            for(int i = 0; i < index - 1; i++) {
                current = current->next;
            }
            toDelete = current->next;
            current->next = toDelete->next;
            if(toDelete == tail) tail = current; 
        }
        delete toDelete;
        length--;
        return true;
    }

    ListNode<T>* fake_remove(int index) {
        if(!index_safe(index)) return nullptr;
        ListNode<T>* toDelete;
        if(index == 0) {
            toDelete = head;
            head = head->next;
            if(head == nullptr) tail = nullptr; 
        }else{
            ListNode<T>* current = head;
            for(int i = 0; i < index - 1; i++) {
                current = current->next;
            }
            toDelete = current->next;
            current->next = toDelete->next;
            if(toDelete == tail) tail = current; 
        }
        length--;
        return toDelete;
    }

    void auto_insert(ListNode<T>* node){
        if(head == nullptr || head->data >= node->data) {
            node->next = head;
            head = node;
            if(tail == nullptr) tail = node; 
        }else{
            ListNode<T>* current = head;
            while(current->next != nullptr && current->next->data < node->data) {
                current = current->next;
            }
            node->next = current->next;
            current->next = node;
            if(node->next == nullptr) tail = node; 
        }
        length++;

    }

    bool getx(int index, T& x) override {
        if(!index_safe(index)) return false;
        ListNode<T>* current = head;
        for(int i = 0; i < index; i++) {
            current = current->next;
        }
        x = current->data;
        return true;
    }

    bool setx(int index, const T& x) override {
        if(!index_safe(index)) return false;
        ListNode<T>* current = head;
        for(int i = 0; i < index; i++) {
            current = current->next;
        }
        current->data = x;
        return true;
    }

    bool empty() override {return length == 0;}
    bool full() override {return false;}
    int size() override {return length;}
    
    bool index_safe(int index){return index >= 0 && index < length;};
    
    T& operator[](int index) {
        if (!index_safe(index)) {
            throw std::out_of_range("Index不在范围内");
        }
        ListNode<T>* current = head;
        for(int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->data;
    }
    
    friend std::ostream& operator<< (std::ostream& os,LinkList<T>& list) {
        os << "[";
        ListNode<T>* current = list.head;
        while(current != nullptr) {
            os << current->data;
            if(current->next != nullptr) os << " ";
            current = current->next;
        }
        os << "]";
        return os;
    }
    
    ListNode<T>* tail_ptr() const {return tail;}

private:
    ListNode<T>* head; 
    ListNode<T>* tail; 
    int length; 
};

#endif