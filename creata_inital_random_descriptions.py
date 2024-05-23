import glob
import os
import xml.etree.ElementTree as ET
import random
import hashlib

def random_description():
    descriptions = [
        "Detailed scenery of nature",
        "Urban landscape at night",
        "Portrait of a young individual",
        "Documentary style video of an event",
        "Sports action captured in high detail",
        "Educational content on science",
        "Artistic interpretation in digital media",
        "Historical archive footage",
        "Cinematic trailer for upcoming movie"
    ]
    return random.choice(descriptions)

def generate_xml_metadata(file_path):
    description = random_description()

    root = ET.Element("metadata")
    ET.SubElement(root, "description").text = description

    tree = ET.ElementTree(root)
    xml_file_path = os.path.splitext(file_path)[0] + ".xml"
    tree.write(xml_file_path)

image_files = glob.glob("bin/data/images/*.jpg") + glob.glob("bin/data/images/*.png")
video_files = glob.glob("bin/data/videos/*.mp4")

for file_path in image_files + video_files:
    generate_xml_metadata(file_path)


def examples_xml(file_path):
    file_name = os.path.basename(file_path)

    root = ET.Element("metadata")
    ET.SubElement(root, "description").text = os.path.splitext(file_name)[0].title()
    
    tree = ET.ElementTree(root)
    xml_file_path = os.path.splitext(file_path)[0] + ".xml"
    tree.write(xml_file_path)
    
for files in glob.glob("bin/data/examples/*.jpg") + glob.glob("bin/data/examples/*.png"):
    examples_xml(files)