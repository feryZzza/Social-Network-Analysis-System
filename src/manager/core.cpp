#include "manager/core.h"
#include "data_structure/huffman.h" // 引入哈夫曼模块
#include <iostream>
#include <iomanip> // 用于格式化输出

using namespace std;

bool Core::loadData() {//调用文件管理器加载数据
    return FileManager::instance().load(all_clients);
}

bool Core::saveData() {//调用文件管理器保存数据
    return FileManager::instance().save(all_clients);
}

Client* Core::getClientById(const std::string& id) {//通过ID查找用户，后续把这个改成哈希表和查找树，接上培宁的代码
    for (int i = 0; i < all_clients.size(); ++i) {
        if (all_clients[i].ID() == id) {
            return &all_clients[i];
        }
    }
    return nullptr;
}

CoreStatus Core::registerClient(const std::string& name, const std::string& id, const std::string& password) {
    //添加新用户
    if (getClientById(id)) {
        return ERR_CLIENT_EXISTS;
    }
    if (all_clients.full()) {
        return ERR_LIST_FULL;
    }
    Client newClient(name, id, password);
    all_clients.add(newClient);
    return SUCCESS;
}

void Core::pushAction(Client* client, Action* action) {//将操作压入用户的撤销栈，若栈满则删除最早的操作记录
    if (client->a_stack_full()) {
        cout << "操作栈已满，自动删除最早的操作记录。" << endl;
        Action* temp = client->add_action(action); // 压入新操作，弹出旧操作
        if (temp) {
            temp->clean(client); // 清理旧操作占用的资源
            delete temp;
        }
    } else {
        client->add_action(action);
    }
}

void Core::sendMessage(Client* sender, Client* receiver, Post* post, massege* msg) {
    //添加消息，添加到接收者的消息队列中
    if (receiver) {
        receiver->receive_messege(msg);
    }
}

CoreStatus Core::userAddPost(Client* client, const std::string& title, const std::string& content) {
    // 用户发帖
    if (!client) return ERR_CLIENT_NOT_FOUND;
    Post p(title, content);
    p.set_author(client);

    client->incrementPostTime(); 
    p.set_idex(client->PostTime());//
    
    client->posts.add(p);
    
    //将发帖操作注册到撤销管理器，并压入用户的撤销栈
    ListNode<Post>* p_pointer = client->posts.tail_ptr();
    PostAction* action = new PostAction(p_pointer);
    action->init(client, true, &p_pointer->data);
    //注册到撤销管理器
    UndoManager::instance().register_action(action->post, action);
    // 压入用户撤销栈
    pushAction(client, action);

    return SUCCESS;
}

CoreStatus Core::userDeletePost(Client* client, int postIndex) {
    if (!client) return ERR_CLIENT_NOT_FOUND;
    
    ListNode<Post>* node = client->posts.fake_remove(postIndex);
    if (!node) return ERR_POST_NOT_FOUND;

    PostAction* action = new PostAction(node);
    action->init(client, false, &node->data); // false 表示删除操作

    pushAction(client, action);
    
    return SUCCESS;
}

CoreStatus Core::userDeletePost(Client* client, Post* post) {
    if (!client || !post) return ERR_POST_NOT_FOUND;
    
    for(int i = 0; i < client->posts.size(); i++){
        if(&client->posts[i] == post){
            return userDeletePost(client, i);
        }
    }
    return ERR_POST_NOT_FOUND;
}

CoreStatus Core::userAddComment(Client* commenter, Post* post, const std::string& content, int reply_floor) {
    if (!commenter) return ERR_CLIENT_NOT_FOUND;
    if (!post) return ERR_POST_NOT_FOUND;

    // 1. 构建评论
    Comment c(commenter, content, reply_floor);
    int new_floor = post->get_floor() + 1;
    c.set_floor(new_floor);
    
    // 2. 数据层添加
    post->comment_list.add(c);
    post->set_floor(new_floor);

    commenter->receive_comment(true);

    // 3. 注册撤销操作
    CommentAction* action = new CommentAction(post->comment_list.tail_ptr());
    action->init(commenter, true, post);
    UndoManager::instance().register_action(post, action);
    pushAction(commenter, action);

    // 4. 发送消息
    CommentMassege* m = new CommentMassege(&post->comment_list.tail_ptr()->data);
    m->init(commenter, post->author, post);
    sendMessage(commenter, post->author, post, m);

    return SUCCESS;
}

CoreStatus Core::userDeleteComment(Client* client, Post* post, int floor) {
    if (!post) return ERR_POST_NOT_FOUND;

    for(int i = 0; i < post->comment_list.size(); i++){
        if(post->comment_list[i].floor() == floor){
            ListNode<Comment>* node = post->comment_list.fake_remove(i);
            if(!node) return ERR_UNKNOWN;

            CommentAction* action = new CommentAction(node);
            action->init(client, false, post); // 删除
            
            pushAction(client, action);
            
            // 被评论数减少
            client->receive_comment(false);
            return SUCCESS;
        }
    }
    return ERR_COMMENT_NOT_FOUND;
}

CoreStatus Core::userLikePost(Client* liker, Post* post) {
    if (!liker || !post) return ERR_POST_NOT_FOUND;

    // 检查是否已赞
    for(int i = 0; i < post->get_likes_list().size(); i++){
        Client* existing = post->get_likes_list()[i];
        if(existing && existing->ID() == liker->ID()){
            // 已赞，执行取消赞逻辑
            post->get_likes_list().remove(i);
            post->decrement_likes();
            
            // 注册取消赞的操作 (Undo后变回赞)
            LikeAction* action = new LikeAction();
            action->init(liker, false, post); 
            UndoManager::instance().register_action(post, action);
            pushAction(liker, action);
            return SUCCESS;
        }
    }

    // 未赞，执行点赞逻辑
    post->get_likes_list().add(liker);
    post->increment_likes();

    LikeAction* action = new LikeAction();
    action->init(liker, true, post);
    UndoManager::instance().register_action(post, action);
    pushAction(liker, action);

    // 发送消息
    LikeMassege* m = new LikeMassege();
    m->init(liker, post->author, post);
    sendMessage(liker, post->author, post, m);

    return SUCCESS;
}

CoreStatus Core::userUndo(Client* client) {
    if (client->action_num() == 0) {
        return ERR_NO_ACTION_TO_UNDO;
    }
    
    Action* a = client->pop_action(); // 从栈中取出
    if (a->undo()) {
        delete a;
        return SUCCESS;
    } else {
        delete a;
        return ERR_ACTION_INVALID;
    }
}

void Core::userReadMessages(Client* client) {
    if (!client) return;
    LinkQueue<massege*>& q = client->getMessages();
    if (q.empty()) {
        cout << "没有新消息。" << endl;
        return;
    }
    while (!q.empty()) {
        massege* m = q.dequeue();
        m->show();
        delete m;
    }
}

//内嵌哈夫曼编码对帖子内容进行深度分析，展示压缩效果
void Core::analyzePostContent(Post* post) {
    if (!post) {
        cout << "[错误] 帖子不存在，无法分析。" << endl;
        return;
    }

    cout << "\n=============================================" << endl;
    cout << "       哈夫曼压缩分析 (Huffman Analysis)" << endl;
    cout << "=============================================" << endl;
    cout << "目标帖子: " << post->get_title() << endl;

    string fullText = "";// 拼接标题和正文及评论内容
    fullText += post->get_title();   // 标题
    fullText += post->get_content(); // 正文
    
    for (int i = 0; i < post->comment_list.size(); ++i) {
        fullText += post->comment_list[i].get_content();
    }

    if (fullText.empty()) {
        cout << "[提示] 帖子内容为空，无需分析。" << endl;
        return;
    }

    cout << "待分析原始文本长度: " << fullText.length() << " Bytes (不含元数据)" << endl;
    cout << "正在构建哈夫曼树..." << endl;

    //统计频率
    SeqList<FreqPair> freqs(512); 
    countFrequency(fullText, freqs);

    // 构建哈夫曼树
    HuffmanTree huffTree(freqs);
    huffTree.generateCodes();

    // 展示编码表
    huffTree.printCodes();

    // 压缩内容
    string compressed = huffTree.compress(fullText);
    
    // 展示结果
    cout << "\n[压缩结果]" << endl;
    // 如果二进制串太长，截断显示
    if (compressed.length() > 100) {
        cout << "二进制流 (前100位): " << compressed.substr(0, 100) << "..." << endl;
    } else {
        cout << "二进制流: " << compressed << endl;
    }

    // 计算压缩率
    // 原始大小：fullText.length() * 8 bits (假设 ASCII/UTF-8 基础单元)
    // 压缩大小：compressed.length() bits (0/1 字符串的长度即 bit 数)
    double originalBits = fullText.length() * 8.0;
    double compressedBits = compressed.length();
    double ratio = (1.0 - compressedBits / originalBits) * 100.0;

    cout << "原始大小: " << originalBits << " bits" << endl;
    cout << "压缩大小: " << compressedBits << " bits" << endl;
    cout << "压缩率: " << fixed << setprecision(2) << ratio << "% (节省空间)" << endl;
    cout << "WPL (带权路径长度): " << huffTree.getWPL() << endl;

    // 6. 解压验证
    string decompressed = huffTree.decompress(compressed);
    if (decompressed == fullText) {
        cout << "[验证] 解压成功！解压后内容与原文完全一致。" << endl;
    } else {
        cout << "[错误] 解压失败！内容不匹配。" << endl;
    }
    cout << "=============================================" << endl;
}