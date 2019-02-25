import os
import pathlib
import shutil
from datetime import date

from icrawler.builtin import (BaiduImageCrawler, BingImageCrawler,
                              FlickrImageCrawler, GoogleImageCrawler)

flickr = FlickrImageCrawler(
    '061beed3b1f1e2f1c95874ea769f7536',
    downloader_threads=4,
    storage={'root_dir': 'bike'})
flickr.crawl(
    group_id="60503902@N00",
    tags='bike',
    max_num=100,
    min_upload_date=date(2017, 1, 1))

flickr = FlickrImageCrawler(
    '061beed3b1f1e2f1c95874ea769f7536',
    downloader_threads=4,
    storage={'root_dir': 'car'})
flickr.crawl(
    group_id="2309748@N20",
    tags='car',
    max_num=100,
    min_upload_date=date(2017, 1, 1))

flickr = FlickrImageCrawler(
    '061beed3b1f1e2f1c95874ea769f7536',
    downloader_threads=4,
    storage={'root_dir': 'truck'})
flickr.crawl(
    group_id="51206186@N00",
    tags='truck',
    max_num=100,
    min_upload_date=date(2017, 1, 1))

flickr = FlickrImageCrawler(
    '061beed3b1f1e2f1c95874ea769f7536',
    downloader_threads=4,
    storage={'root_dir': 'bus'})
flickr.crawl(
    group_id="56649345@N00",
    tags='bus',
    max_num=100,
    min_upload_date=date(2017, 1, 1))

dir_path = os.path.dirname(os.path.realpath(__file__))

for cat in ["car", "bike", "truck", "bus"]:
    abs_path = os.path.join(dir_path, cat)
    for i in os.listdir(abs_path):
        file_path = os.path.join(abs_path, i)
        if "png" in file_path:
            p = pathlib.Path(file_path)
            p.rename(p.with_suffix(".jpg"))

dir_path = os.path.dirname(os.path.realpath(__file__))

# create data folder
root = os.path.join(dir_path, "data")
if not os.path.exists(root):
    os.makedirs(root)
    train = os.path.join(root, "train")
    test = os.path.join(root, "test")
    os.makedirs(train)
    os.makedirs(test)
    for cat in ["car", "bike", "truck", "bus"]:
        train_cat = os.path.join(train, cat)
        test_cat = os.path.join(test, cat)
        os.makedirs(train_cat)
        os.makedirs(test_cat)

for cat in ["car", "bike", "truck", "bus"]:
    abs_path = os.path.join(dir_path, cat)
    i = 0
    for files in os.listdir(abs_path):
        i += 1
        file_path = os.path.join(abs_path, files)
        if i > 80:
            # move to testing
            target = os.path.join(root, "test", cat)
            shutil.move(file_path, target)
        else:
            # move to training
            target = os.path.join(root, "train", cat)
            shutil.move(file_path, target)
    shutil.rmtree(cat)
