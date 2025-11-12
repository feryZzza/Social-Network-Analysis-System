#include "models/sort.h"

//  自定义工具函数 
template <typename T>
void mySwap(T& a, T& b) {
    T temp = std::move(a);
    a = std::move(b);
    b = std::move(temp);
}

int myMin(int a, int b) {
    return a < b ? a : b;
}
