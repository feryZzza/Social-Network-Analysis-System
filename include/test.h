#include "lin_list.h"

//评论类
class huifu{
public:
    std::string author;
    std::string content;
    int likes;
};
//点赞类
class likes{
public:
    std::string user;
    std::string time;
};

//帖子类
class tiezi{
public:
    std::string title;
    std::string author;
    std::string content;
    int likes;

    SeqList<huifu> comments; //评论列表

    LinkList<likes> like_users; //点赞用户列表
};