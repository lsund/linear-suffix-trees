#include "stree_aux.h"

Uint get_depth_head(STree *stree, Uint *depth, Uint *head, Uint *vertexp, Uint *largep)
{
    Uint distance = 0;
    if(stree->chainstart != NULL && vertexp >= stree->chainstart) {

        distance = 1 +
            DIVBYSMALLINTS((Uint) (stree->inner_vertices.next_free - (vertexp)));
        *depth = stree->currentdepth + distance;
        *head = stree->leaf_vertices.next_free_num - distance;

    } else {

        if(ISLARGE(*(vertexp))) {

            *depth = GETDEPTH(vertexp);
            *head = GETHEADPOS(vertexp);

        } else {

            distance = GETDISTANCE(vertexp);
            GETCHAINEND(largep, vertexp, distance);
            *depth = GETDEPTH(largep) + distance;
            *head = GETHEADPOS(largep) - distance;

        }
    }
    return distance;
}


void get_head(STree *stree, Uint *vertexp, Uint **largep, Uint *distance, Uint *head)
{
    if(stree->chainstart != NULL && vertexp >= stree->chainstart) {
        *distance = 1 + DIVBYSMALLINTS((Uint) (stree->inner_vertices.next_free - vertexp));
        *head = stree->leaf_vertices.next_free_num - *distance;
    } else
    {
        if(ISLARGE(*(vertexp))) {
            *head = GETHEADPOS(vertexp);
        } else {
            *distance = GETDISTANCE(vertexp);
            GETCHAINEND(*largep, vertexp, *distance);
            *head = GETHEADPOS(*largep) - *distance;
        }
    }
}

Uint get_depth(STree *stree, Uint *vertexp, Uint *distance, Uint **largep)
{
    if(stree->chainstart != NULL && vertexp >= stree->chainstart) {
        *distance = 1 + DIVBYSMALLINTS((Uint) (stree->inner_vertices.next_free - vertexp));
        return stree->currentdepth  + *distance;
    } else {
        if(ISLARGE(*vertexp)) {
            return GETDEPTH(vertexp);
        } else {
            *distance = GETDISTANCE(vertexp);
            GETCHAINEND(*largep, vertexp, *distance);
            return GETDEPTH(*largep) + *distance;
        }
    }
}

