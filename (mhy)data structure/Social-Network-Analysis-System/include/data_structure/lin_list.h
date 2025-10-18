#ifndef LIN_LIST_H
#define LIN_LIST_H

#include <iostream>
#include <iostream>
#include <ostream>
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
    virtual bool insert(int index, const T& x) = 0; // 插入元素
    virtual bool add(const T& x) = 0; // 添加元素到末尾
    virtual bool remove(int index) = 0; // 删除元素
    //virtual int locate(const T& x,int num = 1) = 0; // 按值查找元素,num表示查找第几个出现的该值，默认查找第一个
    virtual bool getx(int index, T& x) = 0; // 按位查找元素
    virtual bool setx(int index, const T& x) = 0; // 按位修改元素
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

    bool insert(int index, const T& x) override {//插入元素
        if(!index_safe(index)&&index!=0) return false;
        if(full()) return false;
        //安全检查通过后，进行插入
        for (int i = length; i > index; i--){//后移
            data[i] = data[i - 1];
        }
        data[index] = x;
        length++;
        return true;
    }

    bool add(const T& x) override {//添加元素到末尾
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

    // int locate(const T& x,int num = 1) override {//按值查找元素
    //     int count = 0;
    //     for (int i = 0; i < length; i++) {
    //         if(data[i]==x){
    //             count++;
    //             if(count == num)return i;
    //         }
    //     }
    //     return -1; // 未找到
    // }

    bool getx(int index, T& x) override {//按位查找元素
        if(!index_safe(index)) return false;
        x = data[index];
        return true;
    }

    bool setx(int index, const T& x) override {//按位修改元素
        if(!index_safe(index)) return false;
        data[index] = x;
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
    friend std::ostream& operator<< (std::ostream& os,SeqList<T>& list) {//重载输出
        //输出形如[a b c d]
        os << "[";
        for (int i = 0; i < list.length; i++) {
            os << list.data[i];
            if (i != list.length - 1) os << " ";
        }
        os << "]";
        return os;
    }

private:
    T* data; // 存储空间基址
    int maxSize; // 存储空间的最大容量
    int length; // 当前长度
};

// 链式表节点类
template <class T>
class ListNode{
public:
    ListNode<T>(): next(nullptr) {}
    T data; // 数据域
    ListNode* next; // 指针
};

template <class T>
class LinkList : public LinearList<T> {

public:
    LinkList() : head(nullptr), length(0),tail(nullptr) {} // 构造函数
    ~LinkList() { //析构函数,递归删除所有节点
        ListNode<T>* current = head;
        while(current!=nullptr) {
            ListNode<T>* temp = current;
            current = current->next;
            delete temp;
        }
    }
    bool insert(int index, const T& x) override {//插入元素
        if(!index_safe(index)&&index!=0) return false;
        ListNode<T>* newListNode = new ListNode<T>();
        newListNode->data = x;
        if(index == 0) {//插入头节点
            newListNode->next = head;
            head = newListNode;
            tail = newListNode;
        }else if(index < length){//插入中间节点
            ListNode<T>* current = head;
            for(int i = 0; i < index - 1; i++) {
                current = current->next;
            }
            newListNode->next = current->next;
            current->next = newListNode;
        }else if(index == length){//插入尾节点
            tail->next = newListNode;
            newListNode->next = nullptr;
            tail = newListNode;
        }
        length++;
        return true;
    }

    bool add(const T& x) override {//添加元素到末尾
        ListNode<T>* newListNode = new ListNode<T>();
        newListNode->data = x;
        newListNode->next = nullptr;
        if(tail == nullptr) {//空链表
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
        if(index == 0) {//删除头节点
            toDelete = head;
            head = head->next;
            if(head == nullptr) tail = nullptr; // 如果链表变空，更新tail
        }else{
            ListNode<T>* current = head;
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

    ListNode<T>* fake_remove(int index) {//用于完成撤销操作，返回被删除节点的指针
        if(!index_safe(index)) return nullptr;
        ListNode<T>* toDelete;
        if(index == 0) {//删除头节点
            toDelete = head;
            head = head->next;
            if(head == nullptr) tail = nullptr; // 如果链表变空，更新tail
        }else{
            ListNode<T>* current = head;
            for(int i = 0; i < index - 1; i++) {
                current = current->next;
            }
            toDelete = current->next;
            current->next = toDelete->next;
            if(toDelete == tail) tail = current; // 如果删除的是尾节点，更新tail
        }
        length--;
        return toDelete;
    }

    void auto_insert(ListNode<T>* node){//用于按从低到高排好需序的节点自动寻找位置插入，撤销时使用
        if(head == nullptr || head->data >= node->data) {//插入头节点
            node->next = head;
            head = node;
            tail = node;
        }else{
            ListNode<T>* current = head;
            while(current->next != nullptr && current->next->data < node->data) {
                current = current->next;
            }
            node->next = current->next;
            current->next = node;
            if(node->next == nullptr) tail = node; // 如果插入的是尾节点，更新tail
        }
        length++;

    }

    // int locate(const T& x,int num = 1) override {//按值查找元素
    //     ListNode<T>* current = head;
    //     int index = 0;
    //     int count = 0;
    //     while(current != nullptr) {
    //         if(current->data == x){
    //             count++;
    //             if(count == num) return index;
    //         }
    //         current = current->next;
    //         index++;
    //     }
    //     return -1; // 未找到
    // }

    bool getx(int index, T& x) override {//按位查找元素
        if(!index_safe(index)) return false;
        ListNode<T>* current = head;
        for(int i = 0; i < index; i++) {
            current = current->next;
        }
        x = current->data;
        return true;
    }

    bool setx(int index, const T& x) override {//按位修改元素
        if(!index_safe(index)) return false;
        ListNode<T>* current = head;
        for(int i = 0; i < index; i++) {
            current = current->next;
        }
        current->data = x;
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
        ListNode<T>* current = head;
        for(int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->data;
    }
    //重载输出
    friend std::ostream& operator<< (std::ostream& os,LinkList<T>& list) {
        //输出形如[a b c d]
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
    //返回尾节点指针,const修饰
    ListNode<T>* tail_ptr() const {return tail;}

private:
    ListNode<T>* head; // 头指针
    ListNode<T>* tail; // 尾指针,便于添加元素
    int length; // 当前长度
};




#endif