#include "ErrorMessages.hpp"

namespace zouipocar::http::ErrorMessages {

const std::string api_fix_missing_parameter("Missing date parameter");
const std::string api_fix_invalid_parameter("Invalid date parameter");
const std::string api_fix_out_of_range_parameter("Out of range date parameter");

const std::string
    api_range_missing_parameter("Missing start and/or stop parameters");
const std::string
    api_range_invalid_parameter("Invalid start and/or stop parameter");
const std::string api_range_out_of_range_parameter(
    "Out of range start and/or stop parameter");
const std::string api_range_invalid_range(
    "start parameter must be strictly less than stop parameter");

} // namespace zouipocar::http::ErrorMessages