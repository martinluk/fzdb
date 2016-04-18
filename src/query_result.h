#ifndef FUZZY_QUERYRESULTS
#define FUZZY_QUERYRESULTS

#include "./base_query_result.h"

// This class is responsible for returning a structured JSON response to the user.
class QueryResult : public BaseQueryResult
{
public:
  QueryResult();
  
  enum class ErrorCode
  {
      NoError = 0,              // Everything was A-OK.
      GenericError,             // Some general error occurred.
      ParseError,               // An error occurred when parsing the query.
      NotImplemented,           // The attempted command is not yet implemented.
      TypeMismatch,             // There was a type mismatch with the data provided.
      FileSystemError,          // There was an error using the file system.
      InsufficientPermissions,  // The user executing this command does not have sufficient privileges.
      UserDataError,            // There was an error with the bookkeeping data for this user. (?)

      UnhandledError            // Implies something happened while processing that we didn't account for.
  };
  
  // Canonical strings for JSON properties.
  static const std::string PROPERTY_STATUS;
  static const std::string PROPERTY_ERRORCODE;
  static const std::string PROPERTY_INFO;
  static const std::string PROPERTY_RESULT;
  static const std::string PROPERTY_RESULTTYPE;
  static const std::string PROPERTY_RESULTDATA;
  static const std::string PROPERTY_VALUE_TEXT;
  static const std::string PROPERTY_VALUE_FSPARQL;

  // Convenience function for generating an error result.
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
