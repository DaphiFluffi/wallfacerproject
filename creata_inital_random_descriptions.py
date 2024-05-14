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
    file_type = "video" if file_path.endswith(".mp4") else "image"
    file_name = os.path.basename(file_path)
    meta_name = hashlib.md5(file_name.encode()).hexdigest()
    description = random_description()

    root = ET.Element("metadata")
    ET.SubElement(root, "type").text = file_type
    ET.SubElement(root, "filename").text = file_name
    ET.SubElement(root, "meta_name").text = meta_name
    ET.SubElement(root, "description").text = description

    tree = ET.ElementTree(root)
    xml_file_path = os.path.splitext(file_path)[0] + ".xml"
    tree.write(xml_file_path)

image_files = glob.glob("bin/data/images/*.jpg") + glob.glob("bin/data/images/*.png")
video_files = glob.glob("bin/data/videos/*.mp4")

for file_path in image_files + video_files:
    generate_xml_metadata(file_path)
    print(f"Metadata XML for {file_path} generated.")
