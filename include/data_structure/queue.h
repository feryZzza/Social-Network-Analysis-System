#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
template<class T>
class Queue{
public:
    Queue() {}
    ~Queue() {}
    virtual void enqueue(const T& x) = 0; //入队
    virtual T dequeue() = 0; //出队
    virtual T& front() = 0; //取队头元素
    virtual bool empty() = 0; //队列是否为空
    virtual bool full() = 0; //队列是否为满
    virtual int size() = 0; //队列的大小
    virtual void clear() = 0; //清空队列
};

template<class T>
class SeqQueue : public Queue<T>{//循环队列
public:
    SeqQueue<T>(int size):maxSize(size),Front(0),Rear(0){//空队列
        data = new T[maxSize];
    }
    ~SeqQueue<T>(){
        delete[] data;
    }
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
    //重载输出,从队列头到队列尾输出
    friend std::ostream& operator<< (std::ostream& os,SeqQueue<T>& queue) {//重载输出
        //输出形如[a b c d],从队头到队尾
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
    int Front; //队头索引
    int Rear; //队尾索引
    bool tag = 0; //标志位，区分队列空和满,若上一次操作为入队列则tag=1,否则tag=0，
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
class LinkDeque;//前向声明,双端队列的链式实现需要访问LinkQueue的私有成员,必须声明为友元类

template<class T>
class LinkQueue : public Queue<T>{//链队列,从链表头出队，从链表尾入队
public:
    LinkQueue():Front(nullptr),Rear(nullptr),length(0){}//空队列
    ~LinkQueue(){
        clear();
    }
    bool empty() override{return Front == nullptr;}
    bool full() override{return false;}//链表不会满
    int size() override{return length;}
    void enqueue(const T& x) override{//入队
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
    T dequeue() override{//出队
        if(empty()) throw std::out_of_range("队列为空");
        QNode<T>* temp = Front;
        T x = Front->data;
        Front = Front->next;
        delete temp;
        length--;
        if(Front == nullptr) Rear = nullptr; //队列为空时，更新Rear指针
        return x;
    }
    T& front() override{//取队头元素
        if(empty()) throw std::out_of_range("队列为空");
        return Front->data;
    }
    void clear() override{//清空队列
        while(!empty()){
            dequeue();
        }
    }
    //重载输出,从队列头到队列尾输出
    friend std::ostream& operator<< (std::ostream& os,LinkQueue<T>& queue) {//重载输出
        //输出形如[a b c d],从队头到队尾
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
    QNode<T>* Front; //队头指针
    QNode<T>* Rear; //队尾指针
    int length; //队列长度
};

template<class T>
class Deque : public Queue<T>{//双端队列的顺序实现
public:
    Deque(int size):data(size){}//空队列
    ~Deque() {}
    bool empty() override{return data.empty();}
    bool full() override{return data.full();}
    int size() override{return data.size();}
    void clear() override{data.clear();}
    void enqueue(const T& x) override{//从队尾入队
        data.enqueue(x);
    }
    void enqueueFront(const T& x){//从队头入队
        if(full()) throw std::out_of_range("队列已满");
        int newFront = (data.Front - 1 + data.maxSize) % data.maxSize;
        data.data[newFront] = x;
        data.Front = newFront;
        if(data.Front == data.Rear) data.tag = 1; //更新tag
    }
    T dequeue() override{//从队头出队
        return data.dequeue();
    }
    T dequeueRear(){//从队尾出队
        if(empty()) throw std::out_of_range("队列为空");
        int newRear = (data.Rear - 1 + data.maxSize) % data.maxSize;
        T x = data.data[newRear];
        data.Rear = newRear;
        if(data.Front == data.Rear) data.tag = 0; //更新tag
        return x;
    }
    T& front() override{//取队头元素
        return data.front();
    }
    T& rear(){//取队尾元素
        if(empty()) throw std::out_of_range("队列为空");
        int rearIndex = (data.Rear - 1 + data.maxSize) % data.maxSize;
        return data.data[rearIndex];
    }
    //重载输出,从队列头到队列尾输出
    friend std::ostream& operator<< (std::ostream& os,Deque<T>& queue) {//重载输出
        os << queue.data;
        return os;
    }
private:
    SeqQueue<T> data;
};

template<class T>
class LinkDeque : public Queue<T>{
public:
    LinkDeque(){}//空队列
    ~LinkDeque() {}
    bool empty() override{return linkQueue.empty();}
    bool full() override{return linkQueue.full();}
    int size() override{return linkQueue.size();}
    void clear() override{linkQueue.clear();}
    void enqueue(const T& x) override{//从队尾入队
        linkQueue.enqueue(x);
    }
    void enqueueFront(const T& x){//从队头入队
        QNode<T>* newNode = new QNode<T>();
        newNode->data = x;
        newNode->next = linkQueue.Front;
        linkQueue.Front = newNode;
        if(linkQueue.Rear == nullptr) linkQueue.Rear = newNode; //如果原来队列为空，更新Rear指针
        linkQueue.length++;
    }
    T dequeue() override{//从队头出队
        return linkQueue.dequeue();
    }
    T dequeueRear(){//从队尾出队
        if(empty()) throw std::out_of_range("队列为空");
        if(linkQueue.Front == linkQueue.Rear){//只有一个元素
            T x = linkQueue.Rear->data;
            delete linkQueue.Rear;
            linkQueue.Front = nullptr;
            linkQueue.Rear = nullptr;
            linkQueue.length--;
            return x;
        }
        //找到倒数第二个节点
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
    T& front() override{//取队头元素
        return linkQueue.front();
    }
    T& rear(){//取队尾元素
        if(empty()) throw std::out_of_range("队列为空");
        return linkQueue.Rear->data;
    }
    //重载输出,从队列头到队列尾输出
    friend std::ostream& operator<< (std::ostream& os,LinkDeque<T>& queue) {//重载输出
        os << queue.linkQueue;
        return os;
    }
private:
    LinkQueue<T> linkQueue;
};//双端队列的链式实现

#endif