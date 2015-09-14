//
//  main.cpp
//  Graph
//
//  Created by sun zewei on 15-8-1.
//  Copyright (c) 2015年 sun zewei. All rights reserved.
//

#include <iostream>
#include <vector>
#include <queue>
using namespace std;
int n,m;

struct edge
{
    int start;
    int end;
    int value;
};

struct vertex
{
    int length;//邻接链表长度
    vector<int> p;//邻接链表
    int color;//颜色，0白，1灰，2黑
    int t1,t2;//DFS，发现时间，结束时间
    int last;//上一个点
    int key;//prim算法，该点到最小树子树的最小距离
    bool in_or_out;//prim算法，判断该点是否已经加入子树，0未加入，1已加入
    int parent;//kruskal算法判断是否联通
    int distance;//距源节点的距离
};

edge E[101];
vertex V[101];
int w[101][101];
vector<int> topo;//拓扑排序，以DFS中的完成时间为序
//edge *E=new edge[n+1];
//vertex *V=new vertex[m+1];

int min(int &a, int &b)
{
    if(a<b) return a;
    else return b;
}

void swap(edge *a, edge *b)
{
    edge temp;
    temp=*a;
    *a=*b;
    *b=temp;
}

void quicksort(edge *a, int min, int max)
{
    int j=min;
    for(int i=min;i<max;i++)
        if(a[i].value<a[max].value)
        {
            swap(&a[i],&a[j]);
            j++;
        }
    swap(&a[max],&a[j]);
    if(min<j-1) quicksort(a,min,j-1);
    if(j+1<max) quicksort(a,j+1,max);
}

void BFS(int s)//宽度优先搜索
{
    for(int i=1;i<=n;i++)
    {
        V[i].color=0;//全染白
        V[i].last=0;
    }
    V[s].color=1;//访问第一个点
    queue<int> q;//队列定义
    q.push(s);//第一个点入队列
    while(!q.empty())
    {
        int u=q.front();//取队列第一个点
        q.pop();
        cout<<u<<endl;
        for(int i=0;i<V[u].length;i++)
            if(V[V[u].p[i]].color==0)//发现未访问过的子女节点
            {
                q.push(V[u].p[i]);//子女节点入队列
                V[V[u].p[i]].color=1;//子女节点染灰
                V[V[u].p[i]].last=u;
            }
        V[u].color=2;//子女节点都已入队列，染黑
    }
}

void DFS_visit(int u)
{
    static int times=0;
    V[u].color=1;//被首次访问，染灰
    times++;
    V[u].t1=times;//首次访问时间
    //cout<<u<<endl;
    for(int i=0;i<V[u].length;i++)
        if(V[V[u].p[i]].color==0)//发现未访问过的子女节点
        {
            V[V[u].p[i]].last=u;
            DFS_visit(V[u].p[i]);//往下走
        }
    V[u].color=2;//该点后代全访问完，该点处理结束，染黑
    times++;
    V[u].t2=times;//结束时间
    topo.push_back(u);
}

void DFS()//深度优先搜索
{
    for(int i=1;i<=n;i++)
        V[i].color=0;//全染白
    for(int i=1;i<=n;i++)
        if(V[i].color==0)
            DFS_visit(i);
}

int root(int x)//返回祖先，有点并查集的味道
{
    while(V[x].parent!=x)
        x=V[x].parent;
    return x;
}

void kruskal()//最小生成树
{
    for(int i=1;i<=n;i++)
        V[i].parent=i;
    quicksort(E,1,m);//边按权重排序
    for(int i=1;i<=m;i++)
        if(root(E[i].start)!=root(E[i].end))//判断是否已联通（拥有同一个祖先），避免成环
        {
            cout<<E[i].start<<'-'<<E[i].end<<endl;
            V[root(V[E[i].end].parent)].parent=root(E[i].start);//联通
        }
}

void prim(int root)//最小生成树
{
    for(int i=1;i<=n;i++)
    {
        V[i].in_or_out=0;//所有点集合
        V[i].key=INT_MAX;//每个点到最小树的最短距离
    }
    V[root].key=0;
    V[root].last=0;
    for(int i=1;i<=n;i++)//n轮，把n个点加进去
    {
        int temp=INT_MAX,k=0;
        for(int j=1;j<=n;j++)//扫描每个点，看哪个点的key最小，即离子树最近
            if(!V[j].in_or_out && V[j].key<temp)
            {
                temp=V[j].key;
                k=j;
            }//k是这一轮的选择
        V[k].in_or_out=1;//标记k，已经入伙
        
        if(i!=root) cout<<V[k].last<<'-'<<k<<endl;//输出加入的边
        for(int j=0;j<V[k].length;j++)
            if(!V[V[k].p[j]].in_or_out && min(w[k][V[k].p[j]],w[V[k].p[j]][k])<V[V[k].p[j]].key)
            {
                V[V[k].p[j]].key=min(w[k][V[k].p[j]],w[V[k].p[j]][k]);//把跟k相接的点都待定，更新他们的key
                V[V[k].p[j]].last=k;
            }
    }
}

void initialize_single_source(int x)
{
    for(int i=1;i<=n;i++)
    {
        V[i].distance=INT_MAX;
        V[i].last=0;
        V[i].in_or_out=0;//为Dijkstra算法服务
    }
    V[x].distance=0;
}

void relax(int x, int y)
{
    if(V[x].distance==INT_MAX || w[x][y]==INT_MAX) return;
    if(V[x].distance+w[x][y]<V[y].distance)
    {
        V[y].distance=V[x].distance+w[x][y];
        V[y].last=x;
    }
}

void Bellman_Ford(int x)//单源有向图最短路径（权可为负，可有环）
{
    initialize_single_source(x);
    for(int i=1;i<=n;i++)//方法1，n轮，每轮更新n个点，O(n^2)
        for(int j=1;j<=n;j++)
            relax(i,j);
    /*for(int i=1;i<=n;i++)//方法2，n轮，每轮更新m条边，O（nm）
        for(int j=1;j<=m;j++)
                relax(E[j].start,E[j].end);*/
    for(int i=1;i<=m;i++)
        if(V[E[i].start].distance<INT_MAX &&
           V[E[i].start].distance+w[E[i].start][E[i].end]>V[E[i].end].distance)//判断是否有负权重环
        {cout<<"存在负权重环";return;}
    for(int i=1;i<=n;i++)
        cout<<V[i].distance<<endl;
}

void DAG_shortest_path(int x)//单源有向无环图最短路径（权可为负）
{
    initialize_single_source(x);
    DFS();//通过DFS完成拓扑排序，O（n+m）
    for(int i=n-1;i>=0;i--)//总体O（n+m）
        for(int j=0;j<V[topo[i]].length;j++)
            relax(topo[i],V[topo[i]].p[j]);
    for(int i=1;i<=n;i++)
        cout<<V[i].distance<<endl;
}

void Dijkstra(int x)//单源有向无环图最短路径（权不可为负），框架跟prim很像，key改为distance，O(n^2+m)，若用最小堆做队列可降为O(nlgn+mlgn)，利用斐波那契堆可降为O(nlgn+m)
{
    initialize_single_source(x);
    V[x].distance=0;
    V[x].last=0;
    for(int i=1;i<=n;i++)//n轮，把n个点加进去
    {
        int temp=INT_MAX,k=0;
        for(int j=1;j<=n;j++)//扫描每个点，看哪个点的distance最小，即离子树最近
            if(!V[j].in_or_out && V[j].distance<temp)
            {
                temp=V[j].distance;
                k=j;
            }//k是这一轮的选择
        V[k].in_or_out=1;//标记k，已经入伙
        
        
        for(int j=0;j<V[k].length;j++)
            if(!V[V[k].p[j]].in_or_out && V[k].distance+w[k][V[k].p[j]]<V[V[k].p[j]].distance)
            {
                V[V[k].p[j]].distance=V[k].distance+w[k][V[k].p[j]];//把跟k相接的点都更新distance
                V[V[k].p[j]].last=k;
            }
    }
    for(int i=1;i<=n;i++)
        cout<<V[i].distance<<endl;
}

void Floyd_Warshall()//所有节点对的最短路径，O(n^3)
{
    int d[n+1][n+1];
    for(int i=1;i<=n;i++)//初始化
        for(int j=1;j<=n;j++)
            d[i][j]=w[i][j];
    for(int k=1;k<=n;k++)
        for(int i=1;i<=n;i++)
            for(int j=1;j<=n;j++)
                if(d[i][k]<INT_MAX && d[k][j]<INT_MAX)//防止溢出
                    d[i][j]=min(d[i][j],d[i][k]+d[k][j]);//核心
    for(int i=1;i<=n;i++)
    {
        for(int j=1;j<=n;j++)
        {
            if(d[i][j]==INT_MAX) cout<<"* ";
            else cout<<d[i][j]<<' ';
        }
        cout<<endl;
    }

}

void input()
{
    cin>>n>>m;
    for(int i=1;i<=n;i++)
        for(int j=1;j<=n;j++)
        {
            if(i==j)
                w[i][j]=0;
            else
                w[i][j]=INT_MAX;
        }
    for(int i=1;i<=m;i++)
    {
        cin>>E[i].start>>E[i].end>>E[i].value;//输入边的始末，权重
        V[E[i].start].p.push_back(E[i].end);//补充点的邻接链表
        V[E[i].start].length++;
        //V[E[i].end].p.push_back(E[i].start);//无向图时补充，prim算法需要
        //V[E[i].end].length++;
        w[E[i].start][E[i].end]=E[i].value;//储存矩阵，另一种储存边的方式
    }
}

int main()
{
    input();
    //Floyd_Warshall();//所有节点对的最短路径
    //Dijkstra(1);//单源最短路径
    //DAG_shortest_path(1);//单源最短路径
    //Bellman_Ford(1);//单源最短路径
    //prim(1);//最小生成树
    //kruskal();//最小生成树
    //DFS();//深度优先搜索
    //BFS(1);//广度优先搜索
    return 0;
}

