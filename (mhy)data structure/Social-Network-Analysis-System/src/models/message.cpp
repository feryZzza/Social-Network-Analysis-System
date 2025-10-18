#include "models/message.h"

bool LikeMassege::show(){//显示消息
    cout<<"[系统消息] "<<sender->Name()<<" 赞了你的帖子 \""<<post->get_title()<<"\""<<endl;
    return true;
}

bool CommentMassege::show(){//显示消息
    cout<<"[系统消息] "<<sender->Name()<<" 评论了你的帖子 \""<<post->get_title()<<"\" : "<<comment->get_content()<<endl;
    return true;
}