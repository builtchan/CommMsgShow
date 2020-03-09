#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QComboBox>
#include <QMenuBar>
#include <QMenu>
#include <QTextEdit>
#include <QLineEdit>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QFileDialog>
#include "commonDef.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>
//#include "UIConfig.h"
#include "CTextParse.h"
class CUIConfig : public QMainWindow
{
    Q_OBJECT

public:
    CUIConfig(QWidget *parent = nullptr);
    ~CUIConfig() override;

    /*
     * initUI  根据配置文件初始化界面，包括大小，输入输出框，菜单栏等
     */
    void initUI();

    //获取当前运行路径
    const QString &getCurrentPath();

    /*
     *  parseUIConfig 界面配置解析 默认文件名称
     *  [out]  stUIConfig 界面配置信息
     */
    bool parseUIConfig(ST_UI_CONFIG &stUIConfig);

    /*
     *  genUIConfig 生成配置文件
     *  [in]  stUIConfig 界面配置信息
     */
    bool genUIConfig(const ST_UI_CONFIG &stUIConfig);

    //清空 ST_UI_CONFIG 结构体
    void clearUIConfigStruct(ST_UI_CONFIG &stUIConfig);

private:

    void initDefaultUI();               //第一次没有配置文件等，生成默认配置，并提示
    void genExampleJson();             //生成报文解析结构配置例子

    /*
     * createMenu 根据Map生成菜单栏
     * [in] pMenu 菜单对象
     * [in] MenuMap 菜单配置
     * [in] bShowAdd 是否添加新增按钮
     */
    bool createMenu(QMenuBar *pMenuWidget, QMenu *pMenu, const QMap<QString, ST_MENU_OR_ACTION> &menuMap,bool bShowAdd = false);

    /*
     *  parseMenu  从配置文件中解析menu
     *  [in] menu 元素
     *  [out] MenuMap 菜单map表
     *  [in] strLevelKey 菜单内部码
     */
    bool parseMenu(QDomElement menu , QMap<QString, ST_MENU_OR_ACTION> &MenuMap, QString strLevelKey="");

    /*
     *  genElement  根据菜单map表生成新的元素
     *  [in] MenuMap 菜单map表
     *  [out] menu xml元素对象
     */
    void genElement(const QMap<QString, ST_MENU_OR_ACTION> &MenuMap, QDomElement &menu);


    /*  findMenuAndUpdateConfigFile 查找处理
     *  [in] MenuMap 需要从中查找的Map
     *  [in] strLevelKey 内部查询键值
     */
    bool findMenuAndUpdateConfigFile(QMap<QString, ST_MENU_OR_ACTION> &MenuMap, QString strLevelKey);

    /*
     * parseFile2Hex 读取待解析文件
     * [in] emFileType 文件类型
     */
    void parseFile2Hex(const QString& strfileName,EM_PARSE_FILE_TYPE emFileType);

    /*  dealManageAction 处理管理 动作
     *  [in]    pAction 触发动作对象
     *  [return]    是否是管理动作
     */
    bool dealManageAction(QAction *pAction);

    /*  createDirOrFile 处理生成文件或者文件夹
     *  [in]        文件夹路径或者文件路径
     *  [return]    true or false
     */
    bool createDirOrFile(QString strPath);
private:
    //删除菜单
    void onDeleteMenu();
    //确认删除菜单
    void onConfirmDeleteMenu();
    //删除动作
    void onDeleteAction();
    //确认删除动作
    void onConfirmDeleteAction();

    //修改菜单
    void onModifyMenuName();
    //确认修改菜单名称
    void onConfirmModifyMenuName();
    //修改动作名称
    void onModifyActionName();
    //修改动作配置路径
    void onModifyActionConfigPath();
    //修改动作键值
    void onModifyActionKey();
    //修改动作配置
    void onModifyActionConfig();
    //确认修改动作名称
    void onConfirmModifyActionName();
    //确认修改动作配置路径
    void onConfirmModifyActionConfigPath();
    //确认修改动作键值
    void onConfirmModifyActionKey();
    //确认修改动作配置
    void onConfirmModifyActionConfig();

    //新增菜单
    void onAddMenu();
    //确认新增菜单
    void onConfirmAddMenu();
    //添加动作名称
    void onAddActionName();
    //添加动作配置文件
    void onAddActionConfigFile();
    //添加动作键值
    void onAddActionKey();
    //确认添加动作名称
    void onConfirmAddActionName();
    //确认添加动作配置文件
    void onConfirmAddActionConfigFile();
    //确认添加动作键值
    void onConfirmAddActionKey();

    //获取当前模式
    EM_CURRENT_MODE getCurrentMode();
    //设置当前模式
    void setCurrentMode(EM_CURRENT_MODE emCurrentMode);
private:

    QStackedWidget *m_pStackWidget;
    QWidget *m_pParseWidget;
    QWidget *m_pManagerWidget;
    QWidget *m_pDealJsonWidget;

    QString m_strCurrentPath;           //当前路径
    QString m_strDefaultJsonTip;        //默认json文件里的说明
    ST_UI_CONFIG m_stUIConfig;          //界面配置
    ST_MENU_OR_ACTION m_stMenuActionTemp;
    bool m_bUIConifgChange;

    //查找到的位置
    QMap<QString, ST_MENU_OR_ACTION> *m_pMenuSourceMap;
    //新增的内容
    QMap<QString, ST_MENU_OR_ACTION> m_MenuMap;
    //需要处理的层级菜单内部key
    QString m_strDealKey;
    //待处理的动作
    QAction *m_pWait2DealAction;
    QMenu *m_pWait2DealMenu;

    QMenuBar *m_pActionBar;

    QTextEdit *m_pSourceTextEdit;       //待解析数据框
    QTextEdit *m_pShowTextEdit;         //数据解析结果

    //当前处理模式
    EM_CURRENT_MODE m_emCurrentMode;

    QLabel *m_pDealTipLabel;            //模式处理label背景
    QPushButton *m_pCancelBtn;          //取消模式
    QPushButton *m_pComfirmlBtn;        //确认
    QPushButton *m_pSaveBtn;            //保存配置
    QPushButton *m_pGetFileName;        //文件选择

    QLineEdit *m_pTempTextEdit;        //新建解析配置文件名，默认在Config目录下
    QComboBox *m_pFileComBox;          //添加到现有文件中;

public slots:
    void onMenuAction(QAction *pAction);
    void onAboutToShow();
    void onCancelDealBtnClick();
    void onComfirmBtnClick();
    void onSaveBtnClick();
    void onGetFileBtnClick();
    void onDropFile(QString str);

signals:
    void createNewMenu(QAction *pAction,QString strLevelKey);

protected:
    void closeEvent(QCloseEvent *e) override;
};

#endif // MAINWINDOW_H
