#include <bits/stdc++.h>
using namespace std;


int infinity = numeric_limits<int>::max();
int graph [100][100];
vector<string> path ;
int dist[100][100]; // The output array.  dist[i] will hold the shortest
    // distance from src to i

bool sptSet[100]; // sptSet[i] will be true if vertex i is included in shortest
    // path tree or shortest distance from src to i is finalized

string paths[100][100];
/******************************************/

int minDistance(int src,int dist[100][100], bool sptSet[] ,int number_of_nodes)
{
   
   int V = number_of_nodes+1;
    // Initialize min value
    int min = INT_MAX, min_index;
 
    for (int v = 0; v < V; v++)
        if (sptSet[v] == false && dist[src][v] <= min)
            min = dist[src][v], min_index = v;
 
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
    path.clear();
    int V = number_of_nodes+1;
    
 
    
 
    // Initialize all distances as INFINITE and stpSet[] as false
    
    for (int i = 1; i < V; i++){
        dist[src][i] = INT_MAX, sptSet[i] = false;
        path.push_back("");
        paths[src][i]="";
    }
    
 
    // Distance of source vertex from itself is always 0
    dist[src][src] = 0;
 
    // Find shortest path for all vertices
    for (int count = 0; count < V - 1; count++) {
        // Pick the minimum distance vertex from the set of vertices not
        // yet processed. u is always equal to src in the first iteration.
        int u = minDistance(src,dist, sptSet, number_of_nodes);
 
        // Mark the picked vertex as processed
        sptSet[u] = true;
 
        // Update dist value of the adjacent vertices of the picked vertex.
        for (int v = 1; v < V; v++)
            
            // Update dist[v] only if is not in sptSet, there is an edge from
            // u to v, and total weight of path from src to  v through u is
            // smaller than current value of dist[v]
            if (!sptSet[v] && graph[u][v] && dist[src][u] != INT_MAX
                && dist[src][u] + graph[u][v] < dist[src][v] ||(!sptSet[v] && graph[u][v] && dist[src][u] != INT_MAX
                && dist[u] + graph[u][v] == dist[v] && u < path[v-1].back() - 0 )){
                dist[src][v] = dist[src][u] + graph[u][v];
                //cout<<"path u-1 "<<path[u-1]<<" u "<<u<<endl; 
                path[v-1]= path[u-1]+" "+to_string(u);
                paths[src][v]=paths[src][u]+" "+to_string(u);
                }
    }
    path[src-1]=to_string(src);
    paths[src][src]=to_string(src);
    // print the constructed distance array
    //printSolution(dist,number_of_nodes);
    //print the paths to each node
    
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

void linkstate(int number_of_nodes){
    for (int i=1;i<=number_of_nodes;i++){
        dijkstra(i,number_of_nodes);
    }
}

int main(){
    
    
    int number_of_nodes = 0;
    int x,y,z;
    FILE* messagefile;
    FILE* changesfile;
    ifstream topofile("topofile.txt");
    char msg[100];
    //ifstream messagefile("messagefile.txt");
    //ifstream changesfile("changesfile.txt");
    messagefile = fopen("messagefile.txt","r");
    changesfile = fopen("changesfile.txt","r");
    ofstream output("output.txt");
    //cout<<infinity;
    while (topofile>>x>>y>>z){
        // cout<<x<<" "<<y<<" "<<z<<endl;
        // vect.push_back(make_pair(x,make_pair(y,z)));
        graph[x][y]=z;
        graph[y][x]=z;
        number_of_nodes = max(number_of_nodes,x);
        number_of_nodes = max(number_of_nodes,y);
        //cout<<"number of nodes is :"<<number_of_nodes<<endl;
    }
    for (int i=1;i<=number_of_nodes;i++){
        output<<"<topology entries for node "<<i<<">"<<endl;
    }
    output<<endl;
    
    int src,dest;
    linkstate(number_of_nodes);
    while (fscanf(messagefile, "%d %d ", &src, &dest) != EOF){
        fgets(msg,30,messagefile);
        if (dist[src][dest]<INT_MAX){
            output<<"from "<<src<<" to "<<dest<<" cost "<<dist[src][dest]<<" hops "<<paths[src][dest]<<" message "<<msg<<endl;
        }
        else{
            output<<"from "<<src<<" to "<<dest<<" cost infinite hops unreachable message "<<msg<<endl;
        }
        //cout<<"from "<<src<<" to "<<dest<<endl;
        
        //printf("from %d to %d cost %d hops \n", src, dest, dist[src][dest]);
        //printf("path from %d to %d is %s\n",src,dest,paths[src][dest]);
        //output<<"path from "<<src<<" to "<<dest<<" is"<<paths[src][dest]<<endl;
        
        
    }
    fseek(messagefile, 0, SEEK_SET);
    int change;
    while (fscanf(changesfile, "%d %d %d", &src, &dest, &change) != EOF){
        output<<endl;
        if(change != -999){
            graph[src][dest]=graph[dest][src]=change;
        }
        else{
            graph[src][dest]=graph[dest][src]=0;
        }
        linkstate(number_of_nodes);
        while (fscanf(messagefile, "%d %d ", &src, &dest) != EOF){
            fgets(msg,30,messagefile);
            if (dist[src][dest]<INT_MAX){
                output<<"from "<<src<<" to "<<dest<<" cost "<<dist[src][dest]<<" hops "<<paths[src][dest]<<" message "<<msg<<endl;
            }
            else{
                output<<"from "<<src<<" to "<<dest<<" cost infinite hops unreachable message "<<msg<<endl;
            }
            //cout<<"from "<<src<<" to "<<dest<<endl;
            
            //printf("from %d to %d cost %d hops \n", src, dest, dist[src][dest]);
            //printf("path from %d to %d is %s\n",src,dest,paths[src][dest]);
            //output<<"path from "<<src<<" to "<<dest<<" is"<<paths[src][dest]<<endl;
            
            
        }
        fseek(messagefile, 0, SEEK_SET);
        
    }
    // puts("");
    // for (int i=0;i<vect.size();i++){
    //     cout<<vect[i].first<<" "<<vect[i].second.first<<" "<<vect[i].second.second<<endl;
    // }
    
    
    
    
}
