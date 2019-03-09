#ifndef CONFIG_H
#define CONFIG_H

#include <QtCore>

#define IP "127.0.0.1"
//#define IP "192.168.0.120"
#define PORT_COMMAND 8888
#define PORT_FILE 8848

#define USE_THREAD_TRANSFER
//#define USE_QTCREATOR

#define SEND_BLOCK_SIZE (qint64)(4 * 1024 * 1024)

#define SPLIT_TYPE_INFO_MAEK        "##"
#define SPLIT_ADDITION_INFO_MARK    ",,"

#define K_ERROR           false
#define K_SUCCEED         true

#define ERROR_CODE_1    "设置的文件路径错误"
#define ERROR_CODE_2    "要上传的文件打开失败"
#define ERROR_CODE_3    "命令发送失败"
#define ERROR_CODE_4    "文件头部信息接收失败"
#define ERROR_CODE_5    "文件数据接收失败"
#define ERROR_CODE_6    "磁盘空间已不足,无法存储该文件"
#define ERROR_CODE_7    "不能再次点击取消操作"
#define ERROR_CODE_8    "查询文件是否存在，附加信息错误"
#define ERROR_CODE_9    "设置的缓存目录错误"
#define ERROR_CODE_10   "文件在缓存中不存在"
#define ERROR_CODE_11   "文件传输超时"

#define SUCCEED_CODE_1    "文件头部信息接收成功"
#define SUCCEED_CODE_2    "接收端文件数据接收完成"
#define SUCCEED_CODE_3    "接收端文件取消完成"
#define SUCCEED_CODE_4    "文件在缓存中存在"
#define SUCCEED_CODE_5    "磁盘空间大小足够存储该文件"

//错误分类
enum ERROR_CLASSIFY_EM
{
	K_FILE_TRANSFER_ERROR = 100,
	K_NETWORK_ERROR = K_FILE_TRANSFER_ERROR + 200,
	K_RESPONSE_TIMEOUT = K_NETWORK_ERROR + 200
};

//文件传输错误
enum K_FILE_TRANSFER_ERROR_EM
{
	K_FILE_TRANSFER_CANCEL_ERROR = ERROR_CLASSIFY_EM::K_FILE_TRANSFER_ERROR + 1			//传输取消错误
};

//网络错误
enum K_NETWORK_ERROR_EM
{
	K_NETWORK_DISCONNECTED_COMMAND_ERROR = ERROR_CLASSIFY_EM::K_NETWORK_ERROR + 1,		//网络连接命令socket错误
	K_NETWORK_DISCONNECTED_FILE_ERROR													//网络连接文件socket错误
};

enum K_RESPONSE_TIMEOUT_EM
{
	K_RESPONSE_CANCEL_FILE_TIMEOUT = ERROR_CLASSIFY_EM::K_RESPONSE_TIMEOUT + 1,			//取消文件超时
	K_RESPONSE_IS_DISK_FREE_SPACE_FILE_TIMEOUT,											//判断磁盘空间是否不足超时
	K_RESPONSE_IS_EXIST_FILE_TIMEOUT,													//判断文件是否存在超时
	K_RESPONSE_FILE_TRANSFER_TIMEOUT													//文件传输超时
};							

//发送的命令类型
#define FILE_HEAD_CODE                              77          //发送文件头信息
#define FILE_HEAD_REC_CODE                          78          //接收文件头响应
#define FILE_CANCEL                                 79          //发送取消文件传输信息
#define FILE_REC_CANCEL                             80          //接收取消传输响应
#define FILE_CODE                                   81          //发送文件数据
#define FILE_REC_CODE                               82          //接收文件响应
#define FILE_IS_EXIST_CODE                          83          //发送查询文件是否存在请求
#define FILE_IS_EXIST_REC_CODE                      84          //接收查询文件是否存在响应
#define FILE_IS_DISK_FREE_SPACE_CODE                85          //发送查询磁盘空间是否不足
#define FILE_IS_DISK_FREE_SPACE_REC_CODE            86          //接收查询磁盘空间是否不足响应

//服务端通知客户端
#define FILE_TRANSFER_TIMEOUT_CODE					100			//文件传输超时,服务端主动通知客户端
#define FILE_TRANSFER_FINISH_CODE					101			//文件传输完成


typedef struct _checkfileStru
{
    QString filePath;
    qint64  fileSize = 0;
	QString md5Str;
    bool    bExist = true;
} checkfileStru;


#endif // CONFIG_H
