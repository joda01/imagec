#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include "onnx_parser.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("onnx_parser:test", "[onnx_parser_test]")
{
  auto result = joda::onnx::Onnx::findOnnxFiles("models");

  REQUIRE(result.contains("models/cell_segmentation_brightfield_in_vitro_v1.onnx"));
  REQUIRE(result.contains("models/nucleus_detection_ex_vivo_v1.onnx"));

  CHECK(result["models/nucleus_detection_ex_vivo_v1.onnx"].classes[0] == "");
  CHECK(result["models/nucleus_detection_ex_vivo_v1.onnx"].classes[1] == "");

  CHECK(result["models/cell_segmentation_brightfield_in_vitro_v1.onnx"].classes[0] == "");
}
