import os
import time
from git import Repo

os.chdir("..")

test_images = range(1, 15)
TEST_IMAGE_FOLDER = "test_images"
TEST_IMAGE_SUB_FOLDER = "benchmark"
THRESHOLD = 100
PHASES = 5


def generate_reference(name):
	LOG_FOLDER = "test_framework"
	LOG_SUB_FOLDER = "standards"
	OUTPUT_FOLDER = "test_framework"
	OUTPUT_SUB_FOLDER = "reference_images"
	log_path = os.path.join(LOG_FOLDER, LOG_SUB_FOLDER, "{}.txt".format(name))
	output_path = os.path.join(OUTPUT_FOLDER, OUTPUT_SUB_FOLDER, "{}.bmp".format(name))
	command = "-t {} -p {} -o {} -f {} {}".format(THRESHOLD, PHASES, output_path, log_path)
	return command


def run_test(name):
	LOG_FOLDER = "test_framework"
	LOG_SUB_FOLDER = "transformation_log"
	OUTPUT_FOLDER = "test_framework"
	OUTPUT_SUB_FOLDER = "image_log"
	log_path = os.path.join(LOG_FOLDER, LOG_SUB_FOLDER, "{}.txt".format(name))
	output_path = os.path.join(OUTPUT_FOLDER, OUTPUT_SUB_FOLDER, "{}.bmp".format(name))
	command = "-t {} -p {} -o {} -f {}".format(THRESHOLD, PHASES, output_path, log_path)
	return command
