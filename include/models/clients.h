#ifndef CLIENTS_H
#define CLIENTS_H
#include<iostream>
#include<string>
#include"models/Post.h"
#include"data_structure/lin_list.h"
#include"data_structure/stack.h"
#include"data_structure/queue.h"
#include"models/action.h"

using namespace std;

class Post;
class Action;

class Client{
public:
    Client() {}
    Client(string name, string id, string password) : name(name), id(id), password(password) {}
    bool undo();
    void addPost(Post p);
    void deletePost(int index);
    inline string Name(){return name;}
    LinkList<Post> posts; //用户发布的帖子列表
    //重载输出
    friend std::ostream& operator<< (std::ostream& os,Client& c);

private:
    string name;//用户昵称
    string id;//用户ID
    string password;//用户密码
    Fake_Stack<Action> a_stack;//用户操作栈,用于实现撤销功能
};
#endif