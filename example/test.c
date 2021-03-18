#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <mysql/mysql.h>

int main() {

  MYSQL* mysql = mysql_init(NULL);
  if(mysql == NULL) {
    printf("init mysql error\n");
    return -1;
  }
  
  //连接服务器
  if(mysql_real_connect(mysql, "127.0.0.1", "root", "", "db_blog", 0, NULL, 0) == NULL) {
    printf("connect mysql server failed: %s\n", mysql_error(mysql));
    return -1;
  }

  //设置客户端字符集
  if(mysql_set_character_set(mysql, "utf8") != 0) {
    printf("set character failed: %s\n", mysql_error(mysql));
    return -1;
  }  

  //选择切换使用的数据库
  if (mysql_select_db(mysql, "db_blog") != 0) {
    printf("select db failed:%s\n", mysql_error(mysql));
    return -1;
  }

  //创建表
  
  //const char* sql = "create table if not exists tb_stu(id int, name varchar(32), info text, score decimal(4, 2), birth datetime);";
  //const char* sql = "insert tb_stu values(1, '张三', '真的好帅', 99.456, '2000-10-25 00:00:00');";
  //const char* sql = "update tb_stu set name='YJK',info='好饿好饿好饿' where id=1;";
  //const char* sql = "delete from tb_stu where name='YJK';";
  
  const char* sql = "select * from tb_stu;";

  int ret = mysql_query(mysql, sql);
  if (ret != 0) {
    printf("query sql  failed:%s\n", mysql_error(mysql));
    return -1;
  }

  MYSQL_RES* res = mysql_store_result(mysql);
  if(res == NULL) {
    printf("store result failed:%s\n", mysql_error(mysql));
    return -1;
  }

  int row = mysql_num_rows(res); //行数
  int col = mysql_num_fields(res); //列数

  int i = 0;
  for(; i < row; i++) {
    //res中有读取位置控制, 每次获取的都是下一跳数据
    MYSQL_ROW row = mysql_fetch_row(res);
    for(int j = 0; j < col; j++) {
      printf("%s\t", row[j]);
    }
    printf("\n");
  }

  mysql_free_result(res); //释放结果集
  mysql_close(mysql);

  return 0;
}





