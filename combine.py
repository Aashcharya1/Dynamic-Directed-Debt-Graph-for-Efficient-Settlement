from PIL import Image, ImageDraw, ImageFont
import os
import platform

# Load images
initial_raw = Image.open("initial_raw.png")
initial = Image.open("initial_graph.png")
raw = Image.open("final_graph_raw.png")
simplified = Image.open("final_graph_simplified.png")

# Get original max dimensions
max_height = max(initial_raw.height, initial.height, raw.height, simplified.height)
max_width = max(initial_raw.width, initial.width, raw.width, simplified.width)

# Scale factor for full-size output
scale_factor = 2.0
scaled_width = int(max_width * scale_factor)
scaled_height = int(max_height * scale_factor)

# Resize images
initial_raw = initial_raw.resize((scaled_width, scaled_height))
initial = initial.resize((scaled_width, scaled_height))
raw = raw.resize((scaled_width, scaled_height))
simplified = simplified.resize((scaled_width, scaled_height))

# Dynamically calculate title height and font size
title_height = int(scaled_height * 0.12)  # make room for big titles
font_size = int(title_height * 0.7)       # font size fits title box

# Font setup — clean and big
try:
    font = ImageFont.truetype("DejaVuSans-Bold.ttf", font_size)
except:
    font = ImageFont.load_default()

# Padding around each image
padding = 40

# Calculate cell and canvas size
cell_width = scaled_width + padding * 2
cell_height = scaled_height + title_height + padding * 2

canvas_width = cell_width * 2  # 2 columns
canvas_height = cell_height * 2  # 2 rows

# Create the canvas
combined = Image.new("RGB", (canvas_width, canvas_height), "white")
draw = ImageDraw.Draw(combined)

# Images and titles
images = [initial_raw, initial, raw, simplified]
titles = ["Initial Raw Graph", "Initial Simplified Graph", "Final Raw Graph", "Final Simplified Graph"]

# Place images with thick borders and bold, large titles
for index in range(4):
    row = index // 2
    col = index % 2

    x_offset = col * cell_width + padding
    y_offset = row * cell_height + padding + title_height

    # Paste image
    combined.paste(images[index], (x_offset, y_offset))

    # Draw border
    border_rect = [
        x_offset - 4, y_offset - 4,
        x_offset + scaled_width + 4, y_offset + scaled_height + 4
    ]
    draw.rectangle(border_rect, outline="black", width=6)

    # Draw centered, big title
    bbox = draw.textbbox((0, 0), titles[index], font=font)
    text_width = bbox[2] - bbox[0]
    text_x = x_offset + (scaled_width - text_width) // 2
    text_y = y_offset - title_height + (title_height - (bbox[3] - bbox[1])) // 2

    draw.text((text_x, text_y), titles[index], fill="black", font=font)

# Save and open final output
output_file = "comparison_graphs.png"
combined.save(output_file)
print(f"✅ Saved: {output_file}")

# Open based on OS
if platform.system() == "Windows":
    os.system(f'start {output_file}')
elif platform.system() == "Darwin":  # macOS
    os.system(f'open {output_file}')
else:  # Linux and others
    os.system(f'xdg-open {output_file}')
