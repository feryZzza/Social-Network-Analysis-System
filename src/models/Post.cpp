#include "models/Post.h"
#include "models/clients.h"
//现在post也一样只负责存储和管理自己的数据结构，具体的操作逻辑交给Core来处理


std::ostream& operator<< (std::ostream& os, Post& p) {
    os << "\n[帖子] " << p.title << "\n";
    if (p.author) {
        os << "作者: " << p.author->Name() << "\n";
    } else {
        os << "作者: [未知]\n";
    }
    os << "内容: " << p.content << "\n";
    
    os << p.likes << " 人赞过.\n";
    
    // 显示部分评论者
    if(p.comment_list.size() > 0){
         os << p.comment_list.size() << " 条评论.\n";
    }
    os << "----------------------\n";
    os << p.comment_list; 
    return os;
}