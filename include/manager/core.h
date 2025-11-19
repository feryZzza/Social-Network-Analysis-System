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
#include <string>

// 定义操作状态码
enum CoreStatus {
    SUCCESS = 0,
    ERR_CLIENT_EXISTS,
    ERR_LIST_FULL,
    ERR_CLIENT_NOT_FOUND,
    ERR_POST_NOT_FOUND,
    ERR_COMMENT_NOT_FOUND,
    ERR_NO_ACTION_TO_UNDO, // <--- 添加这一行
    ERR_ACTION_INVALID,
    ERR_UNKNOWN
};

class Core {
private:
    SeqList<Client> all_clients; // 系统维护的所有客户端数据
    
    // 单例模式
    Core() : all_clients(100) {} 
    Core(const Core&) = delete;
    Core& operator=(const Core&) = delete;

    // 内部辅助：将操作压入用户的撤销栈
    void pushAction(Client* client, Action* action);
    // 内部辅助：发送消息
    void sendMessage(Client* sender, Client* receiver, Post* post, massege* msg);

public:
    static Core& instance() {
        static Core instance; 
        return instance;
    }

    // --- 数据存取 ---
    bool loadData();
    bool saveData();
    
    SeqList<Client>& getAllClients() { return all_clients; }
    
    Client* getClientById(const std::string& id);

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
};

#endif