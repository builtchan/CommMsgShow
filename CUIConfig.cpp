#include "CUIConfig.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QString>
#include <QTextEdit>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QTextStream>
#include <QTextCodec>
#include "CTextParse.h"
#include <QMessageBox>
#include "CTextParse.h"
#include "CMyTextEdit.h"
#include <QFileDialog>
#include <QRegExp>
#include <QDesktopServices>
#pragma execution_character_set("utf-8")
CUIConfig::CUIConfig(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("数据解析工具1.0  by chenjc");
    m_pStackWidget = new QStackedWidget(this);
    m_pParseWidget = new QStackedWidget(this);
    m_pManagerWidget = new QStackedWidget(this);
    m_pDealJsonWidget = new QStackedWidget(this);

    m_pSourceTextEdit = new CMyTextEdit(this);
    m_pShowTextEdit = new QTextEdit(this);
    m_pActionBar = this->menuBar();
    m_pDealTipLabel = NULL;
    m_strCurrentPath = QCoreApplication::applicationDirPath();

    m_emCurrentMode = EM_CURRENT_MODE_NORMAL;
    m_bUIConifgChange = false;
    m_pWait2DealAction = NULL;
    m_pWait2DealMenu = NULL;


    m_strDefaultJsonTip = "{\r\t\"description0\": \"解析配置格式说明请在软件'管理'->'说明'查阅。type：[0:原文 1:字符串 2:整型 3:循环体 4:变长 5:按位处理]\",\
            \r\t\"%1\":[{\r\t\t\"name\":\"例子\",\r\t\t\"length\":2,\r\t\t\"type\":0\r\t\t}\r\t]\
            \r}";

    connect(m_pSourceTextEdit,SIGNAL(dropFile(QString)),this,SLOT(onDropFile(QString)));

    initUI();
}

CUIConfig::~CUIConfig()
{
    qDebug() << "~CUIConfig";
}

void CUIConfig::closeEvent(QCloseEvent *e)
{
    qDebug() << "closeEvent";
}

const QString& CUIConfig::getCurrentPath()
{
    return m_strCurrentPath;
}

/*
 * initUI
 */
void CUIConfig::initUI()
{
    QFileInfo fileinfo(getCurrentPath() + "/UI.xml");
    if(fileinfo.isFile())
    {
        if(!parseUIConfig(m_stUIConfig))
        {
            qDebug() << "parseUIConfig failed";
            return;
        }
    }
    else
    {
        if(0 == QMessageBox::question(this,"","第一次使用将在本地生成默认配置文件和Config文件夹哦","同意","不同意"))
            initDefaultUI();
        else
        {
            exit(6);
            return;
        }
    }

    this->setGeometry(400,200,m_stUIConfig.iWidth,m_stUIConfig.iHeigth);
    m_pParseWidget->setGeometry(0,0,m_stUIConfig.iWidth,m_stUIConfig.iHeigth);
    m_pManagerWidget->setGeometry(0,0,m_stUIConfig.iWidth,m_stUIConfig.iHeigth);
    m_pDealJsonWidget->setGeometry(0,0,m_stUIConfig.iWidth,m_stUIConfig.iHeigth);
    this->setFixedSize(m_stUIConfig.iWidth,m_stUIConfig.iHeigth);

    m_pStackWidget = new QStackedWidget(this);
    m_pParseWidget = new QStackedWidget(this);
    m_pManagerWidget = new QStackedWidget(this);
    m_pDealJsonWidget = new QStackedWidget(this);


    m_pSourceTextEdit->setGeometry(2,33,m_stUIConfig.iWidth-4,(m_stUIConfig.iHeigth - 40) / 2);
    m_pShowTextEdit->setGeometry(2,m_stUIConfig.iHeigth/2 + 18,m_stUIConfig.iWidth-4,(m_stUIConfig.iHeigth - 40) / 2);

    QFont font;
    font.setPixelSize(15);
    font.setFamily("Microsoft YaHei");
    m_pSourceTextEdit->setFont(font);
    m_pShowTextEdit->setFont(font);

    if(m_pActionBar)
        m_pActionBar->clear();
    m_pActionBar->setObjectName("menubar");
    createMenu(m_pActionBar,NULL,m_stUIConfig.stMenuMap);
    connect(m_pActionBar,SIGNAL(triggered(QAction*)),this,SLOT(onMenuAction(QAction*)));

    if(NULL == m_pDealTipLabel)
    {
        m_pDealTipLabel = new QLabel(this);
        m_pDealTipLabel->setGeometry(0,m_pActionBar->height(),m_stUIConfig.iWidth,m_stUIConfig.iHeigth-m_pActionBar->height());
        m_pDealTipLabel->setStyleSheet("QLabel{background-color:#C7EDCC;font-size:20pt;}");
        m_pDealTipLabel->hide();
        m_pDealTipLabel->setAlignment(Qt::AlignCenter);
        m_pDealTipLabel->setWordWrap(true);

        m_pCancelBtn = new QPushButton(m_pDealTipLabel);
        m_pCancelBtn->setGeometry(m_pDealTipLabel->width()-30,0,30,30);
        m_pCancelBtn->setStyleSheet("font-size:15pt;");
        m_pCancelBtn->setText("×");

        m_pComfirmlBtn = new QPushButton(m_pDealTipLabel);
        m_pComfirmlBtn->setGeometry(m_pDealTipLabel->width() / 2 - 90,m_pDealTipLabel->height() - 35,80,30);
        m_pComfirmlBtn->setStyleSheet("font-size:15pt;");
        m_pComfirmlBtn->setText("确认");
        m_pComfirmlBtn->hide();

        m_pSaveBtn = new QPushButton(m_pDealTipLabel);
        m_pSaveBtn->setGeometry(m_pComfirmlBtn->x() + m_pComfirmlBtn->width() + 20,m_pComfirmlBtn->y(),m_pComfirmlBtn->width(),m_pComfirmlBtn->height());
        m_pSaveBtn->setStyleSheet("font-size:15pt;");
        m_pSaveBtn->setText("保存");
        m_pSaveBtn->hide();

        m_pFileComBox = new QComboBox(m_pDealTipLabel);
        m_pFileComBox->setGeometry(10,m_pComfirmlBtn->y() - m_pComfirmlBtn->height() - 5,m_stUIConfig.iWidth - 10,m_pComfirmlBtn->height());
        m_pFileComBox->addItem("新建");
        m_pFileComBox->setStyleSheet("font-size:15pt;");
        m_pFileComBox->hide();

        m_pTempTextEdit = new QLineEdit(m_pDealTipLabel);
        m_pTempTextEdit->setGeometry(10,m_pComfirmlBtn->y() - m_pComfirmlBtn->height() - 5,m_stUIConfig.iWidth - 50,m_pComfirmlBtn->height());
        m_pTempTextEdit->setStyleSheet("font-size:15pt;");
        m_pTempTextEdit->hide();

        m_pGetFileName = new QPushButton(m_pDealTipLabel);
        m_pGetFileName->setGeometry(m_pTempTextEdit->x() + m_pTempTextEdit->width(),m_pComfirmlBtn->y() - m_pComfirmlBtn->height() - 5,m_stUIConfig.iWidth-m_pTempTextEdit->x()*2-m_pTempTextEdit->width(),m_pComfirmlBtn->height());
        m_pGetFileName->setText("...");
        m_pGetFileName->hide();

        connect(m_pCancelBtn,SIGNAL(clicked()),this,SLOT(onCancelDealBtnClick()));
        connect(m_pComfirmlBtn,SIGNAL(clicked()),this,SLOT(onComfirmBtnClick()));
        connect(m_pSaveBtn,SIGNAL(clicked()),this,SLOT(onSaveBtnClick()));
        connect(m_pGetFileName,SIGNAL(clicked()),this,SLOT(onGetFileBtnClick()));
    }
}

/*
 * createMenu 根据Map生成菜单栏
 * [in] pMenu 菜单对象
 * [in] MenuMap 菜单配置
 * [in] bShowAdd 是否添加新增按钮
 */
bool CUIConfig::createMenu(QMenuBar *pMenuBar, QMenu *pMenu, const QMap<QString, ST_MENU_OR_ACTION> &menuMap, bool bShowAdd)
{
    QString strKey = "00";
    if(NULL == pMenu)
    {
        qDebug() << "manage";
        QMenu *pMenu = new QMenu("管理",this);
        QAction *pActionAdd = new QAction("新建",this);
        pActionAdd->setToolTip("add");
        pMenu->addAction(pActionAdd);
        QAction *pActionDel = new QAction("删除",this);
        pActionDel->setToolTip("delete");
        pMenu->addAction(pActionDel);
        QAction *pActionChange = new QAction("修改",this);
        pActionChange->setToolTip("change");
        pMenu->addAction(pActionChange);
        QAction *pParseFile = new QAction("解析文件",this);
        pParseFile->setToolTip("ParseFile");
        pMenu->addAction(pParseFile);
        QAction *pIntroduce = new QAction("说明",this);
        pIntroduce->setToolTip("introduction");
        pMenu->addAction(pIntroduce);
        pMenuBar->addMenu(pMenu);
    }
    if(NULL == pMenuBar)
    {
        qDebug() << "NULL == pMenuBar";
        return false;
    }

    QMap<QString, ST_MENU_OR_ACTION>::const_iterator it = menuMap.begin();
    for(;it != menuMap.end();it++)
    {
        strKey = it.key();
        qDebug() << __FUNCTION__ << __LINE__ << "key " << strKey ;
        if(EM_MENU_TYPE_MENU == it.value().emType)
        {
            QMenu *pSubMenu = NULL;
            if(NULL == pMenu)
            {
                pSubMenu = new QMenu(it.value().strName,this);
                pMenuBar->addMenu(pSubMenu);
            }
            else
            {
                pSubMenu = new QMenu(it.value().strName,this);
                pMenu->addMenu(pSubMenu);
            }
            pSubMenu->setObjectName(it.value().strName);         //object name
            pSubMenu->setToolTip(it.value().strName);
            pSubMenu->setWhatsThis(it.key());
            connect(pSubMenu,SIGNAL(aboutToShow()),this,SLOT(onAboutToShow()));
            if(createMenu(pMenuBar,pSubMenu,it.value().stMenuActoinMap,bShowAdd))
            {
               qDebug() << "create menu" <<  it.value().strName;
               if(it.value().stMenuActoinMap.isEmpty() && bShowAdd)
               {
                   QAction *pAction = new QAction("+",this);
                   pAction->setToolTip("+");
                   pAction->setStatusTip("");
                   pAction->setWhatsThis(it.key()+"00");
                   pSubMenu->addAction(pAction);
               }
            }
        }
        else
        {
            QAction *pAction = new QAction(it.value().strName,this);
            pAction->setObjectName(it.value().strName);         //object name
            //处理方法json文件
            pAction->setToolTip(it.value().strJsonPath);
            //key值
            pAction->setStatusTip(it.value().strKeyWord);
            //菜单内部处理路径
            pAction->setWhatsThis(it.key());
            if(pMenu)
                pMenu->addAction(pAction);
            else
                pMenuBar->addAction(pAction);
            qDebug() << "create action " << it.value().strName;
        }
    }
    if(NULL == pMenu)
    {
        //第一梯队
        if(bShowAdd)
        {
            QAction *pAction = new QAction("+",this);
            pAction->setToolTip("+");
            pAction->setStatusTip("");
            pAction->setWhatsThis(strKey);//key
            qDebug() << __FUNCTION__<<__LINE__<<"strkey" << pAction->whatsThis();
            pMenuBar->addAction(pAction);
        }
        return false;
    }

    if(menuMap.isEmpty())
    {
        qDebug() << "menuMap.isEmpty" << strKey;
        return true;
    }

    //同一级
    if(bShowAdd)
    {
        QAction *pAction = new QAction("+",this);
        pAction->setToolTip("+");
        pAction->setStatusTip("");
        pAction->setWhatsThis(strKey);
        pMenu->addAction(pAction);
        qDebug() << __FUNCTION__<<__LINE__<<"strkey" << strKey;
    }

    return true;
}
//生成默认界面和菜单
void CUIConfig::initDefaultUI()
{
    m_pSourceTextEdit->setObjectName("sourceText");
    m_pSourceTextEdit->setText("20 20 02 02 68 65 6C 6C 6F 20 77 6F 72 6C 64 ff 00 02 00 01 00 aa 02 aa 00 02 31 32");
    m_pSourceTextEdit->setGeometry(5,35,400,200);

    m_pShowTextEdit->setObjectName("showText");
    m_pShowTextEdit->setGeometry(5,275,400,200);

    m_stUIConfig.stMenuMap.clear();
    m_stUIConfig.iWidth = 800;
    m_stUIConfig.iHeigth = 600;

    ST_MENU_OR_ACTION stMenu,stAction;
    stMenu.emType = EM_MENU_TYPE_MENU;
    stMenu.strName = "样例";

    stAction.emType = EM_MENU_TYPE_ACTION;
    stAction.strName = "example";
    stAction.strJsonPath = "example.json";
    stAction.strKeyWord = "example";
    stMenu.stMenuActoinMap.insert(stAction.strName,stAction);

    m_stUIConfig.stMenuMap.insert(stMenu.strName,stMenu);
    genExampleJson();
    genUIConfig(m_stUIConfig);
}

//生成默认报文解析文件
void CUIConfig::genExampleJson()
{
    QDir dir(m_strCurrentPath+"/Config");
    if(!dir.exists())
    {
        dir.mkdir(m_strCurrentPath +"/Config");
    }
    QMap<QString ,ST_TEXT_PARSE_FORMAT> stTextParseFormatMap;
    ST_TEXT_PARSE_FORMAT stTextFormat;
    stTextFormat.iLength = 0;
    stTextFormat.stProtocolDetailList.clear();
    ST_PROTOCOL_DETAIL stProtocolDetail;

    stProtocolDetail.strPrintName = "解析配置格式说明请在软件'管理'->'说明'查阅";
    stTextFormat.stProtocolDetailList.push_back(stProtocolDetail);
    stTextParseFormatMap.insert("description0",stTextFormat);
    stTextFormat.iLength = 0;
    stTextFormat.stProtocolDetailList.clear();

//    20 20 02 02 68 65 6C 6C 6F 20 77 6F 72 6C 64 ff 00 02 00 01 00 aa 02 aa 00 02 31 32";

    stProtocolDetail.strPrintName = "原始数据";
    stProtocolDetail.iLen = 4;
    stProtocolDetail.emTextType = EM_TEXT_TYPE_RAW;
    stTextFormat.stProtocolDetailList.push_back(stProtocolDetail);

    stProtocolDetail.strPrintName = "转成字符串";
    stProtocolDetail.iLen = 11;
    stProtocolDetail.emTextType = EM_TEXT_TYPE_ASIIC;
    stTextFormat.stProtocolDetailList.push_back(stProtocolDetail);

    stProtocolDetail.strPrintName = "转成整型";
    stProtocolDetail.iLen = 2;
    stProtocolDetail.emTextType = EM_TEXT_TYPE_INT;
    stTextFormat.stProtocolDetailList.push_back(stProtocolDetail);

    stProtocolDetail.strPrintName = "循环体";
    stProtocolDetail.iLen = 2;
    stProtocolDetail.emTextType = EM_TEXT_TYPE_LOOP_NUM;

    ST_PROTOCOL_DETAIL loop;
    loop.strPrintName = "inloop1";
    loop.emTextType = EM_TEXT_TYPE_RAW;
    loop.iLen = 1;
    stProtocolDetail.stLoopOrRuleList.push_back(loop);
    loop.strPrintName = "inloop2";
    loop.emTextType = EM_TEXT_TYPE_INT;
    loop.iLen = 2;
    loop.iIsBigEndian = 1;
    stProtocolDetail.stLoopOrRuleList.push_back(loop);
    stTextFormat.stProtocolDetailList.push_back(stProtocolDetail);

    stProtocolDetail.stLoopOrRuleList.clear();
    stProtocolDetail.strPrintName = "不定长（暂支持0和有长度）";
    stProtocolDetail.iLen = 1;
    loop.emLenType = EM_LENGTH_TYPE_ASIIC;
    stProtocolDetail.emTextType = EM_TEXT_TYPE_UNCERTAIN_LEN;
    loop.strPrintName = "1";
    loop.emTextType = EM_TEXT_TYPE_RAW;
    loop.iLen = 1;
    loop.iIsBigEndian = -1;
    stProtocolDetail.stLoopOrRuleList.push_back(loop);
    loop.strPrintName = "2";
    loop.emTextType = EM_TEXT_TYPE_RAW;
    loop.iLen = 1;
    stProtocolDetail.stLoopOrRuleList.push_back(loop);
    stTextFormat.stProtocolDetailList.push_back(stProtocolDetail);

    stTextParseFormatMap.insert("example",stTextFormat);
    CTextParse text;
    text.genJsonConfigFile(stTextParseFormatMap,m_strCurrentPath + "/Config/example.json");
}

/*
 *  parseUIConfig
 *  [out]  stUIConfig
 */
bool CUIConfig::parseUIConfig(ST_UI_CONFIG &stUIConfig)
{
    stUIConfig.stMenuMap.clear();
    QFileInfo fileinfo(m_strCurrentPath + "/UI.xml");
    if(!fileinfo.isFile())
    {
        qDebug() << "UI.xml no exist";
        return false;
    }
    QFile file(m_strCurrentPath + "/UI.xml");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "UI.xml open failed";
        return false;
    }

    QTextStream textStream(file.readAll());
    file.close();
    textStream.setCodec(QTextCodec::codecForName("UTF-8"));
    QDomDocument dom;
    dom.setContent(textStream.readAll());
    if(dom.isNull())
    {
        qDebug() << "QDomDocument is NULL";
        return false;
    }

    QDomElement UIConfig = dom.documentElement();
    QDomElement DocMain = UIConfig.firstChildElement("main");
    QDomElement menus = UIConfig.firstChildElement("menus");
    QDomElement menu = menus.firstChildElement("menu");

    if(!DocMain.isNull())
    {
        if(DocMain.hasAttribute("width"))
        {
            stUIConfig.iWidth = DocMain.attribute("width").toInt();
            qDebug() << "stUIConfig.iWidth" <<stUIConfig.iWidth;
        }
        else
            stUIConfig.iWidth = 400;

        if(DocMain.hasAttribute("heigth"))
        {
            stUIConfig.iHeigth = DocMain.attribute("heigth").toInt();
            qDebug() << "stUIConfig.iHeigth" <<stUIConfig.iHeigth;
        }
        else
            stUIConfig.iHeigth = 800;
    }

    return parseMenu(menu,stUIConfig.stMenuMap);
}
/*
 *  getMenu
 *  [in] NodeList
 *  [out] MenuMap
 *  [in] strLevelKey 菜单内部码
 */
bool CUIConfig::parseMenu(QDomElement menu , QMap<QString, ST_MENU_OR_ACTION> &MenuMap, QString strLevelKey)
{
    if(menu.isNull())
    {
        qDebug() << "menu.isNull()";
        return true;
    }
    int iLevel = 0;
    while(!menu.isNull())
    {
        ST_MENU_OR_ACTION stMenuOrAction;
        stMenuOrAction.stMenuActoinMap.clear();

        if(menu.hasAttribute("name"))
        {
            stMenuOrAction.strName = menu.attribute("name");
            qDebug() << "name" << stMenuOrAction.strName;
        }
        else
        {
            qDebug() << "menu hasn't name";
            return false;
        }
        if(menu.hasAttribute("jsonpath"))
        {
            stMenuOrAction.emType = EM_MENU_TYPE_ACTION;
            qDebug() << "type" << stMenuOrAction.emType;
            stMenuOrAction.strJsonPath = menu.attribute("jsonpath");
            qDebug() << "jsonpath" << stMenuOrAction.strJsonPath;

            if(menu.hasAttribute("key"))
            {
                stMenuOrAction.strKeyWord = menu.attribute("key");
                qDebug() << "key" << stMenuOrAction.strKeyWord;
            }
            else
            {
                qDebug() << "menu hasn't key";
                return false;
            }
        }
        else
        {
            stMenuOrAction.emType = EM_MENU_TYPE_MENU;
            qDebug() << "type" << stMenuOrAction.emType;
        }
        QString currentLevelKey = strLevelKey+QString("%1").arg(iLevel,2,16,QChar('0'));
        qDebug() << "currentLevelKey" << currentLevelKey;
        if(!parseMenu(menu.firstChildElement(),stMenuOrAction.stMenuActoinMap,currentLevelKey))
        {
            qDebug() << "parse failed";
            return false;
        }

        MenuMap.insert(currentLevelKey,stMenuOrAction);
        menu = menu.nextSiblingElement();
        iLevel++;
    }
    return true;
}

/*
 *  genUIConfig 生成配置文件
 *  [in]  stUIConfig 界面配置信息
 */
bool CUIConfig::genUIConfig(const ST_UI_CONFIG &stUIConfig)
{
    QFile file(m_strCurrentPath + "/UI.xml");
    if(file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        file.resize(0);
        QDomDocument doc;
        doc.appendChild(doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"utf-8\""));

        QDomElement root = doc.createElement("UIConfig");
        QDomElement main = doc.createElement("main");
        QDomElement menus = doc.createElement("menus");
        main.setAttribute("width",stUIConfig.iWidth);
        main.setAttribute("heigth",stUIConfig.iHeigth);

        genElement(stUIConfig.stMenuMap,menus);

        root.appendChild(main);
        root.appendChild(menus);
        doc.appendChild(root);
        file.write(doc.toByteArray());
        file.close();
    }
    return false;
}

/*
 *  genElement  根据菜单map表生成新的元素
 *  [in] MenuMap 菜单map表
 *  [out] menu xml元素对象
 */
void CUIConfig::genElement(const QMap<QString, ST_MENU_OR_ACTION> &MenuMap, QDomElement &menus)
{
    if(MenuMap.isEmpty())
    {
        return;
    }

    QDomDocument doc;
    foreach (ST_MENU_OR_ACTION stMenuOrAction, MenuMap) {

        QDomElement newElement = doc.createElement("menu");
        newElement.setAttribute("name",stMenuOrAction.strName);
        if(EM_MENU_TYPE_ACTION == stMenuOrAction.emType)
        {
            newElement.setAttribute("jsonpath",stMenuOrAction.strJsonPath);
            newElement.setAttribute("key",stMenuOrAction.strKeyWord);
        }
        genElement(stMenuOrAction.stMenuActoinMap,newElement);
        menus.appendChild(newElement);
    }
}

/*  findMenuAndUpdateConfigFile 查找处理
 *  [in] MenuMap 需要从中查找的Map
 *  [in] strLevelKey 内部查询键值
 */
bool CUIConfig::findMenuAndUpdateConfigFile(QMap<QString, ST_MENU_OR_ACTION> &MenuMap, QString strLevelKey)
{
    if(MenuMap.isEmpty())
    {
        return false;
    }
    for(QMap<QString, ST_MENU_OR_ACTION>::iterator it = MenuMap.begin();it != MenuMap.end();it++)
    {
        qDebug() << "findMenuAndUpdateConfigFile" << it.key() << "key" << strLevelKey << "in size " << it.value().stMenuActoinMap.size() << "mode " << getCurrentMode();
        if(it.key().length() != strLevelKey.length()
                && it.key() == strLevelKey.left(it.key().length()))
        {
            //一般在添加模式里
            if(it.value().stMenuActoinMap.isEmpty())
            {
                m_pMenuSourceMap = &it.value().stMenuActoinMap;
                break;
            }
            else if(findMenuAndUpdateConfigFile(it.value().stMenuActoinMap,strLevelKey))
            {
                qDebug() << "findMenuAndUpdateConfigFile getCurrentMode " << getCurrentMode();
                qDebug() <<  "findMenuAndUpdateConfigFile" << it.key() << "key" << strLevelKey << "in size " << it.value().stMenuActoinMap.size() << "mode " << getCurrentMode();
                m_pMenuSourceMap = &it.value().stMenuActoinMap;
                m_strDealKey = strLevelKey;
                break;
            }
        }
        else if(it.key().length() == strLevelKey.length() && it.key() == strLevelKey)
        {
            m_pMenuSourceMap = &MenuMap;
            qDebug() <<  "findMenuAndUpdateConfigFile" << "直接 找到" << it.key() << it.value().strName;
            m_strDealKey = strLevelKey;
            return true;
        }
    }
    return false;
}

/*  dealManageAction 处理管理 动作
 *  [in]    pAction 触发动作对象
 *  [return]    是否是管理动作
 */
bool CUIConfig::dealManageAction(QAction *pAction)
{
    //进入删除模式
    if("delete" == pAction->toolTip())
    {
        if(EM_CURRENT_MODE_READY_ADD <= getCurrentMode() && getCurrentMode() <= EM_CURRENT_MODE_ADD_ACTION_KEY)
        {
            m_pActionBar->clear();
            createMenu(m_pActionBar,NULL,m_stUIConfig.stMenuMap);
        }
        setCurrentMode(EM_CURRENT_MODE_READY_DELETE);
        m_pDealTipLabel->setText("如果是动作菜单，请直接点击，如果删除普通菜单，请停留选择并点击确认按钮");
        m_pDealTipLabel->show();
    }
    else if("add" == pAction->toolTip())
    {
        m_pActionBar->clear();
        createMenu(m_pActionBar,NULL,m_stUIConfig.stMenuMap,true);
        setCurrentMode(EM_CURRENT_MODE_READY_ADD);
        m_pDealTipLabel->setText("请确认需要添加的菜单位置，并点击同等级菜单下\"+\"进行新增");
        m_pDealTipLabel->show();
        m_pTempTextEdit->setFocus(Qt::FocusReason::MouseFocusReason);
    }
    else if("change" == pAction->toolTip())
    {
        if(EM_CURRENT_MODE_READY_ADD <= getCurrentMode() && getCurrentMode() <= EM_CURRENT_MODE_ADD_ACTION_KEY)
        {
            m_pActionBar->clear();
            createMenu(m_pActionBar,NULL,m_stUIConfig.stMenuMap);
        }
        setCurrentMode(EM_CURRENT_MODE_READY_MODIFY);
        m_pDealTipLabel->setText("如果是动作菜单，请直接点击，如果删除普通菜单，请停留选择并点击确认按钮");
        m_pDealTipLabel->show();
    }
    else if("ParseFile" == pAction->toolTip())
    {
        QUrl file = QFileDialog::getOpenFileUrl(this,"请选择要解析的文件",QUrl(QString("file:///%1").arg(getCurrentPath())),"*");
        if(file.isEmpty())
            return true;
        QMessageBox messageBox;
        messageBox.setIcon(QMessageBox::Question);
        messageBox.setWindowTitle("请选择");
        messageBox.setText("请选择");
        QPushButton menuBtn;
        menuBtn.setText("二进制文件");
        QPushButton actionBtn;
        actionBtn.setText("源文件输出");
        messageBox.addButton(&menuBtn, QMessageBox::AcceptRole);
        messageBox.addButton(&actionBtn,QMessageBox::AcceptRole);
        int iRet = messageBox.exec();
        parseFile2Hex(file.toLocalFile(),EM_PARSE_FILE_TYPE(iRet));
    }
    else if("introduction" == pAction->toolTip())
    {
        QString strPattem = "<font color=red><b>%1</b></font>";
        QString strIntroduction = QString("%1%2").arg(strPattem).arg("name");
        m_pShowTextEdit->clear();
        m_pShowTextEdit->append(QObject::tr("<font size='+2'><font color=blue><b>配置文件说明</b></font></font>"));
        m_pShowTextEdit->append("<font color=red><b>UI.xml</b></font> 界面配置文件，保存宽高，菜单信息");
        m_pShowTextEdit->append("<font color=green><b>name</b></font> 菜单中需要展示的名称");
        m_pShowTextEdit->append("<font color=green><b>jsonpath</b></font> 动作相应的数据解析格式配置文件，当有这个属性时，说明这是一个动作。路径默认在程序当前文件夹的Config下");
        m_pShowTextEdit->append("<font color=green><b>key</b></font> 动作对应的键值，用于在数据解析格式配置文件中查找对应解析方法");
        m_pShowTextEdit->append("<font color=red><b>Config下的配置文件</b></font> 数据解析格式配置文件，一个文件里可以有多个解析方法，对应不同key值");

        m_pShowTextEdit->append(QObject::tr("<font size='+2'><font color=blue><b>使用说明</b></font></font>"));
        m_pShowTextEdit->append("<font color=red><b>上方输入框</b></font> 可输入待解析的数据");
        m_pShowTextEdit->append("<font color=red><b>下方方输入框</b></font> 展示解析结果");
        m_pShowTextEdit->append("<font color=red><b>解析数据格式</b></font> 仅支持asiic码的16进制字符串形式，如3132 或者 ff aa，且格式必须统一一种");
        m_pShowTextEdit->append("<font color=red><b>普通菜单</b></font> 相当于文件夹，可根据自己喜好进行分类，暂只支持创建时规划好，否则需要删除新建");
        m_pShowTextEdit->append("<font color=red><b>动作菜单</b></font> 动作，用于触发解析数据，动作数据中包含对应的解析格式配置文件和对应的键值，该动作的键值对应解析格式配置文件里一组解析格式的key值");
        m_pShowTextEdit->append(QObject::tr("<font size='+2'><font color=blue><b>管理</b></font></font>"));
        m_pShowTextEdit->append("<font color=red><b>新建</b></font> 可以添加新的菜单或者动作，菜单相当于文件夹，动作相当于执行命令");
        m_pShowTextEdit->append("<font color=red><b>修改</b></font> 可以修改菜单名称，动作的名称，动作键值和动作绑定的解析文件");
        m_pShowTextEdit->append("<font color=red><b>删除</b></font> 可以删除菜单和动作");
        m_pShowTextEdit->append("<font color=red><b>解析文件</b></font> 可以打开需要解析的文件，也可以直接拖动需要解析的文件到上方的输入框");

        m_pShowTextEdit->append(QObject::tr("<font size='+2'><font color=blue><b>解析格式配置文件说明</b></font></font>"));
        m_pShowTextEdit->append(QObject::tr("<font color=green><b>name</b></font> 解析名称"));
        m_pShowTextEdit->append(QObject::tr("<font color=green><b>remark</b></font> 备注"));
        m_pShowTextEdit->append(QObject::tr("<font color=green><b>length</b></font> 占用字节数，小于等于0时，只显示remark"));
        m_pShowTextEdit->append(QObject::tr("<font color=green><b>bigEndian</b></font> 是否是大端数据，默认小端数据。值为true或者false，type为2、3、4、5时可用"));
        m_pShowTextEdit->append(QObject::tr("<font color=green><b>lengthType</b></font> 数字转换类型 1：整型（7B 00） 2：BCD（01 23） 3：Asiic（31 32 33）。type为2、3、4时使用,没配置默认为整型处理。"));
        m_pShowTextEdit->append(QObject::tr("<font color=green><b>index</b></font> type为3、4、5时使用，用于指定整数字段的相对位置，对应解析成循环体个数或者长度"));
        m_pShowTextEdit->append(QObject::tr("<font color=green><b>loopNum</b></font> type为3时使用，当没有循环体个数字段时使用,该值可以大于实际循环体数量"));
        m_pShowTextEdit->append(QObject::tr("<font color=green><b>array</b></font> 数组类型，type为3、4、5时必须有"));
        m_pShowTextEdit->append(QObject::tr("<font color=red><b>type</b></font>"));
        m_pShowTextEdit->append(QObject::tr("<font color=green><b>type=0</b></font> 保持原报文输出"));
        m_pShowTextEdit->append(QObject::tr("<font color=green><b>type=1</b></font> 转成字符串"));
        m_pShowTextEdit->append(QObject::tr("<font color=green><b>type=2</b></font> 转成10进制整数 默认处理小端字节序，如果需要指定处理大小端 请添加 bigEndian：true/false"));
        m_pShowTextEdit->append(QObject::tr("<font color=green><b>type=3</b></font> 循环体，此时length代表循环体个数字段字节数，会转换成循环体个数，转换规则和type=2一致。如循环体数据在前两字节位置，index=-2，length=1。当有loopNum字段时，不使用length和index。应有 array:[{}] 作为该段的循环体结构"));
        m_pShowTextEdit->append(QObject::tr("<font color=green><b>type=4</b></font> 不定长数据，此时length代表长度字段字节数，会转换成该组长度，转换规则和type=2一致。如长度数据在前两字节位置，index=-8，length=4。当有数据长度时，应有 array:[{}]作为该段的解析方法"));
        m_pShowTextEdit->append(QObject::tr("<font color=green><b>type=5</b></font> 按位解析数据，应有 array:[{}]作为该段的解析方法，此时array数组中的length均作为‘位数’处理"));
    }
    else
        return false;
    return true;
}

//数据处理菜单动作
void CUIConfig::onMenuAction(QAction *pAction)
{
    qDebug() << pAction->toolTip() << pAction->statusTip() << pAction->whatsThis();

    if(dealManageAction(pAction))
        return;

    switch(getCurrentMode())
    {
    case EM_CURRENT_MODE_NORMAL:
        break;
    case EM_CURRENT_MODE_READY_ADD:
    case EM_CURRENT_MODE_ADD_ACTION_KEY:
    case EM_CURRENT_MODE_ADD_ACTION_NAME:
    case EM_CURRENT_MODE_ADD_ACTION_CONFIG_FILE:
    case EM_CURRENT_MODE_ADD_MENU:
        if(pAction->toolTip().contains("+"))
        {
            QMessageBox messageBox;
            messageBox.setIcon(QMessageBox::Question);
            messageBox.setWindowTitle("请选择");
            messageBox.setText("是添加菜单请点击ok，是动作请点击no，取消请点x");
            QPushButton menuBtn;
            menuBtn.setText("添加菜单");
            QPushButton actionBtn;
            actionBtn.setText("添加动作");
            QPushButton cancelBtn;
            cancelBtn.setText("取消");
            messageBox.addButton(&menuBtn, QMessageBox::AcceptRole);
            messageBox.addButton(&actionBtn,QMessageBox::ApplyRole);
            messageBox.addButton(&cancelBtn,QMessageBox::NoRole);
            int iRet = messageBox.exec();
//            int iRet = QMessageBox::question(this,"warning","是添加菜单请点击ok，是动作请点击no，取消请点x",QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
            qDebug() << iRet << pAction->whatsThis();
            m_strDealKey = pAction->whatsThis();
            m_pWait2DealAction = pAction;
            findMenuAndUpdateConfigFile(m_stUIConfig.stMenuMap,pAction->whatsThis());
            if(0 == iRet)
            {
                onAddMenu();
            }
            else if(1 == iRet)
            {
                onAddActionName();
            }
        }
        return;
    case EM_CURRENT_MODE_READY_DELETE:
    case EM_CURRENT_MODE_DELETE_MENU:
    case EM_CURRENT_MODE_DELETE_ACTION:
        m_pWait2DealAction = pAction;
        findMenuAndUpdateConfigFile(m_stUIConfig.stMenuMap,pAction->whatsThis());
        onDeleteAction();
        return;
    case EM_CURRENT_MODE_READY_MODIFY:
    case EM_CURRENT_MODE_MODIFY_MENU_NAME:
    case EM_CURRENT_MODE_MODIFY_ACTION_NAME:
    case EM_CURRENT_MODE_MODIFY_ACTION_CONFIG_PATH:
    case EM_CURRENT_MODE_MODIFY_ACTION_KEY:
    case EM_CURRENT_MODE_MODIFY_ACTION_CONFIG:
    {
        m_pWait2DealAction = pAction;
        findMenuAndUpdateConfigFile(m_stUIConfig.stMenuMap,pAction->whatsThis());
        m_pDealTipLabel->setText(QObject::tr("将修改<font size='+2'><font color=red><b>%1</b></font></font>，请选择需要修改的选项；不确定请重新选择；取消请点击右上角\"x\"按钮").arg(pAction->objectName()));
        QMessageBox messageBox;
        messageBox.setIcon(QMessageBox::Question);
        messageBox.setWindowTitle("请选择");
        messageBox.setText("请选择");
        QPushButton menuBtn;
        menuBtn.setText("重命名动作");
        QPushButton actionBtn;
        actionBtn.setText("修改动作配置文件内容");
        QPushButton actionBtnfile;
        actionBtnfile.setText("修改动作配置文件名称");
        QPushButton actionBtnKey;
        actionBtnKey.setText("修改动作对应json解析的键值");
        QPushButton cancelBtn;
        cancelBtn.setText("取消");
        messageBox.addButton(&menuBtn, QMessageBox::AcceptRole);
        messageBox.addButton(&actionBtn,QMessageBox::AcceptRole);
        messageBox.addButton(&actionBtnfile,QMessageBox::AcceptRole);
        messageBox.addButton(&actionBtnKey,QMessageBox::AcceptRole);
        messageBox.addButton(&cancelBtn,QMessageBox::NoRole);
        int iRet = messageBox.exec();
        qDebug() << iRet ;
        if(0 == iRet)
        {
            setCurrentMode(EM_CURRENT_MODE_MODIFY_ACTION_NAME);
            onModifyActionName();
        }
        else if(1 == iRet)
        {
            setCurrentMode(EM_CURRENT_MODE_MODIFY_ACTION_CONFIG);
            onModifyActionConfig();
        }
        else if(2 == iRet)
        {
            setCurrentMode(EM_CURRENT_MODE_MODIFY_ACTION_CONFIG_PATH);
            onModifyActionConfigPath();
        }
        else if(3 == iRet)
        {
            setCurrentMode(EM_CURRENT_MODE_MODIFY_ACTION_KEY);
            onModifyActionKey();
        }
        return;
    }
    default:
        break;
    }

    if(EM_CURRENT_MODE_NORMAL == getCurrentMode())
    {
        CTextParse textParse;
        QString show;
        textParse.TextParse(getCurrentPath()+ "/Config/" + pAction->toolTip(),show);
        if(m_pSourceTextEdit->toPlainText().isEmpty())
        {
            QMessageBox::warning(this,"warning","请输入报文","返回");
            return ;
        }
        else
        {

            CTextParse textParse;
            QString strShow;
            m_pShowTextEdit->setText(QString("<font size='+2'><font color=red><b>%1</b></font></font>\n").arg(pAction->objectName()));
            if(textParse.parseDataByJSONFile(getCurrentPath()+ "/Config/" + pAction->toolTip(),pAction->statusTip(),m_pSourceTextEdit->toPlainText(),strShow))
                m_pShowTextEdit->append(strShow);
            else
            {
                QMessageBox::warning(this,"warning",QString("文件解析错误:")+textParse.getLastErrorStr(),"返回");
            }
        }
    }
}


void CUIConfig::onAboutToShow()
{
    m_pWait2DealMenu = qobject_cast<QMenu *>(QObject::sender());
    switch(getCurrentMode())
    {
    case EM_CURRENT_MODE_READY_DELETE:
    case EM_CURRENT_MODE_DELETE_MENU:
    case EM_CURRENT_MODE_DELETE_ACTION:
        setCurrentMode(EM_CURRENT_MODE_DELETE_MENU);
        onDeleteMenu();
        break;
    case EM_CURRENT_MODE_READY_MODIFY:
    case EM_CURRENT_MODE_MODIFY_MENU_NAME:
    case EM_CURRENT_MODE_MODIFY_ACTION_NAME:
    case EM_CURRENT_MODE_MODIFY_ACTION_CONFIG_PATH:
    case EM_CURRENT_MODE_MODIFY_ACTION_KEY:
    case EM_CURRENT_MODE_MODIFY_ACTION_CONFIG:
        setCurrentMode(EM_CURRENT_MODE_MODIFY_MENU_NAME);
        onModifyMenuName();
        break;
    case EM_CURRENT_MODE_NORMAL:
    case EM_CURRENT_MODE_READY_ADD:
    case EM_CURRENT_MODE_ADD_ACTION_KEY:
    case EM_CURRENT_MODE_ADD_ACTION_NAME:
    case EM_CURRENT_MODE_ADD_ACTION_CONFIG_FILE:
    case EM_CURRENT_MODE_ADD_MENU:
        m_pWait2DealMenu = NULL;
        return;
    default:
        break;
    }
    findMenuAndUpdateConfigFile(m_stUIConfig.stMenuMap,m_pWait2DealMenu->whatsThis());
//    qDebug() << "onAboutToShow" << m_pWait2DealMenu->toolTip() << m_pWait2DealMenu->statusTip() << m_pWait2DealMenu->whatsThis();
}

void CUIConfig::onCancelDealBtnClick()
{
    if(m_bUIConifgChange)
    {
//        int iRet = QMessageBox::question(this,"?",QString("已新增或者修改成功，尚未保存，是否放弃保存？"),"是","否");
//        qDebug() <<iRet;
        if(0 != QMessageBox::question(this,"?",QString("已新增或者修改成功，尚未保存，是否放弃保存？"),"是","否"))
            return;
    }

    m_pDealTipLabel->hide();
    m_pTempTextEdit->hide();
    m_pSaveBtn->hide();
    m_pComfirmlBtn->hide();
    m_pFileComBox->hide();
    m_pGetFileName->hide();

    qDebug() << "clear";
    if(EM_CURRENT_MODE_READY_ADD <= getCurrentMode() || EM_CURRENT_MODE_ADD_ACTION_KEY >= getCurrentMode())
    {
        m_pActionBar->clear();
        createMenu(m_pActionBar,NULL,m_stUIConfig.stMenuMap);
    }
    setCurrentMode(EM_CURRENT_MODE_NORMAL);
}
void CUIConfig::onComfirmBtnClick()
{
    qDebug() << "onComfirmBtnClick " <<getCurrentMode();

    switch(getCurrentMode())
    {
    case EM_CURRENT_MODE_DELETE_MENU:
        onConfirmDeleteMenu();
        break;
    case EM_CURRENT_MODE_DELETE_ACTION:
        onConfirmDeleteAction();
        break;
    case EM_CURRENT_MODE_MODIFY_MENU_NAME:
        onConfirmModifyMenuName();
        break;
    case EM_CURRENT_MODE_MODIFY_ACTION_NAME:
        onConfirmModifyActionName();
        break;
    case EM_CURRENT_MODE_MODIFY_ACTION_CONFIG_PATH:
        onConfirmModifyActionConfigPath();
        return;
    case EM_CURRENT_MODE_MODIFY_ACTION_KEY:
        onConfirmModifyActionKey();
        break;
    case EM_CURRENT_MODE_MODIFY_ACTION_CONFIG:
        onConfirmModifyActionConfig();
        break;
    case EM_CURRENT_MODE_ADD_MENU:
        onConfirmAddMenu();
        break;
    case EM_CURRENT_MODE_ADD_ACTION_NAME:
        onConfirmAddActionName();
        break;
    case EM_CURRENT_MODE_ADD_ACTION_KEY:
        onConfirmAddActionKey();
        break;
    case EM_CURRENT_MODE_ADD_ACTION_CONFIG_FILE:
        onConfirmAddActionConfigFile();
        break;
    default:
        break;
    }
}

void CUIConfig::onSaveBtnClick()
{
    qDebug() << "need to update UI or other config file";
    genUIConfig(m_stUIConfig);
    m_bUIConifgChange = false;
    m_pComfirmlBtn->hide();
    m_pSaveBtn->hide();
    m_pDealTipLabel->setText("配置文件重新生成成功；可继续继续操作；退出请点击右上角\"x\"按钮");
}
void CUIConfig::onGetFileBtnClick()
{
    QUrl file;
    if(m_pWait2DealAction)
        file = QFileDialog::getOpenFileUrl(this,"请选择配置文件",QUrl(QString("file:///%1/Config/%2").arg(getCurrentPath()).arg(m_pWait2DealAction->toolTip())),"*.json");
    else
        file = QFileDialog::getOpenFileUrl(this,"请选择配置文件",QUrl(QString("file:///%1/Config/").arg(getCurrentPath())),"*.json");

    qDebug() << file.toString();
    qDebug() << getCurrentPath();

    QString strTemp = "file:///" + getCurrentPath()+"/Config/";

    if(!file.toString().contains(strTemp))
    {
        QMessageBox::warning(this,"错误","配置文件必须在程序运行目录的Config文件下");
        return;
    }

    m_pTempTextEdit->setText(file.toString().replace(strTemp,""));

    qDebug() << file.toString();
}

void CUIConfig::onDropFile(QString strFile)
{
    QMessageBox messageBox;
    messageBox.setIcon(QMessageBox::Question);
    messageBox.setWindowTitle("请选择");
    messageBox.setText("请选择");
    QPushButton menuBtn;
    menuBtn.setText("二进制文件");
    QPushButton actionBtn;
    actionBtn.setText("源文件输出");
    QPushButton cancelBtn;
    cancelBtn.setText("取消");
    messageBox.addButton(&menuBtn, QMessageBox::AcceptRole);
    messageBox.addButton(&actionBtn,QMessageBox::AcceptRole);
    messageBox.addButton(&cancelBtn,QMessageBox::NoRole);

    int iRet = messageBox.exec();
    if(2 == iRet)
        return;
    parseFile2Hex(strFile,EM_PARSE_FILE_TYPE(iRet));
}

/*  createDirOrFile 处理生成文件或者文件夹
 *  [in]        文件夹路径或者文件路径 默认Config目录下部分
 *  [return]    true or false
 */
bool CUIConfig::createDirOrFile(QString strPath)
{
    QString path = strPath.replace("\\","/");
    if(strPath.contains("/"))
    {
        QString strTemp = getCurrentPath()+"/Config/"+strPath.left(strPath.lastIndexOf("/"));
        QFileInfo info(strTemp);
        if(!info.exists())
        {
            if(0 == QMessageBox::warning(this,"错误",QString("没有 Config/%1 目录,是否新建").arg(strPath.left(strPath.lastIndexOf("/"))),"是","否"))
            {
                QDir dir;
                if(!dir.mkpath(strTemp))
                    return false;
            }
            else
                return false;
        }
    }


    if(!strPath.contains(".json"))
        path.append(".json");

    QString strTemp = getCurrentPath()+"/Config/"+ path;
    QFileInfo info(strTemp);
    if(!info.exists())
    {
        if(0 == QMessageBox::warning(this,"错误",QString("没有 Config/%1 该文件,是否新建").arg(path),"是","否"))
        {
            QFile file(strTemp);
            if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                m_pDealTipLabel->setText(QString("创建%1文件失败").arg(path));
                return false;
            }
            file.write(QString(m_strDefaultJsonTip).arg(m_stMenuActionTemp.strKeyWord).toStdString().c_str());
            file.close();
        }
    }
    //是否需要打开
    if(0 == QMessageBox::question(this,"?",QString("是否需要打开配置文件进行编辑？"),"是","否"))
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile("file:///" + strTemp));
    }
    return true;
}
