#include "CTextParse.h"
#include <QFile>
#include <QString>
#include <QDebug>
#include <QtEndian>
#pragma execution_character_set("utf-8")
CTextParse::CTextParse()
{

}

EM_TEXT_PARSE_RESULT CTextParse::TextParse(QString strJSONCfg,  const QString &strSourceText)
{
    QMap<QString ,ST_TEXT_PARSE_FORMAT> stMap;
    EM_TEXT_PARSE_RESULT emRet = jsonToStruct(strJSONCfg,stMap);
    if(EM_TEXT_PARSE_RESULT_OK == emRet)
        genJsonConfigFile(stMap,"test.json");
    return emRet;
}

/* parseDataByJSONFile 根据json文件直接解析报文，并组成输出
 * [in] strJSONCfg   json文件
 * [in] strKey       对应解析方法的key
 * [in] pszSourceText  待解析数据格式16进制空格或者不带空格  31 32 33
 * [out] strOutPrintData  根据json格式解析数据组成输出
 */
bool CTextParse::parseDataByJSONFile(const QString &strJSONCfg,const QString &strKey,const QString &strSourceText, QString &pstrOutPrintData)
{
    int iDealIndex = 0;
    bool bOk;

    QFile file(strJSONCfg);
    if(!file.open(QFile::ReadOnly | QIODevice::Text))
    {
        m_strErrorStr = QString("open %1 failed").arg(strJSONCfg);
        return false;
    }

    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(),&jsonError);
    file.close();
    if(jsonError.error != QJsonParseError::NoError)
    {
        m_strErrorStr = QString("%1文件格式不对,error:%2").arg(strJSONCfg).arg(jsonError.errorString());
        return false;
    }

    if(!jsonDoc.isObject())
    {
        m_strErrorStr = QString("%1文件内部格式不对").arg(strJSONCfg);
        return false;
    }
    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject::iterator it = jsonObj.find(strKey);
    EM_TEXT_FORMAT emTextFormat;
    int iDataLen = 0 , iLeftDataLen = 0;

    QString strBuf(strSourceText);
    //去掉换行
    strBuf.replace("\n\r","").replace("\n","").append(" ");

    //去掉前面空格
    while(!strBuf.mid(0,1).compare(" "))
        strBuf.remove(0,1);

    if(!strBuf.mid(2,1).compare(" "))
    {
        qDebug() << QString("有空格");
        emTextFormat = EM_TEXT_FORMAT_HEX_HEX;
    }
    else
    {
        qDebug() << QString("没空格");
        emTextFormat = EM_TEXT_FORMAT_HEXHEX;
    }
    iLeftDataLen = iDataLen = strBuf.length();
    if(0 == iDataLen)
    {
        m_strErrorStr = QString("数据不对");
        return false;
    }

    if(it != jsonObj.end())
    {
        QJsonArray array(it->toArray());
        for(int i = 0 ; i < array.size();i++)
        {
//            qDebug() << "i" << i << "array[i]" << array[i].type();
            if(QJsonValue::Type::Object != array[i].type())
            {
                m_strErrorStr = QString("%1文件内部格式不对").arg(strJSONCfg);
                return false;
            }
            QJsonObject jsonobjcet(array[i].toObject());
            if(!oneBlockDataParse(jsonobjcet,strBuf.toStdString().c_str(),iDealIndex,iLeftDataLen,emTextFormat,pstrOutPrintData))
                return false;
            if(iLeftDataLen <= 0)
                break;
        }
    }
    else
    {
        m_strErrorStr = QString("json文件中没有该%1值").arg(strKey);
        return false;
    }
    if(iLeftDataLen > 1)
        pstrOutPrintData.append(QString("未处理 : ")+(strBuf.toStdString().c_str() + (iDataLen - iLeftDataLen)));
    return true;
}

/* parseDataByMap 根据Map解析报文，并组成输出
 * [in] stTextParseFormatMap   格式map
 * [in] strKey       对应解析方法的key
 * [in] pszSourceText  待解析数据格式16进制空格或者不带空格  31 32 33
 * [out] strOutPrintData  根据json格式解析数据组成输出
 */
bool CTextParse::parseDataByMap(const QMap<QString, ST_TEXT_PARSE_FORMAT> &stTextParseFormatMap,const QString &strKey, const char *pszSourceText, int iSourceTextLen, QString &pstrOutPrintData)
{
    int iDealIndex = 0;
    bool bOk;




    return bOk;
}


EM_TEXT_PARSE_RESULT CTextParse::jsonToStruct(QString strJSONCfg,QMap<QString ,ST_TEXT_PARSE_FORMAT> &stTextParseFormat)
{
    QFile file(strJSONCfg);
    if(!file.open(QFile::ReadOnly | QIODevice::Text))
    {
        m_strErrorStr = QString("open %1 failed").arg(strJSONCfg);
        return EM_TEXT_PARSE_RESULT_JSON_FILE_OPEN_FAILED;
    }

    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(),&jsonError);
    file.close();
    if(jsonError.error != QJsonParseError::NoError)
    {
        m_strErrorStr = QString("%1文件格式不对,error:%2").arg(strJSONCfg).arg(jsonError.errorString());
        return EM_TEXT_PARSE_RESULT_JSON_FILE_ERROR;
    }

    if(!jsonDoc.isObject())
    {
        m_strErrorStr = QString("%1文件内部格式不对").arg(strJSONCfg);
        return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
    }

    //第一个 description
    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject::iterator it = jsonObj.begin();

    ST_TEXT_PARSE_FORMAT stTextFormat;
    stTextFormat.stProtocolDetailList.clear();
    stTextFormat.iLength = 0;

    EM_TEXT_PARSE_RESULT emParseResult = EM_TEXT_PARSE_RESULT_OK;
    //对应解析格式集
    for(;it != jsonObj.end();it++)
    {
        stTextFormat.stProtocolDetailList.clear();
        qDebug() << "it->type()" <<it->type();
        //context key特殊处理
        if(QJsonValue::Type::String ==  it->type() && it.key().contains("description"))
        {
            ST_PROTOCOL_DETAIL stProtocalDetail;
            stProtocalDetail.strPrintName = it.value().toString();
            stTextFormat.stProtocolDetailList.push_back(stProtocalDetail);
            stTextParseFormat.insert(it.key(),stTextFormat);
            continue;
        }
        else if(QJsonValue::Type::Array !=  it->type() && !it.key().contains("description"))
        {
            m_strErrorStr = QString("%1文件内部格式不对，只允许在description里备注").arg(strJSONCfg);
            return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
        }

        QJsonArray array(it->toArray());

        for(int i = 0 ; i < array.size();i++)
        {
            qDebug() << "i" << i << "array[i].type" << array[i].type();
            if(QJsonValue::Type::Object != array[i].type())
            {
                m_strErrorStr = QString("%1文件内部格式不对").arg(strJSONCfg);
                return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
            }
            QJsonObject jsonobjcet(array[i].toObject());
            emParseResult = oneBlockFormatParse(jsonobjcet,stTextFormat.stProtocolDetailList,stTextFormat.iLength);
            if(EM_TEXT_PARSE_RESULT_OK != emParseResult)
                return emParseResult;
        }
        stTextParseFormat.insert(it.key(),stTextFormat);
    }

    return emParseResult;
}
/*
 *  oneBlockFormatParse 一块解析数据解析
 *  [in] jsonObject  json对象
 *  [out]   protolDetailList    解析数据数据结构列表
 *  [out]   iTotalLength        解析数据总长度
 */
EM_TEXT_PARSE_RESULT CTextParse::oneBlockFormatParse(QJsonObject &jsonObject,QList<ST_PROTOCOL_DETAIL> &protolDetailList,int &iTotalLength)
{
    ST_PROTOCOL_DETAIL stProtocalDetail;
    stProtocalDetail.iIndex = 0;
    stProtocalDetail.iIsBigEndian = -1;
    stProtocalDetail.stLoopOrRuleList.clear();

    //对应字段内部解析类型（必须）
    if(jsonObject.contains("type"))
    {
        QJsonValue value(jsonObject.take("type"));
        if(QJsonValue::Type::Double != value.type())
        {
            m_strErrorStr = QString("type类型必须是整型");
            qDebug() << m_strErrorStr;
            return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
        }
        stProtocalDetail.emTextType = (EM_TEXT_TYPE)value.toInt();
        qDebug() << "emTextType" << stProtocalDetail.emTextType;
    }
    else
    {
        m_strErrorStr = QString("缺少type");
        qDebug() << m_strErrorStr;
        return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
    }

    //对应字段名称解释
    if(jsonObject.contains("name"))
    {
        QJsonValue value(jsonObject.take("name"));
        if(QJsonValue::Type::String != value.type())
        {
            m_strErrorStr = QString("name类型必须是字符串");
            qDebug() << m_strErrorStr;
            return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
        }
        stProtocalDetail.strPrintName = value.toString();
        qDebug() << "strPrintName" << stProtocalDetail.strPrintName;
    }
    else if(stProtocalDetail.emTextType != EM_TEXT_TYPE_UNCERTAIN_LEN)
    {
        m_strErrorStr = QString("缺少name");
        qDebug() << m_strErrorStr;
        return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
    }

    //对应字段长度
    if(jsonObject.contains("length"))
    {
        QJsonValue value(jsonObject.take("length"));
        if(QJsonValue::Type::Double != value.type())
        {
            m_strErrorStr = QString("length类型必须是整型");
            qDebug() << m_strErrorStr;
            return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
        }
        stProtocalDetail.iLen = value.toInt();
        if( stProtocalDetail.iLen > 0)
            iTotalLength += stProtocalDetail.iLen;
        qDebug() << "iLen" << stProtocalDetail.iLen;
    }
    else
    {
        m_strErrorStr = QString("缺少length");
        qDebug() << m_strErrorStr;
        return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
    }

    //备注字段
    if(jsonObject.contains("remask"))
    {
        QJsonValue value(jsonObject.take("remask"));
        if(QJsonValue::Type::String != value.type())
        {
            m_strErrorStr = QString("remask类型必须是字符串");
            qDebug() << m_strErrorStr;
            return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
        }
        stProtocalDetail.strRemark = value.toString();
        qDebug() << "strRemark" << stProtocalDetail.strRemark;
    }

    qDebug() << "stProtocalDetail.emTextType" << stProtocalDetail.emTextType;


    if(jsonObject.contains("index"))
    {
        QJsonValue value(jsonObject.take("index"));
        if(QJsonValue::Type::Double != value.type())
        {
            m_strErrorStr = QString("index 为整数");
            qDebug() << m_strErrorStr;
            return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
        }
        stProtocalDetail.iIndex = value.toInt();
        qDebug() << "iIndex" << stProtocalDetail.iIndex;
    }

    if(jsonObject.contains("bigEndian"))
    {
        QJsonValue value(jsonObject.take("bigEndian"));
        if(QJsonValue::Type::Bool != value.type())
        {
            m_strErrorStr = QString("bigEndian bool类型");
            qDebug() << m_strErrorStr;
            return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
        }
        stProtocalDetail.iIsBigEndian = value.toBool();
        qDebug() << "iIsBigEndian" << stProtocalDetail.iIsBigEndian;
    }

    if(jsonObject.contains("reverse"))
    {
        QJsonValue value(jsonObject.take("reverse"));
        if(QJsonValue::Type::Bool != value.type())
        {
            m_strErrorStr = QString("reverse bool类型");
            qDebug() << m_strErrorStr;
            return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
        }
        stProtocalDetail.bReverse = value.toBool();
        qDebug() << "reverse" << stProtocalDetail.bReverse;
    }

    if(jsonObject.contains("lengthType"))
    {
        QJsonValue value(jsonObject.take("lengthType"));
        if(QJsonValue::Type::Double != value.type())
        {
            m_strErrorStr = QString("lengthType 为整数");
            qDebug() << m_strErrorStr;
            return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
        }
        stProtocalDetail.emLenType = (EM_LENGTH_TYPE)value.toInt();
        qDebug() << "emLenType" << stProtocalDetail.emLenType;
    }

    if(jsonObject.contains("loopNum"))
    {
        QJsonValue value(jsonObject.take("loopNum"));
        if(QJsonValue::Type::Double != value.type())
        {
            m_strErrorStr = QString("loopNum 为整数");
            qDebug() << m_strErrorStr;
            return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
        }
        stProtocalDetail.iLoopNum = value.toInt();
        qDebug() << "loopNum" << stProtocalDetail.iLoopNum;
    }

    //4 循环体类型 需要有 loop
    if(stProtocalDetail.emTextType == EM_TEXT_TYPE_LOOP_NUM
           || EM_TEXT_TYPE_UNCERTAIN_LEN == stProtocalDetail.emTextType
            ||  EM_TEXT_TYPE_BIT_PROCESING == stProtocalDetail.emTextType)
    {
        qDebug() << "array" ;
        if(jsonObject.contains("array"))
        {
            QJsonValue value(jsonObject.take("array"));
            if(QJsonValue::Type::Array != value.type())
            {
                m_strErrorStr = QString("array应该是数组类型");
                qDebug() << m_strErrorStr;
                return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
            }

            QJsonArray array(value.toArray());
            for(int j = 0; j < array.size(); j++)
            {
                QJsonObject Obj(array.at(j).toObject());
                if(EM_TEXT_PARSE_RESULT_OK != oneBlockFormatParse(Obj,stProtocalDetail.stLoopOrRuleList,iTotalLength))
                    return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
            }
        }
        else
        {
            m_strErrorStr = QString("缺少array数组");
            qDebug() << m_strErrorStr;
            return EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR;
        }
    }
    protolDetailList.push_back(stProtocalDetail);
    if(0 == jsonObject.size())
        return EM_TEXT_PARSE_RESULT_OK;
    return EM_TEXT_PARSE_RESULT_OK;
}

/*  oneBlockDataParse     当个格式报文解析
 *  [in] jsonObject       json对象
 *  [in] pszSourceText    待解析数据
 *  [in/out] iDealIndex   处理到的位置
 *  [in/out] iLeftTextLen 待处理数据长度
 *  [out]  strOutPrintData  根据json格式解析数据组成输出
 *  [out] bIsDealBit  是否按位处理
 */
bool CTextParse::oneBlockDataParse(QJsonObject &jsonObject, const char *pszSourceText,int&iDealIndex, int &iLeftTextLen, EM_TEXT_FORMAT emTextFormat, QString &strOutPrintData,bool bIsDealBit)
{
    //长度
    int iLen = 0;
    //数据类型
    EM_TEXT_TYPE emTextType;
    //备注
    QString strRemark = "";
    //循环长度
    long long lLoopNum = -1;
    //名称
    QString strName = "";
    //长度转换类型
    EM_LENGTH_TYPE emLengthType = EM_LENGTH_TYPE_INTEGER;
    //大小端
    bool bIsBigEndian = false;
    //相对位置
    int iIndex = NO_INDEX;
    //数据
    QJsonValue *pArray = NULL;
    //数组翻转解析
    bool bReverse = false;

    //对应字段内部解析类型（必须）
    if(jsonObject.contains("type"))
    {
        QJsonValue value(jsonObject.take("type"));
        if(QJsonValue::Type::Double != value.type())
        {
            m_strErrorStr = QString("type类型必须是整型");
            qDebug() << m_strErrorStr;
            return false;
        }
        emTextType = (EM_TEXT_TYPE)value.toInt();
    }
    else
        emTextType = EM_TEXT_TYPE_RAW;

    //对应字段名称解释(部分类型必须)
    if(jsonObject.contains("name"))
    {
        QJsonValue value(jsonObject.take("name"));
        if(QJsonValue::Type::String != value.type())
        {
            m_strErrorStr = QString("name类型必须是字符串");
            qDebug() << m_strErrorStr;
            return false;
        }
        strName = value.toString();
        if(strName.length())
            strOutPrintData.append(strName + " : ");
        else
            strOutPrintData.append("\n");
    }
    else if(emTextType != EM_TEXT_TYPE_UNCERTAIN_LEN)
    {
        m_strErrorStr = QString("缺少name");
        qDebug() << m_strErrorStr;
        return false;
    }

    //备注字段
    if(jsonObject.contains("remark"))
    {
        QJsonValue value(jsonObject.take("remark"));
        qDebug() << value.type();
        if(QJsonValue::Type::String != value.type())
        {
            m_strErrorStr = QString("remark类型必须是字符串");
            qDebug() << m_strErrorStr;
            return false;
        }
        strRemark = value.toString();
    }

    //循环体长
    if(jsonObject.contains("loopNum"))
    {
        QJsonValue value(jsonObject.take("loopNum"));
        if(QJsonValue::Type::Double != value.type())
        {
            m_strErrorStr = QString("loopNum 为整数");
            qDebug() << m_strErrorStr;
            return false;
        }
        lLoopNum = value.toInt();
        qDebug() << "loopNum" << lLoopNum;
    }

    //相对位置
    if(jsonObject.contains("index"))
    {
        QJsonValue value(jsonObject.take("index"));
        if(QJsonValue::Type::Double != value.type())
        {
            m_strErrorStr = QString("index 为整数");
            qDebug() << m_strErrorStr;
            return false;
        }
        iIndex = value.toInt();
        qDebug() << "index" << iIndex;
    }

    //对应字段长度
    if(jsonObject.contains("length"))
    {
        QJsonValue value(jsonObject.take("length"));
        if(QJsonValue::Type::Double != value.type())
        {
            m_strErrorStr = QString("length类型必须是整型");
            qDebug() << m_strErrorStr;
            return false;
        }
        iLen = value.toInt();

        //如果配置长度大于待解析的数据长度，则都放到里面
        if(iLen*emTextFormat > iLeftTextLen)
        {
            if(strRemark.isEmpty())
                strOutPrintData.append(QString(pszSourceText+iDealIndex) + "\n");
            else
                strOutPrintData.append(QString(pszSourceText+iDealIndex) + " (" + strRemark + ")\n");
            iLeftTextLen -= (iLen*emTextFormat);
            return true;
        }
        else if(0 >= iLen)
        {
            strOutPrintData.append(" (" + strRemark + ")\n");
            return true;
        }
    }
    else
    {
        if(-1 == lLoopNum && emTextType != EM_TEXT_TYPE_LOOP_NUM)
        {
            m_strErrorStr = QString("缺少length");
            qDebug() << m_strErrorStr;
            return false;
        }
    }

    //长度转换类型
    if(jsonObject.contains("lengthType"))
    {
        QJsonValue value(jsonObject.take("lengthType"));
        if(QJsonValue::Type::Double == value.type())
            emLengthType = (EM_LENGTH_TYPE)value.toInt();
    }

    //大小端
    if(jsonObject.contains("bigEndian"))
    {
        QJsonValue value(jsonObject.take("bigEndian"));
        if(QJsonValue::Type::Bool == value.type())
            bIsBigEndian = value.toBool();
    }
    //翻转
    if(jsonObject.contains("reverse"))
    {
        QJsonValue value(jsonObject.take("reverse"));
        if(QJsonValue::Type::Bool == value.type())
            bReverse = value.toBool();
    }

    if(bIsDealBit)
    {
        qDebug() << "按位处理" << iLen;
        unsigned long long llBit = text2Integer(iLen,emTextFormat,EM_LENGTH_TYPE_BIT,pszSourceText + iDealIndex ,!bIsBigEndian);
        strOutPrintData.append(QString("%1\n").arg(llBit));
        iDealIndex += iLen *emTextFormat;
        iLeftTextLen -= iLen *emTextFormat;
        qDebug() << "该位数据" << llBit;
        return true;
    }

    //数组
    if(jsonObject.contains("array"))
    {
//        QJsonValue value(jsonObject.take("array"));
        pArray = new QJsonValue(jsonObject.take("array"));
        if(QJsonValue::Type::Array != pArray->type())
        {
            m_strErrorStr = QString("array应该是数组类型");
            qDebug() << m_strErrorStr;
            delete pArray;
            pArray = NULL;
            return false;
        }
    }

    if((emTextType == EM_TEXT_TYPE_INT || emTextType == EM_TEXT_TYPE_LOOP_NUM || emTextType == EM_TEXT_TYPE_UNCERTAIN_LEN)
            && iLen > 8 && emLengthType == EM_LENGTH_TYPE_INTEGER)
    {
        m_strErrorStr = QString("整型转换超过8字节，暂不能处理");
        qDebug() << m_strErrorStr;
        return false;
    }


    //1 原始数据
    if(EM_TEXT_TYPE_RAW == emTextType)
    {
        iLeftTextLen -= iLen*emTextFormat;
        if(strRemark.length())
            strOutPrintData.append(QString(pszSourceText+iDealIndex).left(iLen * emTextFormat) + " (" + strRemark + ")\n");
        else
            strOutPrintData.append(QString(pszSourceText+iDealIndex).left(iLen * emTextFormat) +  "\n");
        iDealIndex += iLen * emTextFormat;
    }
    //2 可转成字符串显示
    else if(EM_TEXT_TYPE_ASIIC== emTextType)
    {
        iLeftTextLen -= iLen*emTextFormat;
        char *pTemp = new char[iLen+1];
        pTemp[iLen] = 0;
        for(int i = 0; i < iLen ; i++)
        {
            pTemp[i] = QString(pszSourceText+ iDealIndex).left(2).toInt(NULL,16);
            iDealIndex += emTextFormat;
        }

        if(strRemark.length())
            strOutPrintData.append("{ " + QString(pTemp) + " } (" + strRemark + " )\n");
        else
            strOutPrintData.append("{ " + QString(pTemp) + " }\n");
        delete [] pTemp;
    }
    //3
    else if(EM_TEXT_TYPE_INT == emTextType)
    {
        iLeftTextLen -= iLen*emTextFormat;

        if(strRemark.length())
            strOutPrintData.append(QString("%1 (%2)\n").arg(text2Integer(iLen,emTextFormat,emLengthType,pszSourceText + iDealIndex,!bIsBigEndian)).arg(strRemark));
        else
            strOutPrintData.append(QString("%1\n").arg(text2Integer(iLen,emTextFormat,emLengthType,pszSourceText + iDealIndex,!bIsBigEndian)));
        iDealIndex += iLen * emTextFormat;
    }
    //4 循环体类型 需要有 loopNum , loop*
    else if(EM_TEXT_TYPE_LOOP_NUM == emTextType)
    {
        if(-1 == lLoopNum)
        {
            if(NO_INDEX == iIndex)
            {
                m_strErrorStr = QString("缺少长度相对位置说明，请添加'index'");
                qDebug() << m_strErrorStr;
                return false;
            }

            lLoopNum = text2Integer(iLen,emTextFormat,emLengthType,pszSourceText + (iDealIndex + iIndex *emTextFormat),!bIsBigEndian);
            qDebug() << "循环体个数" << lLoopNum;

            //将长度和后面数据一起处理时
            if(0 == iIndex)
            {
                iDealIndex += iLen*emTextFormat;
                iLeftTextLen -= iLen*emTextFormat;
                strOutPrintData.append(QString("%1\n").arg(lLoopNum));
            }
        }

        //循环体结构 loop
        if(lLoopNum > 0)
        {
            if(NULL != pArray)
            {
                qDebug() << "loop";
                QJsonArray array(pArray->toArray());
                if(strRemark.length())
                    strOutPrintData.append("( %1 )\n").arg(strRemark);
                else
                    strOutPrintData.append("\n");
                for(int i = 0; i < lLoopNum; i++)
                {
                    strOutPrintData.append(QString("-----loop%1-----\n").arg(i+1));
                    for(int j = 0; j < array.size(); j++)
                    {
                        QJsonObject Obj(array.at(j).toObject());
                        if(!oneBlockDataParse(Obj,pszSourceText,iDealIndex,iLeftTextLen,emTextFormat,strOutPrintData))
                            return false;
                    }
                    if(iLeftTextLen < 1)
                        break;
                }
                if(lLoopNum > 0)
                    strOutPrintData.append("----------\n");
            }
            else
            {
                m_strErrorStr = QString("缺少array数组结构");
                qDebug() << m_strErrorStr;
                return false;
            }
        }

    }
    //5 不定长类型
    else if(EM_TEXT_TYPE_UNCERTAIN_LEN == emTextType)
    {
        if(NO_INDEX == iIndex)
        {
            m_strErrorStr = QString("缺少长度相对位置说明，请添加'index'");
            qDebug() << m_strErrorStr;
            return false;
        }

        unsigned long long lRule = text2Integer(iLen,emTextFormat,emLengthType,pszSourceText + (iDealIndex + emTextFormat * iIndex),!bIsBigEndian);
        qDebug() << "不定长长度为" << lRule;

        //将长度和后面数据一起处理时
        if(0 == iIndex)
        {
            iDealIndex += iLen*emTextFormat;
            iLeftTextLen -= iLen*emTextFormat;
            strOutPrintData.append(QString("%1\n").arg(lRule));
        }
        if(strRemark.length())
            strOutPrintData.append("( %1 )\n").arg(strRemark);

        if(NULL == pArray)
        {
            m_strErrorStr = QString("缺少 array ");
            qDebug() << m_strErrorStr;
            delete pArray;
            return false;
        }
        else if(lRule > 0)
        {
            if(0 == pArray->toArray().size())
            {
                m_strErrorStr = QString("长度为%1，但是rule数组大小为0，不处理").arg(lRule);
                qDebug() << m_strErrorStr;
                delete pArray;
                return false;
            }
            QJsonArray array(pArray->toArray());
            for(int i = 0 ; i < array.size();i++)
            {
                QJsonObject Obj(array.at(i).toObject());
                if(!oneBlockDataParse(Obj,pszSourceText,iDealIndex,iLeftTextLen,emTextFormat,strOutPrintData))
                {
                    delete pArray;
                    return false;
                }
            }
        }
    }
    else if(EM_TEXT_TYPE_BIT_PROCESING == emTextType)
    {
        if(NULL == pArray)
        {
            m_strErrorStr = QString("缺少 array ");
            qDebug() << m_strErrorStr;
            delete pArray;
            return false;
        }
        int iSubDealIndex = 0;
        int iSubLeftLength = iLen * 8 *emTextFormat;
        char *pBuf = new char[iSubLeftLength+1];
        memset(pBuf,0,iSubLeftLength+1);

        for(int i = 0; i < iLen; i++)
        {
            unsigned char ucTemp = 0;
            if(bIsBigEndian)
                ucTemp = QString("%1").arg(pszSourceText+(iDealIndex + (iLen - i - 1)*emTextFormat)).left(2).toUInt(NULL,16);
            else
                ucTemp = QString("%1").arg(pszSourceText+(iDealIndex+i*emTextFormat)).left(2).toUInt(NULL,16);

            qDebug() << "ucTemp" << ucTemp;

            for(int j = 0; j < 8; j++)
            {
                if(bReverse)
                    sprintf(pBuf,"%s%02x%s",pBuf,((ucTemp >> (8 - j - 1)) & 0x1),(EM_TEXT_FORMAT_HEXHEX==emTextFormat)?"":" ");
                else
                    sprintf(pBuf,"%s%02x%s",pBuf,((ucTemp >> j) & 0x1),(EM_TEXT_FORMAT_HEXHEX==emTextFormat)?"":" ");
            }
        }
        qDebug() << "pBuf" << pBuf;
        strOutPrintData.append("\n");
        QJsonArray array(pArray->toArray());
        for(int i = 0 ; i < array.size();i++)
        {
            QJsonObject Obj(array.at(i).toObject());
            if(!oneBlockDataParse(Obj,pBuf,iSubDealIndex,iSubLeftLength,emTextFormat,strOutPrintData,true))
            {
                delete pArray;
                return false;
            }
            if(iSubLeftLength < 2)
                break;
        }
        delete [] pBuf;
        iDealIndex += iLen*emTextFormat;
        iLeftTextLen -= iLen*emTextFormat;
    }
    if(pArray)
        delete pArray;

    return true;
}

/* genJsonConfigFile 根据map生成json配置文件
 * [in] stTextParseFormatMap   解析方法Map
 * [in] strFileName       json文件名
 */
bool CTextParse::genJsonConfigFile(QMap<QString ,ST_TEXT_PARSE_FORMAT> &stTextParseFormatMap, const QString &strFileName)
{
    if(stTextParseFormatMap.isEmpty())
    {
        m_strErrorStr = QString("Map is empty");
        return false;
    }
    QJsonDocument jsonDom;
    QMap<QString ,ST_TEXT_PARSE_FORMAT>::iterator it = stTextParseFormatMap.begin();

    QJsonObject jsonMainObj;
    for(;it != stTextParseFormatMap.end();it++)
    {
        //description 特殊处理
        if(it.key().contains("description"))
        {
            if(0 == it.value().stProtocolDetailList.size())
            {
                m_strErrorStr = QString("description must has a value");
                return false;
            }
            jsonMainObj.insert(it.key(),it.value().stProtocolDetailList[0].strPrintName);
            continue;
        }

        QJsonArray jsonArray;
        bool bRet = genOneBlockJson(jsonArray,it->stProtocolDetailList);
        if(!bRet)
            return false;
        jsonMainObj.insert(it.key(),jsonArray);
    }
    jsonDom.setObject(jsonMainObj);

    QFile file(strFileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    if(file.isOpen())
    {
        file.write(jsonDom.toJson());
        file.close();
    }
    else
        return false;
    return true;
}
/*
 *      genOneBlockJson  根据结构生成一块解析数据
 *      mainArray   数组对象
 *      stProtocolList 待解析数据列表
 */
bool CTextParse::genOneBlockJson(QJsonArray &mainArray,const QList<ST_PROTOCOL_DETAIL> &stProtocolList)
{
    qDebug() << "stProtocolList.size()" << stProtocolList.size();
    for(int i = 0 ; i < stProtocolList.size(); i++)
    {
        QJsonObject jsonObj;
        jsonObj.insert("name",stProtocolList.at(i).strPrintName);
        jsonObj.insert("length",stProtocolList.at(i).iLen);
        jsonObj.insert("type",stProtocolList.at(i).emTextType);

        if(stProtocolList.at(i).emLenType > EM_LENGTH_TYPE_UNKNOWM)
            jsonObj.insert("lengthType",stProtocolList.at(i).emLenType);

        if(stProtocolList.at(i).iLoopNum > 0)
            jsonObj.insert("loopNum",stProtocolList.at(i).iLoopNum);

        if(stProtocolList.at(i).iIsBigEndian >= 0)
        {
            QJsonValue value((bool)stProtocolList.at(i).iIsBigEndian);
            jsonObj.insert("bigEndian",value);
        }
        if(stProtocolList.at(i).bReverse)
        {
            QJsonValue value((bool)stProtocolList.at(i).bReverse);
            jsonObj.insert("reverse",value);
        }
        //循环体  变长
        if(EM_TEXT_TYPE_LOOP_NUM == stProtocolList.at(i).emTextType
                || EM_TEXT_TYPE_UNCERTAIN_LEN == stProtocolList.at(i).emTextType
                || EM_TEXT_TYPE_BIT_PROCESING == stProtocolList.at(i).emTextType)
        {
            jsonObj.insert("index",stProtocolList.at(i).iIndex);

            for(int j = 0 ; j < stProtocolList.at(i).stLoopOrRuleList.size(); j++)
            {
                QJsonArray array;
                bool bRet = genOneBlockJson(array,stProtocolList.at(i).stLoopOrRuleList);
                if(!bRet)
                    return false;
                jsonObj.insert("array",array);
            }
        }
        mainArray.insert(i,jsonObj);
    }
    return true;
}
/*
 *  text2Integer  块转成整型等
 *  [in]    iByteCnt  字节数
 *  [in]    emTextFormat 当前数据格式
 *  [in]    emLengthType 长度转换类型
 *  [in]    pTextData 原始数据
 *  [in]    bIsLittleEndian 是否是小端数据
 *  return 转换后的计数
 */
unsigned long long CTextParse::text2Integer(unsigned int iByteCnt,EM_TEXT_FORMAT emTextFormat,EM_LENGTH_TYPE emLengthType, const char *pTextData,bool bIsLittleEndian)
{
    switch(emLengthType)
    {
    case EM_LENGTH_TYPE_INTEGER:
    {
        Asiic2long a2l,test;
        memset(&a2l,0,sizeof(a2l));
        memset(&test,0,sizeof(test));
        test.ulData = 1;
        qDebug() << "bIsLittleEndian" << bIsLittleEndian;
        qDebug() << "1 == test.szData[0]" << (1 == test.szData[0]);
        for(int i = 0; i < iByteCnt ; i++)
        {
            if((bIsLittleEndian && 1 == test.szData[0]) || (!bIsLittleEndian && 0 == test.szData[0]))
                a2l.szData[i] = QString(pTextData+(i*emTextFormat)).left(2).toInt(NULL,16);
            else
                a2l.szData[iByteCnt - i - 1] = QString(pTextData+(i*emTextFormat)).left(2).toInt(NULL,16);
        }
        return a2l.ulData;
    }
    case EM_LENGTH_TYPE_BCD:
    {
        QString strTemp;
        for(int i = 0 ; i < iByteCnt; i++)
            strTemp.append(QString(pTextData+i*emTextFormat).left(2));
        return strTemp.toULong();
    }
    case EM_LENGTH_TYPE_ASIIC:
    {
        QString strTemp;
        for(int i = 0 ; i < iByteCnt; i++)
            strTemp.append(QString(pTextData+i*emTextFormat+1).left(1));
        return strTemp.toULong();
    }
    case EM_LENGTH_TYPE_BIT:
    {
        Asiic2long a2l,test;
        memset(&a2l,0,sizeof(a2l));
        memset(&test,0,sizeof(test));
        test.ulData = 1;
        qDebug() << "bIsLittleEndian" << bIsLittleEndian;
        qDebug() << "1 == test.szData[0]" << (1 == test.szData[0]);
        bool bSave = ((bIsLittleEndian && 1 == test.szData[0]) || (!bIsLittleEndian && 0 == test.szData[0]));
        for(int i = 0; i < iByteCnt ; i++)
        {
            if(bSave)
                a2l.ulData |= (QString(pTextData+(i*emTextFormat)).left(2).toUInt() << i);
            else
                a2l.ulData |= (QString(pTextData+(i*emTextFormat)).left(2).toUInt() << (iByteCnt - i - 1));
        }
        return a2l.ulData;
    }
    default:
        qDebug() << "unknown length type " << emLengthType;
        break;
    }
    return 0;
}

/*
 * getLastErrorStr 获取最近一次错误信息
 * return QString
 */
const QString &CTextParse::getLastErrorStr()
{
    return m_strErrorStr;
}


