#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "data_structure/lin_list.h"
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

    bool save(SeqList<Client>& clients);
    bool load(SeqList<Client>& clients);

private:
    FileManager() = default;
    
    // 临时结构：用于存储“帖子-点赞者ID列表”的关系
    struct TempPostLikers {
        Post* post_ptr; // 指向内存中稳定的 Post 对象
        LinkList<std::string> liker_ids;
    };
    LinkList<TempPostLikers> temp_liker_links;

    // JSON 辅助函数
    std::string escapeJsonString(const std::string& s);
    std::string unescapeJsonString(const std::string& s);
    
    // 解析辅助函数
    std::string extractValue(const std::string& json, const std::string& key, int& startPos);
    
    // 查找辅助
    Client* findClient(SeqList<Client>& clients, const std::string& id);
    Post* findPost(SeqList<Client>& clients, const std::string& globalId);
};

#endif