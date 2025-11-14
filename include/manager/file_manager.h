#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <iostream>
#include <fstream>
#include <string>
#include "data_structure/lin_list.h"

// 前向声明核心数据结构和模型
class Client;
class Post;
class Comment;


class FileManager {
public:
    // 文件路径常量
    const std::string file = "clients.dat";

    // 单例模式访问
    inline static FileManager& instance() {
        static FileManager instance; 
        return instance;
    }

    // 核心功能：保存整个系统状态（包括所有客户端及其内容）
    bool save(SeqList<Client>& clients);

    // 核心功能：加载整个系统状态
    bool load(SeqList<Client>& clients);

private:
    FileManager() = default;
    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;

    
    bool reconstructPointers(SeqList<Client>& clients);// 重建加载后对象间的指针关系
};

#endif