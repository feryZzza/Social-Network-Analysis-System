#include <iostream>
#include "manager/core.h"

using namespace std;

int main() {
    cout << "--- 社交网络管理系统 (Core版) ---" << endl;

    Core& core = Core::instance();

    // 1. 加载数据 (JSON)
    if (core.loadData()) {
        cout << "[系统] 数据加载成功。" << endl;
    } else {
        cout << "[系统] 未找到数据或解析失败，启动新系统。" << endl;
    }

    // 2. 如果是空系统，生成测试数据
    if (core.getAllClients().empty()) {
        cout << "正在生成测试数据..." << endl;
        
        core.registerClient("张伟", "zhangwei", "pass123");
        core.registerClient("李静", "lijing", "pass456");
        core.registerClient("王磊", "wanglei", "pass789");

        Client* zw = core.getClientById("zhangwei");
        Client* lj = core.getClientById("lijing");
        Client* wl = core.getClientById("wanglei");

        // 张伟发帖
        core.userAddPost(zw, "C++从入门到放弃", "指针太难了！");
        Post* p1 = &zw->posts[0];

        // 李静评论
        core.userAddComment(lj, p1, "多写代码就好了");
        
        // 王磊点赞
        core.userLikePost(wl, p1);
        
        // 李静发帖
        core.userAddPost(lj, "周末去爬山", "有人一起吗？");
        
        // 撤销测试：张伟撤销最后一次操作(这里没操作，加一个点赞试试)
        core.userLikePost(zw, &lj->posts[0]);
        cout << "张伟点赞后..." << endl;
        cout << lj->posts[0]; // 此时应该有张伟的点赞

        core.userUndo(zw); // 撤销点赞
        cout << "张伟撤销点赞后..." << endl;
        cout << lj->posts[0]; // 此时应该没有点赞
    }

    // 3. 显示所有数据
    cout << "\n=== 当前系统状态 ===\n";
    SeqList<Client>& clients = core.getAllClients();
    for(int i=0; i<clients.size(); ++i) {
        cout << clients[i] << endl;
    }

    // 4. 保存数据
    if (core.saveData()) {
        cout << "[系统] 数据已保存到 JSON 文件。" << endl;
    } else {
        cout << "[错误] 保存失败。" << endl;
    }

    return 0;
}