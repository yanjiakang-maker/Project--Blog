#include "db.hpp"
#include "httplib.h"

using namespace httplib;

blog_system::TableBlog* table_blog;
blog_system::TableTag* table_tag;

//插入博客的业务逻辑
void InsertBlog(const Request& req, Response& rsp) {
  //从请求中取出正文 -- 正文就是提交的博客信息, 以json格式的字符串形式组织的
  //req.body;  
  //将json格式的字符串进行反序列化, 得到各个博客信息
  Json::Reader reader;
  Json::Value blog;
  Json::FastWriter writer;
  Json::Value errmsg;

  bool ret = reader.parse(req.body, blog);//将请求正文进行json反序列化-因为正文就是json格式的博客信息
  if (ret == false) {
    printf("InsertBlog pares blog json failed!\n");
    rsp.status = 400;
    errmsg["ok"] = false;
    errmsg["reason"] = "pares blog json failed!";
    rsp.set_content(writer.write(errmsg), "application/json"); //添加正文到rsp.body中
    return;
  }
  
  //将得到的博客信息插入到数据库
  ret = table_blog->Insert(blog);
  if (ret == false) {
    printf("InsertBlog insert into database failed!\n");
    rsp.status = 500;
    return;
  }
  rsp.status = 200;

  return;
}

//删除博客的业务逻辑
void DeleteBlog(const Request& req, Response& rsp) {
  // /blog/123  /blog/(\d+)  req.matches[0] = blog/123  req.matches[0] = 123
  int blog_id = std::stoi(req.matches[1]);
  bool ret = table_blog->Delete(blog_id);
  if (ret == false) {
    printf("DaleteBlog delete from database failed!\n");
    rsp.status = 500;
    return ;
  }

  rsp.status = 200;
  return;
}

//修改博客的业务逻辑
void UpdateBlog(const Request& req, Response& rsp) {
  int blog_id = std::stoi(req.matches[1]);
  Json::Value blog;
  Json::Reader reader;
  
  bool ret = reader.parse(req.body, blog);
  if (ret == false) {
    printf("UpdateBlog pares json failed!\n");
    rsp.status = 400;
    return ;
  }

  blog["id"] = blog_id;
  ret = table_blog->Update(blog);
  if (ret == false) {
    printf("UpdateBlog update database failed!\n");
    rsp.status = 500;
    return ;
  }

  rsp.status = 200;
  return;
}

//获取全部博客的业务逻辑
void GetAllBlog(const Request& req, Response& rsp) {
  //从数据库中取出博客列表数据
  Json::Value blogs;
  bool ret = table_blog->GetAll(&blogs);
  if (ret == false) {
    printf("GetAllBlog select from database failed!\n");
    rsp.status = 500;
    return ;
  }

  //将数据进行json序列化, 添加到rsp正文中
  Json::FastWriter writer;
  rsp.set_content(writer.write(blogs), "application/json");
  rsp.status = 200;
  return;
}

//获取一条博客的业务逻辑
void GetOneBlog(const Request& req, Response& rsp) {
  int blog_id = std::stoi(req.matches[1]);
  Json::Value blog;
  blog["id"] = blog_id;
  bool ret = table_blog->GetOne(&blog);
  if (ret == false) {
    printf("GetAllBlog select from database failed!\n");
    rsp.status = 500;
    return ;
  }

  //将数据进行json序列化, 添加到rsp正文中
  Json::FastWriter writer;
  rsp.set_content(writer.write(blog), "application/json");
  rsp.status = 200;
  
  return;
}


void InsertTag(const Request& req, Response& rsp) {
  Json::Reader reader;
  Json::Value tag;
  Json::FastWriter writer;
  Json::Value errmsg;

  bool ret = reader.parse(req.body, tag);//将请求正文进行json反序列化-因为正文就是json格式的博客信息
  if (ret == false) {
    printf("InsertBlog pares tag json failed!\n");
    rsp.status = 400;
    errmsg["ok"] = false;
    errmsg["reason"] = "pares tag json failed!";
    rsp.set_content(writer.write(errmsg), "application/json"); //添加正文到rsp.body中
    return;
  }
  
  //将得到的博客信息插入到数据库
  ret = table_tag->Insert(tag);
  if (ret == false) {
    printf("InsertBlog insert into database failed!\n");
    rsp.status = 500;
    return;
  }
  rsp.status = 200;

  return;
}

void DeleteTag(const Request& req, Response& rsp) {
  int tag_id = std::stoi(req.matches[1]);
  bool ret = table_tag->Delete(tag_id);
  if (ret == false) {
    printf("DaleteBlog delete from database failed!\n");
    rsp.status = 500;
    return ;
  }

  rsp.status = 200;
  return;
}

void UpdateTag(const Request& req, Response& rsp) {
  int tag_id = std::stoi(req.matches[1]);
  Json::Value tag;
  Json::Reader reader;
  
  bool ret = reader.parse(req.body, tag);
  if (ret == false) {
    printf("UpdateBlog pares json failed!\n");
    rsp.status = 400;
    return ;
  }

  tag["id"] = tag_id;
  ret = table_tag->Update(tag);
  if (ret == false) {
    printf("UpdateBlog update database failed!\n");
    rsp.status = 500;
    return ;
  }

  rsp.status = 200;
  return;
}

void GetAllTag(const Request& req, Response& rsp) {
  Json::Value tags;
  bool ret = table_tag->GetAll(&tags);
  if (ret == false) {
    printf("GetAllTag select from database failed!\n");
    rsp.status = 500;
    return ;
  }

  //将数据进行json序列化, 添加到rsp正文中
  Json::FastWriter writer;
  rsp.set_content(writer.write(tags), "application/json");
  rsp.status = 200;
  return;
}


void GetOneTag(const Request& req, Response& rsp) {
  int tag_id = std::stoi(req.matches[1]);
  Json::Value tag;
  tag["id"] = tag_id;
  bool ret = table_tag->GetOne(&tag);
  if (ret == false) {
    printf("GetAllTag select from database failed!\n");
    rsp.status = 500;
    return ;
  }

  //将数据进行json序列化, 添加到rsp正文中
  Json::FastWriter writer;
  rsp.set_content(writer.write(tag), "application/json");
  rsp.status = 200;
  return;
}


//具体对应的业务处理
int main() {
  MYSQL* mysql = blog_system::MysqlInit();
  table_blog = new blog_system::TableBlog(mysql);
  table_tag= new blog_system::TableTag(mysql);

  //业务处理模块
  Server server;
  //设置相对根目录的目的: 当客户端请求静态文件资源时, httplib会直接根据路径读取文件数据进行响应
  //  /index.html --> ./www/index.html
  server.set_base_dir("./www"); //设置url中的资源相对根目录, 并且在请求/时候自动添加index.html
  //博客信息的增删查改
  server.Post("/blog", InsertBlog); 
  //正则表达式: \d--匹配数字字符  +表示匹配前面的字符一次或多次  ()为了临时保存匹配的数据
  // /blog/(\d+) 表示匹配以/blog/开头, 后面跟了一个数字的字符串格式, 并且临界保存后边的数字
  server.Delete(R"(/blog/(\d+))", DeleteBlog); //R"()" 取出括号中所有的特殊字符
  server.Put(R"(/blog/(\d+))", UpdateBlog);
  server.Get("/blog", GetAllBlog);
  server.Get(R"(/blog/(\d+))", GetOneBlog);

  //标签信息的增删查改
  server.Post("/tag", InsertTag); 
  server.Delete(R"(/tag/(\d+))", DeleteTag); 
  server.Put(R"(/tag/(\d+))", UpdateTag);
  server.Get("/tag", GetAllTag);
  server.Get(R"(/tag/(\d+))", GetOneTag);



  //网络通信 --- 搭建http服务器

  server.listen("0.0.0.0", 9000);
  return 0;
}



/*
void test() {
  MYSQL* mysql = blog_system::MysqlInit();
  blog_system::TableBlog table_blog(mysql);
  
  Json::Value blog;
  blog["id"] = 1;
  //blog["tag_id"] = 3;
  //blog["title"] = "这是C++博客";
  //blog["content"] = "C++太难啦~~~~";
  //table_blog.Insert(blog);
  //table_blog.Update(blog);
  table_blog.GetOne(&blog);
  Json::StyledWriter writer;
  std::cout << writer.write(blog) << std::endl;



  blog_system::TableTag table_tag(mysql);
  Json::Value tag;
  //tag["name"] = "C#";
  //tag["id"] = 1;
  //table_tag.Insert(tag); 
  //table_tag.Delete(4); 
  table_tag.GetOne(&tag);

  Json::StyledWriter writer;
  std::cout << writer.write(tag) << std::endl;
}
*/
