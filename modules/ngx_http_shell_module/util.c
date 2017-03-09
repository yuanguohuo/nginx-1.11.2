#include "util.h"  
  
ngx_int_t fill_chain(ngx_http_request_t* r,  
                     u_char*             data,  
                     size_t              len,  
                     int                 memory,  
                     ngx_chain_t**       chain,  
                     size_t*             content_len)  
{  
  ngx_buf_t* b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));  
  if(NULL == b)  
  {  
    ngx_log_error(NGX_LOG_ERR,  
                  r->connection->log,  
                  0,  
                  "Failed to allocate response buffer.");  
    return NGX_HTTP_INTERNAL_SERVER_ERROR;  
  }  
  
  b->pos = data;  
  b->last = b->pos + len;  
  if(memory)  
  {  
    b->memory = 1;  
  }  
  
  ngx_chain_t* out = ngx_pcalloc(r->pool, sizeof(ngx_chain_t));  
  if(NULL == out)  
  {  
    ngx_log_error(NGX_LOG_ERR,  
                  r->connection->log,  
                  0,  
                  "Failed to allocate chain.");  
    return NGX_HTTP_INTERNAL_SERVER_ERROR;  
  }  
  out->buf = b;  
  
  *content_len += len;  
  if(NULL == *chain)  
  {  
    *chain = out;  
  }  
  else  
  {  
    (*chain)->next = out;  
    *chain = (*chain)->next;  
  }  
  
  return NGX_OK;  
}  
  
ngx_int_t fill_header(ngx_http_request_t* r,  
                      ngx_table_elt_t*    header,  
                      ngx_chain_t**       curr,  
                      size_t*             content_len)  
{  
  fill_chain(r, header->key.data,   header->key.len,    1, curr, content_len);  
  fill_chain(r, (u_char*)"=",       1,                  1, curr, content_len);  
  fill_chain(r, header->value.data, header->value.len,  1, curr, content_len);  
  fill_chain(r, (u_char*)"  ",      2,                  1, curr, content_len);  
  
  return NGX_OK;  
}  
