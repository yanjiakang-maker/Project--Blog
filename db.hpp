#include <iostream>
#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>
#include <mutex>

using namespace std;

#define MYSQL_HOST "127.0.0.1"
#define MYSQL_USER "root"
#define MYSQL_PSWD ""
#define MYSQL_DB "db_blog"

namespace blog_system {

  static mutex g_mutex;

  MYSQL* MysqlInit() {
    //向外提供接口,返回初始化的mysql句柄(连接服务器,选择数据库,设置字符集)
    MYSQL* mysql;

    //初始化
    mysql = mysql_init(NULL);
    if(mysql == NULL) {
      printf("init mysql error\n");
      return NULL;
    }

    //连接服务器
    if(mysql_real_connect(mysql, MYSQL_HOST, MYSQL_USER, MYSQL_PSWD, NULL, 0, NULL, 0) == NULL) {
      printf("connect mysql server error:%s\n", mysql_error(mysql));
      mysql_close(mysql);
      return NULL;
    }

    //设置字符集
    if(mysql_set_character_set(mysql, "utf8") != 0) {
      printf("set client character error:%s\n", mysql_error(mysql));
      mysql_close(mysql);
      return NULL;
    }

    //选择数据库
    if(mysql_select_db(mysql, MYSQL_DB) != 0) {
      printf("select db error:%s\n", mysql_error(mysql));
      mysql_close(mysql);
      return NULL;
    }

    return mysql;
  }


  void MysqlRelease(MYSQL* mysql) {
    //销毁句柄
    if (mysql) {
      mysql_close(mysql);
    }
  }


  bool MysqlQuery(MYSQL* mysql, const char* sql) {
    //执行语句的公有接口
    int ret = mysql_query(mysql, sql);
    if(ret != 0) {
      printf("query sql:[%s] failed:%s", sql, mysql_error(mysql));
      return false;
    }
    return true;
  }


  class TableBlog {
    public:
      TableBlog(MYSQL* mysql) :_mysql(mysql) {}


      bool Insert(Json::Value& blog) {
        //从blog中取出博客信息, 组织sql语句 
#define INSERT_BLOG "insert tb_blog values(null, '%d', '%s', '%s', now());"
        //因为博客正文长度不懂, 有可能会很长, 因此如果直接定义固定长度tmp, 有可能访问越界
        int len = blog["content"].asString().size() + 4096;
        //string sql;
        //sql.resize(len);
        char* sql = (char*)malloc(len);
        sprintf(sql, INSERT_BLOG, blog["tag_id"].asInt(), blog["title"].asCString(), 
            blog["content"].asCString());
        bool ret = MysqlQuery(_mysql, sql);

        free(sql);
        return ret;
      }


      bool Delete(int blog_id) {
        //根据博客id删除博客
#define DELETE_BLOG "delete from tb_blog where id=%d;"
        char sql[1024] = {0};
        sprintf(sql, DELETE_BLOG, blog_id);
        bool ret = MysqlQuery(_mysql, sql);
        return ret;
      }


      bool Update(Json::Value& blog) {
        //从blog中取出博客信息, 组织sql语句,更新数据库中的数据
        //id tag_id title content ctime
#define UPDATE_BLOG "update tb_blog set tag_id=%d, title='%s', content='%s' where id=%d;"
        int len = blog["content"].asString().size() + 4096;
        char* sql = (char*)malloc(len);
        sprintf(sql, UPDATE_BLOG, blog["tag_id"].asInt(), blog["title"].asCString(), 
            blog["content"].asCString(), blog["id"].asInt());

        bool ret = MysqlQuery(_mysql, sql);
        free(sql);
        return ret;
      }


      bool GetAll(Json::Value* blogs) {
        //通过blog返回所有的博客信息(通常是列表展示, 不返回正文)

        //执行查询语句
#define GETALL_BLOG "select id, tag_id, title, ctime from tb_blog;"

        g_mutex.lock();
        bool ret = MysqlQuery(_mysql, GETALL_BLOG);
        if (ret == false) {
          g_mutex.unlock();
          return false;
        }

        //保存结果集
        MYSQL_RES* res = mysql_store_result(_mysql);
        g_mutex.unlock();

        if (res == NULL) {
          printf("store all blog result failed:%s\n", mysql_error(_mysql));
          return false;
        }

        //遍历结果集
        int row_num = mysql_num_rows(res);
        for(int i = 0; i < row_num; i++) {
          MYSQL_ROW row = mysql_fetch_row(res);
          Json::Value blog;
          blog["id"] = stoi(row[0]);
          blog["tag_id"] = stoi(row[1]);
          blog["title"] = row[2];
          blog["ctime"] = row[3];

          blogs->append(blog); //添加json数组元素
        }

        mysql_free_result(res);
        return true;

      }


      bool GetOne(Json::Value* blog) {
        //返回单个博客信息, 包含正文
#define GETONE_BLOG "select tag_id, title, content, ctime from tb_blog where id=%d;"
        char sql[1024] = {0};
        sprintf(sql, GETONE_BLOG, (*blog)["id"].asInt());

        g_mutex.lock();
        bool ret = MysqlQuery(_mysql, sql);
        if(ret == false) {
          g_mutex.unlock();
          return false;
        }

        MYSQL_RES* res = mysql_store_result(_mysql);
        g_mutex.unlock();
        if (res == NULL) {
          printf("store all blog result failed:%s\n", mysql_error(_mysql));
          return false;
        }

        int row_num = mysql_num_rows(res);
        g_mutex.unlock();

        if (row_num != 1) {
          printf("get one blog result error\n");
          mysql_free_result(res);
          return false;
        }
        MYSQL_ROW row = mysql_fetch_row(res);
        (*blog)["tag_id"] = stoi(row[0]);
        (*blog)["title"] = row[1];
        (*blog)["content"] = row[2];
        (*blog)["ctime"] = row[3];

        mysql_free_result(res);
        return true;
      }

    private:
      MYSQL* _mysql;
  };


  class TableTag {
    public:
      TableTag(MYSQL* mysql) :_mysql(mysql) {}
      bool Insert(Json::Value& tag) {
#define INSERT_TAG "insert tb_tag values(null, '%s');"
        char sql[1024] = {0};
        sprintf(sql, INSERT_TAG, tag["name"].asCString());
        return MysqlQuery(_mysql, sql);
      }

      bool Delete(int tag_id) {
#define DELETE_TAG "delete from tb_tag where id=%d;"
        char sql[1024] = {0};
        sprintf(sql, DELETE_TAG, tag_id);
        return MysqlQuery(_mysql, sql);
      }

      bool Update(Json::Value& tag) {
#define UPDATE_TAG "update tb_tag set name='%s' where id=%d;"
        char sql[1024] = {0};
        sprintf(sql, UPDATE_TAG, tag["name"].asCString(), tag["id"].asInt());
        return MysqlQuery(_mysql, sql);
      }

      bool GetAll(Json::Value* tags) {
#define GETALL_TAG "select id, name from tb_tag;"
        g_mutex.lock();
        bool ret = MysqlQuery(_mysql, GETALL_TAG);
        if(ret == false) {
          g_mutex.unlock();
          return false;
        }

        MYSQL_RES* res = mysql_store_result(_mysql);
        g_mutex.unlock();

        if(res == NULL) {
          printf("store all tag result failed:%s\n", mysql_error(_mysql));
          return false;
        }

        int row_num = mysql_num_rows(res);
        for(int i = 0; i < row_num; i++) {
          MYSQL_ROW row = mysql_fetch_row(res);
          Json::Value tag;
          tag["id"] = stoi(row[0]);
          tag["name"] = row[1];
          tags->append(tag);
        }

        mysql_free_result(res);
        return true;
      }

      bool GetOne(Json::Value* tag) {
#define GETONE_TAG "select name from tb_tag where id=%d;"
        char sql[1024] = {0};
        sprintf(sql, GETONE_TAG, (*tag)["id"].asInt());

        g_mutex.lock();
        bool ret = MysqlQuery(_mysql, sql);
        if (ret == false) {
          g_mutex.unlock();
          return false;
        }
        MYSQL_RES* res = mysql_store_result(_mysql);
        g_mutex.unlock();

        if(res == NULL) {
          printf("store one tag result failed:%s\n", mysql_error(_mysql));
          return false;
        }

        int row_num = mysql_num_rows(res);
        if(row_num != 1) {
          printf("get one tag result error\n");
          mysql_free_result(res);
          return false;
        }

        MYSQL_ROW row = mysql_fetch_row(res);
        (*tag)["name"] = row[0];
        mysql_free_result(res);
        return true;

      }


    private:
      MYSQL* _mysql;
  };
}
