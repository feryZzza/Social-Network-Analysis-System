#include <ios>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "data_structure/lin_list.h"
#include "data_structure/stack.h"
#include "data_structure/queue.h"
#include "models/Post.h"
#include "models/clients.h"

using namespace std;

int main() {
    Client cl1("自信的空空", "2022211001001306", "123456");
    Post post1("家人门谁懂啊，普坝出心了", "骗你的");
    cl1.addPost(post1);
    cout << cl1;


    
    return 0;
}