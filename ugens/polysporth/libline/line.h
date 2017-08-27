/*2:*/
#line 5 "./header.w"

#ifndef LINE_H
#define LINE_H
#ifdef LL_SPORTH_STANDALONE
#include <soundpipe.h> 
#include <sporth.h> 
#endif

/*4:*/
#line 22 "./header.w"

#ifndef LLFLOAT
typedef float ll_flt;
#else
typedef LLFLOAT ll_flt;
#endif
#define UINT unsigned int
/*:4*//*5:*/
#line 31 "./header.w"

typedef struct ll_point ll_point;

/*:5*//*6:*/
#line 36 "./header.w"

typedef struct ll_line ll_line;

/*:6*//*7:*/
#line 40 "./header.w"

typedef struct ll_lines ll_lines;


/*:7*//*8:*/
#line 48 "./header.w"

typedef void*(*ll_cb_malloc)(void*ud,size_t size);
typedef void(*ll_cb_free)(void*ud,void*ptr);

/*:8*//*9:*/
#line 54 "./header.w"

typedef ll_flt(*ll_cb_step)(ll_point*pt,void*ud,UINT pos,UINT dur);

/*:9*//*11:*/
#line 60 "./header.w"

void*ll_malloc(void*ud,size_t size);
void ll_free(void*ud,void*ptr);
void ll_free_nothing(void*ud,void*ptr);

/*:11*//*12:*/
#line 68 "./header.w"

void*ll_point_malloc(ll_point*pt,size_t size);
void ll_point_free(ll_point*pt,void*ptr);
void ll_point_destroy(ll_point*pt);

/*:12*//*14:*/
#line 77 "./header.w"

size_t ll_line_size(void);
size_t ll_point_size(void);

/*:14*//*15:*/
#line 84 "./header.w"

void ll_point_init(ll_point*pt);
void ll_line_init(ll_line*ln,int sr);

/*:15*//*17:*/
#line 91 "./header.w"

void ll_point_value(ll_point*pt,ll_flt val);
void ll_point_dur(ll_point*pt,ll_flt dur);
ll_flt ll_point_get_dur(ll_point*pt);

/*:17*//*18:*/
#line 97 "./header.w"

void ll_point_set_next_value(ll_point*pt,ll_flt*val);

/*:18*//*19:*/
#line 101 "./header.w"

ll_flt ll_point_A(ll_point*pt);
ll_flt ll_point_B(ll_point*pt);

/*:19*//*20:*/
#line 107 "./header.w"

ll_flt*ll_point_get_value(ll_point*pt);

/*:20*//*21:*/
#line 112 "./header.w"

void ll_point_set_next_point(ll_point*pt,ll_point*next);

/*:21*//*22:*/
#line 118 "./header.w"

ll_point*ll_point_get_next_point(ll_point*pt);

/*:22*//*23:*/
#line 122 "./header.w"

ll_flt ll_point_step(ll_point*pt,UINT pos,UINT dur);

/*:23*//*24:*/
#line 126 "./header.w"

void ll_point_data(ll_point*pt,void*data);
void ll_point_cb_step(ll_point*pt,ll_cb_step stp);
void ll_point_cb_destroy(ll_point*pt,ll_cb_free destroy);

/*:24*//*25:*/
#line 133 "./header.w"

void ll_point_mem_callback(ll_point*pt,ll_cb_malloc m,ll_cb_free f);

/*:25*//*27:*/
#line 140 "./header.w"

void ll_line_append_point(ll_line*ln,ll_point*p);

/*:27*//*28:*/
#line 149 "./header.w"

ll_point*ll_line_append(ll_line*ln,ll_flt val,ll_flt dur);

/*:28*//*29:*/
#line 155 "./header.w"

void ll_line_free(ll_line*ln);

/*:29*//*30:*/
#line 160 "./header.w"

void ll_line_mem_callback(ll_line*ln,ll_cb_malloc m,ll_cb_free f);


/*:30*//*31:*/
#line 168 "./header.w"

void ll_line_done(ll_line*ln);


/*:31*//*32:*/
#line 175 "./header.w"

ll_flt ll_line_step(ll_line*ln);


/*:32*//*33:*/
#line 180 "./header.w"

void ll_line_print(ll_line*ln);

/*:33*//*34:*/
#line 184 "./header.w"

void ll_linpoint(ll_point*pt);

/*:34*//*35:*/
#line 188 "./header.w"

void ll_exppoint(ll_point*pt,ll_flt curve);

/*:35*//*36:*/
#line 192 "./header.w"

void ll_tick(ll_point*pt);

/*:36*//*38:*/
#line 198 "./header.w"

size_t ll_lines_size();
void ll_lines_init(ll_lines*l,int sr);
void ll_lines_mem_callback(ll_lines*l,void*ud,ll_cb_malloc m,ll_cb_free f);
void ll_lines_append(ll_lines*l,ll_line**line,ll_flt**val);
void ll_lines_step(ll_lines*l);
void ll_lines_free(ll_lines*l);

/*:38*//*39:*/
#line 208 "./header.w"

void ll_add_linpoint(ll_lines*l,ll_flt val,ll_flt dur);
void ll_add_exppoint(ll_lines*l,ll_flt val,ll_flt dur,ll_flt curve);
void ll_add_step(ll_lines*l,ll_flt val,ll_flt dur);
void ll_add_tick(ll_lines*l,ll_flt dur);
void ll_end(ll_lines*l);

/*:39*//*40:*/
#line 218 "./header.w"

#ifdef LL_SPORTH
void ll_sporth_ugen(ll_lines*l,plumber_data*pd,const char*ugen);
ll_line*ll_sporth_line(ll_lines*l,plumber_data*pd,const char*name);
#endif

#line 1 "./point.w"
/*:40*/
#line 13 "./header.w"

#endif

/*:2*/
