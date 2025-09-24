#include "models/action.h"

PostAction::~PostAction(){
    if(is_add){
        //如果是添加操作，删除该帖子
        delete post;
        post = nullptr;
    }
    delete post;
}