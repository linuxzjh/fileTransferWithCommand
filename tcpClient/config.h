#ifndef CONFIG_H
#define CONFIG_H

#define IP "127.0.0.1"
#define PORT1 8888
#define PORT2 8848

#define SEND_BLOCK_SIZE (qint64)(4 * 1024 * 1024)

#define ERROR           false
#define SUCCEED         true

#define ERROR_CODE_1    "设置的文件路径错误 "
#define ERROR_CODE_2    "要上传的文件打开失败 "
#define ERROR_CODE_3    "命令发送失败 "
#define ERROR_CODE_4    "文件头部信息接收失败 "
#define ERROR_CODE_5    "文件数据接收失败 "

#define SUCCEED_CODE_1    "文件头部信息接收成功 "
#define SUCCEED_CODE_2    "接收端文件数据接收完成 "
#define SUCCEED_CODE_3    "接收端文件取消完成 "

//发送的命令类型
#define FILE_HEAD_CODE          77          //发送文件头信息
#define FILE_HEAD_REC_CODE      78          //接收文件头信息
#define FILE_CANCEL             79          //发送取消文件传输信息
#define FILE_REC_CANCEL         80          //接收取消传输信息
#define FILE_CODE               81          //发送文件数据
#define FILE_REC_CODE           82          //接受文件信息


#endif // CONFIG_H
