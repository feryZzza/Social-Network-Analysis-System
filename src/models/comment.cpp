#include "models/comment.h"

// 重载输出运算符
std::ostream& operator<< (std::ostream& os, const Comment& c) {
    os << "Floor " << c.floor << " by " << c.author->Name() << ": " << c.content;
    return os;
}