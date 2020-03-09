#include "CUIConfig.h"
#include <QDebug>
#include <QDialog>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#pragma execution_character_set("utf-8")

//获取当前模式
EM_CURRENT_MODE CUIConfig::getCurrentMode()
{
    return m_emCurrentMode;
}
//设置当前模式
void CUIConfig::setCurrentMode(EM_CURRENT_MODE emCurrentMode)
{
    m_emCurrentMode = emCurrentMode;
}

//删除菜单
void CUIConfig::onDeleteMenu()
{
    if(NULL == m_pWait2DealMenu)
    {
        m_pDealTipLabel->setText("当前处理菜单为空，不进行处理，请重新选择");
        return;
    }
    m_pDealTipLabel->setText(QObject::tr("将移除<font size='+2'><font color=red><b>%1</b></font></font>，确认移除请点击确认按钮；不确定请重新选择；取消请点击右上角\"x\"按钮").arg(m_pWait2DealMenu->toolTip()));
    m_pComfirmlBtn->show();
}
//确认删除菜单
void CUIConfig::onConfirmDeleteMenu()
{
    if(NULL == m_pWait2DealMenu)
    {
        m_pDealTipLabel->setText("当前处理菜单为空，不进行处理，请重新选择");
        return;
    }

    QString strName = m_pWait2DealMenu->toolTip();

    if(QMessageBox::Cancel == QMessageBox::question(this,"请确认",QObject::tr("请确认是否移除<font size='+2'><font color=red><b>%1</b></font></font>").arg(strName),QMessageBox::Ok,QMessageBox::Cancel))
    {
        return;
    }
    m_bUIConifgChange = true;
    m_pMenuSourceMap->remove(m_strDealKey);
    m_pActionBar->clear();
    createMenu(m_pActionBar,NULL,m_stUIConfig.stMenuMap);
    m_pWait2DealMenu = nullptr;
    m_pDealTipLabel->setText(QObject::tr("移除<font size='+2'><font color=red><b>%1</b></font></font>成功，可继续选择需要删除的菜单或动作；如确认本次修改，请点击保存按钮；取消请点击右上角\"x\"按钮").arg(strName));
    m_pSaveBtn->show();
    m_pComfirmlBtn->hide();
}
//删除动作
void CUIConfig::onDeleteAction()
{
    if(NULL == m_pWait2DealAction)
    {
        m_pDealTipLabel->setText("当前处理动作为空，不进行处理，请重新选择");
        return;
    }
    m_pDealTipLabel->setText(QObject::tr("将移除<font size='+2'><font color=red><b>%1</b></font></font>，确认移除请点击确认按钮；不确定请重新选择；取消请点击右上角\"x\"按钮").arg(m_pWait2DealAction->objectName()));
     setCurrentMode(EM_CURRENT_MODE_DELETE_ACTION);
     m_pComfirmlBtn->show();
}
//确认删除动作
void CUIConfig::onConfirmDeleteAction()
{
    if(NULL == m_pWait2DealAction)
    {
        m_pDealTipLabel->setText("当前处理动作为空，不进行处理，请重新选择");
        return;
    }
    QString strName = m_pWait2DealAction->objectName();
    if(QMessageBox::Cancel == QMessageBox::question(this,"请确认",QObject::tr("请确认是否移除<font size='+2'><font color=red><b>%1</b></font></font>").arg(strName),QMessageBox::Ok,QMessageBox::Cancel))
    {
        return;
    }
    m_bUIConifgChange = true;
    m_pMenuSourceMap->remove(m_strDealKey);
    m_pActionBar->removeAction(m_pWait2DealAction);
    delete m_pWait2DealAction;
    m_pWait2DealAction = NULL;
    m_pDealTipLabel->setText(QObject::tr("移除<font size='+2'><font color=red><b>%1</b></font></font>成功，可继续选择需要删除的菜单或动作；如确认本次修改，请点击保存按钮；取消请点击右上角\"x\"按钮").arg(strName));
    m_pSaveBtn->show();
    m_pComfirmlBtn->hide();
}

//修改菜单
void CUIConfig::onModifyMenuName()
{
    if(NULL == m_pWait2DealMenu)
    {
        m_pDealTipLabel->setText("当前处理菜单为空，不进行处理，请重新选择");
        return;
    }
    m_pDealTipLabel->setText(QObject::tr("将修改<font size='+2'><font color=red><b>%1</b></font></font>的名称，请填写修改后的名称，点击确认进行修改").arg(m_pWait2DealMenu->toolTip()));
    m_pTempTextEdit->clear();
    m_pTempTextEdit->show();
    m_pTempTextEdit->setText(m_pWait2DealMenu->toolTip());
    m_pTempTextEdit->setFocus();
    m_pComfirmlBtn->show();
}
//确认修改菜单
void CUIConfig::onConfirmModifyMenuName()
{
    if(NULL == m_pWait2DealMenu)
    {
        m_pDealTipLabel->setText("当前处理菜单为空，不进行处理，请重新选择");
        return;
    }
    if(m_pTempTextEdit->text().isEmpty())
    {
        m_pDealTipLabel->setText("请填写修改后的名称");
        return;
    }
    m_bUIConifgChange = true;
    m_pDealTipLabel->setText("修改成功，可以点击保存按钮保存到配置文件");
    m_pWait2DealMenu->setTitle(m_pTempTextEdit->text());
    m_pWait2DealMenu->setToolTip(m_pTempTextEdit->text());
    m_pWait2DealMenu = NULL;
    m_pTempTextEdit->hide();
    m_pMenuSourceMap->find(m_strDealKey).value().strName = m_pTempTextEdit->text();
    m_pSaveBtn->show();
    m_pComfirmlBtn->hide();
}
//修改动作名称
void CUIConfig::onModifyActionName()
{
    if(NULL == m_pWait2DealAction)
    {
        m_pDealTipLabel->setText("当前处理菜单为空，不进行处理，请重新选择");
        return;
    }
    m_pDealTipLabel->setText(QObject::tr("将修改<font size='+2'><font color=red><b>%1</b></font></font>的名称，请填写修改后的名称，点击确认进行修改").arg(m_pWait2DealAction->objectName()));
    m_pTempTextEdit->clear();
    m_pTempTextEdit->show();
    m_pTempTextEdit->setText(m_pWait2DealAction->objectName());
    m_pTempTextEdit->setFocus();
}
//确认修改动作名称
void CUIConfig::onConfirmModifyActionName()
{
    if(NULL == m_pWait2DealAction)
    {
        m_pDealTipLabel->setText("当前处理菜单为空，不进行处理，请重新选择");
        return;
    }
    if(m_pTempTextEdit->text().isEmpty())
    {
        m_pDealTipLabel->setText("请填写修改后的名称");
        return;
    }
    m_bUIConifgChange = true;
    m_pDealTipLabel->setText("修改成功，可以点击保存按钮保存到配置文件");
    m_pWait2DealAction->setText(m_pTempTextEdit->text());
    m_pWait2DealAction->setObjectName(m_pTempTextEdit->text());
    m_pWait2DealAction = NULL;
    m_pTempTextEdit->hide();
    m_pMenuSourceMap->find(m_strDealKey).value().strName = m_pTempTextEdit->text();
    m_pSaveBtn->show();
    m_pComfirmlBtn->hide();
}
//修改动作配置
void CUIConfig::onModifyActionConfig()
{
    if(NULL == m_pWait2DealAction)
    {
        m_pDealTipLabel->setText("当前处理菜单为空，不进行处理，请重新选择");
        return;
    }
    m_pTempTextEdit->hide();
    m_pDealTipLabel->setText("将为你打开配置文件，请根据规矩进行修改，点击确认按钮确认修改完毕");

//    qDebug() << m_pWait2DealAction->whatsThis() << m_pWait2DealAction->toolTip() << m_pWait2DealAction->statusTip();
//    qDebug() << QCoreApplication::applicationDirPath();

    m_pComfirmlBtn->show();
    QString file = QString("file:///%1/Config/%2").arg(QCoreApplication::applicationDirPath()).arg(m_pWait2DealAction->toolTip());
    if(!QDesktopServices::openUrl(QUrl::fromLocalFile((file))))
        m_pDealTipLabel->setText(QString("打开%1失败").arg(file));

    if(m_bUIConifgChange)
    {
        m_pSaveBtn->show();
    }
}
//确认修改动作配置
void CUIConfig::onConfirmModifyActionConfig()
{
    m_pDealTipLabel->hide();
    m_pTempTextEdit->hide();
    m_pSaveBtn->hide();
    m_pComfirmlBtn->hide();
    m_pFileComBox->hide();
    setCurrentMode(EM_CURRENT_MODE_NORMAL);
    m_pWait2DealAction = NULL;
}


//修改动作配置路径
void CUIConfig::onModifyActionConfigPath()
{
    if(NULL == m_pWait2DealAction)
    {
        m_pDealTipLabel->setText("当前处理动作为空，不进行处理，请重新选择");
        return;
    }
    m_pDealTipLabel->setText(QObject::tr("当前配置文件为为<font size='+2'><font color=red><b>%1</b></font></font>，请输入或者打开需要修改的文件").arg(m_pWait2DealAction->toolTip()));
    m_pTempTextEdit->show();
    m_pTempTextEdit->setFocus();
    m_pTempTextEdit->setText(m_pWait2DealAction->toolTip());
    m_pGetFileName->show();
}

//确认修改动作配置路径
void CUIConfig::onConfirmModifyActionConfigPath()
{
    if(NULL == m_pWait2DealAction)
    {
        m_pDealTipLabel->setText("当前处动作单为空，不进行处理，请重新选择");
        return;
    }
    if(m_pTempTextEdit->text().isEmpty())
    {
        m_pDealTipLabel->setText("请填写修改后的名称");
        return;
    }

    QString strTemp = getCurrentPath()+"/Config/"+m_pTempTextEdit->text();
    qDebug() << strTemp;
    QFileInfo info(strTemp);
    if(!info.isFile())
    {
        if(0 == QMessageBox::warning(this,"错误",QString("没有 Config/%1 该文件,是否新建").arg(m_pTempTextEdit->text()),"是","否"))
        {
            QFile file(strTemp);
            if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                m_pDealTipLabel->setText(QString("创建%1文件失败").arg(strTemp));
                return;
            }
            file.write(QString(m_strDefaultJsonTip.arg(m_pWait2DealAction->toolTip())).toStdString().c_str());
            file.close();

            //是否需要打开
            if(0 == QMessageBox::question(this,"?",QString("文件创建成功，是否需要打开新建的文件进行编辑？"),"是","否"))
            {
                QDesktopServices::openUrl(QUrl::fromLocalFile("file:///" + strTemp));
            }
        }
        else
        {
            m_pDealTipLabel->setText("请重新填写修改后的名称");
            return;
        }
    }

    m_bUIConifgChange = true;
    m_pDealTipLabel->setText("修改成功，可以点击保存按钮保存到配置文件");
    m_pWait2DealAction->setToolTip(m_pTempTextEdit->text());
    m_pWait2DealAction = NULL;
    m_pTempTextEdit->hide();
    m_pGetFileName->hide();
    m_pMenuSourceMap->find(m_strDealKey).value().strJsonPath = m_pTempTextEdit->text();
    m_pSaveBtn->show();
    m_pComfirmlBtn->hide();
}

//修改动作键值
void CUIConfig::onModifyActionKey()
{
    if(NULL == m_pWait2DealAction)
    {
        m_pDealTipLabel->setText("当前处理动作为空，不进行处理，请重新选择");
        return;
    }
    m_pDealTipLabel->setText(QObject::tr("当前键值为<font size='+2'><font color=red><b>%1</b></font></font>，请输入修改后的键值，请输入中文或者英文，不要输入特殊字符").arg(m_pWait2DealAction->statusTip()));
    m_pTempTextEdit->show();
    m_pTempTextEdit->setFocus();
    m_pTempTextEdit->setText(m_pWait2DealAction->statusTip());
}

//确认修改动作键值
void CUIConfig::onConfirmModifyActionKey()
{
    if(NULL == m_pWait2DealAction)
    {
        m_pDealTipLabel->setText("当前处理菜单为空，不进行处理，请重新选择");
        return;
    }
    if(m_pTempTextEdit->text().isEmpty())
    {
        m_pDealTipLabel->setText("请填写修改后的查找键值");
        return;
    }
    m_bUIConifgChange = true;
    m_pDealTipLabel->setText("修改成功，可以点击保存按钮保存到配置文件");
    m_pWait2DealAction->setStatusTip(m_pTempTextEdit->text());
    m_pWait2DealAction = NULL;
    m_pTempTextEdit->hide();
    m_pMenuSourceMap->find(m_strDealKey).value().strKeyWord = m_pTempTextEdit->text();
    m_pSaveBtn->show();
    m_pComfirmlBtn->hide();
    if(0 == QMessageBox::question(this,"question","你已修改动作的键值，是否打开json配置文件修改对应的键值","是","否"))
        onModifyActionConfig();
}

//新增菜单
void CUIConfig::onAddMenu()
{
    if(NULL == m_pWait2DealAction)
    {
        m_pDealTipLabel->setText("当前处理菜单为空，不进行处理，请重新选择");
        return;
    }
    qDebug() << "onAddMenu" << m_pWait2DealAction->toolTip() << m_pWait2DealAction->whatsThis();
    m_pDealTipLabel->setText("请输入菜单的名称，并点击确认按钮");
    m_pTempTextEdit->clear();
    m_pTempTextEdit->show();
    m_pComfirmlBtn->show();
    m_pTempTextEdit->setFocus();
    setCurrentMode(EM_CURRENT_MODE_ADD_MENU);
}
//确认新增菜单
void CUIConfig::onConfirmAddMenu()
{
    if(NULL == m_pWait2DealAction)
    {
        m_pDealTipLabel->setText("当前处理菜单为空，不进行处理，请重新选择");
        return;
    }
    if(m_pTempTextEdit->text().isEmpty())
    {
        m_pDealTipLabel->setText("请填写修改后的名称");
        return;
    }
    QString strKey = QString("%1").arg(m_pWait2DealAction->whatsThis().toInt() + 1,m_pWait2DealAction->whatsThis().length(),10,QChar('0'));

    qDebug() << "onConfirmAddMenu"  << m_pWait2DealAction->whatsThis().length() << strKey;

    ST_MENU_OR_ACTION stMenuOrAction;
    stMenuOrAction.emType = EM_MENU_TYPE_MENU;
    stMenuOrAction.strName = m_pTempTextEdit->text();
    qDebug() << "m_pMenuSourceMap->size()" << m_pMenuSourceMap->size();
    m_pMenuSourceMap->insert(strKey,stMenuOrAction);

    m_pActionBar->clear();
    createMenu(m_pActionBar,NULL,m_stUIConfig.stMenuMap,true);
    m_pDealTipLabel->setText("添加成功，可以继续选择添加位置，点击\"+\"按钮");
    m_bUIConifgChange = true;
    m_pTempTextEdit->hide();
    m_pSaveBtn->show();
    m_pComfirmlBtn->hide();
    m_pWait2DealAction = NULL;
}
//添加动作名称
void CUIConfig::onAddActionName()
{
    if(NULL == m_pWait2DealAction)
    {
        m_pDealTipLabel->setText("当前处理为空，不进行处理，请重新选择");
        return;
    }
    m_pTempTextEdit->clear();
    m_pTempTextEdit->show();
    m_pDealTipLabel->setText("请输入动作的名称，作为界面显示名称");
    m_stMenuActionTemp.clear();
    m_stMenuActionTemp.emType = EM_MENU_TYPE_ACTION;
    m_pComfirmlBtn->show();
    setCurrentMode(EM_CURRENT_MODE_ADD_ACTION_NAME);
}

//确认添加动作名称
void CUIConfig::onConfirmAddActionName()
{
    if(m_pTempTextEdit->text().isEmpty())
    {
        m_pDealTipLabel->setText("名称不能为空，请重新输入动作的名称");
        m_pComfirmlBtn->show();
        return;
    }
    m_stMenuActionTemp.strName = m_pTempTextEdit->text();
    onAddActionKey();
}

//添加动作键值
void CUIConfig::onAddActionKey()
{
    if(NULL == m_pWait2DealAction)
    {
        m_pDealTipLabel->setText("当前处理为空，不进行处理，请重新选择");
        return;
    }
    m_pDealTipLabel->setText("请输入动作键值，对应格式配置文件里一组解析格式的key值，请输入中文或者英文，不要输入特殊字符");
    m_pTempTextEdit->clear();
    m_pTempTextEdit->show();
    m_pComfirmlBtn->show();
    setCurrentMode(EM_CURRENT_MODE_ADD_ACTION_KEY);
}

//确认添加动作键值
void CUIConfig::onConfirmAddActionKey()
{
    if(m_pTempTextEdit->text().isEmpty())
    {
        m_pDealTipLabel->setText("名称不能为空，请重新输入动作键值");
        return;
    }
    m_stMenuActionTemp.strKeyWord = m_pTempTextEdit->text();
    onAddActionConfigFile();
}

//添加动作配置文件
void CUIConfig::onAddActionConfigFile()
{
    if(NULL == m_pWait2DealAction)
    {
        m_pDealTipLabel->setText("当前处理为空，不进行处理，请重新选择");
        return;
    }
    m_pDealTipLabel->setText("请输入配置文件路径或打开该动作的配置文件,也可以直接输入文件名，文件默认创建在Config下");
    m_pTempTextEdit->clear();
    m_pTempTextEdit->show();
    m_pComfirmlBtn->show();
    m_pGetFileName->show();
    setCurrentMode(EM_CURRENT_MODE_ADD_ACTION_CONFIG_FILE);
}

//确认添加动作配置文件
void CUIConfig::onConfirmAddActionConfigFile()
{
    if(m_pTempTextEdit->text().isEmpty())
    {
        m_pDealTipLabel->setText("请输入或打开该动作的配置文件");
        return;
    }

    if(!createDirOrFile(m_pTempTextEdit->text()))
        return;

    QString strKey = QString("%1").arg(m_pWait2DealAction->whatsThis().toInt() + 1,m_pWait2DealAction->whatsThis().length(),10,QChar('0'));
    m_stMenuActionTemp.strJsonPath = m_pTempTextEdit->text();
    if(!m_stMenuActionTemp.strJsonPath.contains(".json"))
        m_stMenuActionTemp.strJsonPath.append(".json");
    qDebug() << __FUNCTION__ << "m_pMenuSourceMap->size()" << m_pMenuSourceMap->size();
    qDebug() << __FUNCTION__ << "strKey" << strKey;
    qDebug() << __FUNCTION__ << "m_stUIConfig.stMenuMap" << m_stUIConfig.stMenuMap.size();
    m_pMenuSourceMap->insert(strKey,m_stMenuActionTemp);
    qDebug() << __FUNCTION__ << "m_stUIConfig.stMenuMap" << m_stUIConfig.stMenuMap.size();
    m_pActionBar->clear();
    createMenu(m_pActionBar,NULL,m_stUIConfig.stMenuMap,true);
    m_pDealTipLabel->setText("添加成功");

    m_bUIConifgChange = true;
    m_pDealTipLabel->setText("修改成功，可以点击保存按钮保存到配置文件");
    m_pTempTextEdit->hide();
    m_pGetFileName->hide();
    m_pSaveBtn->show();
    m_pSaveBtn->setFocus();
    m_pComfirmlBtn->hide();
    m_pWait2DealAction = NULL;
}

/*
 *    parseFile2Hex 读取待解析文件
 */
void CUIConfig::parseFile2Hex(const QString &strfileName,EM_PARSE_FILE_TYPE emFileType)
{
    QFile file(strfileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,"错误",QString("%1 文件打开失败").arg(strfileName),"确定");
        return;
    }
    qDebug() << "file size = " << file.size();
    QByteArray fileByteArray =  file.readAll();

    unsigned char *pData = (unsigned char *)fileByteArray.data();

    qDebug() << "fileByteArray.size()" << fileByteArray.size();
    if(EM_PARSE_FILE_TYPE_BINARY ==  emFileType)
    {
        QString str;
        for(int i = 0; i < fileByteArray.size();i++)
        {
            str.append(QString("%1").arg(pData[i],2,16,QChar('0')));
        }
        m_pSourceTextEdit->setText(str);
    }
    else if(EM_PARSE_FILE_TYPE_HEX ==  emFileType)
    {
        m_pSourceTextEdit->setText(fileByteArray);
    }

    file.close();
}
