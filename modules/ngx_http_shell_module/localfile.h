#ifndef __NGX_HTTP_SHELL_LOCALFILE_H__  
#define __NGX_HTTP_SHELL_LOCALFILE_H__  
  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <errno.h>  
  
#include <ngx_config.h>  
#include <ngx_core.h>  
#include <ngx_http.h>  
  
#include "util.h"  
  
ngx_int_t cat_file(ngx_http_request_t*  r,  
                   const char*          fname,  
                   ngx_chain_t**        curr,  
                   size_t*              content_len);  
  
ngx_int_t head_file(ngx_http_request_t*  r,  
                    const char*          fname,  
                    int                  headn,  
                    ngx_chain_t**        curr,  
                    size_t*              content_len);  
  
ngx_int_t tail_file(ngx_http_request_t*  r,  
                    const char*          fname,  
                    int                  tailn,  
                    ngx_chain_t**        curr,  
                    size_t*              content_len);  
  
ngx_int_t ls_file_dir(ngx_http_request_t*   r,  
                   const char*              path,  
                   int                      opt_l,  
                   int                      opt_a,  
                   int                      opt_h,  
                   ngx_chain_t**            curr,  
                   size_t*                  content_len);  
  
#endif  
