#include "models/action.h"
#include "models/clients.h" // 必须包含完整的定义以访问 posts/posts.fake_remove
#include "models/Post.h"
#include "models/comment.h"
#include "manager/undo_manager.h"

// 基类初始化
void Action::init(Client* c, bool add, Post* p) {
    this->client = c;
    is_add = add;
    post = p;
    if (p) {
        index = p->get_idex();
    }
}

// --- PostAction 实现 ---

void PostAction::clean(Client* client_context) {
    if (!post || post_node == nullptr) return;

    // 这里的逻辑是：操作被挤出栈了，且这是一个"删帖操作"(is_add=0)，
    // 意味着帖子节点被临时保存在 post_node 中，现在彻底不需要了，必须释放内存。
    // 如果是"发帖操作"(is_add=1)，节点在链表中，不需要我们删。
    
    // 修正逻辑：原代码意图似乎是清理 悬挂 的节点
    // 如果 is_add=false (删帖)，post_node 指向被 fake_remove 的节点，必须 delete
    // 如果 is_add=true (发帖)，post_node 指向链表中的节点。如果 clean 被调用，说明这个发帖操作太久远了，不能撤销了。
    // 此时帖子本身还在链表中，不能 delete post_node。

    // 但是，如果 post 已经被 UndoManager 标记销毁了呢？
    // 保持原版逻辑的简化版，只做最基础的安全清理
    if (!is_add) { // 如果是当初执行了删除操作，现在连撤销记录都要删了，那就真的删了吧
        if (post_node) {
            delete post_node; 
            post_node = nullptr;
        }
    }
}

bool PostAction::undo() {
    if (!check() || !client) {
        cout << "[撤销失败] 帖子对象已失效。" << endl;
        used = true;
        return false;
    }

    if (is_add) { 
        // 原操作是：发帖
        // 撤销逻辑：删帖 (从链表中移除，但不 delete，因为以后可能 Redo，或者仅仅是 fake_remove)
        // 注意：这里我们执行真删除的逻辑（fake_remove），并将节点保存在 post_node 中供以后恢复？
        // 不，根据原设计，undo 后 Action 也就结束了（除非做 redo）。
        // 这里简单处理：从链表中移除，并 delete 节点（因为没有实现 redo）
        
        int index_to_remove = -1;
        for (int i = 0; i < client->posts.size(); i++) {
            if (&client->posts[i] == post) {
                index_to_remove = i;
                break;
            }
        }

        if (index_to_remove != -1) {
            // fake_remove 返回节点指针，我们负责 delete，因为这是"撤销发帖"
            ListNode<Post>* node = client->posts.fake_remove(index_to_remove);
            UndoManager::instance().notify_post_destroyed(post);
            delete node; // 彻底删除
            post = nullptr;
            post_node = nullptr;
        }
        return true;

    } else {
        // 原操作是：删帖
        // 撤销逻辑：恢复帖子 (将节点插回链表)
        if (post_node) {
            client->posts.auto_insert(post_node);
            // 恢复后，需要重新注册 UndoManager? 
            // 不，UndoManager 里的记录可能被清除了，但因为 post 指针没变，如果之前没彻底清理，可能还有效
            // 简单起见，恢复即可
            return true;
        }
        return false;
    }
}

// --- LikeAction 实现 ---

void LikeAction::clean(Client* client_context) {
    if (post) {
        UndoManager::instance().unregister_action_self(post, this);
    }
}

bool LikeAction::undo() {
    if (!check() || !client) {
        cout << "[撤销失败] 帖子已不存在。" << endl;
        used = true;
        return false;
    }

    // 直接操作数据，不调用 Post::receive_likes
    LinkList<Client*>& likes_list = post->get_likes_list();

    if (is_add) {
        // 原操作：点赞
        // 撤销：取消点赞 (移除)
        for (int i = 0; i < likes_list.size(); i++) {
            if (likes_list[i] && likes_list[i]->ID() == client->ID()) {
                likes_list.remove(i);
                post->decrement_likes();
                
                // 注销引用
                UndoManager::instance().unregister_action_self(post, this);
                used = true;
                return true;
            }
        }
    } else {
        // 原操作：取消点赞
        // 撤销：重新点赞 (添加)
        // 查重防止重复添加
        bool exists = false;
        for(int i=0; i<likes_list.size(); ++i) {
            if(likes_list[i]->ID() == client->ID()) exists = true;
        }
        if (!exists) {
            likes_list.add(client);
            post->increment_likes();
            UndoManager::instance().unregister_action_self(post, this); // 这里的逻辑根据原设计可能不需要，但为了对称
            used = true;
            return true;
        }
    }
    return false;
}

// --- CommentAction 实现 ---

void CommentAction::clean(Client* client_context) {
    if (!post || !comment_node) return;
    
    if (!is_add) { // 如果是删评操作被清理，彻底删除节点
        delete comment_node;
        comment_node = nullptr;
    }
}

bool CommentAction::undo() {
    if (!check() || !client) {
        cout << "[撤销失败] 帖子已不存在。" << endl;
        used = true;
        return false;
    }

    if (is_add) {
        // 原操作：发评论
        // 撤销：删评论
        int index_to_remove = -1;
        // 只能通过楼层号匹配，因为指针可能变了? 不，comment_node->data 是对象
        // 我们用楼层号找
        int target_floor = comment_node->data.floor();
        
        for (int i = 0; i < post->comment_list.size(); ++i) {
            if (post->comment_list[i].floor() == target_floor) {
                index_to_remove = i;
                break;
            }
        }

        if (index_to_remove != -1) {
            ListNode<Comment>* node = post->comment_list.fake_remove(index_to_remove);
            if (node) {
                // 评论者的计数减一 (注意: 这里的 client 是发评论的人)
                client->receive_comment(false);
                
                UndoManager::instance().unregister_action_self(post, this);
                delete node; // 彻底删除
            }
            return true;
        }

    } else {
        // 原操作：删评论
        // 撤销：恢复评论
        if (comment_node) {
            post->comment_list.auto_insert(comment_node);
            client->receive_comment(true); // 计数加一
            used = true;
            return true;
        }
    }
    return false;
}