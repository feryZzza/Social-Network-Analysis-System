#ifndef STACK_H
#define STACK_H

#include <iostream>

template<class T>
class StackBase{
public:
    StackBase() {}
    ~StackBase() {}
    virtual bool push(const T& elem) = 0; //入栈
    virtual T pop() = 0; //出栈
    virtual T top() = 0; //取栈顶元素
    virtual bool empty() = 0; //栈是否为空
    virtual bool full() = 0; //栈是否为满
    virtual int size() = 0; //栈的大小
    virtual void clear() = 0; //清空栈
    
};
//顺序栈
template<class T>
class SeqStack : public StackBase<T>{
public:
    SeqStack(int size):maxSize(size),topIndex(-1){
        data = new T[maxSize];
    }
    ~SeqStack(){
        delete[] data;
    }
    bool push(const T& elem) override{
        if(full()) return false; //栈满
        data[++topIndex] = elem;
        return true;
    }
    T pop() override{
        if(empty()) throw std::out_of_range("栈为空");
        return data[topIndex--];//栈顶元素出栈,并且返回原栈顶元素
    }
    T top() override{
        if(empty()) throw std::out_of_range("栈为空");
        return data[topIndex];//返回栈顶元素
    }

    bool empty() override{return topIndex == -1;}
    int size() override{return topIndex + 1;}
    void clear() override{topIndex = -1;}
    bool full() override{return topIndex >= maxSize - 1;};

private:
    T* data; //存储空间基址
    int maxSize; //存储空间的最大容量
    int topIndex; //栈顶索引
};
//链栈
template<class T>
class Node{
public:
    T data; // 数据域
    Node* next; // 指针
};

template<class T>
class LinkStack : public StackBase<T>{
public:
    LinkStack():topNode(nullptr),length(0){}
    ~LinkStack(){
        clear();
    }
    bool push(const T& elem) override{
        Node<T>* newNode = new Node<T>();
        newNode->data = elem;
        newNode->next = topNode;
        topNode = newNode;
        length++;
        return true;
    }
    T pop() override{
        if(empty()) throw std::out_of_range("栈为空");
        Node<T>* toDelete = topNode;
        T elem = toDelete->data;
        topNode = topNode->next;
        delete toDelete;
        length--;
        return elem;
    }
    T top() override{
        if(empty()) throw std::out_of_range("栈为空");
        return topNode->data;
    }
    bool empty() override{return length == 0;}
    int size() override{return length;}
    void clear() override{
        while(!empty()){
            pop();
        }
    }
    bool full() override{return false;} //链栈不会满
private:
    Node<T>* topNode; //栈顶节点
    int length; //栈的长度
};


#endif