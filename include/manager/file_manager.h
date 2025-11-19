#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "data_structure/lin_list.h"
#include "models/social_graph.h" // 引入图结构
#include <string>

class Client; // 前向声明
class Post;

class FileManager {
public:
    const std::string file_path = "../data/clients.json";

    static FileManager& instance() {
        static FileManager instance; 
        return instance;
    }

    bool save(SeqList<Client>& clients, SocialGraph& graph);
    bool load(SeqList<Client>& clients, SocialGraph& graph);

private:
    FileManager() = default;
    
    // 临时结构：用于存储“帖子-点赞者Name列表”的关系
    struct TempPostLikers {
        Post* post_ptr; // 指向内存中稳定的 Post 对象
        LinkList<std::string> liker_names; // 修改为存储 Name
    };
    LinkList<TempPostLikers> temp_liker_links;

    // 临时结构：用于存储“用户-好友Name列表”的关系
    struct TempFriendships {
        std::string user_name; // 修改为存储 Name
        LinkList<std::string> friend_names; // 修改为存储 Name
    };
    LinkList<TempFriendships> temp_friend_links;

    // JSON 辅助函数
    std::string escapeJsonString(const std::string& s);
    std::string unescapeJsonString(const std::string& s);
    
    // 解析辅助函数
    std::string extractValue(const std::string& json, const std::string& key, int& startPos);
    
    // 查找辅助
    // 修改参数名为 name，明确语义
    Client* findClient(SeqList<Client>& clients, const std::string& name);
    
    // globalId 格式现在为 "clientName_postIndex"
    Post* findPost(SeqList<Client>& clients, const std::string& globalId);
    
    //获取用户索引的辅助函数
    int getClientIndex(SeqList<Client>& clients, Client* c);
};

#endif