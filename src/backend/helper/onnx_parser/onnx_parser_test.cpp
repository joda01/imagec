#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include "onnx_parser.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("onnx_parser:test", "[onnx_parser_test]")
{
  auto result = joda::onnx::OnnxParser::findAiModelFiles("resources/models");

  REQUIRE(result.contains("resources/models/cell_segmentation_brightfield_in_vitro_v1.onnx"));
  REQUIRE(result.contains("resources/models/nucleus_detection_ex_vivo_v1.onnx"));

  CHECK(result["resources/models/nucleus_detection_ex_vivo_v1.onnx"].classes[0] == "nucleus");
  CHECK(result["resources/models/nucleus_detection_ex_vivo_v1.onnx"].classes[1] == "nucleus_unfocused");
  CHECK(result["resources/models/cell_segmentation_brightfield_in_vitro_v1.onnx"].classes[0] == "cell");
  CHECK(result["resources/models/cell_segmentation_brightfield_in_vitro_v1.onnx"].modelPath ==
        "models/cell_segmentation_brightfield_in_vitro_v1.onnx");
}
