#include "models/action.h"
#include "models/comment.h"


bool LikeAction::undo() {//主动从栈中弹出点赞操作并撤销
    //点赞变为取消点赞，取消点赞变为点赞，只需调用receive_likes函数即可
    if(post == nullptr) return false;
    //说明原操作的帖子已经被彻底删除，无法撤销，这时调用撤回会提示错误，并选择是否删除栈中的该操作
    else{
        post->receive_likes(client);//传入nullptr表示是撤销操作
        return true;
    }
}