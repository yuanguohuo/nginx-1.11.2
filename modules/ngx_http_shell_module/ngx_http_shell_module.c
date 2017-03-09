#include <ngx_config.h>  
#include <ngx_core.h>  
#include <ngx_http.h>  
  
#include "util.h"  
#include "localfile.h"  
  
typedef struct  
{  
  ngx_str_t doc_root;  
  ngx_array_t* ls_opts;  
  ngx_str_t head_n;  
  ngx_str_t tail_n;  
} ngx_http_shell_loc_conf_t;  
  
//hooks in module  
static ngx_int_t init_master(ngx_log_t *log);  
static ngx_int_t init_module(ngx_cycle_t *cycle);  
static ngx_int_t init_process(ngx_cycle_t *cycle);  
static ngx_int_t init_thread(ngx_cycle_t *cycle);  
static void exit_thread(ngx_cycle_t *cycle);  
static void exit_process(ngx_cycle_t *cycle);  
static void exit_master(ngx_cycle_t *cycle);  
  
//hooks in context  
static ngx_int_t preconfiguration(ngx_conf_t *cf);  
static ngx_int_t postconfiguration(ngx_conf_t *cf);  
//static void* create_main_conf(ngx_conf_t *cf);  
static char* init_main_conf(ngx_conf_t *cf, void *conf);  
//static void* create_srv_conf(ngx_conf_t *cf);  
static char* merge_srv_conf(ngx_conf_t *cf, void *prev, void *conf);  
static void* create_loc_conf(ngx_conf_t *cf);  
static char* merge_loc_conf(ngx_conf_t *cf, void *prev, void *conf);  
  
//hooks in commands  
static char* ngx_http_shell_ls(ngx_conf_t* cf, ngx_command_t* cmd, void* conf);  
static char* ngx_http_shell_head(ngx_conf_t* cf, ngx_command_t* cmd, void* conf);  
static char* ngx_http_shell_tail(ngx_conf_t* cf, ngx_command_t* cmd, void* conf);  
static char* ngx_http_shell_cat(ngx_conf_t* cf, ngx_command_t* cmd, void* conf);  
  
static ngx_command_t ngx_http_shell_commands[] =  
{  
  {  
    ngx_string("document_root"),  
  
    //this cmd may appear in main, server or location block;  
    NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,  
  
    //this cmd does nothing but set variable head_def_n; so we use ngx_conf_set_{type}_slot as  
    //the handler;  
    ngx_conf_set_str_slot,  
    NGX_HTTP_LOC_CONF_OFFSET,  
    offsetof(ngx_http_shell_loc_conf_t, doc_root),  
    NULL  
  },  
  {  
    ngx_string("ls"),  
    NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,  //3 allowed options: -l -a -h; but it's treated as one param of array type  
    ngx_http_shell_ls,  
    NGX_HTTP_LOC_CONF_OFFSET,  
    offsetof(ngx_http_shell_loc_conf_t, ls_opts),  
    NULL  
  },  
  {  
    ngx_string("head"),  
    NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,    //1 allowed option: -num (similar to shell command "head -20")  
    ngx_http_shell_head,  
    NGX_HTTP_LOC_CONF_OFFSET,  
    offsetof(ngx_http_shell_loc_conf_t, head_n),  
    NULL  
  },  
  {  
    ngx_string("tail"),  
    NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,    //1 allowed option: num (similar to shell command "tail -20")  
    ngx_http_shell_tail,  
    NGX_HTTP_LOC_CONF_OFFSET,  
    offsetof(ngx_http_shell_loc_conf_t, tail_n),  
    NULL  
  },  
  {  
    ngx_string("cat"),  
    NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS,  
    ngx_http_shell_cat,  
    NGX_HTTP_LOC_CONF_OFFSET,  
    0,        //no args  
    NULL  
  },  
  ngx_null_command  
};  
  
static ngx_http_module_t ngx_http_shell_module_ctx =  
{  
  preconfiguration,  
  postconfiguration,  
  NULL,  
  init_main_conf,  
  NULL,  
  merge_srv_conf,  
  create_loc_conf,  
  merge_loc_conf  
};  
  
ngx_module_t ngx_http_shell_module =  
{  
  NGX_MODULE_V1,  
  &ngx_http_shell_module_ctx,  
  ngx_http_shell_commands,  
  NGX_HTTP_MODULE,  
  init_master,  
  init_module,  
  init_process,  
  init_thread,  
  exit_thread,  
  exit_process,  
  exit_master,  
  NGX_MODULE_V1_PADDING  
};  
  
//hooks in module  
static ngx_int_t init_master(ngx_log_t *log)  
{  
  ngx_log_error(NGX_LOG_EMERG, log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_log_error(NGX_LOG_EMERG, log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return NGX_OK;  
}
static ngx_int_t init_module(ngx_cycle_t *cycle)  
{  
  ngx_log_error(NGX_LOG_EMERG, cycle->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_log_error(NGX_LOG_EMERG, cycle->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return NGX_OK;  
}  
static ngx_int_t init_process(ngx_cycle_t *cycle)  
{  
  ngx_log_error(NGX_LOG_EMERG, cycle->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_log_error(NGX_LOG_EMERG, cycle->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return NGX_OK;  
}  
static ngx_int_t init_thread(ngx_cycle_t *cycle)  
{  
  ngx_log_error(NGX_LOG_EMERG, cycle->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_log_error(NGX_LOG_EMERG, cycle->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return NGX_OK;  
}  
static void exit_thread(ngx_cycle_t *cycle)  
{  
  ngx_log_error(NGX_LOG_EMERG, cycle->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_log_error(NGX_LOG_EMERG, cycle->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
}  
static void exit_process(ngx_cycle_t *cycle)  
{  
  ngx_log_error(NGX_LOG_EMERG, cycle->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_log_error(NGX_LOG_EMERG, cycle->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
}  
static void exit_master(ngx_cycle_t *cycle)  
{  
  ngx_log_error(NGX_LOG_EMERG, cycle->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_log_error(NGX_LOG_EMERG, cycle->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
}  
  
//hooks in context  
static ngx_int_t preconfiguration(ngx_conf_t *cf)  
{  
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return NGX_OK;  
}  
static ngx_int_t postconfiguration(ngx_conf_t *cf)  
{  
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return NGX_OK;  
}  
/* 
static void* create_main_conf(ngx_conf_t *cf) 
{ 
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return NULL; 
} 
*/  
static char* init_main_conf(ngx_conf_t *cf, void *conf)  
{  
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return NGX_CONF_OK;  
}  
/* 
static void* create_srv_conf(ngx_conf_t *cf) 
{ 
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return NULL; 
} 
*/  
static char* merge_srv_conf(ngx_conf_t *cf, void *prev, void *conf)  
{  
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return NGX_CONF_OK;  
}  
static void* create_loc_conf(ngx_conf_t* cf)  
{  
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_http_shell_loc_conf_t* conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_shell_loc_conf_t));  
  if (conf == NULL)  
  {  
    ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
    return NGX_CONF_ERROR;  
  }  
  conf->doc_root.len = 0;  
  conf->doc_root.data = NULL;  
  
  conf->ls_opts = NGX_CONF_UNSET_PTR;  
  
  conf->head_n.len = 0;  
  conf->head_n.data = NULL;  
  
  conf->tail_n.len = 0;  
  conf->tail_n.data = NULL;  
  
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return conf;  
}  
static char* merge_loc_conf(ngx_conf_t* cf, void* prev, void* conf)  
{  
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_http_shell_loc_conf_t* parent = prev;  
  ngx_http_shell_loc_conf_t* child = conf;  
  
  ngx_conf_merge_str_value(child->doc_root, parent->doc_root, "/tmp");  
  ngx_conf_merge_ptr_value(child->ls_opts, parent->ls_opts, NGX_CONF_UNSET_PTR);  
  ngx_conf_merge_str_value(child->head_n, parent->head_n, "-10");  
  ngx_conf_merge_str_value(child->tail_n, parent->tail_n, "-10");  
  
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return NGX_CONF_OK;  
}  
  
//hooks in commands  
static ngx_int_t ngx_http_shell_handler(ngx_http_request_t* r)  
{
  ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);

  ngx_int_t rc;  
  ngx_chain_t *first, *curr=NULL;  
  size_t content_len = 0;  
  
  ngx_http_shell_loc_conf_t* cf = ngx_http_get_module_loc_conf(r, ngx_http_shell_module);  
  
  ////////////////  string "uri: " ////////////////  
  fill_chain(r, (u_char*)"URI:\t\t", 6, 1, &curr, &content_len);  
  first = curr;  
  
  ////////////////  uri ////////////////  
  fill_chain(r, r->uri.data, r->uri.len, 1, &curr, &content_len);  
  
  ////////////////  carriage-return and line-feed ////////////////  
  fill_chain(r, (u_char*)"\r\n", 2, 1, &curr, &content_len);  
  
  ////////////////  headers in ////////////////  
  fill_chain(r, (u_char*)"HEADERS:\t", 9, 1, &curr, &content_len);  
  fill_header(r, r->headers_in.host,        &curr, &content_len);  
  fill_header(r, r->headers_in.user_agent,  &curr, &content_len);  
  
  ////////////////  carriage-return and line-feed ////////////////  
  fill_chain(r, (u_char*)"\r\n\r\n", 4, 1, &curr, &content_len);  
  
  //////////////// shell commands ////////////////  
  u_char pathname[256];  
  ngx_memset(pathname, '\0', 256);  
  
  ngx_memcpy(pathname, cf->doc_root.data, cf->doc_root.len);  
  
  //uri is like "/shell/{cmd}/file", we need to skip /shell/{cmd}  
  size_t pos = 0;  
  int c=0;  
  
  while(pos < r->uri.len)  
  {  
    if(*(r->uri.data+pos) == (u_char)'/')  
    {  
      if(++c == 3) break;  
    }  
    pos++;  
  }  
  ngx_memcpy(pathname+cf->doc_root.len, r->uri.data+pos, r->uri.len-pos);  
  
  fill_chain(r, (u_char*)"file=", 5, 1, &curr, &content_len);  
  fill_chain(r, pathname, cf->doc_root.len+r->uri.len-pos, 1, &curr, &content_len);  
  
  if(ngx_strncmp((char*)r->uri.data, "/shell/ls", 9) ==0)  
  {  
    fill_chain(r, (u_char*)" command=ls ", 12, 1, &curr, &content_len);  
    ngx_str_t* opt;  
    ngx_uint_t i;  
  
    int l=0;  
    int a=0;  
    int h=0;  
    for(i=0; i<cf->ls_opts->nelts; i++)  
    {  
      opt = ((ngx_str_t*)cf->ls_opts->elts) + i;  
      fill_chain(r, opt->data, opt->len, 1, &curr, &content_len);  
      if(ngx_strncmp((char*)opt->data, "-l", 2)==0)  
      {  
        l=1;  
      }  
      else if(ngx_strncmp((char*)opt->data, "-a", 2)==0)  
      {  
        a=1;  
      }  
      else if(ngx_strncmp((char*)opt->data, "-h", 2)==0)  
      {  
        h=1;  
      }  
      else  
      {  
        fill_chain(r, (u_char*)"\r\nError: unknown option ", 24, 1, &curr, &content_len);  
        fill_chain(r, opt->data, opt->len, 1, &curr, &content_len);  
      }  
    }  
    fill_chain(r, (u_char*)"\r\n", 2, 1, &curr, &content_len);  
  
    ls_file_dir(r, (char*)pathname, l, a, h, &curr, &content_len);  
  }  
  else if(ngx_strncmp((char*)r->uri.data, "/shell/head", 11) ==0)  
  {  
    fill_chain(r, (u_char*)" command=head ", 14, 1, &curr, &content_len);  
    fill_chain(r, cf->head_n.data, cf->head_n.len, 1, &curr, &content_len);  
    fill_chain(r, (u_char*)"\r\n", 2, 1, &curr, &content_len);  
  
    int headn = ngx_atoi(cf->head_n.data+1, cf->head_n.len-1); //note: head -10; +1 is to skip the '-'  
    head_file(r, (char*)pathname, headn, &curr, &content_len);  
  }  
  else if(ngx_strncmp((char*)r->uri.data, "/shell/tail", 11) ==0)  
  {  
    fill_chain(r, (u_char*)" command=tail ", 14, 1, &curr, &content_len);  
    fill_chain(r, cf->tail_n.data, cf->tail_n.len, 1, &curr, &content_len);  
    fill_chain(r, (u_char*)"\r\n", 2, 1, &curr, &content_len);  
  
    int tailn = ngx_atoi(cf->tail_n.data+1, cf->tail_n.len-1); //note: head -10; +1 is to skip the '-'  
    tail_file(r, (char*)pathname, tailn, &curr, &content_len);  
  }  
  else if(ngx_strncmp((char*)r->uri.data, "/shell/cat", 10) ==0)  
  {  
    fill_chain(r, (u_char*)" command=cat\r\n", 14, 1, &curr, &content_len);  
    cat_file(r, (char*)pathname, &curr, &content_len);  
  }  
  else  
  {  
    fill_chain(r, (u_char*)" command=unknown\r\n", 18, 1, &curr, &content_len);  
  }  
  
  //mark curr buf as last buf  
  curr->buf->last_buf = 1;  
  curr->next = NULL;  
  
  r->headers_out.content_type.len = sizeof("text/html") - 1;  
  r->headers_out.content_type.data = (u_char*)"text/html";  
  r->headers_out.status = NGX_HTTP_OK;  
  r->headers_out.content_length_n = content_len;  
  
  rc = ngx_http_send_header(r);  
  if (rc == NGX_ERROR || rc > NGX_OK)  
  {  
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "YuanguoDbg %s:%d %s Failed to send http header.", __FILE__,__LINE__,__func__);
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
    return rc;  
  }  
  
  ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return ngx_http_output_filter(r, first);  
}  
  
static char* ngx_http_shell_ls(ngx_conf_t* cf, ngx_command_t* cmd, void* conf)  
{  
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_http_core_loc_conf_t* clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);  
  clcf->handler = ngx_http_shell_handler;  
  ngx_conf_set_str_array_slot(cf,cmd,conf);  
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return NGX_CONF_OK;  
}  
static char* ngx_http_shell_head(ngx_conf_t* cf, ngx_command_t* cmd, void* conf)  
{  
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_http_core_loc_conf_t* clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);  
  clcf->handler = ngx_http_shell_handler;  
  ngx_conf_set_str_slot(cf,cmd,conf);  
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return NGX_CONF_OK;  
}  
  
static char* ngx_http_shell_tail(ngx_conf_t* cf, ngx_command_t* cmd, void* conf)  
{  
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_http_core_loc_conf_t* clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);  
  clcf->handler = ngx_http_shell_handler;  
  ngx_conf_set_str_slot(cf,cmd,conf);  
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return NGX_CONF_OK;  
}  

static char* ngx_http_shell_cat(ngx_conf_t* cf, ngx_command_t* cmd, void* conf)  
{  
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Enter", __FILE__,__LINE__,__func__);
  ngx_http_core_loc_conf_t* clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);  
  clcf->handler = ngx_http_shell_handler;  
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "YuanguoDbg %s:%d %s Exit", __FILE__,__LINE__,__func__);
  return NGX_CONF_OK;  
}
