#ifndef COMMONDEF_H
#define COMMONDEF_H

#include <QString>
#include <QStringLiteral>
#include <QList>

//当前处理模式
typedef enum
{
    EM_CURRENT_MODE_NORMAL,                     //正常
    EM_CURRENT_MODE_READY_DELETE,               //删除模式
    EM_CURRENT_MODE_DELETE_MENU,                //删除菜单
    EM_CURRENT_MODE_DELETE_ACTION,              //删除动作
    EM_CURRENT_MODE_READY_MODIFY,          //修改模式
    EM_CURRENT_MODE_MODIFY_MENU_NAME,      //修改菜单名称
    EM_CURRENT_MODE_MODIFY_ACTION_NAME,    //修改动作名称
    EM_CURRENT_MODE_MODIFY_ACTION_CONFIG_PATH,  //修改动作配置路径
    EM_CURRENT_MODE_MODIFY_ACTION_KEY,      //修改动作配置键值
    EM_CURRENT_MODE_MODIFY_ACTION_CONFIG,  //修改动作配置
    EM_CURRENT_MODE_READY_ADD,             //添加模式
    EM_CURRENT_MODE_ADD_MENU,              //添加菜单
    EM_CURRENT_MODE_ADD_ACTION_NAME,       //添加动作名称
    EM_CURRENT_MODE_ADD_ACTION_CONFIG_FILE,//添加配置文件
    EM_CURRENT_MODE_ADD_ACTION_KEY,        //添加动作查找键值
}EM_CURRENT_MODE;

typedef enum
{
    EM_MENU_TYPE_MENU,
    EM_MENU_TYPE_ACTION,
}EM_MENU_TYPE;

//单个控件配置
struct ST_ITEM_CONFIG
{
    ST_ITEM_CONFIG():iX(0),iY(0),iWidth(0),iHeigth(0){}
    QString strType;        //控件类型
    QString strName;        //控件名称
    int iX;                 //控件左偏移量
    int iY;                 //控件右偏移量
    int iWidth;             //控件宽
    int iHeigth;            //控件高
    QString strText;        //控件显示文本
    QString strStyleSheet;  //控件样式配置
};

//目录或者动作配置
struct ST_MENU_OR_ACTION
{
    ST_MENU_OR_ACTION():emType(EM_MENU_TYPE_MENU){}
    EM_MENU_TYPE emType;
    QString strName;                //目录或者action名称
    QString strJsonPath;            //指向报文结构json文件
    QString strKeyWord;             //json文件中key值
    QMap<QString,ST_MENU_OR_ACTION> stMenuActoinMap;
    void clear()
    {
        emType = EM_MENU_TYPE_MENU;
        strName.clear();
        strJsonPath.clear();
        strKeyWord.clear();
        stMenuActoinMap.clear();
    }
};


//界面整体配置
typedef struct _ST_UI_CONFIG_
{
    _ST_UI_CONFIG_():iWidth(800),iHeigth(600){}
    int iWidth;                         //整体宽度
    int iHeigth;                        //整体高度
    QMap<QString,ST_MENU_OR_ACTION> stMenuMap;//目录配置
}ST_UI_CONFIG;

//文件类型
typedef enum
{
    EM_PARSE_FILE_TYPE_BINARY,
    EM_PARSE_FILE_TYPE_HEX,
}EM_PARSE_FILE_TYPE;

#endif // COMMONDEF_H
