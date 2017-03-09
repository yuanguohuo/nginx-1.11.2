#ifndef __NGX_HTTP_SHELL_UTIL_H__  
#define __NGX_HTTP_SHELL_UTIL_H__  
  
#include <ngx_config.h>  
#include <ngx_core.h>  
#include <ngx_http.h>  
  
//build a chain whose content is data;  
//param r:            we allocate memory from r->pool;  
//param data:         the content of the chain;  
//param len:          the length of the content;  
//param memory:       if the buf is in memory;  
//param chain:        output param. if it's NULL, make it point to the new chain  
//                    we build; else, link the new chain we build to it;  
//param content_len:  accumulate the length of all chains;  
ngx_int_t fill_chain(ngx_http_request_t* r,  
                     u_char*             data,  
                     size_t              len,  
                     int                 memory,  
                     ngx_chain_t**       chain,  
                     size_t*             content_len);  
  
//fill a header to the output chains; this function is used for debug (we fill  
//request's headers_in into the output chains)  
ngx_int_t fill_header(ngx_http_request_t* r,  
                      ngx_table_elt_t*    header,  
                      ngx_chain_t**       curr,  
                      size_t*             content_len);  
  
#endif  
