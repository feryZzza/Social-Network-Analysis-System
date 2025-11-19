#ifndef CLIENTS_H
#define CLIENTS_H
#include <iostream>
#include <string>
#include "data_structure/lin_list.h"
#include "data_structure/stack.h"
#include "data_structure/queue.h"
#include "models/Post.h"

// 前向声明
class Post;
class Action;
class massege;

using namespace std;

class Client {
public:
    Client() {}
    Client(string name, string id, string password) : name(name), id(id), password(password) {}
    ~Client(); // 析构函数保留，用于清理栈和消息

    // --- 数据访问 ---
    string Name() const { return name; }
    string ID() const { return id; }
    string Password() const { return password; }
    int PostTime() const { return post_time; }
    void setPostTime(int t) { post_time = t; }
    void incrementPostTime() { post_time++; }

    // --- 状态修改 (仅限基础计数) ---
    void receive_comment(bool add) {
        if(add) comments_received++;
        else comments_received--;
    }
    void make_friend(bool add) {
        if(add) friends_num++;
        else friends_num--;
    }

    // --- 容器访问 (供 Core 使用) ---
    LinkList<Post> posts; // 帖子列表

    // --- Undo 系统支持 ---
    bool a_stack_full() { return a_stack.full(); }
    int action_num() { return a_stack.size(); }
    
    // 压入操作，如果满则返回溢出的元素
    Action* add_action(Action* a) { return a_stack.push_and_get(a); }
    // 弹出操作
    Action* pop_action() { return a_stack.pop(); }

    // --- 消息系统支持 ---
    void receive_messege(massege* m) { m_q.enqueue(m); }
    LinkQueue<massege*>& getMessages() { return m_q; }

    // 重载
    friend std::ostream& operator<< (std::ostream& os, Client& c);
    // 重载比较运算符，基于 name 字段，在搜索树中使用
    bool operator==(const Client& other) const { return this->name == other.name; }
    bool operator<(const Client& other) const { return this->name < other.name; }
    bool operator>(const Client& other) const { return this->name > other.name; }
    bool operator<=(const Client& other) const { return this->name <= other.name; }
    bool operator>=(const Client& other) const { return this->name >= other.name; }

private:
    int post_time = 0;
    string name;
    string id;
    string password;
    Fake_Stack<Action*> a_stack; // 操作栈
    LinkQueue<massege*> m_q;     // 消息队列
    int likes_received = 0;
    int comments_received = 0;
    int friends_num = 0;//好友数
};
#endif