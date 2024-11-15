#!/usr/bin/env python3

import os
import requests
import random

# List of random image URLs
IMAGE_URLS = [
    "https://picsum.photos/200/300",
]

def main():
    # Choose a random image URL
    image_url = random.choice(IMAGE_URLS)
    
    # Fetch the image
    response = requests.get(image_url)
    
    # Check if the image was successfully fetched
    if response.status_code == 200:
        image_data = response.content
        content_length = len(image_data)
        
        # Set headers
        print("Content-Type: text/html\r")
        print(f"Content-Length: 7\r")
        print("\r")
        print("hello")
        # Output image data
        # sys.stdout.buffer.write(image_data)
    else:
        # Output an error message if image could not be fetched
        print("Content-Type: text/html")
        print()
        print("<html><body><h1>Failed to load image</h1></body></html>")

if __name__ == "__main__":
    main()
