#include <dirent.h>  
#include <sys/types.h>  
#include <dirent.h>  
#include <string.h>  
  
#include "localfile.h"  
  
ngx_int_t cat_file(ngx_http_request_t*  r,  
                   const char*          fname,  
                   ngx_chain_t**        curr,  
                   size_t*              content_len)  
{  
  ngx_int_t rc = NGX_OK;  
  int fd = open(fname, O_RDONLY);  
  if(fd!=-1)  
  {  
    fill_chain(r, (u_char*)"------------------\r\n", 20, 1, curr, content_len);  
  
    int num;  
    while(1)  
    {  
      char* buf = ngx_pcalloc(r->pool, sizeof(char)*256);  
      if((num=read(fd, buf, 256)) > 0)  
      {  
        fill_chain(r, (u_char*)buf, num, 1, curr, content_len);  
      }  
      else if(num<0)  
      {  
        fill_chain(r, (u_char*)"\r\nread error\r\n", 14, 1, curr, content_len);  
        rc = NGX_ERROR;  
        break;  
      }  
      else  //num==0, EOF  
      {  
        break;  
      }  
    }  
    close(fd);  
  }  
  else  
  {  
    fill_chain(r, (u_char*)"\r\nopen error\r\n", 14, 1, curr, content_len);  
    rc = NGX_ERROR;  
  }  
  return rc;  
}  
  
ngx_int_t head_file(ngx_http_request_t*  r,  
                    const char*          fname,  
                    int                  headn,  
                    ngx_chain_t**        curr,  
                    size_t*              content_len)  
{  
  ngx_int_t rc = NGX_OK;  
  
  int fd = open(fname, O_RDONLY);  
  if(fd!=-1)  
  {  
    fill_chain(r, (u_char*)"------------------\r\n", 20, 1, curr, content_len);  
  
    int i, num, lines=0;  
    int enough=0;  
    while(!enough)  
    {  
      char* buf = ngx_pcalloc(r->pool, sizeof(char)*256);  
      if((num=read(fd, buf, 256)) > 0)  
      {  
        for(i=0;i<num;i++)  
        {  
          if(buf[i] == '\n')  
          {  
            if(++lines == headn)  
            {  
              num = i+1;   //num points to '\n'; discard contents after this '\n'  
              enough = 1;  
              break;  
            }  
          }  
        }  
        fill_chain(r, (u_char*)buf, num, 1, curr, content_len);  
      }  
      else if(num<0)  
      {  
        fill_chain(r, (u_char*)"\r\nread error\r\n", 14, 1, curr, content_len);  
        rc = NGX_ERROR;  
        break;  
      }  
      else //num==0, EOF  
      {  
        break;  
      }  
    }  
    close(fd);  
  }  
  else  
  {  
    fill_chain(r, (u_char*)"\r\nopen error\r\n", 14, 1, curr, content_len);  
    rc = NGX_ERROR;  
  }  
  return rc;  
}  
  
ngx_int_t tail_file(ngx_http_request_t*  r,  
                    const char*          fname,  
                    int                  tailn,  
                    ngx_chain_t**        curr,  
                    size_t*              content_len)  
{  
  ngx_int_t rc = NGX_OK;  
  
  tailn++;  //the last line contains '\n'  
  
  int fd = open(fname, O_RDONLY);  
  if(fd!=-1)  
  {  
    fill_chain(r, (u_char*)"------------------\r\n", 20, 1, curr, content_len);  
  
    off_t pos = lseek(fd, 0, SEEK_END);   //pos points to the end of the file;  
    int i, num, lines=0;  
    ngx_chain_t *last = NULL;  
    ngx_chain_t *first = NULL;  
    int enough = 0;  
  
    //eg. a file whose size is 800; we read it like this:  
    //     [544, 800)  
    //     [288, 544)  
    //     [32,  288)  
    //     [0,   32)  
    //and we quit read once we got enough lines;  
    while(!enough && pos>0)  
    {  
      ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "pos=%d",pos);  
      size_t count;  
      if(pos < 256) //from 0 to pos < 256, read all (from 0 to pos);  
      {  
        count = pos;                      //read 'pos' bytes next time;  
        pos = lseek(fd, -pos, SEEK_CUR);  //move backward by 'pos' bytes (to 0)  
      }  
      else //from begin to pos >= 256, read 256 bytes (from pos-256 to pos)  
      {  
        count = 256;                      //read 256 bytes next time;  
        pos = lseek(fd, -256, SEEK_CUR);  //move backward by 256 bytes;  
      }  
      ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "after seek pos=%d, count=%d",pos, count);  
  
      char* buf = ngx_pcalloc(r->pool, sizeof(char)*count);  
      if((num=pread(fd,buf,count,pos))>0)  
      {  
        for(i=count-1;i>=0;i--)  
        {  
          if(buf[i]=='\n')  
          {  
            if(++lines == tailn)  
            {  
              enough = 1;  
              break;  
            }  
          }  
        }  
  
        //read in reverse order, so we have to link the chain manaully;  
        ngx_chain_t * ret = NULL;  
        fill_chain(r, (u_char*)(buf+i+1), count-i-1, 1, &ret, content_len);  
        if(NULL == last)  
        {  
          first = last = ret;  
        }  
        else  
        {  
          ret->next = first;  
          first = ret;  
        }  
      }  
      else if(num != 0)  
      {  
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "pread errno=%d, errmsg=%s",errno, strerror(errno));  
        fill_chain(r, (u_char*)"\r\npread error\r\n", 14, 1, curr, content_len);  
        rc = NGX_ERROR;  
        break;  
      }  
    }  
  
    //we have got a piece of chain:  first->...->...->last; link it to curr;  
    if(NULL != first)  
    {  
      (*curr)->next = first;  
      (*curr) = last;  
    }  
    close(fd);  
  }  
  else  
  {  
    rc = NGX_ERROR;  
  }  
  return rc;  
}  
  
static ngx_int_t ll_file(ngx_http_request_t*   r,  
                         const char*           pathname,  
                         const char*           shortname,  
                         int                   opt_h,  
                         ngx_chain_t**         curr,  
                         size_t*               content_len)  
{  
  struct stat buf;  
  if(lstat(pathname, &buf)!=0)  
  {  
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "lstat pathname=%s, errno=%d, errmsg=%s", pathname, errno, strerror(errno));  
    fill_chain(r, (u_char*)"\r\nlstat error\r\n", 15, 1, curr, content_len);  
    return NGX_ERROR;  
  }  
  
  //file type  
  if(S_ISLNK(buf.st_mode))  
  {  
    fill_chain(r, (u_char*)"l", 1, 1, curr, content_len);  
  }  
  else if(S_ISREG(buf.st_mode))  
  {  
    fill_chain(r, (u_char*)"-", 1, 1, curr, content_len);  
  }  
  else if(S_ISDIR(buf.st_mode))  
  {  
    fill_chain(r, (u_char*)"d", 1, 1, curr, content_len);  
  }  
  else if(S_ISFIFO(buf.st_mode))  
  {  
    fill_chain(r, (u_char*)"p", 1, 1, curr, content_len);  
  }  
  else if(S_ISSOCK(buf.st_mode))  
  {  
    fill_chain(r, (u_char*)"s", 1, 1, curr, content_len);  
  }  
  else if(S_ISBLK(buf.st_mode))  
  {  
    fill_chain(r, (u_char*)"b", 1, 1, curr, content_len);  
  }  
  else if(S_ISCHR(buf.st_mode))  
  {  
    fill_chain(r, (u_char*)"c", 1, 1, curr, content_len);  
  }  
  else  
  {  
    fill_chain(r, (u_char*)" ", 1, 1, curr, content_len);  
  }  
  
  //permissions;  
  fill_chain(r, (S_IRUSR&buf.st_mode)?((u_char*)"r"):((u_char*)"-"), 1, 1, curr, content_len);  
  fill_chain(r, (S_IWUSR&buf.st_mode)?((u_char*)"w"):((u_char*)"-"), 1, 1, curr, content_len);  
  fill_chain(r, (S_IXUSR&buf.st_mode)?((u_char*)"x"):((u_char*)"-"), 1, 1, curr, content_len);  
  
  fill_chain(r, (S_IRGRP&buf.st_mode)?((u_char*)"r"):((u_char*)"-"), 1, 1, curr, content_len);  
  fill_chain(r, (S_IWGRP&buf.st_mode)?((u_char*)"w"):((u_char*)"-"), 1, 1, curr, content_len);  
  fill_chain(r, (S_IXGRP&buf.st_mode)?((u_char*)"x"):((u_char*)"-"), 1, 1, curr, content_len);  
  
  fill_chain(r, (S_IROTH&buf.st_mode)?((u_char*)"r"):((u_char*)"-"), 1, 1, curr, content_len);  
  fill_chain(r, (S_IWOTH&buf.st_mode)?((u_char*)"w"):((u_char*)"-"), 1, 1, curr, content_len);  
  fill_chain(r, (S_IXOTH&buf.st_mode)?((u_char*)"x"):((u_char*)"-"), 1, 1, curr, content_len);  
  
  //space  
  fill_chain(r, (u_char*)"\t", 1, 1, curr, content_len);  
  
  ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "stat st_uid=%d, st_gid=%d", buf.st_uid, buf.st_gid);  
  
  //user id  
  char* uidbuf = ngx_pcalloc(r->pool, 32);  
  sprintf(uidbuf,"%u",buf.st_uid);  
  fill_chain(r, (u_char*)uidbuf, strlen(uidbuf), 1, curr, content_len);  
  
  //space  
  fill_chain(r, (u_char*)"\t", 1, 1, curr, content_len);  
  
  //group id  
  char* gidbuf = ngx_pcalloc(r->pool, 32);  
  sprintf(gidbuf,"%u",buf.st_uid);  
  fill_chain(r, (u_char*)gidbuf, strlen(gidbuf), 1, curr, content_len);  
  
  //space  
  fill_chain(r, (u_char*)"\t", 1, 1, curr, content_len);  
  
  //size  
  long size = buf.st_size;  
  if(!opt_h)  
  {  
    char* sizebuf = ngx_pcalloc(r->pool, 32);  
    sprintf(sizebuf, "%10.li", size);  
    fill_chain(r, (u_char*)sizebuf, strlen(sizebuf), 1, curr, content_len);  
  }  
  else  
  {  
    float fsize=0;  
    char* unit = ngx_pcalloc(r->pool, 1);  
    if(size>=1024*1024*1024)  
    {  
      unit[0] = 'G';  
      fsize = ((float)size)/(1024*1024*1024);  
    }  
    else if(size>=1024*1024)  
    {  
      unit[0] = 'M';  
      fsize = ((float)size)/(1024*1024);  
    }  
    else if(size>=1024)  
    {  
      unit[0] = 'K';  
      fsize = ((float)size)/(1024);  
    }  
    else  
    {  
      fsize = size;  
      unit[0] = 'B';  
    }  
    char* fsizebuf = ngx_pcalloc(r->pool, 32);  
    sprintf(fsizebuf,"%10.1f", fsize);  
    fill_chain(r, (u_char*)fsizebuf, strlen(fsizebuf), 1, curr, content_len);  
    fill_chain(r, (u_char*)unit, 1, 1, curr, content_len);  
  }  
  
  //space  
  fill_chain(r, (u_char*)"\t", 1, 1, curr, content_len);  
  
  //file name  
  fill_chain(r, (u_char*)shortname, strlen(shortname), 1, curr, content_len);  
  
  fill_chain(r, (u_char*)"\r\n", 2, 1, curr, content_len);  
  return NGX_OK;  
}  
  
ngx_int_t ls_file_dir(ngx_http_request_t*   r,  
                   const char*              path,  
                   int                      opt_l,  
                   int                      opt_a,  
                   int                      opt_h,  
                   ngx_chain_t**            curr,  
                   size_t*                  content_len)  
{  
  ngx_int_t rc = NGX_OK;  
  
  struct stat statbuf;  
  if(stat(path, &statbuf) != 0)  
  {  
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "stat path=%s, errno=%d, errmsg=%s", path, errno, strerror(errno));  
    fill_chain(r, (u_char*)"\r\nstat error\r\n", 14, 1, curr, content_len);  
    return NGX_ERROR;  
  }  
  
  
  if(S_ISDIR(statbuf.st_mode))  //directory  
  {  
    fill_chain(r, (u_char*)"\r\ndirectory\r\n", 13, 1, curr, content_len);  
    fill_chain(r, (u_char*)"------------------\r\n", 20, 1, curr, content_len);  
  
    DIR * dir = opendir(path);  
    if(NULL == dir)  
    {  
      ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "opendir errno=%d, errmsg=%s",errno, strerror(errno));  
      fill_chain(r, (u_char*)"\r\nopendir error\r\n", 17, 1, curr, content_len);  
      return NGX_ERROR;  
    }  
  
    struct dirent denbuf, *result;  
    int ret = readdir_r(dir, &denbuf, &result);  
    while(0 == ret && NULL != result)  
    {  
      size_t len = strlen(denbuf.d_name);  
  
      char* dname = ngx_pcalloc(r->pool, len+1);  
      strcpy(dname,denbuf.d_name);  
  
      ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "d_name=%s, len=%d",dname, len);  
  
      if(!opt_a && dname[0]=='.')   //skip invisible files if -a is not set  
      {  
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "skip %s because it's invisible and -a not set", dname);  
      }  
      else  
      {  
        if(!opt_l)  
        {  
          fill_chain(r, (u_char*)dname, len, 1, curr, content_len);  
          fill_chain(r, (u_char*)"\t", 1, 1, curr, content_len);  
        }  
        else  
        {  
          size_t pathlen = strlen(path);  
  
          char* fullname = ngx_pcalloc(r->pool, pathlen+len+2);  
          strcpy(fullname, path);  
          if(path[pathlen-1] != '/')  
          {  
            strcpy(fullname+pathlen, "/");  
            pathlen++;  
          }  
          strcpy(fullname+pathlen,dname);  
          ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "fullname=%s, len=%d",fullname, strlen(fullname));  
  
          ll_file(r, fullname, dname, opt_h, curr, content_len);  
        }  
      }  
  
      ret = readdir_r(dir, &denbuf, &result);  
    }  
    fill_chain(r, (u_char*)"\r\n", 2, 1, curr, content_len);  
    closedir(dir);  
    if(ret != 0 )  
    {  
      ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "readdir_r errno=%d, errmsg=%s",ret, strerror(ret));  
      fill_chain(r, (u_char*)"\r\nreaddir_r error\r\n", 17, 1, curr, content_len);  
      return NGX_ERROR;  
    }  
  }  
  else //file  
  {  
    fill_chain(r, (u_char*)"\r\nfile\r\n", 8, 1, curr, content_len);  
    fill_chain(r, (u_char*)"------------------\r\n", 20, 1, curr, content_len);  
  
    size_t pathlen = strlen(path);  
  
    int p;  
    for(p=pathlen-1; p>=0&&path[p]!='/'; p--);  
    char* shortname = ngx_pcalloc(r->pool, pathlen-p);  
    strcpy(shortname, path+p+1);  
    if(!opt_l)  
    {  
      fill_chain(r, (u_char*)shortname, strlen(shortname), 1, curr, content_len);  
    }  
    else  
    {  
      ll_file(r, path, shortname, opt_h, curr, content_len);  
    }  
  }  
  
  return rc;  
}  
