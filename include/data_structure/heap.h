#ifndef HEAP_H
#define HEAP_H

#include <algorithm> // std::swap
#include "data_structure/lin_list.h" // 用于堆的构造函数

//虽然堆按照定义来说是一个完全二叉树结构，但为了性能和简化实现，通常使用数组来存储堆，所以不把堆放到树结构文件中
template <typename T>
class MinHeap {
public:
    // 默认构造函数
    MinHeap(int maxsize = 100) : maxsize(maxsize), currentSize(0) {
        heap = new T[maxsize];
    }
    // 通过线性表构建堆
    MinHeap(LinearList<T>& list) {
        currentSize = list.size();
        maxsize = currentSize + 100;// 预留一些空间以便后续插入
        heap = new T[maxsize];

        for (int i = 0; i < currentSize; ++i) {
            list.getx(i, heap[i]);
        }

        if (currentSize > 1) {
            for (int i = (currentSize - 2) / 2; i >= 0; --i) {
                siftDown(i);
            }
        }
    }

    ~MinHeap() {
        delete[] heap;
    }

    void push(T val) {
        if (currentSize >= maxsize) return;
        heap[currentSize] = val;
        siftUp(currentSize);
        currentSize++;
    }

    T pop() {
        if (currentSize == 0) return T();
        T top = heap[0];
        currentSize--;
        heap[0] = heap[currentSize];
        siftDown(0);
        return top;
    }

    T top() const {
        if (currentSize == 0) return T();
        return heap[0];
    }

    bool empty() const { return currentSize == 0; }
    int size() const { return currentSize; }

private:
    T* heap;
    int currentSize;
    int maxsize;

    void siftUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (heap[idx] < heap[parent]) {
                std::swap(heap[idx], heap[parent]);
                idx = parent;
            } else {
                break;
            }
        }
    }

    void siftDown(int idx) {
        while (true) {
            int smallest = idx;
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;

            if (left < currentSize && heap[left] < heap[smallest])
                smallest = left;
            if (right < currentSize && heap[right] < heap[smallest])
                smallest = right;

            if (smallest != idx) {
                std::swap(heap[idx], heap[smallest]);
                idx = smallest;
            } else {
                break;
            }
        }
    }
};


template <typename T>
class MaxHeap {
public:
    // 默认构造函数
    MaxHeap(int maxsize = 100) : maxsize(maxsize), currentSize(0) {
        heap = new T[maxsize];
    }

    // 通过线性表构建堆
    MaxHeap(LinearList<T>& list) {
        currentSize = list.size();
        maxsize = currentSize + 100;
        heap = new T[maxsize];

        // 1. 复制数据
        for (int i = 0; i < currentSize; ++i) {
            list.getx(i, heap[i]);
        }

        // 2. 下沉建堆
        if (currentSize > 1) {
            for (int i = (currentSize - 2) / 2; i >= 0; --i) {
                siftDown(i);
            }
        }
    }

    ~MaxHeap() {
        delete[] heap;
    }

    void push(T val) {
        if (currentSize >= maxsize) return;
        heap[currentSize] = val;
        siftUp(currentSize);
        currentSize++;
    }

    T pop() {
        if (currentSize == 0) return T();
        T top = heap[0];
        currentSize--;
        heap[0] = heap[currentSize];
        siftDown(0);
        return top;
    }

    T top() const {
        if (currentSize == 0) return T();
        return heap[0];
    }

    bool empty() const { return currentSize == 0; }
    int size() const { return currentSize; }

private:
    T* heap;
    int currentSize;
    int maxsize;

    void siftUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (heap[idx] > heap[parent]) {
                std::swap(heap[idx], heap[parent]);
                idx = parent;
            } else {
                break;
            }
        }
    }

    void siftDown(int idx) {
        while (true) {
            int largest = idx;
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;

            if (left < currentSize && heap[left] > heap[largest])
                largest = left;
            if (right < currentSize && heap[right] > heap[largest])
                largest = right;

            if (largest != idx) {
                std::swap(heap[idx], heap[largest]);
                idx = largest;
            } else {
                break;
            }
        }
    }
};

#endif