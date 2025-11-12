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
#include "models/comment.h"
#include "models/action.h"
#include "models/message.h"
#include "models/social_graph.h"
#include "data_structure/huffman.h"
#include "models/sort.h"

using namespace std;

void module_3_demonstration(SeqList<Client>& clients); 

int main() {

    SeqList<Client> clients(100);//100个用户的顺序表
    SeqList<Client*> clients_ptr(100);//存储用户指针用于排序等操作

    clients.add(Client("自信的空空", "2022211001001306", "123456"));
    clients_ptr.add(&clients[0]);
    clients.add(Client("我不想上学", "2022211001001307", "123456"));
    clients_ptr.add(&clients[1]);
    clients.add(Client("一只死肥宅", "2022211001001308", "123456"));
    clients_ptr.add(&clients[2]);
    clients.add(Client("贪吃的猪", "2022211001001309", "123456"));
    clients_ptr.add(&clients[3]);

    //给用户添加帖子
    Post post1("家人门谁懂啊，普坝出心了", "骗你的");
    Post post2("三角洲3✖3打累了","谁陪我农两把");

    clients[0].addPost(post1);

    clients[1].addPost(post2);

    Comment comment1(&clients[1],"我懂，因为我真出了");
    

    clients[1].addComment(&clients[0].posts[0],comment1);
    clients[1].addComment(&clients[0].posts[0],comment1);
    clients[1].deleteComment(&clients[0].posts[0],2);

    Comment comment2(&clients[0],"欧狗滚出三角洲！",2);
    clients[0].addComment(&clients[0].posts[0],comment2);
    clients[0].addPost(post1);
    //clients[1].addPost(post2);
    clients[1].like(&clients[0].posts[0]);



    clients[1].undo();
    clients[1].undo();



    clients[0].read_messege();

    // 构建用户关系图：节点是用户，边是好友/关注关系
    SocialGraph graph(static_cast<std::size_t>(clients.size()));
    graph.addEdge(0, 1); // 自信的空空 ↔ 我不想上学
    graph.addEdge(1, 2); // 我不想上学 ↔ 一只死肥宅
    graph.addEdge(2, 3); // 一只死肥宅 ↔ 贪吃的猪
    graph.addEdge(0, 2); // 额外关系，方便形成更短路径

//好友数
    for (int i = 0; i < clients.size(); ++i) {

        clients.friendCount= graph.degree(i);
    }


    //模块六
    haoyoushu(clients);    // 用户按好友数排序
    dianzanshu(clients);   // 帖子按点赞数排序
}
