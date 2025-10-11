/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ansi_color.c
 * PURPOSE: Implementation of ANSI SGR color parser for GtkTextBuffer
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/ansi_color.h"

struct _UmiAnsi { GtkTextBuffer *buf; };

static void ensure(GtkTextBuffer *b, const char *name, const char *prop, const char *val){
  GtkTextTagTable *tt = gtk_text_buffer_get_tag_table(b);
  if(gtk_text_tag_table_lookup(tt, name)) return;
  GtkTextTag *t = gtk_text_tag_new(name);
  g_object_set(G_OBJECT(t), prop, val, NULL);
  gtk_text_tag_table_add(tt, t);
}

void umi_ansi_ensure_tags(GtkTextBuffer *b){
  ensure(b, "ansi-bold", "weight", "700");
  ensure(b, "ansi-italic", "style", "italic");
  ensure(b, "ansi-underline", "underline", "single");
  ensure(b, "ansi-red", "foreground", "red");
  ensure(b, "ansi-yellow", "foreground", "#e0a000");
  ensure(b, "ansi-green", "foreground", "green");
  ensure(b, "ansi-blue", "foreground", "blue");
  ensure(b, "ansi-magenta", "foreground", "magenta");
  ensure(b, "ansi-cyan", "foreground", "cyan");
  ensure(b, "ansi-gray", "foreground", "gray");
}

UmiAnsi *umi_ansi_new(GtkTextBuffer *buf){
  if(!buf) return NULL;
  umi_ansi_ensure_tags(buf);
  UmiAnsi *a = g_new0(UmiAnsi,1);
  a->buf = buf;
  return a;
}

void umi_ansi_free(UmiAnsi *a){ if(a) g_free(a); }

/* Supported subset:
 * - CSI m with 0 reset, 1 bold, 3 italic, 4 underline
 * - 30..37 fg colors; 90..97 bright colors (map to base colors)
 */
typedef struct {
  gboolean bold, italic, underline;
  const char *color_tag; /* e.g., "ansi-red" */
} Style;

static void apply_style(GtkTextBuffer *b, GtkTextIter *s, GtkTextIter *e, const Style *st){
  if(st->bold)      gtk_text_buffer_apply_tag_by_name(b, "ansi-bold", s, e);
  if(st->italic)    gtk_text_buffer_apply_tag_by_name(b, "ansi-italic", s, e);
  if(st->underline) gtk_text_buffer_apply_tag_by_name(b, "ansi-underline", s, e);
  if(st->color_tag) gtk_text_buffer_apply_tag_by_name(b, st->color_tag, s, e);
}

static const char *map_color(int code){
  switch(code){
    case 30: case 90: return "ansi-gray";
    case 31: case 91: return "ansi-red";
    case 32: case 92: return "ansi-green";
    case 33: case 93: return "ansi-yellow";
    case 34: case 94: return "ansi-blue";
    case 35: case 95: return "ansi-magenta";
    case 36: case 96: return "ansi-cyan";
    case 37: case 97: return "ansi-gray";
    default: return NULL;
  }
}

static void reset(Style *s){ s->bold=s->italic=s->underline=FALSE; s->color_tag=NULL; }

void umi_ansi_append_line(UmiAnsi *a, const char *ansi_text){
  if(!a || !ansi_text) return;
  GtkTextBuffer *b = a->buf;
  GtkTextIter end;
  gtk_text_buffer_get_end_iter(b, &end);

  Style st; reset(&st);
  GtkTextIter seg_start = end, seg_end = end;

  const char *p = ansi_text;
  while(*p){
    if(*p == '\x1b' && *(p+1)=='['){
      /* flush previous segment */
      gtk_text_buffer_get_end_iter(b, &seg_end);
      apply_style(b, &seg_start, &seg_end, &st);

      /* parse SGR like ESC[31m or ESC[0;1m */
      p += 2;
      int nums[8]; int n=0; int v=0; gboolean have_num=FALSE;
      while(*p && *p!='m' && n<8){
        if(g_ascii_isdigit(*p)){ v = v*10 + (*p - '0'); have_num=TRUE; }
        else if(*p==';'){ nums[n++] = have_num ? v : 0; v=0; have_num=FALSE; }
        else break;
        p++;
      }
      if(have_num && n<8) nums[n++] = v;
      if(*p=='m') p++;

      for(int i=0;i<n;i++){
        int code = nums[i];
        if(code==0) reset(&st);
        else if(code==1) st.bold = TRUE;
        else if(code==3) st.italic = TRUE;
        else if(code==4) st.underline = TRUE;
        else if((code>=30 && code<=37) || (code>=90 && code<=97)) st.color_tag = map_color(code);
      }
      gtk_text_buffer_get_end_iter(b, &seg_start);
    }else{
      gunichar ch = g_utf8_get_char(p);
      /* length computed implicitly; no temp variable needed */ g_unichar_to_utf8(ch, NULL);
      /* append char */
      char tmp[8]; int w = g_unichar_to_utf8(ch, tmp);
      gtk_text_buffer_insert(b, &end, tmp, w);
      p = g_utf8_next_char(p);
    }
  }
  /* final apply */
  gtk_text_buffer_get_end_iter(b, &seg_end);
  apply_style(b, &seg_start, &seg_end, &st);
  gtk_text_buffer_insert(b, &end, "\n", 1);
}
