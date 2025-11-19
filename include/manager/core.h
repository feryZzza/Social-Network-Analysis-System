#ifndef CORE_H
#define CORE_H

#include "models/clients.h"
#include "models/Post.h"
#include "models/comment.h"
#include "models/action.h"
#include "models/message.h"
#include "data_structure/lin_list.h"
#include "manager/undo_manager.h"
#include "manager/file_manager.h"
#include "data_structure/huffman.h" // 引入哈夫曼模块
#include "data_structure/search_tree.h" // 引入搜索树模块
#include "models/social_graph.h"// 引入社交图模块
#include <string>

// 定义操作状态码
enum CoreStatus {
    SUCCESS = 0,
    ERR_CLIENT_EXISTS,
    ERR_LIST_FULL,
    ERR_CLIENT_NOT_FOUND,
    ERR_POST_NOT_FOUND,
    ERR_COMMENT_NOT_FOUND,
    ERR_NO_ACTION_TO_UNDO, 
    ERR_ACTION_INVALID,
    ERR_UNKNOWN,
    ERR_ALREADY_FRIENDS,
    ERR_SELF_FRIEND 
};
class AVL_node {
public:
    std::string name; // 存储名字副本，用于比较键
    Client* client;   // 指向实际数据的指针
    // 构造函数：传入指针，自动提取名字
    AVL_node(Client* c) : client(c) {
        if (c) name = c->Name();
    }
    // 构造函数：传入名字和指针（用于查找时构造临时对象）
    AVL_node(std::string n, Client* c = nullptr) : name(n), client(c) {}

    // 比较运算符重载：只比较 name
    bool operator==(const AVL_node& other) const {//理论上不允许重名
        return this->name == other.name;
    }
    bool operator<(const AVL_node& other) const {
        return this->name < other.name;
    }
    bool operator>(const AVL_node& other) const {
        return this->name > other.name;
    }
    bool operator<=(const AVL_node& other) const {//理论上不允许重名
        return this->name <= other.name;
    }
    bool operator>=(const AVL_node& other) const {//理论上不允许重名
        return this->name >= other.name;
    }
};

class Core {
private:

    SeqList<Client> all_clients; // 系统维护的所有客户端数据
    AVLTree<AVL_node> client_index; // 使用 AVL_node 作为索引元素
    SocialGraph social_net;// 社交图对象

    bool is_avl_init = false; //平衡树是否初始化 
    
    // 单例模式
    Core() : all_clients(100),social_net(100) {} 
    Core(const Core&) = delete;
    Core& operator=(const Core&) = delete;

    // 内部辅助：将操作压入用户的撤销栈
    void pushAction(Client* client, Action* action);
    // 内部辅助：发送消息
    void sendMessage(Client* sender, Client* receiver, Post* post, massege* msg);

    void rebuildIndex();

    int getClientIndex(Client* c);// 获取用户在线性表中的索引位置,利用了指针的连续性

public:
    static Core& instance() {
        static Core instance; 
        return instance;
    }

    // --- 数据存取 ---
    bool loadData();
    bool saveData();
    
    SeqList<Client>& getAllClients() { return all_clients; }
    
    Client* getClientByName(const std::string& name);



    // --- 业务功能 ---

    // 注册新用户
    CoreStatus registerClient(const std::string& name, const std::string& id, const std::string& password);
    
    // 用户发帖
    CoreStatus userAddPost(Client* client, const std::string& title, const std::string& content);

    // 用户删帖
    CoreStatus userDeletePost(Client* client, int postIndex);
    CoreStatus userDeletePost(Client* client, Post* post);
    
    // 用户发表评论
    CoreStatus userAddComment(Client* commenter, Post* post, const std::string& content, int reply_floor = -1);

    // 用户删除评论
    CoreStatus userDeleteComment(Client* client, Post* post, int floor);
    
    // 用户点赞
    CoreStatus userLikePost(Client* liker, Post* post);

    // 用户撤销操作
    CoreStatus userUndo(Client* client);
    
    // 用户读取消息
    void userReadMessages(Client* client);

    // 用户加好友
    CoreStatus makeFriend(Client* a, Client* b);

    // 用户删好友
    CoreStatus deleteFriend(Client* a, Client* b);

    // 获取两用户在社交图中的关系距离
    int getRelationDistance(Client* a, Client* b);

    // 使用哈夫曼树分析帖子内容，显示编码、压缩二进制并验证解压
    void analyzePostContent(Post* post);
};

#endif