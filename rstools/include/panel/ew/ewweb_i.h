// -------------------------------------------------------------------------------------------------
// 2013-08-13  Интерфейс расширения для WebForm

#ifndef _EWWEB_I_H_
#define _EWWEB_I_H_


// -----------------------------------------------------------------------------
// Сообщения, поддерживаемые обработчиком THandler
// Новые сообщения добавлять только в конец перечисления!
enum
   {
    EW_WEB_Version = 1,
    EW_WEB_Init,
    EW_WEB_CleanUp,
    EW_WEB_GetTypeInfoCount,
    EW_WEB_GetTypeInfo,
    EW_WEB_GetIDsOfNames,
    EW_WEB_Invoke
  };


// -----------------------------------------------------------------------------
// Прототип функции-обработчика, экспортируемый из DLL расширения
// [in]      - идентификатор сообщения (EW_WEB_)
// [in, out] - указатель на структуру
// [in, out] - резерв
typedef HRESULT (CALLBACK *THandler)(int msg, void *hi, void *low);



// -----------------------------------------------------------------------------
// Структуры, передаваемые в обработчик

// Сообщение EW_WEB_Version
typedef struct
      {
       int32            version;
      } TEWWVersion;


// Сообщение EW_WEB_Init
typedef struct
      {
       void            *appIntf;
       HRESULT          retVal;
      } TEWWInit; 


// Сообщение EW_WEB_CleanUp
typedef struct
      {
       HRESULT          retVal;
      } TEWWCleanUp;


// Сообщение EW_WEB_GetTypeInfoCount
typedef struct
      {
       UINT            *nInfo;
       HRESULT          retVal;
      } TEWWGetTypeInfoCount;


// Сообщение EW_WEB_GetTypeInfo
typedef struct
      {
       UINT             iTInfo;
       LCID             lcid;
       ITypeInfo      **ppTInfo;
       HRESULT          retVal;
      } TEWWGetTypeInfo;


// Сообщение EW_WEB_GetIDsOfNames
typedef struct
      {
       IID              riid;
       LPOLESTR        *rgszNames;
       UINT             cNames;
       LCID             lcid;
       DISPID          *rgDispId;
       HRESULT          retVal;
      } TEWWGetIDsOfNames;

// Сообщение EW_WEB_Invoke
typedef struct
      {
       DISPID           dispIdMember;
       IID              riid;
       LCID             lcid;
       WORD             wFlags;
       DISPPARAMS      *pDispParams;
       VARIANT         *pVarResult;
       EXCEPINFO       *pExcepInfo;
       UINT            *puArgErr;
       HRESULT          retVal;
      } TEWWInvoke;

#endif

/* <-- EOF --> */