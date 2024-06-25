// 包含基本数据类型定义的头文件
#include "kernel/types.h"
// 包含文件状态信息定义的头文件
#include "kernel/stat.h"
// 包含用户空间函数定义的头文件
#include "user/user.h"
// 包含文件系统相关函数定义的头文件
#include "kernel/fs.h"

// 定义find函数，用于在指定目录下查找文件
void find(char *path, char *des) {
    char buf[512], *p; // 定义缓冲区和指针用于构建路径
    int fd; // 定义文件描述符
    struct dirent de; // 定义dirent结构体，用于存储目录项信息
    struct stat st; // 定义stat结构体，用于存储文件状态信息

    // 尝试打开指定路径的目录，如果失败打印错误并返回
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 获取打开目录的文件状态，如果失败打印错误，关闭目录并返回
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // 检查路径是否为目录，如果不是目录打印用法信息并返回
    if (st.type != T_DIR) {
        fprintf(2, "usage: find <DIRECTORY> <filename>\n");
        return;
    }

    // 检查路径长度是否过长，如果是打印错误并返回
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        fprintf(2, "find: path too long\n");
        return;
    }

    // 将路径复制到缓冲区，并在末尾添加一个分隔符'/'
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    // 循环读取目录项，直到没有更多项
    while (read(fd, &de, sizeof de) == sizeof de) {
        // fprintf(1, "%d %s\n", de.inum, de.name);
        // 0表示当前目录文件读取完毕
        if (de.inum == 0) {
            continue;
        }
        // 将目录项名称复制到缓冲区的路径末尾
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0; // 设置字符串结束标志
        // 获取当前目录项的文件状态，如果失败打印错误并继续循环
        if (stat(buf, &st) < 0) {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }
        // 如果当前目录项是目录且不是'.'或'..'，则递归调用find函数
        if (st.type == T_DIR && strcmp(p, ".") != 0 && strcmp(p, "..") != 0) {
            find(buf, des);
        } 
        // 如果当前目录项是文件且文件名匹配des参数，则打印路径
        else if (strcmp(des, p) == 0) {
            printf("%s\n", buf);
        }
    }

    // 关闭目录文件描述符
    close(fd);
}

// 定义主函数，程序入口点
int main(int argc, char *argv[]) {
    // 检查命令行参数数量，如果少于3个则退出程序
    if (argc < 3) {
        exit(0);
    }
    // 从命令行参数中获取要搜索的路径和文件名
    char *path = argv[1];
    char *des = argv[2];
    // 调用find函数开始搜索
    find(path, des);
    // 程序正常退出
    exit(0);
}