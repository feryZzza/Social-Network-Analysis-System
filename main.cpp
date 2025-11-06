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

using namespace std;

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

    //clients[1].addPost(post2);

    Comment comment1(&clients[1],"我懂，因为我真出了");
    

    clients[1].addComment(&clients[0].posts[0],comment1);
    clients[1].addComment(&clients[0].posts[0],comment1);
    clients[1].deleteComment(&clients[0].posts[0],2);

    Comment comment2(&clients[0],"欧狗滚出三角洲！",2);
    clients[0].addComment(&clients[0].posts[0],comment2);
    clients[0].addPost(post1);
    //clients[1].addPost(post2);
    clients[1].like(&clients[0].posts[0]);



    cout<<clients[0].posts[0];
    clients[1].undo();
    clients[1].undo();



    cout<<clients[0].posts[0];
    clients[0].read_messege();

    // 构建用户关系图：节点是用户，边是好友/关注关系
    SocialGraph graph(static_cast<std::size_t>(clients.size()));
    graph.addEdge(0, 1); // 自信的空空 ↔ 我不想上学
    graph.addEdge(1, 2); // 我不想上学 ↔ 一只死肥宅
    graph.addEdge(2, 3); // 一只死肥宅 ↔ 贪吃的猪
    graph.addEdge(0, 2); // 额外关系，方便形成更短路径

    // 输出每个用户的度（好友数）
    auto degrees = graph.degrees();
    for (int i = 0; i < clients.size(); ++i) {
        cout << clients[i].Name() << " 的好友数: " << degrees[i] << endl;
    }

    // 查找好友最多的用户
    int maxIndex = graph.indexWithMaxDegree();
    if (maxIndex != -1) {
        cout << "好友最多的用户: " << clients[maxIndex].Name()
             << "，好友数: " << graph.degree(maxIndex) << endl;
    }

    // BFS查询任意两位用户之间的最短关系链
    auto findClientIndex = [&](const string& nickname) -> int {
        for (int i = 0; i < clients.size(); ++i) {
            if (clients[i].Name() == nickname) {
                return i;
            }
        }
        return -1;
    };

    auto showRelationshipChain = [&](const string& from, const string& to) {
        int start = findClientIndex(from);
        int target = findClientIndex(to);
        if (start == -1 || target == -1) {
            cout << "无法找到用户: " << from << " 或 " << to << endl;
            return;
        }
        auto path = graph.shortestPath(start, target);
        if (path.empty()) {
            cout << from << " 和 " << to << " 之间暂无好友链路" << endl;
            return;
        }
        cout << from << " 到 " << to << " 的最短好友链路: ";
        for (std::size_t i = 0; i < path.size(); ++i) {
            cout << clients[path[i]].Name();
            if (i + 1 < path.size()) {
                cout << " -> ";
            }
        }
        cout << endl;
    };

    showRelationshipChain("自信的空空", "贪吃的猪");
    showRelationshipChain("我不想上学", "自信的空空");






    //cout<<clients[0].posts[0].likes_num()<<endl;
    //clients[1].undo();

    //cout<<clients[0].posts.size()<<endl;
    
    //cout << cl1;

    return 0;
}
