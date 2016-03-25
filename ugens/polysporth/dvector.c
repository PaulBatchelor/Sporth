#include <stdio.h>
#include <stdlib.h>
#include "plumber.h"
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
        printf("%d\n", val->delta);
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
    int grp_start, int grp_end, int delta, int dur) 
{
    dvalue *new = malloc(sizeof(dvalue));
    new->delta = delta;
    new->grp_start = grp_start;
    new->grp_end = grp_end;
    new->dur = dur;
    dvector_append_value(dv, new);
}

void dvector_init(dvector *dv) 
{
    dv->size = 0;
    dv->last = &dv->root;
}

void dvector_free(dvector *dv)
{
    int i;
    dvalue *val = dv->root.next;
    dvalue *next;
    for(i = 0; i < dv->size; i++) {
        next = val->next;
        free(val);
        val = next;
    }
}

dvector dvector_merge(dvector *dvect1, dvector *dvect2)
{
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

    printf("m_1\tm_2\t<=\tout_d\tout_t\n");
    for(i = 0; i < dvect1->size + dvect2->size; i++) {
        if((dv1_t <= dv2_t || append)) {
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
}

void dvector_pop(dvector *dvect, int *nvoice, dvalue **start)
{
    *nvoice = 0;
    dvalue *val = dvect->root.next, *next;
    *start = dvect->root.next;
    int run = 1;
    if(dvect->size == 0) return;
    while(run != 0 && dvect->size != 0) {
        if(val->delta == 0) {
            dvect->size--;
            next = val->next;
            //free(val);
            *nvoice = *nvoice + 1;
            dvect->root.next = next;
            val = next;
        } else {
            val->delta--;
            run = 0;
        }
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

//int main()
//{
//    dvector dv1, dv2;
//
//    dvector_init(&dv1);
//
//    dvector_append(&dv1, 0);
//    dvector_append(&dv1, 3);
//    dvector_append(&dv1, 8);
//
//    dvector_print(&dv1);
//    
//    dvector_init(&dv2);
//    dvector_append(&dv2, 0);
//    dvector_append(&dv2, 2);
//    dvector_append(&dv2, 4);
//    dvector_append(&dv2, 6);
//    dvector_append(&dv2, 1);
//    dvector_append(&dv2, 9);
//    dvector_append(&dv2, 1);
//    dvector_append(&dv2, 2);
//    dvector_append(&dv2, 3);
//    
//    dvector_print(&dv2);
//
//    printf("---\n");
//    dvector merge = dvector_merge(&dv1, &dv2);
//    printf("---\n");
//    dvector_print(&merge);
//
//    dvector_free(&merge);
//
//    printf("---\n");
//
//    dvector dv3;
//    dvector_init(&dv3);
//    dvector_append(&dv3, 0);
//    dvector_append(&dv3, 1);
//    dvector_append(&dv3, 1);
//    dvector_append(&dv3, 4);
//    dvector_append(&dv3, 9);
//    dvector_time_to_delta(&dv3);
//    dvector_print(&dv3);
//
//    int i,v;
//    int nvoices = 0;
//    printf("---\n");
//    for(i = 0; i < 15; i++) {
//        printf("%d: ", i);
//        dvector_pop(&dv3, &nvoices);
//        for(v = 0; v < nvoices; v++) {
//            printf("%d ", v); 
//        }
//        printf("\n");
//    }
//
//    dvector_free(&dv3);
//    return 0;
//}
