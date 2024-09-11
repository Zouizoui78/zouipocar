#ifndef HTTP_ERROR_MESSAGES_HPP
#define HTTP_ERROR_MESSAGES_HPP

#include <string>

namespace zouipocar::http::ErrorMessages {

extern const std::string api_fix_missing_parameter;
extern const std::string api_fix_invalid_parameter;
extern const std::string api_fix_out_of_range_parameter;

extern const std::string api_range_missing_parameter;
extern const std::string api_range_invalid_parameter;
extern const std::string api_range_out_of_range_parameter;
extern const std::string api_range_invalid_range;

} // namespace zouipocar::http::ErrorMessages

#endif