#include <iostream>
#include <cmath>
using namespace std;
double f(double x) {
    return powl(x,x);
}
int main() {
    long double x = 0;
    double h = 0.01;
    double d_x = 10e-6; //微小变化量
    double y = 1;
    //求最小值
    for (int time = 0;time<1000;time++){ 
        cout<<x<<endl;
        y = f(x);
        double y_ = f(x + d_x);
        cout<<"y_: "<<y_<<" y: "<<y<<endl;
        double grad = (y_-y)/d_x;
        x = x - h*grad;
    }
    cout<<"f(x)的最小值为:"<<y<<endl;
    cout<<x<<std::endl;


    return 0;
}