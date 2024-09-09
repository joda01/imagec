THEN("Check the Confidence for 0 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3725.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(6250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
}
}
THEN("Check the Area size for 0 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(29.3826, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(22.6720, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(309.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(350.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4378.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5668.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(19.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(15.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(33.8322, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(28.9090, 0.0001));
}
}
THEN("Check the Perimeter for 0 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(17.6316, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(15.4163, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(70.3259, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(92.3675, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.6569, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5.6569, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2627.1051, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3854.0769, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(15.3137, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(13.8995, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(9.4207, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8.8643, 0.0001));
}
}
THEN("Check the Circularity for 0 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.9431, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.9427, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.6245, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.4756, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(140.5224, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(235.6788, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.9964, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.9915, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0806, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0891, 0.0001));
}
}
THEN("Check the Intensity sum for 0 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7413.0872, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3940.1240, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(35781.4564, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(16433.6240, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(450271.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(169369.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2473900.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(684444.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(299.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(314.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(646.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(957.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1104550.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(985031.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5331437.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4108406.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2495.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1959.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(11144.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(7878.5000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(36859.9412, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(11197.4731, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(202164.1625, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(46167.8663, 0.0001));
}
}
THEN("Check the Intensity avg for 0 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(153.1384, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(142.0962, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(687.5925, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(572.3871, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1457.1877, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(483.9114, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(8006.1489, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1955.5543, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(99.6667, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(103.8333, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(215.3333, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(213.6000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(22817.6241, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(35524.0559, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(102451.2850, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(143096.7655, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(137.6000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(135.7700, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(617.6250, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(552.3316, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(110.0968, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(31.8662, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(623.9314, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(193.2315, 0.0001));
}
}
THEN("Check the Intensity min for 0 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(105.9530, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(106.6840, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(468.2416, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(419.8960, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(151.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(241.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1022.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(924.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(55.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(54.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(154.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(152.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(15787.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(26671.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(69768.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(104974.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(105.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(104.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(465.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(401.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(15.4072, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(20.4498, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(125.1295, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(129.8538, 0.0001));
}
}
THEN("Check the Intensity max for 0 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(272.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(210.9720, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1221.7651, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(842.6680, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7280.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1798.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(46219.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8762.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(109.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(112.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(247.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(261.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(40528.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(52743.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(182043.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(210667.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(185.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(172.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(825.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(723.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(593.3112, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(151.2897, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3729.4090, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(652.6690, 0.0001));
}
}
THEN("Check the x for 0 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1166.3154, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(832.0040, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2028.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2041.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(45.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(173781.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(208001.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1446.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(703.5000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(649.8175, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(560.6153, 0.0001));
}
}
THEN("Check the y for 0 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1062.9195, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1200.4760, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1993.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2047.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(158375.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(300119.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(959.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1223.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(513.7714, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(625.1319, 0.0001));
}
}
THEN("Check the Cross channel count for 0 data")
{
}
THEN("Check the  for 0 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.5973, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.9600, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(24.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(29.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(834.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1240.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3.1147, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.8338, 0.0001));
}
}
THEN("Check the  for 0 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(250.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.5973, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.9600, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(24.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(29.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(834.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1240.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::A,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3.1147, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.8338, 0.0001));
}
}
THEN("Check the Confidence for 1 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(416.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(19550.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(35360.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
}
}
THEN("Check the Area size for 1 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(416.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(30.8043, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(24.9062, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(353.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(384.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7085.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(10361.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(20.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(18.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(35.8377, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(27.8729, 0.0001));
}
}
THEN("Check the Perimeter for 1 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(416.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(18.2568, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(16.6694, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(85.8822, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(90.1249, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.6569, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5.6569, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4199.0617, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(6934.4510, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(15.8995, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(15.3137, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(10.4334, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8.7769, 0.0001));
}
}
THEN("Check the Circularity for 1 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(416.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.9202, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.9273, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.5554, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.3742, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(211.6396, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(385.7718, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.9477, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.9674, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0979, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.1069, 0.0001));
}
}
THEN("Check the Intensity sum for 1 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(416.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(416.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5905.8000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3522.7428, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(30400.3652, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(15781.5144, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(458151.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(176904.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2519565.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(735092.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(201.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(245.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1317.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1014.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1358334.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1465461.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6992084.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(6565110.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2192.5000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1965.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(11320.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8817.5000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(30332.7072, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(9256.2677, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(167067.0216, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(39192.0668, 0.0001));
}
}
THEN("Check the Intensity avg for 1 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(416.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(416.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.2195, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(117.7903, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(613.0224, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(532.4967, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1297.8782, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(460.6875, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7137.5781, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1914.3021, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(63.9375, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(78.1579, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(379.5000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(338.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(28110.4793, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(49000.7716, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(140995.1508, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(221518.6364, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(110.6879, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(112.0500, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(560.1403, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(498.4050, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(82.7742, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(29.8655, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(454.8668, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(160.7133, 0.0001));
}
}
THEN("Check the Intensity min for 1 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(416.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(416.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(85.4261, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(86.6322, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(469.5957, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(425.2188, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(143.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(175.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1109.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(983.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(59.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(295.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(255.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(19648.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(36039.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(108007.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(176891.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(84.5000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(84.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(445.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(407.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(14.7702, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(15.6866, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(112.8323, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(109.5021, 0.0001));
}
}
THEN("Check the Intensity max for 1 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(416.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(416.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(198.0696, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(166.9375, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(993.5870, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(728.5986, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7280.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1798.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(46219.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8762.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(70.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(417.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(352.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(45556.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(69446.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(228525.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(303097.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(147.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(146.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(697.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(613.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(474.8887, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(103.0870, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3013.7564, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(519.4021, 0.0001));
}
}
THEN("Check the x for 1 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(416.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1323.4870, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(822.7308, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2046.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2042.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(304402.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(342256.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1535.5000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(701.5000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(601.7528, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(564.6143, 0.0001));
}
}
THEN("Check the y for 1 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(416.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(947.8478, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1185.7572, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2047.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2046.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(218005.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(493275.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(810.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1224.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(501.1337, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(611.3158, 0.0001));
}
}
THEN("Check the Cross channel count for 1 data")
{
}
THEN("Check the  for 1 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(416.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.7174, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5.3798, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(26.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(27.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1315.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2238.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3.1861, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.8858, 0.0001));
}
}
THEN("Check the  for 1 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(416.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.7174, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5.3798, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(26.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(27.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1315.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2238.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::B,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3.1861, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.8858, 0.0001));
}
}
THEN("Check the Confidence for 2 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(38.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6272.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8631.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6272.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8631.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6272.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8631.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(238336.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(405657.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6272.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8631.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
}
}
THEN("Check the Area size for 2 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(38.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(11667.6053, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(12608.8723, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(25301.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(62046.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2003.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1827.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(443369.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(592617.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(11179.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8351.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5519.6669, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(10755.7239, 0.0001));
}
}
THEN("Check the Perimeter for 2 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(38.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(377.0204, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(371.6861, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(619.4012, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(975.7586, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(178.1493, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(175.8061, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(14326.7736, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(17469.2455, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(368.3488, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(312.2153, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(102.2538, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(149.2648, 0.0001));
}
}
THEN("Check the Circularity for 2 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(38.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.9565, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.9629, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.7157, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.7428, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(36.3465, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(45.2583, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0702, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0715, 0.0001));
}
}
THEN("Check the Intensity sum for 2 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(38.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(38.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(916635.7105, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1034412.1489, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4161003.6579, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4416380.5319, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2043814.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5844449.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(9666147.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(27048093.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(103525.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(159690.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(565905.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(638391.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(34832157.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(48617371.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(158118139.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(207569885.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(849829.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(699025.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3876322.5000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2707554.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(429887.0917, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(962988.8278, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2039704.2973, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4450321.7913, 0.0001));
}
}
THEN("Check the Intensity avg for 2 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(38.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(38.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(78.2491, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(81.1300, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(353.3824, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(336.6246, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(106.1267, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(94.2485, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(438.5411, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(435.9361, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(51.6850, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(66.4534, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(279.4523, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(275.2735, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2973.4649, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3813.1082, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(13428.5312, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(15821.3542, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(78.9610, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(81.4516, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(352.0223, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(336.6460, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(9.8676, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5.9336, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(40.8587, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(32.8102, 0.0001));
}
}
THEN("Check the Intensity min for 2 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(38.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(38.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(43.8158, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(45.4894, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(205.7105, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(204.5745, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(55.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(53.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(241.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(246.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(24.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(24.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(150.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(164.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1665.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2138.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7817.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(9615.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(44.5000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(46.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(209.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(204.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6.5795, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.9382, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(18.5893, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(18.6381, 0.0001));
}
}
THEN("Check the Intensity max for 2 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(38.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(38.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(282.7368, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(297.2128, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1205.5263, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1181.6809, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1362.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1798.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5582.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8762.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(130.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(152.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(454.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(429.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(10744.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(13969.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(45810.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(55539.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(227.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(988.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(871.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(196.4949, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(285.0615, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(878.3181, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1232.7786, 0.0001));
}
}
THEN("Check the x for 2 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(38.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1244.4737, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(861.2128, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2021.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1938.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(50.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(24.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(47290.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(40477.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1381.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(739.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(602.3841, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(593.0759, 0.0001));
}
}
THEN("Check the y for 2 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(38.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(910.7105, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1029.7447, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1962.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2068.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(26.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(113.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(34607.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(48398.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(856.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1081.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(538.8731, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(600.1200, 0.0001));
}
}
THEN("Check the Cross channel count for 2 data")
{
}
THEN("Check the  for 2 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(38.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(125.1579, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(122.1489, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(238.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(327.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(53.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(49.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4756.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5741.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(117.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(106.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(37.6359, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(51.2844, 0.0001));
}
}
THEN("Check the  for 2 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(38.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(47.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(125.1579, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(122.1489, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(238.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(327.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(53.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(49.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4756.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5741.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(117.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(106.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::C,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(37.6359, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(51.2844, 0.0001));
}
}
THEN("Check the Confidence for 3 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(17.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1502.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1548.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1502.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1548.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1502.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1548.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(25534.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(38700.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1502.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1548.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
}
}
THEN("Check the Area size for 3 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(17.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(53469.9412, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(57971.7200, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(527490.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(776912.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2834.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2152.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(908989.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1449293.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(14972.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(12631.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(125406.7449, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(153974.7228, 0.0001));
}
}
THEN("Check the Perimeter for 3 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(17.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1774.4989, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2137.3175, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(12202.7051, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(22725.2793, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(239.5046, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(294.6589, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(30166.4812, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(53432.9369, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(719.3839, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(693.6265, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2853.5559, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4503.4979, 0.0001));
}
}
THEN("Check the Circularity for 3 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(17.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.3190, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.2593, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.8102, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.6789, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0381, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0189, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.4236, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(6.4836, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.2491, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.2324, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.2350, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.1544, 0.0001));
}
}
THEN("Check the Intensity sum for 3 data")
{
}
THEN("Check the Intensity avg for 3 data")
{
}
THEN("Check the Intensity min for 3 data")
{
}
THEN("Check the Intensity max for 3 data")
{
}
THEN("Check the x for 3 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(17.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1001.5882, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1021.0400, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2016.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1928.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(57.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(37.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(17027.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25526.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(881.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1111.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(686.8708, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(600.3143, 0.0001));
}
}
THEN("Check the y for 3 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(17.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(931.7647, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(966.6000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1988.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2123.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(44.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(97.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(15840.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(24165.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(914.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(923.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(645.6636, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(623.6497, 0.0001));
}
}
THEN("Check the Cross channel count for 3 data")
{
}
THEN("Check the  for 3 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(17.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(246.2941, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(266.3200, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(980.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1285.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(64.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(51.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4187.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(6658.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(195.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.1784, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(262.9306, 0.0001));
}
}
THEN("Check the  for 3 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(17.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(246.2941, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(266.3200, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(980.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1285.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(64.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(51.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4187.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(6658.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(149.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(195.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::D,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(230.1784, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(262.9306, 0.0001));
}
}
THEN("Check the Confidence for 4 data")
{
}
THEN("Check the Area size for 4 data")
{
}
THEN("Check the Perimeter for 4 data")
{
}
THEN("Check the Circularity for 4 data")
{
}
THEN("Check the Intensity sum for 4 data")
{
}
THEN("Check the Intensity avg for 4 data")
{
}
THEN("Check the Intensity min for 4 data")
{
}
THEN("Check the Intensity max for 4 data")
{
}
THEN("Check the x for 4 data")
{
}
THEN("Check the y for 4 data")
{
}
THEN("Check the Cross channel count for 4 data")
{
}
THEN("Check the  for 4 data")
{
}
THEN("Check the  for 4 data")
{
}
THEN("Check the Confidence for 5 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.9596, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.9592, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.5000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.6250, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(117.0724, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(210.0650, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CONFIDENCE,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0677, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0677, 0.0001));
}
}
THEN("Check the Area size for 5 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(28.4918, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(19.9178, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(301.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(304.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3476.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4362.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(20.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(13.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::AREA_SIZE,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(33.9331, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(26.0556, 0.0001));
}
}
THEN("Check the Perimeter for 5 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(17.4505, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(14.4879, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(70.3259, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(81.2965, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2.8284, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.8284, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2128.9659, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3172.8595, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(15.3137, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(12.4853, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::PERIMETER,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(9.5214, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8.2914, 0.0001));
}
}
THEN("Check the Circularity for 5 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.9314, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.9304, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.6459, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.4810, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(113.6279, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(203.7649, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.9582, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.9674, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CIRCULARITY,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0850, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.1018, 0.0001));
}
}
THEN("Check the Intensity sum for 5 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7971.9918, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3591.3562, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(39302.5410, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(15128.2785, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(447193.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(157145.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2461311.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(648678.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(103.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(525.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(387.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(972583.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(786507.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4794910.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3313093.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2687.5000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1782.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(11655.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(7708.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(40382.9236, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(10968.2820, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_SUM,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(222046.6776, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(45737.6512, 0.0001));
}
}
THEN("Check the Intensity avg for 5 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(158.1100, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(144.3047, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(729.6097, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(600.4859, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1485.6910, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(516.9243, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(8177.1128, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2133.8093, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(103.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(103.8333, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(392.7500, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(361.1111, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(19289.4184, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(31602.7377, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(89012.3779, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(131506.4072, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(137.6500, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(136.8000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(635.9955, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(564.7143, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(124.2301, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(35.3245, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_AVG,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(693.4357, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(205.9629, 0.0001));
}
}
THEN("Check the Intensity min for 5 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(109.4426, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(109.2877, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(501.6393, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(453.5479, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(151.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(208.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1022.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1031.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(82.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(68.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(295.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(255.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(13352.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(23934.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(61200.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(99327.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(107.5000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(106.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(494.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(423.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(14.4491, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(19.4694, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MIN,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(108.6648, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(139.2376, 0.0001));
}
}
THEN("Check the Intensity max for 5 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(266.1639, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(200.6667, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1310.3361, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(852.6393, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7280.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1798.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(46219.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8762.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(103.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(118.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(462.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(387.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(32472.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(43946.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(159861.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(186728.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(182.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(171.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(823.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(724.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(645.1862, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(130.4062, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::INTENSITY_MAX,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 1,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4115.9523, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(671.5272, 0.0001));
}
}
THEN("Check the x for 5 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1181.6066, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(819.0502, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2028.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2041.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(45.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(144156.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(179372.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1473.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(702.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_X,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(651.3802, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(563.5683, 0.0001));
}
}
THEN("Check the y for 5 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1046.7459, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1211.8630, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1993.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2047.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(127703.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(265398.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(936.5000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1230.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::CENTER_OF_MASS_Y,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(500.6781, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(627.1953, 0.0001));
}
}
THEN("Check the Cross channel count for 5 data")
{
}
THEN("Check the  for 5 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.6311, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.6941, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(24.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(26.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(687.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1028.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_WIDTH,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3.1101, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.6721, 0.0001));
}
}
THEN("Check the  for 5 data")
{
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::CNT,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(219.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::AVG,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.6311, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.6941, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::MAX,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(24.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(26.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::MIN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::SUM,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(687.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1028.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::MEDIAN,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
}
{
auto table = joda::db::StatsPerPlate::toHeatmap(
joda::db::QueryFilter{.analyzer              = results.get(),
                      .plateRows             = 16,
                      .plateCols             = 24,
                      .plateId               = 0,
                      .actGroupId            = 0,
                      .actImageId            = 0,
                      .clusterId             = joda::enums::ClusterId::F,
                      .classId               = joda::enums::ClassId::C0,
                      .className             = "",
                      .measurementChannel    = joda::enums::Measurement::BOUNDING_BOX_HEIGHT,
                      .stats                 = joda::enums::Stats::STDDEV,
                      .crossChanelStack_c    = 0,
                      .crossChannelClusterId = joda::enums::ClusterId::NONE,
                      .crossChannelClassId   = joda::enums::ClassId::C0});
CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3.1101, 0.0001));
CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.6721, 0.0001));
}
}
