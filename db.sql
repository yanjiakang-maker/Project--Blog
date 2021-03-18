drop database if exists db_blog;

create database if not exists db_blog;
use db_blog;

drop table if exists tb_tag;

create table if not exists tb_tag (
  id int primary key auto_increment comment '标签ID',
  name varchar(32) comment '标签名称'
);

drop table if exists tb_blog;

create table if not exists tb_blog(
  id int primary key auto_increment comment '博客ID',
  tag_id int comment '所属标签的ID',
  title varchar(255) comment '博客标题',
  content text comment '博客内容',
  ctime datetime comment '博客创建时间'
);

insert tb_tag values(null, 'C++'), (null, 'Java'), (null, 'Linux'), (null, 'MySQL');
insert tb_blog values (null, 1, '这是一个C++博客', '##C++是最好的语言', now()),
                      (null, 2, '这是一个Java博客', '##Java是最好的语言', now()),
                      (null, 3, '这是一个Linux博客', '##Linux好难丫~', now());
