#include <ios>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;

int main() {
    int n,r;
    cin>>n>>r;
    vector<int> v;
    for(int i=0;i<n;i++){
        int x;
        cin>>x;
        v.push_back(x);
    }
    sort(v.begin(),v.end());
    int sum=0;
    int cnt=1;
    int h=0;
    for(int i=n-1;i>=0;i--){
        int fuck = 0;
        if(i!=0){
            fuck =(v[i]-v[i-1]);
            sum += fuck*cnt;
            h = v[i-1];
        }else{
            fuck = v[i];
            sum+= fuck * cnt;
            h = 0;
        }
        if(sum>r){
            int up = (sum-r)/cnt+1;
            while(sum-up*fuck>r){
                up--;
                sum-=fuck;
            }
            h+=up;           
            break;
        }
        cnt++;
    }
    cout<<h-1<<endl;

    
    return 0;
}