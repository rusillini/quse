/* -*- C++ -*- */
#include <config.h>
#include <memory.h>
#include <environ.h>

char* osp_getenv(const char* name)
{
  return osp_strdup(getenv(name));
}

char* osp_getenv2(const char* name, u_intp_ot len)
{
  char *tmp = osp_strdup2(name, len), *ret = NULL;
  ret = osp_getenv(tmp);
  osp_free(tmp);
  return ret;
}

int osp_putenv(const char* env)
{
  return putenv((char*)env);
}

int osp_putenv2(const char* name, const char* val)
{
  int ret;
  u_intp_ot len = 3 + osp_strlen(name) + osp_strlen(val);
  char* env = (char*)osp_malloc(len);
  
  sprintf(env, "%s=%s", name, val);
  ret = osp_putenv(env);
  osp_free(env);
  return ret;
}

char* osp_str_norm(const char* path)
{
#if defined(OSP_FM_UNIX) || defined(OSP_FM_WIN32)
  char *ret = osp_strdup(path), *cur;
  cur = ret;
  while ( *cur )
    {
      if ( *cur == '/' )
        *cur = *OSP_PATH_DELIM;
      ++cur;
    }
  return ret;
#else
  return NULL;
#endif
}


#define TMP_APPEND_DATA(from, to)                 \
{                                                 \
  tmp_len = to-from;                              \
  ret = osp_realloc_ex(ret, len + tmp_len + 1);   \
  if ( ret ) {                                    \
    strncpy(ret+len, from, tmp_len);              \
    *(ret+len+tmp_len) = '\0';                    \
    len += tmp_len;                               \
  } else                                          \
    len = 0;                                      \
}

char* osp_str_subst(const char* str)
{
  const char *start = NULL;
  char *ret = NULL;
  const char *var_s = NULL, *var_e = NULL;
  char* envp = NULL;
  u_intp_ot len = 0, tmp_len = 0;
  
  start = str;
  while ( *str ) {
    if ( *str != '%' )
      {
        ++str;
        continue;
      }
    
    /* Копируем накопленный хвост */
    TMP_APPEND_DATA(start, str);
    
    /* "%%" есть "%", хвостовой "%" игнорируем */
    if ( *(str+1)=='%' )
      {
        var_s = str; var_e = str+1;
        str += 2;
        start = str;
      }
    else if ( *(str+1)=='\0' )
      {
        ++str;
        continue;
      }
    else
      {
        var_s = str + 1;
        var_e = str + 1;
        while ( (*var_e!='%') && *var_e )
          ++var_e;
        { /* Корректируем позиции в исходной строке */
          if ( *var_e=='%' )
            str = var_e + 1;
          else
            str = var_e;
          start = str;
        }
        /* Вытаскиваем переменную окружения */
        envp = osp_getenv2(var_s, var_e-var_s);
        if ( envp==NULL )
          {
            var_s = "";
            var_e = var_s;
          }
        else
          {
            var_s = envp;
            var_e = envp + osp_strlen(envp);
          }
      }
    
    /* Копируем переменную */
    TMP_APPEND_DATA(var_s, var_e);
    if ( envp )
      {
        osp_free(envp);
        envp = NULL;
      }
  }
  
  /* Копируем возможный остаток данных */
  TMP_APPEND_DATA(start, str);
  return ret;
}

char* osp_path_norm(const char* path)
{
  /* Тупая, зато простенькая реализация */
  char *tmp, *ret;
  tmp = osp_str_subst(path);
  ret = osp_str_norm(tmp);
  osp_free(tmp);
  return ret;
}

char* osp_modpath(const char* type, const char* id,
                  const char* dir)
{
  char *tmp_dir = NULL, *tmp_dir2 = NULL;
  u_intp_ot tmp_n;
  if ( dir==NULL || *dir=='\0' )
    dir = "%SRVTHOME%/mod/";
  
  tmp_n = osp_strlen(dir);
  if ( dir[tmp_n-1]!='/' )
    {
      tmp_dir2 = osp_strdup2(dir, tmp_n+2);
      tmp_dir2[tmp_n] = '/';
      tmp_dir2[tmp_n+1] = '\0';
      tmp_dir = osp_path_norm(tmp_dir2);
    }
  else
    tmp_dir = osp_path_norm(dir);
  tmp_dir = osp_strcat(tmp_dir, type);
  tmp_dir = osp_strcat(tmp_dir, "_");
  tmp_dir = osp_strcat(tmp_dir, id);
  tmp_dir = osp_strcat(tmp_dir, OSP_DLL_EXT);
  
  return tmp_dir;
}

char* osp_path_build(const char* const args[])
{
  char* ret = NULL;
  u_intp_ot len = 0, old_len = 0;
  u_intp_ot tmp;
  while ( *args )
    {
      tmp = osp_strlen(*args);
      len += tmp + 2;
      if ( old_len ) ++len;
      ret = (char*)osp_realloc_ex(ret, len+1);
      if ( ret==NULL )
        return NULL;
      if ( old_len ) {
        *(ret+old_len) = ' '; ++old_len;
      }
      *(ret+old_len) = '\"'; ++old_len;
      strcpy(ret+old_len, *args);
      *(ret+old_len+tmp) = '\"';
      *(ret+old_len+tmp+1) = '\0';
      old_len = len;
      ++args;
    }
  return ret;
}

int osp_initenv(const char* name)
{
  return 0;
}
