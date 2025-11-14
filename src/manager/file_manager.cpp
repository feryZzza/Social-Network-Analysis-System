#include "manager/file_manager.h"
#include "models/clients.h"
#include "models/Post.h"
#include "models/comment.h"
#include "data_structure/lin_list.h" // 明确包含我们自己的数据结构
#include <fstream>
#include <string>

using namespace std;

// 写入字符串：先写长度，再写内容
void writeString(ofstream& out, const string& s) {
    size_t len = s.length();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(s.c_str(), len);
}

// 读取字符串：先读长度，再读内容 (不使用 std::vector)
void readString(ifstream& in, string& s) {
    size_t len;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (in.eof() || len <= 0) { // 添加 eof 检查和长度检查
        s = "";
        return;
    }
    // 创建一个临时缓冲区
    char* buffer = new char[len + 1];
    in.read(buffer, len);
    buffer[len] = '\0'; // 确保字符串以 null 结尾
    s = buffer; // 赋值给 std::string
    delete[] buffer; // 释放缓冲区
}


// 用于在加载后重建指针的数据结构
struct PostLoadInfo {
    string global_post_id; // 帖子的唯一ID (格式: ClientID_PostIDex)
    LinkList<string> liker_ids; // 点赞者ID列表
    LinkList<string> comment_author_ids; // 评论者ID列表 (按顺序)

    // 为 LinkList 搜索提供比较，不写==重载过不了编译
    bool operator==(const PostLoadInfo& other) const {
        return global_post_id == other.global_post_id;
    }
};

// 用于在加载时临时存储所有需要链接的信息
// 我们把它放在静态变量里，这样 load 和 reconstructPointers 都可以访问
static LinkList<PostLoadInfo> posts_to_link_list;

// 重建指针的辅助函数 (不使用 std::map)
bool FileManager::reconstructPointers(SeqList<Client>& clients) {
    if (posts_to_link_list.empty()) {
        // 如果没有需要链接的数据 (比如是新系统或加载失败)，直接返回
        return true; 
    }

    // 遍历所有 Client 和 Post，开始重建指针
    for (int i = 0; i < clients.size(); ++i) {
        Client& client = clients[i];
        for (int j = 0; j < client.posts.size(); ++j) {
            Post& post = client.posts[j];
            // 用加载时生成的同一个 global_post_id 来查找
            string global_post_id = client.ID() + "_" + to_string(post.get_idex());
            
            // 在 LinkList 中查找此 post 的加载信息 (线性查找)
            PostLoadInfo* info = nullptr;
            for (int k = 0; k < posts_to_link_list.size(); ++k) {
                if (posts_to_link_list[k].global_post_id == global_post_id) {
                    info = &posts_to_link_list[k];
                    break;
                }
            }

            // 如果没找到，跳过这篇 post
            if (!info) continue;

            // 3a. 链接点赞列表 (likes_list)
            for (int k = 0; k < info->liker_ids.size(); ++k) {
                string& liker_id = info->liker_ids[k];
                
                // 4a. 查找 liker_id 对应的 Client* (嵌套循环 O(N) 查找)
                Client* liker_client = nullptr;
                for (int m = 0; m < clients.size(); ++m) {
                    if (clients[m].ID() == liker_id) {
                        liker_client = &clients[m];
                        break;
                    }
                }
                
                if (liker_client) {
                    post.get_likes_list().add(liker_client);
                }
            }

            // 3b. 链接评论作者 (comment.author)
            for (int k = 0; k < post.comment_list.size(); ++k) {
                if (k >= info->comment_author_ids.size()) break; // 防止越界

                string& author_id = info->comment_author_ids[k];
                
                // 4b. 查找 author_id 对应的 Client* (嵌套循环 O(N) 查找)
                Client* author_client = nullptr;
                for (int m = 0; m < clients.size(); ++m) {
                    if (clients[m].ID() == author_id) {
                        author_client = &clients[m];
                        break;
                    }
                }

                if (author_client) {
                    post.comment_list[k].set_author(author_client);
                }
            }
        }
    }

    posts_to_link_list.~LinkList(); // 清理临时数据
    new (&posts_to_link_list) LinkList<PostLoadInfo>(); // 重建
    
    cout << "[核心日志] 指针重建完成。" << endl;
    return true;
}


bool FileManager::save(SeqList<Client>& clients) {
    ofstream out_file(file, ios::binary | ios::trunc); // 每次都覆盖写入
    if (!out_file.is_open()) {
        cerr << "[错误] 无法打开文件进行保存: " << file << endl;
        return false;
    }

    try {
        // 1. 写入 Client 数量
        int client_count = clients.size();
        out_file.write(reinterpret_cast<const char*>(&client_count), sizeof(client_count));

        // 2. 遍历并写入每个 Client
        for (int i = 0; i < client_count; ++i) {
            Client& client = clients[i];
            
            // 写入 Client 基础数据
            writeString(out_file, client.Name());
            writeString(out_file, client.ID());
            writeString(out_file, client.Password()); // (需要 getPassword())
            int post_time = client.PostTime(); // (需要 getPostTime())
            out_file.write(reinterpret_cast<const char*>(&post_time), sizeof(post_time));

            // 3. 写入 Post 数量
            int post_count = client.posts.size();
            out_file.write(reinterpret_cast<const char*>(&post_count), sizeof(post_count));

            // 4. 遍历并写入每个 Post
            for (int j = 0; j < post_count; ++j) {
                Post& post = client.posts[j];

                // 写入 Post 基础数据
                int idex = post.get_idex();
                int floor = post.get_floor(); // (需要 get_floor())
                out_file.write(reinterpret_cast<const char*>(&idex), sizeof(idex));
                out_file.write(reinterpret_cast<const char*>(&floor), sizeof(floor));
                writeString(out_file, post.get_title());
                writeString(out_file, post.get_content()); // (需要 get_content())

                // 5. 写入 Comment 数量
                int comment_count = post.comment_list.size();
                out_file.write(reinterpret_cast<const char*>(&comment_count), sizeof(comment_count));
                
                // 6. 遍历并写入每个 Comment
                for (int k = 0; k < comment_count; ++k) {
                    Comment& comment = post.comment_list[k];
                    
                    int c_floor = comment.floor();
                    int c_reply_floor = comment.get_comment_floor(); // (需要 get_comment_floor())
                    
                    out_file.write(reinterpret_cast<const char*>(&c_floor), sizeof(c_floor));
                    out_file.write(reinterpret_cast<const char*>(&c_reply_floor), sizeof(c_reply_floor));
                    writeString(out_file, comment.get_content());
                    
                    // --- 关键点：写入作者ID，而不是指针！ ---
                    string author_id = comment.get_author() ? comment.get_author()->ID() : "";
                    writeString(out_file, author_id);
                }

                // 7. 写入 Likes 数量
                LinkList<Client*>& likes_list = post.get_likes_list(); // (需要 get_likes_list())
                int like_count = likes_list.size();
                out_file.write(reinterpret_cast<const char*>(&like_count), sizeof(like_count));

                // 8. 遍历并写入每个 Liker 的 ID
                for (int k = 0; k < like_count; ++k) {
                    // --- 关键点：写入点赞者ID，而不是指针！ ---
                    string liker_id = likes_list[k] ? likes_list[k]->ID() : "";
                    writeString(out_file, liker_id);
                }
            }
        }
    } catch (const exception& e) {
        cerr << "[错误] 保存期间发生异常: " << e.what() << endl;
        out_file.close();
        return false;
    }

    out_file.close();
    // cout << "[成功] 数据已保存到 " << file << endl; // 这个日志移到 main.cpp 中
    return true;
}

bool FileManager::load(SeqList<Client>& clients) {
    ifstream in_file(file, ios::binary);
    if (!in_file.is_open()) {
        // cout << "[提示] 未找到保存文件 " << file << "。将启动一个新系统。" << endl;
        return false; // 这不是一个错误，只是文件不存在
    }

    // 确保开始加载前，临时 list 是空的
    posts_to_link_list.~LinkList();
    new (&posts_to_link_list) LinkList<PostLoadInfo>();
    
    // 确保 clients 列表是空的
    clients.~SeqList(); // 析构旧列表 (如果里面有东西)
    new (&clients) SeqList<Client>(100); // 重新构造空列表

    try {
        // 1. 读取 Client 数量
        int client_count = 0;
        in_file.read(reinterpret_cast<char*>(&client_count), sizeof(client_count));
        if (in_file.fail() || client_count < 0 || client_count > 10000) { // 添加一个合理性检查
             cout << "[提示] 文件为空或已损坏。" << endl;
             in_file.close();
             return false;
        }

        // 2. 遍历并读取每个 Client
        for (int i = 0; i < client_count; ++i) {
            string name, id, password;
            int post_time;
            
            readString(in_file, name);
            readString(in_file, id);
            readString(in_file, password);
            in_file.read(reinterpret_cast<char*>(&post_time), sizeof(post_time));
            
            Client client(name, id, password);
            client.setPostTime(post_time); // (需要 setPostTime())

            // 3. 读取 Post 数量
            int post_count = 0;
            in_file.read(reinterpret_cast<char*>(&post_count), sizeof(post_count));

            // 4. 遍历并读取每个 Post
            for (int j = 0; j < post_count; ++j) {
                int idex, floor;
                string title, content;

                in_file.read(reinterpret_cast<char*>(&idex), sizeof(idex));
                in_file.read(reinterpret_cast<char*>(&floor), sizeof(floor));
                readString(in_file, title);
                readString(in_file, content);

                Post post(title, content);
                post.set_idex(idex);
                post.set_floor(floor); // (需要 set_floor())
                post.set_author(&client); // 临时设置作者为当前 client

                // 生成唯一ID，用于重建
                string global_post_id = client.ID() + "_" + to_string(idex);
                PostLoadInfo post_info;
                post_info.global_post_id = global_post_id;

                // 5. 读取 Comment 数量
                int comment_count = 0;
                in_file.read(reinterpret_cast<char*>(&comment_count), sizeof(comment_count));

                // 6. 遍历并读取每个 Comment
                for (int k = 0; k < comment_count; ++k) {
                    int c_floor, c_reply_floor;
                    string c_content, c_author_id;
                    
                    in_file.read(reinterpret_cast<char*>(&c_floor), sizeof(c_floor));
                    in_file.read(reinterpret_cast<char*>(&c_reply_floor), sizeof(c_reply_floor));
                    readString(in_file, c_content);
                    readString(in_file, c_author_id);
                    
                    // --- 关键点：此时 author 传 nullptr，稍后重建 ---
                    Comment comment(nullptr, c_content, c_reply_floor);
                    comment.set_floor(c_floor);
                    
                    post.comment_list.add(comment); // 添加到 Post
                    post_info.comment_author_ids.add(c_author_id); // 存储 ID 用于重建
                }

                // 7. 读取 Likes 数量
                int like_count = 0;
                in_file.read(reinterpret_cast<char*>(&like_count), sizeof(like_count));

                // 8. 遍历并读取 Liker ID
                for (int k = 0; k < like_count; ++k) {
                    string liker_id;
                    readString(in_file, liker_id);
                    post_info.liker_ids.add(liker_id); // 存储 ID 用于重建
                }
                
                client.posts.add(post); // 将 Post 添加到 Client
                posts_to_link_list.add(post_info); // 存储此 Post 的链接信息
            }
            
            clients.add(client); // 将 Client 添加到主列表
        }
    } catch (const exception& e) {
        cerr << "[错误] 加载期间发生异常: " << e.what() << endl;
        in_file.close();
        clients.~SeqList(); // 清理已部分加载的数据
        new (&clients) SeqList<Client>(100); // 重建为空列表
        posts_to_link_list.~LinkList(); // 清理临时 list
        new (&posts_to_link_list) LinkList<PostLoadInfo>();
        return false;
    }

    in_file.close();
    
    // --- 关键一步：调用重建函数 ---
    reconstructPointers(clients); 

    // cout << "[成功] 数据已从 " << file << " 加载。" << endl; // 移到 main.cpp 中
    return true;
}