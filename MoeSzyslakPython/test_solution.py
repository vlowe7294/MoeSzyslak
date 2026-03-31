import requests
import re

def render_unicode_grid(doc_url: str):
    """
    Given the URL of a published Google Doc containing entries of the form:
        CHARACTER: X
        X: <number>
        Y: <number>
    …fetches the document, parses the entries, builds the 2D grid, and prints it.
    """

    # Convert published-doc URL to raw text export URL
    # Works for URLs ending in /pub or /edit?usp=sharing
    match = re.search(r"/document/d/([^/]+)", doc_url)
    if not match:
        raise ValueError("Invalid Google Doc URL")

    doc_id = match.group(1)
    txt_url = f"https://docs.google.com/document/d/{doc_id}/export?format=txt"

    # Fetch the raw text
    response = requests.get(txt_url)
    response.raise_for_status()
    text = response.text

    # Parse blocks of the form:
    # CHARACTER: X
    # X: 12
    # Y: 4
    pattern = re.compile(
        r"CHARACTER:\s*(.+?)\s*X:\s*(\d+)\s*Y:\s*(\d+)",
        re.DOTALL
    )

    entries = []
    max_x = 0
    max_y = 0

    for char, x, y in pattern.findall(text):
        x = int(x)
        y = int(y)
        entries.append((char, x, y))
        max_x = max(max_x, x)
        max_y = max(max_y, y)

    # Build grid (rows = y, columns = x)
    width = max_x + 1
    height = max_y + 1

    grid = [[" " for _ in range(width)] for _ in range(height)]

    # Place characters
    for char, x, y in entries:
        grid[y][x] = char

    # Print the result
    for row in grid:
        print("".join(row))
