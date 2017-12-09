#include <sqlite3.h>
#include <stdio.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName);

int main(int argc, char const *argv[]) {
  /* code */
  sqlite3 *db;
  int ret;
  char name[64] = "Yunsu";
  char ip[64] = "127.0.0.1";
  char up_date[128] = "1988/11/21 13:20:45";
  char *errMsg;
  int count = 0;
  int size = 0;
  
  ret = sqlite3_open("filelist.db", &db);
  if(ret == SQLITE_OK){
      printf("database Opened\n");
      // char query[200];
      // sprintf(query, "insert into file_info(name, id, up_date, count, size) values('%s', '%s', '%s', %d, %d)",name, id, up_date, count, size);
      char *query = sqlite3_mprintf("insert into file_info(name, ip, up_date, count, size) values('%q', '%q', '%q', '%d', '%d');", name, ip, up_date, count, size);
      printf("%s\n", query);
      ret = sqlite3_exec(db, query, callback,0,&errMsg);
      if(ret != SQLITE_OK){
        printf("%s\n", errMsg);
      }
  }
  sqlite3_close(db);
  return 0;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i; 
  for( i = 0; i < argc; i++) { 
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL"); 
  } 
  printf("\n"); 
  return 0;
}