/*

(c) 2012 Sven Geggus <sven-nss@geggus.net>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY;without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.

*/

#define CFGFILE "/etc/passwd_nss_catchall"

#include <stdio.h>
#include <syslog.h>
#include <nss.h>
#include <pwd.h>
#include <string.h>

// define DEBUG to print log debugging information via syslog
#define DEBUG 0
#define debug_syslog(fmt, ...) do { if (DEBUG) syslog(LOG_DEBUG, fmt, __VA_ARGS__); } while (0)

enum nss_status _nss_catchall_getpwnam_r(const char *, struct passwd *, char *, size_t, int *);

enum nss_status _nss_catchall_getpwnam_r(const char *name,
 struct passwd *result,
 char *buf,
 __attribute__((unused)) size_t buflen,
 __attribute__((unused)) int *errnop) {
 
  FILE *fp;
  size_t len;
  char *p, *val;
  char cfgfilent[NSS_BUFLEN_PASSWD];
 
  openlog("nss_catchall",LOG_PERROR|LOG_PID,LOG_LOCAL7);
  debug_syslog("nss_catchall_getpwnam_r: got username >%s<",name);
 
  fp=fopen(CFGFILE,"rb");
  if (NULL==fp) {
    syslog(LOG_ERR,"nss_catchall_getpwnam_r: unable to open file %s\n",CFGFILE);
    closelog();
    return NSS_STATUS_UNAVAIL;  
  }
  len=fread(cfgfilent,sizeof(char),1024,fp);
  cfgfilent[len-1]='\0';
  fclose(fp);
 
  strcpy(buf,name);
  p=cfgfilent;
  while (*p && *p != ':') p++;
  strcpy(buf+strlen(name),p); 
  p=buf+strlen(name);
  result->pw_name=buf;
  while (*p && *p != ':') p++; *p='\0'; p++;
  result->pw_passwd=p;
  while (*p && *p != ':') p++; *p='\0'; p++;
  val=p;
  while (*p && *p != ':') p++; *p='\0';
  sscanf(val,"%u",&result->pw_uid);
  p++;
  val=p;
  while (*p && *p != ':') p++; *p='\0';
  sscanf(val,"%u",&result->pw_gid);
  p++;
  result->pw_gecos=p;
  while (*p && *p != ':') p++; *p='\0'; p++;
  result->pw_dir=p;
  while (*p && *p != ':') p++; *p='\0'; p++;
  result->pw_shell=p;
  while (*p && *p != ':') p++; *p='\0'; p++;
         
  closelog();
  return NSS_STATUS_SUCCESS;
}
