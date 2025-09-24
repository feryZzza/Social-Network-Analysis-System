#ifndef POST_H
#define POST_H
#include <iostream>
#include "data_structure/lin_list.h"
#include "data_structure/stack.h"
#include "data_structure/queue.h"
#include "models/clients.h"
#include "models/comment.h"

class Client;
class Comment;

class Post{
public:
    Post() {}
    Post(std::string title, Client* author, std::string content) : title(title), author(author), content(content), likes(0), comments(0) {}
    Post(std::string title, std::string content) : title(title), content(content), likes(0), comments(0) {}
    void addComment(Comment &c,Client* commenter);
    void set_author(Client* a);
    //重载输出
    friend std::ostream& operator<< (std::ostream& os,Post& p);
private:
    int floor = 1;//评论楼层数，借鉴贴吧
    std::string title;//帖子标题
    std::string author_name;//帖子作者
    std::string content;//帖子内容
    int likes;//点赞数
    int comments;//评论数
    int idex = 0;//帖子的序号
    LinkList<Comment> comment_list; //评论列表
    Client* author; //作者指针
};
#endif