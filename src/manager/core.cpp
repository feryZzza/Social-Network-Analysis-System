#include "manager/core.h"
#include "models/clients.h"
#include <iostream>
#include <iomanip> // 用于格式化输出

using namespace std;

bool Core::loadData() {//调用文件管理器加载数据

    bool success = FileManager::instance().load(all_clients, social_net);
    if(success) {
        rebuildIndex(); // 别忘了重建索引
    }
    return success;
}

bool Core::saveData() {//调用文件管理器保存数据
    return FileManager::instance().save(all_clients, social_net);
}

void Core::rebuildIndex() {
    // 遍历 all_clients 插入索引
    for(int i = 0; i < all_clients.size(); ++i) {
        // 取出线性表中对象的地址
        Client* ptr = &all_clients[i];
        // 使用指针构造，会自动提取 Name
        client_index.insert(AVL_node(ptr));
    }
}

int Core::getClientIndex(Client* c) {
    if (!c) return -1;
    // 获取数组首地址
    Client* base = &all_clients[0];
    // 计算偏移量
    long long diff = c - base; 
    // 检查是否在合法范围内
    if (diff >= 0 && diff < all_clients.size()) {
        return (int)diff;
    }
    return -1;
}

Client* Core::getClientByName(const std::string& name) {
    // 构造一个临时的 AVL_node 用于查找
    AVL_node target(name, nullptr);
    
    // 使用 AVL 树进行查找
    TreeNode<AVL_node>* node = client_index.find(target);
    
    if (node) {
        // 找到后返回存储的真实指针
        return node->data.client;
    }
    // 没找到
    return nullptr;
}

CoreStatus Core::registerClient(const std::string& name, const std::string& id, const std::string& password) {
    //添加新用户
    if (getClientByName(name)) {
        return ERR_CLIENT_EXISTS;
    }
    if (all_clients.full()) {
        return ERR_LIST_FULL;
    }
    Client newClient(name, id, password);
    all_clients.add(newClient);

    // 插入索引,更新平衡树
    Client* ptr = &all_clients[all_clients.size() - 1];
    client_index.insert(AVL_node(ptr));
    
    
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

    // 构建评论
    Comment c(commenter, content, reply_floor);
    int new_floor = post->get_floor() + 1;
    c.set_floor(new_floor);
    // 数据层添加
    post->comment_list.add(c);
    post->set_floor(new_floor);

    commenter->receive_comment(true);

    //注册撤销操作
    CommentAction* action = new CommentAction(post->comment_list.tail_ptr());
    action->init(commenter, true, post);
    UndoManager::instance().register_action(post, action);
    pushAction(commenter, action);

    //发送消息
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
        // 修改: 比较 Name 而不是 ID
        if(existing && existing->Name() == liker->Name()){
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

CoreStatus Core::makeFriend(Client* a, Client* b) {
    if (!a || !b) return ERR_CLIENT_NOT_FOUND;
    if (a == b) return ERR_SELF_FRIEND;

    int idxA = getClientIndex(a);
    int idxB = getClientIndex(b);

    if (idxA == -1 || idxB == -1) return ERR_UNKNOWN;

    social_net.addEdge(idxA, idxB);
    this->all_clients[idxA].make_friend(true);
    this->all_clients[idxB].make_friend(true);
    
    return SUCCESS;
}

CoreStatus Core::deleteFriend(Client* a, Client* b) {
    if (!a || !b) return ERR_CLIENT_NOT_FOUND;
    if (a == b) return ERR_SELF_FRIEND;

    int idxA = getClientIndex(a);
    int idxB = getClientIndex(b);

    if (idxA == -1 || idxB == -1) return ERR_UNKNOWN;

    social_net.removeEdge(idxA, idxB);
    this->all_clients[idxA].make_friend(false);
    this->all_clients[idxB].make_friend(false);
    
    return SUCCESS;
}

// 计算社交距离
int Core::getRelationDistance(Client* a, Client* b) {
    if (!a || !b) return -1;
    if (a == b) return 0;

    int idxA = getClientIndex(a);
    int idxB = getClientIndex(b);
    
    if (idxA == -1 || idxB == -1) return -1;

    LinkList<int> path;
    if (social_net.shortestPath(idxA, idxB, path)) {
        return path.size() - 1;
    }
    
    return -1; // 不连通
}


//内嵌哈夫曼编码对帖子内容进行深度分析，展示压缩效果
void Core::analyzePostContent(Post* post) {
    cout << getHuffmanAnalysisResult(post) << endl;
}

// GUI 版哈夫曼分析，返回结果字符串
string Core::getHuffmanAnalysisResult(Post* post) {
    if (!post) {
        return "[错误] 帖子不存在，无法分析。";
    }

    stringstream ss;
    ss << "=============================================\n";
    ss << "       哈夫曼压缩分析 (Huffman Analysis)\n";
    ss << "=============================================\n";
    ss << "目标帖子: " << post->get_title() << "\n\n";

    string fullText = "";// 拼接标题和正文及评论内容
    fullText += post->get_title();   // 标题
    fullText += post->get_content(); // 正文
    
    for (int i = 0; i < post->comment_list.size(); ++i) {
        fullText += post->comment_list[i].get_content();
    }

    if (fullText.empty()) {
        return "[提示] 帖子内容为空，无需分析。";
    }

    ss << "待分析原始文本长度: " << fullText.length() << " Bytes\n";
    ss << "正在构建哈夫曼树...\n\n";

    //统计频率
    SeqList<FreqPair> freqs(512); 
    countFrequency(fullText, freqs);

    // 构建哈夫曼树
    HuffmanTree huffTree(freqs);
    huffTree.generateCodes();

    // 压缩内容
    string compressed = huffTree.compress(fullText);
    
    // 展示结果
    ss << "[压缩结果]\n";
    if (compressed.length() > 500) {
        ss << "二进制流 (前500位): " << compressed.substr(0, 500) << "...\n";
    } else {
        ss << "二进制流: " << compressed << "\n";
    }
    ss << "\n";

    // 计算压缩率
    double originalBits = fullText.length() * 8.0;
    double compressedBits = compressed.length();
    double ratio = (1.0 - compressedBits / originalBits) * 100.0;

    ss << fixed << setprecision(2);
    ss << "原始大小: " << (int)originalBits << " bits\n";
    ss << "压缩大小: " << (int)compressedBits << " bits\n";
    ss << "压缩率: " << ratio << "% (节省空间)\n";
    ss << "WPL (带权路径长度): " << huffTree.getWPL() << "\n\n";

    // 解压验证
    string decompressed = huffTree.decompress(compressed);
    if (decompressed == fullText) {
        ss << "✅ [验证] 解压成功！解压后内容与原文完全一致。\n";
    } else {
        ss << "❌ [错误] 解压失败！内容不匹配。\n";
    }
    ss << "=============================================";

    return ss.str();
}

// 实现排行榜功能

void Core::showUserRanking() {
    if (all_clients.size() == 0) {
        cout << "[系统提示] 当前没有任何用户数据。" << endl;
        return;
    }
    // 使用内置的成员变量 sorter，而不是重复实例化
    sorter.haoyoushu(all_clients);
}

void Core::showHotPostRanking() {
    if (all_clients.size() == 0) {
        cout << "[系统提示] 当前没有任何用户数据。" << endl;
        return;
    }
    // 使用内置的成员变量 sorter，而不是重复实例化
    sorter.dianzanshu(all_clients);
}