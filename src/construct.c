#include "stree.h"
#include "insert.h"
#include "skip_count.h"
#include "scan.h"

///////////////////////////////////////////////////////////////////////////////
// Private


static void insert_tailedge(STree *st)
{
    if(base_is_vertex(st)) {
        insert(st);
    } else {
        split_and_insert(st);
    }
}


static bool label_empty(Label l)
{
    return l.start == l.end;
}


static void find_base(STree *st)
{
    if (head_is_root(st)) {
        if (label_empty(st->hd.l)) {
            st->hd.l.end = NULL;
        } else {
            st->hd.l.start++;
            skip_count(st);
        }
    } else {
        follow_link(st);
        skip_count(st);
    }
}

static void find_nxt_head(STree * st) {
    if(head_is_root(st) && base_is_vertex(st)) {
        st->tail++;
        scan_tail(st);
    } else {
        if (is_head_old(st)) {
            follow_link(st);
            scan_tail(st);
        } else {
            find_base(st);
            if(base_is_vertex(st)) {
                finalize_chain(st);
                scan_tail(st);
            } else {
                grow_chain(st);
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Public API


void construct(STree *st)
{
    init(st);
    while(!tail_at_lastchar(st)) {
        find_nxt_head(st);
        insert_tailedge(st);
    }
}
