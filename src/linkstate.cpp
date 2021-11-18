#include <bits/stdc++.h>
using namespace std;


int infinity = numeric_limits<int>::max();
int graph [100][100];


/******************************************/

int minDistance(int dist[], bool sptSet[] ,int number_of_nodes)
{
   
   int V = number_of_nodes+1;
    // Initialize min value
    int min = INT_MAX, min_index;
 
    for (int v = 0; v < V; v++)
        if (sptSet[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;
 
    return min_index;
}
 
// A utility function to print the constructed distance array
void printSolution(int dist[] , int number_of_nodes)
{
    int V = number_of_nodes+1;
    cout <<"Vertex \t Distance from Source" << endl;
    for (int i = 1; i < V; i++)
        cout  << i << " \t\t"<<dist[i]<< endl;
}


// Function that implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency matrix representation
void dijkstra(int src , int number_of_nodes)
{
    int V = number_of_nodes+1;
    vector<string> path ;
    int dist[V]; // The output array.  dist[i] will hold the shortest
    // distance from src to i
 
    bool sptSet[V]; // sptSet[i] will be true if vertex i is included in shortest
    // path tree or shortest distance from src to i is finalized
 
    // Initialize all distances as INFINITE and stpSet[] as false
    
    for (int i = 1; i < V; i++){
        dist[i] = INT_MAX, sptSet[i] = false;
        path.push_back("");
    }
    
 
    // Distance of source vertex from itself is always 0
    dist[src] = 0;
 
    // Find shortest path for all vertices
    for (int count = 0; count < V - 1; count++) {
        // Pick the minimum distance vertex from the set of vertices not
        // yet processed. u is always equal to src in the first iteration.
        int u = minDistance(dist, sptSet, number_of_nodes);
 
        // Mark the picked vertex as processed
        sptSet[u] = true;
 
        // Update dist value of the adjacent vertices of the picked vertex.
        for (int v = 1; v < V; v++)
 
            // Update dist[v] only if is not in sptSet, there is an edge from
            // u to v, and total weight of path from src to  v through u is
            // smaller than current value of dist[v]
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX
                && dist[u] + graph[u][v] < dist[v]){
                dist[v] = dist[u] + graph[u][v];
                cout<<"path u-1 "<<path[u-1]<<" u "<<u<<endl; 
                path[v-1]= path[u-1]+to_string(u);
                }
    }
    path[src-1]=to_string(src);
    // print the constructed distance array
    printSolution(dist,number_of_nodes);
    puts("path\n\n");
    for(int i=0;i<path.size();i++){
        cout<<path[i]<<endl;
    }
}

/*******************************************/









// void dj(int src , int dest , int number_of_nodes  ){
    
//     set<int, greater<int> > visited;
//     visited.insert(src);
//     int distance[number_of_nodes+1];
    
//     for (int i=1; i<= number_of_nodes;i++){
//         if (i==src) distance[i]=0;
//         else  distance[i]=cost[src][i];
//     }
    
    
    
    
// }

int main(){
    
    
    int number_of_nodes = 0;
    int x,y,z;
    
    freopen("input.in","r",stdin);
    //cout<<infinity;
    while (cin>>x>>y>>z){
        // cout<<x<<" "<<y<<" "<<z<<endl;
        // vect.push_back(make_pair(x,make_pair(y,z)));
        graph[x][y]=z;
        graph[y][x]=z;
        number_of_nodes = max(number_of_nodes,x);
        number_of_nodes = max(number_of_nodes,y);
        //cout<<"number of nodes is :"<<number_of_nodes<<endl;
    }
    
        dijkstra(2,number_of_nodes);
    // puts("");
    // for (int i=0;i<vect.size();i++){
    //     cout<<vect[i].first<<" "<<vect[i].second.first<<" "<<vect[i].second.second<<endl;
    // }
    
    
    
    
}
