#ifndef _OCIQuery_h
#define _OCIQuery_h
/*
#ifndef OCCI_ORACLE

#define WIN32COMMON

#include <occi.h>
#include <ocidem.h>

using namespace oracle::occi;


#endif
*/


#include <oci.h>
#include <oratypes.h>
#include <ocidfn.h>
#include <ocidem.h>
#include <ociapr.h>

#include <stdio.h>
#include <math.h>

//#include <exception.h>

#define __DEBUG__

#define SQL_LENGTH  5120							//一般可执行的sql语句长度		
const   int PREFETCH_ROWS = 20000;						//预先提取n行数据到缓冲区,减少网络流量 //???? 如果超出了记录会出现乱码,将来仔细考虑
const	int MAX_PARAM_NAME_LENGTH = 256;//255;			//最大的名称长度
const	int MAX_STRING_VALUE_LENGTH = 5120;//255;			//返回的字符串最大的列、返回过程参数长度
const	int MAX_LOB_BUFFER_LENGTH = 10240;//1024;			//LOB数据缓冲区的最大空间
const	int MAX_ERRMSG_LENGTH = 1024;					//错误信息的最大长度
const	int MAX_SQLSTMT_LENGTH = 1024;				//出现错误的SQL语句长度
const	int MAX_PARAMS_COUNT = 100;						//参数最大数目
const	int MAX_ERR_CAT_LENGTH = 50;					//错误分类长度
const	int LOB_FLUSH_BUFFER_SIZE = 400*1024;		//LOB数据积累到此量时，写入数据库

//error message definination:
const	char* const ERR_GENERAL = "General Error: %s"; //throw TOCIException("TOCIQuery(TOCIDatabase &db)", ERR_GENERAL, "Can not declare a TOCIQuery when the database is not connected");
const	char* const ERR_INDEX_OUT_OF_BOUND = "%s";    //throw TOCIException(fSqlStmt , ERR_INDEX_OUT_OF_BOUND, "field index out of bound when call Field(i)");	
const	char* const ERR_DB_INIT = "OCI: OCI handles init fail in TDatabase constructor: @line:%d";
const	char* const ERR_SET_LOGIN = "OCI: You can only set login infomation on disconnect status: line %d";
const	char* const ERR_CONNECT_NO_LOGIN_INFO = "No login information provided before Connect(), call SetLogin first, line:%d";
const	char* const ERR_NO_DATASET = "OCI: function:%s , Result Dataset is on Bof/Eof. field:%s"; //throw TOCIException(fParentQuery->fSqlStmt, ERR_NO_DATASET, "asBlobBuffer()", name);
const	char* const ERR_DATA_TYPE_CONVERT = "Data type convertion error: field:%s data type:%d can not be access by %s"; //throw TOCIException(fParentQuery->fSqlStmt, ERR_DATA_TYPE_CONVERT, name, type, "asLobBuffer()");
const	char* const ERR_NOMORE_MEMORY_BE_ALLOCATED = "no more memory can be allocate when :%s, source code:%d"; //throw TOCIException(fParentQuery->fSqlStmt, ERR_NOMORE_MEMORY_BE_ALLOCATED, "asBlobBuffer()", __LINE__);
const	char* const ERR_FILE_IO = "%s: can not open file:%s"; //throw TOCIException(fParentQuery->fSqlStmt, ERR_FILE_IO, "LoadFromFile()", fileName);
const	char* const ERR_MEM_BUFFER_IO = "asBlobWriter() error: read from file to buffer, field:%s, file:%s, @line:%d"; //throw TOCIException(fParentQuery->fSqlStmt, ERR_MEM_BUFFER_IO, name, fileName, __LINE__);
const	char* const ERR_DATA_TYPE_NOT_SUPPORT = "field:%s, datatype:%d not yet supported"; //, pCurrField->name,innerDataType);
const	char* const ERR_PARAM_NOT_EXISTS = "param:%s does not exists."; //throw TOCIException(fSqlStmt, ERR_PARAM_NO_EXISTS, paramName, "check spelling error");
const	char* const ERR_FIELD_NOT_EXISTS = "field:%s does not exists.";
const	char* const ERR_INVALID_METHOD_CALL	= "%s: invalid call method:%s";
const	char* const ERR_CAPABILITY_NOT_YET_SUPPORT = "capability not support yet:%s"; //例如参数个数超越范围
const	char* const ERR_READ_PARAM_DATA = "read parameter value data type error, parameter name:%s, method:%s";

//const define:
const char* const NULL_STRING = "";
const int NULL_NUMBER = 0;

/* classes defined in this file: */
class TOCIException;
class TOCIDatabase;
class TOCIQuery;
class TOCIField;
class TOCIParam;

class TOCIException 
{
public:
	char *GetErrMsg() const;
	char *GetErrSrc() const;
	
public:
	TOCIException(sword errNumb, OCIError *err, const char *cat, const char *sql);//执行OCI函数发生的错误
	TOCIException(const char *sql, const char* errFormat, ...);	
	~TOCIException();
	
private:
	char	errCategory[MAX_ERR_CAT_LENGTH+1];	//错误分类
	unsigned char errMessage[MAX_ERRMSG_LENGTH+1];		//错误信息
	char	errSQL[MAX_SQLSTMT_LENGTH+1];			//发生错误的sql语句
	int	errNo;										//错误号
};

class TOCIDatabase
{
	friend class TOCIQuery;
public:
	
public:
	TOCIDatabase();
	~TOCIDatabase();
	void SetLogin(char *user, char *password, char *tnsString) ;
	bool Connect();
	bool Connect(char *usr, char *pwd, char *tns) ;
	void Disconnect();
private:
	char *usr, *pwd, *tns;	//登录名
	bool fConnected;			//在Connect中是否连接成功
	
	OCIError *hDBErr;
	//OCISvcCtx *hDBSvc;		//用于登录链接服务器	
	OCIEnv *hEnv;
	OCIServer *hSvr;
};

class TOCIParam
{
	friend class TOCIQuery;
public:
	~TOCIParam();
	char	name[MAX_PARAM_NAME_LENGTH];
	
	int		asInteger();	//读取返回参数值
	double	asFloat();
	long		asLong();
	char		*asString();
	bool		isNULL();
private:
	TOCIParam();
	ub2		dataType;
	int		intValue;
	double	dblValue;			//存储输入和输出的值(缓冲区)
	long		longValue;			//long数据缓冲区
	char		stringValue[MAX_STRING_VALUE_LENGTH];		//字符串返回缓冲区
	bool		fIsOutput;			//是否是输出参数.默认是输入参数
	sb2		indicator;			//在返回值时候是否为空
	OCIBind  *hBind;
public:
	operator char * ()
	{
		return asString();
	}
	
	operator double()
	{
		return asFloat();
	}
	
	operator long()
	{
		return asLong();
	}
	
	operator int()
	{
		return asInteger();
	}
	
	operator float()
	{
		return asFloat();
	}
};

class TOCIField
{
	friend class TOCIQuery;
	
public:
	~TOCIField(); 
	char	*name;				//字段名称
	long	size;					//数据长度
	long	type;					//数据类型 in(INT_TYPE,FLOAT_TYPE,DATE_TYPE,STRING_TYPE,ROWID_TYPE)
	int	precision;			//数值总长度
	int	scale;				//数值中小数点个数
	bool	nullable;			//字段定义时是否允许为空值--为了和其他的相一致
	
	bool		isNULL();			//在fetch过程中该列的数据是否为空
	char		*asString();
	double	asFloat();
	int		asInteger();
	//Blob处理
	void		asBlobFile(const char *fileName);			//读取到file中
	void		LoadFromFile(const char *fileName);			//写入到blob中
	void		LoadFromBuffer(unsigned char *buf, unsigned int bufLength);	//把LOB的内容用缓冲区内容替代
	void		asBlobBuffer(unsigned char* &buf, unsigned int *bufLength);	//保存到缓冲区,缓冲区的大小自动创建，并返回缓冲区大小*bufLength.
	
	//日期处理
	char		*asDateTimeString();	//把日期型的列以HH:MM:DD HH24:MI格式读取,使用asString()读取的日期型数据类型为HH:MM:DD
	void		asDateTime(int &year, int &month, int &day, int &hour, int &minute, int &second); //返回日期的各个部分
private:
	//日期处理
	void		asDateTimeInternal(int &year, int &month, int &day, int &hour, int &minute, int &second); //返回日期的各个部分,没有作其他校验，只是内部调用
	TOCIField();					
	TOCIQuery *fParentQuery;					//指向该Field所属于的Query
	
	//数据缓冲区,分别为字符串、整数、浮点数分配空间
	ub1	fStrBuffer[MAX_STRING_VALUE_LENGTH];	//用于保存转换为字符串后的值
	ub1	*fDataBuf;						//在分析字段时候获得空间max(该列的最大长度,MAX_STRING_VALUE_LENGTH), 在Destructor中释放
	OCILobLocator *hBlob;				//支持LOB
	sb2	*fDataIndicator;				//在defineByPos中使用，用于在fetch时察看是否有字段值返回、字段值是否被截断;valueIsNULL, isTruncated根据此值获得结果
	
	ub2   fReturnDataLen;				//读取数据时返回的真实长度
	ub2	fInternalType;					//Oracle内部数据类型
	ub2	fRequestType;					//在读取数据时候的请求数据类型
	OCIDefine *hDefine;					//用于读取列信息
public:
	operator char *()
	{
		return asString();
	}
	
	operator double()
	{
		return asFloat();
	}
	
	operator long()
	{
		return asInteger();
	}
	
	operator int()
	{
		return asInteger();
	}
	
	operator float()
	{
		return asFloat();
	}
};

class TOCIQuery
{
	friend class TOCIField;
	
public:
	static bool CompareStrNoCase(char *ori, char *des);
	

public:
	//主要功能
	void Close();													//关闭SQL语句，以准备接收下一个sql语句
	void SetSQL(char *inSqlstmt);								//设置Sqlstatement
	void Open(int prefetchRows=PREFETCH_ROWS);			//打开SQL SELECT语句返回结果集
	bool First();
	bool Last();
	bool Prior();
	bool Next();													//移动到下一个记录
	bool Execute();												//执行非SELECT语句,没有返回结果集
	bool Commit();													//事务提交
	bool Rollback();												//事务回滚
	int  RowsAffected() { return fTotalRowsFetched;};	//DELETE/UPDATE/INSERT语句修改的记录数目
	int  GetSQLCode() { return fErrorNo;};				//返回Oracle执行结果代码

	//获得返回的记录总数
	int  RecordCount();
	
	//与列信息相关				
	int FieldCount();												//总共有几个列
	TOCIField& Field(int index)		;						//返回第i个列信息
	TOCIField& Field(const char *fieldName) ;						//根据列名(不分大小写)返回列信息; 建议使用Field(int i)获得更高的效率
	
	//与参数信息相关
	int ParamCount();
	TOCIParam& Param(int index);								//返回第i个列信息
	TOCIParam& Param(char *paramName);						//根据列名(不分大小写)返回列信息; 建议使用Field(int i)获得更高的效率
	//以下是设置参数值
	void SetParameter(const char *paramName, char* paramValue, bool isOutput = false); 	
	void SetParameter(const char *paramName, int paramValue, bool isOutput = false); 
	void SetParameter(const char *paramName, double paramValue, bool isOutput = false) ;
	void SetParameter(const char *paramName, long paramValue, bool isOutput = false);
	void SetParameterNULL(const char *paramName);
	
	//constructor & destructor
	TOCIQuery(TOCIDatabase *oradb);
	~TOCIQuery();
	
private:
	char *fSqlStmt;						//保存open的Select语句，可以方便调试
	ub2 fStmtType;							//***ub2!!! 保存sqlstmt的类型:SELECT/UPDATE/DELETE/INSERT/CREATE/BEGIN/ALTER...
	bool fActivated;						//是否已经处于打开状态，在调用OCIStmtPrepare成功后为True
	
	unsigned	fFetched;					//0..prefetchRows
	unsigned	fPrefetchRows;				//1.. 
	unsigned	fCurrRow;					//0..fetched */
	unsigned	fTotalRowsFetched;		//rows fetched from the start
	
	int fFieldCount;						//字段个数
	TOCIDatabase *db;						//此query属于哪个Dabase,在Constructor中创建
	TOCIField *fieldList;				//在内部保存的所有字段信息
	void GetFieldsDef();					//获得字段信息,并为字段分配取值的缓冲区
	
	TOCIParam *ParamByName(const char *paramName);//在内部使用，直接返回参数的指针
	
	void CheckError() ;					//用于判断当前的语句是否正确执行，如果有错误则把错误信息放入errMsg;
	
	int fParamCount;						//参数个数
	TOCIParam *paramList;				//所有参数设置的信息
	void GetParamsDef();					//在setSQL时候获得参数信息
	int nTransTimes;						//是否曾经执行过Execute()事务操作，以便与回滚.
	
	OCISession *hUser;
	//OCITrans* hTrans;
	OCIStmt *hStmt;						//用于分析sql语句的handle
	OCISvcCtx *hSvc;						//服务
	OCIError *hErr;						//错误处理
	sword fErrorNo;						//错误号
	bool fEof;							//在Fetch时候，已经达到最后一个记录,防止已经读到最后一个记录后，又fetch发生的错误
	bool fBof;							//在Open()时候为True,在Next()如果有数据为false;用于判断用户是否可以从缓冲区中读取列值,该部分尚未完成
	bool fOpened;						//数据集是否打开	
#ifdef __DEBUG__
    bool bExecuteFlag;
#endif
public:
	bool Eof()
	{
		return 	fEof;	
	}


};

#endif

