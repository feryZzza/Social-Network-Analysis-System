#ifndef STACK_H
#define STACK_H

#include <iostream>
#include "data_structure/queue.h"


template<class T>
class StackBase{
public:
    StackBase() {}
    ~StackBase() {}
    virtual bool push(const T& x) = 0; //入栈
    virtual T pop() = 0; //出栈
    virtual T& top() = 0; //取栈顶元素
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
    bool push(const T& x) override{
        if(full()) return false; //栈满
        data[++topIndex] = x;
        return true;
    }
    T pop() override{
        if(empty()) throw std::out_of_range("栈为空");
        return data[topIndex--];//栈顶元素出栈,并且返回原栈顶元素
    }
    T& top() override{
        if(empty()) throw std::out_of_range("栈为空");
        return data[topIndex];//返回栈顶元素
    }

    bool empty() override{return topIndex == -1;}
    int size() override{return topIndex + 1;}
    void clear() override{topIndex = -1;}
    bool full() override{return topIndex >= maxSize - 1;};

    //重载输出
    friend std::ostream& operator<< (std::ostream& os,SeqStack<T>& stack) {//重载输出
        //输出形如[a b c d],从栈顶到栈底，与链栈相同
        os << "[";
        for (int i = stack.topIndex; i >= 0; i--) {
            os << stack.data[i];
            if (i != 0) os << " ";
        }
        os << "]";

        return os;
    }

private:
    T* data; //存储空间基址
    int maxSize; //存储空间的最大容量
    int topIndex; //栈顶索引
};
//链栈
template<class T>
class StackNode{
public:
    StackNode<T>() : next(nullptr) {}
    T data; // 数据域
    StackNode* next; // 指针
};

template<class T>
class LinkStack : public StackBase<T>{//以链表头为栈顶实现
public:
    LinkStack():topNode(nullptr),length(0){}
    ~LinkStack(){
        clear();
    }
    bool push(const T& x) override{
        StackNode<T>* newStackNode = new StackNode<T>();
        newStackNode->data = x;
        newStackNode->next = topNode;
        topNode = newStackNode;
        length++;
        return true;
    }
    T pop() override{
        if(empty()) throw std::out_of_range("栈为空");
        StackNode<T>* toDelete = topNode;
        T x = toDelete->data;
        topNode = topNode->next;
        delete toDelete;
        length--;
        return x;
    }
    T& top() override{
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

    //重载输出
    friend std::ostream& operator<< (std::ostream& os,LinkStack<T>& stack) {//重载输出
        //输出形如[a b c d],按照链表从topNode开始向后输出，从栈顶到栈底
        os << "[";
        StackNode<T>* current = stack.topNode;
        while(current != nullptr) {
            os << current->data;
            if (current->next != nullptr) os << " ";
            current = current->next;
        }
        os << "]";
        return os;
    }
private:
    StackNode<T>* topNode; //栈顶节点
    int length; //栈的长度
};

template<class T>
class Fake_Stack : public StackBase<T>{
//伪栈，实际上是个队列,在没满的情况下，只能从队尾入栈，从队尾出栈。在满的情况下，再次入栈会弹出队头元素
public:
    Fake_Stack():maxSize(10){}//默认最大容量为10
    Fake_Stack(int size):maxSize(size){}
    ~Fake_Stack() {}
    bool push(const T& x) override{//入栈
        if(full()){//栈满，弹出队头元素
            deque.dequeue();
        }
        deque.enqueue(x);
        return true;
    }
    T pop() override{//出栈
        if(empty()) throw std::out_of_range("栈为空");
        return deque.dequeueRear();//从队尾出队
    }
    T& top() override{//取栈顶元素
        if(empty()) throw std::out_of_range("栈为空");
        return deque.rear();//取队尾元素
    }
    bool empty() override{return deque.empty();}
    int size() override{return deque.size();}
    void clear() override{deque.clear();}
    bool full() override{return deque.size() >= maxSize;}

    //重载输出
    friend std::ostream& operator<< (std::ostream& os,Fake_Stack<T>& stack) {//重载输出
        //输出形如[a b c d],从栈顶到栈底，与链栈相同
        os << "[";
        if(!stack.empty()){
            LinkDeque<T> tempDeque = stack.deque; //创建一个临时的deque来保存原始数据
            while(!tempDeque.empty()){
                os << tempDeque.dequeueRear(); //从队尾出队，模拟栈的顺序
                if(!tempDeque.empty()) os << " ";
            }
        }
        os << "]";
        return os;
    }
private:
    LinkDeque<T> deque;
    int maxSize;
};


#endif