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
    Post post2("三角洲3✖3打累了","谁陪我农两把");
    cl1.addPost(post1);
    cl1.addPost(post2);
    //cout<<cl1.posts[0];
    cout<<cl1.posts.size()<<endl;
    cl1.deletePost(0);
    cout<<cl1.posts.size()<<endl;
    
    
    cout << cl1;


    
    return 0;
}