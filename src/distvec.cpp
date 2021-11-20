#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define INF -999
#define MY_INF 999999
#define MAT_SIZE 20
#define NUM_NEIGHBOR 10


int maxRouterIdx;

typedef struct router{
    int neighbor[NUM_NEIGHBOR];
    int weight[NUM_NEIGHBOR];
    int numNeighbor;
}router_t;

router_t** getRouters(FILE* fp);
void printRouters(router_t** routers);
void initTable(int** table, int** nhtable, router_t** routers);
int updateTable(int** ftable, int** nhtable, router_t** routers);
void printTable(int** table);
void printNodeTopo(FILE* fp, int** ftable, int** nhtable);
void printMessage(FILE* fp, int src, int dest, int** ftable, int** nhtable, const char* msg);
void removeEdge(router_t* router, int n);

int main(int argc, char** argv) {
    //printf("Number of arguments: %d", argc);
    if (argc != 4) {
        printf("Usage: ./distvec topofile messagefile changesfile\n");
        return -1;
    }

    /* read topology file and initialize routers */
    FILE *fpTopo;
    router_t** routers;
    fpTopo = fopen(argv[1], "r");
    routers = getRouters(fpTopo);
    // printRouters(routers);
    fclose(fpTopo);

    /* initialize forwarding table and next hop table */
    int** ftable;
    int** nhtable;
    ftable = (int**) malloc(maxRouterIdx * sizeof(int*));
    for (int i = 0; i < maxRouterIdx; i++){
        ftable[i] = (int*) malloc(maxRouterIdx * sizeof(int));
    }
    nhtable = (int**) malloc(maxRouterIdx * sizeof(int*));
    for (int i = 0; i < maxRouterIdx; i++){
        nhtable[i] = (int*) malloc(maxRouterIdx * sizeof(int));
    }
    initTable(ftable, nhtable, routers);
    // printf("Forwarding table:\n");
    // printTable(ftable);
    // printf("Next hop table:\n");
    // printTable(nhtable);

    /* update table until converge */
    int flag;
    // int iter = 0;
    do {
        // printf("\nITERATION %d\n", iter);
        // iter++;
        flag = updateTable(ftable, nhtable, routers);
        // printf("Forwarding table:\n");
        // printTable(ftable);
        // printf("Next hop table:\n");
        // printTable(nhtable);
    } while(flag == 1);
    /* set diagnal to 0 */
    for (int i = 0; i < maxRouterIdx; i++){
        ftable[i][i] = 0;
        nhtable[i][i] = i + 1;
    }
    printf("Forwarding table:\n");
    printTable(ftable);
    printf("Next hop table:\n");
    printTable(nhtable);

    /* output logic here */
    FILE *fpOut = NULL;
    FILE *fpChange;
    FILE *fpMsg;
    fpOut = fopen("outputdiv.txt", "w+");
    if(fpOut == NULL){
        printf("cannot open output.txt");
        exit(1);
    }
    fpMsg = fopen(argv[2], "r");
    fpChange = fopen(argv[3], "r");

    /* print message for initial configuration */
    printNodeTopo(fpOut, ftable, nhtable);
    char msgBuf[100];
    int src, dest, weight;
    while(fscanf(fpMsg, "%d %d ", &src, &dest) != EOF){
        fgets(msgBuf, 30, fpMsg);
        printMessage(fpOut, src, dest, ftable, nhtable, msgBuf);
    }
    fseek(fpMsg, 0, SEEK_SET);

    /* iterate for each change */
    int find; /* indicate if the edge previously exists */
    int numNeighbor; /* temporary variable */
    while (fscanf(fpChange, "%d %d %d", &src, &dest, &weight) != EOF){
        find = 0;
        /* change the routers */
        for (int n = 0; n < routers[src-1]->numNeighbor; n++){
            if(routers[src-1]->neighbor[n] == dest){
                find = 1;
                if(weight != INF){
                    routers[src-1]->weight[n] = weight; break;
                }else{
                    removeEdge(routers[src-1], n);
                }
            }
        }
        if(!find && weight != INF){
            numNeighbor = routers[src-1]->numNeighbor;
            routers[src-1]->neighbor[numNeighbor] = dest;
            routers[src-1]->weight[numNeighbor] = weight;
            routers[src-1]->numNeighbor++;
        }
        /* do the same thing for dest node */
        for (int n = 0; n < routers[dest-1]->numNeighbor; n++){
            if(routers[dest-1]->neighbor[n] == src){
                find = 1;
                if(weight != INF){
                    routers[dest-1]->weight[n] = weight; break;
                }else{
                    removeEdge(routers[dest-1], n);
                }
            }
        }
        if(!find && weight != INF){
            numNeighbor = routers[dest-1]->numNeighbor;
            routers[dest-1]->neighbor[numNeighbor] = src;
            routers[dest-1]->weight[numNeighbor] = weight;
            routers[dest-1]->numNeighbor++;
        }

        /* reset the tables */
        initTable(ftable, nhtable, routers);

        /* renew the tables */
        do {
            flag = updateTable(ftable, nhtable, routers);
        } while(flag == 1);

        /* modify diagonal values */
        for (int i = 0; i < maxRouterIdx; i++){
            ftable[i][i] = 0;
            nhtable[i][i] = i + 1;
        }

        /* check tables */
        printf("Forwarding table:\n");
        printTable(ftable);
        printf("Next hop table:\n");
        printTable(nhtable);

        /* print message */
        printNodeTopo(fpOut, ftable, nhtable);
        while(fscanf(fpMsg, "%d %d ", &src, &dest) != EOF){
            fgets(msgBuf, 30, fpMsg);
            printMessage(fpOut, src, dest, ftable, nhtable, msgBuf);
        }
        fseek(fpMsg, 0, SEEK_SET);
    }

    fclose(fpOut);
    fclose(fpChange);
    fclose(fpMsg);

    /* free array */
    for (int i = 0; i < maxRouterIdx; i++){
        free(routers[i]);
    }
    free(routers);
    for (int i = 0; i < maxRouterIdx; i++){
        free(ftable[i]);
    }
    free(ftable);
    


    return 0;
}

router_t** getRouters(FILE* fp){

    int src, dest, weight;
    int n_idx;
    router_t** routers;

    /* find the number of routers */
    while (fscanf(fp, "%d %d %d", &src, &dest, &weight) != EOF){
        /* find the largest router No. */
        if(src > maxRouterIdx) maxRouterIdx = src;
        if(dest > maxRouterIdx) maxRouterIdx = dest;
    }

    /* instantiate router structs */
    routers = (router_t**) malloc(maxRouterIdx * sizeof(router_t*));
    for (int i = 0; i < maxRouterIdx; i++){
        routers[i] = (router_t*) malloc(sizeof(router_t));
        memset(routers[i], 0, sizeof(router_t));
    }
    
    /* reset the file pointer and fill in router structs */
    fseek(fp, 0, SEEK_SET);
    while (fscanf(fp, "%d %d %d", &src, &dest, &weight) != EOF && weight != INF){
        n_idx = routers[src-1]->numNeighbor;
        routers[src-1]->neighbor[n_idx] = dest;
        routers[src-1]->weight[n_idx] = weight;
        routers[src-1]->numNeighbor = n_idx + 1;
        n_idx = routers[dest-1]->numNeighbor;
        routers[dest-1]->neighbor[n_idx] = src;
        routers[dest-1]->weight[n_idx] = weight;
        routers[dest-1]->numNeighbor = n_idx + 1;
    }
    
    return routers;
}

void printRouters(router_t** routers){
    /* for debugging */
    for (int i = 0; i < maxRouterIdx; i++){
        printf("Router %d:\n", i+1);
        for (int j = 0; j < routers[i]->numNeighbor; j++){
            printf("  To neighbor %d, weight %d\n", routers[i]->neighbor[j], routers[i]->weight[j]);
        }
    }
}

void initTable(int** ftable, int** nhtable, router_t** routers){
    for (int i = 0; i < maxRouterIdx; i++){
        for (int j = 0; j < maxRouterIdx; j++){
            ftable[i][j] = MY_INF;
            nhtable[i][j] = INF;
        }
    }
    
    for (int i = 0; i < maxRouterIdx; i++){
        for (int j = 0; j < routers[i]->numNeighbor; j++){
            ftable[i][routers[i]->neighbor[j]-1] = routers[i]->weight[j];
            nhtable[i][routers[i]->neighbor[j]-1] = routers[i]->neighbor[j];
        }
    }
}

int updateTable(int** ftable, int** nhtable, router_t** routers){
    router_t* router;
    int neighbor, w;
    int flag = 0;
    /* for each router */
    for (int i = 0; i < maxRouterIdx; i++){
        router = routers[i];
        /* for each neighbor */
        for (int n = 0; n < router->numNeighbor; n++){
            neighbor = router->neighbor[n] - 1; /* the index of the neighbor router */
            w = router->weight[n];  /* weight from this router to this neighbor */
            /* for each destination of that neighbor */
            for (int dest = 0; dest < maxRouterIdx; dest++){
                if(ftable[neighbor][dest] + w < ftable[i][dest]){
                    ftable[i][dest] = ftable[neighbor][dest] + w;
                    nhtable[i][dest] = neighbor + 1;
                    flag = 1;
                }else if(ftable[neighbor][dest] + w == ftable[i][dest] && neighbor + 1 < nhtable[i][dest]){
                    /* tie breaking. choose the next hop with lower ID */
                    nhtable[i][dest] = neighbor + 1;
                    flag = 1;
                }
            }
        }
    }

    return flag; /* check if the iteration converges */
}

void printTable(int** table){
    for (int i = 0; i < maxRouterIdx; i++){
        for (int j = 0; j < maxRouterIdx; j++){
            printf(" %-2d", table[i][j]);
        }
        printf("\n");
    }
}

void printNodeTopo(FILE* fp, int** ftable, int** nhtable){
    for (int i = 0; i < maxRouterIdx; i++){
        for (int dest = 0; dest < maxRouterIdx; dest++){
            fprintf(fp, "%d %d %d\n", dest+1, nhtable[i][dest], ftable[i][dest]);
        }
        fprintf(fp, "\n");
    }
}

void printMessage(FILE* fp, int src, int dest, int** ftable, int** nhtable, const char* msg){

    if(nhtable[src-1][dest-1] == INF){
        fprintf(fp, "from %d to %d cost infinite hops unreachable message %s\n", src, dest, msg);
    } else {
        fprintf(fp, "from %d to %d cost %d hops ", src, dest, ftable[src-1][dest-1]);

        int nextHop = src;

        while(nextHop != dest){
            fprintf(fp, "%d ", nextHop);
            nextHop = nhtable[nextHop-1][dest-1];
        }
        fprintf(fp, "%d message %s\n", dest, msg);
    }

}

void removeEdge(router_t* router, int n){
    router->numNeighbor--;
    for (int i = n; i < router->numNeighbor; i++){
        router->neighbor[i] = router->neighbor[i+1];
        router->weight[i] = router->weight[i+1];
    }
    
}

