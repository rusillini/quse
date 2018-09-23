/* -*- C++ -*- */
#include <config.h>
#include <memory.h>

void *osp_malloc(u_intp_ot n)
{
  return malloc(n);
}

void *osp_realloc(void* p, u_intp_ot n)
{
  return realloc(p, n);
}

void *osp_realloc_ex(void* p, u_intp_ot n)
{
  void* t = realloc(p, n);
  if ( t==NULL )
    {
      if ( p )
        free(p);
    }
  return t;
}

void osp_free(void* p)
{
  if (p)
    free(p);
}

u_intp_ot osp_strlen(const char* s)
{
  register u_intp_ot len = 0;
  if ( s==NULL )
    return 0;
  while ( *s ) {
    ++s, ++len;
  }
  return (u_intp_ot)len;
}

char* osp_strdup(const char* s)
{
  return (char*) osp_memdup(s, osp_strlen(s)+1);
}

char* osp_strdup2(const char* s, u_intp_ot n)
{
  char* ret = (char*)osp_malloc(n+1), *cur;
  cur = ret;
  while ( *s && n-- > 0 )
    *(cur++) = *(s++);
  *cur = '\0';
  return ret;
}

intp_ot osp_strcmp(const char* s1, const char* s2)
{
  if ( s1==s2 ) return 0;
  if ( s2==NULL ) return 1;
  if ( s1==NULL ) return -1;
  while ( *s1 && *s2 )
    {
      if ( *s1 > *s2 ) return 1;
      if ( *s1 < *s2 ) return 1;
      ++s1, ++s2;
    }
  if ( *s1 ) return 1;
  if ( *s2 ) return -1;
  return 0;
}

intp_ot osp_strncmp(const char* s1, const char* s2,
                    intp_ot len)
{
  if ( s1==s2 ) return 0;
  if ( s2==NULL ) return 1;
  if ( s1==NULL ) return -1;
  while ( *s1 && *s2 && (len-- > 0) )
    {
      if ( *s1 > *s2 ) return 1;
      if ( *s1 < *s2 ) return 1;
      ++s1, ++s2;
    }
  if ( len > 0 ) {
    if ( *s1 ) return 1;
    if ( *s2 ) return -1;
  }
  return 0;
}

char* osp_strcat(char* s1, const char* s2)
{
  u_intp_ot
    len1 = osp_strlen(s1),
    len2 = osp_strlen(s2),
    i = 0;
  s1 = osp_realloc_ex(s1, len1+len2+1);
  if ( s1==NULL )
    return NULL;
  for ( i=0; i<=len2; ++i )
    s1[len1+i] = s2[i];
  return s1;
}

char* osp_strstr(char* s, const char* ss)
{
  register const char *ret, *tmp1, *tmp2;
  
  if ( s==NULL || *s=='\0' ) return s;
  if ( ss==NULL || *ss=='\0' ) return s;
  if ( (const char*)s==ss ) return s;
  
  for ( ret=s; *ret; ++ret )
    {
      if ( *ret != *ss )
        continue;
      for ( tmp1=ret, tmp2=ss; *tmp1 && *tmp2; ++tmp1, ++tmp2 ) {
        if ( *tmp1 != *tmp2 )
          break;
      }
      if ( *tmp2 == '\0' )
        return (char*)ret;
    }
  return NULL;
}

void* osp_memdup(const void* d, u_intp_ot n)
{
  register void* c = osp_malloc(n);
  if ( c==NULL )
    return NULL;
  if ( d==NULL )
    {
      if ( n ) *(char*)c = '\0';
      return c;
    }
  while ( n-- > 0 )
    {
      *(((char*)c)+n) = *(((const char*)d)+n);
    }
  return c;
}

void* osp_memdup2(const void* d, u_intp_ot n,
                  u_intp_ot k)
{
  register void* c = osp_malloc(k);
  if ( c==NULL )
    return NULL;
  if ( d==NULL )
    {
      if ( k ) *(char*)c = '\0';
      return c;
    }
  if ( n>k )
    n = k;
  while ( n-- > 0 )
    {
      *(((char*)c)+n) = *(((const char*)d)+n);
    }
  return c;
}

void osp_check_mem(const char* check)
{
  printf("## memcheck <%s>...", check); fflush(stdout);
  void* data[50]; unsigned int i;
  for ( i=0; i<sizeof(data)/sizeof(void*); i++ )
    data[i] = osp_malloc(37);
  for ( i=0; i<sizeof(data)/sizeof(void*); i++ )
    osp_free(data[i]);
  printf("done\n"); fflush(stdout);
}

// End Of File
