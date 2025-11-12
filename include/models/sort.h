#ifndef SORT_H
#define SORT_H
#include <iostream>
#include "data_structure/lin_list.h"
#include "data_structure/stack.h"
#include "data_structure/queue.h"
#include <string>
#include "models/clients.h"
#include "models/Post.h"
#include "models/comment.h"

//自定义函数说明
template <typename T>
void mySwap(T& a, T& b)；
int myMin(int a, int b)；

// 比较函数前置声明
bool compareHaoyou(const client& a, const client& b);
bool comparePost(const Post& a, const Post& b);

// 排序算法前置声明
template <typename T>
void insertionSort(SeqList<T>& arr, bool (*count)(const T& a, const T& b));

template <typename T>
int partition(SeqList<T>& arr, int left, int right, bool (*count)(const T& a, const T& b));

template <typename T>
void quickSort(SeqList<T>& arr, int left, int right, bool (*count)(const T& a, const T& b));

template <typename T>
void heapify(SeqList<T>& arr, int n, int i, bool (*count)(const T& a, const T& b));

template <typename T>
void heapSort(SeqList<T>& arr, bool (*count)(const T& a, const T& b));

template <typename T>
void ShiyingSort(SeqList<T>& arr, bool (*count)(const T& a, const T& b));

// 排行榜核心函数前置声明
void haoyoushu(SeqList<client>& clients);
void dianzanshu(SeqList<client>& users);
