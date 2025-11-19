#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "data_structure/lin_list.h"
#include <string>

class Client; // 前向声明

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
    
    // 辅助结构：用于加载时临时存储关系
    struct TempLoadData {
        std::string client_id;
        std::string post_id; // 格式: clientID_postIndex
        LinkList<std::string> liker_ids;
        LinkList<std::string> comment_author_ids;
        bool operator==(const TempLoadData& other) const {
            return post_id == other.post_id;
        }
    };
    LinkList<TempLoadData> temp_load_data;

    // JSON 辅助函数
    std::string escapeJsonString(const std::string& s);
    std::string unescapeJsonString(const std::string& s);
    
    // 解析辅助函数
    std::string extractValue(const std::string& json, const std::string& key, int& startPos);
    std::string extractObject(const std::string& json, int& startPos);
    std::string extractArray(const std::string& json, int& startPos);
    
    bool reconstructPointers(SeqList<Client>& clients);
};

#endif