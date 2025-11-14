#include <iostream>
#include "data_structure/lin_list.h"
#include "data_structure/stack.h"
#include "data_structure/queue.h"
#include "manager/undo_manager.h"
#include "manager/file_manager.h" // 包含 FileManager
#include "models/Post.h"
#include "models/clients.h"
#include "models/action.h"
#include "models/comment.h" // 包含 Comment

using namespace std;

// --- 全局数据 ---
// 假设最大客户端数量为100 (可以根据需要调整)
SeqList<Client> all_clients(100); 

// --- 辅助函数 ---

// 根据 ID 查找客户端 (返回指针)
Client* getClientById(const string& id) {
    for (int i = 0; i < all_clients.size(); ++i) {
        if (all_clients[i].ID() == id) {
            return &all_clients[i];
        }
    }
    return nullptr; // 未找到
}

// 创建新客户端
void createClient(const string& name, const string& id, const string& password) {
    if (getClientById(id)) {
        cout << "[错误] 客户端ID '" << id << "' 已存在！" << endl;
        return;
    }
    if (all_clients.full()) {
        cout << "[错误] 客户端列表已满，无法添加新客户端！" << endl;
        return;
    }
    
    Client newClient(name, id, password);
    all_clients.add(newClient);
    cout << "[日志] 成功创建客户端: " << name << " (ID: " << id << ")" << endl;
}

// --- 主函数 (应用启动点) ---
int main() {
    
    cout << "--- 社交网络管理系统 (核心) ---" << endl;

    // 1. 启动时加载所有数据
    if (FileManager::instance().load(all_clients)) {
        cout << "成功从 " << FileManager::instance().file_path << " 加载数据。" << endl;
    } else {
        cout << "未找到数据文件或加载失败。启动空系统。" << endl;
    }

    // --- Qt 应用启动点 ---
    //
    // (未来的 Qt GUI 代码会在这里初始化并接管 all_clients 列表)
    // (GUI 将调用 all_clients[i].addPost(), getClientById(), 等等)
    //
    // --- 填充初始数据 (仅当文件不存在时) ---
    //
    if (all_clients.empty()) {
        cout << "系统为空，正在创建更丰富的中文测试数据..." << endl;
        
        // 1. 创建更多用户
        createClient("张伟", "zhangwei", "pass123");
        createClient("李静", "lijing", "pass456");
        createClient("王磊", "wanglei", "pass789");
        createClient("刘洋", "liuyang", "pass101");
        createClient("陈秀英", "chenxiuying", "pass112");

        // 2. 获取用户指针
        Client* pZhangWei = getClientById("zhangwei");
        Client* pLiJing = getClientById("lijing");
        Client* pWangLei = getClientById("wanglei");
        Client* pLiuYang = getClientById("liuyang");
        Client* pChenXiuying = getClientById("chenxiuying");

        if (pZhangWei && pLiJing && pWangLei && pLiuYang && pChenXiuying) {
            
            cout << "正在创建帖子..." << endl;
            // 3. 张伟 发帖 (技术宅)
            Post postZW1("关于C++模板元编程的思考", "C++模板元编程真是太神奇了，编译期就能完成计算！");
            pZhangWei->addPost(postZW1);
            Post postZW2("新键盘到了", "HHKB 静电容yyds！");
            pZhangWei->addPost(postZW2);

            // 4. 李静 发帖 (生活达人)
            Post postLJ1("周末去哪儿玩？", "大家有什么推荐的周末好去处吗？想去爬山或者看展。");
            pLiJing->addPost(postLJ1);

            // 5. 王磊 发帖 (吃货)
            Post postWL1("食堂的饭", "今天食堂的糖醋里脊绝了，你们吃了吗？");
            pWangLei->addPost(postWL1);

            // 6. 陈秀英 发帖 (广场舞爱好者)
            Post postCXY1("今晚广场舞新曲目", "《最炫民族风》已加入歌单，大家准时到！");
            pChenXiuying->addPost(postCXY1);

            // 7. 获取帖子指针 (必须在 addPost 之后获取)
            Post* pPostZW1 = &pZhangWei->posts[0]; // 张伟的C++帖
            Post* pPostLJ1 = &pLiJing->posts[0];   // 李静的周末帖
            Post* pPostWL1 = &pWangLei->posts[0];   // 王磊的食堂帖

            // 8. 添加更复杂的互动
            cout << "正在添加复杂的互动 (点赞、评论、楼中楼)..." << endl;
            
            // --- 互动: C++ 帖子 (pPostZW1) ---
            // 李静 评论
            Comment commentLJ_on_ZW1(pLiJing, "哇，虽然看不懂，但是感觉好厉害！");
            pLiJing->addComment(pPostZW1, commentLJ_on_ZW1); // 2楼
            
            // 王磊 评论
            Comment commentWL_on_ZW1(pWangLei, "太难了哥，学不动了...");
            pWangLei->addComment(pPostZW1, commentWL_on_ZW1); // 3楼

            // 张伟 回复 王磊 (楼中楼)
            // 注意：王磊是3楼 (pPostZW1->comment_list[1])
            int floor_wanglei = pPostZW1->comment_list[1].floor(); // 获取王磊的楼层号 (应该是 3)
            Comment commentZW_reply_WL1(pZhangWei, "别放弃，我发你个入门教程！", floor_wanglei);
            pZhangWei->addComment(pPostZW1, commentZW_reply_WL1); // 4楼 (回复 3 楼)

            // 刘洋 点赞
            pLiuYang->like(pPostZW1);


            // --- 互动: 周末帖 (pPostLJ1) ---
            // 大家都喜欢这篇
            pZhangWei->like(pPostLJ1);
            pWangLei->like(pPostLJ1);
            pLiuYang->like(pPostLJ1);
            pChenXiuying->like(pPostLJ1);

            // 王磊 评论
            Comment commentWL_on_LJ1(pWangLei, "我推荐去植物园，最近花都开了！");
            pWangLei->addComment(pPostLJ1, commentWL_on_LJ1); // 2楼

            // 张伟 评论
            Comment commentZW_on_LJ1(pZhangWei, "附议，植物园不错。");
            pZhangWei->addComment(pPostLJ1, commentZW_on_LJ1); // 3楼

            
            // --- 互动: 食堂帖 (pPostWL1) ---
            // 刘洋 评论
            Comment commentLY_on_WL1(pLiuYang, "真的吗？我中午去吃！");
            pLiuYang->addComment(pPostWL1, commentLY_on_WL1); // 2楼

            // 李静 点赞
            pLiJing->like(pPostWL1);

            // --- 互动: 广场舞帖 (pPostCXY1) ---
            // (无人互动，测试冷清的帖子)

            cout << "测试数据创建完毕！" << endl;
        }
    }

    // --- 米可添加：输出验证 ---
    cout << "\n\n--- Zhengzai yanzheng dangqian shuju ---" << endl;
    if (all_clients.empty()) {
        cout << "Xitong zhong meiyou kehu duan shuju." << endl;
    } else {
        cout << "Xitong zhong gongyou " << all_clients.size() << " ge kehu duan：" << endl;
        for (int i = 0; i < all_clients.size(); ++i) {
            cout << "\n---------------------------\n";
            cout << "--- Kehu duan " << (i + 1) << " xiangqing ---\n";
            cout << all_clients[i]; // 利用重载的 << 运算符
            cout << "---------------------------\n";
        }
    }
    cout << "--- Shuju yanzheng wanbi ---\n\n" << endl;


    //
    // --- Qt 应用关闭点 ---
    //
    // (当 Qt 应用关闭时，它会触发这里的保存逻辑)
    //
    
    // 2. 退出前保存所有数据
    if (FileManager::instance().save(all_clients)) {
        cout << "Shuju yi chenggong baocun dao " << FileManager::instance().file_path << endl;
    } else {
        cerr << "[Yanzhong cuowu] Shuju baocun shibai！" << endl;
    }
    
    cout << "Guanli hexin yi tingzhi yunxing. Zaijian miao！" << endl;

    return 0;
}