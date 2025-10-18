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
#include "models/huffman.h"

using namespace std;

void module_3_demonstration(SeqList<Client>& clients); 

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






    //cout<<clients[0].posts[0].likes_num()<<endl;
    //clients[1].undo();

    //cout<<clients[0].posts.size()<<endl;
    
    //cout << cl1;

    module_3_demonstration(clients);
    
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

    // 2. 哈夫曼树构建与编码
    HuffmanTree huffman_tree(frequencies);
    huffman_tree.generateCodes();
    huffman_tree.printCodes();
    
    // 3. 压缩与解压模拟
    cout << "\n--- 压缩与解压模拟 ---\n";
    string original_text = "我真的出心了";
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
}