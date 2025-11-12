#include "models/action.h"
#include "manager/undo_manager.h"
#include "models/comment.h"


void Action::init(Client* c,bool add,Post* p){//初始化操作
    this->client = c;
    is_add=add;
    index = p->get_idex();
    post = p;
}//初始化



PostAction::~PostAction(){
    if(!check())return;
    if(!used){//操作未被撤销，说明帖子仍然存在，彻底删除帖子前检查涉及到的操作栈，防止野指针
        if(!is_add){//删帖操作，彻底删除帖子

            delete post_node;//删除该帖子
        }
    }
}

bool PostAction::undo() {//主动从栈中弹出发帖操作并撤销
    if(!check()){
        cout<<"该操作涉及的帖子已被删除，无法撤销"<<endl;
        used = true;
        return false;
    }else{
        if(is_add){//发帖操作，撤销即删帖
            client->deletePost(post);//假删除帖子
            //通知其他操作栈中的该帖子指针被删除，防止野指针
            UndoManager::instance().notify_post_destroyed(post);
            cout<<post_node->data<<endl;
            delete post_node;//删除该帖子
            post_node = nullptr;
            cout<<"撤销成功"<<endl;
            used = true;
            return true;
        }else{//删帖操作，撤销即恢复帖子
            client->posts.auto_insert(post_node);//将帖子按序插入帖子列表
            used = true;
        }
        return true;
    }
}

bool LikeAction::undo() {//主动从栈中弹出点赞操作并撤销
    //点赞变为取消点赞，取消点赞变为点赞，只需调用receive_likes函数即可
    if(!check()){//说明原操作的帖子已经被彻底删除，无法撤销，这时调用撤回会提示错误，并选择是否删除栈中的该操作
        cout<<"该次点赞涉及的帖子已被删除，无法撤销"<<endl;
        post = nullptr;
        used = true;
        return false;
    }else{
        post->receive_likes(client,true);
        return true;
    }
}

CommentAction::~CommentAction(){
    if(!check())return;
    if(!used){//操作未被撤销，说明帖子仍然存在，彻底删除帖子前检查涉及到的操作栈，防止野指针
        if(!is_add){//删评论操作，彻底删除评论
            delete comment_node;//删除该评论
        }
    }
}

bool CommentAction::undo() {//主动从栈中弹出评论操作并撤销
    if(!check()){
        cout<<"该评论涉及的帖子已被删除，无法撤销"<<endl;
        used = true;
        return false;
    }else{
        if(is_add){//评论操作，撤销即删评论
            post->author->deleteComment(post,comment_node->data.floor());//通过楼层删除评论
            used = true;
            return true;
        }else{//删评论操作，撤销即恢复评论
            post->comment_list.auto_insert(comment_node);//将评论按序插入评论列表
            client->receive_comment(1);//被评论数加一
            used = true;
        }
        return true;
    }
}
