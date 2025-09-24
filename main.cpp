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
    SeqList<Client> clients(100);//100个用户的顺序表
    SeqList<Client*> clients_ptr(100);//存储用户指针用于排序等操作

    clients.add(Client("自信的空空", "2022211001001306", "123456"));
    clients_ptr.add(&clients[0]);
    clients.add(Client("我不想上学", "2022211001001307", "123456"));
    clients_ptr.add(&clients[1]);

    //给用户添加帖子
    Post post1("家人门谁懂啊，普坝出心了", "骗你的");
    Post post2("三角洲3✖3打累了","谁陪我农两把");

    clients[0].addPost(post1);
    clients[1].addPost(post2);

    Comment comment1(&clients[1],"我懂，因为我真出了");
    

    clients[1].addComment(&clients[0].posts[0],comment1);
    Comment comment2(&clients[0],"欧狗滚出三角洲！",2);
    clients[0].addComment(&clients[0].posts[0],comment2);

    clients[0].addPost(post1);
    clients[1].addPost(post2);
    clients[1].like(&clients[0].posts[0]);
    clients[1].like(&clients[0].posts[0]);
    clients[1].undo();

    cout<<clients[0].posts[0];

    
    
    //cout << cl1;


    
    return 0;
}