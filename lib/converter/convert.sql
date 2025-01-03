CREATE TABLE image_stats AS
SELECT
	ANY_VALUE(analyze_id) as analyze_id,
	ANY_VALUE(image_id) as image_id,
	ANY_VALUE(channel_id) as channel_id,
	ANY_VALUE(tile_id) as tile_id,
MAP{
	131071:  SUM(element_at(values, 131071)[1]),
	196607 : SUM(element_at(values, 196607 )[1]),
	262143 : SUM(element_at(values, 262143 )[1]),
	327679 : SUM(element_at(values, 327679 )[1]),
	393215 : SUM(element_at(values, 393215 )[1]),
	458751 : SUM(element_at(values, 458751 )[1]),
	524287 : SUM(element_at(values, 524287 )[1]),
	589823 : SUM(element_at(values, 589823 )[1]),
	655359 : SUM(element_at(values, 655359 )[1]),
	720895 : SUM(element_at(values, 720895 )[1]),
	786431 : SUM(element_at(values, 786431 )[1]),
	851967 : SUM(element_at(values, 851967 )[1]),
	917503 : SUM(element_at(values, 917503 )[1]),
	917508 : SUM(element_at(values, 917508 )[1]),
	983044 : SUM(element_at(values, 983044 )[1]),
	1048580: SUM(element_at(values, 1048580)[1])
	} as sum,

MAP{
	131071:  COUNT(element_at(values, 131071)[1]),
	196607 : COUNT(element_at(values, 196607 )[1]),
	262143 : COUNT(element_at(values, 262143 )[1]),
	327679 : COUNT(element_at(values, 327679 )[1]),
	393215 : COUNT(element_at(values, 393215 )[1]),
	458751 : COUNT(element_at(values, 458751 )[1]),
	524287 : COUNT(element_at(values, 524287 )[1]),
	589823 : COUNT(element_at(values, 589823 )[1]),
	655359 : COUNT(element_at(values, 655359 )[1]),
	720895 : COUNT(element_at(values, 720895 )[1]),
	786431 : COUNT(element_at(values, 786431 )[1]),
	851967 : COUNT(element_at(values, 851967 )[1]),
	917503 : COUNT(element_at(values, 917503 )[1]),
	917508 : COUNT(element_at(values, 917508 )[1]),
	983044 : COUNT(element_at(values, 983044 )[1]),
	1048580: COUNT(element_at(values, 1048580)[1])
	} as cnt,
MAP{
	131071:  MIN(element_at(values, 131071)[1]),
	196607 : MIN(element_at(values, 196607 )[1]),
	262143 : MIN(element_at(values, 262143 )[1]),
	327679 : MIN(element_at(values, 327679 )[1]),
	393215 : MIN(element_at(values, 393215 )[1]),
	458751 : MIN(element_at(values, 458751 )[1]),
	524287 : MIN(element_at(values, 524287 )[1]),
	589823 : MIN(element_at(values, 589823 )[1]),
	655359 : MIN(element_at(values, 655359 )[1]),
	720895 : MIN(element_at(values, 720895 )[1]),
	786431 : MIN(element_at(values, 786431 )[1]),
	851967 : MIN(element_at(values, 851967 )[1]),
	917503 : MIN(element_at(values, 917503 )[1]),
	917508 : MIN(element_at(values, 917508 )[1]),
	983044 : MIN(element_at(values, 983044 )[1]),
	1048580: MIN(element_at(values, 1048580)[1])
	} as min,
MAP{
	131071:  MAX(element_at(values, 131071)[1]),
	196607 : MAX(element_at(values, 196607 )[1]),
	262143 : MAX(element_at(values, 262143 )[1]),
	327679 : MAX(element_at(values, 327679 )[1]),
	393215 : MAX(element_at(values, 393215 )[1]),
	458751 : MAX(element_at(values, 458751 )[1]),
	524287 : MAX(element_at(values, 524287 )[1]),
	589823 : MAX(element_at(values, 589823 )[1]),
	655359 : MAX(element_at(values, 655359 )[1]),
	720895 : MAX(element_at(values, 720895 )[1]),
	786431 : MAX(element_at(values, 786431 )[1]),
	851967 : MAX(element_at(values, 851967 )[1]),
	917503 : MAX(element_at(values, 917503 )[1]),
	917508 : MAX(element_at(values, 917508 )[1]),
	983044 : MAX(element_at(values, 983044 )[1]),
	1048580: MAX(element_at(values, 1048580)[1])
	} as max,
MAP{
	131071:  MEDIAN(element_at(values, 131071)[1]),
	196607 : MEDIAN(element_at(values, 196607 )[1]),
	262143 : MEDIAN(element_at(values, 262143 )[1]),
	327679 : MEDIAN(element_at(values, 327679 )[1]),
	393215 : MEDIAN(element_at(values, 393215 )[1]),
	458751 : MEDIAN(element_at(values, 458751 )[1]),
	524287 : MEDIAN(element_at(values, 524287 )[1]),
	589823 : MEDIAN(element_at(values, 589823 )[1]),
	655359 : MEDIAN(element_at(values, 655359 )[1]),
	720895 : MEDIAN(element_at(values, 720895 )[1]),
	786431 : MEDIAN(element_at(values, 786431 )[1]),
	851967 : MEDIAN(element_at(values, 851967 )[1]),
	917503 : MEDIAN(element_at(values, 917503 )[1]),
	917508 : MEDIAN(element_at(values, 917508 )[1]),
	983044 : MEDIAN(element_at(values, 983044 )[1]),
	1048580: MEDIAN(element_at(values, 1048580)[1])
	} as median,
MAP{
	131071:  AVG(element_at(values, 131071)[1]),
	196607 : AVG(element_at(values, 196607 )[1]),
	262143 : AVG(element_at(values, 262143 )[1]),
	327679 : AVG(element_at(values, 327679 )[1]),
	393215 : AVG(element_at(values, 393215 )[1]),
	458751 : AVG(element_at(values, 458751 )[1]),
	524287 : AVG(element_at(values, 524287 )[1]),
	589823 : AVG(element_at(values, 589823 )[1]),
	655359 : AVG(element_at(values, 655359 )[1]),
	720895 : AVG(element_at(values, 720895 )[1]),
	786431 : AVG(element_at(values, 786431 )[1]),
	851967 : AVG(element_at(values, 851967 )[1]),
	917503 : AVG(element_at(values, 917503 )[1]),
	917508 : AVG(element_at(values, 917508 )[1]),
	983044 : AVG(element_at(values, 983044 )[1]),
	1048580: AVG(element_at(values, 1048580)[1])
	} as avg,
MAP{
	131071:  STDDEV(element_at(values, 131071)[1]),
	196607 : STDDEV(element_at(values, 196607 )[1]),
	262143 : STDDEV(element_at(values, 262143 )[1]),
	327679 : STDDEV(element_at(values, 327679 )[1]),
	393215 : STDDEV(element_at(values, 393215 )[1]),
	458751 : STDDEV(element_at(values, 458751 )[1]),
	524287 : STDDEV(element_at(values, 524287 )[1]),
	589823 : STDDEV(element_at(values, 589823 )[1]),
	655359 : STDDEV(element_at(values, 655359 )[1]),
	720895 : STDDEV(element_at(values, 720895 )[1]),
	786431 : STDDEV(element_at(values, 786431 )[1]),
	851967 : STDDEV(element_at(values, 851967 )[1]),
	917503 : STDDEV(element_at(values, 917503 )[1]),
	917508 : STDDEV(element_at(values, 917508 )[1]),
	983044 : STDDEV(element_at(values, 983044 )[1]),
	1048580: STDDEV(element_at(values, 1048580)[1])
	} as stddev
FROM objects 
WHERE validity = 0
GROUP BY (channel_id, image_id, tile_id);
