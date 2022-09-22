#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "graph.h"

/* basic directed graph type */
/* the implementation uses adjacency lists
 * represented as variable-length arrays */

/* these arrays may or may not be sorted: if one gets long enough
 * and you call graph_has_edge on its source, it will be */

struct graph {
    int n;              /* number of vertices */
    int m;              /* number of edges */
    struct successors {
        int p;          /* flag : 0 no assigned yet, 1 linked to a node, 2 center node */
        int d;          /* number of successors */
        int len;        /* number of slots in array */
        char is_sorted; /* true if list is already sorted */
        int list[1];    /* actual list of successors */
    } *alist[1];
};

/* create a new graph with n vertices labeled 0..n-1 and no edges */
Graph
graph_create(int n)
{
    Graph g;
    int i;

    g = malloc(sizeof(struct graph) + sizeof(struct successors *) * (n-1));
    assert(g);

    g->n = n;
    g->m = 0;

    for(i = 0; i < n; i++) {
        g->alist[i] = malloc(sizeof(struct successors));
        assert(g->alist[i]);

        g->alist[i]->p = 0;
        g->alist[i]->d = 0;
        g->alist[i]->len = 1;
        g->alist[i]->is_sorted= 1;
    }
    
    return g;
}

/* free all space used by graph */
void
graph_destroy(Graph g)
{
    int i;

    for(i = 0; i < g->n; i++) free(g->alist[i]);
    free(g);
}

/* add an edge to an existing graph */
void
graph_add_edge(Graph g, int u, int v)
{
    assert(u >= 0);
    assert(u < g->n);
    assert(v >= 0);
    assert(v < g->n);
    /* do we need to grow the list? */
    while(g->alist[u]->d >= g->alist[u]->len) {
        g->alist[u]->len *= 2;
        g->alist[u] =
            realloc(g->alist[u], 
                sizeof(struct successors) + sizeof(int) * (g->alist[u]->len - 1));
    }

    /* now add the new sink */
    g->alist[u]->list[g->alist[u]->d++] = v;
    g->alist[u]->is_sorted = 0;

    /* bump edge count */
    g->m++;
}

/* return the number of vertices in the graph */
int
graph_vertex_count(Graph g)
{
    return g->n;
}

/* return the number of vertices in the graph */
int
graph_edge_count(Graph g)
{
    return g->m;
}

/* return the out-degree of a vertex */
int
graph_out_degree(Graph g, int source)
{
    assert(source >= 0);
    assert(source < g->n);

    return g->alist[source]->d;
}

/* when we are willing to call bsearch */
#define BSEARCH_THRESHOLD (10)

static int
intcmp(const void *a, const void *b)
{
    return *((const int *) a) - *((const int *) b);
}

/* return 1 if edge (source, sink) exists), 0 otherwise */
int
graph_has_edge(Graph g, int source, int sink)
{
    int i;

    assert(source >= 0);
    assert(source < g->n);
    assert(sink >= 0);
    assert(sink < g->n);

    if(graph_out_degree(g, source) >= BSEARCH_THRESHOLD) {
        /* make sure it is sorted */
        if(! g->alist[source]->is_sorted) {
            qsort(g->alist[source]->list,
                    g->alist[source]->d,
                    sizeof(int),
                    intcmp);
        }
        
        /* call bsearch to do binary search for us */
        return 
            bsearch(&sink,
                    g->alist[source]->list,
                    g->alist[source]->d,
                    sizeof(int),
                    intcmp)
            != 0;
    } else {
        /* just do a simple linear search */
        /* we could call lfind for this, but why bother? */
        for(i = 0; i < g->alist[source]->d; i++) {
            if(g->alist[source]->list[i] == sink) return 1;
        }
        /* else */
        return 0;
    }
}

/* invoke f on all edges (u,v) with source u */
/* supplying data as final parameter to f */
void
graph_foreach(Graph g, int source,
    void (*f)(Graph g, int source, int sink, void *data),
    void *data)
{
    int i;

    assert(source >= 0);
    assert(source < g->n);

    for(i = 0; i < g->alist[source]->d; i++) {
        f(g, source, g->alist[source]->list[i], data);
    }
}

/* create a new text file containing the graph */
void
print_graph(Graph g){
    char dest[300]="";
    FILE *f;
    // mode write
    f = fopen("print_graph.txt","w");
    for(int source=0;source<g->n;source++){
        char src[150]=" : ";
        strcpy(dest,"");
        sprintf(dest,"%d",source);
        strcat(dest,src);
        int succs[g->alist[source]->d];
        for(int i=0;i<g->alist[source]->d;i++){
            sprintf(src,"%d",g->alist[source]->list[i]);
            strcat(dest,src);
            strcat(dest,", ");
        }
        strcat(dest,"\n");
        //final string
        fprintf(f,dest); 
    }
    fclose(f);
    return;
}

/* return the first node with the highest degree */
int 
max_degree(Graph g){
    int max = 0;
    /* find the highest degree */
    for(int i=0;i<g->n;i++){
        /* check only non assigned nodes */
        if(g->alist[i]->p == 0){
            int tmp = graph_out_degree(g,i);
            if(tmp>max)
                max = tmp;
        }
    }
    /* get the node number */
    for(int i=0;i<g->n;i++){
        if(graph_out_degree(g,i) == max && g->alist[i]->p == 0)
            return i;
    }
    /* error code */
    return -1;
}

/* check if the max dominating set is done */ 
int 
matched(Graph g){
    for(int i=0;i<g->n;i++){
        if(g->alist[i]->p == 0)
            /* some node still unassigned */
            return 0;
    }
    return 1;
}
/* return an array of int  containing the dominating set */
void
dominating_set(Graph g){
    //counter for the result array size
    int j=0;
    //keep looping until the whole graph is covered
    while(matched(g) == 0){
        //get the highest node
        int node = max_degree(g);
        //printf("node %d : %d\n",j,node);
        //check for error
        if(node == -1){
            puts("error");
            return;
        }
        j++;
        //flag the current node
        g->alist[node]->p = 2;
        //flag all the node around it to 1
        for(int i=0;i < g->alist[node]->d;i++){
            int succ = g->alist[node]->list[i];
            if(g->alist[succ]->p == 0)
                g->alist[succ]->p = 1;
        }
    }
    int result[j];
    //fill the array
    for(int i=0,k=0;i < g->n;i++){
        if(g->alist[i]->p == 2){
            result[k] = i;
            k++;
        }
    }
    printf("\ndominating set[%d] : ",j);
    for(int i=0;i<j-1;i++){
        printf("%d, ",result[i]);
    }
    printf("%d\n",result[j-1]);
    return;
}
