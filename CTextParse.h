#ifndef TEXTPARSE_H
#define TEXTPARSE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonValue>
#include <QJsonValueRef>
#include <QList>

//数据格式
typedef enum
{
    EM_TEXT_FORMAT_HEX_HEX = 3,   //16进制报文带空格符或者其他符号作为分割
    EM_TEXT_FORMAT_HEXHEX = 2,    //16进制报文连续不带空格等
    EM_TEXT_FORMAT_OTHER,
}EM_TEXT_FORMAT;

//长度类型
typedef enum
{
    EM_LENGTH_TYPE_UNKNOWM,
    EM_LENGTH_TYPE_INTEGER,     //整型处理      7b 00
    EM_LENGTH_TYPE_BCD,         //BCD码处理     01 23
    EM_LENGTH_TYPE_ASIIC,       //asiic码处理   31 32 33
    EM_LENGTH_TYPE_BIT,         //按位处理      01 01 00 01 01 01 01
}EM_LENGTH_TYPE;

//当前字段类型
typedef enum
{
    EM_TEXT_TYPE_RAW = 0,               //原始数据
    EM_TEXT_TYPE_ASIIC = 1,             //可转成字符串显示
    EM_TEXT_TYPE_INT = 2,               //转成10进制整数 默认处理小端字节序，如果需要指定处理大小端 请添加 bigEndian：true/false
    EM_TEXT_TYPE_LOOP_NUM = 3,          //循环体 name 可以不要 需要有 index 段 此时 length 代表转换成循环体的字节数，指出长度数据所在位置 例如在前两个字节就是 index=-2 ，length=2 ，长度转换和 type=3时规则一致；，应有 loop:[{}] 作为该段的循环体结构
    EM_TEXT_TYPE_UNCERTAIN_LEN = 4,     //不定长数据 name 可以不要 需要有 index 段 此时 length 代表转换成长度字段字节数，指出长度数据所在位置 例如在前两个字节就是 index=-2 ，length=2 ，长度转换和 type=3时规则一致；当有数据长度时，应有rule:[{}]作为该段的解析方法
    EM_TEXT_TYPE_BIT_PROCESING = 5,     //按位解析数据，应有 array:[{}]作为该段的解析方法，此时array数组中的length均作为‘位数’处理")
}EM_TEXT_TYPE;


//json中协议段 单个有效块的配置
struct ST_PROTOCOL_DETAIL
{
    ST_PROTOCOL_DETAIL():iIsBigEndian(-1),emTextType(EM_TEXT_TYPE_RAW),emLenType(EM_LENGTH_TYPE_UNKNOWM),iIndex(0),iLoopNum(-1){}
    QString strPrintName;       //字段表示名称（必须）
    QString strRemark;           //备注
    int iLen;                   //占用字节长度（必须）
    ST_PROTOCOL_DETAIL *pParent;//用于Array查找上一级
    QList<ST_PROTOCOL_DETAIL> stLoopOrRuleList;//为循环体或者不定长时使用
    EM_TEXT_TYPE emTextType;    //字段内部类型（必须）
    EM_LENGTH_TYPE emLenType;   //不定长和循环体类型时使用 1：整型 2：BCD 3：Asiic
    int iIndex;                 //不定长和循环体类型 用于指定长度数据或者循环次数字段的位置
    int iIsBigEndian;           //大小端，转换成整型时需要用到，-1没有，0不是，1是
    int iLoopNum;               //指定循环体个数
};

//报文格式  json文件每次都重新加载
typedef struct _ST_TEXT_PARSE_FORMAT_
{
    int iLength;        //报文总长度
    QList<ST_PROTOCOL_DETAIL> stProtocolDetailList; //更加以上信息找到的报文解析结构
}ST_TEXT_PARSE_FORMAT;

typedef enum
{
    EM_TEXT_PARSE_RESULT_OK,
    EM_TEXT_PARSE_RESULT_JSON_FILE_OPEN_FAILED,     //JSON文件打开失败
    EM_TEXT_PARSE_RESULT_JSON_FILE_ERROR,           //JSON文件格式错误
    EM_TEXT_PARSE_RESULT_JSON_INNER_FORMAT_ERROR,   //JSON内部格式错误
}EM_TEXT_PARSE_RESULT;

union Asiic2long
{
    unsigned char szData[8];
    unsigned long long ulData;
};

#define NO_INDEX -23333


class CTextParse
{
public:
    CTextParse();

    /*
     *  TextParse
     *  [in] strJSONCfg
     *  [in] strSourceText
     */
    EM_TEXT_PARSE_RESULT TextParse(QString strJSONCfg, const QString &strSourceText);

    /*
     * getLastErrorStr 获取最近一次错误信息
     * return QString
     */
    const QString &getLastErrorStr();


    /* genJsonConfigFile 根据map生成json配置文件
     * [in] stTextParseFormatMap   解析方法Map
     * [in] strFileName       json文件名
     */
    bool genJsonConfigFile(QMap<QString, ST_TEXT_PARSE_FORMAT> &stTextParseFormatMap, const QString &strFileName);

    /* parseDataByJSONFile 根据json文件直接解析报文，并组成输出
     * [in] strJSONCfg   json文件
     * [in] strKey       对应解析方法的key
     * [in] pszSourceText  待解析数据格式16进制空格或者不带空格  31 32 33
     * [out] strOutPrintData  根据json格式解析数据组成输出
     */
    bool parseDataByJSONFile(const QString &strJSONCfg, const QString &strKey,const QString &strSourceText, QString &strOutPrintData);

    /* parseDataByMap 根据Map解析报文，并组成输出
     * [in] stTextParseFormatMap   格式map
     * [in] strKey       对应解析方法的key
     * [in] pszSourceText  待解析数据格式16进制空格或者不带空格  31 32 33
     * [out] strOutPrintData  根据json格式解析数据组成输出
     */
    bool parseDataByMap(const QMap<QString ,ST_TEXT_PARSE_FORMAT> &stTextParseFormatMap,const QString &strKey,const char *pszSourceText,int iSourceTextLen,QString &printData);

private:

    /*  oneBlockDataParse     当个格式报文解析
     *  [in] jsonObject       json对象
     *  [in] pszSourceText    待解析数据
     *  [in/out] iDealIndex   处理到的位置
     *  [in/out] iLeftTextLen 待处理数据长度
     *  [out]  strOutPrintData  根据json格式解析数据组成输出
    *  [out] bIsDealBit  是否按位处理
     */
    bool oneBlockDataParse(QJsonObject &jsonObject, const char *pszSourceText, int&iDealIndex, int &iLeftTextLen, EM_TEXT_FORMAT emTextFormat, QString &strOutPrintData, bool bIsDealBit = false);

    /*  jsonToStruct          将解析方法的json文件解析到结构体中
     *  [in] strJSONCfg       文件
     *  [out]  stTextParseFormatMap  解析key和方法的map表
     */
    EM_TEXT_PARSE_RESULT jsonToStruct(QString strJSONCfg,QMap<QString ,ST_TEXT_PARSE_FORMAT> &stTextParseFormatMap);

    /*
     *  oneBlockFormatParse 一块解析数据解析
     *  [in] jsonObject  json对象
     *  [out]   protolDetailList    解析数据数据结构列表
     *  [out]   iTotalLength        解析数据总长度
     */
    EM_TEXT_PARSE_RESULT oneBlockFormatParse(QJsonObject &jsonObject, QList<ST_PROTOCOL_DETAIL> &protolDetailList, int &iTotalLength);

    /*
     *      genOneBlockJson  根据结构生成一块解析数据
     *      mainArray   数组对象
     *      stProtocolList 待解析数据列表
     */
    bool genOneBlockJson(QJsonArray &mainArray, const QList<ST_PROTOCOL_DETAIL> &stProtocolList);


    /*
     *  text2Integer  数据转成整型等
     *  [in]    iByteCnt  字节数
     *  [in]    emTextFormat 当前数据格式
     *  [in]    emLengthType 长度转换类型
     *  [in]    pTextData 原始数据
     *  [in]    bIsLittleEndian 是否是小端数据
     *  return 转换后的计数
     */
    unsigned long long text2Integer(unsigned int iByteCnt,EM_TEXT_FORMAT emTextFormat, EM_LENGTH_TYPE emLengthType,const char *pTextData,bool bIsLittleEndian);

private:
    QString m_strJsonCfg;
    QString m_strSourceText;
    QString m_strResult;
    QString m_strErrorStr;
};


#endif // TEXTPARSE_H
