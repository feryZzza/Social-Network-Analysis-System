#include <ios>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "data_structure/lin_list.h"
#include "data_structure/stack.h"
#include "data_structure/queue.h"
#include "manager/undo_manager.h"
#include "models/Post.h"
#include "models/clients.h"
#include "models/comment.h"
#include "models/action.h"
#include "models/message.h"
#include "models/social_graph.h"
#include "data_structure/huffman.h"

using namespace std;

void module_3_demonstration(SeqList<Client>& clients); 

int main() {

    SeqList<Client> clients(100);//100个用户的顺序表
    SeqList<Client*> clients_ptr(100);//存储用户指针用于排序等操作

    clients.add(Client("自信的空空", "2022211001001306", "123456"));
    //clients_ptr.add(&clients[0]);
    clients.add(Client("我不想上学", "2022211001001307", "123456"));
    //clients_ptr.add(&clients[1]);
    //clients.add(Client("一只死肥宅", "2022211001001308", "123456"));
    //clients_ptr.add(&clients[2]);
    //clients.add(Client("贪吃的猪", "2022211001001309", "123456"));
    //clients_ptr.add(&clients[3]);

    //给用户添加帖子
    Post post1("家人门谁懂啊，普坝出心了", "骗你的");

    Post post2("三角洲3✖3打累了","谁陪我农两把");

    clients[0].addPost(post1);
    //UndoManager::instance().show_register();
    clients[0].addPost(post2);
    Comment comment1(&clients[1],"我懂，因为我真出了");
    clients[1].addComment(&clients[0].posts[1], comment1);
    //clients[1].like(&clients[0].posts[1]);
    //clients[0].undo();
    clients[0].deletePost(1);
    UndoManager::instance().show_register();
    
    cout<<"撤销"<<endl;
    clients[1].undo();

    UndoManager::instance().show_register();



    

    return 0;
}