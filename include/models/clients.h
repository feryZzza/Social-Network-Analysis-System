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
#include "models/message.h"

using namespace std;

class massege;
class Post;
class Action;
class Comment;

class Client{
public:
    Client() {}
    Client(string name, string id, string password) : name(name), id(id), password(password) {}
    ~Client();

    bool undo();//撤销上一次操作

    void addPost(Post &p);//发帖

    void addComment(Post* post, Comment &comment);//评论帖子

    void deletePost(int index);//删帖（通过帖子序号）

    void deletePost(Post* post);//删帖（通过帖子指针）

    void deleteComment(Post* post,int floor);//删评论

    void deleteComment(Post* post,Comment* comment);//删评论

    string Name(){return name;}//返回昵称

    inline void receive_comment(bool add){//被评论数加一或减一
        if(add) comments_received++;
        else comments_received--;
    }
    
    Action* add_action(Action* a){return a_stack.push_and_get(a);}//添加操作到操作假栈

    void like(Post* post,bool undo=false);//点赞操作，参考了朋友圈的点赞功能，重复点赞变为取消点赞

    LinkList<Post> posts; //用户发布的帖子列表

    bool a_stack_full(){return a_stack.full();}//操作栈是否为满

    void receive_messege(massege* m){//接收消息
        m_q.enqueue(m);
    }

    void read_messege();

    string ID(){return id;}//返回用户ID
    string Password() const { return password;}
    int PostTime() const { return post_time;}
    void setPostTime(int t) { post_time = t;}


    //重载输出
    friend std::ostream& operator<< (std::ostream& os,Client& c);
    //重载==
    bool operator==(const Client& other) const {return this->name == other.name;}

    int action_num(){return a_stack.size();}

private:
    int post_time = 0;//发帖次数,用于生成帖子ID，每发一次帖+1，不会因为删帖而减少，用于区分帖子先后
    string name;//用户昵称
    string id;//用户ID
    string password;//用户密码
    Fake_Stack<Action*> a_stack;//用户操作栈,用于实现撤销功能
    LinkQueue<massege*> m_q;//消息队列
    int likes_received = 0;
    int comments_received = 0;//被评论数
};
#endif