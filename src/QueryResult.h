#ifndef FUZZY_QUERYRESULTS
#define FUZZY_QUERYRESULTS

#include "BaseQueryResult.h"

class QueryResult : public BaseQueryResult
{
public:
  QueryResult();
  
  enum class ErrorCode
  {
      NoError = 0,
      GenericError,
      ParseError,
      NotImplemented,
      TypeMismatch,
      FileSystemError,
      InsufficientPermissions,
      UserDataError,

      UnhandledError    // Implies something happened while processing that we didn't account for.
  };
  
  static const std::string PROPERTY_STATUS;
  static const std::string PROPERTY_ERRORCODE;
  static const std::string PROPERTY_INFO;
  static const std::string PROPERTY_RESULT;
  static const std::string PROPERTY_RESULTTYPE;
  static const std::string PROPERTY_RESULTDATA;
  static const std::string PROPERTY_VALUE_TEXT;
  static const std::string PROPERTY_VALUE_FSPARQL;

  static QueryResult generateError(ErrorCode code, const std::string &errorString);
  
  // QueryResult interface - these properties should always be present within the JSON.
  /*
   * {
   *    status: [bool],
   *    errorCode: [int],
   *    info: [string],
   * 
   *    // Empty object if not applicable.
   *    result:
   *    {
   *        type: [string],    // "text" or "fsparql"
   *        data: [string] if "text", [array] if "fsparql" 
   *    }
   * }
   */
  
  bool status() const;
  void setStatus(bool success);
  
  // Automatically sets status too.
  ErrorCode errorCode() const;
  void setErrorCode(ErrorCode code);
  
  std::string info() const;
  void setInfo(const std::string &str);
  
  std::string resultType() const;
  void setResultType(const std::string &type);
  
  std::string resultDataText() const;
  void setResultDataText(const std::string &data);
  
  // Expects an array value.
  void setResultDataFsparql(rapidjson::Value &data);
};

#endif
