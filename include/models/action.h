#ifndef ACTION_H
#define ACTION_H
#include <iostream>
#include "data_structure/lin_list.h"
#include "manager/undo_manager.h"
#include <string>

using namespace std;

// 前向声明
class Client;
class Post;
class Comment;

class Action {
public:
    Action() {}
    virtual ~Action() {}
    
    // 纯虚函数：撤销操作
    // 返回 true 表示撤销成功
    virtual bool undo() = 0;
    
    // 初始化
    void init(Client* client, bool add, Post* p);
    
    inline bool check() { return post != nullptr; }
    
    // 当操作被废弃（如Post被删导致Action失效）时调用
    virtual void invalidate() { post = nullptr; used = true; };

    // 当操作从栈中因满而被挤出时调用，用于清理特定的节点内存
    virtual void clean(Client* client_context) {};

    virtual string type() = 0;
    
    bool is_add = false; // true=添加操作(撤销时需删除), false=删除操作(撤销时需恢复)
    bool used = false;   // 是否已失效
    int index = -1;      // 辅助索引
    
    Client* client = nullptr;
    Post* post = nullptr;
};

class PostAction: public Action {
public:
    PostAction(ListNode<Post>* post_node) : post_node(post_node) {}
    PostAction() {}
    ~PostAction() override {} 
    
    void invalidate() override { 
        post_node = nullptr; 
        post = nullptr; 
        used = true; 
    }

    bool undo() override;
    
    void clean(Client* client) override;

    string type() override { return is_add ? "发帖操作" : "删帖操作"; };

    ListNode<Post>* post_node; // 持有链表节点的指针，用于恢复
};


class LikeAction: public Action {
public:
    LikeAction() {}
    ~LikeAction() override {};

    bool undo() override;
    
    // LikeAction 不需要特殊的 clean，因为它不持有 ListNode
    void clean(Client* client_context) override;

    string type() override { return is_add ? "点赞操作" : "取消点赞操作"; };
};

class CommentAction: public Action {
public:
    CommentAction(ListNode<Comment>* comment_node) : comment_node(comment_node) {}
    CommentAction() {}
    ~CommentAction() override {}

    void invalidate() override { 
        comment_node = nullptr; 
        post = nullptr; 
        used = true; 
    }
    
    bool undo() override;
    
    void clean(Client* client_context) override;

    string type() override { return is_add ? "添加评论操作" : "删除评论操作"; };

    ListNode<Comment>* comment_node;
};

#endif