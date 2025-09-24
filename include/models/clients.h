#ifndef CLIENTS_H
#define CLIENTS_H
#include<iostream>
#include<string>
#include <type_traits>
#include"models/Post.h"
#include"data_structure/lin_list.h"
#include"data_structure/stack.h"
#include"data_structure/queue.h"
#include"models/action.h"

using namespace std;

class Post;
class Action;
class Comment;

class Client{
public:
    Client() {}
    Client(string name, string id, string password) : name(name), id(id), password(password) {}
    bool undo();
    void addPost(Post p);
    void addComment(Post* post, Comment &comment);
    void deletePost(int index);
    string Name(){return name;}
    void receive_comment(bool add){
        if(add) comments_received++;
        else comments_received--;
    }
    LinkList<Post> posts; //用户发布的帖子列表
    //重载输出
    friend std::ostream& operator<< (std::ostream& os,Client& c);

private:
    string name;//用户昵称
    string id;//用户ID
    string password;//用户密码
    Fake_Stack<Action> a_stack;//用户操作栈,用于实现撤销功能
    int likes_received = 0;
    int comments_received = 0;//被评论数
};
#endif