#include <QApplication>
#include "mainwindow.h"
#include "models/clients.h"
#include "models/Post.h"
#include "models/comment.h"
#include "models/action.h"
#include "models/clients_table_model.h"
#include "models/posts_table_model.h"
#include <QFile>
#include <QIODevice>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    // 构造示例数据（与控制台 main 类似）
    // 使用系统默认主题（白色背景）；不加载暗色样式
    SeqList<Client> clients(10);
    clients.add(Client("自信的空空", "2022211001001306", "123456"));
    clients.add(Client("我不想上学", "2022211001001307", "123456"));

    Post post1("家人门谁懂啊，普坝出心了", "骗你的");
    clients[0].addPost(post1);
    // 给默认帖子添加一条评论，便于界面初始可见
    Comment demoC(&clients[1], std::string("我懂，因为我真出了"));
    clients[1].addComment(&clients[0].posts[0], demoC);

    // 绑定到表格模型
    std::vector<Client*> clientPtrs = { &clients[0], &clients[1] };
    std::vector<Post*> postPtrs;
    for (int i = 0; i < clients[0].posts.size(); ++i) postPtrs.push_back(&clients[0].posts[i]);

    auto* clientsModel = new ClientsTableModel(&app);
    clientsModel->setClients(clientPtrs);

    auto* postsModel = new PostsTableModel(&app);
    postsModel->setPosts(postPtrs);

    MainWindow w;
    w.setClientsModel(clientsModel);
    w.setPostsModel(postsModel);
    w.show();
    return app.exec();
}


