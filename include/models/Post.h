#ifndef POST_H
#define POST_H
#include <iostream>
#include "data_structure/lin_list.h"
#include "data_structure/stack.h"
#include "data_structure/queue.h"
// #include "models/clients.h" // <-- 移除这个，打破循环！
#include "models/comment.h"
#include "manager/undo_manager.h"
using namespace std;

class Client; // <-- 改用前向声明
class Comment;

class Post{
public:
    Post() {}
    Post(std::string title, Client* author, std::string content) : title(title), author(author), content(content), likes(0) {}
    Post(std::string title, std::string content) : title(title), content(content), likes(0) {}
    void addComment(Comment &c,Client* commenter);
    void set_author(Client* a);
    void receive_likes(Client* liker,bool undo=false);
    int comments_num(){return comment_list.size();}
    int likes_num(){return likes;}
    void set_idex(int i){idex = i;}
    int get_idex(){return idex;}

    string get_title() const { return title; }
    string get_content() const { return content; }
    string get_author_name() const { return author_name; }
    int get_floor() const { return floor; }
    void set_floor(int f) { floor = f; }
    LinkList<Client*>& get_likes_list() { return likes_list; }

    //重载输出
    friend std::ostream& operator<< (std::ostream& os,Post& p);
    //重载比较运算符
    bool operator==(const Post& other) const {return this->idex == other.idex;}
    bool operator<=(const Post& other) const {return this->idex <= other.idex;}
    bool operator<(const Post& other) const {return this->idex < other.idex;}
    bool operator>(const Post& other) const {return this->idex > other.idex;}
    bool operator>=(const Post& other) const {return this->idex >= other.idex;}
    
    Client* author; //作者指针
    LinkList<Comment> comment_list; //评论列表
private:
    int floor = 1;//评论楼层数，借鉴贴吧,每添加一个评论楼层数加一，不会因为删除评论而减少，楼主永远是1楼
    std::string title;//帖子标题
    std::string author_name;//帖子作者
    std::string content;//帖子内容
    int likes;//点赞数
    int idex = 0;//帖子的序号
    
    LinkList<Client*> likes_list; //点赞用户列表
    
};
#endif