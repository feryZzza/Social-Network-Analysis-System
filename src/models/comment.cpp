#include "models/comment.h"
#include "models/clients.h" 

// 重载输出运算符
std::ostream& operator<< (std::ostream& os, const Comment& c) {
    os << endl;
    os << endl;
    if (c.author) { // 安全检查
        os << c.author->Name();
    } else {
        os << "[未知用户]";
    }
    if(c.comment_floor != -1) os << " 回复了第 " << c.comment_floor << " 楼";
    os << ": "<<endl;
    os << "    " <<c.content << "\n";    
    
    os << "楼层———— " << c.Floor << endl;
    
    os << endl;
    return os;
}