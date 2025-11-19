#include "manager/file_manager.h"
#include "models/clients.h"
#include "models/Post.h"
#include "models/comment.h"
#include <fstream>
#include <sstream>

using namespace std;

// 简单的 JSON 字符串转义
string FileManager::escapeJsonString(const string& s) {
    string res = "";
    for (char c : s) {
        if (c == '"') res += "\\\"";
        else if (c == '\\') res += "\\\\";
        else if (c == '\n') res += "\\n";
        else if (c == '\t') res += "\\t";
        else res += c;
    }
    return res;
}

// 简单的 JSON 字符串反转义
string FileManager::unescapeJsonString(const string& s) {
    string res = "";
    for (size_t i = 0; i < s.length(); ++i) {
        if (s[i] == '\\' && i + 1 < s.length()) {
            char next = s[i+1];
            if (next == '"') res += '"';
            else if (next == '\\') res += '\\';
            else if (next == 'n') res += '\n';
            else if (next == 't') res += '\t';
            else res += next; 
            i++; 
        } else {
            res += s[i];
        }
    }
    return res;
}

// 辅助查找 Client
Client* FileManager::findClient(SeqList<Client>& clients, const std::string& id) {
    for(int i = 0; i < clients.size(); ++i) {
        if(clients[i].ID() == id) return &clients[i];
    }
    return nullptr;
}

// 辅助查找 Post (globalId 格式: "clientId_postIndex")
Post* FileManager::findPost(SeqList<Client>& clients, const std::string& globalId) {
    size_t underscore = globalId.find('_');
    if(underscore == string::npos) return nullptr;
    
    string clientId = globalId.substr(0, underscore);
    string indexStr = globalId.substr(underscore + 1);
    int index = stoi(indexStr);
    
    Client* c = findClient(clients, clientId);
    if(!c) return nullptr;
    
    // 遍历用户的帖子找到对应的 idex
    for(int i = 0; i < c->posts.size(); ++i) {
        if(c->posts[i].get_idex() == index) {
            return &c->posts[i];
        }
    }
    return nullptr;
}

bool FileManager::save(SeqList<Client>& clients) {
    ofstream out(file_path);
    if (!out.is_open()) return false;

    out << "{\n";

    // 1. 写入所有 Clients
    out << "  \"clients\": [\n";
    for (int i = 0; i < clients.size(); ++i) {
        Client& c = clients[i];
        out << "    { \"name\": \"" << escapeJsonString(c.Name()) << "\", "
            << "\"id\": \"" << escapeJsonString(c.ID()) << "\", "
            << "\"password\": \"" << escapeJsonString(c.Password()) << "\", "
            << "\"post_time\": " << c.PostTime() << " }";
        if (i < clients.size() - 1) out << ",";
        out << "\n";
    }
    out << "  ],\n";

    // 2. 写入所有 Posts (扁平化)
    out << "  \"posts\": [\n";
    bool firstPost = true;
    for (int i = 0; i < clients.size(); ++i) {
        Client& c = clients[i];
        for (int j = 0; j < c.posts.size(); ++j) {
            if (!firstPost) out << ",\n";
            Post& p = c.posts[j];
            out << "    {\n";
            out << "      \"global_id\": \"" << c.ID() << "_" << p.get_idex() << "\",\n"; // 唯一标识
            out << "      \"author_id\": \"" << c.ID() << "\",\n";
            out << "      \"title\": \"" << escapeJsonString(p.get_title()) << "\",\n";
            out << "      \"content\": \"" << escapeJsonString(p.get_content()) << "\",\n";
            out << "      \"idex\": " << p.get_idex() << ",\n";
            out << "      \"floor\": " << p.get_floor() << ",\n";
            
            // Likers
            out << "      \"likers\": [";
            for(int k=0; k < p.get_likes_list().size(); ++k) {
                if(p.get_likes_list()[k])
                    out << "\"" << escapeJsonString(p.get_likes_list()[k]->ID()) << "\"";
                if(k < p.get_likes_list().size() - 1) out << ", ";
            }
            out << "]\n";
            out << "    }";
            firstPost = false;
        }
    }
    out << "\n  ],\n";

    // 3. 写入所有 Comments (扁平化)
    out << "  \"comments\": [\n";
    bool firstComment = true;
    for (int i = 0; i < clients.size(); ++i) {
        Client& c = clients[i];
        for (int j = 0; j < c.posts.size(); ++j) {
            Post& p = c.posts[j];
            for(int k=0; k < p.comment_list.size(); ++k) {
                if (!firstComment) out << ",\n";
                Comment& com = p.comment_list[k];
                out << "    {\n";
                out << "      \"post_global_id\": \"" << c.ID() << "_" << p.get_idex() << "\",\n";
                out << "      \"author_id\": \"" << (com.get_author() ? com.get_author()->ID() : "") << "\",\n";
                out << "      \"content\": \"" << escapeJsonString(com.get_content()) << "\",\n";
                out << "      \"floor\": " << com.floor() << ",\n";
                out << "      \"reply_floor\": " << com.get_comment_floor() << "\n";
                out << "    }";
                firstComment = false;
            }
        }
    }
    out << "\n  ]\n";

    out << "}";
    out.close();
    return true;
}

// 简单的查找辅助函数
string FileManager::extractValue(const string& json, const string& key, int& startPos) {
    string searchKey = "\"" + key + "\":";
    size_t found = json.find(searchKey, startPos);
    if (found == string::npos) return "";

    size_t valStart = found + searchKey.length();
    while(valStart < json.length() && (json[valStart] == ' ' || json[valStart] == '\n' || json[valStart] == '\t')) valStart++;

    if (valStart >= json.length()) return "";

    if (json[valStart] == '"') { // 字符串
        size_t valEnd = json.find("\"", valStart + 1);
        while(valEnd != string::npos && json[valEnd-1] == '\\') {
             valEnd = json.find("\"", valEnd + 1);
        }
        if (valEnd == string::npos) return "";
        startPos = valEnd + 1;
        return unescapeJsonString(json.substr(valStart + 1, valEnd - valStart - 1));
    } else { // 数字 或 bool
        size_t valEnd = valStart;
        while(valEnd < json.length() && (isdigit(json[valEnd]) || json[valEnd] == '-')) valEnd++;
        startPos = valEnd;
        return json.substr(valStart, valEnd - valStart);
    }
}

bool FileManager::load(SeqList<Client>& clients) {
    ifstream in(file_path);
    if (!in.is_open()) return false;

    stringstream buffer;
    buffer << in.rdbuf();
    string json = buffer.str();
    in.close();

    // 重置
    clients.~SeqList();
    new (&clients) SeqList<Client>(100);
    temp_liker_links.~LinkList();
    new (&temp_liker_links) LinkList<TempPostLikers>();

    // --- 1. 解析 Clients ---
    size_t clientsStartKey = json.find("\"clients\":");
    if(clientsStartKey != string::npos) {
        size_t arrStart = json.find("[", clientsStartKey);
        size_t arrEnd = json.find("]", arrStart); // 这是一个简化的查找，假设没有嵌套数组在对象里
        
        int pos = arrStart + 1;
        while(pos < arrEnd) {
            size_t objStart = json.find("{", pos);
            if(objStart == string::npos || objStart > arrEnd) break;
            
            int localPos = objStart;
            string name = extractValue(json, "name", localPos);
            string id = extractValue(json, "id", localPos);
            string pwd = extractValue(json, "password", localPos);
            string pt = extractValue(json, "post_time", localPos);
            
            Client c(name, id, pwd);
            if(!pt.empty()) c.setPostTime(stoi(pt));
            clients.add(c);
            
            pos = json.find("}", objStart) + 1;
        }
    }

    // --- 2. 解析 Posts ---
    size_t postsStartKey = json.find("\"posts\":");
    if(postsStartKey != string::npos) {
        size_t arrStart = json.find("[", postsStartKey);
        // 我们需要更强壮的循环，因为 post 里面有 likers 数组
        int pos = arrStart + 1;
        
        while(true) {
            size_t objStart = json.find("{", pos);
            // 检查是否到达下一个由Core管理的数组或文件结束
            size_t nextArrKey = json.find("\"comments\":", pos); 
            // 如果找不到下一个 key，就看文件最后的 }
            if(nextArrKey == string::npos) nextArrKey = json.rfind("}"); 

            if(objStart == string::npos || objStart > nextArrKey) break;

            int localPos = objStart;
            string aid = extractValue(json, "author_id", localPos);
            string title = extractValue(json, "title", localPos);
            string content = extractValue(json, "content", localPos);
            string idexStr = extractValue(json, "idex", localPos);
            string floorStr = extractValue(json, "floor", localPos);
            
            Client* author = findClient(clients, aid);
            if(author) {
                Post p(title, content);
                if(!idexStr.empty()) p.set_idex(stoi(idexStr));
                if(!floorStr.empty()) p.set_floor(stoi(floorStr));
                p.set_author(author); // 此时 author 是内存中稳定的指针
                
                author->posts.add(p);
                
                // 记录点赞 (p 是拷贝，我们需要获取链表中稳定的指针)
                Post* stablePtr = author->posts.tail_ptr() ? &author->posts.tail_ptr()->data : nullptr;
                
                if(stablePtr) {
                    TempPostLikers tpl;
                    tpl.post_ptr = stablePtr;
                    
                    // 解析 likers
                    size_t lKey = json.find("\"likers\":", localPos); // localPos 已经被 extractValue 更新过
                    size_t lStart = json.find("[", lKey);
                    size_t lEnd = json.find("]", lStart);
                    string lRaw = json.substr(lStart+1, lEnd-lStart-1);
                    
                    int lp = 0;
                    while(true) {
                        size_t q1 = lRaw.find("\"", lp);
                        if(q1 == string::npos) break;
                        size_t q2 = lRaw.find("\"", q1+1);
                        string lid = unescapeJsonString(lRaw.substr(q1+1, q2-q1-1));
                        tpl.liker_ids.add(lid);
                        lp = q2 + 1;
                    }
                    temp_liker_links.add(tpl);
                }
            }
            
            // 跳过当前对象，找到结束的 }
            // 注意：对象里有 likers 数组，所以简单的 find("}") 可能找到数组的结尾
            // 我们通过 counting braces 或者只是简单地跳过 likers 数组后再找 }
            // 这里采用简单策略：extractValue 已经推进了 localPos 经过了 floor
            // 我们只需要再处理 likers。
            
            // 查找对象结束
            // 上面的 likers 解析没有更新 localPos，这里修正
            // 手动查找当前层级的 }
            // 我们可以利用 "likers" 后面跟着的 ]，然后再找 }
            size_t lKey = json.find("\"likers\":", objStart);
            size_t lEnd = json.find("]", lKey);
            pos = json.find("}", lEnd) + 1;
        }
    }

    // --- 3. 解析 Comments ---
    size_t commentsStartKey = json.find("\"comments\":");
    if(commentsStartKey != string::npos) {
        size_t arrStart = json.find("[", commentsStartKey);
        int pos = arrStart + 1;
        
        while(true) {
            size_t objStart = json.find("{", pos);
            size_t fileEnd = json.rfind("}");
            // 检查是否超出范围 (例如到了最后的 })
            // 简单检查：如果 } 比 { 近，说明数组结束
            size_t arrEndC = json.find("]", pos); // 当前数组结束
            
            if(objStart == string::npos || (arrEndC != string::npos && objStart > arrEndC)) break;

            int localPos = objStart;
            string pid = extractValue(json, "post_global_id", localPos);
            string aid = extractValue(json, "author_id", localPos);
            string content = extractValue(json, "content", localPos);
            string floorStr = extractValue(json, "floor", localPos);
            string repFloorStr = extractValue(json, "reply_floor", localPos);
            
            Post* post = findPost(clients, pid);
            Client* author = findClient(clients, aid);
            
            if(post && author) {
                int rf = repFloorStr.empty() ? -1 : stoi(repFloorStr);
                Comment c(author, content, rf); // 此时 author 指针稳定
                if(!floorStr.empty()) c.set_floor(stoi(floorStr));
                
                post->comment_list.add(c);
            }
            
            pos = json.find("}", objStart) + 1;
        }
    }

    // --- 4. 链接点赞 ---
    for(int i=0; i<temp_liker_links.size(); ++i) {
        TempPostLikers& tpl = temp_liker_links[i];
        for(int j=0; j<tpl.liker_ids.size(); ++j) {
            Client* liker = findClient(clients, tpl.liker_ids[j]);
            if(liker) {
                tpl.post_ptr->get_likes_list().add(liker);
            }
        }
    }
    
    // 清理
    temp_liker_links.~LinkList();
    new (&temp_liker_links) LinkList<TempPostLikers>();

    return true;
}