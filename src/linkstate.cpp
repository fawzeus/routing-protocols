#include <bits/stdc++.h> // its anclude all c++ libraries

using namespace std;


//global variables

int graph [100][100]; //adjacency matrix
vector<string> path ;

int dist[100][100]; // The output array.  dist[src][i] will hold the shortest
    // distance from src to i

bool sptSet[100]; // sptSet[i] will be true if vertex i is included in shortest
    // path tree or shortest distance from src to i is finalized

//matrix that contains all paths
string paths[100][100];



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
 

// Function that implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency matrix representation
void dijkstra(int src , int number_of_nodes)
{
    path.clear();
    int V = number_of_nodes+1;
    
 
    
 
    // Initialize all distances as INFINITE , stpSet[] as false and paths as "" 
    
    for (int i = 1; i < V; i++){
        dist[src][i] = INT_MAX, sptSet[i] = false;
        path.push_back("");
        paths[src][i]="";
    }
    
 
    dist[src][src] = graph[src][src];
 
    // Find shortest path for all vertices
    for (int count = 0; count < V - 1; count++) {
        // Pick the minimum distance vertex from the set of vertices not
        // yet processed. u is always equal to src in the first iteration.
        int u = minDistance(src,dist, sptSet, number_of_nodes);
 
        // Mark the picked vertex as processed
        sptSet[u] = true;
 
        // Update dist value of the adjacent vertices of the picked vertex.
        for (int v = 1; v < V; v++)
            
            // Update dist[src][v] only if is not in sptSet, there is an edge from
            // u to v, and total weight of path from src to  v through u is
            // smaller than current value of dist[src][v]
            if (!sptSet[v] && graph[u][v] && dist[src][u] != INT_MAX
                && dist[src][u] + graph[u][v] < dist[src][v] ||(!sptSet[v] && graph[u][v] && dist[src][u] != INT_MAX
                && dist[u] + graph[u][v] == dist[v] && u < path[v-1].back() - 0 )){
                dist[src][v] = dist[src][u] + graph[u][v]; 
                path[v-1]= path[u-1]+" "+to_string(u);
                paths[src][v]=paths[src][u]+" "+to_string(u);
                }
    }
    path[src-1]=to_string(src);
    paths[src][src]=to_string(src);
    
}

void linkstate(int number_of_nodes){
    for (int i=1;i<=number_of_nodes;i++){
        dijkstra(i,number_of_nodes);
    }
}

void topology_entries(int number_of_nodes ,FILE* topofile,ofstream& output){
    int x,y,z,out;
    while (fscanf(topofile,"%d %d %d",&x,&y,&z) != EOF ){
        
        out=graph[x][y];
        if (out == 0 && x!=y) out=-999;
        output<<x<<" "<<y<<" "<<out<<endl;
    }
    fseek(topofile, 0, SEEK_SET);
}

int fill_adjacency_matrix(FILE* topofile){
    int x,y,z,number_of_nodes=0;
    while (fscanf(topofile,"%d %d %d",&x,&y,&z) != EOF ){
        //filling the adjacency matrix
        graph[x][y]=z;
        graph[y][x]=z; // adjancency matrix is symmetric
        dist[x][y]=z;
        dist[y][x]=z;
        number_of_nodes = max(number_of_nodes,x);
        number_of_nodes = max(number_of_nodes,y); //counting the number of nodes
    }
    fseek(topofile, 0, SEEK_SET);
    return number_of_nodes;
}

void send_messages(FILE* messagefile,ofstream& output,char* msg){
    int src,dest;
    while (fscanf(messagefile, "%d %d ", &src, &dest) != EOF){
        fgets(msg,30,messagefile);
        if (dist[src][dest]<INT_MAX){
            output<<"from "<<src<<" to "<<dest<<" cost "<<dist[src][dest]<<" hops"<<paths[src][dest]<<" message "<<msg<<endl;
        }
        else{
            output<<"from "<<src<<" to "<<dest<<" cost infinite hops unreachable message "<<msg<<endl;
        }
    }
    fseek(messagefile, 0, SEEK_SET);
}

void change_topology(FILE* topofile,FILE* changesfile,FILE* messagefile,ofstream& output,char* msg,int number_of_nodes){
    int change,src,dest;
    while (fscanf(changesfile, "%d %d %d", &src, &dest, &change) != EOF){
        if(change != -999){
            //update the adjacency matrix
            graph[src][dest]=graph[dest][src]=change;
        }
        else{
            //update the adjacency matrix
            //if change is -999 then thers is no route from x to y 
            graph[src][dest]=graph[dest][src]=0;
        }
        //recreate the routing tables for each change
        linkstate(number_of_nodes);
        //printing topology entries
        topology_entries(number_of_nodes ,topofile,output);
        //read messages from messagefile and send them if possible
        send_messages(messagefile,output,msg);
        
    }
}

int main(int argc, char** argv){
    
    //printf("Number of arguments: %d", argc);
    if (argc != 4) {
        printf("Usage: ./linkstate topofile messagefile changesfile\n");
        return -1;
    }

    int number_of_nodes = 0;
    int x,y,z;
    FILE* messagefile;
    FILE* changesfile;
    FILE* topofile;;
    char msg[100];

    //open needed files
    //ifstream topofile(argv[1]);
    topofile = fopen(argv[1],"r");
    messagefile = fopen(argv[2], "r");
    changesfile = fopen(argv[3], "r");
    ofstream output("output.txt");

    //reading topology inputs and counting the number of nodes
    number_of_nodes=fill_adjacency_matrix(topofile);
    //printing topology entries
    topology_entries(number_of_nodes ,topofile,output);
    //applying linkstate algorithm to fill routing tables using the initial topology
    linkstate(number_of_nodes);

    int src,dest;
    //read messages from messagefile and send them if possible, according to initial topology
    send_messages(messagefile,output,msg);
    //change the topology for each change in the changesfile
    change_topology(topofile,changesfile,messagefile,output,msg,number_of_nodes);

    //closing files
    fclose(messagefile);
    fclose(changesfile);    
}
