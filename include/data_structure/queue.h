#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#include "data_structure/heap.h"
template<class T>
class Queue{
public:
    Queue() {}
    virtual ~Queue() {} // 建议虚析构
    virtual void enqueue(const T& x) = 0; 
    virtual T dequeue() = 0; 
    virtual T& front() = 0; 
    virtual bool empty() = 0; 
    virtual bool full() = 0; 
    virtual int size() = 0; 
    virtual void clear() = 0; 
};

template<class T>
class SeqQueue : public Queue<T>{
public:
    SeqQueue<T>(int size):maxSize(size),Front(0),Rear(0){
        data = new T[maxSize];
    }
    ~SeqQueue<T>(){
        delete[] data;
    }
    // 建议为SeqQueue也添加拷贝构造，此处略，重点修复LinkQueue

    bool full() override{return Front==Rear && tag==1;}
    bool empty() override{return Front==Rear && tag==0;}
    int size() override{
        if(full()) return maxSize;
        if(empty()) return 0;
        if(Rear>Front) return Rear-Front;
        else return Rear+maxSize-Front;
    }
    void clear() override{
        Front = 0;
        tag = 0;
        Rear = 0;
    }
    void enqueue(const T& x) override{
        if(full()) throw std::out_of_range("队列已满");
        data[Rear] = x;
        Rear = (Rear + 1) % maxSize;
        tag = 1;
    }
    T dequeue() override{
        if(empty()) throw std::out_of_range("队列为空");
        T x = data[Front];
        Front = (Front + 1) % maxSize;
        tag = 0;
        return x;
    }
    T& front() override{
        if(empty()) throw std::out_of_range("队列为空");
        return data[Front];
    }
    
    friend std::ostream& operator<< (std::ostream& os,SeqQueue<T>& queue) {
        os << "[";
        if(!queue.empty()){
            int i = queue.Front;
            while(true){
                os << queue.data[i];
                i = (i + 1) % queue.maxSize;
                if(i == queue.Rear) break;
                os << " ";
            }
        }
        os << "]";
        return os;
    }
private:
    T* data;
    int maxSize;
    int Front; 
    int Rear; 
    bool tag = 0; 
};

//链队列节点
template<class T>
class QNode{
public:
    QNode<T>() : next(nullptr) {}
    T data;
    QNode* next;
};



template<class T>
class LinkDeque;

template<class T>
class LinkQueue : public Queue<T>{
public:
    LinkQueue():Front(nullptr),Rear(nullptr),length(0){}
    ~LinkQueue(){
        clear();
    }

    LinkQueue(const LinkQueue& other) : Front(nullptr), Rear(nullptr), length(0) {
        QNode<T>* current = other.Front;
        while (current) {
            this->enqueue(current->data);
            current = current->next;
        }
    }

    LinkQueue& operator=(const LinkQueue& other) {
        if (this == &other) return *this;
        clear();
        QNode<T>* current = other.Front;
        while (current) {
            this->enqueue(current->data);
            current = current->next;
        }
        return *this;
    }

    bool empty() override{return Front == nullptr;}
    bool full() override{return false;}
    int size() override{return length;}
    void enqueue(const T& x) override{
        QNode<T>* newNode = new QNode<T>();
        newNode->data = x;
        newNode->next = nullptr;
        if(empty()){
            Front = newNode;
            Rear = newNode;
        }else{
            Rear->next = newNode;
            Rear = newNode;
        }
        length++;
    }
    T dequeue() override{
        if(empty()) throw std::out_of_range("队列为空");
        QNode<T>* temp = Front;
        T x = Front->data;
        Front = Front->next;
        delete temp;
        length--;
        if(Front == nullptr) Rear = nullptr; 
        return x;
    }
    T& front() override{
        if(empty()) throw std::out_of_range("队列为空");
        return Front->data;
    }
    void clear() override{
        while(!empty()){
            dequeue();
        }
    }
    
    friend std::ostream& operator<< (std::ostream& os,LinkQueue<T>& queue) {
        os << "[";
        if(!queue.empty()){
            QNode<T>* current = queue.Front;
            while(current != nullptr){
                os << current->data;
                current = current->next;
                if(current != nullptr) os << " ";
            }
        }
        os << "]";
        return os;
    }
    friend class LinkDeque<T>;
private:
    QNode<T>* Front; 
    QNode<T>* Rear; 
    int length; 
};

template<class T>
class Deque : public Queue<T>{
public:
    Deque(int size):data(size){}
    ~Deque() {}
    bool empty() override{return data.empty();}
    bool full() override{return data.full();}
    int size() override{return data.size();}
    void clear() override{data.clear();}
    void enqueue(const T& x) override{
        data.enqueue(x);
    }
    void enqueueFront(const T& x){
        if(full()) throw std::out_of_range("队列已满");
        int newFront = (data.Front - 1 + data.maxSize) % data.maxSize;
        data.data[newFront] = x;
        data.Front = newFront;
        data.tag = 1; 
    }
    T dequeue() override{
        return data.dequeue();
    }
    T dequeueRear(){
        if(empty()) throw std::out_of_range("队列为空");
        int newRear = (data.Rear - 1 + data.maxSize) % data.maxSize;
        T x = data.data[newRear];
        data.Rear = newRear;
        data.tag = 0; 
        return x;
    }
    T& front() override{
        return data.front();
    }
    T& rear(){
        if(empty()) throw std::out_of_range("队列为空");
        int rearIndex = (data.Rear - 1 + data.maxSize) % data.maxSize;
        return data.data[rearIndex];
    }
    
    friend std::ostream& operator<< (std::ostream& os,Deque<T>& queue) {
        os << queue.data;
        return os;
    }
private:
    SeqQueue<T> data;
};

template<class T>
class LinkDeque : public Queue<T>{
public:
    LinkDeque(){}
    ~LinkDeque() {}
    bool empty() override{return linkQueue.empty();}
    bool full() override{return linkQueue.full();}
    int size() override{return linkQueue.size();}
    void clear() override{linkQueue.clear();}
    void enqueue(const T& x) override{
        linkQueue.enqueue(x);
    }
    void enqueueFront(const T& x){
        QNode<T>* newNode = new QNode<T>();
        newNode->data = x;
        newNode->next = linkQueue.Front;
        linkQueue.Front = newNode;
        if(linkQueue.Rear == nullptr) linkQueue.Rear = newNode; 
        linkQueue.length++;
    }
    T dequeue() override{
        return linkQueue.dequeue();
    }
    T dequeueRear(){
        if(empty()) throw std::out_of_range("队列为空");
        if(linkQueue.Front == linkQueue.Rear){
            T x = linkQueue.Rear->data;
            delete linkQueue.Rear;
            linkQueue.Front = nullptr;
            linkQueue.Rear = nullptr;
            linkQueue.length--;
            return x;
        }
        
        QNode<T>* current = linkQueue.Front;
        while(current->next != linkQueue.Rear){
            current = current->next;
        }
        T x = linkQueue.Rear->data;
        delete linkQueue.Rear;
        linkQueue.Rear = current;
        linkQueue.Rear->next = nullptr;
        linkQueue.length--;
        return x;
    }
    T& front() override{
        return linkQueue.front();
    }
    T& rear(){
        if(empty()) throw std::out_of_range("队列为空");
        return linkQueue.Rear->data;
    }
    
    friend std::ostream& operator<< (std::ostream& os,LinkDeque<T>& queue) {
        os << queue.linkQueue;
        return os;
    }
private:
    LinkQueue<T> linkQueue;
};

template <typename T>
class PriorityQueue {
public:
    PriorityQueue(int maxsize = 100) : heap(maxsize) {}
    
    PriorityQueue(LinearList<T>& list) : heap(list) {}

    virtual ~PriorityQueue() {}

    void enqueue(const T& x) {
        heap.push(x);
    }

    T dequeue() {
        return heap.pop();
    }
    
    bool empty() {
        return heap.empty();
    }

    bool full() {

        return false; 
    }

    int size() {
        return heap.size();
    }

    void clear() {
        while(!heap.empty()) heap.pop();
    }

private:
    MinHeap<T> heap; 
};

#endif