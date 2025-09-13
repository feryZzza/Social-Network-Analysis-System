#ifndef LIN_LIST_H
#define LIN_LIST_H

#include <iostream>
#include <iostream>
#include <stdexcept>

//线性表抽象类，暂时不加入重载
template <class T>
class LinearList {
public:
    LinearList() {};
    ~LinearList() {};
    // 基础功能
    virtual bool empty() = 0; // 判断线性表是否为空
    virtual bool full() = 0; // 判断线性表是否为满
    virtual int size() = 0; // 获取线性表的当前长度
    virtual bool insert(int index, const T& elem) = 0; // 插入元素
    virtual bool add(const T& elem) = 0; // 添加元素到末尾
    virtual bool remove(int index) = 0; // 删除元素
    virtual int locate(const T& elem) = 0; // 按值查找元素
    virtual bool getElem(int index, T& elem) = 0; // 按位查找元素
    virtual bool setElem(int index, const T& elem) = 0; // 按位修改元素
};

//顺序表子类
template <class T>
class SeqList : public LinearList<T> {
public:
    SeqList(int size,int length_now = 0):maxSize(size),length(length_now){//输入为最大大小
        data = new T[maxSize];
    }
    ~SeqList() {
        delete[] data;
    }

    bool empty() override {return length == 0;};//返回是否为空
    bool full() override {return length >= maxSize;};//返回是否为满
    int size() override {return length;};//返回当前长度

    bool insert(int index, const T& elem) override {//插入元素
        if(!index_safe(index)&&index!=0) return false;
        if(full()) return false;
        //安全检查通过后，进行插入
        for (int i = length; i > index; i--){//后移
            data[i] = data[i - 1];
        }
        data[index] = elem;
        length++;
        return true;
    }

    bool add(const T& elem) override {//添加元素到末尾
        if(full()) return false;
        data[length] = elem;
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

    int locate(const T& elem) override {//按值查找元素
        for (int i = 0; i < length; i++) {
            if(data[i]==elem)return i;
        }
        return -1; // 未找到
    }

    bool getElem(int index, T& elem) override {//按位查找元素
        if(!index_safe(index)) return false;
        elem = data[index];
        return true;
    }

    bool setElem(int index, const T& elem) override {//按位修改元素
        if(!index_safe(index)) return false;
        data[index] = elem;
        return true;
    }
    
    bool index_safe(int index){return index >= 0 && index < length;};
    
    //重载[],可以直接使用list[i]访问元素，使访问修改更方便
    T& operator[](int index) {
        if (!index_safe(index)) {
            throw std::out_of_range("Index不在范围内");
        }
        return data[index];
    }

private:
    T* data; // 存储空间基址
    int maxSize; // 存储空间的最大容量
    int length; // 当前长度
};

// 链式表节点类
template <class T>
class Node{
public:
    T data; // 数据域
    Node* next; // 指针
};

template <class T>
class LinkList : public LinearList<T> {

public:
    LinkList() : head(nullptr), length(0),tail(nullptr) {} // 构造函数
    ~LinkList() { //析构函数,递归删除所有节点
        Node<T>* current = head;
        while(current!=nullptr) {
            Node<T>* temp = current;
            current = current->next;
            delete temp;
        }
    }
    bool insert(int index, const T& elem) override {//插入元素
        if(!index_safe(index)&&index!=0) return false;
        Node<T>* newNode = new Node<T>();
        newNode->data = elem;
        if(index == 0) {//插入头节点
            newNode->next = head;
            head = newNode;
            tail = newNode;
        }else if(index < length){//插入中间节点
            Node<T>* current = head;
            for(int i = 0; i < index - 1; i++) {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }else if(index == length){//插入尾节点
            tail->next = newNode;
            newNode->next = nullptr;
            tail = newNode;
        }
        length++;
        return true;
    }

    bool add(const T& elem) override {//添加元素到末尾
        Node<T>* newNode = new Node<T>();
        newNode->data = elem;
        newNode->next = nullptr;
        if(tail == nullptr) {//空链表
            head = newNode;
            tail = newNode;
        }else{
            tail->next = newNode;
            tail = newNode;
        }
        length++;
        return true;
    }

    bool remove(int index) override {
        if(!index_safe(index)) return false;
        Node<T>* toDelete;
        if(index == 0) {//删除头节点
            toDelete = head;
            head = head->next;
            if(head == nullptr) tail = nullptr; // 如果链表变空，更新tail
        }else{
            Node<T>* current = head;
            for(int i = 0; i < index - 1; i++) {
                current = current->next;
            }
            toDelete = current->next;
            current->next = toDelete->next;
            if(toDelete == tail) tail = current; // 如果删除的是尾节点，更新tail
        }
        delete toDelete;
        length--;
        return true;
    }

    int locate(const T& elem) override {//按值查找元素
        Node<T>* current = head;
        int index = 0;
        while(current != nullptr) {
            if(current->data == elem) return index;
            current = current->next;
            index++;
        }
        return -1; // 未找到
    }

    bool getElem(int index, T& elem) override {//按位查找元素
        if(!index_safe(index)) return false;
        Node<T>* current = head;
        for(int i = 0; i < index; i++) {
            current = current->next;
        }
        elem = current->data;
        return true;
    }

    bool setElem(int index, const T& elem) override {//按位修改元素
        if(!index_safe(index)) return false;
        Node<T>* current = head;
        for(int i = 0; i < index; i++) {
            current = current->next;
        }
        current->data = elem;
        return true;
    }

    bool empty() override {return length == 0;};//返回是否为空
    bool full() override {return false;};//链表不会满
    int size() override {return length;};//返回当前长度
    
    bool index_safe(int index){return index >= 0 && index < length;};
    //重载[],可以直接使用list[i]访问元素，使访问修改更方便
    T& operator[](int index) {
        if (!index_safe(index)) {
            throw std::out_of_range("Index不在范围内");
        }
        Node<T>* current = head;
        for(int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->data;
    }

private:
    Node<T>* head; // 头指针
    Node<T>* tail; // 尾指针,便于添加元素
    int length; // 当前长度
};




#endif