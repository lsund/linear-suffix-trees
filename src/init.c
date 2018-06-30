#include "init.h"

// Iterate over the text and store the counts of each character. Then iterate
// over 1->256 (all uchars) and store them in the `alpha` parameter, if the
// occurence is greater than 0.
static Uint get_characters()
{
    Wchar alpha[MAX_CHARS + 1];
    Uint counts[MAX_CHARS + 1] = {0};
    Wchar *text_probe;

    for (text_probe = text.fst; text_probe < text.fst + text.len; text_probe++) {
        counts[(Uint) *text_probe]++;
    }

    Uint i, j;
    for (j = 0, i = 0; i <= MAX_CHARS; i++) {
        if (counts[i] > 0) {
            alpha[j++] = (Wchar) i;
        }
    }
    return j;
}

static void init_root_children(STree *st)
{
    Uint *v;
    for (v = st->rs; v <= st->rs + MAX_CHARS; v++) {
        *v = UNDEF;
    }
}

static void nullify_tablestart(STree *st)
{
    for (int i = 0; i < LARGE_VERTEXSIZE; i++) {
        st->is.fst[i] = 0;
    }
}


static void init_head(STree *st)
{
    st->head.vertex    = st->is.fst;
    st->head.label.end = NULL;
    st->head.depth     = 0;
}


static void init_tail(STree *st)
{
    st->tail = text.fst;
}


static void init_vertices(STree *st)
{
    st->is.nxt = st->is.fst;
    st->is.nxt_ind = 0;
    DEPTH(st->is.nxt) = 0;
    HEADPOS(st->is.nxt) = 0;
    SET_CHILD(st->is.nxt, MAKE_LEAF(0));
    SIBLING(st->is.nxt) = 0;
    st->ls.fst[0]   = 0;
    st->ls.nxt_ind  = LEAF_VERTEXSIZE;
    st->ls.nxt      = st->ls.fst + LEAF_VERTEXSIZE;
    st->is.nxt      = st->is.fst + LARGE_VERTEXSIZE;
    st->is.nxt_ind  = LARGE_VERTEXSIZE;
}


static void insert_firstleaf(STree *st)
{
    st->rs[*text.fst] = MAKE_LEAF(0);
}


static void init_split(STree *st)
{
    st->split.child = UNDEF;
    st->split.left  = UNDEF;
}

void init(STree *st)
{
    st->is.size = START_ALLOCSIZE;
    st->ls.fst = realloc(NULL, sizeof(Uint) * text.len + 2);
    st->is.fst = realloc(NULL, sizeof(Uint) * st->is.size);
    st->rs     = realloc(NULL, sizeof(Uint) *  (MAX_CHARS + 1));

    init_root_children(st);
    nullify_tablestart(st);

    st->is.alloc = st->is.fst + st->is.size - LARGE_VERTEXSIZE;

    init_vertices(st);
    init_tail(st);
    init_head(st);
    init_split(st);
    reset_chain(st);
    insert_firstleaf(st);

    text.asize = get_characters();
}

void destroy(STree *st)
{
    free(st->rs);
    free(st->ls.fst);
    free(st->is.fst);
}
