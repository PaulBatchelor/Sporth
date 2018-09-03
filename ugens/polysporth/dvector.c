#include <stdio.h>
#include <stdlib.h>
#include "plumber.h"
#include "scheme-private.h"
#include "polysporth.h"

void dvector_print(dvector *dv)
{
    if(dv->size == 0) {
        printf("The vector is empty!\n");
        return;
    }

    int i;
    dvalue *val = dv->root.next;
    dvalue *next;
    for(i = 0; i < dv->size; i++) {
        next = val->next;
        val = next;
    }
}

void dvector_append_value(dvector *dv, dvalue *new)
{
    dv->last->next = new;
    dv->last = new;
    dv->size++;
}

void dvector_append(dvector *dv,
    int grp_start, int grp_end, int delta, int dur,
    SPFLOAT *args, int nargs)
{
    dvalue *new = malloc(sizeof(dvalue));
    new->delta = delta;
    new->grp_start = grp_start;
    new->grp_end = grp_end;
    new->dur = dur;
    new->type = PS_NOTE;
    new->nargs = nargs;
    new->args = args;
    new->next = NULL;
    dvector_append_value(dv, new);
}

void dvector_append_metanote(dvector *dv, int start, pointer func)
{
    dvalue *new = malloc(sizeof(dvalue));
    new->delta = start;
    new->func = func;
    new->type = PS_METANOTE;
    dvector_append_value(dv, new);
}

void dvector_init(dvector *dv)
{
    dv->size = 0;
    dv->last = &dv->root;
    dv->root.next = NULL;
}

void dvector_free(dvector *dv)
{
    int i;
    dvalue *val = dv->root.next;
    dvalue *next;
    for(i = 0; i < dv->size; i++) {
        next = val->next;
        dvalue_free(&val);
        val = next;
    }
}

dvector dvector_merge(dvector *dvect1, dvector *dvect2)
{
    /* Check for empty vectors */

    if(dvect1->size == 0 && dvect2->size == 0) {
        return *dvect1;
    } else if(dvect1->size == 0 && dvect2->size > 0) {
        return *dvect2;
    } else if(dvect1->size > 0 && dvect2->size == 0) {
        return *dvect1;
    }

    dvector new, *tmp;
    dvector_init(&new);

    if(dvect1->size < dvect2->size) {
        tmp = dvect1;
        dvect1 = dvect2;
        dvect2 = tmp;
    }

    dvalue *val1 = dvect1->root.next;
    dvalue *val2 = dvect2->root.next;

    int dv1_t = val1->delta, dv2_t = val2->delta;
    int dv1_pos = 0, dv2_pos = 0;

    int out_d = 0;
    int out_t = 0;

    int i;
    int append = 0;
    for(i = 0; i < dvect1->size + dvect2->size; i++) {
        /* TODO: fix this cluster fuck of a conditional... */
        if((dv1_t <= dv2_t || append) && dv1_pos < dvect1->size) {
            dv1_pos++;
            out_d = dv1_t - out_t;
            val1->delta = out_d;
            dvector_append_value(&new, val1);
            if(dv1_pos < dvect1->size) {
                val1 = val1->next;
                dv1_t += val1->delta;
            }
        } else {
            dv2_pos++;
            out_d = dv2_t - out_t;
            val2->delta = out_d;
            dvector_append_value(&new, val2);
            if(dv2_pos >= dvect2->size) {
                append = 1;
            } else {
                val2 = val2->next;
                dv2_t += val2->delta;
            }
        }
        out_t += out_d;
    }
    return new;
}

void dvector_time_sort(dvector *dvect)
{
    dvalue *entry;
    dvalue *head;
    dvalue *current;
    dvalue *p;

    entry = dvect->root.next;
    head = NULL;

    while(entry != NULL) {
        current = entry;
        entry = entry->next;
        if(head == NULL || current->delta < head->delta) {
            current->next = head;
            head = current;
        } else {
            p = head;

            while(p != NULL) {
                if(p->next == NULL /* last element */ ||
                    current->delta < p->next->delta) {
                    current->next = p->next;
                    p->next = current;
                    break;
                }

                p = p->next;
            }
        }
    }

    dvect->root.next = head;
}

int dvector_pop(dvector *dvect, dvalue **start)
{
    dvalue *val = dvect->top, *next;
    *start = dvect->top;
    if(dvect->pos <= 0) return 0;
    if(val->delta == 0) {
        dvect->pos--;
        next = val->next;
        dvect->top = next;
        return 1;
    } else {
        val->delta--;
        return 0;
    }
}

void dvector_time_to_delta(dvector *dvect)
{
    dvalue *val = dvect->root.next, *next;
    int i;
    int pdelta = 0;
    int tmp;
    for(i = 0; i < dvect->size; i++) {
        next = val->next;
        tmp = val->delta ;
        val->delta = val->delta - pdelta;
        pdelta = tmp;
        val = next;
    }
}

void dvalue_free(dvalue **val)
{
    dvalue *pval = *val;
    if(pval->nargs > 0) {
        free(pval->args);
    }
    free(*val);
}

void dvector_rewind(dvector *dv)
{
    dv->top = dv->root.next;
    dv->pos = dv->size;
}
