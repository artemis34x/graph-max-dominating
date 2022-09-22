#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "graph.h"

#define TEST_SIZE (1000)

/* never call this */
static void
match_sink(Graph g, int source, int sink, void *data)
{
    assert(data && sink == *((int *) data));
}

int
main(int argc, char **argv)
{
    Graph g;
    int i;
    int j;

    g = graph_create(TEST_SIZE);

    srand(time(NULL));
    while(graph_edge_count(g) != 15000){
        int u = rand() % TEST_SIZE;
        if(graph_out_degree(g,u) < TEST_SIZE){
            int v = rand() % TEST_SIZE;
            if(graph_has_edge(g,u,v) == 0 && u != v)
                graph_add_edge(g,u,v);
        }
    }
    int max = 0;
    for(int i=0;i<TEST_SIZE;i++){
        int tmp = graph_out_degree(g,i);
        if(tmp>max)
            max = tmp;
    }
    for(int i=0;i<TEST_SIZE;i++){
        if(graph_out_degree(g,i) == max)
            printf("%d,",i);
    }
    //print_graph(g);
    /* free it */
    graph_destroy(g);

    return 0;
}
