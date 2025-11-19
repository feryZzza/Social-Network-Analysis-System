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

bool FileManager::save(SeqList<Client>& clients) {
    ofstream out(file_path);
    if (!out.is_open()) return false;

    out << "{\n  \"clients\": [\n";

    for (int i = 0; i < clients.size(); ++i) {
        Client& c = clients[i];
        out << "    {\n";
        out << "      \"name\": \"" << escapeJsonString(c.Name()) << "\",\n";
        out << "      \"id\": \"" << escapeJsonString(c.ID()) << "\",\n";
        out << "      \"password\": \"" << escapeJsonString(c.Password()) << "\",\n";
        out << "      \"post_time\": " << c.PostTime() << ",\n";
        
        out << "      \"posts\": [\n";
        for (int j = 0; j < c.posts.size(); ++j) {
            Post& p = c.posts[j];
            out << "        {\n";
            out << "          \"title\": \"" << escapeJsonString(p.get_title()) << "\",\n";
            out << "          \"content\": \"" << escapeJsonString(p.get_content()) << "\",\n";
            out << "          \"idex\": " << p.get_idex() << ",\n";
            out << "          \"floor\": " << p.get_floor() << ",\n";
            
            // 保存点赞者ID数组
            out << "          \"likers\": [";
            for(int k=0; k < p.get_likes_list().size(); ++k) {
                if(p.get_likes_list()[k])
                    out << "\"" << escapeJsonString(p.get_likes_list()[k]->ID()) << "\"";
                if(k < p.get_likes_list().size() - 1) out << ", ";
            }
            out << "],\n";

            // 保存评论
            out << "          \"comments\": [\n";
            for(int k=0; k < p.comment_list.size(); ++k) {
                Comment& com = p.comment_list[k];
                out << "            {\n";
                out << "              \"content\": \"" << escapeJsonString(com.get_content()) << "\",\n";
                out << "              \"floor\": " << com.floor() << ",\n";
                out << "              \"reply_floor\": " << com.get_comment_floor() << ",\n";
                string authorId = com.get_author() ? com.get_author()->ID() : "";
                out << "              \"author_id\": \"" << escapeJsonString(authorId) << "\"\n";
                out << "            }";
                if(k < p.comment_list.size() - 1) out << ",";
                out << "\n";
            }
            out << "          ]\n";
            out << "        }";
            if(j < c.posts.size() - 1) out << ",";
            out << "\n";
        }
        out << "      ]\n";
        out << "    }";
        if(i < clients.size() - 1) out << ",";
        out << "\n";
    }

    out << "  ]\n}";
    out.close();
    return true;
}

// 简单的查找辅助函数
// 在 json 字符串中从 startPos 开始查找 "key": value
string FileManager::extractValue(const string& json, const string& key, int& startPos) {
    string searchKey = "\"" + key + "\":";
    size_t found = json.find(searchKey, startPos);
    if (found == string::npos) return "";

    size_t valStart = found + searchKey.length();
    // 跳过空白
    while(valStart < json.length() && (json[valStart] == ' ' || json[valStart] == '\n' || json[valStart] == '\t')) valStart++;

    if (valStart >= json.length()) return "";

    if (json[valStart] == '"') { // 字符串
        size_t valEnd = json.find("\"", valStart + 1);
        // 处理转义引号的情况 (简单处理)
        while(valEnd != string::npos && json[valEnd-1] == '\\') {
             valEnd = json.find("\"", valEnd + 1);
        }
        if (valEnd == string::npos) return "";
        // 更新 startPos 以便下次搜索
        startPos = valEnd + 1;
        return unescapeJsonString(json.substr(valStart + 1, valEnd - valStart - 1));
    } else { // 数字 或 bool
        size_t valEnd = valStart;
        while(valEnd < json.length() && (isdigit(json[valEnd]) || json[valEnd] == '-')) valEnd++;
        startPos = valEnd;
        return json.substr(valStart, valEnd - valStart);
    }
}

// 加载函数
bool FileManager::load(SeqList<Client>& clients) {
    ifstream in(file_path);
    if (!in.is_open()) return false;

    stringstream buffer;
    buffer << in.rdbuf();
    string json = buffer.str();
    in.close();

    // 重置列表
    clients.~SeqList();
    new (&clients) SeqList<Client>(100);
    temp_load_data.~LinkList();
    new (&temp_load_data) LinkList<TempLoadData>();

    int globalPos = 0;
    
    // 1. 查找 "clients": [
    size_t clientsStart = json.find("\"clients\":");
    if (clientsStart == string::npos) return false;
    size_t arrayStart = json.find("[", clientsStart);
    if (arrayStart == string::npos) return false;
    
    globalPos = arrayStart + 1;

    // 循环解析 Client 对象
    while (true) {
        size_t objStart = json.find("{", globalPos);
        size_t arrayEnd = json.find("]", globalPos);
        
        // 如果先遇到了 ]，说明数组结束
        if (arrayEnd != string::npos && (objStart == string::npos || arrayEnd < objStart)) break;
        
        // 解析 Client 基础信息
        int clientPos = objStart;
        string name = extractValue(json, "name", clientPos);
        string id = extractValue(json, "id", clientPos);
        string pwd = extractValue(json, "password", clientPos);
        string ptStr = extractValue(json, "post_time", clientPos);
        int postTime = ptStr.empty() ? 0 : stoi(ptStr);

        Client client(name, id, pwd);
        client.setPostTime(postTime);

        // 解析 Posts
        size_t postsStartKey = json.find("\"posts\":", clientPos);
        if (postsStartKey != string::npos) {
            size_t pArrayStart = json.find("[", postsStartKey);
            int postGlobalPos = pArrayStart + 1;
            
            while(true) {
                size_t pObjStart = json.find("{", postGlobalPos);
                size_t pArrayEnd = json.find("]", postGlobalPos);
                // 检查 Posts 数组是否结束
                if (pArrayEnd < pObjStart) break; 

                int pPos = pObjStart;
                string title = extractValue(json, "title", pPos);
                string content = extractValue(json, "content", pPos);
                string idexStr = extractValue(json, "idex", pPos);
                string floorStr = extractValue(json, "floor", pPos);
                
                Post post(title, content);
                post.set_idex(stoi(idexStr));
                post.set_floor(stoi(floorStr));
                // --- 关键修改：初始化为 nullptr，防止指向栈上即将销毁的 client ---
                post.set_author(nullptr); 

                TempLoadData tData;
                tData.client_id = id;
                tData.post_id = id + "_" + idexStr;

                // 解析 Likers 数组
                size_t likerKey = json.find("\"likers\":", pPos);
                size_t lArrStart = json.find("[", likerKey);
                size_t lArrEnd = json.find("]", lArrStart);
                string likersRaw = json.substr(lArrStart+1, lArrEnd - lArrStart - 1);
                
                int lPos = 0;
                while(true) {
                    size_t q1 = likersRaw.find("\"", lPos);
                    if(q1 == string::npos) break;
                    size_t q2 = likersRaw.find("\"", q1+1);
                    string lid = unescapeJsonString(likersRaw.substr(q1+1, q2-q1-1));
                    tData.liker_ids.add(lid);
                    lPos = q2 + 1;
                }

                // 解析 Comments 数组
                size_t commentsKey = json.find("\"comments\":", pPos);
                size_t cArrayStart = json.find("[", commentsKey);
                int cPos = cArrayStart + 1;
                
                while(true) {
                     size_t cObjStart = json.find("{", cPos);
                     size_t checkClose = cPos;
                     while(isspace(json[checkClose])) checkClose++;
                     if(json[checkClose] == ']') break;

                     string cContent = extractValue(json, "content", cPos);
                     string cFloor = extractValue(json, "floor", cPos);
                     string cRepFloor = extractValue(json, "reply_floor", cPos);
                     string cAuthId = extractValue(json, "author_id", cPos);
                     
                     if(cContent.empty()) break;

                     Comment cmt(nullptr, cContent, stoi(cRepFloor));
                     cmt.set_floor(stoi(cFloor));
                     post.comment_list.add(cmt);
                     tData.comment_author_ids.add(cAuthId);
                     
                     size_t nextObj = json.find("}", cPos); 
                     cPos = nextObj + 1;
                     while(isspace(json[cPos]) || json[cPos] == ',') cPos++;
                     if(json[cPos] == ']') break;
                }
                
                client.posts.add(post);
                temp_load_data.add(tData);

                postGlobalPos = json.find("}", pPos) + 1; // Post 结束
                globalPos = postGlobalPos;
            }
        }
        
        clients.add(client);
        
        size_t nextClientObj = json.find("}", globalPos); 
        globalPos = nextClientObj + 1;
    }

    reconstructPointers(clients);
    return true;
}

bool FileManager::reconstructPointers(SeqList<Client>& clients) {
    for (int i = 0; i < clients.size(); ++i) {
        Client& client = clients[i];
        for (int j = 0; j < client.posts.size(); ++j) {
            Post& post = client.posts[j];
            
            // --- 关键修复 ---
            // 重新将 author 指向内存中真正的 client 对象
            post.set_author(&client);

            // 后续逻辑：恢复点赞和评论关系
            string global_post_id = client.ID() + "_" + to_string(post.get_idex());

            TempLoadData* info = nullptr;
            for (int k = 0; k < temp_load_data.size(); ++k) {
                if (temp_load_data[k].post_id == global_post_id) {
                    info = &temp_load_data[k];
                    break;
                }
            }
            
            if (!info) continue;

            // 重建点赞
            for (int k = 0; k < info->liker_ids.size(); ++k) {
                string targetId = info->liker_ids[k];
                for (int m = 0; m < clients.size(); ++m) {
                    if (clients[m].ID() == targetId) {
                        post.get_likes_list().add(&clients[m]);
                        break;
                    }
                }
            }

            // 重建评论作者
            for (int k = 0; k < post.comment_list.size(); ++k) {
                if (k >= info->comment_author_ids.size()) break;
                string targetId = info->comment_author_ids[k];
                for (int m = 0; m < clients.size(); ++m) {
                    if (clients[m].ID() == targetId) {
                        post.comment_list[k].set_author(&clients[m]);
                        break;
                    }
                }
            }
        }
    }
    
    temp_load_data.~LinkList();
    new (&temp_load_data) LinkList<TempLoadData>();
    return true;
}