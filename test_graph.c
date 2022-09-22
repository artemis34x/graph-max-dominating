#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "graph.h"


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

    //generate all the nodes
    g = graph_create(1000);

    //link all the node randomly
    srand(time(NULL));
    while(graph_edge_count(g) != 15000){
        int u = rand() % 1000;
        if(graph_out_degree(g,u) < 1000){
            int v = rand() % 1000;
            if(graph_has_edge(g,u,v) == 0 && u != v)
                graph_add_edge(g,u,v);
        }
    }
    print_graph(g);
    dominating_set(g);
    /* free it */
    graph_destroy(g);

    return 0;
}
