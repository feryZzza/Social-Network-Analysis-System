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
    for (int i = 0; i < clients.size(); ++i) {
        cout << clients[i].Name() << " 的好友数: " << graph.degree(i) << endl;
        clients.friendCount= graph.degree(i);
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
        LinkList<int> path;
        if (!graph.shortestPath(start, target, path)) {
            cout << from << " 和 " << to << " 之间暂无好友链路" << endl;
            return;
        }
        cout << from << " 到 " << to << " 的最短好友链路: ";
        for (int i = 0; i < path.size(); ++i) {
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

    //module_3_demonstration(clients);
    
    return 0;
}

// 模块三：信息压缩与编码模块 功能演示函数
void module_3_demonstration(SeqList<Client>& clients) {
    cout << "\n\n======================================================\n";
    cout << "      模块三：哈夫曼编码功能演示\n";
    cout << "======================================================\n";

    // 1. 词频统计：分析所有用户发布的历史帖子内容
    string all_post_content = "";
    for (int i = 0; i < clients.size(); ++i) {
        for (int j = 0; j < clients[i].posts.size(); ++j) {
             all_post_content += clients[i].posts[j].get_title();
        }
    }
    
    if (all_post_content.empty()) {
        cout << "没有可用于分析的帖子内容。" << endl;
        return;
    }

    cout << "用于分析的帖子总内容: \n\"" << all_post_content << "\"\n\n";

    map<string, int> frequencies = countFrequency(all_post_content);
cout << "--- 词频统计分析 ---\n";
    cout << "分析完毕，共统计到 " << frequencies.size() << " 个独立字符。\n\n";
    
    int total_chars = 0; // 用于计算总字符数
    
    // 遍历 map 并打印每个字符的频率（权重）
    // (这里使用了 C++17 的结构化绑定，你的编译器支持)
    for (auto const& [key, val] : frequencies) {
        // 使用 \t (制表符) 来尝试对齐，使输出更直观
        cout << "字符: '" << key << "' \t 出现次数 (权重): " << val << endl;
        total_chars += val;
    }
    
    cout << "\n总字符数 (所有权重之和): " << total_chars << endl;
    cout << "------------------------\n\n";
    // 2. 哈夫曼树构建与编码
    HuffmanTree huffman_tree(frequencies);
    huffman_tree.generateCodes();
    huffman_tree.printCodes();
    
    // 3. 压缩与解压模拟
    cout << "\n--- 压缩与解压模拟 ---\n";
    string original_text = "出心了";
    cout << "原始文本: " << original_text << endl;
    
    string compressed_code = huffman_tree.compress(original_text);
    cout << "压缩后的编码: " << compressed_code << endl;

    string decompressed_text = huffman_tree.decompress(compressed_code);
    cout << "解压后的文本: " << decompressed_text << endl;
    
    // 4. 树遍历与效率分析
    cout << "\n--- 效率分析 ---\n";
    double wpl = huffman_tree.getWPL();
    cout << "哈夫曼树的带权路径长度 (WPL) 为: " << wpl << endl;
    cout << "======================================================\n";



    //模块六
    haoyoushu(clients);    // 用户按好友数排序
    dianzanshu(clients);   // 帖子按点赞数排序
}
