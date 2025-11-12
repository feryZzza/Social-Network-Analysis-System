#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include <iostream>
#include "data_structure/lin_list.h"

class Post;
class Action;
class Client;

//这个文件实现了单例模式的 UndoManager 类，用于管理 Action 对 Post 的引用关系，防止野指针问题。
//当 Post 被销毁时，通知所有引用该 Post 的 Action，使其无效化，这样无需在 Action 中保存 过多的状态信息和指针

struct post_resource{
    Post* post = nullptr;
    LinkList<Action*> actions; 
    //使用 LinkList 存储引用该 Post 的 Action 列表，在 Post 被销毁时遍历该列表通知所有 Action
    //在 Action 被撤销时也会从该列表中移除对应的 Action 指针
    
    bool operator==(const post_resource& other) const {// 重载等于运算符，便于查找（在自己写的数据结构中，按照内容查找索引的那个函数）
        return this->post == other.post;
    }
};


class UndoManager {
private:

    LinkList<post_resource> registry;// 注册表，存储所有 Post 及其对应的 Action 列表
    // 设计为单例模式
    UndoManager() {}
    UndoManager(const UndoManager&) = delete;
    UndoManager& operator=(const UndoManager&) = delete;

    int find_post_index(Post* p); // 查找 Post 在注册表中的索引

public:

    inline static UndoManager& instance() {//外部访问单例实例接口
        static UndoManager instance; 
        return instance;
    }

    void register_action(Post* p, Action* a) ;// 注册 Action 对 Post 的引用关系

    void notify_post_destroyed(Post* p); // 通知所有引用该 Post 的 Action，使其无效化

    void unregister_action_self(Post* p, Action* a);// Action 主动注销自身对 Post 的引用关系
};

#endif // UNDO_MANAGER_H