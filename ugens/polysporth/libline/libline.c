/*1:*/
#line 10 "./libline.w"

#include <stdlib.h> 
#include <stdio.h> 
#include <math.h> 
#ifdef LL_SPORTH_UGEN
#include "plumber.h"
#endif
#include "line.h"
/*42:*/
#line 3 "./point.w"
/*44:*/
#line 9 "./point.w"

struct ll_point{
ll_flt A;
ll_flt dur;
ll_flt*B;

/*:44*//*45:*/
#line 18 "./point.w"

ll_point*next;

/*:45*//*46:*/
#line 24 "./point.w"

ll_cb_malloc malloc;
ll_cb_free free;
void*ud;

/*:46*//*47:*/
#line 32 "./point.w"

void*data;
ll_cb_free destroy;

/*:47*//*48:*/
#line 38 "./point.w"

ll_cb_step step;


};

/*:48*//*50:*/
#line 47 "./point.w"

size_t ll_point_size(void)
{
return sizeof(ll_point);
}

/*:50*//*51:*/
#line 55 "./point.w"

/*64:*/
#line 164 "./point.w"

static ll_flt step(ll_point*pt,void*ud,UINT pos,UINT dur)
{
return ll_point_A(pt);
}

/*:64*//*65:*/
#line 173 "./point.w"

void ll_point_data(ll_point*pt,void*data)
{
pt->data= data;
}

void ll_point_cb_step(ll_point*pt,ll_cb_step stp)
{
pt->step= stp;
}

void ll_point_cb_destroy(ll_point*pt,ll_cb_free destroy)
{
pt->destroy= destroy;
}

/*:65*/
#line 56 "./point.w"

void ll_point_init(ll_point*pt)
{
pt->A= 1.0;
pt->dur= 1.0;
pt->B= &pt->A;
pt->ud= NULL;
pt->free= ll_free;
pt->malloc= ll_malloc;
pt->data= NULL;
pt->destroy= ll_free_nothing;
pt->step= step;
}

/*:51*//*53:*/
#line 74 "./point.w"

void ll_point_value(ll_point*pt,ll_flt val)
{
pt->A= val;
}


/*:53*//*54:*/
#line 83 "./point.w"

void ll_point_set_next_value(ll_point*pt,ll_flt*val)
{
pt->B= val;
}

/*:54*//*55:*/
#line 90 "./point.w"

void ll_point_dur(ll_point*pt,ll_flt dur)
{
pt->dur= dur;
}

ll_flt ll_point_get_dur(ll_point*pt)
{
return pt->dur;
}

/*:55*//*56:*/
#line 102 "./point.w"

void ll_point_set_next_point(ll_point*pt,ll_point*next)
{
pt->next= next;
}

/*:56*//*57:*/
#line 109 "./point.w"

ll_point*ll_point_get_next_point(ll_point*pt)
{
return pt->next;
}

/*:57*//*58:*/
#line 118 "./point.w"

ll_flt*ll_point_get_value(ll_point*pt)
{
return&pt->A;
}

/*:58*//*59:*/
#line 126 "./point.w"

ll_flt ll_point_A(ll_point*pt)
{
return pt->A;
}

ll_flt ll_point_B(ll_point*pt)
{
return*pt->B;
}

/*:59*//*61:*/
#line 142 "./point.w"

void*ll_point_malloc(ll_point*pt,size_t size)
{
return pt->malloc(pt->ud,size);
}
void ll_point_free(ll_point*pt,void*ptr)
{
pt->free(pt->ud,ptr);
}

/*:61*//*62:*/
#line 154 "./point.w"

void ll_point_destroy(ll_point*pt)
{
pt->destroy(pt,pt->data);
}

/*:62*//*66:*/
#line 190 "./point.w"

ll_flt ll_point_step(ll_point*pt,UINT pos,UINT dur)
{
return pt->step(pt,pt->data,pos,dur);
}

/*:66*//*67:*/
#line 199 "./point.w"

void ll_point_mem_callback(ll_point*pt,ll_cb_malloc m,ll_cb_free f)
{
pt->malloc= m;
pt->free= f;
}
#line 1 "./line.w"
/*:67*/
#line 3 "./point.w"


/*:42*//*68:*/
#line 4 "./line.w"
/*70:*/
#line 10 "./line.w"

struct ll_line{
ll_point*root;
ll_point*last;
int size;
int curpos;

/*:70*//*71:*/
#line 20 "./line.w"

int sr;

/*:71*//*72:*/
#line 26 "./line.w"

unsigned int counter;

/*:72*//*73:*/
#line 32 "./line.w"

unsigned int idur;

/*:73*//*74:*/
#line 38 "./line.w"

ll_cb_malloc malloc;
ll_cb_free free;

/*:74*//*75:*/
#line 44 "./line.w"

void*ud;

/*:75*//*76:*/
#line 49 "./line.w"

int end;

/*:76*//*77:*/
#line 54 "./line.w"

ll_flt tscale;
};

/*:77*//*78:*/
#line 59 "./line.w"

size_t ll_line_size(void)
{
return sizeof(ll_line);
}


/*:78*//*80:*/
#line 71 "./line.w"

void ll_line_init(ll_line*ln,int sr)
{
ln->root= NULL;
ln->last= NULL;
ln->size= 0;
ln->sr= sr;
ln->malloc= ll_malloc;
ln->free= ll_free;
ln->idur= 0;
ln->counter= 0;
ln->curpos= 0;
ln->end= 0;
ln->tscale= 1.0;
}

/*:80*//*81:*/
#line 91 "./line.w"

void ll_line_timescale(ll_line*ln,ll_flt scale)
{
ln->tscale= scale;
}

/*:81*//*83:*/
#line 110 "./line.w"

void ll_line_append_point(ll_line*ln,ll_point*p)
{
if(ln->size==0){
ln->root= p;
}else{
ll_point_set_next_point(ln->last,p);
ll_point_set_next_value(ln->last,ll_point_get_value(p));
}
ln->last= p;
ln->size++;
}


/*:83*//*84:*/
#line 133 "./line.w"

ll_point*ll_line_append(ll_line*ln,ll_flt val,ll_flt dur)
{
ll_point*pt;

pt= ln->malloc(ln->ud,ll_point_size());

ll_point_init(pt);
ll_point_value(pt,val);
ll_point_dur(pt,dur);
ll_point_mem_callback(pt,ln->malloc,ln->free);

ll_line_append_point(ln,pt);

return pt;
}

/*:84*//*85:*/
#line 153 "./line.w"

void ll_line_done(ll_line*ln)
{
ln->curpos= 0;
ln->last= ln->root;
ln->idur= ll_point_get_dur(ln->root)*ln->sr*ln->tscale;
ln->counter= ln->idur;
ln->end= 0;
}

/*:85*//*87:*/
#line 167 "./line.w"

void ll_line_free(ll_line*ln)
{
ll_point*pt;
ll_point*next;
unsigned int i;

pt= ln->root;
for(i= 0;i<ln->size;i++){
next= ll_point_get_next_point(pt);
ll_point_destroy(pt);
ln->free(ln->ud,pt);
pt= next;
}
}

/*:87*//*89:*/
#line 188 "./line.w"

ll_flt ll_line_step(ll_line*ln)
{
UINT dur;
UINT pos;
/*:89*//*90:*/
#line 197 "./line.w"

if(ln->end){
return ll_point_A(ln->last);
}

/*:90*//*91:*/
#line 205 "./line.w"

if(ln->counter==0){

/*:91*//*92:*/
#line 213 "./line.w"

if(ln->curpos<(ln->size-1)){

/*:92*//*93:*/
#line 219 "./line.w"

ln->last= ll_point_get_next_point(ln->last);
ln->idur= ll_point_get_dur(ln->last)*ln->sr*ln->tscale;
ln->counter= ln->idur;
ln->curpos++;
/*:93*//*94:*/
#line 226 "./line.w"

}else{
ln->end= 1;
}
}

/*:94*//*95:*/
#line 237 "./line.w"

dur= ln->idur;
pos= dur-ln->counter;
ln->counter--;
return ll_point_step(ln->last,pos,dur);
}

/*:95*//*97:*/
#line 248 "./line.w"

void ll_line_print(ll_line*ln)
{
ll_point*pt;
ll_point*next;
unsigned int i;
ll_flt*val;

pt= ln->root;
printf("there are %d points...\n",ln->size);
for(i= 0;i<ln->size;i++){
next= ll_point_get_next_point(pt);
val= ll_point_get_value(pt);
printf("point %d: dur %g, val %g\n",
i,
ll_point_get_dur(pt),
*val
);

pt= next;
}
}
#line 1 "./lines.w"
/*:97*/
#line 4 "./line.w"


/*:68*//*98:*/
#line 4 "./lines.w"
/*100:*/
#line 10 "./lines.w"

typedef struct ll_line_entry{
ll_line*ln;
ll_flt val;
struct ll_line_entry*next;
}ll_line_entry;

/*:100*//*101:*/
#line 19 "./lines.w"

struct ll_lines{
ll_line_entry*root;
ll_line_entry*last;
unsigned int size;
int sr;
ll_cb_malloc malloc;
ll_cb_free free;
void*ud;
ll_line*line;
ll_point*pt;
ll_flt tscale;
};

/*:101*//*103:*/
#line 37 "./lines.w"


size_t ll_lines_size()
{
return sizeof(ll_lines);
}

/*:103*//*104:*/
#line 46 "./lines.w"

void ll_lines_init(ll_lines*l,int sr)
{
l->root= NULL;
l->last= NULL;
l->size= 0;
l->malloc= ll_malloc;
l->free= ll_free;
l->sr= sr;
l->tscale= 1.0;
}

/*:104*//*106:*/
#line 62 "./lines.w"

void ll_lines_mem_callback(ll_lines*l,void*ud,ll_cb_malloc m,ll_cb_free f)
{
l->malloc= m;
l->free= f;
l->ud= ud;
}

/*:106*//*107:*/
#line 71 "./lines.w"

void ll_lines_free(ll_lines*l)
{
unsigned int i;
ll_line_entry*entry;
ll_line_entry*next;

entry= l->root;

for(i= 0;i<l->size;i++){
next= entry->next;
ll_line_free(entry->ln);
l->free(l->ud,entry->ln);
l->free(l->ud,entry);
entry= next;
}
}

/*:107*//*109:*/
#line 94 "./lines.w"

void ll_lines_append(ll_lines*l,ll_line**line,ll_flt**val)
{
ll_line_entry*entry;

entry= l->malloc(l->ud,sizeof(ll_line_entry));
entry->val= 0.f;
entry->ln= l->malloc(l->ud,ll_line_size());
ll_line_init(entry->ln,l->sr);
ll_line_timescale(entry->ln,l->tscale);

if(line!=NULL)*line= entry->ln;
if(val!=NULL)*val= &entry->val;

if(l->size==0){
l->root= entry;
}else{
l->last->next= entry;
}

l->size++;
l->last= entry;
l->line= entry->ln;
}

/*:109*//*111:*/
#line 123 "./lines.w"

void ll_lines_step(ll_lines*l)
{
unsigned int i;
ll_line_entry*entry;

entry= l->root;

for(i= 0;i<l->size;i++){
entry->val= ll_line_step(entry->ln);
entry= entry->next;
}
}

/*:111*//*112:*/
#line 141 "./lines.w"

void ll_add_linpoint(ll_lines*l,ll_flt val,ll_flt dur)
{
ll_point*pt;
pt= ll_line_append(l->line,val,dur);
ll_linpoint(pt);
}

void ll_add_exppoint(ll_lines*l,ll_flt val,ll_flt dur,ll_flt curve)
{
ll_point*pt;
pt= ll_line_append(l->line,val,dur);
ll_exppoint(pt,curve);
}

void ll_add_step(ll_lines*l,ll_flt val,ll_flt dur)
{
ll_line_append(l->line,val,dur);
}

void ll_add_tick(ll_lines*l,ll_flt dur)
{
ll_point*pt;
pt= ll_line_append(l->line,0.0,dur);
ll_tick(pt);
}

void ll_end(ll_lines*l)
{
ll_line_done(l->line);
}

void ll_timescale(ll_lines*l,ll_flt scale)
{
l->tscale= scale;
}

void ll_timescale_bpm(ll_lines*l,ll_flt bpm)
{
l->tscale= 60.0/bpm;
}
#line 1 "./linpoint.w"
/*:112*/
#line 4 "./lines.w"


/*:98*//*113:*/
#line 3 "./linpoint.w"

/*114:*/
#line 9 "./linpoint.w"

typedef struct{
ll_flt inc;
ll_flt acc;
}linpoint;

/*:114*//*115:*/
#line 18 "./linpoint.w"

/*116:*/
#line 33 "./linpoint.w"


static ll_flt linpoint_step(ll_point*pt,void*ud,UINT pos,UINT dur)
{
linpoint*lp;
ll_flt val;

lp= ud;

if(pos==0){
lp->acc= ll_point_A(pt);
lp->inc= (ll_point_B(pt)-ll_point_A(pt))/dur;
}

val= lp->acc;
lp->acc+= lp->inc;
return val;
}

/*:116*//*117:*/
#line 53 "./linpoint.w"

static void ll_linpoint_destroy(void*ud,void*ptr)
{
ll_point*pt;
pt= ud;
ll_point_free(pt,ptr);
}

#line 1 "./exppoint.w"
/*:117*/
#line 19 "./linpoint.w"

void ll_linpoint(ll_point*pt)
{
linpoint*lp;
lp= ll_point_malloc(pt,sizeof(linpoint));
ll_point_cb_step(pt,linpoint_step);
ll_point_data(pt,lp);
ll_point_cb_destroy(pt,ll_linpoint_destroy);
}

/*:115*/
#line 4 "./linpoint.w"


/*:113*//*118:*/
#line 3 "./exppoint.w"

/*119:*/
#line 8 "./exppoint.w"


typedef struct{
SPFLOAT curve;
}exppoint;

/*:119*//*120:*/
#line 15 "./exppoint.w"

/*121:*/
#line 48 "./exppoint.w"

static ll_flt exppoint_step(ll_point*pt,void*ud,UINT pos,UINT dur)
{
exppoint*ep;
ll_flt val;

ep= ud;

val= ll_point_A(pt)+
(ll_point_B(pt)-ll_point_A(pt))*
(1-exp(pos*ep->curve/(dur-1)))/(1-exp(ep->curve));
return val;
}

/*:121*//*122:*/
#line 64 "./exppoint.w"

static void exppoint_destroy(void*ud,void*ptr)
{
ll_point*pt;
pt= ud;
ll_point_free(pt,ptr);
}

#line 1 "./tick.w"
/*:122*/
#line 16 "./exppoint.w"

void ll_exppoint(ll_point*pt,ll_flt curve)
{
exppoint*ep;
ep= ll_point_malloc(pt,sizeof(exppoint));
ep->curve= curve;
ll_point_cb_step(pt,exppoint_step);
ll_point_data(pt,ep);
ll_point_cb_destroy(pt,exppoint_destroy);
}


/*:120*/
#line 4 "./exppoint.w"


/*:118*//*123:*/
#line 5 "./tick.w"
/*124:*/
#line 10 "./tick.w"


static ll_flt tick_step(ll_point*pt,void*ud,UINT pos,UINT dur)
{
if(pos==0){
return 1.0;
}else{
return 0.0;
}
}

/*:124*//*125:*/
#line 23 "./tick.w"

void ll_tick(ll_point*pt)
{
ll_point_cb_step(pt,tick_step);
}
#line 1 "./mem.w"
/*:125*/
#line 5 "./tick.w"


/*:123*//*126:*/
#line 8 "./mem.w"

void*ll_malloc(void*ud,size_t size)
{
return malloc(size);
}

void ll_free(void*ud,void*ptr)
{
free(ptr);
}

void ll_free_nothing(void*ud,void*ptr)
{

}
#line 1 "./sporth.w"
/*:126*//*127:*/
#line 4 "./sporth.w"

#ifdef LL_SPORTH
/*128:*/
#line 15 "./sporth.w"

/*130:*/
#line 40 "./sporth.w"

static int sporth_ll(plumber_data*pd,sporth_stack*stack,void**ud)
{
ll_lines*l;
l= *ud;
if(pd->mode==PLUMBER_COMPUTE)ll_lines_step(l);
return PLUMBER_OK;
}
/*:130*/
#line 16 "./sporth.w"

void ll_sporth_ugen(ll_lines*l,plumber_data*pd,const char*ugen)
{
plumber_ftmap_add_function(pd,ugen,sporth_ll,l);
}

/*:128*//*129:*/
#line 24 "./sporth.w"

ll_line*ll_sporth_line(ll_lines*l,plumber_data*pd,const char*name)
{
ll_line*ln;
ll_flt*val;

ll_lines_append(l,&ln,&val);

plumber_ftmap_delete(pd,0);
plumber_ftmap_add_userdata(pd,name,val);
plumber_ftmap_delete(pd,1);
return ln;
}

/*:129*/
#line 6 "./sporth.w"

#endif

/*:127*/
#line 18 "./libline.w"


#line 1 "./header.w"
/*:1*/
