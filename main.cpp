#include <ios>
#include <iostream>
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

using namespace std;

int findClientIndex(SeqList<Client>& clients, const string& nickname) {
    for (int i = 0; i < clients.size(); ++i) {
        if (clients[i].Name() == nickname) {
            return i;
        }
    }
    return -1; 
}

void showRelationshipChain(SocialGraph& graph, SeqList<Client>& clients, const string& from, const string& to) {
    
    int start = findClientIndex(clients, from); 
    int target = findClientIndex(clients, to);

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
}


void module_3_demonstration(SeqList<Client>& clients); 

int main() {

    SeqList<Client> clients(100);
    SeqList<Client*> clients_ptr(100);

    clients.add(Client("自信的空空", "2022211001001306", "123456"));
    clients_ptr.add(&clients[0]);

    clients.add(Client("我不想上学", "2022211001001307", "123456"));
    clients_ptr.add(&clients[1]);

    clients.add(Client("一只死肥宅", "2022211001001308", "123456"));
    clients_ptr.add(&clients[2]);

    clients.add(Client("贪吃的猪", "2022211001001309", "123456"));
    clients_ptr.add(&clients[3]);


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
    
    clients[1].like(&clients[0].posts[0]);



    cout<<clients[0].posts[0];

    clients[1].undo();
    clients[1].undo();



    cout<<clients[0].posts[0];
    
    clients[0].read_messege();

    SocialGraph graph(static_cast<std::size_t>(clients.size()));
    graph.addEdge(0, 1); 
    graph.addEdge(1, 2); 
    graph.addEdge(2, 3); 
    graph.addEdge(0, 2); 

    for (int i = 0; i < clients.size(); ++i) {
        cout << clients[i].Name() << " 的好友数: " << graph.degree(i) << endl;
    }
    int maxIndex = graph.indexWithMaxDegree();
    if (maxIndex != -1) {
        cout << "好友最多的用户: " << clients[maxIndex].Name()
             << "，好友数: " << graph.degree(maxIndex) << endl;
    }

    showRelationshipChain(graph, clients, "自信的空空", "贪吃的猪");
    showRelationshipChain(graph, clients, "我不想上学", "自信的空空");
    
    return 0;
}

void module_3_demonstration(SeqList<Client>& clients) {
    cout << "\n\n======================================================\n";
    cout << "      模块三：哈夫曼编码功能演示\n";
    cout << "======================================================\n";

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

    
    LinkList<FrequencyPair> frequencies = countFrequency(all_post_content); 
    
    cout << "--- 词频统计分析 ---\n";
    cout << "分析完毕，共统计到 " << frequencies.size() << " 个独立字符。\n\n";
    
    int total_chars = 0; 
    
    for (int i = 0; i < frequencies.size(); i++) {
        cout << "字符: '" << frequencies[i].data << "' \t 出现次数 (权重): " << frequencies[i].weight << endl;
        total_chars += frequencies[i].weight;
    }
    
    cout << "\n总字符数 (所有权重之和): " << total_chars << endl;
    cout << "------------------------\n\n";
    
    HuffmanTree huffman_tree(frequencies);
    huffman_tree.generateCodes();
    huffman_tree.printCodes();
    
    cout << "\n--- 压缩与解压模拟 ---\n";
    string original_text = "出心了";
    cout << "原始文本: " << original_text << endl;
    
    string compressed_code = huffman_tree.compress(original_text);
    cout << "压缩后的编码: " << compressed_code << endl;

    string decompressed_text = huffman_tree.decompress(compressed_code);
    cout << "解压后的文本: " << decompressed_text << endl;
    
    cout << "\n--- 效率分析 ---\n";
    double wpl = huffman_tree.getWPL();
    cout << "哈夫曼树的带权路径长度 (WPL) 为: " << wpl << endl;
    cout << "======================================================\n";
}