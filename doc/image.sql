CREATE TABLE IF NOT EXISTS `experiment` (
	`id` integer primary key NOT NULL UNIQUE,
	`name` TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS `image` (
	`id` INTEGER NOT NULL UNIQUE,
	`experiment_id` INTEGER NOT NULL,
	`name` TEXT NOT NULL,
FOREIGN KEY(`experiment_id`) REFERENCES `experiment`(`id`)
);
CREATE TABLE IF NOT EXISTS `channel` (
	`id` INTEGER NOT NULL UNIQUE,
	`image_id` INTEGER NOT NULL UNIQUE,
	`name` TEXT NOT NULL,
FOREIGN KEY(`image_id`) REFERENCES `image`(`id`)
);
CREATE TABLE IF NOT EXISTS `object` (
	`object_id` integer primary key NOT NULL UNIQUE,
	`measure_ch_id` integer primary key NOT NULL UNIQUE,
	`channel_id` INTEGER NOT NULL UNIQUE,
	`image_id` INTEGER NOT NULL UNIQUE,
	`value` REAL NOT NULL,
FOREIGN KEY(`channel_id`) REFERENCES `channel`(`id`),
FOREIGN KEY(`image_id`) REFERENCES `channel`(`image_id`)
);

FOREIGN KEY(`experiment_id`) REFERENCES `experiment`(`id`)
FOREIGN KEY(`image_id`) REFERENCES `image`(`id`)
FOREIGN KEY(`channel_id`) REFERENCES `channel`(`id`)
FOREIGN KEY(`image_id`) REFERENCES `channel`(`image_id`)