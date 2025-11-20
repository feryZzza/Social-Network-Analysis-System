#include "models/sort.h"
#include "models/clients.h"
#include "models/Post.h"
#include <utility> // for std::swap

// =========================================================
//  内部辅助函数与算法实现 (对外部隐藏)
// =========================================================

// --- 基础工具 ---
template <typename T>
void mySwap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

int myMin(int a, int b) {
    return a < b ? a : b;
}

// --- 比较函数 (Internal) ---

// 用户排序：按好友数降序，ID升序
static bool compareHaoyou(const Client& a, const Client& b) {
    if (a.get_friends_num() != b.get_friends_num()) { 
        return a.get_friends_num() > b.get_friends_num(); 
    }
    return a.ID() < b.ID(); 
}

// 帖子排序：按点赞数降序，ID升序
static bool comparePost(const Post& a, const Post& b) {
    if (a.likes_num() != b.likes_num()) { 
        return a.likes_num() > b.likes_num(); 
    }
    return a.get_idex() < b.get_idex(); 
}

// --- 排序算法模板 (Internal) ---
// 这些算法保持不变，专门针对 SeqList<T> 进行排序

// 插入排序（降序）
template <typename T>
void insertionSort(SeqList<T>& arr, bool (*count)(const T& a, const T& b)) {
    int n = arr.size(); 
    for (int i = 1; i < n; i++) {
        T key = arr[i];
        int j = i - 1;
        while (j >= 0 && count(key, arr[j])) { 
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// 快速排序（降序）
template <typename T>
int partition(SeqList<T>& arr, int left, int right, bool (*count)(const T& a, const T& b)) {
    T pivot = arr[right];
    int i = left - 1;
    for (int j = left; j <= right - 1; j++) {
        if (count(arr[j], pivot)) {
            i++;
            mySwap(arr[i], arr[j]);
        }
    }
    mySwap(arr[i + 1], arr[right]);
    return i + 1;
}

template <typename T>
void quickSort(SeqList<T>& arr, int left, int right, bool (*count)(const T& a, const T& b)) {
    if (left < right) {
        int pivotIdx = partition(arr, left, right, count);
        quickSort(arr, left, pivotIdx - 1, count);
        quickSort(arr, pivotIdx + 1, right, count);
    }
}

// 堆排序（降序）
template <typename T>
void heapify(SeqList<T>& arr, int n, int i, bool (*count)(const T& a, const T& b)) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && count(arr[left], arr[largest])) largest = left;
    if (right < n && count(arr[right], arr[largest])) largest = right;

    if (largest != i) {
        mySwap(arr[i], arr[largest]);
        heapify(arr, n, largest, count);
    }
}

template <typename T>
void heapSort(SeqList<T>& arr, bool (*count)(const T& a, const T& b)) {
    int n = arr.size();
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i, count);
    }
    for (int i = n - 1; i > 0; i--) {
        mySwap(arr[0], arr[i]);
        heapify(arr, i, 0, count);
    }
}

const int SMALL_SCALE = 100;   
const int LARGE_SCALE = 10000; 

template <typename T>
void ShiyingSort(SeqList<T>& arr, bool (*count)(const T& a, const T& b)) {
    int n = arr.size(); 
    if (n <= 1) return;

    if (n <= SMALL_SCALE) {
        insertionSort(arr, count);
    }
    else if (n <= LARGE_SCALE) {
        quickSort(arr, 0, n - 1, count);
    }
    else {
        heapSort(arr, count);
    }
}

//这个地方这么写是因为你的排序函数输入参数是引用类型，而我魔改后需要对指针类型进行排序。
//为了不改你程序，就需要一个中转函数，用来解引用指针并调用实际的比较函数。好好看一下理解理解
//在调用你的排序函数前，你线需要把比较函数指针赋值给这两个全局变量，明确了指针解引用后的比较逻辑。然后输入两个指针类型参数，解引用后调用实际比较函数。
//这样可以直接适用你现有的排序函数。

static bool (*g_clientCompare)(const Client&, const Client&) = nullptr;
static bool (*g_postCompare)(const Post&, const Post&) = nullptr;

static bool compareClientPtr(Client* const& a, Client* const& b) {
    if (!a || !b) return false;
    return g_clientCompare(*a, *b);
}

static bool comparePostPtr(Post* const& a, Post* const& b) {
    if (!a || !b) return false;
    return g_postCompare(*a, *b);
}

Sorter::Sorter() : clientIndex(nullptr), postIndex(nullptr) {}

Sorter::~Sorter() {
    if(clientIndex) delete clientIndex;
    if(postIndex) delete postIndex;
}

// 直接接收 SeqList<Client>，初始化用户指针索引
void Sorter::initClientIndices(SeqList<Client>& source) {
    if (clientIndex) delete clientIndex;
    clientIndex = new SeqList<Client*>(source.size() + 10); 

    for(int i = 0; i < source.size(); ++i) {
        clientIndex->add(&source[i]);
    }
}

// 接收LinkList<Post>，初始化帖子指针索引
void Sorter::initPostIndices(LinkList<Post>& source) {
    if (postIndex) delete postIndex;
    postIndex = new SeqList<Post*>(source.size() + 10);

    for(int i = 0; i < source.size(); ++i) {
        postIndex->add(&source[i]);
    }
}

// 接收 SeqList<Client>，不断寻找用户的帖子，初始化全局帖子指针索引
void Sorter::initGlobalPostIndices(SeqList<Client>& users) {
    int totalPosts = 0;
    for(int i = 0; i < users.size(); ++i) {
        totalPosts += users[i].posts.size();
    }

    if (postIndex) delete postIndex;
    postIndex = new SeqList<Post*>(totalPosts + 10);

    for(int i = 0; i < users.size(); ++i) {

        LinkList<Post>& userPosts = users[i].posts;
        for(int j = 0; j < userPosts.size(); ++j) {
            postIndex->add(&userPosts[j]);
        }
    }
}
//排序，在外部直接输入你的比较函数指针，类内部通过全局变量传递给比较中转函数经历上面解释的那一套逻辑
void Sorter::sortClientIndices(bool (*compare)(const Client&, const Client&)) {//输入排序比较函数指针
    if (!clientIndex || clientIndex->empty()) return;
    g_clientCompare = compare;
    ShiyingSort(*clientIndex, compareClientPtr);
}

void Sorter::sortPostIndices(bool (*compare)(const Post&, const Post&)) {//输入排序比较函数指针
    if (!postIndex || postIndex->empty()) return;
    g_postCompare = compare;
    ShiyingSort(*postIndex, comparePostPtr);
}


// 类的公开接口
void Sorter::haoyoushu(SeqList<Client>& clients) {
    if (clients.size() == 0) {
        std::cout << "没有用户数据可生成排行榜" << std::endl;
        return;
    }

    initClientIndices(clients);
    
    sortClientIndices(compareHaoyou);

    std::cout << "\n=== 用户影响力排行榜（按好友数） ===" << std::endl;
    int displayUserCnt = myMin(10, clientIndex->size());
    
    for (int i = 0; i < displayUserCnt; i++) {
        Client* user = (*clientIndex)[i]; 
        if(!user) continue;

        std::cout << i + 1 << ". 用户ID: " << user->ID()
            << ", 昵称: " << user->Name()
            << ", 好友数: " << user->get_friends_num()
            << ", 发布帖子数: " << user->posts.size() << std::endl;

        if (user->posts.size() > 0) {
            initPostIndices(user->posts);
            sortPostIndices(comparePost);

            int displayPostCnt = myMin(3, postIndex->size());
            if (displayPostCnt > 0) {
                std::cout << "   个人热门帖子：" << std::endl;
                for (int j = 0; j < displayPostCnt; j++) {
                    Post* post = (*postIndex)[j];
                    if(post) {
                        std::cout << "     " << j + 1 << ". 帖子ID: " << post->get_idex()
                            << ", 标题: " << post->get_title()
                            << ", 点赞数: " << post->likes_num() << std::endl;
                    }
                }
            }
        }
    }

    if (clientIndex->size() > 10) {
        std::cout << "... 还有 " << clientIndex->size() - 10 << " 名用户未显示" << std::endl;
    }
}

// 全局热门帖子排行榜
void Sorter::dianzanshu(SeqList<Client>& users) { 
    if (users.size() == 0) {
        std::cout << "没有用户数据，无法生成热门帖子排行榜" << std::endl;
        return;
    }

    initGlobalPostIndices(users);
    
    if (postIndex->size() == 0) {
        std::cout << "没有帖子数据可生成排行榜" << std::endl;
        return;
    }

    sortPostIndices(comparePost);

    std::cout << "\n=== 全局热门帖子排行榜（按点赞数） ===" << std::endl;
    int displayPostCnt = myMin(10, postIndex->size());
    for (int i = 0; i < displayPostCnt; i++) {
        Post* post = (*postIndex)[i];
        if(!post) continue;

        std::string authorName = post->author ? post->author->Name() : "未知";
        
        std::cout << i + 1 << ". 帖子ID: " << post->get_idex()
            << ", 作者: " << authorName
            << ", 标题: " << post->get_title()
            << ", 点赞数: " << post->likes_num() << std::endl;
    }

    if (postIndex->size() > 10) {
        std::cout << "... 还有 " << postIndex->size() - 10 << " 个帖子未显示" << std::endl;
    }
}